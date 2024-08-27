/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#include "LocIdlClientDevice.h"

LocIdlClientDevice *LocIdlClientDevice::mInstance = nullptr;

#define GPTP_IF_LIB_NAME      "libgptp.so"

void gptpUpdateNotification(struct gptp_update update);
const char * libName = GPTP_IF_LIB_NAME;
void *gPTPLibHandle = nullptr;

const static gPTPLibInterfaceEvent gPTPEvent = {
   gptpUpdateNotification,
};
const gPTPLibInterfaceReq  *gPTPReqIf = nullptr;

void gptpUpdateNotification(struct gptp_update update)
{
    LOC_LOGV("GPTP Update Notification");
}

void loadGptpLibFile(void)
{
    char *errorDll;

    if ((gPTPLibHandle = dlopen(libName, RTLD_NOW)) != nullptr) {
        LOC_LOGD("%s is present", libName);
        get_gPTPLib_if_t getter = (get_gPTPLib_if_t)dlsym(gPTPLibHandle, "get_gPTPLib_if");

        if ((errorDll = dlerror()) != nullptr) {
            LOC_LOGE("%s get_gPTPLib_if failed, error = %s", libName, errorDll);
            getter = nullptr;
        }

        if (getter != nullptr) {
            gPTPReqIf = (getter)(&gPTPEvent);
            if (gPTPReqIf != nullptr) {
                return;
            } else {
                LOC_LOGE("%s lib provided Command Interface as NULL", libName);
            }
        }
    } else {
        errorDll = dlerror();
        LOC_LOGE("%s failed to load, handle %p error: %s", libName, gPTPLibHandle,
             ((nullptr != errorDll) ? errorDll : "No Error"));
    }
}

bool LocIdlClientDevice::getClockBootTimeNs(uint64_t &clk_bootTime)
{
    LOC_LOGV("%s] --> ", __func__);
    struct timespec ts = {};
    int res = -1;
    res = clock_gettime(CLOCK_BOOTTIME, &ts);
    clk_bootTime = (ts.tv_sec * 1000000000ULL + ts.tv_nsec);
    return (res == 0);
}

void LocIdlClientDevice::getGptpTimeNs(uint64_t &gptp_time_ns)
{
    uint64_t ptpTime = 0;
    LOC_LOGV("%s] --> ", __func__);
    if ((nullptr != gPTPReqIf) && (nullptr != gPTPReqIf->gptpGetCurPtpTimeIf)) {
        gPTPReqIf->gptpGetCurPtpTimeIf(&ptpTime);
     }

    gptp_time_ns = ptpTime;
}

class IpcListener : public ILocIpcListener {
    LocIdlClientDevice& mClientInstance;
public:
    inline IpcListener(LocIdlClientDevice& sc) : mClientInstance(sc) {}
    virtual void onListenerReady() override;
    virtual void onReceive(const char* data,
                    uint32_t length, const LocIpcRecver* recver) override;
};

void IpcListener::onListenerReady()
{
    LOC_LOGD("%s] LocIdlClientDevice --> ", __func__);
    mClientInstance.mIsListenerReady = true;
    return;
}


void IpcListener::onReceive(const char* data, uint32_t length,
                                const LocIpcRecver* recver)
{
    if (nullptr == data || 0 == length)
        return;
    ReqMsgBase           *m_msg = (ReqMsgBase *)data;
    mClientInstance.sendReqMsg(m_msg, length);
}

void LocIdlClientDevice::waitForSocketDir(const char *dirPath)
{
    LOC_LOGD("%s] --> ", __func__);
    struct stat buf;
    if (nullptr != dirPath) {
        while (1) {
            int rc = stat(dirPath, &buf);
            if (!rc) {
                break;
            }
           LOC_LOGD("Waiting for.. :%s",dirPath);
            usleep(200000);
        }
    }
}

void LocIdlClientDevice::initSocket()
{
    LOC_LOGD("%s] --> ", __func__);
    DIR *pDir = opendir(LOC_IDL_SOCKET_DIR);
    if (pDir == nullptr) {
        LOC_LOGE("LocIdlClientDevice: Failed to create opendir!");
        return;
    }
    mIpcSender = LocIpc::getLocIpcLocalSender(LOC_IDL_SOCKET_Device);
    auto listener = make_shared<IpcListener>(*this);
    mIpcrecver = LocIpc::getLocIpcLocalRecver(listener, LOC_IDL_SOCKET_HAL);

    /* Create and listen on socket*/
    if (!mLocalIpc.startNonBlockingListening(mIpcrecver)) {
        LOC_LOGE("LocIdlClientDevice: Failed to start receiver!");
        return;
    }
    LOC_LOGW("initSocket done");
}

void LocIdlClientDevice::initGptp()
{
    /* GPTP */
    loadGptpLibFile();
    if ((nullptr != gPTPReqIf) && (nullptr != gPTPReqIf->gptpInitIf)) {
        bool ret;
        ret = gPTPReqIf->gptpInitIf();
        LOC_LOGW("gptpInitIf ret:%d", ret);
    }
}

void LocIdlClientDevice::sendGetCapabilityMsg()
{
    LOC_LOGD("%s] --> ", __func__);

    struct sendGetCapability: public LocMsg
    {
        LocIdlClientDevice           *pObj;
        sendGetCapability
        (
            LocIdlClientDevice              *Obj
        ) :
            LocMsg(),
            pObj(Obj)
        {}
        virtual void proc() const
        {
            pObj->getLocationCapabilities();
        }
    };
    mMsgTask->sendMsg(new sendGetCapability(this));
}

void LocIdlClientDevice::subscribeServiceMsgs()
{
    LOC_LOGD("%s] --> ", __func__);
    LocIdlClientDevice *pInstance = LocIdlClientDevice::getInstance();

        myProxy->getProxyStatusEvent().subscribe([&, this]
                (const CommonAPI::AvailabilityStatus status) {
        switch(status) {
        case CommonAPI::AvailabilityStatus::UNKNOWN:
            LOC_LOGD("%s] UNKNOWN", __func__);
            break;
        case CommonAPI::AvailabilityStatus::NOT_AVAILABLE:
            if (this->states == ClientDeviceStates::DEVICE_STATE_READY ||
                    this->states == ClientDeviceStates::DEVICE_STATE_IN_SESSION) {
                this->sendDeviceStateEvent(ClientDeviceStates::DEVICE_STATE_DOWN);
            }
            if (this->states == ClientDeviceStates::DEVICE_STATE_IN_SESSION)
                this->UnSubscribeGnssResports();
            this->states = ClientDeviceStates::DEVICE_STATE_DOWN;
            LOC_LOGD("%s] NOT_AVAILABLE", __func__);
            break;
        case CommonAPI::AvailabilityStatus::AVAILABLE:
            LOC_LOGD("%s] AVAILABLE", __func__);
            if (this->states == ClientDeviceStates::DEVICE_STATE_UNDEFINED)
                this->sendDeviceStateEvent(ClientDeviceStates::DEVICE_STATE_DOWN);
            this->sendGetCapabilityMsg();
            break;
        }
    });

    // Subscribe for receiving values
    capsSubscription = myProxy->getGnssCapabilitiesEvent().subscribe(
    [&, this](const uint32_t &val) {
        LOC_LOGD("%s] Received caps change event: %d", __func__, val);
        this->recvdCapsMask = val;
        if (0 == val) {
            if (this->states != ClientDeviceStates::DEVICE_STATE_DOWN) {
                this->states = ClientDeviceStates::DEVICE_STATE_DOWN;
                this->sendDeviceStateEvent(ClientDeviceStates::DEVICE_STATE_DOWN);
            }
        } else {
            if (this->states < ClientDeviceStates::DEVICE_STATE_READY) {
                this->states = ClientDeviceStates::DEVICE_STATE_READY;
                this->sendDeviceStateEvent(ClientDeviceStates::DEVICE_STATE_READY);
            }
        }
        sendDeviceCapabilityEvent();
    });
}

void LocIdlClientDevice::getLocationCapabilities() {
    if (myProxy) {
        CommonAPI::CallStatus callStatus = CommonAPI::CallStatus::UNKNOWN;
        uint32_t capsMask = 0;

        myProxy->GetLocationCapabilities(callStatus, capsMask, &info);
        if (callStatus != CommonAPI::CallStatus::SUCCESS) {
            LOC_LOGE("GetLocationCapabilities() Remote call failed! callStatus : %d",
                                            (int)callStatus);
        } else {
            LOC_LOGD("%s] GetLocationCapabilities caps: %d", __func__, capsMask);
            this->recvdCapsMask = capsMask;
            if (capsMask == 0) {
                if (this->states != ClientDeviceStates::DEVICE_STATE_DOWN) {
                    this->states = ClientDeviceStates::DEVICE_STATE_DOWN;
                    this->sendDeviceStateEvent(ClientDeviceStates::DEVICE_STATE_DOWN);
                }
            } else {
                if (this->states < ClientDeviceStates::DEVICE_STATE_READY) {
                    this->states = ClientDeviceStates::DEVICE_STATE_READY;
                    this->sendDeviceStateEvent(ClientDeviceStates::DEVICE_STATE_READY);
                }
            }
            sendDeviceCapabilityEvent();
        }
    } else {
        LOC_LOGE("%s] myProxy NULL", __func__);
    }
}

void LocIdlClientDevice::subscribeGnssResports()
{
    LOC_LOGV("%s] --> ", __func__);
    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_LOCATION_CB_INFO_BIT) {
        pvtSubscription = myProxy->getGnssLocationReportEvent().subscribe(
        [&, this](const LocationTypes::LocationReportT &locationReportInfo) {
            LOC_LOGD("%s] --> Posreport ", __func__);
            this->sendPosRespEvent(locationReportInfo);
        });
    }

    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_MEAS_CB_INFO_BIT) {
        measSubscription = myProxy->getGnssMeasurementReportEvent().subscribe(
        [&, this](const LocationTypes::GnssMeasurementsT& gnssMeasurements) {
            this->sendGnssMeasRespEvent(gnssMeasurements);
        });
    }

    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_SV_CB_INFO_BIT) {
        svSubscription = myProxy->getGnssSvReportEvent().subscribe(
        [&, this](const vector<LocationTypes::GnssSvDataT> &gnssSv) {
            this->sendSvRespEvent(gnssSv);
        });
    }

    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_NMEA_CB_INFO_BIT) {
        nmeaSubscription = myProxy->getGnssNmeaEvent().subscribe(
        [&, this](const uint64_t timestamp, const string nmea) {
            this->sendNmeaRespEvent(timestamp, nmea);
        });
    }

    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_DATA_CB_INFO_BIT) {
        dataSubscription = myProxy->getGnssDataReportEvent().subscribe(
        [&, this](const LocationTypes::GnssDataT& gnssData) {
           this->sendGnssDataRespEvent(gnssData);
        });
    }
}

void LocIdlClientDevice::UnSubscribeGnssResports()
{
    LOC_LOGD("%s] --> ", __func__);
    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_DATA_CB_INFO_BIT) {
        myProxy->getGnssDataReportEvent().unsubscribe(dataSubscription);
    }
    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_LOCATION_CB_INFO_BIT) {
        myProxy->getGnssLocationReportEvent().unsubscribe(pvtSubscription);
    }
    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_MEAS_CB_INFO_BIT) {
        myProxy->getGnssMeasurementReportEvent().unsubscribe(measSubscription);
    }
    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_SV_CB_INFO_BIT) {
        myProxy->getGnssSvReportEvent().unsubscribe(svSubscription);
    }
    if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_NMEA_CB_INFO_BIT) {
        myProxy->getGnssNmeaEvent().unsubscribe(nmeaSubscription);
    }
}

void LocIdlClientDevice::initSomeIp()
{
    LOC_LOGD("%s] --> ", __func__);
    CommonAPI::Runtime::setProperty("LogContext", "Location");
    CommonAPI::Runtime::setProperty("LogApplication", "Location");
    CommonAPI::Runtime::setProperty("LibraryBase", "Location");

    runtime = CommonAPI::Runtime::get();

    string domain = "local";
    string instance = "com.qualcomm.qti.location.Location";
    string connection = "location-fidl-client";

    myProxy = runtime->buildProxy<LocationProxy>(domain,
            instance, connection);

    LOC_LOGE("%s] Checking Service availability!", __func__);
    while (!myProxy->isAvailable())
        usleep(10);
    LOC_LOGE("%s] Service available!", __func__);
}

LocIdlClientDevice::LocIdlClientDevice():
    myProxy(nullptr),
    runtime(nullptr),
    states(ClientDeviceStates::DEVICE_STATE_UNDEFINED),
    mask(0x1F),
    info(1000),
    mMsgTask(new MsgTask("LocIdlClientMsgTask"))
{

}

LocIdlClientDevice::~LocIdlClientDevice()
{

}

LocIdlClientDevice* LocIdlClientDevice::getInstance()
{
    if (nullptr == mInstance) {
        mInstance = new LocIdlClientDevice();
    }
    return mInstance;
}

static void signalHandler(int signal)
{
    LOC_LOGE("%s] --> ", __func__);
    LocIdlClientDevice *pInstance = NULL;
    pInstance = LocIdlClientDevice::getInstance();
    if (pInstance->states == ClientDeviceStates::DEVICE_STATE_IN_SESSION) {
        pInstance->states = ClientDeviceStates::DEVICE_STATE_DOWN;
        pInstance->sessionStop();
    }
    pInstance->UnSubscribeGnssResports();
    pInstance->sendDeviceStateEvent(ClientDeviceStates::DEVICE_STATE_DOWN);
    usleep(1);
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

int main()
{
    LOC_LOGD("%s] --> ", __func__);
    setenv("VSOMEIP_CONFIGURATION", "/vendor/etc/vsomeip-client.json", 1);
    setenv("COMMONAPI_CONFIG", "/vendor/etc/commonapi4someip.ini" ,1);
    LocIdlClientDevice *pInstance = NULL;
    pInstance = LocIdlClientDevice::getInstance();
    if (NULL == pInstance) {
        LOC_LOGE("failed to get LocIdlClientDevice instance");
        exit(1);
    }
    usleep(100);
        /* signal Handler */
    regSigHandler();
    pInstance->initSomeIp();
    pInstance->initGptp();
    pInstance->initSocket();
    pInstance->subscribeServiceMsgs();
    int fd[2], n = 0;
    char buffer[10];
    if (pipe(fd) != -1) {
        n = read(fd[0], buffer, 10);
        if (n > 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 0;
}
