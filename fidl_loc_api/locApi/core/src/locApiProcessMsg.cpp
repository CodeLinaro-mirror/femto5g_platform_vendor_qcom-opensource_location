/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <dlfcn.h>

#include "msg_q.h"
#include "gps_extended_c.h"
#include "LocationDataTypes.h"
#include "loc_gps.h"
#include "loc_fidl_interface.h"
#include "loc_pla.h"
#include "log_util.h"
#include "locFidlMain.h"
#include "locClientFidlIf.h"

#define LOC_CLIENT_FIDL_IF_LIB_NAME  "libLocationFidlV02.so"

void eventLocClientFidlReportPosition(UlpLocation& location,
                    GpsLocationExtended& locationExtended,
                    enum loc_sess_status status,
                    LocPosTechMask loc_technology_mask,
                    GnssDataNotification* pDataNotify,
                    int msInWeek, void *context) {

    fidlThreadContext *fidlContext = (fidlThreadContext *) context;

    if (NULL != fidlContext) {
        fidlContext->eventCallback->fidlReportPosition(location, locationExtended,
           status, loc_technology_mask, pDataNotify, msInWeek, fidlContext->fidlLocApiContext);
    }
}

void eventLocClientFidlReportSv(GnssSvNotification& svNotify, void *context) {
    fidlThreadContext *fidlContext = (fidlThreadContext *) context;

    if (NULL != fidlContext) {
        fidlContext->eventCallback->fidlReportSv(svNotify,
           fidlContext->fidlLocApiContext);
    }
}

void eventLocClientFidlReportSvMeasurement(GnssMeasurements &svMeasurementSet,
        void *context) {
    fidlThreadContext *fidlContext = (fidlThreadContext *) context;

    if (NULL != fidlContext) {
        fidlContext->eventCallback->fidlReportGnssMeasurementData (svMeasurementSet,
           0, fidlContext->fidlLocApiContext);
    }
}

void eventLocClientFidlReportNmea(const char* nmea, int length, void *context) {
    fidlThreadContext *fidlContext = (fidlThreadContext *) context;

    if (NULL != fidlContext) {
        fidlContext->eventCallback->fidlReportNmea(nmea, length,
            fidlContext->fidlLocApiContext);
    }
}

void eventLocClientFidlReportGnssMeasurementData(GnssMeasurements& measurements,
        int msInWeek, void *context) {
    fidlThreadContext *fidlContext = (fidlThreadContext *) context;

    if (NULL != fidlContext) {
        fidlContext->eventCallback->fidlReportGnssMeasurementData (measurements,
           msInWeek, fidlContext->fidlLocApiContext);
    }
}

void eventLocClientFidlReportGnssConfig(uint32_t sessionId, const GnssConfig& gnssConfig,
        void *context) {
    fidlThreadContext *fidlContext = (fidlThreadContext *) context;

    if (NULL != fidlContext) {
        fidlContext->eventCallback->fidlReportGnssConfig(sessionId,
           gnssConfig, fidlContext->fidlLocApiContext);
    }
}


void updateLocClientFidlEvent(fidlThreadContext *fidlContext) {
   if (NULL != fidlContext) {
        fidlContext->locClientFildEvent.locClientFidlReportPosition
            = eventLocClientFidlReportPosition;
        fidlContext->locClientFildEvent.locClientFidlReportSv
            = eventLocClientFidlReportSv;

        fidlContext->locClientFildEvent.locClientFidlReportSvMeasurement
            = eventLocClientFidlReportSvMeasurement;

        fidlContext->locClientFildEvent.locClientFidlReportNmea
            = eventLocClientFidlReportNmea;

        fidlContext->locClientFildEvent.locClientFidlReportGnssMeasurementData
            = eventLocClientFidlReportGnssMeasurementData;

        fidlContext->locClientFildEvent.locClientFidlReportGnssConfig
            = eventLocClientFidlReportGnssConfig;
    }

}


void processFidlInitMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    const char * libName = nullptr;
    const char *error = nullptr;

    LOC_LOGD("[%s]---> %p ", __func__, fidlContext);

    if (NULL != fidlContext){

        updateLocClientFidlEvent(fidlContext);
        libName = LOC_CLIENT_FIDL_IF_LIB_NAME;

        if ((fidlContext->fidlIfLibFileHandle = dlopen(libName, RTLD_NOW)) != nullptr) {
            LOC_LOGD("[%s] %s is present", __func__, libName);

            get_locClientFidl_if_api_t getter = (get_locClientFidl_if_api_t)
                        dlsym(fidlContext->fidlIfLibFileHandle, "get_locClientFidl_if_api");
            if ((error = dlerror()) != nullptr) {
                LOC_LOGE("[%s] %s get_locClientFidl_if_api_t failed, error = %s",
                         __func__, libName, error);
                getter = nullptr;
            }

            if (getter != nullptr) {
                fidlContext->locClientFildReq = (getter)(&(fidlContext->locClientFildEvent),
                                                     ((void *)fidlContext));
                if (fidlContext->locClientFildReq != nullptr) {
                    LOC_LOGD("[%s] interface ", __func__ );
                    fidlContext->fidlEngineState = FIDL_ENGINE_STATE_RESET;
                    return;
                } else {
                    LOC_LOGE("[%s] %s LOC Client FIDL IF lib provided Command Interface as NULL",
                         __func__, libName);
                }
            }
        } else {
            char *errorDll = dlerror();

            LOC_LOGE("[%s] %s failed to load, handle %p error: %s", __func__, libName,
                  fidlContext->fidlIfLibFileHandle,
                  ((nullptr != errorDll) ? errorDll : "No Error"))
        }
    }

    LOC_LOGD("[%s]<---  ", __func__ );
}

void processFidlOpenMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    static uint64_t supportedMask;
    static locClientFidlFeatureList fidlFeatures;

    LOC_LOGD("[%s]---> ", __func__);

    if ((nullptr != rxMsg) && (nullptr != fidlContext) &&
        (nullptr != fidlContext->locClientFildReq) &&
        (nullptr != fidlContext->locClientFildReq->locClientFidlOpen)) {
        fidlContext->locClientFildReq->locClientFidlOpen(rxMsg->u.msgOpen.mask,
            rxMsg->u.msgOpen.isMaster, &supportedMask, &fidlFeatures,
            ((void *)fidlContext));
    }

    if (NULL != fidlContext->eventCallback->fidlHandleEngineUpEvent) {
        fidlContext->eventCallback->fidlHandleEngineUpEvent(rxMsg->contextIn);
    }

}

void processFidlCloseMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);

    if ((nullptr != rxMsg) && (nullptr != fidlContext) &&
        (nullptr != fidlContext->locClientFildReq) &&
        (nullptr != fidlContext->locClientFildReq->locClientFidlClose)) {
        fidlContext->locClientFildReq->locClientFidlClose(((void *)fidlContext));
    }
}

void processFidlStartMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    static locClientFidlPosMode posMode;

    LOC_LOGD("[%s]---> ", __func__);

    if ((nullptr == rxMsg) || (nullptr == fidlContext)) {
        LOC_LOGE("[%s] Error Message ", __func__);
        return;
    }
    LOC_LOGD("[%s]---> 2", __func__);
    if ((nullptr != fidlContext->locClientFildReq) &&
         (nullptr != fidlContext->locClientFildReq->locClientFidlStartFix)) {
         memset(&posMode, 0x00, sizeof(locClientFidlPosMode));

         posMode.mode         = rxMsg->u.msgStartFix.mode;
         posMode.recurrence   = rxMsg->u.msgStartFix.recurrence;
         posMode.min_interval = rxMsg->u.msgStartFix.min_interval;
         posMode.powerMode    = rxMsg->u.msgStartFix.powerMode;
         posMode.share_position = rxMsg->u.msgStartFix.share_position;
         posMode.preferred_time = rxMsg->u.msgStartFix.preferred_time;
         posMode.preferred_accuracy = rxMsg->u.msgStartFix.preferred_accuracy;
         posMode.timeBetweenMeasurements = rxMsg->u.msgStartFix.timeBetweenMeasurements;

         fidlContext->locClientFildReq->locClientFidlStartFix(posMode, ((void *)fidlContext));
    }

    LOC_LOGD("[%s]<--- ", __func__);

}

void processFidlStopMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
    if ((nullptr != rxMsg) && (nullptr != fidlContext) &&
        (nullptr != fidlContext->locClientFildReq) &&
        (nullptr != fidlContext->locClientFildReq->locClientFidlStopFix)) {
        fidlContext->locClientFildReq->locClientFidlStopFix(((void *)fidlContext));
    }
}

void processFidlDeleteAidMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
}

void processFidlInjectPositionMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
}

void processFidlSetPosModeMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
}

void processFidlSetTimeMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
}

void processFidlReqXTRAMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
    /* Supported Single */
    if ((NULL == rxMsg) || (NULL == fidlContext)) {
        LOC_LOGE("[%s] Error: Msg or Context is NULL ", __func__);
        return;
    }
}

void processFidlEngineMessages(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
    if ((NULL != rxMsg) && (NULL != fidlContext)) {
        switch (rxMsg->msgType) {
            case FIDL_ENGINE_MSG_INIT: {
                processFidlInitMsg(rxMsg, fidlContext);
            }
            break;

            case FIDL_ENGINE_MSG_OPEN: {
                processFidlOpenMsg(rxMsg, fidlContext);
            }
            break;

            case FIDL_ENGINE_MSG_CLOSE: {
                processFidlCloseMsg(rxMsg, fidlContext);
            }
            break;

            case FIDL_ENGINE_MSG_START_FIX: {
                processFidlStartMsg(rxMsg, fidlContext);
            }
            break;

            case FIDL_ENGINE_MSG_STOP_FIX: {
                processFidlStopMsg(rxMsg, fidlContext);
            }
            break;

            case FIDL_ENGINE_MSG_DELETE_AID: {
                processFidlDeleteAidMsg(rxMsg, fidlContext);
            }
            break;

            case FIDL_ENGINE_MSG_INJ_POS: {
                processFidlInjectPositionMsg(rxMsg, fidlContext);
            }
            break;

            case FIDL_ENGINE_MSG_SET_POS_MODE: {
                processFidlSetPosModeMsg(rxMsg, fidlContext);
            }
            break;

            case FIDL_ENGINE_MSG_SET_TIME: {
                processFidlSetTimeMsg(rxMsg, fidlContext);
            }
            break;

            case FIDL_ENGINE_MSG_XTRA_REQ: {
                processFidlReqXTRAMsg(rxMsg, fidlContext);
            }
            break;

            default: {
                LOC_LOGD("[%s] Msg is not supported %d", __func__, rxMsg->msgType);
            }
            break;
        }
    }
    LOC_LOGD("[%s]<---- ", __func__);
    return;
}


void* processFidlEngineMsgThread(void * argv)
{
    bool isCleanUpReceived = false;
    fidlEngineMsg *rxMsg = NULL;
    fidlThreadContext *fidlEngContext = NULL;

    if (NULL != argv) {
        fidlEngContext = (fidlThreadContext *)argv;
    }

    do {
        rxMsg  = recvMsg4FidlEngine();
        if (NULL != rxMsg) {
            if (FIDL_ENGINE_MSG_CLEANUP == rxMsg->msgType) {
                isCleanUpReceived = true;
            }
            processFidlEngineMessages(rxMsg, fidlEngContext);
        } else {
            isCleanUpReceived = true;
        }

    } while (false == isCleanUpReceived);
    return fidlEngContext;
}
