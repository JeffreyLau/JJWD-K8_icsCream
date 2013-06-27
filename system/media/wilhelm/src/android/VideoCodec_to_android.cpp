/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "sles_allinclusive.h"

#include <media/IMediaPlayerService.h>
#include <media/stagefright/OMXClient.h>
#include <media/stagefright/OMXCodec.h>
#include <media/IOMX.h>
#include <media/stagefright/MediaDefs.h>


namespace android {

static sp<IOMX> omx;

// listed in same order as VideoCodecIds[] in file "../devices.c" with ANDROID defined
static const char *kVideoMimeTypes[] = {
        MEDIA_MIMETYPE_VIDEO_MPEG2,
        MEDIA_MIMETYPE_VIDEO_H263,
        MEDIA_MIMETYPE_VIDEO_MPEG4,
        MEDIA_MIMETYPE_VIDEO_AVC,
        MEDIA_MIMETYPE_VIDEO_VPX
};
static const size_t kNbVideoMimeTypes = sizeof(kVideoMimeTypes) / sizeof(kVideoMimeTypes[0]);

// codec capabilities in the following arrays maps to the mime types defined in kVideoMimeTypes
static Vector<CodecCapabilities> VideoDecoderCapabilities[kNbVideoMimeTypes];
static XAuint32 VideoDecoderNbProfLevel[kNbVideoMimeTypes];

static XAuint32 NbSupportedDecoderTypes = 0;


XAuint32 convertOpenMaxIlToAl(OMX_U32 ilVideoProfileOrLevel) {
    // For video codec profiles and levels, the number of trailing zeroes in OpenMAX IL
    // are equal to the matching OpenMAX AL constant value plus 1, for example:
    //    XA_VIDEOPROFILE_H263_BACKWARDCOMPATIBLE ((XAuint32) 0x00000003)
    //        matches
    //    OMX_VIDEO_H263ProfileBackwardCompatible  = 0x04
    return (XAuint32) (__builtin_ctz(ilVideoProfileOrLevel) + 1);
}


bool android_videoCodec_expose() {
    SL_LOGV("android_videoCodec_expose()");

    sp<IMediaPlayerService> service(IMediaDeathNotifier::getMediaPlayerService());
    if (service == NULL) {
        // no need to SL_LOGE; getMediaPlayerService already will have done so
        return false;
    }

    omx = service->getOMX();
    if (omx.get() == NULL) {
        LOGE("android_videoCodec_expose() couldn't access OMX interface");
        return false;
    }

    // used to check whether no codecs were found, which is a sign of failure
    NbSupportedDecoderTypes = 0;
    for (size_t m = 0 ; m < kNbVideoMimeTypes ; m++) {
        if (OK == QueryCodecs(omx, kVideoMimeTypes[m], true /* queryDecoders */,
                true /* hwCodecOnly */, &VideoDecoderCapabilities[m])) {
            if (!VideoDecoderCapabilities[m].empty()) {
                NbSupportedDecoderTypes++;
            }
            // for each decoder of the given decoder ID, verify it is a hardware decoder
            for (size_t c = 0 ; c < VideoDecoderCapabilities[m].size() ; c++) {
                VideoDecoderNbProfLevel[c] = 0;
                const String8& compName =
                        VideoDecoderCapabilities[m].itemAt(c).mComponentName;
                // get the number of profiles and levels for this decoder
                VideoDecoderNbProfLevel[m] =
                        VideoDecoderCapabilities[m].itemAt(c).mProfileLevels.size();
                if (VideoDecoderNbProfLevel[m] != 0) {
                    SL_LOGV("codec %d nb prof/level=%d", m, VideoDecoderNbProfLevel[m]);
                    break;
                }
            }
        }
    }

    return (NbSupportedDecoderTypes > 0);
}


void android_videoCodec_deinit() {
    SL_LOGV("android_videoCodec_deinit()");
    for (size_t m = 0 ; m < kNbVideoMimeTypes ; m++) {
        VideoDecoderCapabilities[m].clear();
    }
}


XAuint32 android_videoCodec_getNbDecoders() {
    return NbSupportedDecoderTypes;
}


void android_videoCodec_getDecoderIds(XAuint32 nbDecoders, XAuint32 *pDecoderIds) {
    XAuint32 *pIds = pDecoderIds;
    XAuint32 nbFound = 0;
    for (size_t m = 0 ; m < kNbVideoMimeTypes ; m++) {
        if (!VideoDecoderCapabilities[m].empty()) {
            *pIds = VideoDecoderIds[m];
            pIds++;
            nbFound++;
        }
        // range check: function can be called for fewer codecs than there are
        if (nbFound == nbDecoders) {
            break;
        }
    }
}


SLresult android_videoCodec_getProfileLevelCombinationNb(XAuint32 decoderId, XAuint32 *pNb)
{
    // translate a decoder ID to an index in the codec table
    size_t decoderIndex = 0;
    *pNb = 0;
    while (decoderIndex < kNbVideoMimeTypes) {
        if (decoderId == VideoDecoderIds[decoderIndex]) {
            *pNb = VideoDecoderNbProfLevel[decoderIndex];
            break;
        }
        decoderIndex++;
    }

    return XA_RESULT_SUCCESS;
}


SLresult android_videoCodec_getProfileLevelCombination(XAuint32 decoderId, XAuint32 plIndex,
        XAVideoCodecDescriptor *pDescr)
{
    // translate a decoder ID to an index in the codec table
    size_t decoderIndex = 0;
    while (decoderIndex < kNbVideoMimeTypes) {
        if (decoderId == VideoDecoderIds[decoderIndex]) {
            if (!(plIndex < VideoDecoderCapabilities[decoderIndex].itemAt(0).mProfileLevels.size()))
            {
                // asking for invalid profile/level
                return XA_RESULT_PARAMETER_INVALID;
            }
            // we only look at the first codec, OpenMAX AL doesn't let you expose the capabilities
            //  of multiple codecs
            //     set the fields we know about
            pDescr->codecId = decoderId;
            pDescr->profileSetting = convertOpenMaxIlToAl(VideoDecoderCapabilities[decoderIndex].
                    itemAt(0).mProfileLevels.itemAt(plIndex).mProfile);
            pDescr->levelSetting =  convertOpenMaxIlToAl(VideoDecoderCapabilities[decoderIndex].
                    itemAt(0).mProfileLevels.itemAt(plIndex).mLevel);
            //     initialize the fields we don't know about
            pDescr->maxWidth = 0;
            pDescr->maxHeight = 0;
            pDescr->maxFrameRate = 0;
            pDescr->maxBitRate = 0;
            pDescr->rateControlSupported = 0;
            break;
        }
        decoderIndex++;
    }
    return (decoderIndex < kNbVideoMimeTypes) ? XA_RESULT_SUCCESS : XA_RESULT_PARAMETER_INVALID;
}

} // namespace android
