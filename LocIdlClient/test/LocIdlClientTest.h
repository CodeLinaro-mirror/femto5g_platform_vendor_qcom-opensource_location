/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#ifndef LOC_IDL_CLIENT_TEST_H
#define LOC_IDL_CLIENT_TEST_H

#include "LocIdlClientIfV02.h"
#include <LocIpc.h>
#include <MsgTask.h>

using namespace std;
using namespace loc_util;

class IpcListener;

class LocIdlClientTest {
    friend     IpcListener;
public:
    uint32_t mask;
    LocIdlClientTest();
    ~LocIdlClientTest();
    static LocIdlClientTest* getInstance();
    unique_ptr<LocIpcRecver>            mIpcrecver;

private:
   static LocIdlClientTest*             mInstance;
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
};

#endif /* LOC_IDL_CLIENT_TEST_H */
