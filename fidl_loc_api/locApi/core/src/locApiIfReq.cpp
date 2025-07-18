/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#define LOG_NDEBUG 0
#define LOG_TAG "FidlLocApi_Core"

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include "msg_q.h"
#include "gps_extended_c.h"
#include "LocationDataTypes.h"
#include "loc_gps.h"
#include "loc_fidl_interface.h"
#include "loc_pla.h"
#include "log_util.h"
#include "locFidlMain.h"


enum loc_api_adapter_err handleLocApiOpen(uint64_t requestedMask, bool isMaster,
             uint64_t *supportedMask, fidlFeatureList * fidlFeatures,
             void *context) {
    static fidlEngineMsg sndMsg;
    size_t retRead = 0;
    uint32_t hwCapabilities = LCMT_HW_CAPABILITY_UNKNOWN;

    LOC_LOGD("%s] ", __func__);

    memset(&sndMsg, 0x00, sizeof(sndMsg));

    sndMsg.msgType = FIDL_ENGINE_MSG_OPEN;
    sndMsg.contextIn = context;

    sendMsg2FidlEngine(&sndMsg);

    /* Read Capabilities from file */

    FILE *fp = NULL;
    fp = fopen(LOC_FIDL_CAPABILITIES_FILE, "r" );
    if (NULL != fp) {
        retRead = fread(&hwCapabilities, 1, sizeof(uint32_t), fp);
        fclose(fp);
    }

    //Set Engine Capabilities
    if (NULL != supportedMask) {
        LOC_LOGD("%s:%d:Set Engine Capabilities",__func__,__LINE__);
        *supportedMask |= LOC_API_ADAPTER_BIT_SATELLITE_REPORT |
                          LOC_API_ADAPTER_BIT_EVENT_REPORT_INFO;

        if (LCMT_HW_CAPABILITY_UNKNOWN != hwCapabilities) {
           if (LCMT_HW_CAPABILITY_TIME_BASED_TRACKING_BIT ==
               (hwCapabilities & LCMT_HW_CAPABILITY_TIME_BASED_TRACKING_BIT)) {
               *supportedMask |= LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT;
           }

           if (LCMT_HW_CAPABILITY_GNSS_MEAS_BIT ==
               (hwCapabilities & LCMT_HW_CAPABILITY_GNSS_MEAS_BIT)) {
               *supportedMask |= LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT;
           }

        } else {
            /* Enable Position and Measurement reprot */
            /* if Hardware capability is not present */
            *supportedMask |= LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT;
            *supportedMask |= LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT;
       }
    } else {
        LOC_LOGE("%s:%d:Engine Capabilities is NULL",__func__,__LINE__);
    }

    //Set Engine Features for HW year as 2015
    if (NULL != fidlFeatures) {
        LOC_LOGD("%s:%d set Engine Features for HW year",__func__,__LINE__);
        fidlFeatures->feature[0] = 0;
        fidlFeatures->feature_len = 0;
    }

    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiClose(void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiStartFix(fidlPosMode& fixCriteria, void *context) {

    static fidlEngineMsg sndMsg;

    LOC_LOGD("%s] ", __func__);

    memset(&sndMsg, 0x00, sizeof(sndMsg));

    sndMsg.msgType = FIDL_ENGINE_MSG_START_FIX;
    sndMsg.contextIn = context;
    sndMsg.u.msgStartFix.mode = fixCriteria.mode;
    sndMsg.u.msgStartFix.min_interval = fixCriteria.min_interval;
    sndMsg.u.msgStartFix.preferred_accuracy = fixCriteria.preferred_accuracy;
    sndMsg.u.msgStartFix.timeBetweenMeasurements = fixCriteria.timeBetweenMeasurements;
    sendMsg2FidlEngine(&sndMsg);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiStopFix(void *context) {

    static fidlEngineMsg sndMsg;

    LOC_LOGD("%s] ", __func__);
    memset(&sndMsg, 0x00, sizeof(sndMsg));

    sndMsg.msgType = FIDL_ENGINE_MSG_STOP_FIX;
    sndMsg.contextIn = context;
    sendMsg2FidlEngine(&sndMsg);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiDeleteAidingData(const GnssAidingData& data, void *context) {

    static fidlEngineMsg sndMsg;

    LOC_LOGD("%s] ", __func__);
    memset(&sndMsg, 0x00, sizeof(sndMsg));

    sndMsg.msgType = FIDL_ENGINE_MSG_DELETE_AID;
    sndMsg.contextIn = context;
    sendMsg2FidlEngine(&sndMsg);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiInjectPosition(const GnssLocationInfoNotification &locationInfo,
            bool onDemandCpi, void *context) {

    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;

}

enum loc_api_adapter_err handleLocApiSetTime(LocGpsUtcTime time, int64_t timeReference,
            int uncertainty, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiSetXtraData(char* data, int length,
            void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiAtlOpenStatus(int handle, int is_succ,
            char* apn, uint32_t apnLen, AGpsBearerType bear, LocAGpsType agpsType,
            LocApnTypeMask mask, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiAtlCloseStatus(int handle, int is_succ,
            void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiSetPositionMode(fidlPosMode& posMode,
            void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;

}

enum loc_api_adapter_err handleLocApiSetServerFidl(const char* url, int len,
            LocServerType type, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiInformNiResponse(GnssNiResponse userResponse,
            const void* passThroughData, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiSetSUPLVersionFidl(GnssConfigSuplVersion version,
            void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiSetNMEATypesFidl(uint32_t typesMask,
            void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;

}

enum loc_api_adapter_err handleLocApiSetLPPConfigFidl(GnssConfigLppProfileMask profileMask,
            void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiSetSensorPropertiesFidl
                        (bool gyroBiasVarianceRandomWalk_valid,
                        float gyroBiasVarianceRandomWalk,
                        bool accelBiasVarianceRandomWalk_valid,
                        float accelBiasVarianceRandomWalk,
                        bool angleBiasVarianceRandomWalk_valid,
                        float angleBiasVarianceRandomWalk,
                        bool rateBiasVarianceRandomWalk_valid,
                        float rateBiasVarianceRandomWalk,
                        bool velocityBiasVarianceRandomWalk_valid,
                        float velocityBiasVarianceRandomWalk,
                        void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiSetSensorPerfControlConfigFidl(int controlMode,
                        int accelSamplesPerBatch,
                        int accelBatchesPerSec,
                        int gyroSamplesPerBatch,
                        int gyroBatchesPerSec,
                        int accelSamplesPerBatchHigh,
                        int accelBatchesPerSecHigh,
                        int gyroSamplesPerBatchHigh,
                        int gyroBatchesPerSecHigh,
                        int algorithmConfig, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;

}

enum loc_api_adapter_err handleLocApiSetAGLONASSProtocolFidl
            (GnssConfigAGlonassPositionProtocolMask aGlonassProtocol, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiSetLPPeProtocolCpFidl
            (GnssConfigLppeControlPlaneMask lppeCP, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiSetLPPeProtocolUpFidl
            (GnssConfigLppeUserPlaneMask lppeUP, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiConvertSuplVersion(const uint32_t suplVersion,
             GnssConfigSuplVersion *suplConfig, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiConvertLppProfile(const uint32_t lppProfile,
             GnssConfigLppProfileMask *gnssLppProfileMask, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiConvertLppeCp(const uint32_t lppeControlPlaneMask,
             GnssConfigLppeControlPlaneMask *gnssLppControlPlanMask, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiConvertLppeUp(const uint32_t lppeUserPlaneMask,
         GnssConfigLppeUserPlaneMask *gnssLppUpMask, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiGetWwanZppFix(void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiGetBestAvailableZppFix(void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiInstallAGpsCert(const LocDerEncodedCertificate* pData,
                                size_t length,
                                uint32_t slotBitMask,
                                void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}
enum loc_api_adapter_err handlFidlSetBlacklistSv(const GnssSvIdConfig& config, void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiGetBlacklistSv(void *context) {
    LOC_LOGD("%s] ", __func__);
    /* Define new message as get blacklist of SV.
       Send this new message to Q and process it.
       After processing 'get blacklist of SV' message
       call 'fidlReportGnssSvIdConfig' event with blacklist of SVs.
    */
    return LOC_API_ADAPTER_ERR_SUCCESS;
}


enum loc_api_adapter_err handleLocApiSetConstellationControl(const GnssSvTypeConfig& config,
        void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiGetConstellationControl(void *context) {
    LOC_LOGD("%s] ", __func__);
    /* Define new message as 'get Constellation Control'.
       Send this new message to Q and process it.
       After processing 'get Constellation Control' message
       call 'fidlReportGnssSvTypeConfig' event with constellation info.
    */
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiResetConstellationControl(void *context) {
    LOC_LOGD("%s] ", __func__);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err handleLocApiRequestXtraConfigInfo(const uint32_t configMask,
                                             const uint32_t configReqSource, void *context) {
    static fidlEngineMsg sndMsg;

    LOC_LOGD("%s] ", __func__);

    memset(&sndMsg, 0x00, sizeof(sndMsg));

    sndMsg.msgType = FIDL_ENGINE_MSG_XTRA_REQ;
    sndMsg.contextIn = context;

    sendMsg2FidlEngine(&sndMsg);

    return LOC_API_ADAPTER_ERR_SUCCESS;
}
