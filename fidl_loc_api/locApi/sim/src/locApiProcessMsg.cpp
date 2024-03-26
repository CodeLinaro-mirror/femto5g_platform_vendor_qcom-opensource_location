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
#include "loc_fidl_interface.h"
#include "loc_pla.h"
#include "log_util.h"
#include "locFidlMain.h"

void processFidlInitMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
}

void processFidlOpenMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
    /* Supported Single */
    if ((NULL == rxMsg) || (NULL == fidlContext)) {
        return;
    }

    if (NULL != fidlContext->eventCallback->fidlHandleEngineUpEvent) {
        fidlContext->eventCallback->fidlHandleEngineUpEvent(rxMsg->contextIn);
    }

}

void processFidlCloseMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
}

void processFidlStartMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    UlpLocation location;
    GpsLocationExtended locationExtended;
    enum loc_sess_status status = LOC_SESS_SUCCESS;
    LocPosTechMask loc_technology_mask = LOC_POS_TECH_MASK_SATELLITE;
    GnssDataNotification dataNotify;
    int msInWeek = 0;

    LOC_LOGD("[%s]---> ", __func__);

    /* Supported Single */
    if ((NULL == rxMsg) || (NULL == fidlContext)) {
        LOC_LOGE("[%s] Error Message ", __func__);
        return;
    }
    LOC_LOGD("[%s]---> 2 PTR %d ", __func__,fidlContext->eventCallback->fidlReportPosition);
    /* Send Simulated PVT Report */
    if (NULL != fidlContext->eventCallback->fidlReportPosition) {
        memset(&location, 0x00, sizeof(location));
        memset(&locationExtended, 0x00, sizeof(locationExtended));

        LOC_LOGD("[%s]---> ", __func__);

        location.gpsLocation.size = sizeof(LocGpsLocation);
        location.gpsLocation.flags =
           (LOC_GPS_LOCATION_HAS_LAT_LONG | LOC_GPS_LOCATION_HAS_ALTITUDE |
            LOC_GPS_LOCATION_HAS_SPEED | LOC_GPS_LOCATION_HAS_ACCURACY |
            LOC_GPS_LOCATION_HAS_VERT_UNCERTAINITY | LOC_GPS_LOCATION_HAS_SOURCE_INFO);

        location.gpsLocation.latitude = 12.9716;
        location.gpsLocation.longitude = 77.5946;
        location.gpsLocation.altitude = 863.287109;
        location.gpsLocation.accuracy = 3.790092;
        location.gpsLocation.vertUncertainity = 10.00;
        location.gpsLocation.speed = 0.1;
        location.position_source = ULP_LOCATION_IS_FROM_GNSS;
        location.tech_mask =
            (LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION | LOC_POS_TECH_MASK_SATELLITE);
        location.unpropagatedPosition = false;

        locationExtended.size = sizeof(locationExtended);
        locationExtended.flags = 0;

        msInWeek = 128990900;
        struct timespec time_info_current;
        if (clock_gettime(CLOCK_REALTIME,&time_info_current) == 0) {
            location.gpsLocation.timestamp = (time_info_current.tv_sec)*1e3 +
                (time_info_current.tv_nsec)/1e6;
            locationExtended.timeStamp.apTimeStamp.tv_sec = time_info_current.tv_sec;
            locationExtended.timeStamp.apTimeStamp.tv_nsec = time_info_current.tv_nsec;
            locationExtended.timeStamp.apTimeStampUncertaintyMs = 0;
        }
        locationExtended.flags |= GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK;
        locationExtended.tech_mask = LOC_POS_TECH_MASK_SATELLITE;

        fidlContext->eventCallback->fidlReportPosition(location, locationExtended,
                status, loc_technology_mask, &dataNotify, msInWeek,
                rxMsg->contextIn);
    }

    LOC_LOGD("[%s]<--- ", __func__);

}

void processFidlStopMsg(fidlEngineMsg *rxMsg, fidlThreadContext *fidlContext)
{
    LOC_LOGD("[%s]---> ", __func__);
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

    if (NULL != fidlContext->eventCallback->fidlRequestXtraData) {
        fidlContext->eventCallback->fidlRequestXtraData(rxMsg->contextIn);
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
