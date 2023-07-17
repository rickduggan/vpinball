#pragma once

#include "flexdmd_i.h"

class FontDef
{
public:
    FontDef(const string& path, OLE_COLOR tint, OLE_COLOR borderTint, int borderSize = 0);
    bool operator==(const FontDef& other) const;
    size_t hash() const;
    string toString() const;

    OLE_COLOR m_tint = RGB(255, 255, 255);
    OLE_COLOR m_borderTint = RGB(255, 255, 255);
    int m_borderSize = 0;
    string m_szPath = "";
};
