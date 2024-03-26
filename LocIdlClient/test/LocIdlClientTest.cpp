/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#include "LocIdlClientTest.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "log_util.h"
#include "log_util.h"
#include "loc_cfg.h"


LocIdlClientTest *LocIdlClientTest::mInstance = nullptr;

using namespace std;

LocIdlClientTest::LocIdlClientTest():mMsgTask(new MsgTask("LocIdlClientTestMsgTask")), mask(0x01)
{
}

LocIdlClientTest::~LocIdlClientTest()
{
}

LocIdlClientTest* LocIdlClientTest::getInstance()
{
    if (nullptr == mInstance) {
        mInstance = new LocIdlClientTest();
    }
    return mInstance;
}

class IpcListener : public ILocIpcListener {
    LocIdlClientTest& mClientInstance;
public:
    inline IpcListener(LocIdlClientTest& sc) : mClientInstance(sc) {}
    virtual void onListenerReady() override;
    virtual void onReceive(const char* data,
                    uint32_t length, const LocIpcRecver* recver) override;
};

void IpcListener::onListenerReady()
{
    cout <<"IpcListener: onListenerReady -->";
    mClientInstance.mIsListenerReady = true;
    return;
}

void IpcListener::onReceive(const char* data, uint32_t length,
                                const LocIpcRecver* recver)
{
    if (nullptr == data || 0 == length)
        return;

    EventMsgBase *msg = (EventMsgBase *)data;
    switch (msg->eventID)
    {
        case IdlClientEvents::CLNT_EVT_DEVICE_UP:
        {
            cout << "CLNT_EVT_DEVICE_UP !!" << endl;
        }
        break;
        case IdlClientEvents::CLNT_EVT_DEVICE_DOWN:
        {
            cout << "CLNT_EVT_DEVICE_DOWN !!" << endl;
        }
        break;
        case IdlClientEvents::CLNT_EVT_RESPONSE:
        {
            EventMsgResponsePkt *msg = (EventMsgResponsePkt *)data;
            cout << "CLNT_EVT_RESPONSE ID" << (unsigned int)msg->rspEvt.reqID << "Status: "
                            "" << (unsigned int)msg->rspEvt.respStat << endl;
        }
        break;
        case IdlClientEvents::CLNT_EVT_POS_RPT:
        {
            EventMsgPosPkt *posData = (EventMsgPosPkt *)data;
            cout << fixed << setprecision(8) <<"PVT, "
                "" << posData->posRpt.location.gpsLocation.timestamp << ", "
                "" << posData->posRpt.location.gpsLocation.latitude << ", "
                "" << posData->posRpt.location.gpsLocation.longitude << ", "
                "" << posData->posRpt.gptp_time_ns << ", "
                "" << posData->posRpt.elapsedgPTPTime <<", "
                "" << fixed << setprecision(3) << ""
                "" <<(float)(posData->posRpt.gptp_time_ns -
                            posData->posRpt.elapsedgPTPTime) / (float)1000000 <<""
                ""  << endl;
        }
        break;
        case IdlClientEvents::CLNT_EVT_SV_RPT:
        {
            EventMsgSvPkt *svData = (EventMsgSvPkt *)data;
            cout << "SV, " << svData->svRpt.svNotify.count << endl;
        }
        break;
        case IdlClientEvents::CLNT_EVT_SV_MEAS_RPT:
        {
        }
        break;
        case IdlClientEvents::CLNT_EVT_NMEA_RPT:
        {
            EventMsgNmeaPkt *nmeaData = (EventMsgNmeaPkt *)data;
            cout << "NMEA, "<< nmeaData->nmeaRpt.timestamp <<", "<< nmeaData->nmeaRpt.nmea << endl;
        }
        break;
        case IdlClientEvents::CLNT_EVT_GNSS_MEAS_RPT:
        {
            EventMsgGnssMeasPkt *measData = (EventMsgGnssMeasPkt *)data;
            cout << "MEAS, "
            "" << measData->measRpt.measurements.gnssMeasNotification.clock.leapSecond<<", "
            "" << measData->measRpt.measurements.gnssMeasNotification.clock.timeNs << ""
            ", "<< measData->measRpt.measurements.gnssMeasNotification.clock.timeUncertaintyNs <<""
            ", " << measData->measRpt.measurements.gnssMeasNotification.count << endl;

        }
        break;
        case IdlClientEvents::CLNT_EVT_CONFIG_RPT:
        {
        }
        break;
        default:
        break;
    }
}

void LocIdlClientTest::waitForSocketDir(const char *dirPath)
{
    struct stat buf;
    if (nullptr != dirPath) {
        while (1) {
            int rc = stat(dirPath, &buf);
            if (!rc) {
                break;
            }
            cout<<"Waiting for.."<<dirPath<<endl;
            usleep(200000);
        }
    }
}

void LocIdlClientTest::sendReqMsgIpcHandler(
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
        LocIdlClientTest   *pObj;
        sendReqPacket
        (
            const ReqMsgBase *msg,
            uint32_t                    inSize,
            LocIdlClientTest              *Obj
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

void LocIdlClientTest::sendReqMsg(IdlClinetRequests req)
{
        switch (req)
        {
            case IdlClinetRequests::CLNT_REQ_OPEN:
            {
                ReqMsgOpenPkt *openReq = new ReqMsgOpenPkt();
                openReq->reqOpen.requestedMask = mask;
                cout<<"Sending CLNT_REQ_OPEN "<< mask <<endl;
                sendReqMsgIpcHandler(openReq, sizeof(ReqMsgOpenPkt));
            }
            break;
            case IdlClinetRequests::CLNT_REQ_CLOSE:
            {
                ReqMsgClosePkt *closeReq = new ReqMsgClosePkt();
                cout<<"Sending CLNT_REQ_CLOSE "<< mask <<endl;
                sendReqMsgIpcHandler(closeReq, sizeof(ReqMsgClosePkt));
            }
            break;
            case IdlClinetRequests::CLNT_REQ_START_FIX:
            {
                ReqMsgStartFix *startfix = new ReqMsgStartFix();
                cout<<"Sending CLNT_REQ_START_FIX"<<endl;
                sendReqMsgIpcHandler(startfix, sizeof(ReqMsgStartFix));
            }
            break;
            case IdlClinetRequests::CLNT_REQ_STOP_FIX:
            {
                ReqMsgStopFix *stopfix = new ReqMsgStopFix();
                cout<<"Sending CLNT_REQ_STOP_FIX"<<endl;
                sendReqMsgIpcHandler(stopfix, sizeof(ReqMsgStopFix));
            }
            break;
            case IdlClinetRequests::CLNT_REQ_DEL_AIDING:
            {
                ReqMsgDelAiding *del = new ReqMsgDelAiding();
                del->reqDel.data.deleteAll = true;
                cout<<"Sending CLNT_REQ_DEL_AIDING"<<endl;
                sendReqMsgIpcHandler(del, sizeof(ReqMsgDelAiding));
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

void LocIdlClientTest::initSocket()
{
    waitForSocketDir(LOC_IDL_SOCKET_DIR);
    mIpcSender = LocIpc::getLocIpcLocalSender(LOC_IDL_SOCKET_HAL);
    auto listener = make_shared<IpcListener>(*this);
    mIpcrecver = LocIpc::getLocIpcLocalRecver(listener, LOC_IDL_SOCKET_Device);

    /* Create and listen on socket*/
    if (!mLocalIpc.startNonBlockingListening(mIpcrecver)) {
        cout<< "Failed to start receiver!";
        return;
    }
}

void ToolUsage()
{
    cout << " Usage : " << endl;
    cout << " LocIdlClientTest -d <test duration in seconds> -m mask" << endl;
    return;
}

static bool parseCommandLine(int argc, char* argv[], int &delay)
{
    extern char *optarg;
    int opt;
    bool flag = false;

    /*60sec / 1 min */
    delay = 20;

    if (argc > 1) {
        while ((opt = getopt(argc, argv,
                  "m:d:h")) != -1) {
             switch (opt) {
                 case 'd':
                    delay = atoi(optarg);
                    flag = true;
                    break;
                 case 'm':
                    LocIdlClientTest::getInstance()->mask = atoi(optarg);
                    flag = true;
                    break;
                 case 'h':
                 default:
                     ToolUsage();
                     return false;
             }
        }
        if (!flag) {
             ToolUsage();
             return false;
        }
    }
    return true;
}

static void signalHandler(int signal)
{
    LocIdlClientTest *pInstance = NULL;
    pInstance = LocIdlClientTest::getInstance();
    cout << "signalHandler " <<endl;
    pInstance->sendReqMsg(IdlClinetRequests::CLNT_REQ_STOP_FIX);
    usleep(2);
    pInstance->sendReqMsg(IdlClinetRequests::CLNT_REQ_CLOSE);
    exit(0);
    return;
}

static void regSigHandler()
{
    struct sigaction mySigAction = {};

    mySigAction.sa_handler = signalHandler;
    sigemptyset(&mySigAction.sa_mask);
    sigaction(SIGHUP, &mySigAction, NULL);
    sigaction(SIGTERM, &mySigAction, NULL);
    sigaction(SIGINT, &mySigAction, NULL);
    sigaction(SIGPIPE, &mySigAction, NULL);
}

int main(int argc, char *argv[])
{

    // Get the GNSS DEPLOYMENT setting which configured in gps.conf
    uint32_t gnssDeployment = 0;
    const loc_param_s_type gnss_deployment_conf_params[] = {
        {"GNSS_DEPLOYMENT", &gnssDeployment, NULL, 'n'},
    };
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, gnss_deployment_conf_params);

    if (4 != gnssDeployment) {
        cout <<"Update gps.conf configure GNSS_DEPLOYMENT as MDM_GNSS_FIDL_ENABLED "<<endl;;
        cout <<"Update gps.conf configure GNSS_DEPLOYMENT as MDM_GNSS_FIDL_ENABLED "<<endl;;
        exit(1);
    }
    LocIdlClientTest *pInstance = NULL;
    pInstance = LocIdlClientTest::getInstance();
    if (NULL == pInstance) {
        cout <<"failed to get LocIdlClientTest instance"<<endl;;
        exit(1);
    }

    int delay;
    /* Command Line parsing*/
    if (!parseCommandLine(argc, argv, delay)) {
        return 0;
    }
    /* signal Handler */
    regSigHandler();
    pInstance->initSocket();
    pInstance->sendReqMsg(IdlClinetRequests::CLNT_REQ_OPEN);
    usleep(2);
    pInstance->sendReqMsg(IdlClinetRequests::CLNT_REQ_START_FIX);
    sleep(delay);
    pInstance->sendReqMsg(IdlClinetRequests::CLNT_REQ_STOP_FIX);
    sleep(1);
    pInstance->sendReqMsg(IdlClinetRequests::CLNT_REQ_CLOSE);
    return 0;
}

