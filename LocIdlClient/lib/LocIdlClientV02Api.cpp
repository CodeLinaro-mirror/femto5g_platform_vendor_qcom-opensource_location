/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#include <iostream>
#include <string>

#ifndef _WIN32
#include <unistd.h>
#endif
#include <chrono>
#include <future>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "locClientFidlIf.h"
#include "LocIdlClientV02Api.h"

#define LOG_TAG "LocIdlClientV02Api"

using namespace std;
#define NSEC_IN_ONE_SEC       (1000000000ULL)   /* nanosec in a sec */

static locClientFidlInterfaceReq fidlLocApiV02Req;
static const locClientFidlInterfaceEvent *fidlLocApiV02EventCb = nullptr;
static void *fidlLocApiContext = nullptr;

void setInterfaceEvent(const locClientFidlInterfaceEvent *fidlEventCb) {
    LOC_LOGD("%s] --> ", __func__);
    fidlLocApiV02EventCb = fidlEventCb;
}

const locClientFidlInterfaceReq*  getInterfaceReq(void)
{
   memset(&(fidlLocApiV02Req), 0x00, sizeof(locClientFidlInterfaceReq));
   LOC_LOGD("%s] --> ", __func__);
   fidlLocApiV02Req.locClientFidlOpen  = locClientFidlOpenReq;
   fidlLocApiV02Req.locClientFidlClose = locClientFidlCloseReq;
   fidlLocApiV02Req.locClientFidlStartFix  = locClientFidlStartFixReq;
   fidlLocApiV02Req.locClientFidlStopFix   = locClientFidlStopFixReq;
   fidlLocApiV02Req.locClientFidlDeleteAidingData = locClientFidlDeleteAidingDataReq;
   fidlLocApiV02Req.locClientFidlSetPositionMode  = locClientFidlSetPositionModeReq;
   fidlLocApiV02Req.locClientFidlSetGpsLockSync   = locClientFidlSetGpsLockSyncReq;
   fidlLocApiV02Req.locClientFidlStartTimeBasedTracking
                               = locClientFidlStartTimeBasedTrackingReq;
   fidlLocApiV02Req.locClientFidlStopTimeBasedTracking
                                = locClientFidlStopTimeBasedTrackingReq;
   return &(fidlLocApiV02Req);
}

static void initFidlCore(void);


LocIdlClientV02Api *LocIdlClientV02Api::mInstance = nullptr;

LocIdlClientV02Api::LocIdlClientV02Api():mMsgTask(new MsgTask("LocIdlClientV02ApiMsgTask"))
{
}

LocIdlClientV02Api::~LocIdlClientV02Api()
{
}

LocIdlClientV02Api* LocIdlClientV02Api::getInstance()
{
    if (nullptr == mInstance) {
        mInstance = new LocIdlClientV02Api();
    }
    return mInstance;
}

class LocIdlClientIpcListener : public ILocIpcListener {
    LocIdlClientV02Api& mClientInstance;
public:
    inline LocIdlClientIpcListener(LocIdlClientV02Api& sc) : mClientInstance(sc) {}
    virtual void onListenerReady() override;
    virtual void onReceive(const char* data,
                    uint32_t length, const LocIpcRecver* recver) override;
};

void LocIdlClientIpcListener::onListenerReady()
{
    LOC_LOGD("%s] -->", __func__);
    mClientInstance.mIsListenerReady = true;
    return;
}

void LocIdlClientV02Api::sendPosRptHelper(
                    EventPosRpt *inMsg,
                    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct sendPosRpt: public LocMsg
    {
        EventPosRpt *pMsg;
        LocIdlClientV02Api           *pObj;
        uint32_t                     size;
        sendPosRpt
        (
            EventPosRpt *msg,
            uint32_t                    inSize,
            LocIdlClientV02Api              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            pObj(Obj),
            size(inSize)
        {}
        virtual void proc() const
        {
            UlpLocation &ulpLoc = pMsg->location;
            GpsLocationExtended &gpsLocExt = pMsg->locationExtended;
            GnssDataNotification dataNotify = pMsg->dataNotify;
            loc_sess_status status = pMsg->status;
            LocPosTechMask loc_technology_mask = pMsg->loc_technology_mask;
            uint32_t msInWeek = pMsg->msInWeek;
            fidlLocApiV02EventCb->locClientFidlReportPosition(ulpLoc, gpsLocExt,status,
                    loc_technology_mask, &dataNotify, msInWeek, fidlLocApiContext);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendPosRpt(inMsg, msgSize, this));
}

void LocIdlClientV02Api::sendPosReport(EventMsgPosPkt *msg) {
    EventPosRpt *posRpt = nullptr;

    posRpt = new EventPosRpt;
    if (nullptr == posRpt)
        return;

    memscpy(&posRpt->location, sizeof(UlpLocation), &msg->posRpt.location, sizeof(UlpLocation));
    memscpy(&posRpt->locationExtended, sizeof(GpsLocationExtended),
                    &msg->posRpt.locationExtended, sizeof(GpsLocationExtended));
    memscpy(&posRpt->dataNotify, sizeof(GnssDataNotification),
                    &msg->posRpt.dataNotify, sizeof(GnssDataNotification));
    posRpt->status = msg->posRpt.status;
    posRpt->loc_technology_mask = msg->posRpt.loc_technology_mask;
    posRpt->msInWeek = msg->posRpt.msInWeek;
    sendPosRptHelper(posRpt, sizeof(EventPosRpt));
}

void LocIdlClientV02Api::sendSvRptHelper(
                    EventSvRpt *inMsg,
                    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct sendSvRpt: public LocMsg
    {
        EventSvRpt *pMsg;
        LocIdlClientV02Api           *pObj;
        uint32_t                     size;
        sendSvRpt
        (
            EventSvRpt *msg,
            uint32_t                    inSize,
            LocIdlClientV02Api              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            pObj(Obj),
            size(inSize)
        {}
        virtual void proc() const
        {
            GnssSvNotification    &svNotify = pMsg->svNotify;
            fidlLocApiV02EventCb->locClientFidlReportSv(svNotify, fidlLocApiContext);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendSvRpt(inMsg, msgSize, this));
}

void LocIdlClientV02Api::sendSvReport(EventMsgSvPkt *msg) {
    EventSvRpt *svRpt = nullptr;

    svRpt = new EventSvRpt;
    if (nullptr == svRpt)
        return;

    memscpy(&svRpt->svNotify, sizeof(GnssSvNotification),
                &msg->svRpt.svNotify, sizeof(GnssSvNotification));
    sendSvRptHelper(svRpt, sizeof(EventSvRpt));
}

void LocIdlClientV02Api::sendMeasRptHelper(
                    EventGnssMeasRpt *inMsg,
                    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct sendMeasRpt: public LocMsg
    {
        EventGnssMeasRpt *pMsg;
        LocIdlClientV02Api           *pObj;
        uint32_t                     size;
        sendMeasRpt
        (
            EventGnssMeasRpt *msg,
            uint32_t                    inSize,
            LocIdlClientV02Api              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            pObj(Obj),
            size(inSize)
        {}
        virtual void proc() const
        {
            GnssMeasurements    &meas = pMsg->measurements;
            fidlLocApiV02EventCb->locClientFidlReportSvMeasurement(meas, fidlLocApiContext);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendMeasRpt(inMsg, msgSize, this));
}

void LocIdlClientV02Api::sendMeasReport(EventMsgGnssMeasPkt *msg) {
    EventGnssMeasRpt *measRpt = nullptr;

    measRpt = new EventGnssMeasRpt;
    if (nullptr == measRpt)
        return;

    memscpy(&measRpt->measurements, sizeof(GnssMeasurements),
                &msg->measRpt.measurements, sizeof(GnssMeasurements));
    sendMeasRptHelper(measRpt, sizeof(EventGnssMeasRpt));
}

void LocIdlClientV02Api::sendNmeaRptHelper(
                    EventNmeaRpt *inMsg,
                    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct sendNmeaRpt: public LocMsg
    {
        EventNmeaRpt *pMsg;
        LocIdlClientV02Api           *pObj;
        uint32_t                     size;
        sendNmeaRpt
        (
            EventNmeaRpt *msg,
            uint32_t                    inSize,
            LocIdlClientV02Api              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            pObj(Obj),
            size(inSize)
        {}
        virtual void proc() const
        {
            char    *nmea = pMsg->nmea;
            int        length = pMsg->length;
            fidlLocApiV02EventCb->locClientFidlReportNmea(nmea, length, fidlLocApiContext);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendNmeaRpt(inMsg, msgSize, this));
}

void LocIdlClientV02Api::sendNmeaReport(EventMsgNmeaPkt *msg) {
    EventNmeaRpt *nmeaRpt = nullptr;

    nmeaRpt = new EventNmeaRpt;
    if (nullptr == nmeaRpt)
        return;

    strlcpy(nmeaRpt->nmea, msg->nmeaRpt.nmea, msg->nmeaRpt.length);
    sendNmeaRptHelper(nmeaRpt, sizeof(EventNmeaRpt));
}

void LocIdlClientV02Api::sendEngineUpEventHelper(
                    EventHwEngStatusConfig *inMsg,
                    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct sendEngineUpEvnt: public LocMsg
    {
        EventHwEngStatusConfig       *pMsg;
        LocIdlClientV02Api           *pObj;
        uint32_t                     size;
        sendEngineUpEvnt
        (
            EventHwEngStatusConfig      *msg,
            uint32_t                    inSize,
            LocIdlClientV02Api              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            pObj(Obj),
            size(inSize)
        {}
        virtual void proc() const
        {
            fidlLocApiV02EventCb->locClientFidlHardWareStatus(FIDL_HARDWARE_UP, fidlLocApiContext);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendEngineUpEvnt(inMsg, msgSize, this));
}


void LocIdlClientV02Api::sendEngineUpEvent() {
   EventHwEngStatusConfig *engineUpMsg = nullptr;

   engineUpMsg = new EventHwEngStatusConfig;
   if (nullptr == engineUpMsg)
       return;

   sendEngineUpEventHelper(engineUpMsg, sizeof(EventHwEngStatusConfig));
}


void LocIdlClientV02Api::sendEngineDownEventHelper(
                    EventHwEngStatusConfig *inMsg,
                    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct sendEngineDownEvnt: public LocMsg
    {
        EventHwEngStatusConfig       *pMsg;
        LocIdlClientV02Api           *pObj;
        uint32_t                     size;
        sendEngineDownEvnt
        (
            EventHwEngStatusConfig      *msg,
            uint32_t                    inSize,
            LocIdlClientV02Api              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            pObj(Obj),
            size(inSize)
        {}
        virtual void proc() const
        {
            fidlLocApiV02EventCb->locClientFidlHardWareStatus(FIDL_HARDWARE_DOWN, fidlLocApiContext);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendEngineDownEvnt(inMsg, msgSize, this));
}


void LocIdlClientV02Api::sendEngineDownEvent() {
   EventHwEngStatusConfig *engineDownMsg = nullptr;

   engineDownMsg = new EventHwEngStatusConfig;
   if (nullptr == engineDownMsg)
       return;

   sendEngineDownEventHelper(engineDownMsg, sizeof(EventHwEngStatusConfig));
}

void LocIdlClientV02Api::sendFidlCapabilitiesEvent(EventMsgCapabilityRptPkt *msg)
{
    EventHwCapability *capRpt;

    capRpt = new EventHwCapability;
    if (nullptr == capRpt)
        return;

    capRpt->capability = msg->capability;
    sendEngineCapabilitiesEventHelper(capRpt, sizeof(EventHwCapability));
}

void LocIdlClientV02Api::sendEngineCapabilitiesEventHelper(EventHwCapability *inMsg, uint32_t msgSize)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct sendCapabilityEvnt: public LocMsg
    {
        EventHwCapability       *pMsg;
        LocIdlClientV02Api      *pObj;
        uint32_t                size;
        sendCapabilityEvnt
        (
            EventHwCapability      *msg,
            uint32_t               inSize,
            LocIdlClientV02Api     *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            pObj(Obj),
            size(inSize)
        {}
        virtual void proc() const
        {
            fidlLocApiV02EventCb->locClientFidlCapabilities(pMsg->capability, fidlLocApiContext);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendCapabilityEvnt(inMsg, msgSize, this));
}


void LocIdlClientIpcListener::onReceive(const char* data, uint32_t length,
                                const LocIpcRecver* recver)
{
    if (nullptr == data || 0 == length)
        return;

    EventMsgBase *msg = (EventMsgBase *)data;
    switch (msg->eventID)
    {
        case IdlClientEvents::CLNT_EVT_DEVICE_UP:
        {
            LOC_LOGD("%s]CLNT_EVT_DEVICE_UP", __func__);
            mClientInstance.sendEngineUpEvent();
        }
        break;
        case IdlClientEvents::CLNT_EVT_DEVICE_DOWN:
        {
            LOC_LOGD("%s]CLNT_EVT_DEVICE_DOWN", __func__);
            mClientInstance.sendEngineDownEvent();
        }
        break;
        case IdlClientEvents::CLNT_EVT_RESPONSE:
        {
        }
        break;
        case IdlClientEvents::CLNT_EVT_POS_RPT:
        {
            mClientInstance.sendPosReport((EventMsgPosPkt *)data);
        }
        break;
        case IdlClientEvents::CLNT_EVT_SV_RPT:
        {
            mClientInstance.sendSvReport((EventMsgSvPkt *)data);
        }
        break;
        case IdlClientEvents::CLNT_EVT_SV_MEAS_RPT:
        {
        }
        break;
        case IdlClientEvents::CLNT_EVT_NMEA_RPT:
        {
            mClientInstance.sendNmeaReport((EventMsgNmeaPkt *)data);
        }
        break;
        case IdlClientEvents::CLNT_EVT_GNSS_MEAS_RPT:
        {
            mClientInstance.sendMeasReport((EventMsgGnssMeasPkt *)data);
        }
        break;
        case IdlClientEvents::CLNT_EVT_CONFIG_RPT:
        {
        }
        break;
        case IdlClientEvents::CLNT_EVT_CAPABILITY_RPT:
        {
            mClientInstance.sendFidlCapabilitiesEvent((EventMsgCapabilityRptPkt*)data);
        }
        break;
        default:
        break;
    }
}

void LocIdlClientV02Api::waitForSocketDir(const char *dirPath)
{
    struct stat buf;
    if (nullptr != dirPath) {
        while (1) {
            int rc = stat(dirPath, &buf);
            if (!rc) {
                break;
            }
            LOC_LOGD("Waiting for..: %s", dirPath);
            usleep(200000);
        }
    }
}

void LocIdlClientV02Api::initSocket()
{
    waitForSocketDir(LOC_IDL_SOCKET_DIR);
    mIpcSender = LocIpc::getLocIpcLocalSender(LOC_IDL_SOCKET_HAL);
    auto listener = make_shared<LocIdlClientIpcListener>(*this);
    mIpcrecver = LocIpc::getLocIpcLocalRecver(listener, LOC_IDL_SOCKET_Device);

    /* Create and listen on socket*/
    if (!mLocalIpc.startNonBlockingListening(mIpcrecver)) {
        LOC_LOGE("LocIdlClientV02Api: Failed to start receiver!");
        return;
    }
}

void initFidlCore(void) {
    LocIdlClientV02Api *pInstance = NULL;
    pInstance = LocIdlClientV02Api::getInstance();
    if (NULL == pInstance) {
        LOC_LOGE("failed to get LocIdlClientV02Api instance");
        return;
    }
    pInstance->initSocket();
}

void LocIdlClientV02Api::openRequest() {
    LOC_LOGD("%s]got CLNT_REQ_OPEN -->", __func__);
    ReqMsgOpenPkt *openeq = new ReqMsgOpenPkt();
    if (nullptr == openeq) {
        return;
     }
    openeq->reqOpen.requestedMask = 0x1F;
    sendReqMsgIpcHandler(openeq, sizeof(ReqMsgOpenPkt));
}

void LocIdlClientV02Api::startRequest(locClientFidlPosMode& fixCriteria) {
    LOC_LOGD("%s]got CLNT_REQ_START_FIX -->", __func__);
    ReqMsgStartFix *startfix = new ReqMsgStartFix();
    if (nullptr == startfix) {
        return;
    }
    sendReqMsgIpcHandler(startfix, sizeof(ReqMsgStartFix));
}

void LocIdlClientV02Api::stopRequest(void) {
    LOC_LOGD("%s]got CLNT_REQ_STOP_FIX -->", __func__);
    ReqMsgStopFix *stopfix = new ReqMsgStopFix();
    if (nullptr == stopfix) {
        return;
    }
    sendReqMsgIpcHandler(stopfix, sizeof(ReqMsgStopFix));
}

void LocIdlClientV02Api::delRequest(const GnssAidingData& data) {
    ReqMsgDelAiding *del = new ReqMsgDelAiding();
    del->reqDel.data.deleteAll = true;
    LOC_LOGD("%s]got CLNT_REQ_DEL_AIDING -->", __func__);
    sendReqMsgIpcHandler(del, sizeof(ReqMsgDelAiding));
}

void LocIdlClientV02Api::sendReqMsgIpcHandler(
                    ReqMsgBase *inMsg,
                    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct sendReqPacket: public LocMsg
    {
        const ReqMsgBase *pMsg;
        uint32_t          size;
        LocIdlClientV02Api   *pObj;
        sendReqPacket
        (
            const ReqMsgBase *msg,
            uint32_t                    inSize,
            LocIdlClientV02Api              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            size(inSize),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(pObj->mIpcSender), (uint8_t*)(pMsg), size);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendReqPacket(inMsg, msgSize, this));
}

void locClientFidlOpenReq(uint64_t requestedMask, bool isMaster,
                uint64_t *supportedMask, locClientFidlFeatureList * fidlFeatures,
                void *context)
{
    LOC_LOGD("%s] --> ", __func__);
    //Set Engine Capabilities
    if (NULL != supportedMask) {
        *supportedMask = LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT |
                           LOC_API_ADAPTER_BIT_SATELLITE_REPORT |
                           LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT |
                           LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT_REPORT |
                           /* LOC_API_ADAPTER_BIT_ASSISTANCE_DATA_REQUEST |*/
                           LOC_API_ADAPTER_BIT_LOCATION_SERVER_REQUEST |
                           LOC_API_ADAPTER_BIT_EVENT_REPORT_INFO;
    }

    //Set Engine Features
    if (NULL != fidlFeatures) {
        fidlFeatures->feature[0] = 0;
        /*  Below capabilites will decide HW Support */
        fidlFeatures->feature[0] |= (1 << LOC_SUPPORTED_FEATURE_AGPM_V02);
        fidlFeatures->feature[0] |= (1 << LOC_SUPPORTED_FEATURE_DEBUG_NMEA_V02);
        fidlFeatures->feature_len  = 1;
    }

    LOC_LOGV("[%s]<--- ", __func__);
}

void locClientFidlCloseReq(void *context)
{
    LOC_LOGD("%s] --> ", __func__);
}

void locClientFidlStartFixReq(locClientFidlPosMode& fixCriteria, void *context)
{
   LOC_LOGD("%s] --> ", __func__);
   LocIdlClientV02Api::getInstance()->startRequest(fixCriteria);
}

void locClientFidlStopFixReq(void *context)
{
   LOC_LOGD("%s] --> ", __func__);
   LocIdlClientV02Api::getInstance()->stopRequest();
}

void locClientFidlDeleteAidingDataReq(const GnssAidingData& data, void *context)
{
   LOC_LOGD("%s] --> ", __func__);
   LocIdlClientV02Api::getInstance()->delRequest(data);
}

void locClientFidlSetPositionModeReq(locClientFidlPosMode& posMode, void *context)
{
   LOC_LOGD("%s] --> ", __func__);
}
void locClientFidlSetGpsLockSyncReq(GnssConfigGpsLock lock, void *context)
{
   LOC_LOGD("%s] --> ", __func__);

}
void locClientFidlStartTimeBasedTrackingReq(const TrackingOptions& options,
        void *context)
{
   LOC_LOGD("%s] --> ", __func__);
}

void locClientFidlStopTimeBasedTrackingReq(void *context)
{
   LOC_LOGD("%s] --> ", __func__);
}

const locClientFidlInterfaceReq* get_locClientFidl_if_api(locClientFidlInterfaceEvent* eventCallback,
                                     void *context)
{
    const locClientFidlInterfaceReq* fidlRqIf = nullptr;
    fidlLocApiContext = context;
    LOC_LOGD("%s] --> ", __func__);
    fidlRqIf = getInterfaceReq();
    setInterfaceEvent(eventCallback);
    initFidlCore();
    return fidlRqIf;
}
