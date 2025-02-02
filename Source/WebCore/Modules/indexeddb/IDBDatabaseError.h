/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IDBDatabaseError_h
#define IDBDatabaseError_h

#include "IDBDatabaseException.h"
#include <wtf/PassRefPtr.h>
#include <wtf/RefCounted.h>
#include <wtf/text/WTFString.h>

#if ENABLE(INDEXED_DATABASE)

namespace WebCore {

class IDBDatabaseError : public RefCounted<IDBDatabaseError> {
public:
    static Ref<IDBDatabaseError> create(unsigned short code)
    {
        ASSERT(code >= IDBDatabaseException::IDBDatabaseExceptionOffset);
        ASSERT(code < IDBDatabaseException::IDBDatabaseExceptionMax);
        return adoptRef(*new IDBDatabaseError(code));
    }

    static Ref<IDBDatabaseError> create(unsigned short code, const String& message)
    {
        ASSERT_WITH_MESSAGE(code >= IDBDatabaseException::IDBDatabaseExceptionOffset, "%d >= %d", code, IDBDatabaseException::IDBDatabaseExceptionOffset);
        ASSERT(code < IDBDatabaseException::IDBDatabaseExceptionMax);
        return adoptRef(*new IDBDatabaseError(code, message));
    }

    ~IDBDatabaseError() { }

    unsigned short code() const { return IDBDatabaseException::getLegacyErrorCode(m_code); }
    unsigned short idbCode() const { return m_code; }
    const String& message() const { return m_message; }
    const String name() const { return IDBDatabaseException::getErrorName(m_code); };

private:
    IDBDatabaseError(unsigned short code)
        : m_code(code), m_message(IDBDatabaseException::getErrorDescription(code)) { }
    IDBDatabaseError(unsigned short code, const String& message)
        : m_code(code), m_message(message) { }

    const unsigned short m_code;
    const String m_message;
};

} // namespace WebCore

#endif

#endif // IDBDatabaseError_h
