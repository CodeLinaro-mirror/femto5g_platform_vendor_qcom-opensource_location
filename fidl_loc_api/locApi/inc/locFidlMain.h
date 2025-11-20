/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#include <stdint.h>
#include <errno.h>
#include "msg_q.h"
#include "loc_fidl_interface.h"
#include "locClientFidlIf.h"

#ifndef LOC_FIDL_MAIN_H
#define LOC_FIDL_MAIN_H

typedef uint32_t fidlEngineState_t;
#define FIDL_ENGINE_STATE_NONE                  (0)
#define FIDL_ENGINE_STATE_INIT                  (1)
#define FIDL_ENGINE_STATE_HW_OPEN_REQ           (3)
#define FIDL_ENGINE_STATE_HW_UP                 (4)
#define FIDL_ENGINE_STATE_IN_SESSION            (5)
#define FIDL_ENGINE_STATE_IN_SESSION_HW_DOWN    (6)
#define FIDL_ENGINE_STATE_HW_CLOSE_REQ          (7)
#define FIDL_ENGINE_STATE_HW_DOWN               (8)
#define FIDL_ENGINE_STATE_RESET                 (9)

typedef uint32_t fidlEngineMessage;

#define FIDL_ENGINE_MSG_MIN              (0)
#define FIDL_ENGINE_MSG_INIT             (1)
#define FIDL_ENGINE_MSG_OPEN             (2)
#define FIDL_ENGINE_MSG_CLOSE            (3)
#define FIDL_ENGINE_MSG_START_FIX        (4)
#define FIDL_ENGINE_MSG_STOP_FIX         (5)
#define FIDL_ENGINE_MSG_DELETE_AID       (6)
#define FIDL_ENGINE_MSG_INJ_POS          (7)
#define FIDL_ENGINE_MSG_SET_POS_MODE     (8)
#define FIDL_ENGINE_MSG_SET_TIME         (9)
#define FIDL_ENGINE_MSG_CLEANUP          (10)
#define FIDL_ENGINE_MSG_XTRA_REQ         (11)
/* Add New Message above MAX */
#define FIDL_ENGINE_MSG_MAX              (12)


#define FIDL_ENGINE_EVENT_MIN              (50)
#define FIDL_ENGINE_EVENT_INIT             (51)
#define FIDL_ENGINE_EVENT_OPEN             (52)
#define FIDL_ENGINE_EVENT_CLOSE            (53)
#define FIDL_ENGINE_EVENT_START_FIX        (54)
#define FIDL_ENGINE_EVENT_STOP_FIX         (55)
#define FIDL_ENGINE_EVENT_DELETE_AID       (56)
#define FIDL_ENGINE_EVENT_INJ_POS          (57)
#define FIDL_ENGINE_EVENT_SET_POS_MODE     (58)
#define FIDL_ENGINE_EVENT_SET_TIME         (59)
#define FIDL_ENGINE_EVENT_MAX              (60)

#define LCMT_HW_CAPABILITY_UNKNOWN         (0x0)
#define LCMT_HW_CAPABILITY_TIME_BASED_TRACKING_BIT (0x1)
#define LCMT_HW_CAPABILITY_GNSS_MEAS_BIT   (0x2)

#define LOG_TAG "Fidl-If-Lib "

#define LOC_FIDL_CAPABILITIES_FILE "/data/vendor/location/fidlCapabilities.txt"

typedef struct {
    LOC_API_ADAPTER_EVENT_MASK_T mask;
    bool isMaster;
    uint64_t *supportedMask;
} fidlOpenMsg;

typedef struct {
    LocPositionMode mode;
    LocGpsPositionRecurrence recurrence;
    uint32_t min_interval;
    uint32_t preferred_accuracy;
    uint32_t preferred_time;
    bool share_position;
    char credentials[14];
    char provider[8];
    GnssPowerMode powerMode;
    uint32_t timeBetweenMeasurements;
} fidlStartFixMsg;

typedef struct {
    GnssAidingData data;
} fidlDeleteAidingMsg;

typedef struct {
    LocPositionMode mode;
    LocGpsPositionRecurrence recurrence;
    uint32_t min_interval;
    uint32_t preferred_accuracy;
    uint32_t preferred_time;
    bool share_position;
    char credentials[14];
    char provider[8];
    GnssPowerMode powerMode;
    uint32_t timeBetweenMeasurements;
} fidlSetPosModeMsg;

typedef struct {
    LocGpsUtcTime time;
    int64_t timeReference;
    int uncertainty;
} fidlSetTimeMsg;

typedef struct {
    fidlEngineMessage msgType;
    void* contextIn;
    union {
        fidlOpenMsg msgOpen;
        fidlStartFixMsg msgStartFix;
        fidlDeleteAidingMsg msgDeleteAid;
        fidlSetPosModeMsg msgPosMode;
        fidlSetTimeMsg  msgSetTime;
    } u;
} fidlEngineMsg;

typedef struct {
    void *fidlLocApiContext;
    fidlEngineState_t fidlEngineState;
    const FidlInterfaceEvent* eventCallback;
    const locClientFidlInterfaceReq* locClientFildReq;
    locClientFidlInterfaceEvent locClientFildEvent;
    void *fidlIfLibFileHandle;
    bool isStartCommandInQ;
    fidlSetPosModeMsg startCommandInQ;
    uint32_t currentGpsTimeOfWeekMs;
    uint64_t systemTimeAtGpsTOW;
} fidlThreadContext;


enum loc_api_adapter_err handleLocApiOpen(uint64_t requestedMask, bool isMaster,
             uint64_t *supportedMask, fidlFeatureList * fidlFeatures,
             void *context);
enum loc_api_adapter_err handleLocApiClose(void *context);
enum loc_api_adapter_err handleLocApiStartFix(fidlPosMode& fixCriteria, void *context);
enum loc_api_adapter_err handleLocApiStopFix(void *context);
enum loc_api_adapter_err handleLocApiDeleteAidingData(const GnssAidingData& data, void *context);
enum loc_api_adapter_err handleLocApiInjectPosition(const GnssLocationInfoNotification &locationInfo,
            bool onDemandCpi, void *context);
enum loc_api_adapter_err handleLocApiSetTime(LocGpsUtcTime time, int64_t timeReference,
            int uncertainty, void *context);
enum loc_api_adapter_err handleLocApiSetXtraData(char* data, int length,
            void *context);
enum loc_api_adapter_err handleLocApiAtlOpenStatus(int handle, int is_succ,
            char* apn, uint32_t apnLen, AGpsBearerType bear, LocAGpsType agpsType,
            LocApnTypeMask mask, void *context);
enum loc_api_adapter_err handleLocApiAtlCloseStatus(int handle, int is_succ,
            void *context);
enum loc_api_adapter_err handleLocApiSetPositionMode(fidlPosMode& posMode,
            void *context);
enum loc_api_adapter_err handleLocApiSetServerFidl(const char* url, int len,
            LocServerType type, void *context);
enum loc_api_adapter_err handleLocApiInformNiResponse(GnssNiResponse userResponse,
            const void* passThroughData, void *context);
enum loc_api_adapter_err handleLocApiSetSUPLVersionFidl(GnssConfigSuplVersion version,
            void *context);
enum loc_api_adapter_err handleLocApiSetNMEATypesFidl(uint32_t typesMask,
            void *context);
enum loc_api_adapter_err handleLocApiSetLPPConfigFidl(GnssConfigLppProfileMask profileMask,
            void *context);
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
                        float velocityBiasVarianceRandomWalk, void *context);
enum loc_api_adapter_err handleLocApiSetSensorPerfControlConfigFidl(int controlMode,
                        int accelSamplesPerBatch,
                        int accelBatchesPerSec,
                        int gyroSamplesPerBatch,
                        int gyroBatchesPerSec,
                        int accelSamplesPerBatchHigh,
                        int accelBatchesPerSecHigh,
                        int gyroSamplesPerBatchHigh,
                        int gyroBatchesPerSecHigh,
                        int algorithmConfig, void *context);
enum loc_api_adapter_err handleLocApiSetAGLONASSProtocolFidl
            (GnssConfigAGlonassPositionProtocolMask aGlonassProtocol, void *context);
enum loc_api_adapter_err handleLocApiSetLPPeProtocolCpFidl
            (GnssConfigLppeControlPlaneMask lppeCP, void *context);
enum loc_api_adapter_err handleLocApiSetLPPeProtocolUpFidl
            (GnssConfigLppeUserPlaneMask lppeUP, void *context);
enum loc_api_adapter_err handleLocApiConvertSuplVersion(const uint32_t suplVersion,
             GnssConfigSuplVersion *suplConfig, void *context);
enum loc_api_adapter_err handleLocApiConvertLppProfile(const uint32_t lppProfile,
             GnssConfigLppProfileMask *gnssLppProfileMask, void *context);
enum loc_api_adapter_err handleLocApiConvertLppeCp(const uint32_t lppeControlPlaneMask,
             GnssConfigLppeControlPlaneMask *gnssLppControlPlanMask, void *context);
enum loc_api_adapter_err handleLocApiConvertLppeUp(const uint32_t lppeUserPlaneMask,
         GnssConfigLppeUserPlaneMask *gnssLppUpMask, void *context);
enum loc_api_adapter_err handleLocApiGetWwanZppFix(void *context);
enum loc_api_adapter_err handleLocApiGetBestAvailableZppFix(void *context);
enum loc_api_adapter_err handleLocApiInstallAGpsCert(const LocDerEncodedCertificate* pData,
                                size_t length,
                                uint32_t slotBitMask,
                                void *context);

enum loc_api_adapter_err handlFidlSetBlacklistSv(const GnssSvIdConfig& config, void *context);

enum loc_api_adapter_err handleLocApiGetBlacklistSv(void *context);

enum loc_api_adapter_err handleLocApiSetConstellationControl(const GnssSvTypeConfig& config,
        void *context);

enum loc_api_adapter_err handleLocApiGetConstellationControl(void *context);

enum loc_api_adapter_err handleLocApiResetConstellationControl(void *context);

enum loc_api_adapter_err handleLocApiRequestXtraConfigInfo(const uint32_t configMask,
                                             const uint32_t configReqSource, void *context);

void* processFidlEngineMsgThread(void * argv);
fidlEngineMsg* recvMsg4FidlEngine(void);
bool sendMsg2FidlEngine(fidlEngineMsg *sndMsg);

#endif /* LOC_FIDL_MAIN_H */
