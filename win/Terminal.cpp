#include "pch.h"
#include "Terminal.h"

#if 0
#include <til/bit.h>
#include <til/unicode.h>
// clang-format off

// A table mapping 8-bit RGB colors, in the form RRRGGGBB,
// down to one of the 16 colors in the legacy palette.
constexpr std::array<BYTE, 256> CompressedRgbToIndex16 = {
     0,  1,  1,  9,  0,  0,  1,  1,  2,  1,  1,  1,  2,  8,  1,  9,
     2,  2,  3,  3,  2,  2, 11,  3, 10, 10, 11, 11, 10, 10, 10, 11,
     0,  5,  1,  1,  0,  0,  1,  1,  8,  1,  1,  1,  2,  8,  1,  9,
     2,  2,  3,  3,  2,  2, 11,  3, 10, 10, 10, 11, 10, 10, 10, 11,
     5,  5,  5,  1,  4,  5,  1,  1,  8,  8,  1,  9,  2,  8,  9,  9,
     2,  2,  3,  3,  2,  2, 11,  3, 10, 10, 11, 11, 10, 10, 10, 11,
     4,  5,  5,  1,  4,  5,  5,  1,  8,  5,  5,  1,  8,  8,  9,  9,
     2,  2,  8,  9, 10,  2, 11,  3, 10, 10, 11, 11, 10, 10, 10, 11,
     4, 13,  5,  5,  4, 13,  5,  5,  4, 13, 13, 13,  6,  8, 13,  9,
     6,  8,  8,  9, 10, 10, 11,  3, 10, 10, 11, 11, 10, 10, 10, 11,
     4, 13, 13, 13,  4, 13, 13, 13,  4, 12, 13, 13,  6, 12, 13, 13,
     6,  6,  8,  9,  6,  6,  7,  7, 10, 14, 14,  7, 10, 10, 14, 11,
     4, 12, 13, 13,  4, 12, 13, 13,  4, 12, 13, 13,  6, 12, 12, 13,
     6,  6, 12,  7,  6,  6,  7,  7,  6, 14, 14,  7, 14, 14, 14, 15,
    12, 12, 13, 13, 12, 12, 13, 13, 12, 12, 12, 13, 12, 12, 12, 13,
     6, 12, 12,  7,  6,  6,  7,  7,  6, 14, 14,  7, 14, 14, 14, 15
};

// A table mapping indexed colors from the 256-color palette,
// down to one of the 16 colors in the legacy palette.
constexpr std::array<BYTE, 256> Index256ToIndex16 = {
     0,  4,  2,  6,  1,  5,  3,  7,  8, 12, 10, 14,  9, 13, 11, 15,
     0,  1,  1,  1,  9,  9,  2,  1,  1,  1,  1,  1,  2,  2,  3,  3,
     3,  3,  2,  2, 11, 11,  3,  3, 10, 10, 11, 11, 11, 11, 10, 10,
    10, 10, 11, 11,  5,  5,  5,  5,  1,  1,  8,  8,  1,  1,  9,  9,
     2,  2,  3,  3,  3,  3,  2,  2, 11, 11,  3,  3, 10, 10, 11, 11,
    11, 11, 10, 10, 10, 10, 11, 11,  4, 13,  5,  5,  5,  5,  4, 13,
    13, 13, 13, 13,  6,  8,  8,  8,  9,  9, 10, 10, 11, 11,  3,  3,
    10, 10, 11, 11, 11, 11, 10, 10, 10, 10, 11, 11,  4, 13, 13, 13,
    13, 13,  4, 12, 13, 13, 13, 13,  6,  6,  8,  8,  9,  9,  6,  6,
     7,  7,  7,  7, 10, 14, 14, 14,  7,  7, 10, 10, 14, 14, 11, 11,
     4, 12, 13, 13, 13, 13,  4, 12, 13, 13, 13, 13,  6,  6, 12, 12,
     7,  7,  6,  6,  7,  7,  7,  7,  6, 14, 14, 14,  7,  7, 14, 14,
    14, 14, 15, 15, 12, 12, 13, 13, 13, 13, 12, 12, 12, 12, 13, 13,
     6, 12, 12, 12,  7,  7,  6,  6,  7,  7,  7,  7,  6, 14, 14, 14,
     7,  7, 14, 14, 14, 14, 15, 15,  0,  0,  0,  0,  0,  0,  8,  8,
     8,  8,  8,  8,  8,  8,  8,  8,  7,  7,  7,  7,  7,  7, 15, 15
};

// clang-format on
// We should only need 4B for TextColor. Any more than that is just waste.
static_assert(sizeof(TextColor) == 4);
// Assert that the use of memcmp() for comparisons is safe.
static_assert(std::has_unique_object_representations_v<TextColor>);

bool TextColor::CanBeBrightened() const noexcept
{
    return IsIndex16() || IsDefault();
}

ColorType TextColor::GetType() const noexcept
{
    return _meta;
}

bool TextColor::IsLegacy() const noexcept
{
    return (IsIndex16() || IsIndex256()) && _index < 16;
}

bool TextColor::IsIndex16() const noexcept
{
    return _meta == ColorType::IsIndex16;
}

bool TextColor::IsIndex256() const noexcept
{
    return _meta == ColorType::IsIndex256;
}

bool TextColor::IsDefault() const noexcept
{
    return _meta == ColorType::IsDefault;
}

bool TextColor::IsDefaultOrLegacy() const noexcept
{
    return _meta != ColorType::IsRgb && _index < 16;
}

bool TextColor::IsRgb() const noexcept
{
    return _meta == ColorType::IsRgb;
}

// Method Description:
// - Sets the color value of this attribute, and sets this color to be an RGB
//      attribute.
// Arguments:
// - rgbColor: the COLORREF containing the color information for this TextColor
// Return Value:
// - <none>
void TextColor::SetColor(const COLORREF rgbColor) noexcept
{
    _meta = ColorType::IsRgb;
    _red = GetRValue(rgbColor);
    _green = GetGValue(rgbColor);
    _blue = GetBValue(rgbColor);
}

// Method Description:
// - Sets this TextColor to be a legacy-style index into the color table.
// Arguments:
// - index: the index of the colortable we should use for this TextColor.
// - isIndex256: is this a 256 color index (true) or a 16 color index (false).
// Return Value:
// - <none>
void TextColor::SetIndex(const BYTE index, const bool isIndex256) noexcept
{
    _meta = isIndex256 ? ColorType::IsIndex256 : ColorType::IsIndex16;
    _index = index;
    _green = 0;
    _blue = 0;
}

// Method Description:
// - Sets this TextColor to be a default text color, who's appearance is
//      controlled by the terminal's implementation of what a default color is.
// Arguments:
// - <none>
// Return Value:
// - <none>
void TextColor::SetDefault() noexcept
{
    _meta = ColorType::IsDefault;
    _red = 0;
    _green = 0;
    _blue = 0;
}

// Method Description:
// - Retrieve the real color value for this TextColor.
//   * If we're an RGB color, we'll use that value.
//   * If we're an indexed color table value, we'll use that index to look up
//     our value in the provided color table.
//     - If brighten is true, and we've got a 16 color index in the "dark"
//       portion of the color table (indices [0,7]), then we'll look up the
//       bright version of this color (from indices [8,15]). This should be
//       true for TextAttributes that are "intense" and we're treating intense
//       as bright (which is the default behavior of most terminals.)
//   * If we're a default color, we'll return the default color provided.
// Arguments:
// - colorTable: The table of colors we should use to look up the value of
//      an indexed attribute from.
// - defaultIndex: The color table index to use if we're a default attribute.
// - brighten: if true, we'll brighten a dark color table index.
// Return Value:
// - a COLORREF containing the real value of this TextColor.
COLORREF TextColor::GetColor(const std::array<COLORREF, TextColor::TABLE_SIZE>& colorTable, const size_t defaultIndex, bool brighten) const noexcept
{
    if (IsDefault())
    {
        const auto defaultColor = til::at(colorTable, defaultIndex);

        if (brighten)
        {
            // See MSFT:20266024 for context on this fix.
            //      Additionally todo MSFT:20271956 to fix this better for 19H2+
            // If we're a default color, check to see if the defaultColor exists
            // in the dark section of the color table. If it does, then chances
            // are we're not a separate default color, instead we're an index
            //      color being used as the default color
            //      (Settings::_DefaultForeground==INVALID_COLOR, and the index
            //      from _wFillAttribute is being used instead.)
            // If we find a match, return instead the bright version of this color

            static_assert(sizeof(COLORREF) * 8 == 32, "The vectorized code broke. If you can't fix COLORREF, just remove the vectorized code.");

#pragma warning(push)
#pragma warning(disable : 26481) // Don't use pointer arithmetic. Use span instead (bounds.1).
#pragma warning(disable : 26490) // Don't use reinterpret_cast (type.1).
#ifdef __AVX2__
            // I wrote this vectorized code one day, because the sun was shining so nicely.
            // There's no other reason for this to exist here, except for being pretty.
            // This code implements the exact same for loop you can find below, but is ~3x faster.
            //
            // A brief explanation for people unfamiliar with vectorized instructions:
            //   Vectorized instructions, like "SSE" or "AVX", allow you to run
            //   common operations like additions, multiplications, comparisons,
            //   or bitwise operations concurrently on multiple values at once.
            //
            // We want to find the given defaultColor in the first 8 values of colorTable.
            // Coincidentally a COLORREF is a DWORD and 8 of them are exactly 256 bits.
            // -- The size of a single AVX register.
            //
            // Thus, the code works like this:
            // 1. Load all 8 DWORDs at once into one register
            // 2. Set the same defaultColor 8 times in another register
            // 3. Compare all 8 values at once
            //    The result is either 0xffffffff or 0x00000000.
            // 4. Extract the most significant bit of each DWORD
            //    Assuming that no duplicate colors exist in colorTable,
            //    the result will be something like 0b00100000.
            // 5. Use BitScanForward (bsf) to find the index of the most significant 1 bit.
            const auto haystack = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(colorTable.data())); // 1.
            const auto needle = _mm256_set1_epi32(std::bit_cast<int>(defaultColor)); // 2.
            const auto result = _mm256_cmpeq_epi32(haystack, needle); // 3.
            const auto mask = _mm256_movemask_ps(_mm256_castsi256_ps(result)); // 4.
            unsigned long index;
            return _BitScanForward(&index, mask) ? til::at(colorTable, static_cast<size_t>(index) + 8) : defaultColor; // 5.
#elif _M_AMD64
            // If you look closely this SSE2 algorithm is the same as the AVX one.
            // The two differences are that we need to:
            // * do everything twice, because SSE is limited to 128 bits and not 256.
            // * use _mm_packs_epi32 to merge two 128 bits vectors into one in step 3.5.
            //   _mm_packs_epi32 takes two SSE registers and truncates all 8 DWORDs into 8 WORDs,
            //   the latter of which fits into a single register (which is then used in the identical step 4).
            // * since the result are now 8 WORDs, we need to use _mm_movemask_epi8 (there's no 16-bit variant),
            //   which unlike AVX's step 4 results in in something like 0b0000110000000000.
            //   --> the index returned by _BitScanForward must be divided by 2.
            const auto haystack1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(colorTable.data() + 0));
            const auto haystack2 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(colorTable.data() + 4));
            const auto needle = _mm_set1_epi32(std::bit_cast<int>(defaultColor));
            const auto result1 = _mm_cmpeq_epi32(haystack1, needle);
            const auto result2 = _mm_cmpeq_epi32(haystack2, needle);
            const auto result = _mm_packs_epi32(result1, result2); // 3.5
            const auto mask = _mm_movemask_epi8(result);
            unsigned long index;
            return _BitScanForward(&index, mask) ? til::at(colorTable, static_cast<size_t>(index / 2) + 8) : defaultColor;
#else
            for (size_t i = 0; i < 8; i++)
            {
                if (til::at(colorTable, i) == defaultColor)
                {
                    return til::at(colorTable, i + 8);
                }
            }
#endif
#pragma warning(pop)
        }

        return defaultColor;
    }
    else if (IsRgb())
    {
        return GetRGB();
    }
    else if (IsIndex16() && brighten)
    {
        return til::at(colorTable, _index | 8);
    }
    else
    {
        return til::at(colorTable, _index);
    }
}

// Method Description:
// - Return a legacy index value that best approximates this color.
// Arguments:
// - defaultIndex: The index to use for a default color.
// Return Value:
// - an index into the 16-color table
BYTE TextColor::GetLegacyIndex(const BYTE defaultIndex) const noexcept
{
    if (IsDefault())
    {
        return defaultIndex;
    }
    else if (IsIndex16() || IsIndex256())
    {
        return til::at(Index256ToIndex16, GetIndex());
    }
    else
    {
        // We compress the RGB down to an 8-bit value and use that to
        // lookup a representative 16-color index from a hard-coded table.
        const BYTE compressedRgb = (_red & 0b11100000) +
            ((_green >> 3) & 0b00011100) +
            ((_blue >> 6) & 0b00000011);
        return til::at(CompressedRgbToIndex16, compressedRgb);
    }
}

// Keeping TextColor compact helps us keeping TextAttribute compact,
// which in turn ensures that our buffer memory usage is low.
static_assert(sizeof(TextAttribute) == 18);
static_assert(alignof(TextAttribute) == 2);
// Ensure that we can memcpy() and memmove() the struct for performance.
static_assert(std::is_trivially_copyable_v<TextAttribute>);
// Assert that the use of memcmp() for comparisons is safe.
static_assert(std::has_unique_object_representations_v<TextAttribute>);

namespace
{
    constexpr std::array<TextColor, 16> s_initLegacyColorMap(const BYTE defaultIndex)
    {
        std::array<TextColor, 16> legacyColorMap;
        for (auto i = 0u; i < legacyColorMap.size(); i++)
        {
            const auto legacyIndex = TextColor::TransposeLegacyIndex(i);
            gsl::at(legacyColorMap, i) = i == defaultIndex ? TextColor{} : TextColor{ legacyIndex, true };
        }
        return legacyColorMap;
    }

    BYTE s_legacyDefaultForeground = 7;
    BYTE s_legacyDefaultBackground = 0;
    BYTE s_ansiDefaultForeground = 7;
    BYTE s_ansiDefaultBackground = 0;
}

// These maps allow for an efficient conversion from a legacy attribute index
// to a TextColor with the corresponding ANSI index, also taking into account
// the legacy index values that need to be converted to a default TextColor.
std::array<TextColor, 16> TextAttribute::s_legacyForegroundColorMap = s_initLegacyColorMap(7);
std::array<TextColor, 16> TextAttribute::s_legacyBackgroundColorMap = s_initLegacyColorMap(0);

// Routine Description:
// - Sets the legacy attributes which map to and from the default colors.
// Parameters:
// - defaultAttributes: the attribute values to be used for default colors.
// Return value:
// - None
void TextAttribute::SetLegacyDefaultAttributes(const WORD defaultAttributes) noexcept
{
    // First we reset the current default color map entries to what they should
    // be for a regular translation from a legacy index to an ANSI TextColor.
    gsl::at(s_legacyForegroundColorMap, s_legacyDefaultForeground) = TextColor{ s_ansiDefaultForeground, true };
    gsl::at(s_legacyBackgroundColorMap, s_legacyDefaultBackground) = TextColor{ s_ansiDefaultBackground, true };

    // Then we save the new default attribute values and their corresponding
    // ANSI translations. We use the latter values to more efficiently handle
    // the "VT Quirk" conversion below.
    s_legacyDefaultForeground = defaultAttributes & FG_ATTRS;
    s_legacyDefaultBackground = (defaultAttributes & BG_ATTRS) >> 4;
    s_ansiDefaultForeground = TextColor::TransposeLegacyIndex(s_legacyDefaultForeground);
    s_ansiDefaultBackground = TextColor::TransposeLegacyIndex(s_legacyDefaultBackground);

    // Finally we set the new default color map entries.
    gsl::at(s_legacyForegroundColorMap, s_legacyDefaultForeground) = TextColor{};
    gsl::at(s_legacyBackgroundColorMap, s_legacyDefaultBackground) = TextColor{};
}

// Routine Description:
// - Returns a WORD with legacy-style attributes for this textattribute.
// Parameters:
// - None
// Return value:
// - a WORD with legacy-style attributes for this textattribute.
WORD TextAttribute::GetLegacyAttributes() const noexcept
{
    const auto fgIndex = _foreground.GetLegacyIndex(s_legacyDefaultForeground);
    const auto bgIndex = _background.GetLegacyIndex(s_legacyDefaultBackground);
    const WORD metaAttrs = static_cast<WORD>(_attrs) & USED_META_ATTRS;
    const auto brighten = IsIntense() && _foreground.CanBeBrightened();
    return fgIndex | (bgIndex << 4) | metaAttrs | (brighten ? FOREGROUND_INTENSITY : 0);
}

bool TextAttribute::IsLegacy() const noexcept
{
    return _foreground.IsLegacy() && _background.IsLegacy();
}

// Method description:
// - Tells us whether the text is a hyperlink or not
// Return value:
// - True if it is a hyperlink, false otherwise
bool TextAttribute::IsHyperlink() const noexcept
{
    // All non-hyperlink text have a default hyperlinkId of 0 while
    // all hyperlink text have a non-zero hyperlinkId
    return _hyperlinkId != 0;
}

TextColor TextAttribute::GetForeground() const noexcept
{
    return _foreground;
}

TextColor TextAttribute::GetBackground() const noexcept
{
    return _background;
}

// Method description:
// - Retrieves the hyperlink ID of the text
// Return value:
// - The hyperlink ID
uint16_t TextAttribute::GetHyperlinkId() const noexcept
{
    return _hyperlinkId;
}

TextColor TextAttribute::GetUnderlineColor() const noexcept
{
    return _underlineColor;
}

// Method description:
// - Retrieves the underline style of the text.
// - If the attribute is not the **current** attribute of the text buffer,
//   (eg. reading an attribute from another part of the text buffer, which
//   was modified using DECRARA), this might return an invalid style. In this
//   case, treat the style as singly underlined.
// Return value:
// - The underline style.
UnderlineStyle TextAttribute::GetUnderlineStyle() const noexcept
{
    const auto styleAttr = WI_EnumValue(_attrs & CharacterAttributes::UnderlineStyle);
    return static_cast<UnderlineStyle>(styleAttr >> UNDERLINE_STYLE_SHIFT);
}

void TextAttribute::SetForeground(const TextColor foreground) noexcept
{
    _foreground = foreground;
}

void TextAttribute::SetBackground(const TextColor background) noexcept
{
    _background = background;
}

void TextAttribute::SetUnderlineColor(const TextColor color) noexcept
{
    // Index16 colors are not supported for underline colors.
    assert(!color.IsIndex16());
    _underlineColor = color;
}

void TextAttribute::SetForeground(const COLORREF rgbForeground) noexcept
{
    _foreground = TextColor(rgbForeground);
}

void TextAttribute::SetBackground(const COLORREF rgbBackground) noexcept
{
    _background = TextColor(rgbBackground);
}

void TextAttribute::SetIndexedForeground(const BYTE fgIndex) noexcept
{
    _foreground = TextColor(fgIndex, false);
}

void TextAttribute::SetIndexedBackground(const BYTE bgIndex) noexcept
{
    _background = TextColor(bgIndex, false);
}

void TextAttribute::SetIndexedForeground256(const BYTE fgIndex) noexcept
{
    _foreground = TextColor(fgIndex, true);
}

void TextAttribute::SetIndexedBackground256(const BYTE bgIndex) noexcept
{
    _background = TextColor(bgIndex, true);
}

void TextAttribute::SetColor(const COLORREF rgbColor, const bool fIsForeground) noexcept
{
    if (fIsForeground)
    {
        SetForeground(rgbColor);
    }
    else
    {
        SetBackground(rgbColor);
    }
}

// Method description:
// - Sets the hyperlink ID of the text
// Arguments:
// - id - the id we wish to set
void TextAttribute::SetHyperlinkId(uint16_t id) noexcept
{
    _hyperlinkId = id;
}

bool TextAttribute::IsTopHorizontalDisplayed() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::TopGridline);
}

bool TextAttribute::IsBottomHorizontalDisplayed() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::BottomGridline);
}

bool TextAttribute::IsLeftVerticalDisplayed() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::LeftGridline);
}

bool TextAttribute::IsRightVerticalDisplayed() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::RightGridline);
}

void TextAttribute::SetLeftVerticalDisplayed(const bool isDisplayed) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::LeftGridline, isDisplayed);
}

void TextAttribute::SetRightVerticalDisplayed(const bool isDisplayed) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::RightGridline, isDisplayed);
}

bool TextAttribute::IsIntense() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::Intense);
}

bool TextAttribute::IsFaint() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::Faint);
}

bool TextAttribute::IsItalic() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::Italics);
}

bool TextAttribute::IsBlinking() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::Blinking);
}

bool TextAttribute::IsInvisible() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::Invisible);
}

bool TextAttribute::IsCrossedOut() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::CrossedOut);
}

// Method description:
// - Returns true if the text is underlined with any underline style.
bool TextAttribute::IsUnderlined() const noexcept
{
    const auto style = GetUnderlineStyle();
    return (style != UnderlineStyle::NoUnderline);
}

bool TextAttribute::IsOverlined() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::TopGridline);
}

bool TextAttribute::IsReverseVideo() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::ReverseVideo);
}

bool TextAttribute::IsProtected() const noexcept
{
    return WI_IsFlagSet(_attrs, CharacterAttributes::Protected);
}

void TextAttribute::SetIntense(bool isIntense) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::Intense, isIntense);
}

void TextAttribute::SetFaint(bool isFaint) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::Faint, isFaint);
}

void TextAttribute::SetItalic(bool isItalic) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::Italics, isItalic);
}

void TextAttribute::SetBlinking(bool isBlinking) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::Blinking, isBlinking);
}

void TextAttribute::SetInvisible(bool isInvisible) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::Invisible, isInvisible);
}

void TextAttribute::SetCrossedOut(bool isCrossedOut) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::CrossedOut, isCrossedOut);
}

// Method description:
// - Sets underline style to singly, doubly, or one of the extended styles.
// Arguments:
// - style - underline style to set.
void TextAttribute::SetUnderlineStyle(const UnderlineStyle style) noexcept
{
    const auto shiftedStyle = WI_EnumValue(style) << UNDERLINE_STYLE_SHIFT;
    _attrs = (_attrs & ~CharacterAttributes::UnderlineStyle) | static_cast<CharacterAttributes>(shiftedStyle);
}

void TextAttribute::SetOverlined(bool isOverlined) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::TopGridline, isOverlined);
}

void TextAttribute::SetReverseVideo(bool isReversed) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::ReverseVideo, isReversed);
}

void TextAttribute::SetProtected(bool isProtected) noexcept
{
    WI_UpdateFlag(_attrs, CharacterAttributes::Protected, isProtected);
}

// Routine Description:
// - swaps foreground and background color
void TextAttribute::Invert() noexcept
{
    WI_ToggleFlag(_attrs, CharacterAttributes::ReverseVideo);
}

void TextAttribute::SetDefaultForeground() noexcept
{
    _foreground = TextColor();
}

void TextAttribute::SetDefaultBackground() noexcept
{
    _background = TextColor();
}

void TextAttribute::SetDefaultUnderlineColor() noexcept
{
    _underlineColor = TextColor{};
}

// Method description:
// - Resets only the rendition character attributes, which includes everything
//     except the Protected attribute.
void TextAttribute::SetDefaultRenditionAttributes() noexcept
{
    _attrs &= ~CharacterAttributes::Rendition;
}

// Method Description:
// - Returns true if this attribute indicates its background is the "default"
//      background. Its _rgbBackground will contain the actual value of the
//      default background. If the default colors are ever changed, this method
//      should be used to identify attributes with the default bg value, and
//      update them accordingly.
// Arguments:
// - <none>
// Return Value:
// - true iff this attribute indicates it's the "default" background color.
bool TextAttribute::BackgroundIsDefault() const noexcept
{
    return _background.IsDefault();
}

// Routine Description:
// - Resets the character attributes, which is what the VT standard
//      requires for most erasing and filling operations. In modern
//      applications it is also expected that hyperlinks are erased.
void TextAttribute::SetStandardErase() noexcept
{
    _attrs = CharacterAttributes::Normal;
    _hyperlinkId = 0;
    _markKind = MarkKind::None;
}

// BODGY: Misdiagnosis in MSVC 17.11: Referencing global constants in the member
// initializer list leads to this warning. Can probably be removed in the future.
#pragma warning(disable : 26493) // Don't use C-style casts (type.4).)

// Routine Description:
// - Constructs a read-only view of data formatted as a single output buffer cell
// Arguments:
// - view - String data for the text displayed on screen
// - dbcsAttr - Describes column width information (double byte character data)
// - textAttr - Describes color and formatting data
// - behavior - Describes where to retrieve color/format data. From this view? From defaults? etc.
OutputCellView::OutputCellView(const std::wstring_view view,
    const DbcsAttribute dbcsAttr,
    const TextAttribute textAttr,
    const TextAttributeBehavior behavior) noexcept :
    _view(view),
    _dbcsAttr(dbcsAttr),
    _textAttr(textAttr),
    _behavior(behavior)
{
}

// Routine Description:
// - Returns reference to view over text data
// Return Value:
// - Reference to UTF-16 character data
// C26445 - suppressed to enable the `TextBufferTextIterator::operator->` method which needs a non-temporary memory location holding the wstring_view.
// TODO: GH 2681 - remove this suppression by reconciling the probably bad design of the iterators that leads to this being required.
GSL_SUPPRESS(26445)
const std::wstring_view& OutputCellView::Chars() const noexcept
{
    return _view;
}

// Routine Description:
// - Reports how many columns we expect the Chars() text data to consume
// Return Value:
// - Count of column cells on the screen
til::CoordType OutputCellView::Columns() const noexcept
{
    return DbcsAttr() == DbcsAttribute::Leading ? 2 : 1;
}

// Routine Description:
// - Retrieves character cell width data
// Return Value:
// - DbcsAttribute data
DbcsAttribute OutputCellView::DbcsAttr() const noexcept
{
    return _dbcsAttr;
}

// Routine Description:
// - Retrieves text color/formatting information
// Return Value:
// - TextAttribute with encoded formatting data
TextAttribute OutputCellView::TextAttr() const noexcept
{
    return _textAttr;
}

// Routine Description:
// - Retrieves behavior for inserting this cell into the buffer. See enum for details.
// Return Value:
// - TextAttributeBehavior enum value
TextAttributeBehavior OutputCellView::TextAttrBehavior() const noexcept
{
    return _behavior;
}

// Routine Description:
// - Compares two views
// Arguments:
// - it - Other view to compare to this one
// Return Value:
// - True if all contents/references are equal. False otherwise.
bool OutputCellView::operator==(const OutputCellView& it) const noexcept
{
    return _view == it._view &&
        _dbcsAttr == it._dbcsAttr &&
        _textAttr == it._textAttr &&
        _behavior == it._behavior;
}

// Routine Description:
// - Compares two views for inequality
// Arguments:
// - it - Other view to compare tot his one.
// Return Value:
// - True if any contents or references are inequal. False if they're all equal.
bool OutputCellView::operator!=(const OutputCellView& it) const noexcept
{
    return !(*this == it);
}

// BODGY: Misdiagnosis in MSVC 17.11: Referencing global constants in the member
// initializer list leads to this warning. Can probably be removed in the future.
#pragma warning(disable : 26493) // Don't use C-style casts (type.4).)

static constexpr TextAttribute InvalidTextAttribute{ INVALID_COLOR, INVALID_COLOR };

OutputCell::OutputCell() noexcept :
    _text{},
    _dbcsAttribute{},
    _textAttribute{ InvalidTextAttribute },
    _behavior{ TextAttributeBehavior::Stored }
{
}

OutputCell::OutputCell(const std::wstring_view charData,
    const DbcsAttribute dbcsAttribute,
    const TextAttributeBehavior behavior) :
    _text{ UNICODE_INVALID },
    _dbcsAttribute{ dbcsAttribute },
    _textAttribute{ InvalidTextAttribute },
    _behavior{ behavior }
{
    THROW_HR_IF(E_INVALIDARG, charData.empty());
    _setFromStringView(charData);
    _setFromBehavior(behavior);
}

OutputCell::OutputCell(const std::wstring_view charData,
    const DbcsAttribute dbcsAttribute,
    const TextAttribute textAttribute) :
    _text{ UNICODE_INVALID },
    _dbcsAttribute{ dbcsAttribute },
    _textAttribute{ textAttribute },
    _behavior{ TextAttributeBehavior::Stored }
{
    THROW_HR_IF(E_INVALIDARG, charData.empty());
    _setFromStringView(charData);
}

OutputCell::OutputCell(const OutputCellView& cell)
{
    _setFromOutputCellView(cell);
}

const std::wstring_view OutputCell::Chars() const noexcept
{
    return _text;
}

void OutputCell::SetChars(const std::wstring_view chars)
{
    _setFromStringView(chars);
}

DbcsAttribute& OutputCell::DbcsAttr() noexcept
{
    return _dbcsAttribute;
}

TextAttribute& OutputCell::TextAttr()
{
    THROW_HR_IF(E_INVALIDARG, _behavior == TextAttributeBehavior::Current);
    return _textAttribute;
}

void OutputCell::_setFromBehavior(const TextAttributeBehavior behavior)
{
    THROW_HR_IF(E_INVALIDARG, behavior == TextAttributeBehavior::Stored);
}

void OutputCell::_setFromStringView(const std::wstring_view view)
{
    _text = view;
}

void OutputCell::_setFromOutputCellView(const OutputCellView& cell)
{
    _dbcsAttribute = cell.DbcsAttr();
    _textAttribute = cell.TextAttr();
    _behavior = cell.TextAttrBehavior();
    _text = cell.Chars();
}

// BODGY: Misdiagnosis in MSVC 17.11: Referencing global constants in the member
// initializer list leads to this warning. Can probably be removed in the future.
#pragma warning(disable : 26493) // Don't use C-style casts (type.4).)

static constexpr TextAttribute InvalidTextAttribute{ INVALID_COLOR, INVALID_COLOR, INVALID_COLOR };

// Routine Description:
// - This is a fill-mode iterator for one particular wchar. It will repeat forever if fillLimit is 0.
// Arguments:
// - wch - The character to use for filling
// - fillLimit - How many times to allow this value to be viewed/filled. Infinite if 0.
OutputCellIterator::OutputCellIterator(const wchar_t& wch, const size_t fillLimit) noexcept :
    _mode(Mode::Fill),
    _currentView(s_GenerateView(wch)),
    _run(),
    _attr(InvalidTextAttribute),
    _pos(0),
    _distance(0),
    _fillLimit(fillLimit)
{
}

// Routine Description:
// - This is a fill-mode iterator for one particular color. It will repeat forever if fillLimit is 0.
// Arguments:
// - attr - The color attribute to use for filling
// - fillLimit - How many times to allow this value to be viewed/filled. Infinite if 0.
OutputCellIterator::OutputCellIterator(const TextAttribute& attr, const size_t fillLimit) noexcept :
    _mode(Mode::Fill),
    _currentView(s_GenerateView(attr)),
    _run(),
    _attr(InvalidTextAttribute),
    _pos(0),
    _distance(0),
    _fillLimit(fillLimit)
{
}

// Routine Description:
// - This is a fill-mode iterator for one particular character and color. It will repeat forever if fillLimit is 0.
// Arguments:
// - wch - The character to use for filling
// - attr - The color attribute to use for filling
// - fillLimit - How many times to allow this value to be viewed/filled. Infinite if 0.
OutputCellIterator::OutputCellIterator(const wchar_t& wch, const TextAttribute& attr, const size_t fillLimit) noexcept :
    _mode(Mode::Fill),
    _currentView(s_GenerateView(wch, attr)),
    _run(),
    _attr(InvalidTextAttribute),
    _pos(0),
    _distance(0),
    _fillLimit(fillLimit)
{
}

// Routine Description:
// - This is a fill-mode iterator for one particular CHAR_INFO. It will repeat forever if fillLimit is 0.
// Arguments:
// - charInfo - The legacy character and color data to use for filling (uses Unicode portion of text data)
// - fillLimit - How many times to allow this value to be viewed/filled. Infinite if 0.
OutputCellIterator::OutputCellIterator(const CHAR_INFO& charInfo, const size_t fillLimit) noexcept :
    _mode(Mode::Fill),
    _currentView(s_GenerateView(charInfo)),
    _run(),
    _attr(InvalidTextAttribute),
    _pos(0),
    _distance(0),
    _fillLimit(fillLimit)
{
}

// Routine Description:
// - This is an iterator over a range of text only. No color data will be modified as the text is inserted.
// Arguments:
// - utf16Text - UTF-16 text range
OutputCellIterator::OutputCellIterator(const std::wstring_view utf16Text) noexcept :
    _mode(Mode::LooseTextOnly),
    _currentView(s_GenerateView(utf16Text)),
    _run(utf16Text),
    _attr(InvalidTextAttribute),
    _pos(0),
    _distance(0),
    _fillLimit(0)
{
}

// Routine Description:
// - This is an iterator over a range text that will apply the same color to every position.
// Arguments:
// - utf16Text - UTF-16 text range
// - attribute - Color to apply over the entire range
OutputCellIterator::OutputCellIterator(const std::wstring_view utf16Text, const TextAttribute& attribute, const size_t fillLimit) noexcept :
    _mode(Mode::Loose),
    _currentView(s_GenerateView(utf16Text, attribute)),
    _run(utf16Text),
    _attr(attribute),
    _distance(0),
    _pos(0),
    _fillLimit(fillLimit)
{
}

// Routine Description:
// - This is an iterator over legacy colors only. The text is not modified.
// Arguments:
// - legacyAttrs - One legacy color item per cell
OutputCellIterator::OutputCellIterator(const std::span<const WORD> legacyAttrs) noexcept :
    _mode(Mode::LegacyAttr),
    _currentView(s_GenerateViewLegacyAttr(til::at(legacyAttrs, 0))),
    _run(legacyAttrs),
    _attr(InvalidTextAttribute),
    _distance(0),
    _pos(0),
    _fillLimit(0)
{
}

// Routine Description:
// - This is an iterator over legacy cell data. We will use the unicode text and the legacy color attribute.
// Arguments:
// - charInfos - Multiple cell with unicode text and legacy color data.
OutputCellIterator::OutputCellIterator(const std::span<const CHAR_INFO> charInfos) noexcept :
    _mode(Mode::CharInfo),
    _currentView(s_GenerateView(til::at(charInfos, 0))),
    _run(charInfos),
    _attr(InvalidTextAttribute),
    _distance(0),
    _pos(0),
    _fillLimit(0)
{
}

// Routine Description:
// - This is an iterator over existing OutputCells with full text and color data.
// Arguments:
// - cells - Multiple cells in a run
OutputCellIterator::OutputCellIterator(const std::span<const OutputCell> cells) :
    _mode(Mode::Cell),
    _currentView(s_GenerateView(til::at(cells, 0))),
    _run(cells),
    _attr(InvalidTextAttribute),
    _distance(0),
    _pos(0),
    _fillLimit(0)
{
}

// Routine Description:
// - Specifies whether this iterator is valid for dereferencing (still valid underlying data)
// Return Value:
// - True if the views on dereference are valid. False if it shouldn't be dereferenced.
OutputCellIterator::operator bool() const noexcept
{
    try
    {
        switch (_mode)
        {
        case Mode::Loose:
        case Mode::LooseTextOnly:
        {
            // In lieu of using start and end, this custom iterator type simply becomes bool false
            // when we run out of items to iterate over.
            return _pos < std::get<std::wstring_view>(_run).length();
        }
        case Mode::Fill:
        {
            if (_fillLimit > 0)
            {
                return _pos < _fillLimit;
            }
            return true;
        }
        case Mode::Cell:
        {
            return _pos < std::get<std::span<const OutputCell>>(_run).size();
        }
        case Mode::CharInfo:
        {
            return _pos < std::get<std::span<const CHAR_INFO>>(_run).size();
        }
        case Mode::LegacyAttr:
        {
            return _pos < std::get<std::span<const WORD>>(_run).size();
        }
        default:
            FAIL_FAST_HR(E_NOTIMPL);
        }
    }
    CATCH_FAIL_FAST();
}

size_t OutputCellIterator::Position() const noexcept
{
    return _pos;
}

// Routine Description:
// - Advances the iterator one position over the underlying data source.
// Return Value:
// - Reference to self after advancement.
OutputCellIterator& OutputCellIterator::operator++()
{
    // Keep track of total distance moved (cells filled)
    _distance++;

    switch (_mode)
    {
    case Mode::Loose:
    {
        if (!_TryMoveTrailing())
        {
            // When walking through a text sequence, we need to move forward by the number of wchar_ts consumed in the previous view
            // in case we had a surrogate pair (or wider complex sequence) in the previous view.
            _pos += _currentView.Chars().size();
            if (operator bool())
            {
                _currentView = s_GenerateView(std::get<std::wstring_view>(_run).substr(_pos), _attr);
            }
        }
        break;
    }
    case Mode::LooseTextOnly:
    {
        if (!_TryMoveTrailing())
        {
            // When walking through a text sequence, we need to move forward by the number of wchar_ts consumed in the previous view
            // in case we had a surrogate pair (or wider complex sequence) in the previous view.
            _pos += _currentView.Chars().size();
            if (operator bool())
            {
                _currentView = s_GenerateView(std::get<std::wstring_view>(_run).substr(_pos));
            }
        }
        break;
    }
    case Mode::Fill:
    {
        if (!_TryMoveTrailing())
        {
            if (_currentView.DbcsAttr() == DbcsAttribute::Trailing)
            {
                _currentView = OutputCellView(_currentView.Chars(),
                    DbcsAttribute::Leading,
                    _currentView.TextAttr(),
                    _currentView.TextAttrBehavior());
            }

            if (_fillLimit > 0)
            {
                // We walk forward by one because we fill with the same cell over and over no matter what
                _pos++;
            }
        }
        break;
    }
    case Mode::Cell:
    {
        // Walk forward by one because cells are assumed to be in the form they needed to be
        _pos++;
        if (operator bool())
        {
            _currentView = s_GenerateView(til::at(std::get<std::span<const OutputCell>>(_run), _pos));
        }
        break;
    }
    case Mode::CharInfo:
    {
        // Walk forward by one because char infos are just the legacy version of cells and prealigned to columns
        _pos++;
        if (operator bool())
        {
            _currentView = s_GenerateView(til::at(std::get<std::span<const CHAR_INFO>>(_run), _pos));
        }
        break;
    }
    case Mode::LegacyAttr:
    {
        // Walk forward by one because color attributes apply cell by cell (no complex text information)
        _pos++;
        if (operator bool())
        {
            _currentView = s_GenerateViewLegacyAttr(til::at(std::get<std::span<const WORD>>(_run), _pos));
        }
        break;
    }
    default:
        FAIL_FAST_HR(E_NOTIMPL);
    }

    return (*this);
}

// Routine Description:
// - Advances the iterator one position over the underlying data source.
// Return Value:
// - Reference to self after advancement.
OutputCellIterator OutputCellIterator::operator++(int)
{
    auto temp = *this;
    operator++();
    return temp;
}

// Routine Description:
// - Reference the view to fully-formed output cell data representing the underlying data source.
// Return Value:
// - Reference to the view
const OutputCellView& OutputCellIterator::operator*() const noexcept
{
    return _currentView;
}

// Routine Description:
// - Get pointer to the view to fully-formed output cell data representing the underlying data source.
// Return Value:
// - Pointer to the view
const OutputCellView* OutputCellIterator::operator->() const noexcept
{
    return &_currentView;
}

// Routine Description:
// - Checks the current view. If it is a leading half, it updates the current
//   view to the trailing half of the same glyph.
// - This helps us to draw glyphs that are two columns wide by "doubling"
//   the view that is returned so it will consume two cells.
// Return Value:
// - True if we just turned a lead half into a trailing half (and caller doesn't
//   need to further update the view).
// - False if this wasn't applicable and the caller should update the view.
bool OutputCellIterator::_TryMoveTrailing() noexcept
{
    if (_currentView.DbcsAttr() == DbcsAttribute::Leading)
    {
        _currentView = OutputCellView(_currentView.Chars(),
            DbcsAttribute::Trailing,
            _currentView.TextAttr(),
            _currentView.TextAttrBehavior());
        return true;
    }
    else
    {
        return false;
    }
}

// Routine Description:
// - Static function to create a view.
// - It's pulled out statically so it can be used during construction with just the given
//   variables (so OutputCellView doesn't need an empty default constructor)
// - This will infer the width of the glyph and specify that the attributes shouldn't be changed.
// Arguments:
// - view - View representing characters corresponding to a single glyph
// Return Value:
// - Object representing the view into this cell
OutputCellView OutputCellIterator::s_GenerateView(const std::wstring_view view) noexcept
{
    return s_GenerateView(view, InvalidTextAttribute, TextAttributeBehavior::Current);
}

// Routine Description:
// - Static function to create a view.
// - It's pulled out statically so it can be used during construction with just the given
//   variables (so OutputCellView doesn't need an empty default constructor)
// - This will infer the width of the glyph and apply the appropriate attributes to the view.
// Arguments:
// - view - View representing characters corresponding to a single glyph
// - attr - Color attributes to apply to the text
// Return Value:
// - Object representing the view into this cell
OutputCellView OutputCellIterator::s_GenerateView(const std::wstring_view view, const TextAttribute attr) noexcept
{
    return s_GenerateView(view, attr, TextAttributeBehavior::Stored);
}

// Routine Description:
// - Static function to create a view.
// - It's pulled out statically so it can be used during construction with just the given
//   variables (so OutputCellView doesn't need an empty default constructor)
// - This will infer the width of the glyph and apply the appropriate attributes to the view.
// Arguments:
// - view - View representing characters corresponding to a single glyph
// - attr - Color attributes to apply to the text
// - behavior - Behavior of the given text attribute (used when writing)
// Return Value:
// - Object representing the view into this cell
OutputCellView OutputCellIterator::s_GenerateView(const std::wstring_view view, const TextAttribute attr, const TextAttributeBehavior behavior) noexcept
{
    const auto glyph = til::utf16_next(view);
    const auto dbcsAttr = IsGlyphFullWidth(glyph) ? DbcsAttribute::Leading : DbcsAttribute::Single;
    return OutputCellView(glyph, dbcsAttr, attr, behavior);
}

// Routine Description:
// - Static function to create a view.
// - It's pulled out statically so it can be used during construction with just the given
//   variables (so OutputCellView doesn't need an empty default constructor)
// - This will infer the width of the glyph and apply the appropriate attributes to the view.
// Arguments:
// - wch - View representing a single UTF-16 character (that can be represented without surrogates)
// Return Value:
// - Object representing the view into this cell
OutputCellView OutputCellIterator::s_GenerateView(const wchar_t& wch) noexcept
{
    const auto glyph = std::wstring_view(&wch, 1);
    const auto dbcsAttr = IsGlyphFullWidth(wch) ? DbcsAttribute::Leading : DbcsAttribute::Single;
    return OutputCellView(glyph, dbcsAttr, InvalidTextAttribute, TextAttributeBehavior::Current);
}

// Routine Description:
// - Static function to create a view.
// - It's pulled out statically so it can be used during construction with just the given
//   variables (so OutputCellView doesn't need an empty default constructor)
// - This will infer the width of the glyph and apply the appropriate attributes to the view.
// Arguments:
// - attr - View representing a single color
// Return Value:
// - Object representing the view into this cell
OutputCellView OutputCellIterator::s_GenerateView(const TextAttribute& attr) noexcept
{
    return OutputCellView({}, {}, attr, TextAttributeBehavior::StoredOnly);
}

// Routine Description:
// - Static function to create a view.
// - It's pulled out statically so it can be used during construction with just the given
//   variables (so OutputCellView doesn't need an empty default constructor)
// - This will infer the width of the glyph and apply the appropriate attributes to the view.
// Arguments:
// - wch - View representing a single UTF-16 character (that can be represented without surrogates)
// - attr - View representing a single color
// Return Value:
// - Object representing the view into this cell
OutputCellView OutputCellIterator::s_GenerateView(const wchar_t& wch, const TextAttribute& attr) noexcept
{
    const auto glyph = std::wstring_view(&wch, 1);
    const auto dbcsAttr = IsGlyphFullWidth(wch) ? DbcsAttribute::Leading : DbcsAttribute::Single;
    return OutputCellView(glyph, dbcsAttr, attr, TextAttributeBehavior::Stored);
}

// Routine Description:
// - Static function to create a view.
// - It's pulled out statically so it can be used during construction with just the given
//   variables (so OutputCellView doesn't need an empty default constructor)
// - This will infer the width of the glyph and apply the appropriate attributes to the view.
// Arguments:
// - legacyAttr - View representing a single legacy color
// Return Value:
// - Object representing the view into this cell
OutputCellView OutputCellIterator::s_GenerateViewLegacyAttr(const WORD& legacyAttr) noexcept
{
    auto cleanAttr = legacyAttr;
    WI_ClearAllFlags(cleanAttr, COMMON_LVB_SBCSDBCS); // don't use legacy lead/trailing byte flags for colors

    const TextAttribute attr(cleanAttr);
    return s_GenerateView(attr);
}

// Routine Description:
// - Static function to create a view.
// - It's pulled out statically so it can be used during construction with just the given
//   variables (so OutputCellView doesn't need an empty default constructor)
// - This will infer the width of the glyph and apply the appropriate attributes to the view.
// Arguments:
// - charInfo - character and attribute pair representing a single cell
// Return Value:
// - Object representing the view into this cell
OutputCellView OutputCellIterator::s_GenerateView(const CHAR_INFO& charInfo) noexcept
{
    const auto glyph = std::wstring_view(&charInfo.Char.UnicodeChar, 1);

    DbcsAttribute dbcsAttr = DbcsAttribute::Single;
    if (WI_IsFlagSet(charInfo.Attributes, COMMON_LVB_LEADING_BYTE))
    {
        dbcsAttr = DbcsAttribute::Leading;
    }
    else if (WI_IsFlagSet(charInfo.Attributes, COMMON_LVB_TRAILING_BYTE))
    {
        dbcsAttr = DbcsAttribute::Trailing;
    }

    const TextAttribute textAttr(charInfo.Attributes);

    const auto behavior = TextAttributeBehavior::Stored;
    return OutputCellView(glyph, dbcsAttr, textAttr, behavior);
}

// Routine Description:
// - Static function to create a view.
// - It's pulled out statically so it can be used during construction with just the given
//   variables (so OutputCellView doesn't need an empty default constructor)
// Arguments:
// - cell - A reference to the cell for which we will make the read-only view
// Return Value:
// - Object representing the view into this cell
OutputCellView OutputCellIterator::s_GenerateView(const OutputCell& cell)
{
    return OutputCellView(cell.Chars(), cell.DbcsAttr(), cell.TextAttr(), cell.TextAttrBehavior());
}

// Routine Description:
// - Gets the distance between two iterators relative to the input data given in.
// Return Value:
// - The number of items of the input run consumed between these two iterators.
til::CoordType OutputCellIterator::GetInputDistance(OutputCellIterator other) const noexcept
{
    return gsl::narrow_cast<til::CoordType>(_pos - other._pos);
}

// Routine Description:
// - Gets the distance between two iterators relative to the number of cells inserted.
// Return Value:
// - The number of cells in the backing buffer filled between these two iterators.
til::CoordType OutputCellIterator::GetCellDistance(OutputCellIterator other) const noexcept
{
    return gsl::narrow_cast<til::CoordType>(_distance - other._distance);
}
#endif 
