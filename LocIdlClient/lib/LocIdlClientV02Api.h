/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#ifndef LOC_CLIENT_FIDL_V02API_H
#define LOC_CLIENT_FIDL_V02API_H

/*=============================================================================
 *============================================================================*/
#include "locClientFidlIf.h"
#include "LocIdlClientIfV02.h"
#include <LocIpc.h>
#include <MsgTask.h>

using namespace std;
using namespace loc_util;

void locClientFidlOpenReq(uint64_t requestedMask, bool isMaster,
                uint64_t *supportedMask, locClientFidlFeatureList * fidlFeatures,
                void *context);
void locClientFidlCloseReq(void *context);
void locClientFidlStartFixReq(locClientFidlPosMode& fixCriteria, void *context);
void locClientFidlStopFixReq(void *context);
void locClientFidlDeleteAidingDataReq(const GnssAidingData& data, void *context);
void locClientFidlSetPositionModeReq(locClientFidlPosMode& posMode, void *context);
void locClientFidlSetGpsLockSyncReq(GnssConfigGpsLock lock, void *context);
void locClientFidlStartTimeBasedTrackingReq(const TrackingOptions& options,
        void *context);
void locClientFidlStopTimeBasedTrackingReq(void *context);

class LocIdlClientIpcListener;

class LocIdlClientV02Api {
    friend     LocIdlClientIpcListener;
public:
    LocIdlClientV02Api();
    ~LocIdlClientV02Api();
    static LocIdlClientV02Api* getInstance();
    unique_ptr<LocIpcRecver>            mIpcrecver;

private:
   static LocIdlClientV02Api* mInstance;
    MsgTask*                            mMsgTask;
    shared_ptr<LocIpcSender>            mIpcSender;
    LocIpc                              mLocalIpc;
    bool                                mIsListenerReady;

public:
    void initSocket();
    //void initGptp();
    void waitForSocketDir(const char *dirPath);
    void sendReqMsgIpcHandler(ReqMsgBase *inMsg, uint32_t msgSize);
    void sendReqMsg(IdlClinetRequests req);
    void startRequest(locClientFidlPosMode& fixCriteria);
    void stopRequest(void);
    void delRequest(const GnssAidingData& data) ;
    void sendPosReport(EventMsgPosPkt *msg);
    void sendPosRptHelper(EventPosRpt *inMsg, uint32_t msgSize);
    void sendNmeaReport(EventMsgNmeaPkt *msg);
    void sendNmeaRptHelper(EventNmeaRpt *inMsg, uint32_t msgSize);
    void sendMeasReport(EventMsgGnssMeasPkt *msg);
    void sendMeasRptHelper(EventGnssMeasRpt *inMsg, uint32_t msgSize);
    void sendSvReport(EventMsgSvPkt *msg);
    void sendSvRptHelper(EventSvRpt *inMsg, uint32_t msgSize);
};

#endif /* LOC_CLIENT_FIDL_V02API_H  */
