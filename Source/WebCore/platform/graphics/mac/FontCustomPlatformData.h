/*
 * Copyright (C) 2007 Apple Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef FontCustomPlatformData_h
#define FontCustomPlatformData_h

#include "TextFlags.h"
#include <CoreFoundation/CFBase.h>
#include <wtf/Forward.h>
#include <wtf/Noncopyable.h>
#include <wtf/RetainPtr.h>

typedef struct CGFont* CGFontRef;
typedef const struct __CTFontDescriptor* CTFontDescriptorRef;

// <rdar://problem/16980736> Web fonts crash on certain OSes when using CTFontManagerCreateFontDescriptorFromData()
#if (PLATFORM(IOS) && __IPHONE_OS_VERSION_MIN_REQUIRED < 80000) || (PLATFORM(MAC) && __MAC_OS_X_VERSION_MIN_REQUIRED < 101000)
#define CORETEXT_WEB_FONTS 0
#else
#define CORETEXT_WEB_FONTS 1
#endif

namespace WebCore {

class FontPlatformData;
class SharedBuffer;

struct FontCustomPlatformData {
    WTF_MAKE_NONCOPYABLE(FontCustomPlatformData);
public:
#if CORETEXT_WEB_FONTS
    explicit FontCustomPlatformData(CTFontDescriptorRef fontDescriptor)
        : m_fontDescriptor(fontDescriptor)
#else
    explicit FontCustomPlatformData(CGFontRef cgFont)
        : m_cgFont(cgFont)
#endif
    {
    }

    ~FontCustomPlatformData();

    FontPlatformData fontPlatformData(int size, bool bold, bool italic, FontOrientation = Horizontal, FontWidthVariant = RegularWidth, FontRenderingMode = NormalRenderingMode);

    static bool supportsFormat(const String&);

#if CORETEXT_WEB_FONTS
    RetainPtr<CTFontDescriptorRef> m_fontDescriptor;
#else
    RetainPtr<CGFontRef> m_cgFont;
#endif
};

std::unique_ptr<FontCustomPlatformData> createFontCustomPlatformData(SharedBuffer&);

}

#endif
