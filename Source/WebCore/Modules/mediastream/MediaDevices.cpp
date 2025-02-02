/*
 * Copyright (C) 2015 Ericsson AB. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Ericsson nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "MediaDevices.h"

#if ENABLE(MEDIA_STREAM)

#include "Dictionary.h"
#include "Document.h"
#include "MediaStream.h"
#include "UserMediaController.h"
#include "UserMediaRequest.h"

namespace WebCore {

Ref<MediaDevices> MediaDevices::create(ScriptExecutionContext* context)
{
    return adoptRef(*new MediaDevices(context));
}

MediaDevices::MediaDevices(ScriptExecutionContext* context)
    : ContextDestructionObserver(context)
{
}

MediaDevices::~MediaDevices()
{
}

Document* MediaDevices::document() const
{
    return downcast<Document>(scriptExecutionContext());
}

void MediaDevices::getUserMedia(const Dictionary& options, ResolveCallback resolveCallback, RejectCallback rejectCallback, ExceptionCode& ec) const
{
    UserMediaController* userMedia = UserMediaController::from(document() ? document()->page() : nullptr);
    if (!userMedia) {
        // FIXME: We probably want to return a MediaStreamError here using the rejectCallback, and get rid off the ExceptionCode parameter.
        ec = NOT_SUPPORTED_ERR;
        return;
    }

    RefPtr<UserMediaRequest> request = UserMediaRequest::create(document(), userMedia, options, WTF::move(resolveCallback), WTF::move(rejectCallback), ec);
    if (!request) {
        ec = NOT_SUPPORTED_ERR;
        return;
    }

    request->start();
}

} // namespace WebCore

#endif // ENABLE(MEDIA_STREAM)
