/*-------------------------------------------------------------------------
 *
 * dynahash.c
 *	  dynamic chained hash tables
 *
 * dynahash.c supports both local-to-a-backend hash tables and hash tables in
 * shared memory.  For shared hash tables, it is the caller's responsibility
 * to provide appropriate access interlocking.  The simplest convention is
 * that a single LWLock protects the whole hash table.  Searches (HASH_FIND or
 * hash_seq_search) need only shared lock, but any update requires exclusive
 * lock.  For heavily-used shared tables, the single-lock approach creates a
 * concurrency bottleneck, so we also support "partitioned" locking wherein
 * there are multiple LWLocks guarding distinct subsets of the table.  To use
 * a hash table in partitioned mode, the HASH_PARTITION flag must be given
 * to hash_create.  This prevents any attempt to split buckets on-the-fly.
 * Therefore, each hash bucket chain operates independently, and no fields
 * of the hash header change after init except nentries and freeList.
 * (A partitioned table uses multiple copies of those fields, guarded by
 * spinlocks, for additional concurrency.)
 * This lets any subset of the hash buckets be treated as a separately
 * lockable partition.  We expect callers to use the low-order bits of a
 * lookup key's hash value as a partition number --- this will work because
 * of the way calc_bucket() maps hash values to bucket numbers.
 *
 * For hash tables in shared memory, the memory allocator function should
 * match malloc's semantics of returning NULL on failure.  For hash tables
 * in local memory, we typically use palloc() which will throw error on
 * failure.  The code in this file has to cope with both cases.
 *
 * dynahash.c provides support for these types of lookup keys:
 *
 * 1. Null-terminated C strings (truncated if necessary to fit in keysize),
 * compared as though by strcmp().  This is selected by specifying the
 * HASH_STRINGS flag to hash_create.
 *
 * 2. Arbitrary binary data of size keysize, compared as though by memcmp().
 * (Caller must ensure there are no undefined padding bits in the keys!)
 * This is selected by specifying the HASH_BLOBS flag to hash_create.
 *
 * 3. More complex key behavior can be selected by specifying user-supplied
 * hashing, comparison, and/or key-copying functions.  At least a hashing
 * function must be supplied; comparison defaults to memcmp() and key copying
 * to memcpy() when a user-defined hashing function is selected.
 *
 * Compared to simplehash, dynahash has the following benefits:
 *
 * - It supports partitioning, which is useful for shared memory access using
 *   locks.
 * - Shared memory hashes are allocated in a fixed size area at startup and
 *   are discoverable by name from other processes.
 * - Because entries don't need to be moved in the case of hash conflicts,
 *   dynahash has better performance for large entries.
 * - Guarantees stable pointers to entries.
 *
 * Portions Copyright (c) 1996-2023, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/utils/hash/dynahash.c
 *
 *-------------------------------------------------------------------------
 */

 /*
  * Original comments:
  *
  * Dynamic hashing, after CACM April 1988 pp 446-457, by Per-Ake Larson.
  * Coded into C, with minor code improvements, and with hsearch(3) interface,
  * by ejp@ausmelb.oz, Jul 26, 1988: 13:16;
  * also, hcreate/hdestroy routines added to simulate hsearch(3).
  *
  * These routines simulate hsearch(3) and family, with the important
  * difference that the hash table is dynamic - can grow indefinitely
  * beyond its original size (as supplied to hcreate()).
  *
  * Performance appears to be comparable to that of hsearch(3).
  * The 'source-code' options referred to in hsearch(3)'s 'man' page
  * are not implemented; otherwise functionality is identical.
  *
  * Compilation controls:
  * HASH_DEBUG controls some informative traces, mainly for debugging.
  * HASH_STATISTICS causes HashAccesses and HashCollisions to be maintained;
  * when combined with HASH_DEBUG, these are displayed by hdestroy().
  *
  * Problems & fixes to ejp@ausmelb.oz. WARNING: relies on pre-processor
  * concatenation property, in probably unnecessary code 'optimization'.
  *
  * Modified margo@postgres.berkeley.edu February 1990
  *		added multiple table interface
  * Modified by sullivan@postgres.berkeley.edu April 1990
  *		changed ctl structure for shared memory
  */

#include <string.h>

#include "wt_mempool.h"
#include "wt_hash.h"

  /*
   * Constants
   *
   * A hash table has a top-level "directory", each of whose entries points
   * to a "segment" of ssize bucket headers.  The maximum number of hash
   * buckets is thus dsize * ssize (but dsize may be expansible).  Of course,
   * the number of records in the table can be larger, but we don't want a
   * whole lot of records per bucket or performance goes down.
   *
   * In a hash table allocated in shared memory, the directory cannot be
   * expanded because it must stay at a fixed address.  The directory size
   * should be selected using hash_select_dirsize (and you'd better have
   * a good idea of the maximum number of entries!).  For non-shared hash
   * tables, the initial directory size can be left at the default.
   */
#define DEF_SEGSIZE			   256
#define DEF_SEGSIZE_SHIFT	   8	/* must be log2(DEF_SEGSIZE) */
#define DEF_DIRSIZE			   256

/* Number of freelists to be used for a partitioned hash table. */
#define NUM_FREELISTS			32

/*
 * Private function prototypes
 */
static HASHSEGMENT seg_alloc(HTAB* hashp);
static bool element_alloc(HTAB* hashp, int nelem, int freelist_idx);
static bool dir_realloc(HTAB* hashp);
static bool expand_table(HTAB* hashp);
static HASHBUCKET get_hash_entry(HTAB* hashp, int freelist_idx);
static void hdefault(HTAB* hashp);
static int	choose_nelem_alloc(Size entrysize);
static bool init_htab(HTAB* hashp, long nelem);
//static void hash_corrupted(HTAB* hashp);
//static long next_pow2_long(long num);
static int	next_pow2_int(long num);
//static void register_seq_scan(HTAB* hashp);
//static void deregister_seq_scan(HTAB* hashp);
static bool has_seq_scans(HTAB* hashp);

/*
 * This hash function was written by Bob Jenkins
 * (bob_jenkins@burtleburtle.net), and superficially adapted
 * for PostgreSQL by Neil Conway. For more information on this
 * hash function, see http://burtleburtle.net/bob/hash/doobs.html,
 * or Bob's article in Dr. Dobb's Journal, Sept. 1997.
 *
 * In the current code, we have adopted Bob's 2006 update of his hash
 * function to fetch the data a word at a time when it is suitably aligned.
 * This makes for a useful speedup, at the cost of having to maintain
 * four code paths (aligned vs unaligned, and little-endian vs big-endian).
 * It also uses two separate mixing functions mix() and final(), instead
 * of a slower multi-purpose function.
 */

 /* Get a bit mask of the bits set in non-uint32 aligned addresses */
#define UINT32_ALIGN_MASK (sizeof(uint32) - 1)

/*
 * Key (also entry) part of a HASHELEMENT
 */
#define ELEMENTKEY(helem)  (((char *)(helem)) + MAXALIGN(sizeof(HASHELEMENT)))

 /*
  * Obtain element pointer given pointer to key
  */
#define ELEMENT_FROM_KEY(key)  \
	((HASHELEMENT *) (((char *) (key)) - MAXALIGN(sizeof(HASHELEMENT))))

/*
 * Fast MOD arithmetic, assuming that y is a power of 2 !
 */
#define MOD(x,y)			   ((x) & ((y)-1))

static inline uint32 pg_rotate_left32(uint32 word, int n)
{
	return (word << n) | (word >> (32 - n));
}

#define rot(x,k) pg_rotate_left32(x, k)

/*----------
 * mix -- mix 3 32-bit values reversibly.
 *
 * This is reversible, so any information in (a,b,c) before mix() is
 * still in (a,b,c) after mix().
 *
 * If four pairs of (a,b,c) inputs are run through mix(), or through
 * mix() in reverse, there are at least 32 bits of the output that
 * are sometimes the same for one pair and different for another pair.
 * This was tested for:
 * * pairs that differed by one bit, by two bits, in any combination
 *	 of top bits of (a,b,c), or in any combination of bottom bits of
 *	 (a,b,c).
 * * "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
 *	 the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
 *	 is commonly produced by subtraction) look like a single 1-bit
 *	 difference.
 * * the base values were pseudorandom, all zero but one bit set, or
 *	 all zero plus a counter that starts at zero.
 *
 * This does not achieve avalanche.  There are input bits of (a,b,c)
 * that fail to affect some output bits of (a,b,c), especially of a.  The
 * most thoroughly mixed value is c, but it doesn't really even achieve
 * avalanche in c.
 *
 * This allows some parallelism.  Read-after-writes are good at doubling
 * the number of bits affected, so the goal of mixing pulls in the opposite
 * direction from the goal of parallelism.  I did what I could.  Rotates
 * seem to cost as much as shifts on every machine I could lay my hands on,
 * and rotates are much kinder to the top and bottom bits, so I used rotates.
 *----------
 */
#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);	c += b; \
  b -= a;  b ^= rot(a, 6);	a += c; \
  c -= b;  c ^= rot(b, 8);	b += a; \
  a -= c;  a ^= rot(c,16);	c += b; \
  b -= a;  b ^= rot(a,19);	a += c; \
  c -= b;  c ^= rot(b, 4);	b += a; \
}

 /*----------
  * final -- final mixing of 3 32-bit values (a,b,c) into c
  *
  * Pairs of (a,b,c) values differing in only a few bits will usually
  * produce values of c that look totally different.  This was tested for
  * * pairs that differed by one bit, by two bits, in any combination
  *	 of top bits of (a,b,c), or in any combination of bottom bits of
  *	 (a,b,c).
  * * "differ" is defined as +, -, ^, or ~^.  For + and -, I transformed
  *	 the output delta to a Gray code (a^(a>>1)) so a string of 1's (as
  *	 is commonly produced by subtraction) look like a single 1-bit
  *	 difference.
  * * the base values were pseudorandom, all zero but one bit set, or
  *	 all zero plus a counter that starts at zero.
  *
  * The use of separate functions for mix() and final() allow for a
  * substantial performance increase since final() does not need to
  * do well in reverse, but is does need to affect all output bits.
  * mix(), on the other hand, does not need to affect all output
  * bits (affecting 32 bits is enough).  The original hash function had
  * a single mixing operation that had to satisfy both sets of requirements
  * and was slower as a result.
  *----------
  */
#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c, 4); \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

/*
 * hash_bytes() -- hash a variable-length key into a 32-bit value
 *		k		: the key (the unaligned variable-length array of bytes)
 *		len		: the length of the key, counting by bytes
 *
 * Returns a uint32 value.  Every bit of the key affects every bit of
 * the return value.  Every 1-bit and 2-bit delta achieves avalanche.
 * About 6*len+35 instructions. The best hash table sizes are powers
 * of 2.  There is no need to do mod a prime (mod is sooo slow!).
 * If you need less than 32 bits, use a bitmask.
 *
 * This procedure must never throw elog(ERROR); the ResourceOwner code
 * relies on this not to fail.
 *
 * Note: we could easily change this function to return a 64-bit hash value
 * by using the final values of both b and c.  b is perhaps a little less
 * well mixed than c, however.
 */
//uint32 hash_bytes(const unsigned char* k, int keylen)
uint32 hash_bytes(const void* key, Size keysize)
{
	uint32	a, b, c, len;
	int keylen;
	const unsigned char* k = (const unsigned char*)key;

	keylen = (int)keysize;

	/* Set up the internal state */
	len = keylen;
	a = b = c = 0x9e3779b9 + len + 3923095;

	/* If the source pointer is word-aligned, we use word-wide fetches */
	if (((uintptr_t)k & UINT32_ALIGN_MASK) == 0)
	{
		/* Code path for aligned source data */
		const uint32* ka = (const uint32*)k;

		/* handle most of the key */
		while (len >= 12)
		{
			a += ka[0];
			b += ka[1];
			c += ka[2];
			mix(a, b, c);
			ka += 3;
			len -= 12;
		}

		/* handle the last 11 bytes */
		k = (const unsigned char*)ka;
#ifdef WORDS_BIGENDIAN
		switch (len)
		{
		case 11:
			c += ((uint32)k[10] << 8);
			/* fall through */
		case 10:
			c += ((uint32)k[9] << 16);
			/* fall through */
		case 9:
			c += ((uint32)k[8] << 24);
			/* fall through */
		case 8:
			/* the lowest byte of c is reserved for the length */
			b += ka[1];
			a += ka[0];
			break;
		case 7:
			b += ((uint32)k[6] << 8);
			/* fall through */
		case 6:
			b += ((uint32)k[5] << 16);
			/* fall through */
		case 5:
			b += ((uint32)k[4] << 24);
			/* fall through */
		case 4:
			a += ka[0];
			break;
		case 3:
			a += ((uint32)k[2] << 8);
			/* fall through */
		case 2:
			a += ((uint32)k[1] << 16);
			/* fall through */
		case 1:
			a += ((uint32)k[0] << 24);
			/* case 0: nothing left to add */
		}
#else				/* !WORDS_BIGENDIAN */
		switch (len)
		{
		case 11:
			c += ((uint32)k[10] << 24);
			/* fall through */
		case 10:
			c += ((uint32)k[9] << 16);
			/* fall through */
		case 9:
			c += ((uint32)k[8] << 8);
			/* fall through */
		case 8:
			/* the lowest byte of c is reserved for the length */
			b += ka[1];
			a += ka[0];
			break;
		case 7:
			b += ((uint32)k[6] << 16);
			/* fall through */
		case 6:
			b += ((uint32)k[5] << 8);
			/* fall through */
		case 5:
			b += k[4];
			/* fall through */
		case 4:
			a += ka[0];
			break;
		case 3:
			a += ((uint32)k[2] << 16);
			/* fall through */
		case 2:
			a += ((uint32)k[1] << 8);
			/* fall through */
		case 1:
			a += k[0];
			/* case 0: nothing left to add */
		}
#endif					/* WORDS_BIGENDIAN */
	}
	else
	{
		/* Code path for non-aligned source data */

		/* handle most of the key */
		while (len >= 12)
		{
#ifdef WORDS_BIGENDIAN
			a += (k[3] + ((uint32)k[2] << 8) + ((uint32)k[1] << 16) + ((uint32)k[0] << 24));
			b += (k[7] + ((uint32)k[6] << 8) + ((uint32)k[5] << 16) + ((uint32)k[4] << 24));
			c += (k[11] + ((uint32)k[10] << 8) + ((uint32)k[9] << 16) + ((uint32)k[8] << 24));
#else							/* !WORDS_BIGENDIAN */
			a += (k[0] + ((uint32)k[1] << 8) + ((uint32)k[2] << 16) + ((uint32)k[3] << 24));
			b += (k[4] + ((uint32)k[5] << 8) + ((uint32)k[6] << 16) + ((uint32)k[7] << 24));
			c += (k[8] + ((uint32)k[9] << 8) + ((uint32)k[10] << 16) + ((uint32)k[11] << 24));
#endif							/* WORDS_BIGENDIAN */
			mix(a, b, c);
			k += 12;
			len -= 12;
		}

		/* handle the last 11 bytes */
#ifdef WORDS_BIGENDIAN
		switch (len)
		{
		case 11:
			c += ((uint32)k[10] << 8);
			/* fall through */
		case 10:
			c += ((uint32)k[9] << 16);
			/* fall through */
		case 9:
			c += ((uint32)k[8] << 24);
			/* fall through */
		case 8:
			/* the lowest byte of c is reserved for the length */
			b += k[7];
			/* fall through */
		case 7:
			b += ((uint32)k[6] << 8);
			/* fall through */
		case 6:
			b += ((uint32)k[5] << 16);
			/* fall through */
		case 5:
			b += ((uint32)k[4] << 24);
			/* fall through */
		case 4:
			a += k[3];
			/* fall through */
		case 3:
			a += ((uint32)k[2] << 8);
			/* fall through */
		case 2:
			a += ((uint32)k[1] << 16);
			/* fall through */
		case 1:
			a += ((uint32)k[0] << 24);
			/* case 0: nothing left to add */
		}
#else							/* !WORDS_BIGENDIAN */
		switch (len)
		{
		case 11:
			c += ((uint32)k[10] << 24);
			/* fall through */
		case 10:
			c += ((uint32)k[9] << 16);
			/* fall through */
		case 9:
			c += ((uint32)k[8] << 8);
			/* fall through */
		case 8:
			/* the lowest byte of c is reserved for the length */
			b += ((uint32)k[7] << 24);
			/* fall through */
		case 7:
			b += ((uint32)k[6] << 16);
			/* fall through */
		case 6:
			b += ((uint32)k[5] << 8);
			/* fall through */
		case 5:
			b += k[4];
			/* fall through */
		case 4:
			a += ((uint32)k[3] << 24);
			/* fall through */
		case 3:
			a += ((uint32)k[2] << 16);
			/* fall through */
		case 2:
			a += ((uint32)k[1] << 8);
			/* fall through */
		case 1:
			a += k[0];
			/* case 0: nothing left to add */
		}
#endif							/* WORDS_BIGENDIAN */
	}

	final(a, b, c);

	/* report the result */
	return c;
}

/*
 * pg_leftmost_one_pos64
 *		As above, but for a 64-bit word.
 */
static inline int pg_leftmost_one_pos64(uint64 word)
{
#ifdef HAVE__BUILTIN_CLZ
	Assert(word != 0);

#if defined(HAVE_LONG_INT_64)
	return 63 - __builtin_clzl(word);
#elif defined(HAVE_LONG_LONG_INT_64)
	return 63 - __builtin_clzll(word);
#else
#error must have a working 64-bit integer datatype
#endif							/* HAVE_LONG_INT_64 */

#elif defined(_MSC_VER) && (defined(_M_AMD64) || defined(_M_ARM64))
	unsigned long result;
	bool		non_zero;

	non_zero = _BitScanReverse64(&result, word);
	Assert(non_zero);
	return (int)result;
#else
	int			shift = 64 - 8;

	Assert(word != 0);

	while ((word >> shift) == 0)
		shift -= 8;

	return shift + pg_leftmost_one_pos[(word >> shift) & 255];
#endif							/* HAVE__BUILTIN_CLZ */
}

/*
 * pg_ceil_log2_64
 *		Returns equivalent of ceil(log2(num))
 */
static inline uint64 pg_ceil_log2_64(uint64 num)
{
	if (num < 2)
		return 0;
	else
		return pg_leftmost_one_pos64(num - 1) + 1;
}

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG 8

/* calculate ceil(log base 2) of num */
int my_log2(long num)
{
#if 0
	/*
	 * guard against too-large input, which would be invalid for
	 * pg_ceil_log2_*()
	 */
	if (num > LONG_MAX / 2)
		num = LONG_MAX / 2;
#endif
#if SIZEOF_LONG < 8
	return pg_ceil_log2_32(num);
#else
	return pg_ceil_log2_64(num);
#endif
}

/* max_dsize value to indicate expansible directory */
#define NO_MAX_DSIZE			(-1)

#define IS_PARTITIONED(hctl)  ((hctl)->num_partitions != 0)

/*
 * Set default HASHHDR parameters.
 */
static void hdefault(HTAB* hashp)
{
	HASHHDR* hctl = hashp->hctl;

	MemSet(hctl, 0, sizeof(HASHHDR));

	hctl->dsize = DEF_DIRSIZE;
	hctl->nsegs = 0;

	hctl->num_partitions = 0;	/* not partitioned */

	/* table has no fixed maximum size */
	hctl->max_dsize = NO_MAX_DSIZE;

	hctl->ssize = DEF_SEGSIZE;
	hctl->sshift = DEF_SEGSIZE_SHIFT;

#ifdef HASH_STATISTICS
	hctl->accesses = hctl->collisions = 0;
#endif
}

/* calculate first power of 2 >= num, bounded to what will fit in an int */
static int next_pow2_int(long num)
{
#if 0
	if (num > INT_MAX / 2)
		num = INT_MAX / 2;
#endif
	return 1 << my_log2(num);
}

/*
 * get_hash_value -- exported routine to calculate a key's hash value
 *
 * We export this because for partitioned tables, callers need to compute
 * the partition number (from the low-order bits of the hash value) before
 * searching.
 */
uint32 get_hash_value(HTAB* hashp, const void* keyPtr)
{
	return hashp->hash(keyPtr, hashp->keysize);
}

/* Convert a hash value to a bucket number */
static inline uint32 calc_bucket(HASHHDR* hctl, uint32 hash_val)
{
	uint32		bucket;

	bucket = hash_val & hctl->high_mask;
	if (bucket > hctl->max_bucket)
		bucket = bucket & hctl->low_mask;

	return bucket;
}


static HASHSEGMENT seg_alloc(HTAB* hashp)
{
	HASHSEGMENT segp;

	segp = (HASHSEGMENT)hashp->alloc(hashp->hcxt, sizeof(HASHBUCKET) * hashp->ssize);

	if (!segp)
		return NULL;

	MemSet(segp, 0, sizeof(HASHBUCKET) * hashp->ssize);

	return segp;
}

/*
 * Compute derived fields of hctl and build the initial directory/segment
 * arrays
 */
static bool init_htab(HTAB* hashp, long nelem)
{
	HASHHDR* hctl = hashp->hctl;
	HASHSEGMENT* segp;
	int			nbuckets;
	int			nsegs;
	int			i;

#if 0
	/*
	 * initialize mutexes if it's a partitioned table
	 */
	if (IS_PARTITIONED(hctl))
		for (i = 0; i < NUM_FREELISTS; i++)
			SpinLockInit(&(hctl->freeList[i].mutex));
#endif
	/*
	 * Allocate space for the next greater power of two number of buckets,
	 * assuming a desired maximum load factor of 1.
	 */
	nbuckets = next_pow2_int(nelem);

	/*
	 * In a partitioned table, nbuckets must be at least equal to
	 * num_partitions; were it less, keys with apparently different partition
	 * numbers would map to the same bucket, breaking partition independence.
	 * (Normally nbuckets will be much bigger; this is just a safety check.)
	 */
	while (nbuckets < hctl->num_partitions)
		nbuckets <<= 1;

	hctl->max_bucket = hctl->low_mask = nbuckets - 1;
	hctl->high_mask = (nbuckets << 1) - 1;

	/*
	 * Figure number of directory segments needed, round up to a power of 2
	 */
	nsegs = (nbuckets - 1) / hctl->ssize + 1;
	nsegs = next_pow2_int(nsegs);

	/*
	 * Make sure directory is big enough. If pre-allocated directory is too
	 * small, choke (caller screwed up).
	 */
	if (nsegs > hctl->dsize)
	{
		if (!(hashp->dir))
			hctl->dsize = nsegs;
		else
			return false;
	}

	/* Allocate a directory */
	if (!(hashp->dir))
	{
		hashp->dir = (HASHSEGMENT*)hashp->alloc(hashp->hcxt, hctl->dsize * sizeof(HASHSEGMENT));
		if (!hashp->dir)
			return false;
	}

	/* Allocate initial segments */
	for (segp = hashp->dir; hctl->nsegs < nsegs; hctl->nsegs++, segp++)
	{
		*segp = seg_alloc(hashp);
		if (*segp == NULL)
			return false;
	}

	/* Choose number of entries to allocate at a time */
	hctl->nelem_alloc = choose_nelem_alloc(hctl->entrysize);

#ifdef HASH_DEBUG
	fprintf(stderr, "init_htab:\n%s%p\n%s%ld\n%s%ld\n%s%d\n%s%ld\n%s%u\n%s%x\n%s%x\n%s%ld\n",
		"TABLE POINTER   ", hashp,
		"DIRECTORY SIZE  ", hctl->dsize,
		"SEGMENT SIZE    ", hctl->ssize,
		"SEGMENT SHIFT   ", hctl->sshift,
		"MAX BUCKET      ", hctl->max_bucket,
		"HIGH MASK       ", hctl->high_mask,
		"LOW  MASK       ", hctl->low_mask,
		"NSEGS           ", hctl->nsegs);
#endif
	return true;
}

/*
 * memory allocation support
 */
static void* DynaHashAlloc(MemoryContext ctx, Size size)
{
	void* ret = NULL;
	if (ctx)
	{
		Assert(MemoryContextIsValid(ctx));

		ret = MemoryContextAllocExtended(ctx, size, MCXT_ALLOC_NO_OOM);
	}

	return ret;
}

/************************** CREATE ROUTINES **********************/

/*
 * hash_create -- create a new dynamic hash table
 *
 *	tabname: a name for the table (for debugging purposes)
 *	nelem: maximum number of elements expected
 *	*info: additional table parameters, as indicated by flags
 *	flags: bitmask indicating which parameters to take from *info
 *
 * The flags value *must* include HASH_ELEM.  (Formerly, this was nominally
 * optional, but the default keysize and entrysize values were useless.)
 * The flags value must also include exactly one of HASH_STRINGS, HASH_BLOBS,
 * or HASH_FUNCTION, to define the key hashing semantics (C strings,
 * binary blobs, or custom, respectively).  Callers specifying a custom
 * hash function will likely also want to use HASH_COMPARE, and perhaps
 * also HASH_KEYCOPY, to control key comparison and copying.
 * Another often-used flag is HASH_CONTEXT, to allocate the hash table
 * under info->hcxt rather than under TopMemoryContext; the default
 * behavior is only suitable for session-lifespan hash tables.
 * Other flags bits are special-purpose and seldom used, except for those
 * associated with shared-memory hash tables, for which see ShmemInitHash().
 *
 * Fields in *info are read only when the associated flags bit is set.
 * It is not necessary to initialize other fields of *info.
 * Neither tabname nor *info need persist after the hash_create() call.
 *
 * Note: It is deprecated for callers of hash_create() to explicitly specify
 * string_hash, tag_hash, uint32_hash, or oid_hash.  Just set HASH_STRINGS or
 * HASH_BLOBS.  Use HASH_FUNCTION only when you want something other than
 * one of these.
 *
 * Note: for a shared-memory hashtable, nelem needs to be a pretty good
 * estimate, since we can't expand the table on the fly.  But an unshared
 * hashtable can be expanded on-the-fly, so it's better for nelem to be
 * on the small side and let the table grow if it's exceeded.  An overly
 * large nelem will penalize hash_seq_search speed without buying much.
 */

HTAB* hash_create(const char* tabname, long nelem, const HASHCTL* info, int flags)
{
	HTAB* hashp;
	HASHHDR* hctl;
	MemoryContext cxt;
	/*
	 * Hash tables now allocate space for key and data, but you have to say
	 * how much space to allocate.
	 */
	Assert(flags & HASH_ELEM);
	Assert(info->keysize > 0);
	Assert(info->entrysize >= info->keysize);
#if 0
	/*
	 * For shared hash tables, we have a local hash header (HTAB struct) that
	 * we allocate in TopMemoryContext; all else is in shared memory.
	 *
	 * For non-shared hash tables, everything including the hash header is in
	 * a memory context created specially for the hash table --- this makes
	 * hash_destroy very simple.  The memory context is made a child of either
	 * a context specified by the caller, or TopMemoryContext if nothing is
	 * specified.
	 */

	if (flags & HASH_SHARED_MEM)
	{
		/* Set up to allocate the hash header */
		CurrentDynaHashCxt = TopMemoryContext;
	}
	else
	{
		/* Create the hash table's private memory context */
		if (flags & HASH_CONTEXT)
			CurrentDynaHashCxt = info->hcxt;
		else
			CurrentDynaHashCxt = TopMemoryContext;
		CurrentDynaHashCxt = AllocSetContextCreate(CurrentDynaHashCxt,
			"dynahash",
			ALLOCSET_DEFAULT_SIZES);
	}
#endif
	//cxt = AllocSetContextCreateInternal2(NULL, "dynahash", ALLOCSET_DEFAULT_SIZES);
	cxt = wt_mempool_create("dynahash", ALLOCSET_DEFAULT_SIZES);

	if (!cxt)
		return NULL;
	/* Initialize the hash header, plus a copy of the table name */
	size_t len = strlen(tabname);
	hashp = (HTAB*)DynaHashAlloc(cxt, sizeof(HTAB) + len + 1);
	MemSet(hashp, 0, sizeof(HTAB));

	hashp->tabname = (char*)(hashp + 1);
	strcpy(hashp->tabname, tabname);

	hashp->hcxt = cxt; /* save the memory conext */

#if 0
	/* If we have a private context, label it with hashtable's name */
	if (!(flags & HASH_SHARED_MEM))
		MemoryContextSetIdentifier(CurrentDynaHashCxt, hashp->tabname);
	/*
	 * Select the appropriate hash function (see comments at head of file).
	 */
	if (flags & HASH_FUNCTION)
	{
		Assert(!(flags & (HASH_BLOBS | HASH_STRINGS)));
		hashp->hash = info->hash;
	}
	else if (flags & HASH_BLOBS)
	{
		Assert(!(flags & HASH_STRINGS));
		/* We can optimize hashing for common key sizes */
		if (info->keysize == sizeof(uint32))
			hashp->hash = uint32_hash;
		else
			hashp->hash = tag_hash;
	}
	else
	{
		/*
		 * string_hash used to be considered the default hash method, and in a
		 * non-assert build it effectively still is.  But we now consider it
		 * an assertion error to not say HASH_STRINGS explicitly.  To help
		 * catch mistaken usage of HASH_STRINGS, we also insist on a
		 * reasonably long string length: if the keysize is only 4 or 8 bytes,
		 * it's almost certainly an integer or pointer not a string.
		 */
		Assert(flags & HASH_STRINGS);
		Assert(info->keysize > 8);

		hashp->hash = string_hash;
	}
#endif
	hashp->hash = hash_bytes;

#if 0
	/*
	 * If you don't specify a match function, it defaults to string_compare if
	 * you used string_hash, and to memcmp otherwise.
	 *
	 * Note: explicitly specifying string_hash is deprecated, because this
	 * might not work for callers in loadable modules on some platforms due to
	 * referencing a trampoline instead of the string_hash function proper.
	 * Specify HASH_STRINGS instead.
	 */
	if (flags & HASH_COMPARE)
		hashp->match = info->match;
	else if (hashp->hash == string_hash)
		hashp->match = (HashCompareFunc)string_compare;
	else
		hashp->match = memcmp;
#endif
	hashp->match = memcmp;
	
#if 0
	/*
	 * Similarly, the key-copying function defaults to strlcpy or memcpy.
	 */
	if (flags & HASH_KEYCOPY)
		hashp->keycopy = info->keycopy;
	else if (hashp->hash == string_hash)
	{
		/*
		 * The signature of keycopy is meant for memcpy(), which returns
		 * void*, but strlcpy() returns size_t.  Since we never use the return
		 * value of keycopy, and size_t is pretty much always the same size as
		 * void *, this should be safe.  The extra cast in the middle is to
		 * avoid warnings from -Wcast-function-type.
		 */
		hashp->keycopy = (HashCopyFunc)(pg_funcptr_t)strlcpy;
	}
	else
		hashp->keycopy = memcpy;
#endif
	hashp->keycopy = memcpy;

#if 0
	/* And select the entry allocation function, too. */
	if (flags & HASH_ALLOC)
		hashp->alloc = info->alloc;
	else
		hashp->alloc = DynaHashAlloc;
#endif
	hashp->alloc = DynaHashAlloc;

#if 0
	if (flags & HASH_SHARED_MEM)
	{
		/*
		 * ctl structure and directory are preallocated for shared memory
		 * tables.  Note that HASH_DIRSIZE and HASH_ALLOC had better be set as
		 * well.
		 */
		hashp->hctl = info->hctl;
		hashp->dir = (HASHSEGMENT*)(((char*)info->hctl) + sizeof(HASHHDR));
		hashp->hcxt = NULL;
		hashp->isshared = true;

		/* hash table already exists, we're just attaching to it */
		if (flags & HASH_ATTACH)
		{
			/* make local copies of some heavily-used values */
			hctl = hashp->hctl;
			hashp->keysize = hctl->keysize;
			hashp->ssize = hctl->ssize;
			hashp->sshift = hctl->sshift;

			return hashp;
		}
	}
	else
#endif
	{
		/* setup hash table defaults */
		hashp->hctl = NULL;
		hashp->dir = NULL;
		hashp->isshared = false;
	}

	if (!hashp->hctl)
	{
		hashp->hctl = (HASHHDR*)hashp->alloc(hashp->hcxt, sizeof(HASHHDR));
		if (!hashp->hctl)
		{
#if 0
			ereport(ERROR,
				(errcode(ERRCODE_OUT_OF_MEMORY),
					errmsg("out of memory")));
#endif
			return NULL;
		}
	}

	hashp->frozen = false;

	hdefault(hashp);

	hctl = hashp->hctl;

#if 0
	if (flags & HASH_PARTITION)
	{
		/* Doesn't make sense to partition a local hash table */
		Assert(flags & HASH_SHARED_MEM);

		/*
		 * The number of partitions had better be a power of 2. Also, it must
		 * be less than INT_MAX (see init_htab()), so call the int version of
		 * next_pow2.
		 */
		Assert(info->num_partitions == next_pow2_int(info->num_partitions));

		hctl->num_partitions = info->num_partitions;
	}
#endif
	if (flags & HASH_SEGMENT)
	{
		hctl->ssize = info->ssize;
		hctl->sshift = my_log2(info->ssize);
		/* ssize had better be a power of 2 */
		Assert(hctl->ssize == (1L << hctl->sshift));
	}

	/*
	 * SHM hash tables have fixed directory size passed by the caller.
	 */
	if (flags & HASH_DIRSIZE)
	{
		hctl->max_dsize = info->max_dsize;
		hctl->dsize = info->dsize;
	}

	/* remember the entry sizes, too */
	hctl->keysize = info->keysize;
	hctl->entrysize = info->entrysize;

	/* make local copies of heavily-used constant fields */
	hashp->keysize = hctl->keysize;
	hashp->ssize = hctl->ssize;
	hashp->sshift = hctl->sshift;

	/* Build the hash directory structure */
	if (!init_htab(hashp, nelem))
	{
		//elog(ERROR, "failed to initialize hash table \"%s\"", hashp->tabname);
		return NULL;
	}

#if 0
	/*
	 * For a shared hash table, preallocate the requested number of elements.
	 * This reduces problems with run-time out-of-shared-memory conditions.
	 *
	 * For a non-shared hash table, preallocate the requested number of
	 * elements if it's less than our chosen nelem_alloc.  This avoids wasting
	 * space if the caller correctly estimates a small table size.
	 */
	if ((flags & HASH_SHARED_MEM) ||
		nelem < hctl->nelem_alloc)
	{
		int			i,
			freelist_partitions,
			nelem_alloc,
			nelem_alloc_first;

		/*
		 * If hash table is partitioned, give each freelist an equal share of
		 * the initial allocation.  Otherwise only freeList[0] is used.
		 */
		if (IS_PARTITIONED(hashp->hctl))
			freelist_partitions = NUM_FREELISTS;
		else
			freelist_partitions = 1;

		nelem_alloc = nelem / freelist_partitions;
		if (nelem_alloc <= 0)
			nelem_alloc = 1;

		/*
		 * Make sure we'll allocate all the requested elements; freeList[0]
		 * gets the excess if the request isn't divisible by NUM_FREELISTS.
		 */
		if (nelem_alloc * freelist_partitions < nelem)
			nelem_alloc_first =
			nelem - nelem_alloc * (freelist_partitions - 1);
		else
			nelem_alloc_first = nelem_alloc;

		for (i = 0; i < freelist_partitions; i++)
		{
			int			temp = (i == 0) ? nelem_alloc_first : nelem_alloc;

			if (!element_alloc(hashp, temp, i))
				ereport(ERROR,
					(errcode(ERRCODE_OUT_OF_MEMORY),
						errmsg("out of memory")));
		}
	}
#endif
	if (flags & HASH_FIXED_SIZE)
		hashp->isfixed = true;
	return hashp;
}

/********************** DESTROY ROUTINES ************************/
void hash_destroy(HTAB* hashp)
{
	if (hashp != NULL)
	{
		/* allocation method must be one we know how to free, too */
		Assert(hashp->alloc == DynaHashAlloc);
		/* so this hashtable must have its own context */
		Assert(hashp->hcxt != NULL);

		//hash_stats("destroy", hashp);

		/*
		 * Free everything by destroying the hash table's memory context.
		 */
		MemoryContextDelete(hashp->hcxt);
	}
}


/*
 * allocate some new elements and link them into the indicated free list
 */
static bool element_alloc(HTAB* hashp, int nelem, int freelist_idx)
{
	HASHHDR* hctl = hashp->hctl;
	Size		elementSize;
	HASHELEMENT* firstElement;
	HASHELEMENT* tmpElement;
	HASHELEMENT* prevElement;
	int			i;

	if (hashp->isfixed)
		return false;

	freelist_idx = 0;
	/* Each element has a HASHELEMENT header plus user data. */
	elementSize = MAXALIGN(sizeof(HASHELEMENT)) + MAXALIGN(hctl->entrysize);

	firstElement = (HASHELEMENT*)hashp->alloc(hashp->hcxt, nelem * elementSize);

	if (!firstElement)
		return false;

	/* prepare to link all the new entries into the freelist */
	prevElement = NULL;
	tmpElement = firstElement;
	for (i = 0; i < nelem; i++)
	{
		tmpElement->link = prevElement;
		prevElement = tmpElement;
		tmpElement = (HASHELEMENT*)(((char*)tmpElement) + elementSize);
	}

#if 0
	/* if partitioned, must lock to touch freeList */
	if (IS_PARTITIONED(hctl))
		SpinLockAcquire(&hctl->freeList[freelist_idx].mutex);
#endif
	/* freelist could be nonempty if two backends did this concurrently */
	firstElement->link = hctl->freeList[freelist_idx].freeList;
	hctl->freeList[freelist_idx].freeList = prevElement;

#if 0
	if (IS_PARTITIONED(hctl))
		SpinLockRelease(&hctl->freeList[freelist_idx].mutex);
#endif
	return true;
}


/*
 * Allocate a new hashtable entry if possible; return NULL if out of memory.
 * (Or, if the underlying space allocator throws error for out-of-memory,
 * we won't return at all.)
 */
static HASHBUCKET get_hash_entry(HTAB* hashp, int freelist_idx)
{
	HASHHDR* hctl = hashp->hctl;
	HASHBUCKET	newElement;

	for (;;)
	{
#if 0
		/* if partitioned, must lock to touch nentries and freeList */
		if (IS_PARTITIONED(hctl))
			SpinLockAcquire(&hctl->freeList[freelist_idx].mutex);
#endif
		/* try to get an entry from the freelist */
		newElement = hctl->freeList[freelist_idx].freeList;

		if (newElement != NULL)
			break;
#if 0
		if (IS_PARTITIONED(hctl))
			SpinLockRelease(&hctl->freeList[freelist_idx].mutex);
#endif
		/*
		 * No free elements in this freelist.  In a partitioned table, there
		 * might be entries in other freelists, but to reduce contention we
		 * prefer to first try to get another chunk of buckets from the main
		 * shmem allocator.  If that fails, though, we *MUST* root through all
		 * the other freelists before giving up.  There are multiple callers
		 * that assume that they can allocate every element in the initially
		 * requested table size, or that deleting an element guarantees they
		 * can insert a new element, even if shared memory is entirely full.
		 * Failing because the needed element is in a different freelist is
		 * not acceptable.
		 */
		if (!element_alloc(hashp, hctl->nelem_alloc, freelist_idx))
		{
#if 0
			int			borrow_from_idx;

			if (!IS_PARTITIONED(hctl))
				return NULL;	/* out of memory */

			/* try to borrow element from another freelist */
			borrow_from_idx = freelist_idx;
			for (;;)
			{
				borrow_from_idx = (borrow_from_idx + 1) % NUM_FREELISTS;
				if (borrow_from_idx == freelist_idx)
					break;		/* examined all freelists, fail */

				SpinLockAcquire(&(hctl->freeList[borrow_from_idx].mutex));
				newElement = hctl->freeList[borrow_from_idx].freeList;

				if (newElement != NULL)
				{
					hctl->freeList[borrow_from_idx].freeList = newElement->link;
					SpinLockRelease(&(hctl->freeList[borrow_from_idx].mutex));

					/* careful: count the new element in its proper freelist */
					SpinLockAcquire(&hctl->freeList[freelist_idx].mutex);
					hctl->freeList[freelist_idx].nentries++;
					SpinLockRelease(&hctl->freeList[freelist_idx].mutex);

					return newElement;
				}

				SpinLockRelease(&(hctl->freeList[borrow_from_idx].mutex));
			}

			/* no elements available to borrow either, so out of memory */
#endif
			return NULL;
		}
	}

	/* remove entry from freelist, bump nentries */
	hctl->freeList[freelist_idx].freeList = newElement->link;
	hctl->freeList[freelist_idx].nentries++;

#if 0
	if (IS_PARTITIONED(hctl))
		SpinLockRelease(&hctl->freeList[freelist_idx].mutex);
#endif 
	return newElement;
}

/*
 * Expand the table by adding one more hash bucket.
 */
static bool expand_table(HTAB* hashp)
{
	HASHHDR* hctl = hashp->hctl;
	HASHSEGMENT old_seg, new_seg;
	long  old_bucket, new_bucket;
	long  new_segnum, new_segndx;
	long  old_segnum, old_segndx;
	HASHBUCKET* oldlink, * newlink;
	HASHBUCKET	currElement, nextElement;

	Assert(!IS_PARTITIONED(hctl));

#ifdef HASH_STATISTICS
	hash_expansions++;
#endif

	new_bucket = hctl->max_bucket + 1;
	new_segnum = new_bucket >> hashp->sshift;
	new_segndx = MOD(new_bucket, hashp->ssize);

	if (new_segnum >= hctl->nsegs)
	{
		/* Allocate new segment if necessary -- could fail if dir full */
		if (new_segnum >= hctl->dsize)
			if (!dir_realloc(hashp))
				return false;
		if (!(hashp->dir[new_segnum] = seg_alloc(hashp)))
			return false;
		hctl->nsegs++;
	}

	/* OK, we created a new bucket */
	hctl->max_bucket++;

	/*
	 * *Before* changing masks, find old bucket corresponding to same hash
	 * values; values in that bucket may need to be relocated to new bucket.
	 * Note that new_bucket is certainly larger than low_mask at this point,
	 * so we can skip the first step of the regular hash mask calc.
	 */
	old_bucket = (new_bucket & hctl->low_mask);

	/*
	 * If we crossed a power of 2, readjust masks.
	 */
	if ((uint32)new_bucket > hctl->high_mask)
	{
		hctl->low_mask = hctl->high_mask;
		hctl->high_mask = (uint32)new_bucket | hctl->low_mask;
	}

	/*
	 * Relocate records to the new bucket.  NOTE: because of the way the hash
	 * masking is done in calc_bucket, only one old bucket can need to be
	 * split at this point.  With a different way of reducing the hash value,
	 * that might not be true!
	 */
	old_segnum = old_bucket >> hashp->sshift;
	old_segndx = MOD(old_bucket, hashp->ssize);

	old_seg = hashp->dir[old_segnum];
	new_seg = hashp->dir[new_segnum];

	oldlink = &old_seg[old_segndx];
	newlink = &new_seg[new_segndx];

	for (currElement = *oldlink; currElement != NULL; currElement = nextElement)
	{
		nextElement = currElement->link;
		if ((long)calc_bucket(hctl, currElement->hashvalue) == old_bucket)
		{
			*oldlink = currElement;
			oldlink = &currElement->link;
		}
		else
		{
			*newlink = currElement;
			newlink = &currElement->link;
		}
	}
	/* don't forget to terminate the rebuilt hash chains... */
	*oldlink = NULL;
	*newlink = NULL;

	return true;
}

/* Check if a table has any active scan */
static bool has_seq_scans(HTAB* hashp)
{
#if 0
	int			i;

	for (i = 0; i < num_seq_scans; i++)
	{
		if (seq_scan_tables[i] == hashp)
			return true;
	}
#endif
	return false;
}

#if 0
#define FREELIST_IDX(hctl, hashcode) \
	(IS_PARTITIONED(hctl) ? (hashcode) % NUM_FREELISTS : 0)
#endif 

void* hash_search_with_hash_value(HTAB* hashp, const void* keyPtr, uint32 hashvalue, HASHACTION action, bool* foundPtr)
{
	HASHHDR* hctl = hashp->hctl;
	int			freelist_idx = 0; // FREELIST_IDX(hctl, hashvalue);
	Size		keysize;
	uint32		bucket;
	long		segment_num;
	long		segment_ndx;
	HASHSEGMENT segp;
	HASHBUCKET	currBucket;
	HASHBUCKET* prevBucketPtr;
	HashCompareFunc match;

#ifdef HASH_STATISTICS
	hash_accesses++;
	hctl->accesses++;
#endif

	/*
	 * If inserting, check if it is time to split a bucket.
	 *
	 * NOTE: failure to expand table is not a fatal error, it just means we
	 * have to run at higher fill factor than we wanted.  However, if we're
	 * using the palloc allocator then it will throw error anyway on
	 * out-of-memory, so we must do this before modifying the table.
	 */
	if (action == HASH_ENTER || action == HASH_ENTER_NULL)
	{
		/*
		 * Can't split if running in partitioned mode, nor if frozen, nor if
		 * table is the subject of any active hash_seq_search scans.
		 */
		if (hctl->freeList[0].nentries > (long)hctl->max_bucket && !IS_PARTITIONED(hctl) && !hashp->frozen && !has_seq_scans(hashp))
			(void)expand_table(hashp);
	}

	/*
	 * Do the initial lookup
	 */
	bucket = calc_bucket(hctl, hashvalue);

	segment_num = bucket >> hashp->sshift;
	segment_ndx = MOD(bucket, hashp->ssize);

	segp = hashp->dir[segment_num];

	if (segp == NULL)
	{
		return NULL;
		//hash_corrupted(hashp);
	}

	prevBucketPtr = &segp[segment_ndx];
	currBucket = *prevBucketPtr;

	/*
	 * Follow collision chain looking for matching key
	 */
	match = hashp->match;		/* save one fetch in inner loop */
	keysize = hashp->keysize;	/* ditto */

	while (currBucket != NULL)
	{
		if (currBucket->hashvalue == hashvalue &&
			match(ELEMENTKEY(currBucket), keyPtr, keysize) == 0)
			break;
		prevBucketPtr = &(currBucket->link);
		currBucket = *prevBucketPtr;
#ifdef HASH_STATISTICS
		hash_collisions++;
		hctl->collisions++;
#endif
	}

	if (foundPtr)
		*foundPtr = (bool)(currBucket != NULL);

	/*
	 * OK, now what?
	 */
	switch (action)
	{
	case HASH_FIND:
		if (currBucket != NULL)
			return (void*)ELEMENTKEY(currBucket);
		return NULL;

	case HASH_REMOVE:
		if (currBucket != NULL)
		{
#if 0
			/* if partitioned, must lock to touch nentries and freeList */
			if (IS_PARTITIONED(hctl))
				SpinLockAcquire(&(hctl->freeList[freelist_idx].mutex));
#endif
			/* delete the record from the appropriate nentries counter. */
			Assert(hctl->freeList[freelist_idx].nentries > 0);
			hctl->freeList[freelist_idx].nentries--;

			/* remove record from hash bucket's chain. */
			*prevBucketPtr = currBucket->link;

			/* add the record to the appropriate freelist. */
			currBucket->link = hctl->freeList[freelist_idx].freeList;
			hctl->freeList[freelist_idx].freeList = currBucket;
#if 0
			if (IS_PARTITIONED(hctl))
				SpinLockRelease(&hctl->freeList[freelist_idx].mutex);
#endif
			/*
			 * better hope the caller is synchronizing access to this
			 * element, because someone else is going to reuse it the next
			 * time something is added to the table
			 */
			return (void*)ELEMENTKEY(currBucket);
		}
		return NULL;

	case HASH_ENTER:
	case HASH_ENTER_NULL:
		/* Return existing element if found, else create one */
		if (currBucket != NULL)
			return (void*)ELEMENTKEY(currBucket);

		/* disallow inserts if frozen */
		if (hashp->frozen)
		{
			return NULL;
#if 0
			elog(ERROR, "cannot insert into frozen hashtable \"%s\"",
				hashp->tabname);
#endif
		}

		currBucket = get_hash_entry(hashp, freelist_idx);
		if (currBucket == NULL)
		{
			return NULL;
#if 0
			/* out of memory */
			if (action == HASH_ENTER_NULL)
				return NULL;
			/* report a generic message */
			if (hashp->isshared)
				ereport(ERROR,
					(errcode(ERRCODE_OUT_OF_MEMORY),
						errmsg("out of shared memory")));
			else
				ereport(ERROR,
					(errcode(ERRCODE_OUT_OF_MEMORY),
						errmsg("out of memory")));
#endif
		}

		/* link into hashbucket chain */
		*prevBucketPtr = currBucket;
		currBucket->link = NULL;

		/* copy key into record */
		currBucket->hashvalue = hashvalue;
		hashp->keycopy(ELEMENTKEY(currBucket), keyPtr, keysize);

		/*
		 * Caller is expected to fill the data field on return.  DO NOT
		 * insert any code that could possibly throw error here, as doing
		 * so would leave the table entry incomplete and hence corrupt the
		 * caller's data structure.
		 */

		return (void*)ELEMENTKEY(currBucket);
	}
#if 0
	elog(ERROR, "unrecognized hash action code: %d", (int)action);
#endif
	return NULL;				/* keep compiler quiet */
}

/*
 * hash_search -- look up key in table and perform action
 * hash_search_with_hash_value -- same, with key's hash value already computed
 *
 * action is one of:
 *		HASH_FIND: look up key in table
 *		HASH_ENTER: look up key in table, creating entry if not present
 *		HASH_ENTER_NULL: same, but return NULL if out of memory
 *		HASH_REMOVE: look up key in table, remove entry if present
 *
 * Return value is a pointer to the element found/entered/removed if any,
 * or NULL if no match was found.  (NB: in the case of the REMOVE action,
 * the result is a dangling pointer that shouldn't be dereferenced!)
 *
 * HASH_ENTER will normally ereport a generic "out of memory" error if
 * it is unable to create a new entry.  The HASH_ENTER_NULL operation is
 * the same except it will return NULL if out of memory.
 *
 * If foundPtr isn't NULL, then *foundPtr is set true if we found an
 * existing entry in the table, false otherwise.  This is needed in the
 * HASH_ENTER case, but is redundant with the return value otherwise.
 *
 * For hash_search_with_hash_value, the hashvalue parameter must have been
 * calculated with get_hash_value().
 */
void* hash_search(HTAB* hashp, const void* keyPtr, HASHACTION action, bool* foundPtr)
{
	void* ret = NULL;
	if (hashp)
	{
		uint32 hashvalue;

		hashvalue = hashp->hash(keyPtr, hashp->keysize);

		ret = hash_search_with_hash_value(hashp, keyPtr, hashvalue, action, foundPtr);
	}

	return ret;
}

/*
 * Given the user-specified entry size, choose nelem_alloc, ie, how many
 * elements to add to the hash table when we need more.
 */
static int choose_nelem_alloc(Size entrysize)
{
	int			nelem_alloc;
	Size		elementSize;
	Size		allocSize;

	/* Each element has a HASHELEMENT header plus user data. */
	/* NB: this had better match element_alloc() */
	elementSize = MAXALIGN(sizeof(HASHELEMENT)) + MAXALIGN(entrysize);

	/*
	 * The idea here is to choose nelem_alloc at least 32, but round up so
	 * that the allocation request will be a power of 2 or just less. This
	 * makes little difference for hash tables in shared memory, but for hash
	 * tables managed by palloc, the allocation request will be rounded up to
	 * a power of 2 anyway.  If we fail to take this into account, we'll waste
	 * as much as half the allocated space.
	 */
	allocSize = 32 * 4;			/* assume elementSize at least 8 */
	do
	{
		allocSize <<= 1;
		nelem_alloc = allocSize / elementSize;
	} while (nelem_alloc < 32);

	return nelem_alloc;
}

static bool dir_realloc(HTAB* hashp)
{
	HASHSEGMENT* p;
	HASHSEGMENT* old_p;
	long		new_dsize;
	long		old_dirsize;
	long		new_dirsize;

	if (hashp->hctl->max_dsize != NO_MAX_DSIZE)
		return false;

	/* Reallocate directory */
	new_dsize = hashp->hctl->dsize << 1;
	old_dirsize = hashp->hctl->dsize * sizeof(HASHSEGMENT);
	new_dirsize = new_dsize * sizeof(HASHSEGMENT);

	old_p = hashp->dir;

	p = (HASHSEGMENT*)hashp->alloc(hashp->hcxt, (Size)new_dirsize);

	if (p != NULL)
	{
		memcpy(p, old_p, old_dirsize);
		MemSet(((char*)p) + old_dirsize, 0, new_dirsize - old_dirsize);
		hashp->dir = p;
		hashp->hctl->dsize = new_dsize;

		/* XXX assume the allocator is palloc, so we know how to free */
		Assert(hashp->alloc == DynaHashAlloc);
		wt_pfree(old_p);

		return true;
	}

	return false;
}
