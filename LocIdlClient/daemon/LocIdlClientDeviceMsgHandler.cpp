/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#include "LocIdlClientDevice.h"

void LocIdlClientDevice::sendReqMsgIpcHandler(
                    ReqMsgBase *inMsg,
                    uint32_t msgSize
)
{
    if (nullptr == inMsg || 0 == msgSize)
        return;

    LOC_LOGV("%s] --> ", __func__);

    struct sendReqPacket: public LocMsg
    {
        const ReqMsgBase*          pMsg;
        uint32_t                   size;
        LocIdlClientDevice*        pObj;
        sendReqPacket
        (
            const ReqMsgBase*        msg,
            uint32_t                inSize,
            LocIdlClientDevice*     Obj
        ) :
            LocMsg(),
            pMsg(msg),
            size(inSize),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            switch (pMsg->reqID)
            {
                case IdlClinetRequests::CLNT_REQ_OPEN:
                {
                    ReqMsgOpenPkt *openMsg = (ReqMsgOpenPkt *)pMsg;
                    ReqOpen open = openMsg->reqOpen;
                    pObj->handleOpen(open);
                }
                break;
                case IdlClinetRequests::CLNT_REQ_CLOSE:
                {
                    pObj->handleClose();
                }
                break;
                case IdlClinetRequests::CLNT_REQ_START_FIX:
                {
                    pObj->sessionStart();
                }
                break;
                case IdlClinetRequests::CLNT_REQ_STOP_FIX:
                {
                    pObj->sessionStop();
                }
                break;
                case IdlClinetRequests::CLNT_REQ_DEL_AIDING:
                {
                    ReqMsgDelAiding *del = (ReqMsgDelAiding *)pMsg;
                    GnssAidingData data = del->reqDel.data;
                    pObj->deleteRequest(data);
                }
                break;
                case IdlClinetRequests::CLNT_REQ_SET_POS_MODE:
                {
                }
                break;
                case IdlClinetRequests::CLNT_REQ_SET_GPS_LOCK:
                {
                }
                break;
                case IdlClinetRequests::CLNT_REQ_START_TIME_BASED_TRK:
                {
                }
                break;
                case IdlClinetRequests::CLNT_REQ_STOP_TIME_BASED_TRK:
                {
                }
                break;
                default:
                break;
            }
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendReqPacket(inMsg, msgSize, this));
}

void LocIdlClientDevice::sendReqMsg(ReqMsgBase  *m_msg, uint32_t length)
{
    LOC_LOGV("%s] ID: %d --> ", __func__, m_msg->reqID);
    switch (m_msg->reqID)
    {
        case IdlClinetRequests::CLNT_REQ_OPEN:
        {
            ReqMsgOpenPkt *open = new ReqMsgOpenPkt();
            open->reqID = m_msg->reqID;
            open->uniqueId = m_msg->uniqueId;
            open->size = m_msg->size;
            open->reqOpen.requestedMask = ((ReqMsgOpenPkt *)m_msg)->reqOpen.requestedMask;
            sendReqMsgIpcHandler(open, length);
        }
        break;
        case IdlClinetRequests::CLNT_REQ_CLOSE:
        {
            ReqMsgClosePkt *close = new ReqMsgClosePkt();
            close->reqID = m_msg->reqID;
            close->uniqueId = m_msg->uniqueId;
            close->size = m_msg->size;
            sendReqMsgIpcHandler(close, length);
        }
        break;
        case IdlClinetRequests::CLNT_REQ_START_FIX:
        {
            ReqMsgStartFix *startfix = new ReqMsgStartFix();
            startfix->reqID = m_msg->reqID;
            startfix->uniqueId = m_msg->uniqueId;
            startfix->size = m_msg->size;
            startfix->reqStart.fixCriteria = ((ReqMsgStartFix *)m_msg)->reqStart.fixCriteria;
            sendReqMsgIpcHandler(startfix, length);
        }
        break;
        case IdlClinetRequests::CLNT_REQ_STOP_FIX:
        {
            ReqMsgStopFix *stopfix = new ReqMsgStopFix();
            stopfix->reqID = m_msg->reqID;
            stopfix->uniqueId = m_msg->uniqueId;
            stopfix->size = m_msg->size;
            stopfix->reqStop.placeHolder = ((ReqMsgStopFix *)m_msg)->reqStop.placeHolder;
            sendReqMsgIpcHandler(stopfix, length);
        }
        break;
        case IdlClinetRequests::CLNT_REQ_DEL_AIDING:
        {
            ReqMsgDelAiding *del = new ReqMsgDelAiding();
            del->reqID = m_msg->reqID;
            del->uniqueId = m_msg->uniqueId;
            del->size = m_msg->size;
            del->reqDel.data = ((ReqMsgDelAiding *)m_msg)->reqDel.data;
            sendReqMsgIpcHandler(del, length);
        }
        break;
        case IdlClinetRequests::CLNT_REQ_SET_POS_MODE:
        {
        }
        break;
        case IdlClinetRequests::CLNT_REQ_SET_GPS_LOCK:
        {
        }
        break;
        case IdlClinetRequests::CLNT_REQ_START_TIME_BASED_TRK:
        {
        }
        break;
        case IdlClinetRequests::CLNT_REQ_STOP_TIME_BASED_TRK:
        {
        }
        break;
        default:
        break;
    }

}

void LocIdlClientDevice::sendEventsIpcHelper(
                    EventMsgBase*    inMsg,
                    uint32_t         msgSize
                )
{
    LOC_LOGV("%s] --> ", __func__);
    if (nullptr == inMsg || 0 == msgSize)
        return;

    struct sendEventPacket: public LocMsg
    {
        const EventMsgBase *pMsg;
        LocIdlClientDevice           *pObj;
        uint32_t                     size;
        sendEventPacket
        (
            const EventMsgBase *msg,
            uint32_t                    inSize,
            LocIdlClientDevice              *Obj
        ) :
            LocMsg(),
            pMsg(msg),
            pObj(Obj),
            size(inSize)
        {}
        virtual void proc() const
        {
            LocIpc::send(*(pObj->mIpcSender), (uint8_t*)(pMsg), size);
            delete pMsg;
        }
    };
    mMsgTask->sendMsg(new sendEventPacket(inMsg, msgSize, this));
}

void LocIdlClientDevice::sendRespEvent(IdlClinetRequests reqType, bool status )
{
    LOC_LOGV("%s] --> ", __func__);

    EventMsgResponsePkt *msg;
    msg = new EventMsgResponsePkt();
    if (nullptr == msg)
        return;

    msg->rspEvt.reqID = reqType;
    if (status)
        msg->rspEvt.respStat = ResponseStatus::CLNT_RESONSE_SUCCESS;
    else
        msg->rspEvt.respStat = ResponseStatus::CLNT_RESONSE_FAILURE;

    sendEventsIpcHelper(msg, sizeof(EventMsgResponsePkt));
}

void LocIdlClientDevice::sendDeviceStateEvent(ClientDeviceStates state)
{
    LOC_LOGV("%s] --> ", __func__);
    if (ClientDeviceStates::DEVICE_STATE_DOWN == state)
    {
        EventMsgEngineDownPkt *msg;
        msg = new EventMsgEngineDownPkt();
        if (nullptr == msg)
            return;
        sendEventsIpcHelper(msg, sizeof(EventMsgEngineDownPkt));
    }
    else if (ClientDeviceStates::DEVICE_STATE_READY == state)
    {
        EventMsgEngineUpPkt *msg;
        msg = new EventMsgEngineUpPkt();
        if (nullptr == msg)
            return;
        sendEventsIpcHelper(msg, sizeof(EventMsgEngineUpPkt));
    }
}

void LocIdlClientDevice::sendPosRespEvent(const LocIdlAPI::IDLLocationReport &_locationReport)
{
    LOC_LOGV("%s] --> ", __func__);
    EventMsgPosPkt *msg;
    uint64_t clk_bootTime = 0;
    uint64_t gptp_time_ns = 0;

    msg = new EventMsgPosPkt();
    if (nullptr == msg)
        return;

    const LocIdlAPI::IDLLocation &location = _locationReport.getLocInfo();
    UlpLocation &ulpLoc = msg->posRpt.location;
    GpsLocationExtended &gpsLocExt = msg->posRpt.locationExtended;;
    GnssDataNotification &dataNotify = msg->posRpt.dataNotify;
    loc_sess_status status;
    LocPosTechMask loc_technology_mask;
    uint32_t st = _locationReport.getSessionStatus();
    locIdlClientDiagPosition    gnssPosDiag;
    float latency = 0;

    dataNotify = mDataNotify;

    if (st == LocIdlAPI::IDLLocSessionStatus::IDL_LOC_SESS_SUCCESS) {
        status = LOC_SESS_SUCCESS;
    } else if (st == LocIdlAPI::IDLLocSessionStatus::IDL_LOC_SESS_INTERMEDIATE) {
        status = LOC_SESS_INTERMEDIATE;
    } else {
        status = LOC_SESS_FAILURE;
    }

    memset(&gnssPosDiag, 0x00, sizeof(locIdlClientDiagPosition));
    getLocationRpt(_locationReport, ulpLoc, gnssPosDiag);
    getLocationExtendedRpt(_locationReport, gpsLocExt, gnssPosDiag);

    msg->posRpt.loc_technology_mask = ulpLoc.tech_mask;
    msg->posRpt.msInWeek = gpsLocExt.gpsTime.gpsWeek;
    msg->posRpt.status = status;

    getClockBootTimeNs(clk_bootTime);
    getGptpTimeNs(gptp_time_ns);

    msg->posRpt.gptp_time_ns = gptp_time_ns;
    msg->posRpt.elapsedgPTPTime = _locationReport.getElapsedgPTPTime();

    if ((msg->posRpt.gptp_time_ns == 0) || (msg->posRpt.elapsedgPTPTime == 0)) {
        latency = 0;
    }
    else if(msg->posRpt.gptp_time_ns > msg->posRpt.elapsedgPTPTime) {
        latency = ((float)(msg->posRpt.gptp_time_ns - msg->posRpt.elapsedgPTPTime)/
                                                        (float)1000000);
    }

    LOC_LOGD("%s] Rx= %llu Tx= %llu Latency= %0.4f--> ", __func__,
                        msg->posRpt.gptp_time_ns, msg->posRpt.elapsedgPTPTime, latency);

    mDiagObj.diagLogPosInfo(gnssPosDiag, clk_bootTime, gptp_time_ns, false, latency);
    sendEventsIpcHelper(msg, sizeof(EventMsgPosPkt));
}

void LocIdlClientDevice::sendGnssMeasRespEvent(const LocIdlAPI::IDLGnssMeasurements& gnssMeasurements)
{
    LOC_LOGV("%s] --> ", __func__);
    EventMsgGnssMeasPkt *msg;
    locIdlClientDiagGnssMeasPacket gnssMeasDiag;
    uint64_t clk_bootTime = 0;
    uint64_t gptp_time_ns = 0;

    msg = new EventMsgGnssMeasPkt();
    if (nullptr == msg)
        return;

    GnssMeasurements &measSet =  msg->measRpt.measurements;
    memset(&gnssMeasDiag, 0x00, sizeof(locIdlClientDiagGnssMeasPacket));
    getMeasurementSet(gnssMeasurements, measSet, gnssMeasDiag);

    getClockBootTimeNs(clk_bootTime);
    getGptpTimeNs(gptp_time_ns);

    mDiagObj.diagLogMeasInfo(gnssMeasDiag, clk_bootTime, gptp_time_ns, false);
    sendEventsIpcHelper(msg, sizeof(EventMsgGnssMeasPkt));
}

void LocIdlClientDevice::sendSvRespEvent(const vector<LocIdlAPI::IDLGnssSv> &gnssSv)
{
    LOC_LOGV("%s] --> ", __func__);
    EventMsgSvPkt *msg;
    uint64_t clk_bootTime = 0;
    uint64_t gptp_time_ns = 0;

    msg = new EventMsgSvPkt();
    if (nullptr == msg)
        return;

    GnssSvNotification &svNotify = msg->svRpt.svNotify;
    locIdlClientDiagGnssSv  gnssSVDiag;

    memset(&gnssSVDiag, 0x00, sizeof(locIdlClientDiagGnssSv));
    getSvRpt(gnssSv, svNotify, gnssSVDiag);
    getClockBootTimeNs(clk_bootTime);
    getGptpTimeNs(gptp_time_ns);
    mDiagObj.diagLogSvInfo(gnssSVDiag, clk_bootTime, gptp_time_ns, false);
    sendEventsIpcHelper(msg, sizeof(EventMsgSvPkt));
}

void LocIdlClientDevice::sendNmeaRespEvent(const uint64_t timestamp, const string nmea)
{
    LOC_LOGV("%s] --> ", __func__);
    EventMsgNmeaPkt *msg;
    uint64_t clk_bootTime = 0;
    uint64_t gptp_time_ns = 0;

    stringstream ss(nmea);
    string nmeaStr;
    while (std::getline(ss, nmeaStr, '\n')) {
        msg = nullptr;
        msg = new EventMsgNmeaPkt();
        if (nullptr == msg)
            return;
        nmeaStr += '\n';
        strlcpy(msg->nmeaRpt.nmea, nmeaStr.c_str(), MAX_NMEA_STRING_SIZE);
        msg->nmeaRpt.length = nmeaStr.length();
        msg->nmeaRpt.timestamp = timestamp;
        sendEventsIpcHelper(msg, sizeof(EventMsgNmeaPkt));
    }
    getClockBootTimeNs(clk_bootTime);
    getGptpTimeNs(gptp_time_ns);
    mDiagObj.diagLogNmeaInfo(timestamp, nmea, clk_bootTime, gptp_time_ns, false);
}

void LocIdlClientDevice::sendGnssDataRespEvent(const LocIdlAPI::IDLGnssData& gnssData)
{
    LOC_LOGV("%s] --> ", __func__);
    vector<uint32_t> dataMask = gnssData.getGnssDataMask();
    vector<double> jammerInd = gnssData.getJammerInd();
    vector<double> agc = gnssData.getAgc();
    GnssDataNotification &dataNotify = mDataNotify;
    locIdlClientDiagGnssData gnssDataDiag;
    uint64_t clk_bootTime = 0;
    uint64_t gptp_time_ns = 0;

    dataNotify.size = dataMask.size() - 1;
    gnssDataDiag.size = dataNotify.size;

    for  (int i = 0; i < (dataMask.size() - 1) && i < GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES; i++) {
        dataNotify.gnssDataMask[i] = (uint32_t)dataMask[i];
        gnssDataDiag.gnssDiagDataMask[i] = (uint32_t)dataMask[i];
        dataNotify.jammerInd[i] = (double)jammerInd[i];
        gnssDataDiag.jammerInd[i] = (double)jammerInd[i];
        dataNotify.agc[i] = (double)agc[i];
        gnssDataDiag.agc[i] = (double)agc[i];
    }

    getClockBootTimeNs(clk_bootTime);
    getGptpTimeNs(gptp_time_ns);
    mDiagObj.diagLogGnssDataInfo(gnssDataDiag, clk_bootTime, gptp_time_ns, false);
}

void LocIdlClientDevice::handleClose()
{
    LOC_LOGV("%s] --> ", __func__);
    mask = 0x00;
    sendRespEvent(IdlClinetRequests::CLNT_REQ_CLOSE, true);
}

void LocIdlClientDevice::handleOpen(ReqOpen &open)
{
    LOC_LOGV("%s] --> ", __func__);
    mask = open.requestedMask;
    if (states == ClientDeviceStates::DEVICE_STATE_IN_SESSION)
    {
        sessionStop();
        UnSubscribeGnssResports();
    }
    else if (states != ClientDeviceStates::DEVICE_STATE_READY)
    {
        sendRespEvent(IdlClinetRequests::CLNT_REQ_OPEN, false);
        return;
    }
    sendRespEvent(IdlClinetRequests::CLNT_REQ_OPEN, true);
}

void LocIdlClientDevice::sessionStart()
{
    LOC_LOGV("%s] --> ", __func__);
    uint32_t _intervalInMs = 100;
    LocIdlAPI::IDLLocationResponse resp;
    CommonAPI::CallStatus callStatus;
    uint64_t clk_bootTime = 0;
    uint64_t gptp_time_ns = 0;
    locIdlClientDiagStartReq startReq;

    info.sender_ = 1234;
    bool sessionStartstat = false;

    subscribeGnssResports();

    myProxy->startPositionSession(_intervalInMs, mask, callStatus, resp, &info);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        LOC_LOGE("startPositionSession() Remote call failed! callStatus : %d", (int)callStatus);
        sessionStartstat = false;
    } else {
        sessionStartstat = true;
        states = ClientDeviceStates::DEVICE_STATE_IN_SESSION;
    }

    getClockBootTimeNs(clk_bootTime);
    getGptpTimeNs(gptp_time_ns);

    startReq.interval = _intervalInMs;
    startReq.mask = mask;
    mDiagObj.diagLogGnssStartReq(startReq, clk_bootTime, gptp_time_ns, false);
    sendRespEvent(IdlClinetRequests::CLNT_REQ_START_FIX, sessionStartstat);
}

void LocIdlClientDevice::sessionStop()
{
    LOC_LOGV("%s] --> ", __func__);
    CommonAPI::CallStatus callStatus;
    bool sessionStopstat = false;
    uint64_t clk_bootTime = 0;
    uint64_t gptp_time_ns = 0;

    UnSubscribeGnssResports();

    myProxy->stopPositionSession(callStatus, &info);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        LOC_LOGE("stopPositionSession() Remote call failed! callStatus : %d", (int)callStatus);
        sessionStopstat = false;
    } else {
        sessionStopstat = true;
    }

    getClockBootTimeNs(clk_bootTime);
    getGptpTimeNs(gptp_time_ns);

    mDiagObj.diagLogGnssStopReq(clk_bootTime, gptp_time_ns, false);
    if (states == ClientDeviceStates::DEVICE_STATE_IN_SESSION) {
        states = ClientDeviceStates::DEVICE_STATE_READY;
        sendRespEvent(IdlClinetRequests::CLNT_REQ_STOP_FIX, sessionStopstat);
    }
}

void LocIdlClientDevice::deleteRequest(GnssAidingData &data) {
    LOC_LOGV("%s] --> ", __func__);
    CommonAPI::CallStatus callStatus;
    uint32_t aidingDataMask = 0;
    LocIdlAPI::IDLLocationResponse resp;
    bool delStat = false;
    uint64_t clk_bootTime = 0;
    uint64_t gptp_time_ns = 0;
    locIdlClientDiagDelReq delReq;

    if (data.deleteAll) {
        aidingDataMask |= LocIdlAPI::IDLAidingDataDeletionMask::IDL_AIDING_DATA_DELETION_ALL;
    }
    if (data.sv.svMask) {
        aidingDataMask |= LocIdlAPI::IDLAidingDataDeletionMask::IDL_AIDING_DATA_DELETION_EPHEMERIS;
    }
    if(data.dreAidingDataMask) {
        aidingDataMask |= LocIdlAPI::IDLAidingDataDeletionMask::IDL_AIDING_DATA_DELETION_DR_SENSOR_CALIBRATION;
    }

    if (myProxy) {
        myProxy->deleteAidingData(aidingDataMask, callStatus, resp, &info);
        if (callStatus != CommonAPI::CallStatus::SUCCESS) {
            LOC_LOGE("deleteAidingData() Remote call failed! callStatus : %d", (int)callStatus);
            delStat = false;
        } else {
            delStat = true;
        }

        getClockBootTimeNs(clk_bootTime);
        getGptpTimeNs(gptp_time_ns);

        delReq.delMask = aidingDataMask;
        mDiagObj.diagLogGnssDelReq(delReq, clk_bootTime, gptp_time_ns, false);
        sendRespEvent(IdlClinetRequests::CLNT_REQ_DEL_AIDING, delStat);
    }
}
