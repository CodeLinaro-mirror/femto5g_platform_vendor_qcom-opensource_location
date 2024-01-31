/*
Changes from Qualcomm Innovation Center are provided under the following license:

Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted (subject to the limitations in the
disclaimer below) provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of Qualcomm Innovation Center, Inc. nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "LocationIntegrationApiDiagLog.h"
#include <log_util.h>

namespace location_integration {

void LocationIntegrationApiDiagLog::fillDiagMmfDataInfo(diagMapMatchedFeedbackData* out,
        const mapMatchedFeedbackData& inMmfData) {
        if (LOC_HAS_VALID_MMFD_UTC_TIME & inMmfData.validityMask) {
            out->validityMask |= DIAG_MMF_VALID_UTC_TIME;
            out->utcTimestampMs = inMmfData.utcTimestampMs;
        }
        if (LOC_HAS_VALID_MMFD_LAT_DIFF & inMmfData.validityMask) {
            out->validityMask |= DIAG_MMF_VALID_LAT_DIFF;
            out->mapMatchedLatitudeDifference = inMmfData.mapMatchedLatitudeDifference;
        }
        if (LOC_HAS_VALID_MMFD_LONG_DIFF & inMmfData.validityMask) {
            out->validityMask |= DIAG_MMF_VALID_LONG_DIFF;
            out->mapMatchedLongitudeDifference = inMmfData.mapMatchedLongitudeDifference;
        }
        if (LOC_HAS_VALID_MMFD_TUNNEL & inMmfData.validityMask) {
            out->validityMask |= DIAG_MMF_VALID_TUNNEL;
            out->isTunnel = inMmfData.isTunnel;
        }
        if (LOC_HAS_VALID_MMFD_BEARING & inMmfData.validityMask) {
            out->validityMask |= DIAG_MMF_VALID_BEARING;
            out->bearing = inMmfData.bearing;
        }
        if (LOC_HAS_VALID_MMFD_ALTITUDE & inMmfData.validityMask) {
            out->validityMask |= DIAG_MMF_VALID_ALTITUDE;
            out->altitude = inMmfData.altitude;
        }
        if (LOC_HAS_VALID_MMFD_HOR_ACC & inMmfData.validityMask) {
            out->validityMask |= DIAG_MMF_VALID_HOR_ACC;
            out->horizontalAccuracy = inMmfData.horizontalAccuracy;
        }
        if (LOC_HAS_VALID_MMFD_ALT_ACC & inMmfData.validityMask) {
            out->validityMask |= DIAG_MMF_VALID_ALT_ACC;
            out->altitudeAccuracy = inMmfData.altitudeAccuracy;
        }
        if (LOC_HAS_VALID_MMFD_BEARING_ACC & inMmfData.validityMask) {
            out->validityMask |= DIAG_MMF_VALID_BEARING_ACC;
            out->bearingAccuracy = inMmfData.bearingAccuracy;
        }
}


void LocationIntegrationApiDiagLog::diagLogMmfData(const mapMatchedFeedbackData& inMmfData) {
    if (mDiagIface) {
        size_t size = 0;
        diagMapMatchedFeedbackData*  mmfDataInfo = NULL;
        diagBuffSrc bufferSrc;
        size = sizeof(diagMapMatchedFeedbackData);
        mmfDataInfo = (diagMapMatchedFeedbackData*)mDiagIface->logAlloc(
                LOG_GNSS_LIA_API_MMF_REPORT_C, size, &bufferSrc);
        if (mmfDataInfo) {
            mmfDataInfo->version = LOG_CLIENT_MMF_DIAG_MSG_VERSION;
            fillDiagMmfDataInfo(mmfDataInfo, inMmfData);
            mDiagIface->logCommit(mmfDataInfo, bufferSrc,
                    LOG_GNSS_LIA_API_MMF_REPORT_C,
                    sizeof(diagMapMatchedFeedbackData));
        } else {
            LOC_LOGe(" Failed to allocate buffer for MMF data !! ");
        }
    }
}


LocationIntegrationApiDiagLog::LocationIntegrationApiDiagLog() {
    if (NULL == mDiagIface) {
        mDiagIface = loadLocDiagIfaceInterface();
        if (nullptr == mDiagIface) {
            LOC_LOGe("Failed to loadLocDiagIfaceInterface!!");
        }
    }
}

LocationIntegrationApiDiagLog::~LocationIntegrationApiDiagLog() {

}
} //  namespace location_integration {
