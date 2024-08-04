#include <stdlib.h>
#include <stdbool.h>

#include "wt_types.h"
#include "wt_internals.h"
#include "wt_mempool.h"


/*
 * MaxAllocSize, MaxAllocHugeSize
 *		Quasi-arbitrary limits on size of allocations.
 *
 * Note:
 *		There is no guarantee that smaller allocations will succeed, but
 *		larger requests will be summarily denied.
 *
 * palloc() enforces MaxAllocSize, chosen to correspond to the limiting size
 * of varlena objects under TOAST.  See VARSIZE_4B() and related macros in
 * postgres.h.  Many datatypes assume that any allocatable size can be
 * represented in a varlena header.  This limit also permits a caller to use
 * an "int" variable for an index into or length of an allocation.  Callers
 * careful to avoid these hazards can access the higher limit with
 * MemoryContextAllocHuge().  Both limits permit code to assume that it may
 * compute twice an allocation's size without overflow.
 */
#define MaxAllocSize	((Size) 0x3fffffff) /* 1 gigabyte - 1 */

#define AllocSizeIsValid(size)	((Size) (size) <= MaxAllocSize)

#define WT_SIZE_MAX               (~(size_t)0)

/* Must be less than WT_SIZE_MAX */
#define MaxAllocHugeSize	(WT_SIZE_MAX / 2)

#define InvalidAllocSize	WT_SIZE_MAX

#define AllocHugeSizeIsValid(size)	((Size) (size) <= MaxAllocHugeSize)

/*
 * The first field of every node is NodeTag. Each node created (with makeNode)
 * will have one of the following tags as the value of its first field.
 *
 * Note that inserting or deleting node types changes the numbers of other
 * node types later in the list.  This is no problem during development, since
 * the node numbers are never stored on disk.  But don't do it in a released
 * branch, because that would represent an ABI break for extensions.
 */
typedef enum NodeTag
{
	T_Invalid = 0,
	T_AllocSetContext = 1,
	T_GenerationContext = 2,
	T_SlabContext = 3,
} NodeTag;

 /*
  * The first field of a node of any type is guaranteed to be the NodeTag.
  * Hence the type of any node can be gotten by casting it to Node. Declaring
  * a variable to be of Node * (instead of void *) can also facilitate
  * debugging.
  */
typedef struct Node
{
	NodeTag		type;
} Node;

#define nodeTag(nodeptr)		(((const Node*)(nodeptr))->type)
#define IsA(nodeptr,_type_)		(nodeTag(nodeptr) == T_##_type_)

/*
 * MemoryContextCounters
 *		Summarization state for MemoryContextStats collection.
 *
 * The set of counters in this struct is biased towards AllocSet; if we ever
 * add any context types that are based on fundamentally different approaches,
 * we might need more or different counters here.  A possible API spec then
 * would be to print only nonzero counters, but for now we just summarize in
 * the format historically used by AllocSet.
 */
typedef struct MemoryContextCounters
{
	Size		nblocks;		/* Total number of malloc blocks */
	Size		freechunks;		/* Total number of free chunks */
	Size		totalspace;		/* Total bytes requested from malloc */
	Size		freespace;		/* The unused portion of totalspace */
} MemoryContextCounters;

typedef struct MemoryChunk
{
#ifdef MEMORY_CONTEXT_CHECKING
	Size		requested_size;
#endif

	/* bitfield for storing details about the chunk */
	uint64		hdrmask;		/* must be last */
} MemoryChunk;

/* Get the MemoryChunk from the pointer */
#define PointerGetMemoryChunk(p) \
	((MemoryChunk *) ((char *) (p) - sizeof(MemoryChunk)))

/* Get the pointer from the MemoryChunk */
#define MemoryChunkGetPointer(c) \
	((void *) ((char *) (c) + sizeof(MemoryChunk)))

/*
 * MemoryContext
 *		A logical context in which memory allocations occur.
 *
 * MemoryContext itself is an abstract type that can have multiple
 * implementations.
 * The function pointers in MemoryContextMethods define one specific
 * implementation of MemoryContext --- they are a virtual function table
 * in C++ terms.
 *
 * Node types that are actual implementations of memory contexts must
 * begin with the same fields as MemoryContextData.
 *
 * Note: for largely historical reasons, typedef MemoryContext is a pointer
 * to the context struct rather than the struct type itself.
 */

typedef void (*MemoryStatsPrintFunc) (MemoryContext context, void* passthru, const char* stats_string, bool print_to_stderr);

typedef struct MemoryContextMethods
{
	void*	(*alloc) (MemoryContext context, Size size);
	/* call this free_p in case someone #define's free() */
	void	(*free_p) (void* pointer);
	void*	(*realloc) (void* pointer, Size size);
	void	(*reset) (MemoryContext context);
	void	(*delete_context) (MemoryContext context);
	MemoryContext	(*get_chunk_context) (void* pointer);
	Size	(*get_chunk_space) (void* pointer);
	bool	(*is_empty) (MemoryContext context);
	void	(*stats) (MemoryContext context, MemoryStatsPrintFunc printfunc, void* passthru, MemoryContextCounters* totals,	bool print_to_stderr);
#ifdef MEMORY_CONTEXT_CHECKING
	void		(*check) (MemoryContext context);
#endif
} MemoryContextMethods;


/*
 * A memory context can have callback functions registered on it.  Any such
 * function will be called once just before the context is next reset or
 * deleted.  The MemoryContextCallback struct describing such a callback
 * typically would be allocated within the context itself, thereby avoiding
 * any need to manage it explicitly (the reset/delete action will free it).
 */
typedef void (*MemoryContextCallbackFunction) (void* arg);

typedef struct MemoryContextCallback
{
	MemoryContextCallbackFunction func; /* function to call */
	void* arg;			/* argument to pass it */
	struct MemoryContextCallback* next; /* next in list of callbacks */
} MemoryContextCallback;

typedef struct MemoryContextData
{
	NodeTag		type;			/* identifies exact kind of context */
	/* these two fields are placed here to minimize alignment wastage: */
	bool		isReset;		/* T = no space alloced since last reset */
	bool		allowInCritSection; /* allow palloc in critical section */
	Size		mem_allocated;	/* track memory allocated for this context */
	const MemoryContextMethods* methods;	/* virtual function table */
	MemoryContext parent;		/* NULL if no parent (toplevel context) */
	MemoryContext firstchild;	/* head of linked list of children */
	MemoryContext prevchild;	/* previous child of same parent */
	MemoryContext nextchild;	/* next child of same parent */
	const char* name;			/* context name (just for debugging) */
	const char* ident;			/* context ID if any (just for debugging) */
	MemoryContextCallback* reset_cbs;	/* list of reset/delete callbacks */
} MemoryContextData;

/*
 * MemoryContextMethodID
 *		A unique identifier for each MemoryContext implementation which
 *		indicates the index into the mcxt_methods[] array. See mcxt.c.
 *
 * For robust error detection, ensure that MemoryContextMethodID has a value
 * for each possible bit-pattern of MEMORY_CONTEXT_METHODID_MASK, and make
 * dummy entries for unused IDs in the mcxt_methods[] array.  We also try
 * to avoid using bit-patterns as valid IDs if they are likely to occur in
 * garbage data, or if they could falsely match on chunks that are really from
 * malloc not palloc.  (We can't tell that for most malloc implementations,
 * but it happens that glibc stores flag bits in the same place where we put
 * the MemoryContextMethodID, so the possible values are predictable for it.)
 */
typedef enum MemoryContextMethodID
{
	MCTX_UNUSED1_ID,			/* 000 occurs in never-used memory */
	MCTX_UNUSED2_ID,			/* glibc malloc'd chunks usually match 001 */
	MCTX_UNUSED3_ID,			/* glibc malloc'd chunks > 128kB match 010 */
	MCTX_ASET_ID,
	MCTX_GENERATION_ID,
	MCTX_SLAB_ID,
	MCTX_ALIGNED_REDIRECT_ID,
	MCTX_UNUSED4_ID				/* 111 occurs in wipe_mem'd memory */
} MemoryContextMethodID;

/*
 * The number of bits that 8-byte memory chunk headers can use to encode the
 * MemoryContextMethodID.
 */
#define MEMORY_CONTEXT_METHODID_BITS 3
#define MEMORY_CONTEXT_METHODID_MASK \
	((((uint64) 1) << MEMORY_CONTEXT_METHODID_BITS) - 1)

/* These functions implement the MemoryContext API for AllocSet context. */
static void* AllocSetAlloc(MemoryContext context, Size size);
static void  AllocSetFree(void* pointer);
static void* AllocSetRealloc(void* pointer, Size size);
static void  AllocSetReset(MemoryContext context);
static void  AllocSetDelete(MemoryContext context);
static MemoryContext AllocSetGetChunkContext(void* pointer);
static Size AllocSetGetChunkSpace(void* pointer);
static bool AllocSetIsEmpty(MemoryContext context);
static void AllocSetStats(MemoryContext context,
							MemoryStatsPrintFunc printfunc, void* passthru,
							MemoryContextCounters* totals,
							bool print_to_stderr);

#ifdef MEMORY_CONTEXT_CHECKING
static void AllocSetCheck(MemoryContext context);
#endif

/* These functions implement the MemoryContext API for Generation context. */
static void* GenerationAlloc(MemoryContext context, Size size);
static void GenerationFree(void* pointer);
static void* GenerationRealloc(void* pointer, Size size);
static void GenerationReset(MemoryContext context);
static void GenerationDelete(MemoryContext context);
static MemoryContext GenerationGetChunkContext(void* pointer);
static Size GenerationGetChunkSpace(void* pointer);
static bool GenerationIsEmpty(MemoryContext context);
static void GenerationStats(MemoryContext context,
							MemoryStatsPrintFunc printfunc, void* passthru,
							MemoryContextCounters* totals,
							bool print_to_stderr);

#ifdef MEMORY_CONTEXT_CHECKING
static void GenerationCheck(MemoryContext context);
#endif

/* These functions implement the MemoryContext API for Slab context. */
static void* SlabAlloc(MemoryContext context, Size size);
static void SlabFree(void* pointer);
static void* SlabRealloc(void* pointer, Size size);
static void SlabReset(MemoryContext context);
static void SlabDelete(MemoryContext context);
static MemoryContext SlabGetChunkContext(void* pointer);
static Size SlabGetChunkSpace(void* pointer);
static bool SlabIsEmpty(MemoryContext context);
static void SlabStats(MemoryContext context,
						MemoryStatsPrintFunc printfunc, void* passthru,
						MemoryContextCounters* totals,
						bool print_to_stderr);

#ifdef MEMORY_CONTEXT_CHECKING
static void SlabCheck(MemoryContext context);
#endif

/*
 * These functions support the implementation of palloc_aligned() and are not
 * part of a fully-fledged MemoryContext type.
 */
static void AlignedAllocFree(void* pointer);
static void* AlignedAllocRealloc(void* pointer, Size size);
static MemoryContext AlignedAllocGetChunkContext(void* pointer);
static Size AlignedAllocGetChunkSpace(void* pointer);

static void BogusFree(void* pointer) {}
static void* BogusRealloc(void* pointer, Size size) { return NULL; }
static MemoryContext BogusGetChunkContext(void* pointer) { return NULL; }
static Size BogusGetChunkSpace(void* pointer) { return 0; }

/*****************************************************************************
 *	  GLOBAL MEMORY															 *
 *****************************************************************************/
static const MemoryContextMethods mcxt_methods[] = 
{
	/* aset.c */
	[MCTX_ASET_ID] .alloc = AllocSetAlloc,
	[MCTX_ASET_ID].free_p = AllocSetFree,
	[MCTX_ASET_ID].realloc = AllocSetRealloc,
	[MCTX_ASET_ID].reset = AllocSetReset,
	[MCTX_ASET_ID].delete_context = AllocSetDelete,
	[MCTX_ASET_ID].get_chunk_context = AllocSetGetChunkContext,
	[MCTX_ASET_ID].get_chunk_space = AllocSetGetChunkSpace,
	[MCTX_ASET_ID].is_empty = AllocSetIsEmpty,
	[MCTX_ASET_ID].stats = AllocSetStats,
#ifdef MEMORY_CONTEXT_CHECKING
	[MCTX_ASET_ID].check = AllocSetCheck,
#endif

	/* generation.c */
	[MCTX_GENERATION_ID].alloc = GenerationAlloc,
	[MCTX_GENERATION_ID].free_p = GenerationFree,
	[MCTX_GENERATION_ID].realloc = GenerationRealloc,
	[MCTX_GENERATION_ID].reset = GenerationReset,
	[MCTX_GENERATION_ID].delete_context = GenerationDelete,
	[MCTX_GENERATION_ID].get_chunk_context = GenerationGetChunkContext,
	[MCTX_GENERATION_ID].get_chunk_space = GenerationGetChunkSpace,
	[MCTX_GENERATION_ID].is_empty = GenerationIsEmpty,
	[MCTX_GENERATION_ID].stats = GenerationStats,
#ifdef MEMORY_CONTEXT_CHECKING
	[MCTX_GENERATION_ID].check = GenerationCheck,
#endif

	/* slab.c */
	[MCTX_SLAB_ID].alloc = SlabAlloc,
	[MCTX_SLAB_ID].free_p = SlabFree,
	[MCTX_SLAB_ID].realloc = SlabRealloc,
	[MCTX_SLAB_ID].reset = SlabReset,
	[MCTX_SLAB_ID].delete_context = SlabDelete,
	[MCTX_SLAB_ID].get_chunk_context = SlabGetChunkContext,
	[MCTX_SLAB_ID].get_chunk_space = SlabGetChunkSpace,
	[MCTX_SLAB_ID].is_empty = SlabIsEmpty,
	[MCTX_SLAB_ID].stats = SlabStats,
#ifdef MEMORY_CONTEXT_CHECKING
	[MCTX_SLAB_ID].check = SlabCheck,
#endif

	/* alignedalloc.c */
	[MCTX_ALIGNED_REDIRECT_ID].alloc = NULL,	/* not required */
	[MCTX_ALIGNED_REDIRECT_ID].free_p = AlignedAllocFree,
	[MCTX_ALIGNED_REDIRECT_ID].realloc = AlignedAllocRealloc,
	[MCTX_ALIGNED_REDIRECT_ID].reset = NULL,	/* not required */
	[MCTX_ALIGNED_REDIRECT_ID].delete_context = NULL,	/* not required */
	[MCTX_ALIGNED_REDIRECT_ID].get_chunk_context = AlignedAllocGetChunkContext,
	[MCTX_ALIGNED_REDIRECT_ID].get_chunk_space = AlignedAllocGetChunkSpace,
	[MCTX_ALIGNED_REDIRECT_ID].is_empty = NULL, /* not required */
	[MCTX_ALIGNED_REDIRECT_ID].stats = NULL,	/* not required */
#ifdef MEMORY_CONTEXT_CHECKING
	[MCTX_ALIGNED_REDIRECT_ID].check = NULL,	/* not required */
#endif

	/*
	 * Unused (as yet) IDs should have dummy entries here.  This allows us to
	 * fail cleanly if a bogus pointer is passed to pfree or the like.  It
	 * seems sufficient to provide routines for the methods that might get
	 * invoked from inspection of a chunk (see MCXT_METHOD calls below).
	 */

	[MCTX_UNUSED1_ID].free_p = BogusFree,
	[MCTX_UNUSED1_ID].realloc = BogusRealloc,
	[MCTX_UNUSED1_ID].get_chunk_context = BogusGetChunkContext,
	[MCTX_UNUSED1_ID].get_chunk_space = BogusGetChunkSpace,

	[MCTX_UNUSED2_ID].free_p = BogusFree,
	[MCTX_UNUSED2_ID].realloc = BogusRealloc,
	[MCTX_UNUSED2_ID].get_chunk_context = BogusGetChunkContext,
	[MCTX_UNUSED2_ID].get_chunk_space = BogusGetChunkSpace,

	[MCTX_UNUSED3_ID].free_p = BogusFree,
	[MCTX_UNUSED3_ID].realloc = BogusRealloc,
	[MCTX_UNUSED3_ID].get_chunk_context = BogusGetChunkContext,
	[MCTX_UNUSED3_ID].get_chunk_space = BogusGetChunkSpace,

	[MCTX_UNUSED4_ID].free_p = BogusFree,
	[MCTX_UNUSED4_ID].realloc = BogusRealloc,
	[MCTX_UNUSED4_ID].get_chunk_context = BogusGetChunkContext,
	[MCTX_UNUSED4_ID].get_chunk_space = BogusGetChunkSpace,
};

/*
 * MemoryContextIsValid
 *		True iff memory context is valid.
 *
 * Add new context types to the set accepted by this macro.
 */
#define MemoryContextIsValid(context) \
	((context) != NULL && \
	 (IsA((context), AllocSetContext) || \
	  IsA((context), SlabContext) || \
	  IsA((context), GenerationContext)))

/*
 * MemoryContextCreate
 *		Context-type-independent part of context creation.
 *
 * This is only intended to be called by context-type-specific
 * context creation routines, not by the unwashed masses.
 *
 * The memory context creation procedure goes like this:
 *	1.  Context-type-specific routine makes some initial space allocation,
 *		including enough space for the context header.  If it fails,
 *		it can ereport() with no damage done.
 *	2.	Context-type-specific routine sets up all type-specific fields of
 *		the header (those beyond MemoryContextData proper), as well as any
 *		other management fields it needs to have a fully valid context.
 *		Usually, failure in this step is impossible, but if it's possible
 *		the initial space allocation should be freed before ereport'ing.
 *	3.	Context-type-specific routine calls MemoryContextCreate() to fill in
 *		the generic header fields and link the context into the context tree.
 *	4.  We return to the context-type-specific routine, which finishes
 *		up type-specific initialization.  This routine can now do things
 *		that might fail (like allocate more memory), so long as it's
 *		sure the node is left in a state that delete will handle.
 *
 * node: the as-yet-uninitialized common part of the context header node.
 * tag: NodeTag code identifying the memory context type.
 * method_id: MemoryContextMethodID of the context-type being created.
 * parent: parent context, or NULL if this will be a top-level context.
 * name: name of context (must be statically allocated).
 *
 * Context routines generally assume that MemoryContextCreate can't fail,
 * so this can contain Assert but not elog/ereport.
 */
static void MemoryContextCreate(MemoryContext node, NodeTag tag, MemoryContextMethodID method_id, MemoryContext parent, const char* name)
{
	/* Creating new memory contexts is not allowed in a critical section */
	Assert(CritSectionCount == 0);

	/* Initialize all standard fields of memory context header */
	node->type = tag;
	node->isReset = true;
	node->methods = &mcxt_methods[method_id];
	node->parent = parent;
	node->firstchild = NULL;
	node->mem_allocated = 0;
	node->prevchild = NULL;
	node->name = name;
	node->ident = NULL;
	node->reset_cbs = NULL;

	/* OK to link node into context tree */
	if (parent)
	{
		node->nextchild = parent->firstchild;
		if (parent->firstchild != NULL)
			parent->firstchild->prevchild = node;
		parent->firstchild = node;
		/* inherit allowInCritSection flag from parent */
		node->allowInCritSection = parent->allowInCritSection;
	}
	else
	{
		node->nextchild = NULL;
		node->allowInCritSection = false;
	}

	VALGRIND_CREATE_MEMPOOL(node, 0, false);
}

/*
 * MemoryContextCallResetCallbacks
 *		Internal function to call all registered callbacks for context.
 */
static void MemoryContextCallResetCallbacks(MemoryContext context)
{
	MemoryContextCallback* cb;

	/*
	 * We pop each callback from the list before calling.  That way, if an
	 * error occurs inside the callback, we won't try to call it a second time
	 * in the likely event that we reset or delete the context later.
	 */
	while ((cb = context->reset_cbs) != NULL)
	{
		context->reset_cbs = cb->next;
		cb->func(cb->arg);
	}
}

/*
 * MemoryContextResetOnly
 *		Release all space allocated within a context.
 *		Nothing is done to the context's descendant contexts.
 */
static void MemoryContextResetOnly(MemoryContext context)
{
	Assert(MemoryContextIsValid(context));

	/* Nothing to do if no pallocs since startup or last reset */
	if (!context->isReset)
	{
		MemoryContextCallResetCallbacks(context);

		/*
		 * If context->ident points into the context's memory, it will become
		 * a dangling pointer.  We could prevent that by setting it to NULL
		 * here, but that would break valid coding patterns that keep the
		 * ident elsewhere, e.g. in a parent context.  So for now we assume
		 * the programmer got it right.
		 */

		context->methods->reset(context);
		context->isReset = true;
		VALGRIND_DESTROY_MEMPOOL(context);
		VALGRIND_CREATE_MEMPOOL(context, 0, false);
	}
}


/*
 * The maximum allowed value that MemoryContexts can store in the value
 * field.  Must be 1 less than a power of 2.
 */
#define MEMORYCHUNK_MAX_VALUE			UINT64CONST(0x3FFFFFFF)

 /*
  * The maximum distance in bytes that a MemoryChunk can be offset from the
  * block that is storing the chunk.  Must be 1 less than a power of 2.
  */
#define MEMORYCHUNK_MAX_BLOCKOFFSET		UINT64CONST(0x3FFFFFFF)

/* define the least significant base-0 bit of each portion of the hdrmask */
#define MEMORYCHUNK_EXTERNAL_BASEBIT	MEMORY_CONTEXT_METHODID_BITS
#define MEMORYCHUNK_VALUE_BASEBIT		(MEMORYCHUNK_EXTERNAL_BASEBIT + 1)
#define MEMORYCHUNK_BLOCKOFFSET_BASEBIT	(MEMORYCHUNK_VALUE_BASEBIT + 30)

/*
 * A magic number for storing in the free bits of an external chunk.  This
 * must mask out the bits used for storing the MemoryContextMethodID and the
 * external bit.
 */
#define MEMORYCHUNK_MAGIC		(UINT64CONST(0xB1A8DB858EB6EFBA) >> \
								 MEMORYCHUNK_VALUE_BASEBIT << \
								 MEMORYCHUNK_VALUE_BASEBIT)

/*
 * MemoryChunkSetHdrMaskExternal
 *		Set 'chunk' as an externally managed chunk.  Here we only record the
 *		MemoryContextMethodID and set the external chunk bit.
 */
static inline void MemoryChunkSetHdrMaskExternal(MemoryChunk* chunk, MemoryContextMethodID methodid)
{
	Assert((int)methodid <= MEMORY_CONTEXT_METHODID_MASK);

	chunk->hdrmask = MEMORYCHUNK_MAGIC | (((uint64)1) << MEMORYCHUNK_EXTERNAL_BASEBIT) | methodid;
}

/*
 * MemoryChunkSetHdrMask
 *		Store the given 'block', 'chunk_size' and 'methodid' in the given
 *		MemoryChunk.
 *
 * The number of bytes between 'block' and 'chunk' must be <=
 * MEMORYCHUNK_MAX_BLOCKOFFSET.
 * 'value' must be <= MEMORYCHUNK_MAX_VALUE.
 */
static inline void MemoryChunkSetHdrMask(MemoryChunk* chunk, void* block, Size value, MemoryContextMethodID methodid) 
{
	Size		blockoffset = (char*)chunk - (char*)block;

	Assert((char*)chunk >= (char*)block);
	Assert(blockoffset <= MEMORYCHUNK_MAX_BLOCKOFFSET);
	Assert(value <= MEMORYCHUNK_MAX_VALUE);
	Assert((int)methodid <= MEMORY_CONTEXT_METHODID_MASK);

	chunk->hdrmask = (((uint64)blockoffset) << MEMORYCHUNK_BLOCKOFFSET_BASEBIT) | (((uint64)value) << MEMORYCHUNK_VALUE_BASEBIT) | methodid;
}


/* private macros for making the inline functions below more simple */
#define HdrMaskIsExternal(hdrmask) \
	((hdrmask) & (((uint64) 1) << MEMORYCHUNK_EXTERNAL_BASEBIT))

/*
 * MemoryChunkIsExternal
 *		Return true if 'chunk' is marked as external.
 */
static inline bool MemoryChunkIsExternal(MemoryChunk* chunk)
{
	/*
	 * External chunks should always store MEMORYCHUNK_MAGIC in the upper
	 * portion of the hdrmask, check that nothing has stomped on that.
	 */
	Assert(!HdrMaskIsExternal(chunk->hdrmask) || HdrMaskCheckMagic(chunk->hdrmask));

	return HdrMaskIsExternal(chunk->hdrmask);
}

/*
 * We should have used up all the bits here, so the compiler is likely to
 * optimize out the & MEMORYCHUNK_MAX_BLOCKOFFSET.
 */
#define HdrMaskBlockOffset(hdrmask) \
	(((hdrmask) >> MEMORYCHUNK_BLOCKOFFSET_BASEBIT) & MEMORYCHUNK_MAX_BLOCKOFFSET)

/*
 * MemoryChunkGetBlock
 *		For non-external chunks, returns the pointer to the block as was set
 *		in MemoryChunkSetHdrMask.
 */
static inline void* MemoryChunkGetBlock(MemoryChunk* chunk)
{
	Assert(!HdrMaskIsExternal(chunk->hdrmask));

	return (void*)((char*)chunk - HdrMaskBlockOffset(chunk->hdrmask));
}

#define HdrMaskGetValue(hdrmask) \
	(((hdrmask) >> MEMORYCHUNK_VALUE_BASEBIT) & MEMORYCHUNK_MAX_VALUE)

/*
 * MemoryChunkGetValue
 *		For non-external chunks, returns the value field as it was set in
 *		MemoryChunkSetHdrMask.
 */
static inline Size MemoryChunkGetValue(MemoryChunk* chunk)
{
	Assert(!HdrMaskIsExternal(chunk->hdrmask));

	return HdrMaskGetValue(chunk->hdrmask);
}

/*--------------------
 * Chunk freelist k holds chunks of size 1 << (k + ALLOC_MINBITS),
 * for k = 0 .. ALLOCSET_NUM_FREELISTS-1.
 *
 * Note that all chunks in the freelists have power-of-2 sizes.  This
 * improves recyclability: we may waste some space, but the wasted space
 * should stay pretty constant as requests are made and released.
 *
 * A request too large for the last freelist is handled by allocating a
 * dedicated block from malloc().  The block still has a block header and
 * chunk header, but when the chunk is freed we'll return the whole block
 * to malloc(), not put it on our freelists.
 *
 * CAUTION: ALLOC_MINBITS must be large enough so that
 * 1<<ALLOC_MINBITS is at least MAXALIGN,
 * or we may fail to align the smallest chunks adequately.
 * 8-byte alignment is enough on all currently known machines.  This 8-byte
 * minimum also allows us to store a pointer to the next freelist item within
 * the chunk of memory itself.
 *
 * With the current parameters, request sizes up to 8K are treated as chunks,
 * larger requests go into dedicated blocks.  Change ALLOCSET_NUM_FREELISTS
 * to adjust the boundary point; and adjust ALLOCSET_SEPARATE_THRESHOLD in
 * memutils.h to agree.  (Note: in contexts with small maxBlockSize, we may
 * set the allocChunkLimit to less than 8K, so as to avoid space wastage.)
 *--------------------
 */

#define ALLOC_MINBITS		3	/* smallest chunk size is 8 bytes */
#define ALLOCSET_NUM_FREELISTS	11
#define ALLOC_CHUNK_LIMIT	(1 << (ALLOCSET_NUM_FREELISTS-1+ALLOC_MINBITS))
 /* Size of largest chunk that we use a fixed size for */
#define ALLOC_CHUNK_FRACTION	4
/* We allow chunks to be at most 1/4 of maxBlockSize (less overhead) */

#define ALLOC_BLOCKHDRSZ	MAXALIGN(sizeof(AllocBlockData))
#define ALLOC_CHUNKHDRSZ	sizeof(MemoryChunk)

typedef struct AllocBlockData* AllocBlock;	/* forward reference */

/*
 * AllocPointer
 *		Aligned pointer which may be a member of an allocation set.
 */
typedef void* AllocPointer;

/*
 * AllocFreeListLink
 *		When pfreeing memory, if we maintain a freelist for the given chunk's
 *		size then we use a AllocFreeListLink to point to the current item in
 *		the AllocSetContext's freelist and then set the given freelist element
 *		to point to the chunk being freed.
 */
typedef struct AllocFreeListLink
{
	MemoryChunk* next;
} AllocFreeListLink;

/*
 * Obtain a AllocFreeListLink for the given chunk.  Allocation sizes are
 * always at least sizeof(AllocFreeListLink), so we reuse the pointer's memory
 * itself to store the freelist link.
 */
#define GetFreeListLink(chkptr) \
	(AllocFreeListLink *) ((char *) (chkptr) + ALLOC_CHUNKHDRSZ)

 /* Validate a freelist index retrieved from a chunk header */
#define FreeListIdxIsValid(fidx) \
	((fidx) >= 0 && (fidx) < ALLOCSET_NUM_FREELISTS)

/* Determine the size of the chunk based on the freelist index */
#define GetChunkSizeFromFreeListIdx(fidx) \
	((((Size) 1) << ALLOC_MINBITS) << (fidx))

/*
 * We always store external chunks on a dedicated block.  This makes fetching
 * the block from an external chunk easy since it's always the first and only
 * chunk on the block.
 */
#define ExternalChunkGetBlock(chunk) \
	(AllocBlock) ((char *) chunk - ALLOC_BLOCKHDRSZ)

/*
 * AllocSetContext is our standard implementation of MemoryContext.
 *
 * Note: header.isReset means there is nothing for AllocSetReset to do.
 * This is different from the aset being physically empty (empty blocks list)
 * because we will still have a keeper block.  It's also different from the set
 * being logically empty, because we don't attempt to detect pfree'ing the
 * last active chunk.
 */
typedef struct AllocSetContext
{
	MemoryContextData header;	/* Standard memory-context fields */
	/* Info about storage allocated in this context: */
	AllocBlock	blocks;			/* head of list of blocks in this set */
	MemoryChunk* freelist[ALLOCSET_NUM_FREELISTS];	/* free chunk lists */
	/* Allocation parameters for this context: */
	Size		initBlockSize;	/* initial block size */
	Size		maxBlockSize;	/* maximum block size */
	Size		nextBlockSize;	/* next block size to allocate */
	Size		allocChunkLimit;	/* effective chunk size limit */
	AllocBlock	keeper;			/* keep this block over resets */
	/* freelist this context could be put in, or -1 if not a candidate: */
	int			freeListIndex;	/* index in context_freelists[], or -1 */
} AllocSetContext;

typedef AllocSetContext* AllocSet;

/*
 * AllocBlock
 *		An AllocBlock is the unit of memory that is obtained by aset.c
 *		from malloc().  It contains one or more MemoryChunks, which are
 *		the units requested by palloc() and freed by pfree(). MemoryChunks
 *		cannot be returned to malloc() individually, instead they are put
 *		on freelists by pfree() and re-used by the next palloc() that has
 *		a matching request size.
 *
 *		AllocBlockData is the header data for a block --- the usable space
 *		within the block begins at the next alignment boundary.
 */
typedef struct AllocBlockData
{
	AllocSet	aset;			/* aset that owns this block */
	AllocBlock	prev;			/* prev block in aset's blocks list, if any */
	AllocBlock	next;			/* next block in aset's blocks list, if any */
	char* freeptr;		/* start of free space in this block */
	char* endptr;			/* end of space in this block */
}	AllocBlockData;

/*
 * PointerIsValid
 *		True iff pointer is valid.
 */
#define PointerIsValid(pointer) ((const void*)(pointer) != NULL)

/*
 * AllocPointerIsValid
 *		True iff pointer is valid allocation pointer.
 */
#define AllocPointerIsValid(pointer) PointerIsValid(pointer)


 /*
  * AllocSetIsValid
  *		True iff set is valid allocation set.
  */
#define AllocSetIsValid(set) \
	(PointerIsValid(set) && IsA(set, AllocSetContext))

  /*
   * AllocBlockIsValid
   *		True iff block is valid block of allocation set.
   */
#define AllocBlockIsValid(block) \
	(PointerIsValid(block) && AllocSetIsValid((block)->aset))

#if 0
/*
 * Rather than repeatedly creating and deleting memory contexts, we keep some
 * freed contexts in freelists so that we can hand them out again with little
 * work.  Before putting a context in a freelist, we reset it so that it has
 * only its initial malloc chunk and no others.  To be a candidate for a
 * freelist, a context must have the same minContextSize/initBlockSize as
 * other contexts in the list; but its maxBlockSize is irrelevant since that
 * doesn't affect the size of the initial chunk.
 *
 * We currently provide one freelist for ALLOCSET_DEFAULT_SIZES contexts
 * and one for ALLOCSET_SMALL_SIZES contexts; the latter works for
 * ALLOCSET_START_SMALL_SIZES too, since only the maxBlockSize differs.
 *
 * Ordinarily, we re-use freelist contexts in last-in-first-out order, in
 * hopes of improving locality of reference.  But if there get to be too
 * many contexts in the list, we'd prefer to drop the most-recently-created
 * contexts in hopes of keeping the process memory map compact.
 * We approximate that by simply deleting all existing entries when the list
 * overflows, on the assumption that queries that allocate a lot of contexts
 * will probably free them in more or less reverse order of allocation.
 *
 * Contexts in a freelist are chained via their nextchild pointers.
 */
#define MAX_FREE_CONTEXTS 100	/* arbitrary limit on freelist length */

typedef struct AllocSetFreeList
{
	int			num_free;		/* current list length */
	AllocSetContext* first_free;	/* list header */
} AllocSetFreeList;

/* context_freelists[0] is for default params, [1] for small params */
static AllocSetFreeList context_freelists[2] =
{
	{
		0, NULL
	},
	{
		0, NULL
	}
};
#endif
/*
 * Array giving the position of the left-most set bit for each possible
 * byte value.  We count the right-most position as the 0th bit, and the
 * left-most the 7th bit.  The 0th entry of the array should not be used.
 *
 * Note: this is not used by the functions in pg_bitutils.h when
 * HAVE__BUILTIN_CLZ is defined, but we provide it anyway, so that
 * extensions possibly compiled with a different compiler can use it.
 */
static const uint8 pg_leftmost_one_pos[256] = 
{
	0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
};

/* ----------
 * AllocSetFreeIndex -
 *
 *		Depending on the size of an allocation compute which freechunk
 *		list of the alloc set it belongs to.  Caller must have verified
 *		that size <= ALLOC_CHUNK_LIMIT.
 * ----------
 */
static inline int AllocSetFreeIndex(Size size)
{
	int			idx;

	if (size > (1 << ALLOC_MINBITS))
	{
		/*----------
		 * At this point we must compute ceil(log2(size >> ALLOC_MINBITS)).
		 * This is the same as
		 *		pg_leftmost_one_pos32((size - 1) >> ALLOC_MINBITS) + 1
		 * or equivalently
		 *		pg_leftmost_one_pos32(size - 1) - ALLOC_MINBITS + 1
		 *
		 * However, for platforms without intrinsic support, we duplicate the
		 * logic here, allowing an additional optimization.  It's reasonable
		 * to assume that ALLOC_CHUNK_LIMIT fits in 16 bits, so we can unroll
		 * the byte-at-a-time loop in pg_leftmost_one_pos32 and just handle
		 * the last two bytes.
		 *
		 * Yes, this function is enough of a hot-spot to make it worth this
		 * much trouble.
		 *----------
		 */
#ifdef HAVE_BITSCAN_REVERSE
		idx = pg_leftmost_one_pos32((uint32)size - 1) - ALLOC_MINBITS + 1;
#else
		uint32		t, tsize;

		/* Statically assert that we only have a 16-bit input value. */
		/*
		StaticAssertDecl(ALLOC_CHUNK_LIMIT < (1 << 16),
			"ALLOC_CHUNK_LIMIT must be less than 64kB");
		*/
		tsize = size - 1;
		t = tsize >> 8;
		idx = t ? pg_leftmost_one_pos[t] + 8 : pg_leftmost_one_pos[tsize];
		idx -= ALLOC_MINBITS - 1;
#endif
		Assert(idx < ALLOCSET_NUM_FREELISTS);
	}
	else
		idx = 0;

	return idx;
}



/*
 * AllocSetAlloc
 *		Returns pointer to allocated memory of given size or NULL if
 *		request could not be completed; memory is added to the set.
 *
 * No request may exceed:
 *		MAXALIGN_DOWN(SIZE_MAX) - ALLOC_BLOCKHDRSZ - ALLOC_CHUNKHDRSZ
 * All callers use a much-lower limit.
 *
 * Note: when using valgrind, it doesn't matter how the returned allocation
 * is marked, as mcxt.c will set it to UNDEFINED.  In some paths we will
 * return space that is marked NOACCESS - AllocSetRealloc has to beware!
 */
void* AllocSetAlloc(MemoryContext context, Size size)
{
	AllocSet	set = (AllocSet)context;
	AllocBlock	block;
	MemoryChunk* chunk;
	int			fidx;
	Size		chunk_size;
	Size		blksize;

	Assert(AllocSetIsValid(set));

	/*
	 * If requested size exceeds maximum for chunks, allocate an entire block
	 * for this request.
	 */
	if (size > set->allocChunkLimit)
	{
#ifdef MEMORY_CONTEXT_CHECKING
		/* ensure there's always space for the sentinel byte */
		chunk_size = MAXALIGN(size + 1);
#else
		chunk_size = MAXALIGN(size);
#endif

		blksize = chunk_size + ALLOC_BLOCKHDRSZ + ALLOC_CHUNKHDRSZ;
		block = (AllocBlock)malloc(blksize);
		if (block == NULL)
			return NULL;

		context->mem_allocated += blksize;

		block->aset = set;
		block->freeptr = block->endptr = ((char*)block) + blksize;

		chunk = (MemoryChunk*)(((char*)block) + ALLOC_BLOCKHDRSZ);

		/* mark the MemoryChunk as externally managed */
		MemoryChunkSetHdrMaskExternal(chunk, MCTX_ASET_ID);

#ifdef MEMORY_CONTEXT_CHECKING
		chunk->requested_size = size;
		/* set mark to catch clobber of "unused" space */
		Assert(size < chunk_size);
		set_sentinel(MemoryChunkGetPointer(chunk), size);
#endif
#ifdef RANDOMIZE_ALLOCATED_MEMORY
		/* fill the allocated space with junk */
		randomize_mem((char*)MemoryChunkGetPointer(chunk), size);
#endif

		/*
		 * Stick the new block underneath the active allocation block, if any,
		 * so that we don't lose the use of the space remaining therein.
		 */
		if (set->blocks != NULL)
		{
			block->prev = set->blocks;
			block->next = set->blocks->next;
			if (block->next)
				block->next->prev = block;
			set->blocks->next = block;
		}
		else
		{
			block->prev = NULL;
			block->next = NULL;
			set->blocks = block;
		}

		/* Ensure any padding bytes are marked NOACCESS. */
		VALGRIND_MAKE_MEM_NOACCESS((char*)MemoryChunkGetPointer(chunk) + size, chunk_size - size);

		/* Disallow access to the chunk header. */
		VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

		return MemoryChunkGetPointer(chunk);
	}

	/*
	 * Request is small enough to be treated as a chunk.  Look in the
	 * corresponding free list to see if there is a free chunk we could reuse.
	 * If one is found, remove it from the free list, make it again a member
	 * of the alloc set and return its data address.
	 *
	 * Note that we don't attempt to ensure there's space for the sentinel
	 * byte here.  We expect a large proportion of allocations to be for sizes
	 * which are already a power of 2.  If we were to always make space for a
	 * sentinel byte in MEMORY_CONTEXT_CHECKING builds, then we'd end up
	 * doubling the memory requirements for such allocations.
	 */
	fidx = AllocSetFreeIndex(size);
	chunk = set->freelist[fidx];
	if (chunk != NULL)
	{
		AllocFreeListLink* link = GetFreeListLink(chunk);

		/* Allow access to the chunk header. */
		VALGRIND_MAKE_MEM_DEFINED(chunk, ALLOC_CHUNKHDRSZ);

		Assert(fidx == MemoryChunkGetValue(chunk));

		/* pop this chunk off the freelist */
		VALGRIND_MAKE_MEM_DEFINED(link, sizeof(AllocFreeListLink));
		set->freelist[fidx] = link->next;
		VALGRIND_MAKE_MEM_NOACCESS(link, sizeof(AllocFreeListLink));

#ifdef MEMORY_CONTEXT_CHECKING
		chunk->requested_size = size;
		/* set mark to catch clobber of "unused" space */
		if (size < GetChunkSizeFromFreeListIdx(fidx))
			set_sentinel(MemoryChunkGetPointer(chunk), size);
#endif
#ifdef RANDOMIZE_ALLOCATED_MEMORY
		/* fill the allocated space with junk */
		randomize_mem((char*)MemoryChunkGetPointer(chunk), size);
#endif

		/* Ensure any padding bytes are marked NOACCESS. */
		VALGRIND_MAKE_MEM_NOACCESS((char*)MemoryChunkGetPointer(chunk) + size,
			GetChunkSizeFromFreeListIdx(fidx) - size);

		/* Disallow access to the chunk header. */
		VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

		return MemoryChunkGetPointer(chunk);
	}

	/*
	 * Choose the actual chunk size to allocate.
	 */
	chunk_size = GetChunkSizeFromFreeListIdx(fidx);
	Assert(chunk_size >= size);

	/*
	 * If there is enough room in the active allocation block, we will put the
	 * chunk into that block.  Else must start a new one.
	 */
	if ((block = set->blocks) != NULL)
	{
		Size		availspace = block->endptr - block->freeptr;

		if (availspace < (chunk_size + ALLOC_CHUNKHDRSZ))
		{
			/*
			 * The existing active (top) block does not have enough room for
			 * the requested allocation, but it might still have a useful
			 * amount of space in it.  Once we push it down in the block list,
			 * we'll never try to allocate more space from it. So, before we
			 * do that, carve up its free space into chunks that we can put on
			 * the set's freelists.
			 *
			 * Because we can only get here when there's less than
			 * ALLOC_CHUNK_LIMIT left in the block, this loop cannot iterate
			 * more than ALLOCSET_NUM_FREELISTS-1 times.
			 */
			while (availspace >= ((1 << ALLOC_MINBITS) + ALLOC_CHUNKHDRSZ))
			{
				AllocFreeListLink* link;
				Size		availchunk = availspace - ALLOC_CHUNKHDRSZ;
				int			a_fidx = AllocSetFreeIndex(availchunk);

				/*
				 * In most cases, we'll get back the index of the next larger
				 * freelist than the one we need to put this chunk on.  The
				 * exception is when availchunk is exactly a power of 2.
				 */
				if (availchunk != GetChunkSizeFromFreeListIdx(a_fidx))
				{
					a_fidx--;
					Assert(a_fidx >= 0);
					availchunk = GetChunkSizeFromFreeListIdx(a_fidx);
				}

				chunk = (MemoryChunk*)(block->freeptr);

				/* Prepare to initialize the chunk header. */
				VALGRIND_MAKE_MEM_UNDEFINED(chunk, ALLOC_CHUNKHDRSZ);
				block->freeptr += (availchunk + ALLOC_CHUNKHDRSZ);
				availspace -= (availchunk + ALLOC_CHUNKHDRSZ);

				/* store the freelist index in the value field */
				MemoryChunkSetHdrMask(chunk, block, a_fidx, MCTX_ASET_ID);
#ifdef MEMORY_CONTEXT_CHECKING
				chunk->requested_size = InvalidAllocSize;	/* mark it free */
#endif
				/* push this chunk onto the free list */
				link = GetFreeListLink(chunk);

				VALGRIND_MAKE_MEM_DEFINED(link, sizeof(AllocFreeListLink));
				link->next = set->freelist[a_fidx];
				VALGRIND_MAKE_MEM_NOACCESS(link, sizeof(AllocFreeListLink));

				set->freelist[a_fidx] = chunk;
			}
			/* Mark that we need to create a new block */
			block = NULL;
		}
	}

	/*
	 * Time to create a new regular (multi-chunk) block?
	 */
	if (block == NULL)
	{
		Size		required_size;

		/*
		 * The first such block has size initBlockSize, and we double the
		 * space in each succeeding block, but not more than maxBlockSize.
		 */
		blksize = set->nextBlockSize;
		set->nextBlockSize <<= 1;
		if (set->nextBlockSize > set->maxBlockSize)
			set->nextBlockSize = set->maxBlockSize;

		/*
		 * If initBlockSize is less than ALLOC_CHUNK_LIMIT, we could need more
		 * space... but try to keep it a power of 2.
		 */
		required_size = chunk_size + ALLOC_BLOCKHDRSZ + ALLOC_CHUNKHDRSZ;
		while (blksize < required_size)
			blksize <<= 1;

		/* Try to allocate it */
		block = (AllocBlock)malloc(blksize);

		/*
		 * We could be asking for pretty big blocks here, so cope if malloc
		 * fails.  But give up if there's less than 1 MB or so available...
		 */
		while (block == NULL && blksize > 1024 * 1024)
		{
			blksize >>= 1;
			if (blksize < required_size)
				break;
			block = (AllocBlock)malloc(blksize);
		}

		if (block == NULL)
			return NULL;

		context->mem_allocated += blksize;

		block->aset = set;
		block->freeptr = ((char*)block) + ALLOC_BLOCKHDRSZ;
		block->endptr = ((char*)block) + blksize;

		/* Mark unallocated space NOACCESS. */
		VALGRIND_MAKE_MEM_NOACCESS(block->freeptr,
			blksize - ALLOC_BLOCKHDRSZ);

		block->prev = NULL;
		block->next = set->blocks;
		if (block->next)
			block->next->prev = block;
		set->blocks = block;
	}

	/*
	 * OK, do the allocation
	 */
	chunk = (MemoryChunk*)(block->freeptr);

	/* Prepare to initialize the chunk header. */
	VALGRIND_MAKE_MEM_UNDEFINED(chunk, ALLOC_CHUNKHDRSZ);

	block->freeptr += (chunk_size + ALLOC_CHUNKHDRSZ);
	Assert(block->freeptr <= block->endptr);

	/* store the free list index in the value field */
	MemoryChunkSetHdrMask(chunk, block, fidx, MCTX_ASET_ID);

#ifdef MEMORY_CONTEXT_CHECKING
	chunk->requested_size = size;
	/* set mark to catch clobber of "unused" space */
	if (size < chunk_size)
		set_sentinel(MemoryChunkGetPointer(chunk), size);
#endif
#ifdef RANDOMIZE_ALLOCATED_MEMORY
	/* fill the allocated space with junk */
	randomize_mem((char*)MemoryChunkGetPointer(chunk), size);
#endif

	/* Ensure any padding bytes are marked NOACCESS. */
	VALGRIND_MAKE_MEM_NOACCESS((char*)MemoryChunkGetPointer(chunk) + size,
		chunk_size - size);

	/* Disallow access to the chunk header. */
	VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

	return MemoryChunkGetPointer(chunk);
}

/*
 * AllocSetFree
 *		Frees allocated memory; memory is removed from the set.
 */
void AllocSetFree(void* pointer)
{
	AllocSet	set;
	MemoryChunk* chunk = PointerGetMemoryChunk(pointer);

	/* Allow access to the chunk header. */
	VALGRIND_MAKE_MEM_DEFINED(chunk, ALLOC_CHUNKHDRSZ);

	if (MemoryChunkIsExternal(chunk))
	{
		/* Release single-chunk block. */
		AllocBlock	block = ExternalChunkGetBlock(chunk);

		/*
		 * Try to verify that we have a sane block pointer: the block header
		 * should reference an aset and the freeptr should match the endptr.
		 */
		if (!AllocBlockIsValid(block) || block->freeptr != block->endptr)
		{
			//elog(ERROR, "could not find block containing chunk %p", chunk);
			exit(1);
		}

		set = block->aset;

#ifdef MEMORY_CONTEXT_CHECKING
		{
			/* Test for someone scribbling on unused space in chunk */
			Assert(chunk->requested_size < (block->endptr - (char*)pointer));
			if (!sentinel_ok(pointer, chunk->requested_size))
				elog(WARNING, "detected write past chunk end in %s %p",
					set->header.name, chunk);
		}
#endif

		/* OK, remove block from aset's list and free it */
		if (block->prev)
			block->prev->next = block->next;
		else
			set->blocks = block->next;
		if (block->next)
			block->next->prev = block->prev;

		set->header.mem_allocated -= block->endptr - ((char*)block);

#ifdef CLOBBER_FREED_MEMORY
		wipe_mem(block, block->freeptr - ((char*)block));
#endif
		free(block);
	}
	else
	{
		AllocBlock	block = MemoryChunkGetBlock(chunk);
		int			fidx;
		AllocFreeListLink* link;

		/*
		 * In this path, for speed reasons we just Assert that the referenced
		 * block is good.  We can also Assert that the value field is sane.
		 * Future field experience may show that these Asserts had better
		 * become regular runtime test-and-elog checks.
		 */
		Assert(AllocBlockIsValid(block));
		set = block->aset;

		fidx = MemoryChunkGetValue(chunk);
		Assert(FreeListIdxIsValid(fidx));
		link = GetFreeListLink(chunk);

#ifdef MEMORY_CONTEXT_CHECKING
		/* Test for someone scribbling on unused space in chunk */
		if (chunk->requested_size < GetChunkSizeFromFreeListIdx(fidx))
			if (!sentinel_ok(pointer, chunk->requested_size))
				elog(WARNING, "detected write past chunk end in %s %p",
					set->header.name, chunk);
#endif

#ifdef CLOBBER_FREED_MEMORY
		wipe_mem(pointer, GetChunkSizeFromFreeListIdx(fidx));
#endif
		/* push this chunk onto the top of the free list */
		VALGRIND_MAKE_MEM_DEFINED(link, sizeof(AllocFreeListLink));
		link->next = set->freelist[fidx];
		VALGRIND_MAKE_MEM_NOACCESS(link, sizeof(AllocFreeListLink));
		set->freelist[fidx] = chunk;

#ifdef MEMORY_CONTEXT_CHECKING

		/*
		 * Reset requested_size to InvalidAllocSize in chunks that are on free
		 * list.
		 */
		chunk->requested_size = InvalidAllocSize;
#endif
	}
}

/*
 * AllocSetRealloc
 *		Returns new pointer to allocated memory of given size or NULL if
 *		request could not be completed; this memory is added to the set.
 *		Memory associated with given pointer is copied into the new memory,
 *		and the old memory is freed.
 *
 * Without MEMORY_CONTEXT_CHECKING, we don't know the old request size.  This
 * makes our Valgrind client requests less-precise, hazarding false negatives.
 * (In principle, we could use VALGRIND_GET_VBITS() to rediscover the old
 * request size.)
 */
void* AllocSetRealloc(void* pointer, Size size)
{
	AllocBlock	block;
	AllocSet	set;
	MemoryChunk* chunk = PointerGetMemoryChunk(pointer);
	Size		oldchksize;
	int			fidx;

	/* Allow access to the chunk header. */
	VALGRIND_MAKE_MEM_DEFINED(chunk, ALLOC_CHUNKHDRSZ);

	if (MemoryChunkIsExternal(chunk))
	{
		/*
		 * The chunk must have been allocated as a single-chunk block.  Use
		 * realloc() to make the containing block bigger, or smaller, with
		 * minimum space wastage.
		 */
		Size		chksize;
		Size		blksize;
		Size		oldblksize;

		block = ExternalChunkGetBlock(chunk);

		/*
		 * Try to verify that we have a sane block pointer: the block header
		 * should reference an aset and the freeptr should match the endptr.
		 */
		if (!AllocBlockIsValid(block) || block->freeptr != block->endptr)
		{
			//elog(ERROR, "could not find block containing chunk %p", chunk);
			exit(1);
		}

		set = block->aset;

		oldchksize = block->endptr - (char*)pointer;

#ifdef MEMORY_CONTEXT_CHECKING
		/* Test for someone scribbling on unused space in chunk */
		Assert(chunk->requested_size < oldchksize);
		if (!sentinel_ok(pointer, chunk->requested_size))
			elog(WARNING, "detected write past chunk end in %s %p",
				set->header.name, chunk);
#endif

#ifdef MEMORY_CONTEXT_CHECKING
		/* ensure there's always space for the sentinel byte */
		chksize = MAXALIGN(size + 1);
#else
		chksize = MAXALIGN(size);
#endif

		/* Do the realloc */
		blksize = chksize + ALLOC_BLOCKHDRSZ + ALLOC_CHUNKHDRSZ;
		oldblksize = block->endptr - ((char*)block);

		block = (AllocBlock)realloc(block, blksize);
		if (block == NULL)
		{
			/* Disallow access to the chunk header. */
			VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);
			return NULL;
		}

		/* updated separately, not to underflow when (oldblksize > blksize) */
		set->header.mem_allocated -= oldblksize;
		set->header.mem_allocated += blksize;

		block->freeptr = block->endptr = ((char*)block) + blksize;

		/* Update pointers since block has likely been moved */
		chunk = (MemoryChunk*)(((char*)block) + ALLOC_BLOCKHDRSZ);
		pointer = MemoryChunkGetPointer(chunk);
		if (block->prev)
			block->prev->next = block;
		else
			set->blocks = block;
		if (block->next)
			block->next->prev = block;

#ifdef MEMORY_CONTEXT_CHECKING
#ifdef RANDOMIZE_ALLOCATED_MEMORY

		/*
		 * We can only randomize the extra space if we know the prior request.
		 * When using Valgrind, randomize_mem() also marks memory UNDEFINED.
		 */
		if (size > chunk->requested_size)
			randomize_mem((char*)pointer + chunk->requested_size,
				size - chunk->requested_size);
#else

		/*
		 * If this is an increase, realloc() will have marked any
		 * newly-allocated part (from oldchksize to chksize) UNDEFINED, but we
		 * also need to adjust trailing bytes from the old allocation (from
		 * chunk->requested_size to oldchksize) as they are marked NOACCESS.
		 * Make sure not to mark too many bytes in case chunk->requested_size
		 * < size < oldchksize.
		 */
#ifdef USE_VALGRIND
		if (Min(size, oldchksize) > chunk->requested_size)
			VALGRIND_MAKE_MEM_UNDEFINED((char*)pointer + chunk->requested_size,
				Min(size, oldchksize) - chunk->requested_size);
#endif
#endif

		chunk->requested_size = size;
		/* set mark to catch clobber of "unused" space */
		Assert(size < chksize);
		set_sentinel(pointer, size);
#else							/* !MEMORY_CONTEXT_CHECKING */

		/*
		 * We may need to adjust marking of bytes from the old allocation as
		 * some of them may be marked NOACCESS.  We don't know how much of the
		 * old chunk size was the requested size; it could have been as small
		 * as one byte.  We have to be conservative and just mark the entire
		 * old portion DEFINED.  Make sure not to mark memory beyond the new
		 * allocation in case it's smaller than the old one.
		 */
		VALGRIND_MAKE_MEM_DEFINED(pointer, Min(size, oldchksize));
#endif

		/* Ensure any padding bytes are marked NOACCESS. */
		VALGRIND_MAKE_MEM_NOACCESS((char*)pointer + size, chksize - size);

		/* Disallow access to the chunk header . */
		VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

		return pointer;
	}

	block = MemoryChunkGetBlock(chunk);

	/*
	 * In this path, for speed reasons we just Assert that the referenced
	 * block is good. We can also Assert that the value field is sane. Future
	 * field experience may show that these Asserts had better become regular
	 * runtime test-and-elog checks.
	 */
	Assert(AllocBlockIsValid(block));
	set = block->aset;

	fidx = MemoryChunkGetValue(chunk);
	Assert(FreeListIdxIsValid(fidx));
	oldchksize = GetChunkSizeFromFreeListIdx(fidx);

#ifdef MEMORY_CONTEXT_CHECKING
	/* Test for someone scribbling on unused space in chunk */
	if (chunk->requested_size < oldchksize)
		if (!sentinel_ok(pointer, chunk->requested_size))
			elog(WARNING, "detected write past chunk end in %s %p",
				set->header.name, chunk);
#endif

	/*
	 * Chunk sizes are aligned to power of 2 in AllocSetAlloc().  Maybe the
	 * allocated area already is >= the new size.  (In particular, we will
	 * fall out here if the requested size is a decrease.)
	 */
	if (oldchksize >= size)
	{
#ifdef MEMORY_CONTEXT_CHECKING
		Size		oldrequest = chunk->requested_size;

#ifdef RANDOMIZE_ALLOCATED_MEMORY
		/* We can only fill the extra space if we know the prior request */
		if (size > oldrequest)
			randomize_mem((char*)pointer + oldrequest,
				size - oldrequest);
#endif

		chunk->requested_size = size;

		/*
		 * If this is an increase, mark any newly-available part UNDEFINED.
		 * Otherwise, mark the obsolete part NOACCESS.
		 */
		if (size > oldrequest)
			VALGRIND_MAKE_MEM_UNDEFINED((char*)pointer + oldrequest,
				size - oldrequest);
		else
			VALGRIND_MAKE_MEM_NOACCESS((char*)pointer + size,
				oldchksize - size);

		/* set mark to catch clobber of "unused" space */
		if (size < oldchksize)
			set_sentinel(pointer, size);
#else							/* !MEMORY_CONTEXT_CHECKING */

		/*
		 * We don't have the information to determine whether we're growing
		 * the old request or shrinking it, so we conservatively mark the
		 * entire new allocation DEFINED.
		 */
		VALGRIND_MAKE_MEM_NOACCESS(pointer, oldchksize);
		VALGRIND_MAKE_MEM_DEFINED(pointer, size);
#endif

		/* Disallow access to the chunk header. */
		VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

		return pointer;
	}
	else
	{
		/*
		 * Enlarge-a-small-chunk case.  We just do this by brute force, ie,
		 * allocate a new chunk and copy the data.  Since we know the existing
		 * data isn't huge, this won't involve any great memcpy expense, so
		 * it's not worth being smarter.  (At one time we tried to avoid
		 * memcpy when it was possible to enlarge the chunk in-place, but that
		 * turns out to misbehave unpleasantly for repeated cycles of
		 * palloc/repalloc/pfree: the eventually freed chunks go into the
		 * wrong freelist for the next initial palloc request, and so we leak
		 * memory indefinitely.  See pgsql-hackers archives for 2007-08-11.)
		 */
		AllocPointer newPointer;
		Size		oldsize;

		/* allocate new chunk */
		newPointer = AllocSetAlloc((MemoryContext)set, size);

		/* leave immediately if request was not completed */
		if (newPointer == NULL)
		{
			/* Disallow access to the chunk header. */
			VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);
			return NULL;
		}

		/*
		 * AllocSetAlloc() may have returned a region that is still NOACCESS.
		 * Change it to UNDEFINED for the moment; memcpy() will then transfer
		 * definedness from the old allocation to the new.  If we know the old
		 * allocation, copy just that much.  Otherwise, make the entire old
		 * chunk defined to avoid errors as we copy the currently-NOACCESS
		 * trailing bytes.
		 */
		VALGRIND_MAKE_MEM_UNDEFINED(newPointer, size);
#ifdef MEMORY_CONTEXT_CHECKING
		oldsize = chunk->requested_size;
#else
		oldsize = oldchksize;
		VALGRIND_MAKE_MEM_DEFINED(pointer, oldsize);
#endif

		/* transfer existing data (certain to fit) */
		memcpy(newPointer, pointer, oldsize);

		/* free old chunk */
		AllocSetFree(pointer);

		return newPointer;
	}
}

/*
 * AllocSetReset
 *		Frees all memory which is allocated in the given set.
 *
 * Actually, this routine has some discretion about what to do.
 * It should mark all allocated chunks freed, but it need not necessarily
 * give back all the resources the set owns.  Our actual implementation is
 * that we give back all but the "keeper" block (which we must keep, since
 * it shares a malloc chunk with the context header).  In this way, we don't
 * thrash malloc() when a context is repeatedly reset after small allocations,
 * which is typical behavior for per-tuple contexts.
 */
void AllocSetReset(MemoryContext context)
{
	AllocSet	set = (AllocSet)context;
	AllocBlock	block;
	Size		keepersize;

	Assert(AllocSetIsValid(set));

#ifdef MEMORY_CONTEXT_CHECKING
	/* Check for corruption and leaks before freeing */
	AllocSetCheck(context);
#endif

	/* Remember keeper block size for Assert below */
	keepersize = set->keeper->endptr - ((char*)set);

	/* Clear chunk freelists */
	MemSetAligned(set->freelist, 0, sizeof(set->freelist));

	block = set->blocks;

	/* New blocks list will be just the keeper block */
	set->blocks = set->keeper;

	while (block != NULL)
	{
		AllocBlock	next = block->next;

		if (block == set->keeper)
		{
			/* Reset the block, but don't return it to malloc */
			char* datastart = ((char*)block) + ALLOC_BLOCKHDRSZ;

#ifdef CLOBBER_FREED_MEMORY
			wipe_mem(datastart, block->freeptr - datastart);
#else
			/* wipe_mem() would have done this */
			VALGRIND_MAKE_MEM_NOACCESS(datastart, block->freeptr - datastart);
#endif
			block->freeptr = datastart;
			block->prev = NULL;
			block->next = NULL;
		}
		else
		{
			/* Normal case, release the block */
			context->mem_allocated -= block->endptr - ((char*)block);

#ifdef CLOBBER_FREED_MEMORY
			wipe_mem(block, block->freeptr - ((char*)block));
#endif
			free(block);
		}
		block = next;
	}

	Assert(context->mem_allocated == keepersize);

	/* Reset block size allocation sequence, too */
	set->nextBlockSize = set->initBlockSize;
}

/*
 * AllocSetDelete
 *		Frees all memory which is allocated in the given set,
 *		in preparation for deletion of the set.
 *
 * Unlike AllocSetReset, this *must* free all resources of the set.
 */
void AllocSetDelete(MemoryContext context)
{
	AllocSet	set = (AllocSet)context;
	AllocBlock	block = set->blocks;
	Size		keepersize;

	Assert(AllocSetIsValid(set));

#ifdef MEMORY_CONTEXT_CHECKING
	/* Check for corruption and leaks before freeing */
	AllocSetCheck(context);
#endif

	/* Remember keeper block size for Assert below */
	keepersize = set->keeper->endptr - ((char*)set);

#if 0
	/*
	 * If the context is a candidate for a freelist, put it into that freelist
	 * instead of destroying it.
	 */
	if (set->freeListIndex >= 0)
	{
		AllocSetFreeList* freelist = &context_freelists[set->freeListIndex];

		/*
		 * Reset the context, if it needs it, so that we aren't hanging on to
		 * more than the initial malloc chunk.
		 */
		if (!context->isReset)
			MemoryContextResetOnly(context);

		/*
		 * If the freelist is full, just discard what's already in it.  See
		 * comments with context_freelists[].
		 */
		if (freelist->num_free >= MAX_FREE_CONTEXTS)
		{
			while (freelist->first_free != NULL)
			{
				AllocSetContext* oldset = freelist->first_free;

				freelist->first_free = (AllocSetContext*)oldset->header.nextchild;
				freelist->num_free--;

				/* All that remains is to free the header/initial block */
				free(oldset);
			}
			Assert(freelist->num_free == 0);
		}

		/* Now add the just-deleted context to the freelist. */
		set->header.nextchild = (MemoryContext)freelist->first_free;
		freelist->first_free = set;
		freelist->num_free++;

		return;
	}
#endif
	/* Free all blocks, except the keeper which is part of context header */
	while (block != NULL)
	{
		AllocBlock	next = block->next;

		if (block != set->keeper)
			context->mem_allocated -= block->endptr - ((char*)block);

#ifdef CLOBBER_FREED_MEMORY
		wipe_mem(block, block->freeptr - ((char*)block));
#endif

		if (block != set->keeper)
			free(block);

		block = next;
	}

	Assert(context->mem_allocated == keepersize);

	/* Finally, free the context header, including the keeper block */
	free(set);
}

/*
 * AllocSetGetChunkContext
 *		Return the MemoryContext that 'pointer' belongs to.
 */
MemoryContext AllocSetGetChunkContext(void* pointer)
{
	MemoryChunk* chunk = PointerGetMemoryChunk(pointer);
	AllocBlock	block;
	AllocSet	set;

	/* Allow access to the chunk header. */
	VALGRIND_MAKE_MEM_DEFINED(chunk, ALLOC_CHUNKHDRSZ);

	if (MemoryChunkIsExternal(chunk))
		block = ExternalChunkGetBlock(chunk);
	else
		block = (AllocBlock)MemoryChunkGetBlock(chunk);

	/* Disallow access to the chunk header. */
	VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

	Assert(AllocBlockIsValid(block));
	set = block->aset;

	return &set->header;
}

/*
 * AllocSetGetChunkSpace
 *		Given a currently-allocated chunk, determine the total space
 *		it occupies (including all memory-allocation overhead).
 */
Size AllocSetGetChunkSpace(void* pointer)
{
	MemoryChunk* chunk = PointerGetMemoryChunk(pointer);
	int			fidx;

	/* Allow access to the chunk header. */
	VALGRIND_MAKE_MEM_DEFINED(chunk, ALLOC_CHUNKHDRSZ);

	if (MemoryChunkIsExternal(chunk))
	{
		AllocBlock	block = ExternalChunkGetBlock(chunk);

		/* Disallow access to the chunk header. */
		VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

		Assert(AllocBlockIsValid(block));

		return block->endptr - (char*)chunk;
	}

	fidx = MemoryChunkGetValue(chunk);
	Assert(FreeListIdxIsValid(fidx));

	/* Disallow access to the chunk header. */
	VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

	return GetChunkSizeFromFreeListIdx(fidx) + ALLOC_CHUNKHDRSZ;
}

/*
 * AllocSetIsEmpty
 *		Is an allocset empty of any allocated space?
 */
bool AllocSetIsEmpty(MemoryContext context)
{
	Assert(AllocSetIsValid(context));

	/*
	 * For now, we say "empty" only if the context is new or just reset. We
	 * could examine the freelists to determine if all space has been freed,
	 * but it's not really worth the trouble for present uses of this
	 * functionality.
	 */
	if (context->isReset)
		return true;
	return false;
}

/*
 * AllocSetStats
 *		Compute stats about memory consumption of an allocset.
 *
 * printfunc: if not NULL, pass a human-readable stats string to this.
 * passthru: pass this pointer through to printfunc.
 * totals: if not NULL, add stats about this context into *totals.
 * print_to_stderr: print stats to stderr if true, elog otherwise.
 */
void AllocSetStats(MemoryContext context, MemoryStatsPrintFunc printfunc, void* passthru, MemoryContextCounters* totals, bool print_to_stderr)
{
#if 0
	AllocSet	set = (AllocSet)context;
	Size		nblocks = 0;
	Size		freechunks = 0;
	Size		totalspace;
	Size		freespace = 0;
	AllocBlock	block;
	int			fidx;

	Assert(AllocSetIsValid(set));

	/* Include context header in totalspace */
	totalspace = MAXALIGN(sizeof(AllocSetContext));

	for (block = set->blocks; block != NULL; block = block->next)
	{
		nblocks++;
		totalspace += block->endptr - ((char*)block);
		freespace += block->endptr - block->freeptr;
	}
	for (fidx = 0; fidx < ALLOCSET_NUM_FREELISTS; fidx++)
	{
		Size		chksz = GetChunkSizeFromFreeListIdx(fidx);
		MemoryChunk* chunk = set->freelist[fidx];

		while (chunk != NULL)
		{
			AllocFreeListLink* link = GetFreeListLink(chunk);

			/* Allow access to the chunk header. */
			VALGRIND_MAKE_MEM_DEFINED(chunk, ALLOC_CHUNKHDRSZ);
			Assert(MemoryChunkGetValue(chunk) == fidx);
			VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

			freechunks++;
			freespace += chksz + ALLOC_CHUNKHDRSZ;

			VALGRIND_MAKE_MEM_DEFINED(link, sizeof(AllocFreeListLink));
			chunk = link->next;
			VALGRIND_MAKE_MEM_NOACCESS(link, sizeof(AllocFreeListLink));
		}
	}

	if (printfunc)
	{
		char		stats_string[200];

		snprintf(stats_string, sizeof(stats_string),
			"%zu total in %zu blocks; %zu free (%zu chunks); %zu used",
			totalspace, nblocks, freespace, freechunks,
			totalspace - freespace);
		printfunc(context, passthru, stats_string, print_to_stderr);
	}

	if (totals)
	{
		totals->nblocks += nblocks;
		totals->freechunks += freechunks;
		totals->totalspace += totalspace;
		totals->freespace += freespace;
	}
#endif
}

#ifdef MEMORY_CONTEXT_CHECKING
/*
 * AllocSetCheck
 *		Walk through chunks and check consistency of memory.
 *
 * NOTE: report errors as WARNING, *not* ERROR or FATAL.  Otherwise you'll
 * find yourself in an infinite loop when trouble occurs, because this
 * routine will be entered again when elog cleanup tries to release memory!
 */
void AllocSetCheck(MemoryContext context)
{
	AllocSet	set = (AllocSet)context;
	const char* name = set->header.name;
	AllocBlock	prevblock;
	AllocBlock	block;
	Size		total_allocated = 0;

	for (prevblock = NULL, block = set->blocks;
		block != NULL;
		prevblock = block, block = block->next)
	{
		char* bpoz = ((char*)block) + ALLOC_BLOCKHDRSZ;
		long		blk_used = block->freeptr - bpoz;
		long		blk_data = 0;
		long		nchunks = 0;
		bool		has_external_chunk = false;

		if (set->keeper == block)
			total_allocated += block->endptr - ((char*)set);
		else
			total_allocated += block->endptr - ((char*)block);

		/*
		 * Empty block - empty can be keeper-block only
		 */
		if (!blk_used)
		{
			if (set->keeper != block)
				elog(WARNING, "problem in alloc set %s: empty block %p",
					name, block);
		}

		/*
		 * Check block header fields
		 */
		if (block->aset != set ||
			block->prev != prevblock ||
			block->freeptr < bpoz ||
			block->freeptr > block->endptr)
			elog(WARNING, "problem in alloc set %s: corrupt header in block %p",
				name, block);

		/*
		 * Chunk walker
		 */
		while (bpoz < block->freeptr)
		{
			MemoryChunk* chunk = (MemoryChunk*)bpoz;
			Size		chsize,
				dsize;

			/* Allow access to the chunk header. */
			VALGRIND_MAKE_MEM_DEFINED(chunk, ALLOC_CHUNKHDRSZ);

			if (MemoryChunkIsExternal(chunk))
			{
				chsize = block->endptr - (char*)MemoryChunkGetPointer(chunk); /* aligned chunk size */
				has_external_chunk = true;

				/* make sure this chunk consumes the entire block */
				if (chsize + ALLOC_CHUNKHDRSZ != blk_used)
					elog(WARNING, "problem in alloc set %s: bad single-chunk %p in block %p",
						name, chunk, block);
			}
			else
			{
				int			fidx = MemoryChunkGetValue(chunk);

				if (!FreeListIdxIsValid(fidx))
					elog(WARNING, "problem in alloc set %s: bad chunk size for chunk %p in block %p",
						name, chunk, block);

				chsize = GetChunkSizeFromFreeListIdx(fidx); /* aligned chunk size */

				/*
				 * Check the stored block offset correctly references this
				 * block.
				 */
				if (block != MemoryChunkGetBlock(chunk))
					elog(WARNING, "problem in alloc set %s: bad block offset for chunk %p in block %p",
						name, chunk, block);
			}
			dsize = chunk->requested_size;	/* real data */

			/* an allocated chunk's requested size must be <= the chsize */
			if (dsize != InvalidAllocSize && dsize > chsize)
				elog(WARNING, "problem in alloc set %s: req size > alloc size for chunk %p in block %p",
					name, chunk, block);

			/* chsize must not be smaller than the first freelist's size */
			if (chsize < (1 << ALLOC_MINBITS))
				elog(WARNING, "problem in alloc set %s: bad size %zu for chunk %p in block %p",
					name, chsize, chunk, block);

			/*
			 * Check for overwrite of padding space in an allocated chunk.
			 */
			if (dsize != InvalidAllocSize && dsize < chsize &&
				!sentinel_ok(chunk, ALLOC_CHUNKHDRSZ + dsize))
				elog(WARNING, "problem in alloc set %s: detected write past chunk end in block %p, chunk %p",
					name, block, chunk);

			/* if chunk is allocated, disallow access to the chunk header */
			if (dsize != InvalidAllocSize)
				VALGRIND_MAKE_MEM_NOACCESS(chunk, ALLOC_CHUNKHDRSZ);

			blk_data += chsize;
			nchunks++;

			bpoz += ALLOC_CHUNKHDRSZ + chsize;
		}

		if ((blk_data + (nchunks * ALLOC_CHUNKHDRSZ)) != blk_used)
			elog(WARNING, "problem in alloc set %s: found inconsistent memory block %p",
				name, block);

		if (has_external_chunk && nchunks > 1)
			elog(WARNING, "problem in alloc set %s: external chunk on non-dedicated block %p",
				name, block);
	}

	Assert(total_allocated == context->mem_allocated);
}
#endif

/* These functions implement the MemoryContext API for Generation context. */
void* GenerationAlloc(MemoryContext context, Size size)
{
	return NULL;
}

void GenerationFree(void* pointer)
{
}

void* GenerationRealloc(void* pointer, Size size)
{
	return NULL;
}

void GenerationReset(MemoryContext context)
{
}

void GenerationDelete(MemoryContext context)
{
}

MemoryContext GenerationGetChunkContext(void* pointer)
{
	return NULL;
}

Size GenerationGetChunkSpace(void* pointer)
{
	return 0;
}

bool GenerationIsEmpty(MemoryContext context)
{
	return false;
}

void GenerationStats(MemoryContext context,
	MemoryStatsPrintFunc printfunc, void* passthru,
	MemoryContextCounters* totals,
	bool print_to_stderr)
{
}

#ifdef MEMORY_CONTEXT_CHECKING
void GenerationCheck(MemoryContext context)
{
}

#endif


/* These functions implement the MemoryContext API for Slab context. */
void* SlabAlloc(MemoryContext context, Size size)
{
	return NULL;
}

void SlabFree(void* pointer)
{
}

void* SlabRealloc(void* pointer, Size size)
{
	return NULL;
}

void SlabReset(MemoryContext context)
{
}

void SlabDelete(MemoryContext context)
{
}

MemoryContext SlabGetChunkContext(void* pointer)
{
	return NULL;
}

Size SlabGetChunkSpace(void* pointer)
{
	return 0;
}

bool SlabIsEmpty(MemoryContext context)
{
	return false;
}

void SlabStats(MemoryContext context, MemoryStatsPrintFunc printfunc, void* passthru, MemoryContextCounters* totals, bool print_to_stderr)
{
}

#ifdef MEMORY_CONTEXT_CHECKING
void SlabCheck(MemoryContext context)
{
}

#endif

/*
 * These functions support the implementation of palloc_aligned() and are not
 * part of a fully-fledged MemoryContext type.
 */
void AlignedAllocFree(void* pointer)
{
}

void* AlignedAllocRealloc(void* pointer, Size size)
{
	return NULL;
}

MemoryContext AlignedAllocGetChunkContext(void* pointer)
{
	return NULL;
}

Size AlignedAllocGetChunkSpace(void* pointer)
{
	return 0;
}

/*
 * MemoryContextStats
 *		Print statistics about the named context and all its descendants.
 *
 * This is just a debugging utility, so it's not very fancy.  However, we do
 * make some effort to summarize when the output would otherwise be very long.
 * The statistics are sent to stderr.
 */
void MemoryContextStats(MemoryContext context)
{
	/* A hard-wired limit on the number of children is usually good enough */
#if 0
	MemoryContextStatsDetail(context, 100, true);
#endif
}

/*
 * Public routines
 */

 /*
  * AllocSetContextCreateInternal
  *		Create a new AllocSet context.
  *
  * parent: parent context, or NULL if top-level context
  * name: name of context (must be statically allocated)
  * minContextSize: minimum context size
  * initBlockSize: initial allocation block size
  * maxBlockSize: maximum allocation block size
  *
  * Most callers should abstract the context size parameters using a macro
  * such as ALLOCSET_DEFAULT_SIZES.
  *
  * Note: don't call this directly; go through the wrapper macro
  * AllocSetContextCreate.
  */
MemoryContext AllocSetContextCreateInternal(MemoryContext parent, const char* name, Size minContextSize, Size initBlockSize, Size maxBlockSize)
{
	int			freeListIndex;
	Size		firstBlockSize;
	AllocSet	set;
	AllocBlock	block;

	/* ensure MemoryChunk's size is properly maxaligned */
	StaticAssertDecl(ALLOC_CHUNKHDRSZ == MAXALIGN(ALLOC_CHUNKHDRSZ), "sizeof(MemoryChunk) is not maxaligned");
	/* check we have enough space to store the freelist link */
	StaticAssertDecl(sizeof(AllocFreeListLink) <= (1 << ALLOC_MINBITS),
		"sizeof(AllocFreeListLink) larger than minimum allocation size");

	/*
	 * First, validate allocation parameters.  Once these were regular runtime
	 * tests and elog's, but in practice Asserts seem sufficient because
	 * nobody varies their parameters at runtime.  We somewhat arbitrarily
	 * enforce a minimum 1K block size.  We restrict the maximum block size to
	 * MEMORYCHUNK_MAX_BLOCKOFFSET as MemoryChunks are limited to this in
	 * regards to addressing the offset between the chunk and the block that
	 * the chunk is stored on.  We would be unable to store the offset between
	 * the chunk and block for any chunks that were beyond
	 * MEMORYCHUNK_MAX_BLOCKOFFSET bytes into the block if the block was to be
	 * larger than this.
	 */
	Assert(initBlockSize == MAXALIGN(initBlockSize) && initBlockSize >= 1024);
	Assert(maxBlockSize == MAXALIGN(maxBlockSize) && maxBlockSize >= initBlockSize && AllocHugeSizeIsValid(maxBlockSize)); /* must be safe to double */
	Assert(minContextSize == 0 || (minContextSize == MAXALIGN(minContextSize) && minContextSize >= 1024 && minContextSize <= maxBlockSize));
	Assert(maxBlockSize <= MEMORYCHUNK_MAX_BLOCKOFFSET);

#if 0
	/*
	 * Check whether the parameters match either available freelist.  We do
	 * not need to demand a match of maxBlockSize.
	 */
	if (minContextSize == ALLOCSET_DEFAULT_MINSIZE && initBlockSize == ALLOCSET_DEFAULT_INITSIZE)
		freeListIndex = 0;
	else if (minContextSize == ALLOCSET_SMALL_MINSIZE && initBlockSize == ALLOCSET_SMALL_INITSIZE)
		freeListIndex = 1;
	else
		freeListIndex = -1;
#endif

	freeListIndex = -1; /* skip the memory pool freelist to be sure we are thread - safe */

#if 0
	/*
	 * If a suitable freelist entry exists, just recycle that context.
	 */
	if (freeListIndex >= 0)
	{
		AllocSetFreeList* freelist = &context_freelists[freeListIndex];

		if (freelist->first_free != NULL)
		{
			/* Remove entry from freelist */
			set = freelist->first_free;
			freelist->first_free = (AllocSet)set->header.nextchild;
			freelist->num_free--;

			/* Update its maxBlockSize; everything else should be OK */
			set->maxBlockSize = maxBlockSize;

			/* Reinitialize its header, installing correct name and parent */
			MemoryContextCreate((MemoryContext)set,
				T_AllocSetContext,
				MCTX_ASET_ID,
				parent,
				name);

			((MemoryContext)set)->mem_allocated = set->keeper->endptr - ((char*)set);

			return (MemoryContext)set;
		}
	}
#endif
	/* Determine size of initial block */
	firstBlockSize = MAXALIGN(sizeof(AllocSetContext)) + ALLOC_BLOCKHDRSZ + ALLOC_CHUNKHDRSZ;
	if (minContextSize != 0)
		firstBlockSize = WTMax(firstBlockSize, minContextSize);
	else
		firstBlockSize = WTMax(firstBlockSize, initBlockSize);

	/*
	 * Allocate the initial block.  Unlike other aset.c blocks, it starts with
	 * the context header and its block header follows that.
	 */
	set = (AllocSet)malloc(firstBlockSize);
	if (set == NULL)
	{
#if 0
		if (TopMemoryContext)
			MemoryContextStats(TopMemoryContext);
		ereport(ERROR,
			(errcode(ERRCODE_OUT_OF_MEMORY),
				errmsg("out of memory"),
				errdetail("Failed while creating memory context \"%s\".",
					name)));
#endif 
		return NULL;
	}

	/*
	 * Avoid writing code that can fail between here and MemoryContextCreate;
	 * we'd leak the header/initial block if we ereport in this stretch.
	 */

	/* Fill in the initial block's block header */
	block = (AllocBlock)(((char*)set) + MAXALIGN(sizeof(AllocSetContext)));
	block->aset = set;
	block->freeptr = ((char*)block) + ALLOC_BLOCKHDRSZ;
	block->endptr = ((char*)set) + firstBlockSize;
	block->prev = NULL;
	block->next = NULL;

	/* Mark unallocated space NOACCESS; leave the block header alone. */
	VALGRIND_MAKE_MEM_NOACCESS(block->freeptr, block->endptr - block->freeptr);

	/* Remember block as part of block list */
	set->blocks = block;
	/* Mark block as not to be released at reset time */
	set->keeper = block;

	/* Finish filling in aset-specific parts of the context header */
	MemSetAligned(set->freelist, 0, sizeof(set->freelist));

	set->initBlockSize = initBlockSize;
	set->maxBlockSize = maxBlockSize;
	set->nextBlockSize = initBlockSize;
	set->freeListIndex = freeListIndex;

	/*
	 * Compute the allocation chunk size limit for this context.  It can't be
	 * more than ALLOC_CHUNK_LIMIT because of the fixed number of freelists.
	 * If maxBlockSize is small then requests exceeding the maxBlockSize, or
	 * even a significant fraction of it, should be treated as large chunks
	 * too.  For the typical case of maxBlockSize a power of 2, the chunk size
	 * limit will be at most 1/8th maxBlockSize, so that given a stream of
	 * requests that are all the maximum chunk size we will waste at most
	 * 1/8th of the allocated space.
	 *
	 * Also, allocChunkLimit must not exceed ALLOCSET_SEPARATE_THRESHOLD.
	 */
	StaticAssertStmt(ALLOC_CHUNK_LIMIT == ALLOCSET_SEPARATE_THRESHOLD,	"ALLOC_CHUNK_LIMIT != ALLOCSET_SEPARATE_THRESHOLD");

	/*
	 * Determine the maximum size that a chunk can be before we allocate an
	 * entire AllocBlock dedicated for that chunk.  We set the absolute limit
	 * of that size as ALLOC_CHUNK_LIMIT but we reduce it further so that we
	 * can fit about ALLOC_CHUNK_FRACTION chunks this size on a maximally
	 * sized block.  (We opt to keep allocChunkLimit a power-of-2 value
	 * primarily for legacy reasons rather than calculating it so that exactly
	 * ALLOC_CHUNK_FRACTION chunks fit on a maximally sized block.)
	 */
	set->allocChunkLimit = ALLOC_CHUNK_LIMIT;
	while ((Size)(set->allocChunkLimit + ALLOC_CHUNKHDRSZ) >
		(Size)((maxBlockSize - ALLOC_BLOCKHDRSZ) / ALLOC_CHUNK_FRACTION))
		set->allocChunkLimit >>= 1;

	/* Finally, do the type-independent part of context creation */
	MemoryContextCreate((MemoryContext)set,
		T_AllocSetContext,
		MCTX_ASET_ID,
		parent,
		name);

	((MemoryContext)set)->mem_allocated = firstBlockSize;

	return (MemoryContext)set;
}

MemoryContext AllocSetContextCreateInternal2(MemoryContext parent, const char* name, Size minContextSize, Size initBlockSize, Size maxBlockSize)
{
	MemoryContext cxt = AllocSetContextCreateInternal((MemoryContext)parent, name, minContextSize, initBlockSize, maxBlockSize);
	return (MemoryPoolContext)cxt;
}

/*
 * MemoryContextAllowInCriticalSection
 *		Allow/disallow allocations in this memory context within a critical
 *		section.
 *
 * Normally, memory allocations are not allowed within a critical section,
 * because a failure would lead to PANIC.  There are a few exceptions to
 * that, like allocations related to debugging code that is not supposed to
 * be enabled in production.  This function can be used to exempt specific
 * memory contexts from the assertion in palloc().
 */
void MemoryContextAllowInCriticalSection(MemoryContext context, bool allow)
{
	Assert(MemoryContextIsValid(context));

	context->allowInCritSection = allow;
}


void* wt_palloc(MemoryPoolContext cxt, size_t size)
{
	/* duplicates MemoryContextAlloc to avoid increased overhead */
	void* ret = NULL;

	if (cxt)
	{
		MemoryContext context = (MemoryContext)cxt;

		Assert(MemoryContextIsValid(context));
		//AssertNotInCriticalSection(context);

		if (!AllocSizeIsValid(size))
		{
			// elog(ERROR, "invalid memory alloc request size %zu", size);
			return NULL;
		}

		context->isReset = false;

		ret = context->methods->alloc(context, size);

		if (unlikely(ret == NULL))
		{
#if 0
			MemoryContextStats(TopMemoryContext);
			ereport(ERROR,
				(errcode(ERRCODE_OUT_OF_MEMORY),
					errmsg("out of memory"),
					errdetail("Failed on request of size %zu in memory context \"%s\".",
						size, context->name)));
#endif 
			return NULL;
		}

		VALGRIND_MEMPOOL_ALLOC(context, ret, size);
	}

	return ret;
}

void* wt_palloc0(MemoryPoolContext cxt, size_t size)
{
	void* ret = NULL;

	if (cxt)
	{
		MemoryContext context = (MemoryContext)cxt;
		Assert(MemoryContextIsValid(context));
		//AssertNotInCriticalSection(context);

		if (!AllocSizeIsValid(size))
		{
			// elog(ERROR, "invalid memory alloc request size %zu", size);
			return NULL;
		}

		context->isReset = false;

		ret = context->methods->alloc(context, size);

		if (unlikely(ret == NULL))
		{
#if 0
			MemoryContextStats(TopMemoryContext);
			ereport(ERROR,
				(errcode(ERRCODE_OUT_OF_MEMORY),
					errmsg("out of memory"),
					errdetail("Failed on request of size %zu in memory context \"%s\".",
						size, context->name)));
#endif
			return NULL;
		}
		VALGRIND_MEMPOOL_ALLOC(context, ret, size);
		MemSetAligned(ret, 0, size);
	}

	return ret;
}

/*
 * GetMemoryChunkMethodID
 *		Return the MemoryContextMethodID from the uint64 chunk header which
 *		directly precedes 'pointer'.
 */
static inline MemoryContextMethodID GetMemoryChunkMethodID(const void* pointer)
{
	uint64		header;
	/*
	 * Try to detect bogus pointers handed to us, poorly though we can.
	 * Presumably, a pointer that isn't MAXALIGNED isn't pointing at an
	 * allocated chunk.
	 */
	Assert(pointer == (const void*)MAXALIGN(pointer));

	/* Allow access to the uint64 header */
	VALGRIND_MAKE_MEM_DEFINED((char*)pointer - sizeof(uint64), sizeof(uint64));

	header = *((const uint64*)((const char*)pointer - sizeof(uint64)));

	/* Disallow access to the uint64 header */
	VALGRIND_MAKE_MEM_NOACCESS((char*)pointer - sizeof(uint64), sizeof(uint64));

	return (MemoryContextMethodID)(header & MEMORY_CONTEXT_METHODID_MASK);
}

/*
 * Call the given function in the MemoryContextMethods for the memory context
 * type that 'pointer' belongs to.
 */
#define MCXT_METHOD(pointer, method) \
	mcxt_methods[GetMemoryChunkMethodID(pointer)].method

/*
 * wt_pfree
 *		Release an allocated chunk.
 */
void wt_pfree(void* pointer)
{
#ifdef USE_VALGRIND
	MemoryContextMethodID method = GetMemoryChunkMethodID(pointer);
	MemoryContext context = GetMemoryChunkContext(pointer);
#endif
	if (pointer)
	{
		MCXT_METHOD(pointer, free_p) (pointer);
	}

#ifdef USE_VALGRIND
	if (method != MCTX_ALIGNED_REDIRECT_ID)
		VALGRIND_MEMPOOL_FREE(context, pointer);
#endif
}

#if 0
/*
 * wt_repalloc
 *		Adjust the size of a previously allocated chunk.
 */
void* wt_repalloc(void* pointer, Size size)
{
#ifdef USE_VALGRIND
	MemoryContextMethodID method = GetMemoryChunkMethodID(pointer);
#endif
#if defined(USE_ASSERT_CHECKING) || defined(USE_VALGRIND)
	MemoryContext context = GetMemoryChunkContext(pointer);
#endif
	void* ret;

	if (!AllocSizeIsValid(size))
	{
		// elog(ERROR, "invalid memory alloc request size %zu", size);
		return NULL;
	}

	// AssertNotInCriticalSection(context);
	/* isReset must be false already */
	Assert(!context->isReset);

	ret = MCXT_METHOD(pointer, realloc) (pointer, size);
	if (unlikely(ret == NULL))
	{
#if 0
		MemoryContext cxt = GetMemoryChunkContext(pointer);

		MemoryContextStats(TopMemoryContext);
		ereport(ERROR,
			(errcode(ERRCODE_OUT_OF_MEMORY),
				errmsg("out of memory"),
				errdetail("Failed on request of size %zu in memory context \"%s\".",
					size, cxt->name)));
#endif
		return NULL;
	}

#ifdef USE_VALGRIND
	if (method != MCTX_ALIGNED_REDIRECT_ID)
		VALGRIND_MEMPOOL_CHANGE(context, pointer, ret, size);
#endif

	return ret;
}
#endif

MemoryPoolContext wt_mempool_create(const char* mempool_name, unsigned int minContextSize, unsigned int initBlockSize, unsigned int maxBlockSize)
{
	MemoryContext cxt;

	if (0 == initBlockSize)
		initBlockSize = ALLOCSET_DEFAULT_INITSIZE;
	if (0 == maxBlockSize)
		maxBlockSize = ALLOCSET_DEFAULT_MAXSIZE;

	cxt = AllocSetContextCreateInternal(NULL, mempool_name, minContextSize, initBlockSize, maxBlockSize);

	return (MemoryPoolContext)cxt;
}

void wt_mempool_destroy(MemoryPoolContext cxt)
{
	if (cxt)
	{
		MemoryContext context = (MemoryContext)cxt;
		context->methods->delete_context(context);
	}
}

void wt_mempool_reset(MemoryPoolContext cxt)
{
	if (cxt)
	{
		MemoryContext context = (MemoryContext)cxt;
		/* save a function call if no pallocs since startup or last reset */
		if (!context->isReset)
		{
			context->methods->reset(context);
			context->isReset = true;
			VALGRIND_DESTROY_MEMPOOL(context);
			VALGRIND_CREATE_MEMPOOL(context, 0, false);
		}
	}
}

/*
 * MemoryContextAlloc
 *		Allocate space within the specified context.
 *
 * This could be turned into a macro, but we'd have to import
 * nodes/memnodes.h into postgres.h which seems a bad idea.
 */
void* MemoryContextAlloc(MemoryContext context, Size size)
{
	void* ret;

	Assert(MemoryContextIsValid(context));
	//AssertNotInCriticalSection(context);

	if (!AllocSizeIsValid(size))
		return NULL;
		//elog(ERROR, "invalid memory alloc request size %zu", size);

	context->isReset = false;

	ret = context->methods->alloc(context, size);
	if (unlikely(ret == NULL))
	{
#if 0
		MemoryContextStats(TopMemoryContext);

		/*
		 * Here, and elsewhere in this module, we show the target context's
		 * "name" but not its "ident" (if any) in user-visible error messages.
		 * The "ident" string might contain security-sensitive data, such as
		 * values in SQL commands.
		 */
		ereport(ERROR,
			(errcode(ERRCODE_OUT_OF_MEMORY),
				errmsg("out of memory"),
				errdetail("Failed on request of size %zu in memory context \"%s\".",
					size, context->name)));
#endif 
		return NULL;
	}

	VALGRIND_MEMPOOL_ALLOC(context, ret, size);

	return ret;
}

/*
 * MemoryContextStrdup
 *		Like strdup(), but allocate from the specified context
 */
static char* MemoryContextStrdup(MemoryContext context, const char* string)
{
	char* nstr;
	Size  len = strlen(string) + 1;

	nstr = (char*)MemoryContextAlloc(context, len);

	if(nstr)
		memcpy(nstr, string, len);

	return nstr;
}

char* wt_pstrdup(MemoryPoolContext cxt, const char* in)
{
	char* ret = NULL;

	if(cxt)
		ret = MemoryContextStrdup((MemoryContext)cxt, in);

	return ret;
}

/*
 * MemoryContextAllocExtended
 *		Allocate space within the specified context using the given flags.
 */
void* MemoryContextAllocExtended(MemoryContext ctx, Size size, int flags)
{
	void* ret;

	MemoryContext context = (MemoryContext)ctx;

	Assert(MemoryContextIsValid(context));
	//AssertNotInCriticalSection(context);

	if (!((flags & MCXT_ALLOC_HUGE) != 0 ? AllocHugeSizeIsValid(size) : AllocSizeIsValid(size)))
	{
		//elog(ERROR, "invalid memory alloc request size %zu", size);
		return NULL;
	}

	context->isReset = false;

	ret = context->methods->alloc(context, size);
	if (unlikely(ret == NULL))
	{
#if 0
		if ((flags & MCXT_ALLOC_NO_OOM) == 0)
		{
			MemoryContextStats(TopMemoryContext);
			ereport(ERROR,
				(errcode(ERRCODE_OUT_OF_MEMORY),
					errmsg("out of memory"),
					errdetail("Failed on request of size %zu in memory context \"%s\".",
						size, context->name)));
		}
#endif
		return NULL;
	}

	VALGRIND_MEMPOOL_ALLOC(context, ret, size);

	if ((flags & MCXT_ALLOC_ZERO) != 0)
		MemSetAligned(ret, 0, size);

	return ret;
}

/*
 * MemoryContextDeleteChildren
 *		Delete all the descendants of the named context and release all
 *		space allocated therein.  The named context itself is not touched.
 */
void MemoryContextDeleteChildren(MemoryContext context)
{
	Assert(MemoryContextIsValid(context));

	/*
	 * MemoryContextDelete will delink the child from me, so just iterate as
	 * long as there is a child.
	 */
	while (context->firstchild != NULL)
		MemoryContextDelete(context->firstchild);
}

/*
 * MemoryContextSetParent
 *		Change a context to belong to a new parent (or no parent).
 *
 * We provide this as an API function because it is sometimes useful to
 * change a context's lifespan after creation.  For example, a context
 * might be created underneath a transient context, filled with data,
 * and then reparented underneath CacheMemoryContext to make it long-lived.
 * In this way no special effort is needed to get rid of the context in case
 * a failure occurs before its contents are completely set up.
 *
 * Callers often assume that this function cannot fail, so don't put any
 * elog(ERROR) calls in it.
 *
 * A possible caller error is to reparent a context under itself, creating
 * a loop in the context graph.  We assert here that context != new_parent,
 * but checking for multi-level loops seems more trouble than it's worth.
 */
void MemoryContextSetParent(MemoryContext context, MemoryContext new_parent)
{
	Assert(MemoryContextIsValid(context));
	Assert(context != new_parent);

	/* Fast path if it's got correct parent already */
	if (new_parent == context->parent)
		return;

	/* Delink from existing parent, if any */
	if (context->parent)
	{
		MemoryContext parent = context->parent;

		if (context->prevchild != NULL)
			context->prevchild->nextchild = context->nextchild;
		else
		{
			Assert(parent->firstchild == context);
			parent->firstchild = context->nextchild;
		}

		if (context->nextchild != NULL)
			context->nextchild->prevchild = context->prevchild;
	}

	/* And relink */
	if (new_parent)
	{
		Assert(MemoryContextIsValid(new_parent));
		context->parent = new_parent;
		context->prevchild = NULL;
		context->nextchild = new_parent->firstchild;
		if (new_parent->firstchild != NULL)
			new_parent->firstchild->prevchild = context;
		new_parent->firstchild = context;
	}
	else
	{
		context->parent = NULL;
		context->prevchild = NULL;
		context->nextchild = NULL;
	}
}

/*
 * MemoryContextDelete
 *		Delete a context and its descendants, and release all space
 *		allocated therein.
 *
 * The type-specific delete routine removes all storage for the context,
 * but we have to recurse to handle the children.
 * We must also delink the context from its parent, if it has one.
 */
void MemoryContextDelete(MemoryContext cxt)
{
	MemoryContext context = (MemoryContext)cxt;

	if (context)
	{
		Assert(MemoryContextIsValid(context));
		/* We had better not be deleting TopMemoryContext ... */
		Assert(context != TopMemoryContext);
		/* And not CurrentMemoryContext, either */
		Assert(context != CurrentMemoryContext);

		/* save a function call in common case where there are no children */
		if (context->firstchild != NULL)
			MemoryContextDeleteChildren(context);

		/*
		 * It's not entirely clear whether 'tis better to do this before or after
		 * delinking the context; but an error in a callback will likely result in
		 * leaking the whole context (if it's not a root context) if we do it
		 * after, so let's do it before.
		 */
		MemoryContextCallResetCallbacks(context);

		/*
		 * We delink the context from its parent before deleting it, so that if
		 * there's an error we won't have deleted/busted contexts still attached
		 * to the context tree.  Better a leak than a crash.
		 */
		MemoryContextSetParent(context, NULL);

		/*
		 * Also reset the context's ident pointer, in case it points into the
		 * context.  This would only matter if someone tries to get stats on the
		 * (already unlinked) context, which is unlikely, but let's be safe.
		 */
		context->ident = NULL;

		context->methods->delete_context(context);

		VALGRIND_DESTROY_MEMPOOL(context);
	}
}


