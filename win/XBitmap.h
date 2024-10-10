#pragma once

extern const unsigned int xbmpLGreenLED[16 * 16];

//////////////////////////////////
#define BITMAP_QUICKASK_N       0
#define BITMAP_QUICKASK_P       1
#define BITMAP_HIDE_GPT_N       2
#define BITMAP_HIDE_GPT_P       3
#define BITMAP_GREEN_LED        4
#define BITMAP_RED_LED          5
//////////////////////////////////
#define BITMAP_MAX_ID           (BITMAP_RED_LED + 1)
#define BITMAP_MIN_ID           0

class BitmapBank final
{
    static constexpr const SIZE btnOrgSize[BITMAP_MAX_ID] =
    {
        {24, 24},   // Hide GPT Button Normal
        {24, 24},   // Hide GPT Button Pressed
        {24, 24},   // Quick Ask Button Normal
        {24, 24},   // Quick Ask Button Pressed
        {16, 16},   // LED green
        {16, 16}    // LED red
    };

    SIZE btnSize[BITMAP_MAX_ID] = { 0 };
    U32* btnData[BITMAP_MAX_ID] = { nullptr };
    bool btnDPIAWareness[BITMAP_MAX_ID] = { false };

    void MakeHideGPTButton(bool isDarkMode = false);
    void MakeQuickAskButton(bool isDarkMode = false);
    void MakeLEDButton(bool isDarkMode = false);

public:
    BitmapBank(UINT dpi = USER_DEFAULT_SCREEN_DPI, bool isDarkMode = false) noexcept
    {
        int size = 0;

        if (m_bitmapData)
        {
            std::free(m_bitmapData);
        }
        m_bitmapData = nullptr;

        for (int i = BITMAP_MIN_ID; i <= BITMAP_HIDE_GPT_P; i++)
        {
            btnDPIAWareness[i] = true;
            btnSize[i].cx = ::MulDiv(btnOrgSize[i].cx, dpi, USER_DEFAULT_SCREEN_DPI);
            btnSize[i].cy = ::MulDiv(btnOrgSize[i].cx, dpi, USER_DEFAULT_SCREEN_DPI);

            size += btnSize[i].cx * btnSize[i].cy;
        }

        for (int i = BITMAP_GREEN_LED; i <= BITMAP_RED_LED; i++)
        {
            btnDPIAWareness[i] = false;
            btnSize[i].cx = btnOrgSize[i].cx;
            btnSize[i].cy = btnOrgSize[i].cy;
            size += btnSize[i].cx * btnSize[i].cy;
        }

        m_bitmapData = static_cast<U32*>(std::malloc(size * sizeof(U32)));

        if (m_bitmapData)
        {
            U32* p = m_bitmapData;

            //ScreenFillColor(m_bitmapData, size, isDarkMode ? 0xFF171717 : 0xFFF0F0F0);
            //ScreenFillColor(m_bitmapData, size, isDarkMode ? 0xFFA7A7A7 : 0xFF0F0F0F);
            
            for (int i = BITMAP_MIN_ID; i < BITMAP_MAX_ID; i++)
            {
                btnData[i] = p;
                p += (btnSize[i].cx * btnSize[i].cy);
            }
        }

        MakeHideGPTButton(isDarkMode);
        MakeQuickAskButton(isDarkMode);
        MakeLEDButton(isDarkMode);
    }

    ~BitmapBank()
    {
        if (m_bitmapData)
        {
            std::free(m_bitmapData);
        }
        m_bitmapData = nullptr;
    }

    // No Copy. 
    BitmapBank(const BitmapBank&) = delete;
    BitmapBank& operator=(const BitmapBank&) & = delete;

    BitmapBank(BitmapBank&&) = default;
    BitmapBank& operator=(BitmapBank&&) & = delete;

    U32* GetBitmapData(int idx, int& width, int& height, bool& dpiAWareness)
    {
        U32* p = nullptr;
        
        width = height = 0;
        dpiAWareness = true;
        if (idx >= BITMAP_MIN_ID && idx < BITMAP_MAX_ID)
        {
            p = btnData[idx];
            width = btnSize[idx].cx;
            height = btnSize[idx].cy;
            dpiAWareness = btnDPIAWareness[idx];
        }
        return p;
    }

private:
    U32* m_bitmapData = nullptr;
};