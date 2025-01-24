/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#ifndef LOC_IDL_CLIENT_DEVICE_H
#define LOC_IDL_CLIENT_DEVICE_H

#include <unistd.h>
#include <iostream>
#include <string>
#include <chrono>
#include <future>
#include <iomanip>
#include <sstream>
#include <string>
#include <CommonAPI/CommonAPI.hpp>
#include <v1/com/qualcomm/qti/location/LocationProxy.hpp>
#include <time.h>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <gptp_helper.h>
#include <LocIpc.h>
#include <MsgTask.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "loc_gps.h"
#include "log_util.h"

#include "LocIdlClientIfV02.h"
#include "LocIdlclientDiagLogTypes.h"

using namespace std;
using namespace loc_util;
using namespace v1::com::qualcomm::qti::location;
using namespace loc_idl_diag;

extern const gPTPLibInterfaceReq  *gPTPReqIf;

/* Request */
enum class ClientDeviceStates
{
    DEVICE_STATE_UNDEFINED = 0,
    DEVICE_STATE_INIT,
    DEVICE_STATE_DOWN,
    DEVICE_STATE_READY,
    DEVICE_STATE_IN_SESSION
};

class IpcListener;

class LocIdlClientDevice {
    friend     IpcListener;
public:
    LocIdlclientDiag mDiagObj;
    shared_ptr<LocationProxy<>> myProxy;
    shared_ptr < CommonAPI::Runtime > runtime;
    ClientDeviceStates states;
    uint32_t pvtSubscription;
    uint32_t svSubscription;
    uint32_t nmeaSubscription;
    uint32_t measSubscription;
    uint32_t dataSubscription;
    uint32_t capsSubscription;
    uint32_t mask;
    CommonAPI::CallInfo info;
    GnssDataNotification mDataNotify;

    MsgTask*                    mMsgTask;
    shared_ptr<LocIpcSender>    mIpcSender;
    LocIpc                         mLocalIpc;
    unique_ptr<LocIpcRecver>    mIpcrecver;
    bool                        mIsListenerReady;
    uint64_t                    mSessionStartBootTimestampNs;
    uint32_t                    recvdCapsMask;

    LocIdlClientDevice();
    ~LocIdlClientDevice();
    static LocIdlClientDevice* getInstance();

    void initSomeIp();
    void initSocket();
    void initGptp();

    void subscribeGnssResports();
    void UnSubscribeGnssResports();
    void subscribeServiceMsgs();

    void getLocationCapabilities();
    void sessionStart();
    void sessionStop();
    void handleOpen(ReqOpen &open);
    void handleClose();
    void deleteRequest(GnssAidingData &data);
    void sendSessionStartRespEvent(bool status);
    void sendRespEvent(IdlClinetRequests reqType, bool status );
    void sendDeviceStateEvent(ClientDeviceStates state);

    void sendPosRespEvent(const LocationTypes::LocationReportT &_locationReport);
    void getLocationRpt(const LocationTypes::LocationReportT &_locationReport,
                        UlpLocation &ulpLoc, locIdlClientDiagPosition &gnssPosDiag);
    void getLocationExtendedRpt(const LocationTypes::LocationReportT &_locationReport,
                        GpsLocationExtended &gpsLocExt,
                        locIdlClientDiagPosition &gnssPosDiag);
    void fillPosTechMask(unsigned int techmask, unsigned int &outMask);
    void sendGnssMeasRespEvent(const LocationTypes::GnssMeasurementsT& gnssMeasurements);
    void getMeasurementSet(const LocationTypes::GnssMeasurementsT& gnssMeasurement,
                         GnssMeasurements &svMeasurementSet,
                         locIdlClientDiagGnssMeasPacket &gnssMeasDiag);
    void sendSvRespEvent(const vector<LocationTypes::GnssSvDataT> &gnssSv);
    void getSvRpt(const vector<LocationTypes::GnssSvDataT> &gnssSvf,
                        GnssSvNotification &svNotify,
                        locIdlClientDiagGnssSv& gnssSVDiag);
    void sendNmeaRespEvent(const uint64_t timestamp, const string nmea);
    void sendGnssDataRespEvent(const LocationTypes::GnssDataT& gnssData);
    void sendEventsIpcHelper(EventMsgBase *inMsg, uint32_t msgSize);
    void sendReqMsgIpcHandler(ReqMsgBase *inMsg, uint32_t msgSize);
    void sendReqMsg(ReqMsgBase     *m_msg, uint32_t length);
    void waitForSocketDir(const char *dirPath);

    bool getClockBootTimeNs(uint64_t &clk_bootTime);
    void getGptpTimeNs(uint64_t &gptp_time_ns);
    void sendGetCapabilityMsg();
    void sendDeviceCapabilityEvent();
    bool getGptpSyncStatus();
    uint32_t parseConstellationType(
            LocationTypes::GnssConstellationTypeT idlConstellation);
    uint32_t parseSvType(LocationTypes::GnssSvTypeT svType);
    uint32_t parseMeasStateMask(uint32_t stateMask);
    uint32_t parseAdrStateMask(uint32_t adrStatemask);
    uint32_t parseMultiPathIndicator(uint32_t multipathIndicator);
    uint32_t parseDataMask(uint32_t mask);
    uint32_t parseGnssSvOptionsMask(uint32_t idlOptionMask);
    uint32_t parsegnssSystemTime(uint8_t idlTimeSrc);
    uint32_t parseLocReliability(uint32_t idlReliability);
    uint32_t parseSignalType (uint32_t idlSignalType);
    uint32_t parseDrSolutionStatusMask(uint32_t idlDrSolMask);
    uint32_t parseEngMask(uint32_t idlEngMask);
    GnssMeasurementsCodeType parseMeasCodeType(uint32_t idlMeasCodeType);
private:
   static LocIdlClientDevice* mInstance;
};

#endif /* LOC_IDL_CLIENT_DEVICE_H */
