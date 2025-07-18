/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#include <stdint.h>
#include <errno.h>
#include "msg_q.h"
#include "gps_extended_c.h"
#include "LocationDataTypes.h"
#include "loc_gps.h"
#include "loc_pla.h"
#include "log_util.h"
#include "loc_fidl_interface.h"
#include "locFidlMain.h"

#define LOG_TAG "Fidl-If-Lib "

/** FIDL Interface Q */
void* locFidlQ;
static pthread_t mlocFidlThreadId;
/** FIDL Event Q */
void* locFidlEventQ;
static pthread_t mlocFidlEventThreadId;

static FidlInterfaceReq locApiReq;
static fidlThreadContext mFidlContextThrd{
        .fidlEngineState = FIDL_ENGINE_STATE_NONE,
    };


FidlInterfaceReq*  getFidlInterface(void)
{
    memset(&(locApiReq), 0x00, sizeof(FidlInterfaceReq));

    locApiReq.fidlOpen = handleLocApiOpen;
    locApiReq.fidlClose = handleLocApiClose;
    locApiReq.fidlStartFix = handleLocApiStartFix;
    locApiReq.fidlStopFix = handleLocApiStopFix;
    locApiReq.fidlDeleteAidingData = handleLocApiDeleteAidingData;
    locApiReq.fidlInjectPosition = handleLocApiInjectPosition;
    locApiReq.fidlSetTime = handleLocApiSetTime;
    locApiReq.fidlSetXtraData = handleLocApiSetXtraData;
    locApiReq.fidlAtlOpenStatus = handleLocApiAtlOpenStatus;
    locApiReq.fidlAtlCloseStatus = handleLocApiAtlCloseStatus;
    locApiReq.fidlSetPositionMode = handleLocApiSetPositionMode;
    locApiReq.fidlSetServerSync = handleLocApiSetServerFidl;
    locApiReq.fidlInformNiResponse = handleLocApiInformNiResponse;
    locApiReq.fidlSetSUPLVersionSync = handleLocApiSetSUPLVersionFidl;
    locApiReq.fidlSetNMEATypesSync = handleLocApiSetNMEATypesFidl;
    locApiReq.fidlSetLPPConfigSync = handleLocApiSetLPPConfigFidl;
    locApiReq.fidlSetSensorPropertiesSync
                = handleLocApiSetSensorPropertiesFidl;
    locApiReq.fidlSetSensorPerfControlConfigSync
                = handleLocApiSetSensorPerfControlConfigFidl;
    locApiReq.fidlSetAGLONASSProtocolSync = handleLocApiSetAGLONASSProtocolFidl;
    locApiReq.fidlSetLPPeProtocolCpSync = handleLocApiSetLPPeProtocolCpFidl;
    locApiReq.fidlSetLPPeProtocolUpSync = handleLocApiSetLPPeProtocolUpFidl;
    locApiReq.fidlConvertSuplVersion = handleLocApiConvertSuplVersion;
    locApiReq.fidlConvertLppProfile = handleLocApiConvertLppProfile;
    locApiReq.fidlConvertLppeCp = handleLocApiConvertLppeCp;
    locApiReq.fidlConvertLppeUp = handleLocApiConvertLppeUp;
    locApiReq.fidlGetWwanZppFix = handleLocApiGetWwanZppFix;
    locApiReq.fidlGetBestAvailableZppFix = handleLocApiGetBestAvailableZppFix;
    locApiReq.fidlInstallAGpsCert = handleLocApiInstallAGpsCert;
    locApiReq.fidlSetConstrainedTuncMode = NULL;
    locApiReq.fidlSetPositionAssistedClockEstimatorMode = NULL;
    locApiReq.fidlGetGnssEnergyConsumed = NULL;
    locApiReq.fidlSetBlacklistSv = handlFidlSetBlacklistSv;
    locApiReq.fidlGetBlacklistSv = handleLocApiGetBlacklistSv;
    locApiReq.fidlSetConstellationControl = handleLocApiSetConstellationControl;
    locApiReq.fidlGetConstellationControl = handleLocApiGetConstellationControl;
    locApiReq.fidlResetConstellationControl = handleLocApiResetConstellationControl;
    locApiReq.fidlRequestXtraConfigInfo = handleLocApiRequestXtraConfigInfo;
    return &(locApiReq);
}

enum loc_api_adapter_err fidlInit(void *context) {
    fidlEngineMsg* sndMsg;

    LOC_LOGD("%s] ", __func__);

    sndMsg = (fidlEngineMsg *)malloc(sizeof(fidlEngineMsg));
    memset(sndMsg, 0x00, sizeof(sndMsg));

    sndMsg->msgType = FIDL_ENGINE_MSG_INIT;
    sndMsg->contextIn = context;
    sendMsg2FidlEngine(sndMsg);
    return LOC_API_ADAPTER_ERR_SUCCESS;
}

void setInterfaceEvent(const FidlInterfaceEvent *fidlIfCb) {
    LOC_LOGD("%s] --> ", __func__);
    mFidlContextThrd.eventCallback = fidlIfCb;
}

void fidlEngineFreeMsg(void *msg)
{
    LOC_LOGD("%s] --> ", __func__);
    if (NULL != msg) {
        free(msg);
    }
}

bool sendMsg2FidlEngine(fidlEngineMsg *sndMsg)
{
    bool retval = true;
    msq_q_err_type result = eMSG_Q_SUCCESS;

    result = msg_q_snd(locFidlQ, (void *)sndMsg, NULL);
    if (eMSG_Q_SUCCESS != result) {
         retval = false;
    }
    return retval;
}

fidlEngineMsg* recvMsg4FidlEngine(void)
{
    void* fidlEngMsg = NULL;
    msq_q_err_type result = eMSG_Q_SUCCESS;

    result = msg_q_rcv(locFidlQ, (void **)&fidlEngMsg);
    if (eMSG_Q_SUCCESS != result) {
        LOC_LOGE("%s:%d] fail to receive FIDL Engine msg: \n", __func__, __LINE__);
        return NULL;
    }
    return (fidlEngineMsg*)fidlEngMsg;
}


void initFidlInterface(fidlThreadContext *context)
{
    int result;

    LOC_LOGD("%s] --> ", __func__);
    locFidlQ = (void*)msg_q_init2();
    if (NULL == locFidlQ) {
        LOC_LOGE("%s:%d] fail to Init FIDL Interface message queue: \n", __func__, __LINE__);
        return;
    }

    /* Create Thread  */
    result = pthread_create(&mlocFidlThreadId, NULL, processFidlEngineMsgThread, context);
    if (0 != result) {
        LOC_LOGE("%s: FIDL Message thread creation failed err= %d \n", __func__, result);
    }
    return;
}


const FidlInterfaceReq* get_fidl_if_api(const FidlInterfaceEvent* eventCallback, void *context)
{
    FidlInterfaceReq*  fidlRqIf = getFidlInterface();

    LOC_LOGD("%s] --> context:%p State:%d ", __func__, context, mFidlContextThrd.fidlEngineState);
    if (FIDL_ENGINE_STATE_NONE == mFidlContextThrd.fidlEngineState) {
        mFidlContextThrd.isStartCommandInQ = false;
        mFidlContextThrd.fidlLocApiContext = context;
        mFidlContextThrd.fidlEngineState = FIDL_ENGINE_STATE_INIT;
        mFidlContextThrd.currentGpsTimeOfWeekMs = 0;
        mFidlContextThrd.systemTimeAtGpsTOW = 0;
        setInterfaceEvent(eventCallback);
        initFidlInterface(&mFidlContextThrd);
        usleep(100);
        fidlInit(context);
    }
    return fidlRqIf;
}
