#pragma once

#define FG_ATTRS (FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY)
#define BG_ATTRS (BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED | BACKGROUND_INTENSITY)
#define META_ATTRS (COMMON_LVB_LEADING_BYTE | COMMON_LVB_TRAILING_BYTE | COMMON_LVB_GRID_HORIZONTAL | COMMON_LVB_GRID_LVERTICAL | COMMON_LVB_GRID_RVERTICAL | COMMON_LVB_REVERSE_VIDEO | COMMON_LVB_UNDERSCORE)
#define USED_META_ATTRS (META_ATTRS & ~COMMON_LVB_SBCSDBCS) // We don't retain lead/trailing byte information in the TextAttribute class

enum class CharacterAttributes : uint16_t
{
    Normal = 0x00,
    Intense = 0x01,
    Italics = 0x02,
    Blinking = 0x04,
    Invisible = 0x08,
    CrossedOut = 0x10,
    Faint = 0x20,

    // 7th, 8th, 9th bit reserved for underline styles
    UnderlineStyle = 0x1C0,

    Unused1 = 0x200,
    TopGridline = COMMON_LVB_GRID_HORIZONTAL, // 0x400
    LeftGridline = COMMON_LVB_GRID_LVERTICAL, // 0x800
    RightGridline = COMMON_LVB_GRID_RVERTICAL, // 0x1000
    Protected = 0x2000,
    ReverseVideo = COMMON_LVB_REVERSE_VIDEO, // 0x4000
    BottomGridline = COMMON_LVB_UNDERSCORE, // 0x8000

    All = 0xFFFF, // All character attributes
    Rendition = All & ~Protected // Only rendition attributes (everything except Protected)
};
DEFINE_ENUM_FLAG_OPERATORS(CharacterAttributes);

constexpr uint8_t UNDERLINE_STYLE_SHIFT = 6;

enum class CursorType : unsigned int
{
    Legacy = 0x0, // uses the cursor's height value to range from underscore-like to full box
    VerticalBar = 0x1, // A single vertical line, '|'
    Underscore = 0x2, // a single horizontal underscore, smaller that the min height legacy cursor.
    EmptyBox = 0x3, // Just the outline of a full box
    FullBox = 0x4, // a full box, similar to legacy with height=100%
    DoubleUnderscore = 0x5 // a double horizontal underscore
};

// Valid COLORREFs are of the pattern 0x00bbggrr. -1 works as an invalid color,
//      as the highest byte of a valid color is always 0.
constexpr COLORREF INVALID_COLOR = 0xffffffff;

constexpr WORD COLOR_TABLE_SIZE = 16;

/*++
Copyright (c) Microsoft Corporation
Licensed under the MIT license.

Module Name:
- DbcsAttribute.hpp

Abstract:
- helper class for storing double byte character set information about a cell in the output buffer.

Author(s):
- Austin Diviness (AustDi) 26-Jan-2018

Revision History:
--*/

#pragma once

enum class DbcsAttribute : uint8_t
{
    Single,
    Leading,
    Trailing,
};

constexpr WORD GeneratePublicApiAttributeFormat(DbcsAttribute attribute) noexcept
{
    switch (attribute)
    {
    case DbcsAttribute::Leading:
        return COMMON_LVB_LEADING_BYTE;
    case DbcsAttribute::Trailing:
        return COMMON_LVB_TRAILING_BYTE;
    default:
        return 0;
    }
}

/*++
Copyright (c) Microsoft Corporation
Licensed under the MIT license.

Module Name:
- LineRendition.hpp

Abstract:
- Enumerated type for the VT line rendition attribute. This determines the
  width and height scaling with which each line is rendered.

--*/

enum class LineRendition : uint8_t
{
    SingleWidth,
    DoubleWidth,
    DoubleHeightTop,
    DoubleHeightBottom
};

constexpr til::inclusive_rect ScreenToBufferLine(const til::inclusive_rect& line, const LineRendition lineRendition)
{
    const auto scale = lineRendition == LineRendition::SingleWidth ? 0 : 1;
    return { line.left >> scale, line.top, line.right >> scale, line.bottom };
}

constexpr til::point ScreenToBufferLineInclusive(const til::point& line, const LineRendition lineRendition)
{
    const auto scale = lineRendition == LineRendition::SingleWidth ? 0 : 1;
    return { line.x >> scale, line.y };
}

constexpr til::rect BufferToScreenLine(const til::rect& line, const LineRendition lineRendition)
{
    const auto scale = lineRendition == LineRendition::SingleWidth ? 0 : 1;
    return { line.left << scale, line.top, line.right << scale, line.bottom };
}

constexpr til::inclusive_rect BufferToScreenLine(const til::inclusive_rect& line, const LineRendition lineRendition)
{
    const auto scale = lineRendition == LineRendition::SingleWidth ? 0 : 1;
    return { line.left << scale, line.top, (line.right << scale) + scale, line.bottom };
}

/*++
Copyright (c) Microsoft Corporation
Licensed under the MIT license.

Module Name:
- marks.hpp

Abstract:
- Definitions for types that are used for "scroll marks" and shell integration
  in the buffer.
- Scroll marks are identified by the existence of "ScrollbarData" on a ROW in the buffer.
- Shell integration will then also markup the buffer with special
  TextAttributes, to identify regions of text as the Prompt, the Command, the
  Output, etc.
- MarkExtents are used to abstract away those regions of text, so a caller
  doesn't need to iterate over the buffer themselves.
--*/

enum class MarkCategory : uint8_t
{
    Default = 0,
    Error = 1,
    Warning = 2,
    Success = 3,
    Prompt = 4
};

// This is the data that's stored on each ROW, to suggest that there's something
// interesting on this row to show in the scrollbar. Also used in conjunction
// with shell integration - when a prompt is added through shell integration,
// we'll also add a scrollbar mark as a quick "bookmark" to the start of that
// command.
struct ScrollbarData
{
    MarkCategory category{ MarkCategory::Default };

    // Scrollbar marks may have been given a color, or not.
    std::optional<til::color> color;

    // Prompts without an exit code haven't had a matching FTCS CommandEnd
    // called yet. Any value other than 0 is an error.
    std::optional<uint32_t> exitCode;
    // Future consideration: stick the literal command as a string on here, if
    // we were given it with the 633;E sequence.
};

// Helper struct for describing the bounds of a command and it's output,
// * The Prompt is between the start & end
// * The Command is between the end & commandEnd
// * The Output is between the commandEnd & outputEnd
//
// These are not actually stored in the buffer. The buffer can produce them for
// callers, to make reasoning about regions of the buffer easier.
struct MarkExtents
{
    // Data from the row
    ScrollbarData data;

    til::point start;
    til::point end; // exclusive
    std::optional<til::point> commandEnd;
    std::optional<til::point> outputEnd;

    // MarkCategory category{ MarkCategory::Info };
    // Other things we may want to think about in the future are listed in
    // GH#11000

    bool HasCommand() const noexcept
    {
        return commandEnd.has_value() && *commandEnd != end;
    }
    bool HasOutput() const noexcept
    {
        return outputEnd.has_value() && *outputEnd != *commandEnd;
    }
    std::pair<til::point, til::point> GetExtent() const
    {
        til::point realEnd{ til::coalesce_value(outputEnd, commandEnd, end) };
        return std::make_pair(start, realEnd);
    }
};

// Another helper, for when callers would like to know just about the data of
// the scrollbar, but don't actually need all the extents of prompts.
struct ScrollMark
{
    til::CoordType row{ 0 };
    ScrollbarData data;
};

/*++
Copyright (c) Microsoft Corporation
Licensed under the MIT license.

Module Name:
- TextAttribute.hpp

Abstract:
- contains data structure for run-length-encoding of text attribute data

Author(s):
- Michael Niksa (miniksa) 10-Apr-2014
- Paul Campbell (paulcam) 10-Apr-2014

Revision History:
- From components of output.h/.c
  by Therese Stowell (ThereseS) 1990-1991
- Pulled into its own file from textBuffer.hpp/cpp (AustDi, 2017)
- Pulled each of the fg/bg colors into their own abstraction (migrie, Nov 2018)
--*/

enum class UnderlineStyle
{
    NoUnderline = 0U,
    SinglyUnderlined = 1U,
    DoublyUnderlined = 2U,
    CurlyUnderlined = 3U,
    DottedUnderlined = 4U,
    DashedUnderlined = 5U,
    Max = DashedUnderlined
};

// We only need a few bits, but uint8_t apparently doesn't satisfy std::has_unique_object_representations_v
enum class MarkKind : uint16_t
{
    None = 0,
    Prompt = 1,
    Command = 2,
    Output = 3,
};

#if 0
class TextAttribute final
{
public:
    constexpr TextAttribute() noexcept :
        _attrs{ CharacterAttributes::Normal },
        _foreground{},
        _background{},
        _hyperlinkId{ 0 },
        _underlineColor{},
        _markKind{ MarkKind::None }
    {
    }

    explicit constexpr TextAttribute(const WORD wLegacyAttr) noexcept :
        _attrs{ gsl::narrow_cast<WORD>(wLegacyAttr & USED_META_ATTRS) },
        _foreground{ gsl::at(s_legacyForegroundColorMap, wLegacyAttr & FG_ATTRS) },
        _background{ gsl::at(s_legacyBackgroundColorMap, (wLegacyAttr & BG_ATTRS) >> 4) },
        _hyperlinkId{ 0 },
        _underlineColor{},
        _markKind{ MarkKind::None }
    {
    }

    constexpr TextAttribute(const COLORREF rgbForeground,
        const COLORREF rgbBackground,
        const COLORREF rgbUnderline = INVALID_COLOR) noexcept :
        _attrs{ CharacterAttributes::Normal },
        _foreground{ rgbForeground },
        _background{ rgbBackground },
        _hyperlinkId{ 0 },
        _underlineColor{ rgbUnderline },
        _markKind{ MarkKind::None }
    {
    }

    constexpr TextAttribute(const CharacterAttributes attrs, const TextColor foreground, const TextColor background, const uint16_t hyperlinkId, const TextColor underlineColor) noexcept :
        _attrs{ attrs },
        _foreground{ foreground },
        _background{ background },
        _hyperlinkId{ hyperlinkId },
        _underlineColor{ underlineColor },
        _markKind{ MarkKind::None }
    {
    }

    static void SetLegacyDefaultAttributes(const WORD defaultAttributes) noexcept;
    WORD GetLegacyAttributes() const noexcept;

    bool IsTopHorizontalDisplayed() const noexcept;
    bool IsBottomHorizontalDisplayed() const noexcept;
    bool IsLeftVerticalDisplayed() const noexcept;
    bool IsRightVerticalDisplayed() const noexcept;

    void SetLeftVerticalDisplayed(const bool isDisplayed) noexcept;
    void SetRightVerticalDisplayed(const bool isDisplayed) noexcept;

    void Invert() noexcept;

    inline bool operator==(const TextAttribute& other) const noexcept
    {
        return memcmp(this, &other, sizeof(TextAttribute)) == 0;
    }

    inline bool operator!=(const TextAttribute& other) const noexcept
    {
        return memcmp(this, &other, sizeof(TextAttribute)) != 0;
    }

    bool IsLegacy() const noexcept;
    bool IsIntense() const noexcept;
    bool IsFaint() const noexcept;
    bool IsItalic() const noexcept;
    bool IsBlinking() const noexcept;
    bool IsInvisible() const noexcept;
    bool IsCrossedOut() const noexcept;
    bool IsUnderlined() const noexcept;
    bool IsOverlined() const noexcept;
    bool IsReverseVideo() const noexcept;
    bool IsProtected() const noexcept;

    void SetIntense(bool isIntense) noexcept;
    void SetFaint(bool isFaint) noexcept;
    void SetItalic(bool isItalic) noexcept;
    void SetBlinking(bool isBlinking) noexcept;
    void SetInvisible(bool isInvisible) noexcept;
    void SetCrossedOut(bool isCrossedOut) noexcept;
    void SetUnderlineStyle(const UnderlineStyle underlineStyle) noexcept;
    void SetOverlined(bool isOverlined) noexcept;
    void SetReverseVideo(bool isReversed) noexcept;
    void SetProtected(bool isProtected) noexcept;

    constexpr void SetCharacterAttributes(const CharacterAttributes attrs) noexcept
    {
        _attrs = attrs;
    }
    constexpr CharacterAttributes GetCharacterAttributes() const noexcept
    {
        return _attrs;
    }

    constexpr void SetMarkAttributes(const MarkKind attrs) noexcept
    {
        _markKind = attrs;
    }
    constexpr MarkKind GetMarkAttributes() const noexcept
    {
        return _markKind;
    }

    bool IsHyperlink() const noexcept;

    TextColor GetForeground() const noexcept;
    TextColor GetBackground() const noexcept;
    uint16_t GetHyperlinkId() const noexcept;
    TextColor GetUnderlineColor() const noexcept;
    UnderlineStyle GetUnderlineStyle() const noexcept;
    void SetForeground(const TextColor foreground) noexcept;
    void SetBackground(const TextColor background) noexcept;
    void SetUnderlineColor(const TextColor color) noexcept;
    void SetForeground(const COLORREF rgbForeground) noexcept;
    void SetBackground(const COLORREF rgbBackground) noexcept;
    void SetIndexedForeground(const BYTE fgIndex) noexcept;
    void SetIndexedBackground(const BYTE bgIndex) noexcept;
    void SetIndexedForeground256(const BYTE fgIndex) noexcept;
    void SetIndexedBackground256(const BYTE bgIndex) noexcept;
    void SetColor(const COLORREF rgbColor, const bool fIsForeground) noexcept;
    void SetHyperlinkId(uint16_t id) noexcept;

    void SetDefaultForeground() noexcept;
    void SetDefaultBackground() noexcept;
    void SetDefaultUnderlineColor() noexcept;
    void SetDefaultRenditionAttributes() noexcept;

    bool BackgroundIsDefault() const noexcept;

    void SetStandardErase() noexcept;

    // This returns whether this attribute, if printed directly next to another attribute, for the space
    // character, would look identical to the other one.
    bool HasIdenticalVisualRepresentationForBlankSpace(const TextAttribute& other, const bool inverted = false) const noexcept
    {
        // sneaky-sneaky: I'm using xor here
        // inverted is whether there's a global invert; Reverse is a local one.
        // global ^ local == true : the background attribute is actually the visible foreground, so we care about the foregrounds being identical
        // global ^ local == false: the foreground attribute is the visible foreground, so we care about the backgrounds being identical
        const auto checkForeground = (inverted != IsReverseVideo());
        return !IsAnyGridLineEnabled() && // grid lines have a visual representation
            // styled underline and crossed out have a visual representation
            !IsUnderlined() && WI_IsFlagClear(_attrs, CharacterAttributes::CrossedOut) &&
            // hyperlinks have a visual representation
            !IsHyperlink() &&
            // all other attributes do not have a visual representation
            _attrs == other._attrs &&
            ((checkForeground && _foreground == other._foreground) ||
                (!checkForeground && _background == other._background)) &&
            IsHyperlink() == other.IsHyperlink();
    }

    constexpr bool IsAnyGridLineEnabled() const noexcept
    {
        return WI_IsAnyFlagSet(_attrs, CharacterAttributes::TopGridline | CharacterAttributes::LeftGridline | CharacterAttributes::RightGridline | CharacterAttributes::BottomGridline);
    }
    constexpr bool HasAnyVisualAttributes() const noexcept
    {
        return GetCharacterAttributes() != CharacterAttributes::Normal || GetHyperlinkId() != 0;
    }

private:
    static std::array<TextColor, 16> s_legacyForegroundColorMap;
    static std::array<TextColor, 16> s_legacyBackgroundColorMap;

    CharacterAttributes _attrs; // sizeof: 2, alignof: 2
    uint16_t _hyperlinkId; // sizeof: 2, alignof: 2
    TextColor _foreground; // sizeof: 4, alignof: 1
    TextColor _background; // sizeof: 4, alignof: 1
    TextColor _underlineColor; // sizeof: 4, alignof: 1
    MarkKind _markKind; // sizeof: 2, alignof: 1

#ifdef UNIT_TESTING
    friend class TextBufferTests;
    friend class TextAttributeTests;
    template<typename TextAttribute>
    friend class WEX::TestExecution::VerifyOutputTraits;
#endif
};
#endif 

enum class TextAttributeBehavior
{
    Stored, // use contained text attribute
    Current, // use text attribute of cell being written to
    StoredOnly, // only use the contained text attribute and skip the insertion of anything else
};

#ifdef UNIT_TESTING

#define LOG_ATTR(attr) (Log::Comment(NoThrowString().Format( \
    L## #attr L"=%s", VerifyOutputTraits<TextAttribute>::ToString(attr).GetBuffer())))

namespace WEX
{
    namespace TestExecution
    {
        template<>
        class VerifyOutputTraits<TextAttribute>
        {
        public:
            static WEX::Common::NoThrowString ToString(const TextAttribute& attr)
            {
                return WEX::Common::NoThrowString().Format(
                    L"{FG:%s,BG:%s,intense:%d,attrs:(0x%02x)}",
                    VerifyOutputTraits<TextColor>::ToString(attr._foreground).GetBuffer(),
                    VerifyOutputTraits<TextColor>::ToString(attr._background).GetBuffer(),
                    attr.IsIntense(),
                    static_cast<DWORD>(attr._attrs));
            }
        };
    }
}
#endif


/*++
Copyright (c) Microsoft Corporation
Licensed under the MIT license.

Module Name:
- OutputCell.hpp

Abstract:
- Representation of all data stored in a cell of the output buffer.
- RGB color supported.

Author:
- Austin Diviness (AustDi) 20-Mar-2018

--*/
#if 0
#include <exception>
#include <variant>

class InvalidCharInfoConversionException : public std::exception
{
    const char* what() const noexcept override
    {
        return "Cannot convert to CHAR_INFO without explicit TextAttribute";
    }
};

class OutputCell final
{
public:
    OutputCell() noexcept;

    OutputCell(const std::wstring_view charData,
        const DbcsAttribute dbcsAttribute,
        const TextAttributeBehavior behavior);

    OutputCell(const std::wstring_view charData,
        const DbcsAttribute dbcsAttribute,
        const TextAttribute textAttribute);

    OutputCell(const OutputCellView& view);

    OutputCell(const OutputCell&) = default;
    OutputCell(OutputCell&&) = default;

    OutputCell& operator=(const OutputCell&) = default;
    OutputCell& operator=(OutputCell&&) = default;

    ~OutputCell() = default;

    const std::wstring_view Chars() const noexcept;
    void SetChars(const std::wstring_view chars);

    DbcsAttribute& DbcsAttr() noexcept;
    TextAttribute& TextAttr();

    constexpr const DbcsAttribute& DbcsAttr() const
    {
        return _dbcsAttribute;
    }

    const TextAttribute& TextAttr() const
    {
        THROW_HR_IF(E_INVALIDARG, _behavior == TextAttributeBehavior::Current);
        return _textAttribute;
    }

    constexpr TextAttributeBehavior TextAttrBehavior() const
    {
        return _behavior;
    }

private:
    // basic_string contains a small storage internally so we don't need
    // to worry about heap allocation for short strings.
    std::wstring _text;
    DbcsAttribute _dbcsAttribute = DbcsAttribute::Single;
    TextAttribute _textAttribute;
    TextAttributeBehavior _behavior;

    void _setFromBehavior(const TextAttributeBehavior behavior);
    void _setFromStringView(const std::wstring_view view);
    void _setFromOutputCellView(const OutputCellView& cell);
};
#endif 


class ROW;
class TextBuffer;

enum class DelimiterClass
{
    ControlChar,
    DelimiterChar,
    RegularChar
};

struct RowWriteState
{
    // The text you want to write into the given ROW. When ReplaceText() returns,
    // this is updated to remove all text from the beginning that was successfully written.
    std::wstring_view text; // IN/OUT
    // The column at which to start writing.
    til::CoordType columnBegin = 0; // IN
    // The first column which should not be written to anymore.
    til::CoordType columnLimit = til::CoordTypeMax; // IN

    // The column 1 past the last glyph that was successfully written into the row. If you need to call
    // ReplaceAttributes() to colorize the written range, etc., this is the columnEnd parameter you want.
    // If you want to continue writing where you left off, this is also the next columnBegin parameter.
    til::CoordType columnEnd = 0; // OUT
    // The first column that got modified by this write operation. In case that the first glyph we write overwrites
    // the trailing half of a wide glyph, leadingSpaces will be 1 and this value will be 1 less than colBeg.
    til::CoordType columnBeginDirty = 0; // OUT
    // This is 1 past the last column that was modified and will be 1 past columnEnd if we overwrote
    // the leading half of a wide glyph and had to fill the trailing half with whitespace.
    til::CoordType columnEndDirty = 0; // OUT
};

struct RowCopyTextFromState
{
    // The row to copy text from.
    const ROW& source; // IN
    // The column at which to start writing.
    til::CoordType columnBegin = 0; // IN
    // The first column which should not be written to anymore.
    til::CoordType columnLimit = til::CoordTypeMax; // IN
    // The column at which to start reading from source.
    til::CoordType sourceColumnBegin = 0; // IN
    // The first column which should not be read from anymore.
    til::CoordType sourceColumnLimit = til::CoordTypeMax; // IN

    til::CoordType columnEnd = 0; // OUT
    // The first column that got modified by this write operation. In case that the first glyph we write overwrites
    // the trailing half of a wide glyph, leadingSpaces will be 1 and this value will be 1 less than colBeg.
    til::CoordType columnBeginDirty = 0; // OUT
    // This is 1 past the last column that was modified and will be 1 past columnEnd if we overwrote
    // the leading half of a wide glyph and had to fill the trailing half with whitespace.
    til::CoordType columnEndDirty = 0; // OUT
    // This is 1 past the last column that was read from.
    til::CoordType sourceColumnEnd = 0; // OUT
};

// This structure is basically an inverse of ROW::_charOffsets. If you have a pointer
// into a ROW's text this class can tell you what cell that pointer belongs to.
struct CharToColumnMapper
{
    CharToColumnMapper(const wchar_t* chars, const uint16_t* charOffsets, ptrdiff_t lastCharOffset, til::CoordType currentColumn) noexcept;

    til::CoordType GetLeadingColumnAt(ptrdiff_t targetOffset) noexcept;
    til::CoordType GetTrailingColumnAt(ptrdiff_t offset) noexcept;
    til::CoordType GetLeadingColumnAt(const wchar_t* str) noexcept;
    til::CoordType GetTrailingColumnAt(const wchar_t* str) noexcept;

private:
    // See ROW and its members with identical name.
    static constexpr uint16_t CharOffsetsTrailer = 0x8000;
    static constexpr uint16_t CharOffsetsMask = 0x7fff;

    const wchar_t* _chars;
    const uint16_t* _charOffsets;
    ptrdiff_t _lastCharOffset;
    til::CoordType _currentColumn;
};

#if 0
class ROW final
{
public:
    // The implicit agreement between ROW and TextBuffer is that the `charsBuffer` and `charOffsetsBuffer`
    // arrays have a minimum alignment of 16 Bytes and a size of `rowWidth+1`. The former is used to
    // implement Reset() efficiently via SIMD and the latter is used to store the past-the-end offset
    // into the `charsBuffer`. Even though the `charsBuffer` could be only `rowWidth` large we need them
    // to be the same size so that the SIMD code can process both arrays in the same loop simultaneously.
    // This wastes up to 5.8% memory but increases overall scrolling performance by around 40%.
    // These methods exists to make this agreement explicit and serve as a reminder.
    //
    // TextBuffer calculates the distance in bytes between two ROWs (_bufferRowStride) as the sum of these values.
    // As such it's important that we return sizes with a minimum alignment of alignof(ROW).
    static constexpr size_t CalculateRowSize() noexcept
    {
        return (sizeof(ROW) + 15) & ~15;
    }
    static constexpr size_t CalculateCharsBufferSize(size_t columns) noexcept
    {
        return (columns * sizeof(wchar_t) + 16) & ~15;
    }
    static constexpr size_t CalculateCharOffsetsBufferSize(size_t columns) noexcept
    {
        return (columns * sizeof(uint16_t) + 16) & ~15;
    }

    ROW() = default;
    ROW(wchar_t* charsBuffer, uint16_t* charOffsetsBuffer, uint16_t rowWidth, const TextAttribute& fillAttribute);

    ROW(const ROW& other) = delete;
    ROW& operator=(const ROW& other) = delete;

    ROW(ROW&& other) = default;
    ROW& operator=(ROW&& other) = default;

    void SetWrapForced(const bool wrap) noexcept;
    bool WasWrapForced() const noexcept;
    void SetDoubleBytePadded(const bool doubleBytePadded) noexcept;
    bool WasDoubleBytePadded() const noexcept;
    void SetLineRendition(const LineRendition lineRendition) noexcept;
    LineRendition GetLineRendition() const noexcept;
    til::CoordType GetReadableColumnCount() const noexcept;

    void Reset(const TextAttribute& attr) noexcept;
    void CopyFrom(const ROW& source);

    til::CoordType NavigateToPrevious(til::CoordType column) const noexcept;
    til::CoordType NavigateToNext(til::CoordType column) const noexcept;
    til::CoordType AdjustToGlyphStart(til::CoordType column) const noexcept;
    til::CoordType AdjustToGlyphEnd(til::CoordType column) const noexcept;

    void ClearCell(til::CoordType column);
    OutputCellIterator WriteCells(OutputCellIterator it, til::CoordType columnBegin, std::optional<bool> wrap = std::nullopt, std::optional<til::CoordType> limitRight = std::nullopt);
    void SetAttrToEnd(til::CoordType columnBegin, TextAttribute attr);
    void ReplaceAttributes(til::CoordType beginIndex, til::CoordType endIndex, const TextAttribute& newAttr);
    void ReplaceCharacters(til::CoordType columnBegin, til::CoordType width, const std::wstring_view& chars);
    void ReplaceText(RowWriteState& state);
    void CopyTextFrom(RowCopyTextFromState& state);

    til::small_rle<TextAttribute, uint16_t, 1>& Attributes() noexcept;
    const til::small_rle<TextAttribute, uint16_t, 1>& Attributes() const noexcept;
    TextAttribute GetAttrByColumn(til::CoordType column) const;
    std::vector<uint16_t> GetHyperlinks() const;
    ImageSlice* SetImageSlice(ImageSlice::Pointer imageSlice) noexcept;
    const ImageSlice* GetImageSlice() const noexcept;
    ImageSlice* GetMutableImageSlice() noexcept;
    uint16_t size() const noexcept;
    til::CoordType GetLastNonSpaceColumn() const noexcept;
    til::CoordType MeasureLeft() const noexcept;
    til::CoordType MeasureRight() const noexcept;
    bool ContainsText() const noexcept;
    std::wstring_view GlyphAt(til::CoordType column) const noexcept;
    DbcsAttribute DbcsAttrAt(til::CoordType column) const noexcept;
    std::wstring_view GetText() const noexcept;
    std::wstring_view GetText(til::CoordType columnBegin, til::CoordType columnEnd) const noexcept;
    til::CoordType GetLeadingColumnAtCharOffset(ptrdiff_t offset) const noexcept;
    til::CoordType GetTrailingColumnAtCharOffset(ptrdiff_t offset) const noexcept;
    DelimiterClass DelimiterClassAt(til::CoordType column, const std::wstring_view& wordDelimiters) const noexcept;

    auto AttrBegin() const noexcept { return _attr.begin(); }
    auto AttrEnd() const noexcept { return _attr.end(); }

    const std::optional<ScrollbarData>& GetScrollbarData() const noexcept;
    void SetScrollbarData(std::optional<ScrollbarData> data) noexcept;
    void StartPrompt() noexcept;
    void EndOutput(std::optional<unsigned int> error) noexcept;

#ifdef UNIT_TESTING
    friend constexpr bool operator==(const ROW& a, const ROW& b) noexcept;
    friend class RowTests;
#endif

private:
    // WriteHelper exists because other forms of abstracting this functionality away (like templates with lambdas)
    // where only very poorly optimized by MSVC as it failed to inline the templates.
    struct WriteHelper
    {
        explicit WriteHelper(ROW& row, til::CoordType columnBegin, til::CoordType columnLimit, const std::wstring_view& chars) noexcept;
        bool IsValid() const noexcept;
        void ReplaceCharacters(til::CoordType width) noexcept;
        void ReplaceText() noexcept;
        void _replaceTextUnicode(size_t ch, std::wstring_view::const_iterator it) noexcept;
        void CopyTextFrom(const std::span<const uint16_t>& charOffsets) noexcept;
        static void _copyOffsets(uint16_t* dst, const uint16_t* src, uint16_t size, uint16_t offset) noexcept;
        void Finish();

        // Parent pointer.
        ROW& row;
        // The text given by the caller.
        const std::wstring_view& chars;

        // This is the same as the columnBegin parameter for ReplaceText(), etc.,
        // but clamped to a valid range via _clampedColumnInclusive.
        uint16_t colBeg;
        // This is the same as the columnLimit parameter for ReplaceText(), etc.,
        // but clamped to a valid range via _clampedColumnInclusive.
        uint16_t colLimit;

        // The column 1 past the last glyph that was successfully written into the row. If you need to call
        // ReplaceAttributes() to colorize the written range, etc., this is the columnEnd parameter you want.
        // If you want to continue writing where you left off, this is also the next columnBegin parameter.
        uint16_t colEnd;
        // The first column that got modified by this write operation. In case that the first glyph we write overwrites
        // the trailing half of a wide glyph, leadingSpaces will be 1 and this value will be 1 less than colBeg.
        uint16_t colBegDirty;
        // Similar to dirtyBeg, this is 1 past the last column that was modified and will be 1 past colEnd if
        // we overwrote the leading half of a wide glyph and had to fill the trailing half with whitespace.
        uint16_t colEndDirty;
        // The offset in ROW::chars at which we start writing the contents of WriteHelper::chars.
        uint16_t chBeg;
        // The offset at which we start writing leadingSpaces-many whitespaces.
        uint16_t chBegDirty;
        // The same as `colBeg - colBegDirty`. This is the amount of whitespace
        // we write at chBegDirty, before the actual WriteHelper::chars content.
        uint16_t leadingSpaces;
        // The amount of characters copied from WriteHelper::chars.
        size_t charsConsumed;
    };

    // To simplify the detection of wide glyphs, we don't just store the simple character offset as described
    // for _charOffsets. Instead we use the most significant bit to indicate whether any column is the
    // trailing half of a wide glyph. This simplifies many implementation details via _uncheckedIsTrailer.
    static constexpr uint16_t CharOffsetsTrailer = 0x8000;
    static constexpr uint16_t CharOffsetsMask = 0x7fff;

    template<typename T>
    constexpr uint16_t _clampedColumn(T v) const noexcept;
    template<typename T>
    constexpr uint16_t _clampedColumnInclusive(T v) const noexcept;

    uint16_t _charSize() const noexcept;
    template<typename T>
    wchar_t _uncheckedChar(T off) const noexcept;
    template<typename T>
    uint16_t _uncheckedCharOffset(T col) const noexcept;
    template<typename T>
    bool _uncheckedIsTrailer(T col) const noexcept;
    template<typename T>
    T _adjustBackward(T column) const noexcept;
    template<typename T>
    T _adjustForward(T column) const noexcept;

    void _init() noexcept;
    void _resizeChars(uint16_t colEndDirty, uint16_t chBegDirty, size_t chEndDirty, uint16_t chEndDirtyOld);
    CharToColumnMapper _createCharToColumnMapper(ptrdiff_t offset) const noexcept;

    // These fields are a bit "wasteful", but it makes all this a bit more robust against
    // programming errors during initial development (which is when this comment was written).
    // * _chars and _charsHeap are redundant
    //   If _charsHeap is stored in _chars, we can still infer that
    //   _chars was allocated on the heap if _chars != _charsBuffer.
    // * _chars doesn't need a size_t size()
    //   The size may never exceed an uint16_t anyways.
    // * _charOffsets doesn't need a size() at all
    //   The length is already stored in _columns.

    // Most text uses only a single wchar_t per codepoint / grapheme cluster.
    // That's why TextBuffer allocates a large blob of virtual memory which we can use as
    // a simplified chars buffer, without having to allocate any additional heap memory.
    // _charsBuffer fits _columnCount characters at most.
    wchar_t* _charsBuffer = nullptr;
    // ...but if this ROW needs to store more than _columnCount characters
    // then it will allocate a larger string on the heap and store it here.
    // The capacity of this string on the heap is stored in _chars.size().
    std::unique_ptr<wchar_t[]> _charsHeap;
    // _chars either refers to our _charsBuffer or _charsHeap, defaulting to the former.
    // _chars.size() is NOT the length of the string, but rather its capacity.
    // _charOffsets[_columnCount] stores the length.
    std::span<wchar_t> _chars;
    // _charOffsets accelerates indexing into the above _chars string given a terminal column,
    // by storing the character index/offset at which a column's text in _chars starts.
    // It stores 1 more item than this row is wide, allowing it to store the
    // past-the-end offset, which is thus equal to the length of the string.
    //
    // For instance given a 4 column ROW containing "abcd" it would store 01234,
    // because each of "abcd" are 1 column wide and 1 wchar_t per column.
    // Given "a\u732Bd" it would store 01123, because "\u732B" is a wide glyph
    // and "11" indicates that both column 1 and 2 start at &_chars[1] (= wide glyph).
    // The fact that the next offset is 2 tells us that the glyph is 1 wchar_t long.
    // Given "a\uD83D\uDE00d" ("\uD83D\uDE00" is an Emoji) it would store 01134,
    // because while it's 2 cells wide as indicated by 2 offsets that are identical (11),
    // the next offset is 3, which indicates that the glyph is 3-1 = 2 wchar_t long.
    //
    // In other words, _charOffsets tells us both the width in chars and width in columns.
    // See CharOffsetsTrailer for more information.
    std::span<uint16_t> _charOffsets;
    // _attr is a run-length-encoded vector of TextAttribute with a decompressed
    // length equal to _columnCount (= 1 TextAttribute per column).
    til::small_rle<TextAttribute, uint16_t, 1> _attr;
    // The width of the row in visual columns.
    uint16_t _columnCount = 0;
    // Stores double-width/height (DECSWL/DECDWL/DECDHL) attributes.
    LineRendition _lineRendition = LineRendition::SingleWidth;
    // Occurs when the user runs out of text in a given row and we're forced to wrap the cursor to the next line
    bool _wrapForced = false;
    // Occurs when the user runs out of text to support a double byte character and we're forced to the next line
    bool _doubleBytePadded = false;

    std::optional<ScrollbarData> _promptData = std::nullopt;

    // Stores any image content covering the row.
    ImageSlice::Pointer _imageSlice;
};

#ifdef UNIT_TESTING
constexpr bool operator==(const ROW& a, const ROW& b) noexcept
{
    // comparison is only used in the tests; this should suffice.
    return a._charsBuffer == b._charsBuffer;
}
#endif


#if 0
class TextBuffer final
{
public:
    TextBuffer(const til::size screenBufferSize,
        const TextAttribute defaultAttributes,
        const UINT cursorSize,
        const bool isActiveBuffer,
        Microsoft::Console::Render::Renderer* renderer);

    TextBuffer(const TextBuffer&) = delete;
    TextBuffer(TextBuffer&&) = delete;
    TextBuffer& operator=(const TextBuffer&) = delete;
    TextBuffer& operator=(TextBuffer&&) = delete;

    ~TextBuffer();

    // Used for duplicating properties to another text buffer
    void CopyProperties(const TextBuffer& OtherBuffer) noexcept;

    // row manipulation
    ROW& GetScratchpadRow();
    ROW& GetScratchpadRow(const TextAttribute& attributes);
    const ROW& GetRowByOffset(til::CoordType index) const;
    ROW& GetMutableRowByOffset(til::CoordType index);

    TextBufferCellIterator GetCellDataAt(const til::point at) const;
    TextBufferCellIterator GetCellLineDataAt(const til::point at) const;
    TextBufferCellIterator GetCellDataAt(const til::point at, const Microsoft::Console::Types::Viewport limit) const;
    TextBufferTextIterator GetTextDataAt(const til::point at) const;
    TextBufferTextIterator GetTextLineDataAt(const til::point at) const;
    TextBufferTextIterator GetTextDataAt(const til::point at, const Microsoft::Console::Types::Viewport limit) const;

    size_t GetCellDistance(const til::point from, const til::point to) const;

    static size_t GraphemeNext(const std::wstring_view& chars, size_t position) noexcept;
    static size_t GraphemePrev(const std::wstring_view& chars, size_t position) noexcept;
    static size_t FitTextIntoColumns(const std::wstring_view& chars, til::CoordType columnLimit, til::CoordType& columns) noexcept;

    til::point NavigateCursor(til::point position, til::CoordType distance) const;

    // Text insertion functions
    void Replace(til::CoordType row, const TextAttribute& attributes, RowWriteState& state);
    void Insert(til::CoordType row, const TextAttribute& attributes, RowWriteState& state);
    void FillRect(const til::rect& rect, const std::wstring_view& fill, const TextAttribute& attributes);

    OutputCellIterator Write(const OutputCellIterator givenIt);

    OutputCellIterator Write(const OutputCellIterator givenIt,
        const til::point target,
        const std::optional<bool> wrap = true);

    OutputCellIterator WriteLine(const OutputCellIterator givenIt,
        const til::point target,
        const std::optional<bool> setWrap = std::nullopt,
        const std::optional<til::CoordType> limitRight = std::nullopt);

    // Scroll needs access to this to quickly rotate around the buffer.
    void IncrementCircularBuffer(const TextAttribute& fillAttributes = {});

    til::point GetLastNonSpaceCharacter(const Microsoft::Console::Types::Viewport* viewOptional = nullptr) const;

    Cursor& GetCursor() noexcept;
    const Cursor& GetCursor() const noexcept;

    uint64_t GetLastMutationId() const noexcept;
    const til::CoordType GetFirstRowIndex() const noexcept;

    const Microsoft::Console::Types::Viewport GetSize() const noexcept;

    void ScrollRows(const til::CoordType firstRow, const til::CoordType size, const til::CoordType delta);
    void CopyRow(const til::CoordType srcRow, const til::CoordType dstRow, TextBuffer& dstBuffer) const;

    til::CoordType TotalRowCount() const noexcept;

    const TextAttribute& GetCurrentAttributes() const noexcept;

    void SetCurrentAttributes(const TextAttribute& currentAttributes) noexcept;

    void SetWrapForced(til::CoordType y, bool wrap);
    void SetCurrentLineRendition(const LineRendition lineRendition, const TextAttribute& fillAttributes);
    void ResetLineRenditionRange(const til::CoordType startRow, const til::CoordType endRow);
    LineRendition GetLineRendition(const til::CoordType row) const;
    bool IsDoubleWidthLine(const til::CoordType row) const;

    til::CoordType GetLineWidth(const til::CoordType row) const;
    til::point ClampPositionWithinLine(const til::point position) const;
    til::point ScreenToBufferPosition(const til::point position) const;
    til::point BufferToScreenPosition(const til::point position) const;

    void Reset() noexcept;
    void ClearScrollback(const til::CoordType start, const til::CoordType height);

    void ResizeTraditional(const til::size newSize);

    void SetAsActiveBuffer(const bool isActiveBuffer) noexcept;
    bool IsActiveBuffer() const noexcept;

    Microsoft::Console::Render::Renderer* GetRenderer() noexcept;

    void NotifyPaintFrame() noexcept;
    void TriggerRedraw(const Microsoft::Console::Types::Viewport& viewport);
    void TriggerRedrawAll();
    void TriggerScroll();
    void TriggerScroll(const til::point delta);
    void TriggerNewTextNotification(const std::wstring_view newText);

    til::point GetWordStart(const til::point target, const std::wstring_view wordDelimiters, bool accessibilityMode = false, std::optional<til::point> limitOptional = std::nullopt) const;
    til::point GetWordEnd(const til::point target, const std::wstring_view wordDelimiters, bool accessibilityMode = false, std::optional<til::point> limitOptional = std::nullopt) const;
    bool MoveToNextWord(til::point& pos, const std::wstring_view wordDelimiters, std::optional<til::point> limitOptional = std::nullopt) const;
    bool MoveToPreviousWord(til::point& pos, const std::wstring_view wordDelimiters) const;

    til::point GetGlyphStart(const til::point pos, std::optional<til::point> limitOptional = std::nullopt) const;
    til::point GetGlyphEnd(const til::point pos, bool accessibilityMode = false, std::optional<til::point> limitOptional = std::nullopt) const;
    bool MoveToNextGlyph(til::point& pos, bool allowBottomExclusive = false, std::optional<til::point> limitOptional = std::nullopt) const;
    bool MoveToPreviousGlyph(til::point& pos, std::optional<til::point> limitOptional = std::nullopt) const;

    const std::vector<til::inclusive_rect> GetTextRects(til::point start, til::point end, bool blockSelection, bool bufferCoordinates) const;
    std::vector<til::point_span> GetTextSpans(til::point start, til::point end, bool blockSelection, bool bufferCoordinates) const;

    void AddHyperlinkToMap(std::wstring_view uri, uint16_t id);
    std::wstring GetHyperlinkUriFromId(uint16_t id) const;
    uint16_t GetHyperlinkId(std::wstring_view uri, std::wstring_view id);
    void RemoveHyperlinkFromMap(uint16_t id) noexcept;
    std::wstring GetCustomIdFromId(uint16_t id) const;
    void CopyHyperlinkMaps(const TextBuffer& OtherBuffer);

    size_t SpanLength(const til::point coordStart, const til::point coordEnd) const;

    std::wstring GetPlainText(til::point start, til::point end) const;

    struct CopyRequest
    {
        // beg and end coordinates are inclusive
        til::point beg;
        til::point end;

        til::CoordType minX;
        til::CoordType maxX;
        bool blockSelection = false;
        bool trimTrailingWhitespace = true;
        bool includeLineBreak = true;
        bool formatWrappedRows = false;

        // whether beg, end coordinates are in buffer coordinates or screen coordinates
        bool bufferCoordinates = false;

        CopyRequest() = default;

        constexpr CopyRequest(const TextBuffer& buffer, const til::point& beg, const til::point& end, const bool blockSelection, const bool includeLineBreak, const bool trimTrailingWhitespace, const bool formatWrappedRows, const bool bufferCoordinates = false) noexcept :
            beg{ std::max(beg, til::point{ 0, 0 }) },
            end{ std::min(end, til::point{ buffer._width - 1, buffer._height - 1 }) },
            minX{ std::min(this->beg.x, this->end.x) },
            maxX{ std::max(this->beg.x, this->end.x) },
            blockSelection{ blockSelection },
            includeLineBreak{ includeLineBreak },
            trimTrailingWhitespace{ trimTrailingWhitespace },
            formatWrappedRows{ formatWrappedRows },
            bufferCoordinates{ bufferCoordinates }
        {
        }

        static CopyRequest FromConfig(const TextBuffer& buffer,
            const til::point& beg,
            const til::point& end,
            const bool singleLine,
            const bool blockSelection,
            const bool trimBlockSelection,
            const bool bufferCoordinates = false) noexcept
        {
            return {
                buffer,
                beg,
                end,
                blockSelection,

                /* includeLineBreak */
                // - SingleLine mode collapses all rows into one line, unless we're in
                //   block selection mode.
                // - Block selection should preserve the visual structure by including
                //   line breaks on all rows (together with `formatWrappedRows`).
                //   (Selects like a box, pastes like a box)
                !singleLine || blockSelection,

                /* trimTrailingWhitespace */
                // Trim trailing whitespace if we're not in single line mode and — either
                // we're not in block selection mode or, we're in block selection mode and
                // trimming is allowed.
                !singleLine && (!blockSelection || trimBlockSelection),

                /* formatWrappedRows */
                // In block selection, we should apply formatting to wrapped rows as well.
                // (Otherwise, they're only applied to non-wrapped rows.)
                blockSelection,

                bufferCoordinates
            };
        }
    };

    std::wstring GetPlainText(const CopyRequest& req) const;

    std::string GenHTML(const CopyRequest& req,
        const int fontHeightPoints,
        const std::wstring_view fontFaceName,
        const COLORREF backgroundColor,
        const bool isIntenseBold,
        std::function<std::tuple<COLORREF, COLORREF, COLORREF>(const TextAttribute&)> GetAttributeColors) const noexcept;

    std::string GenRTF(const CopyRequest& req,
        const int fontHeightPoints,
        const std::wstring_view fontFaceName,
        const COLORREF backgroundColor,
        const bool isIntenseBold,
        std::function<std::tuple<COLORREF, COLORREF, COLORREF>(const TextAttribute&)> GetAttributeColors) const noexcept;

    void Serialize(const wchar_t* destination) const;

    struct PositionInformation
    {
        til::CoordType mutableViewportTop{ 0 };
        til::CoordType visibleViewportTop{ 0 };
    };

    static void Reflow(TextBuffer& oldBuffer, TextBuffer& newBuffer, const Microsoft::Console::Types::Viewport* lastCharacterViewport = nullptr, PositionInformation* positionInfo = nullptr);

    std::optional<std::vector<til::point_span>> SearchText(const std::wstring_view& needle, SearchFlag flags) const;
    std::optional<std::vector<til::point_span>> SearchText(const std::wstring_view& needle, SearchFlag flags, til::CoordType rowBeg, til::CoordType rowEnd) const;

    // Mark handling
    std::vector<ScrollMark> GetMarkRows() const;
    std::vector<MarkExtents> GetMarkExtents(size_t limit = SIZE_T_MAX) const;
    void ClearMarksInRange(const til::point start, const til::point end);
    void ClearAllMarks();
    std::wstring CurrentCommand() const;
    std::vector<std::wstring> Commands() const;
    void StartPrompt();
    bool StartCommand();
    bool StartOutput();
    void EndCurrentCommand(std::optional<unsigned int> error);
    void SetScrollbarData(ScrollbarData mark, til::CoordType y);
    void ManuallyMarkRowAsPrompt(til::CoordType y);

private:
    void _reserve(til::size screenBufferSize, const TextAttribute& defaultAttributes);
    void _commit(const std::byte* row);
    void _decommit() noexcept;
    void _construct(const std::byte* until) noexcept;
    void _destroy() const noexcept;
    ROW& _getRowByOffsetDirect(size_t offset);
    ROW& _getRow(til::CoordType y) const;
    til::CoordType _estimateOffsetOfLastCommittedRow() const noexcept;

    void _SetFirstRowIndex(const til::CoordType FirstRowIndex) noexcept;
    void _ExpandTextRow(til::inclusive_rect& selectionRow) const;
    DelimiterClass _GetDelimiterClassAt(const til::point pos, const std::wstring_view wordDelimiters) const;
    til::point _GetWordStartForAccessibility(const til::point target, const std::wstring_view wordDelimiters) const;
    til::point _GetWordStartForSelection(const til::point target, const std::wstring_view wordDelimiters) const;
    til::point _GetWordEndForAccessibility(const til::point target, const std::wstring_view wordDelimiters, const til::point limit) const;
    til::point _GetWordEndForSelection(const til::point target, const std::wstring_view wordDelimiters) const;
    void _PruneHyperlinks();

    std::wstring _commandForRow(const til::CoordType rowOffset, const til::CoordType bottomInclusive) const;
    MarkExtents _scrollMarkExtentForRow(const til::CoordType rowOffset, const til::CoordType bottomInclusive) const;
    bool _createPromptMarkIfNeeded();

    std::tuple<til::CoordType, til::CoordType, bool> _RowCopyHelper(const CopyRequest& req, const til::CoordType iRow, const ROW& row) const;

    static void _AppendRTFText(std::string& contentBuilder, const std::wstring_view& text);

    Microsoft::Console::Render::Renderer* _renderer = nullptr;

    std::unordered_map<uint16_t, std::wstring> _hyperlinkMap;
    std::unordered_map<std::wstring, uint16_t> _hyperlinkCustomIdMap;
    uint16_t _currentHyperlinkId = 1;

    // This block describes the state of the underlying virtual memory buffer that holds all ROWs, text and attributes.
    // Initially memory is only allocated with MEM_RESERVE to reduce the private working set of conhost.
    // ROWs are laid out like this in memory:
    //   ROW                <-- sizeof(ROW), stores
    //   (padding)
    //   ROW::_charsBuffer  <-- _width * sizeof(wchar_t)
    //   (padding)
    //   ROW::_charOffsets  <-- (_width + 1) * sizeof(uint16_t)
    //   (padding)
    //   ...
    // Padding may exist for alignment purposes.
    //
    // The base (start) address of the memory arena.
    wil::unique_virtualalloc_ptr<std::byte> _buffer;
    // The past-the-end pointer of the memory arena.
    std::byte* _bufferEnd = nullptr;
    // The range between _buffer (inclusive) and _commitWatermark (exclusive) is the range of
    // memory that has already been committed via MEM_COMMIT and contains ready-to-use ROWs.
    //
    // The problem is that calling VirtualAlloc(MEM_COMMIT) on each ROW one by one is extremely expensive, which forces
    // us to commit ROWs in batches and avoid calling it on already committed ROWs. Let's say we commit memory in
    // batches of 128 ROWs. One option to know whether a ROW has already been committed is to allocate a vector<uint8_t>
    // of size `(height + 127) / 128` and mark the corresponding slot as 1 if that 128-sized batch has been committed.
    // That way we know not to commit it again. But ROWs aren't accessed randomly. Instead, they're usually accessed
    // fairly linearly from row 1 to N. As such we can just commit ROWs up to the point of the highest accessed ROW
    // plus some read-ahead of 128 ROWs. This is exactly what _commitWatermark stores: The highest accessed ROW plus
    // some read-ahead. It's the amount of memory that has been committed and is ready to use.
    //
    // _commitWatermark will always be a multiple of _bufferRowStride away from _buffer.
    // In other words, _commitWatermark itself will either point exactly onto the next ROW
    // that should be committed or be equal to _bufferEnd when all ROWs are committed.
    std::byte* _commitWatermark = nullptr;
    // This will MEM_COMMIT 128 rows more than we need, to avoid us from having to call VirtualAlloc too often.
    // This equates to roughly the following commit chunk sizes at these column counts:
    // *  80 columns (the usual minimum) =  60KB chunks,  4.1MB buffer at 9001 rows
    // * 120 columns (the most common)   =  80KB chunks,  5.6MB buffer at 9001 rows
    // * 400 columns (the usual maximum) = 220KB chunks, 15.5MB buffer at 9001 rows
    // There's probably a better metric than this. (This comment was written when ROW had both,
    // a _chars array containing text and a _charOffsets array contain column-to-text indices.)
    static constexpr size_t _commitReadAheadRowCount = 128;
    // Before TextBuffer was made to use virtual memory it initialized the entire memory arena with the initial
    // attributes right away. To ensure it continues to work the way it used to, this stores these initial attributes.
    TextAttribute _initialAttributes;
    // ROW ---------------+--+--+
    // (padding)          |  |  v _bufferOffsetChars
    // ROW::_charsBuffer  |  |
    // (padding)          |  v _bufferOffsetCharOffsets
    // ROW::_charOffsets  |
    // (padding)          v _bufferRowStride
    size_t _bufferRowStride = 0;
    size_t _bufferOffsetChars = 0;
    size_t _bufferOffsetCharOffsets = 0;
    // The width of the buffer in columns.
    uint16_t _width = 0;
    // The height of the buffer in rows, excluding the scratchpad row.
    uint16_t _height = 0;

    TextAttribute _currentAttributes;
    til::CoordType _firstRow = 0; // indexes top row (not necessarily 0)
    uint64_t _lastMutationId = 0;

    Cursor _cursor;
    bool _isActiveBuffer = false;

#ifdef UNIT_TESTING
    friend class TextBufferTests;
    friend class UiaTextRangeTests;
#endif
};
#endif 
#endif 