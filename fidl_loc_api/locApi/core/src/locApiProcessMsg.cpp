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

    double current_timestamp = static_cast<double>(location.gpsLocation.timestamp);
    double current_min_interval = static_cast<double>(fidlContext->startCommandInQ.min_interval);

    if (NULL != fidlContext) {

       if (fidlContext->startCommandInQ.min_interval > 1000) {
           if (0.0 == fmod((current_timestamp / 1000.0), (current_min_interval / 1000.0))) {
               LOC_LOGD("[%s] min_interval %d timestamp %" PRId64" ", __func__,
                   fidlContext->startCommandInQ.min_interval,
                   location.gpsLocation.timestamp);
               fidlContext->eventCallback->fidlReportPosition(location,
                   locationExtended, status, loc_technology_mask,
                   pDataNotify, msInWeek, fidlContext->fidlLocApiContext);
           }

       } else if (1000 == fidlContext->startCommandInQ.min_interval) {
           if (0.0 == fmod(current_timestamp, 1000.0)) {
               LOC_LOGD("[%s] min_interval %d timestamp %" PRId64" ", __func__,
                   fidlContext->startCommandInQ.min_interval,
                   location.gpsLocation.timestamp);

               fidlContext->eventCallback->fidlReportPosition(location,
                   locationExtended, status, loc_technology_mask,
                   pDataNotify, msInWeek, fidlContext->fidlLocApiContext);
           }
       } else if (fidlContext->startCommandInQ.min_interval > 100) {
           if (0.0 == fmod((current_timestamp / 100.0), (current_min_interval / 100.0))) {
               LOC_LOGD("[%s] min_interval %d timestamp %" PRId64" ", __func__,
                   fidlContext->startCommandInQ.min_interval,
                   location.gpsLocation.timestamp);

               fidlContext->eventCallback->fidlReportPosition(location,
                   locationExtended, status, loc_technology_mask,
                   pDataNotify, msInWeek, fidlContext->fidlLocApiContext);
           }
       } else {
           /* Min interval is less than or equal to 100 mSec */
           fidlContext->eventCallback->fidlReportPosition(location,
                   locationExtended, status, loc_technology_mask,
                   pDataNotify, msInWeek, fidlContext->fidlLocApiContext);
       }
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

void eventLocClientFidlReportHardwareStatus(uint32_t hardwareStatus, void *context) {
    fidlThreadContext *fidlContext = (fidlThreadContext *) context;

    if (NULL != fidlContext) {
        switch(hardwareStatus) {
           case FIDL_HARDWARE_UP: {
               LOC_LOGD("[%s]---> FIDL_HARDWARE_UP ", __func__);
               fidlContext->fidlEngineState = FIDL_ENGINE_STATE_HW_UP;
               fidlContext->eventCallback->fidlHandleEngineUpEvent(fidlContext->fidlLocApiContext);

               if (true == fidlContext->isStartCommandInQ) {
                   static fidlEngineMsg sndMsg;
                   memset(&sndMsg, 0x00, sizeof(sndMsg));

                   sndMsg.msgType = FIDL_ENGINE_MSG_START_FIX;
                   sndMsg.contextIn = context;
                   sndMsg.u.msgStartFix.mode = fidlContext->startCommandInQ.mode;
                   sndMsg.u.msgStartFix.min_interval
                           = fidlContext->startCommandInQ.min_interval;
                   sndMsg.u.msgStartFix.preferred_accuracy
                           = fidlContext->startCommandInQ.preferred_accuracy;
                   sendMsg2FidlEngine(&sndMsg);
                   fidlContext->isStartCommandInQ = false;
               }

           }
           break;
           case FIDL_HARDWARE_DOWN: {
               LOC_LOGD("[%s]---> FIDL_HARDWARE_DOWN ", __func__);
               fidlContext->eventCallback->fidlHandleEngineDownEvent(fidlContext->fidlLocApiContext);
               if (FIDL_ENGINE_STATE_IN_SESSION == fidlContext->fidlEngineState) {
                   fidlContext->fidlEngineState = FIDL_ENGINE_STATE_IN_SESSION_HW_DOWN;
                   fidlContext->isStartCommandInQ = true;
               }
           }
           break;
           deafult: {
          }
       }
    }
}

void eventLocClientFidlReportCapabilities(uint32_t hwCapabilities, void *context) {
    /* Store Capabilities in file */

   if (0 != hwCapabilities) {
        FILE *fp = NULL;
        fp = fopen(LOC_FIDL_CAPABILITIES_FILE, "w+" );
        if (NULL != fp) {
            fwrite(&hwCapabilities, 1, sizeof(uint32_t), fp);
            fclose(fp);
        }
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

        fidlContext->locClientFildEvent.locClientFidlHardWareStatus
            = eventLocClientFidlReportHardwareStatus;

        fidlContext->locClientFildEvent.locClientFidlCapabilities
            = eventLocClientFidlReportCapabilities;
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
        ((FIDL_ENGINE_STATE_RESET == fidlContext->fidlEngineState) ||
         (FIDL_ENGINE_STATE_INIT == fidlContext->fidlEngineState) ||
         (FIDL_ENGINE_STATE_HW_DOWN == fidlContext->fidlEngineState)) &&
        (nullptr != fidlContext->locClientFildReq) &&
        (nullptr != fidlContext->locClientFildReq->locClientFidlOpen)) {
            fidlContext->locClientFildReq->locClientFidlOpen(rxMsg->u.msgOpen.mask,
                rxMsg->u.msgOpen.isMaster, &supportedMask, &fidlFeatures,
                ((void *)fidlContext));
            fidlContext->fidlEngineState = FIDL_ENGINE_STATE_HW_OPEN_REQ;
          LOC_LOGD("[%s]---> sending HW down", __func__);
          fidlContext->eventCallback->fidlHandleEngineDownEvent(rxMsg->contextIn);
        LOC_LOGD("[%s]---> HW_OPEN_REQ", __func__);
    } else {
                LOC_LOGD("[%s]---> HW_OPEN_REQ failed fidlContext->fidlEngineState:%d,"
                         " locClientFildReq:%p, locClientFidlOpen:%p", __func__,
                          (unsigned int)fidlContext->fidlEngineState,
                          fidlContext->locClientFildReq,
                          fidlContext->locClientFildReq->locClientFidlOpen);
    }

    if ((NULL != fidlContext->eventCallback->fidlHandleEngineUpEvent) &&
        ((FIDL_ENGINE_STATE_HW_UP == fidlContext->fidlEngineState) ||
         (FIDL_ENGINE_STATE_IN_SESSION == fidlContext->fidlEngineState))) {
           LOC_LOGD("[%s]---> fidlHandleEngineUpEvent call", __func__);
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
    /* Store Start Request*/
    fidlContext->startCommandInQ.mode = rxMsg->u.msgStartFix.mode;
    fidlContext->startCommandInQ.recurrence     = rxMsg->u.msgStartFix.recurrence;
    fidlContext->startCommandInQ.min_interval   = rxMsg->u.msgStartFix.min_interval;
    fidlContext->startCommandInQ.powerMode      = rxMsg->u.msgStartFix.powerMode;
    fidlContext->startCommandInQ.share_position = rxMsg->u.msgStartFix.share_position;
    fidlContext->startCommandInQ.preferred_time = rxMsg->u.msgStartFix.preferred_time;
    fidlContext->startCommandInQ.preferred_accuracy
                                = rxMsg->u.msgStartFix.preferred_accuracy;
    fidlContext->startCommandInQ.timeBetweenMeasurements
                                = rxMsg->u.msgStartFix.timeBetweenMeasurements;

    if ((FIDL_ENGINE_STATE_HW_UP == fidlContext->fidlEngineState) ||
        (FIDL_ENGINE_STATE_IN_SESSION == fidlContext->fidlEngineState)) {
         memset(&posMode, 0x00, sizeof(locClientFidlPosMode));

         posMode.mode         = rxMsg->u.msgStartFix.mode;
         posMode.recurrence   = rxMsg->u.msgStartFix.recurrence;
         posMode.min_interval = rxMsg->u.msgStartFix.min_interval;
         posMode.powerMode    = rxMsg->u.msgStartFix.powerMode;
         posMode.share_position = rxMsg->u.msgStartFix.share_position;
         posMode.preferred_time = rxMsg->u.msgStartFix.preferred_time;
         posMode.preferred_accuracy = rxMsg->u.msgStartFix.preferred_accuracy;
         posMode.timeBetweenMeasurements = rxMsg->u.msgStartFix.timeBetweenMeasurements;
         if ((nullptr != fidlContext->locClientFildReq) &&
             (nullptr != fidlContext->locClientFildReq->locClientFidlStartFix)) {
             fidlContext->locClientFildReq->locClientFidlStartFix(posMode, ((void *)fidlContext));
         } else {
             LOC_LOGE("[%s] FidlStartFix interface is not registered.", __func__);
         }
         fidlContext->isStartCommandInQ = false;
         fidlContext->fidlEngineState = FIDL_ENGINE_STATE_IN_SESSION;
    } else {
         /* Queue start command and wait for Hardware Up */
         fidlContext->isStartCommandInQ = true;
    }
    LOC_LOGD("[%s]<--- ", __func__);

}

void processFidlStopMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
    if ((nullptr != rxMsg) && (nullptr != fidlContext)) {

        if (FIDL_ENGINE_STATE_IN_SESSION_HW_DOWN == fidlContext->fidlEngineState) {
            fidlContext->fidlEngineState = FIDL_ENGINE_STATE_HW_DOWN;
        } else if (FIDL_ENGINE_STATE_IN_SESSION == fidlContext->fidlEngineState) {
            if ((nullptr != fidlContext->locClientFildReq) &&
                (nullptr != fidlContext->locClientFildReq->locClientFidlStopFix)) {
                fidlContext->locClientFildReq->locClientFidlStopFix(((void *)fidlContext));
            }
            fidlContext->fidlEngineState = FIDL_ENGINE_STATE_HW_UP;
        }
        fidlContext->isStartCommandInQ = false;
    }
}

void processFidlDeleteAidMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    static GnssAidingData delData;
    LOC_LOGD("[%s]---> ", __func__);
    if ((nullptr != rxMsg) && (nullptr != fidlContext) &&
        (nullptr != fidlContext->locClientFildReq) &&
        (nullptr != fidlContext->locClientFildReq->locClientFidlDeleteAidingData)) {
        memset(&delData, 0x00, sizeof(GnssAidingData));
        delData = rxMsg->u.msgDeleteAid.data;
        fidlContext->locClientFildReq->locClientFidlDeleteAidingData(delData,
                                        ((void *)fidlContext));
    }
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
