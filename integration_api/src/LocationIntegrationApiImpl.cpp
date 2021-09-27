/* Copyright (c) 2019-2021 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
Changes from Qualcomm Innovation Center are provided under the following license:

Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted (subject to the limitations in the
disclaimer below) provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of Qualcomm Innovation Center, Inc. nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
Changes from Qualcomm Innovation Center are provided under the following license:

Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted (subject to the limitations in the
disclaimer below) provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of Qualcomm Innovation Center, Inc. nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define LOG_TAG "LocSvc_LocationIntegrationApiImpl"

#include <sys/types.h>
#include <unistd.h>
#include <loc_cfg.h>
#include <LocationIntegrationApiImpl.h>
#include <log_util.h>
#include <gps_extended_c.h>

static uint32_t sXtraTestEnabled = 0;
static const loc_param_s_type gConfigTable[] = {
    {"XTRA_TEST_ENABLED", &sXtraTestEnabled, NULL, 'n'}
};

namespace location_integration {

/******************************************************************************
Utilities
******************************************************************************/

static LocConfigTypeEnum getLocConfigTypeFromMsgId(ELocMsgID  msgId) {
    LocConfigTypeEnum configType = (LocConfigTypeEnum) 0;
    switch (msgId) {
    case E_INTAPI_CONFIG_CONSTRAINTED_TUNC_MSG_ID:
        configType = CONFIG_CONSTRAINED_TIME_UNCERTAINTY;
        break;
    case E_INTAPI_CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR_MSG_ID:
        configType = CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR;
        break;
    case E_INTAPI_CONFIG_SV_CONSTELLATION_MSG_ID:
        configType = CONFIG_CONSTELLATIONS;
        break;
    case E_INTAPI_CONFIG_AIDING_DATA_DELETION_MSG_ID:
        configType = CONFIG_AIDING_DATA_DELETION;
        break;
    case E_INTAPI_CONFIG_LEVER_ARM_MSG_ID:
        configType = CONFIG_LEVER_ARM;
        break;
    case E_INTAPI_CONFIG_ROBUST_LOCATION_MSG_ID:
        configType = CONFIG_ROBUST_LOCATION;
        break;
    case E_INTAPI_CONFIG_MIN_GPS_WEEK_MSG_ID:
        configType = CONFIG_MIN_GPS_WEEK;
        break;
    case E_INTAPI_CONFIG_DEAD_RECKONING_ENGINE_MSG_ID:
        configType = CONFIG_DEAD_RECKONING_ENGINE;
        break;
    case E_INTAPI_CONFIG_MIN_SV_ELEVATION_MSG_ID:
        configType = CONFIG_MIN_SV_ELEVATION;
        break;
    case E_INTAPI_CONFIG_OUTPUT_NMEA_TYPES_MSG_ID:
        configType = CONFIG_OUTPUT_NMEA_TYPES;
        break;
    case E_INTAPI_CONFIG_USER_CONSENT_TERRESTRIAL_POSITIONING_MSG_ID:
        configType = CONFIG_USER_CONSENT_TERRESTRIAL_POSITIONING;
        break;
    case E_INTAPI_CONFIG_ENGINE_INTEGRITY_RISK_MSG_ID:
        configType = CONFIG_ENGINE_INTEGRITY_RISK;
        break;
    case E_INTAPI_CONFIG_XTRA_PARAMS_MSG_ID:
        configType = CONFIG_XTRA_PARAMS;
        break;
    case E_INTAPI_GET_ROBUST_LOCATION_CONFIG_REQ_MSG_ID:
    case E_INTAPI_GET_ROBUST_LOCATION_CONFIG_RESP_MSG_ID:
        configType = GET_ROBUST_LOCATION_CONFIG;
        break;
    case E_INTAPI_GET_MIN_GPS_WEEK_REQ_MSG_ID:
    case E_INTAPI_GET_MIN_GPS_WEEK_RESP_MSG_ID:
        configType = GET_MIN_GPS_WEEK;
        break;
    case E_INTAPI_GET_MIN_SV_ELEVATION_REQ_MSG_ID:
    case E_INTAPI_GET_MIN_SV_ELEVATION_RESP_MSG_ID:
        configType = GET_MIN_SV_ELEVATION;
        break;
    case E_INTAPI_GET_XTRA_STATUS_REQ_MSG_ID:
    case E_INTAPI_GET_XTRA_STATUS_RESP_MSG_ID:
        configType = GET_XTRA_STATUS;
        break;
    case E_INTAPI_REGISTER_XTRA_STATUS_UPDATE_REQ_MSG_ID:
    case E_INTAPI_DEREGISTER_XTRA_STATUS_UPDATE_REQ_MSG_ID:
        configType = REGISTER_XTRA_STATUS_UPDATE;
        break;
    default:
        break;
    }
    return configType;
}

static LocIntegrationResponse getLocIntegrationResponse(::LocationError error) {
    LocIntegrationResponse response;

    switch (error) {
        case LOCATION_ERROR_SUCCESS:
            response = LOC_INT_RESPONSE_SUCCESS;
            break;
        case LOCATION_ERROR_NOT_SUPPORTED:
            response = LOC_INT_RESPONSE_NOT_SUPPORTED;
            break;
        case LOCATION_ERROR_INVALID_PARAMETER:
            response = LOC_INT_RESPONSE_PARAM_INVALID;
            break;
        default:
            response = LOC_INT_RESPONSE_FAILURE;
            break;
    }

    return response;
}

/******************************************************************************
ILocIpcListener override
******************************************************************************/
class IpcListener : public ILocIpcListener {
    MsgTask& mMsgTask;
    LocationIntegrationApiImpl& mApiImpl;
    const SockNode::Type mSockTpye;
public:
    inline IpcListener(LocationIntegrationApiImpl& apiImpl, MsgTask& msgTask,
                       const SockNode::Type sockType) :
            mMsgTask(msgTask), mApiImpl(apiImpl), mSockTpye(sockType) {}
    virtual void onListenerReady() override;
    virtual void onReceive(const char* data, uint32_t length,
                           const LocIpcRecver* recver) override;
};

/******************************************************************************
LocIpcQrtrWatcher override
******************************************************************************/
class HalDaemonQrtrWatcher : public LocIpcQrtrWatcher {
    const weak_ptr<IpcListener> mIpcListener;
    const weak_ptr<LocIpcSender> mIpcSender;
    LocIpcQrtrWatcher::ServiceStatus mKnownStatus;
    MsgTask& mMsgTask;

public:
    inline HalDaemonQrtrWatcher(shared_ptr<IpcListener>& listener, shared_ptr<LocIpcSender>& sender,
                          MsgTask& msgTask) :
            LocIpcQrtrWatcher({LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID}),
            mIpcListener(listener), mIpcSender(sender),
            mMsgTask(msgTask), mKnownStatus(LocIpcQrtrWatcher::ServiceStatus::DOWN) {
    }
    inline virtual void onServiceStatusChange(int serviceId, int instanceId,
            LocIpcQrtrWatcher::ServiceStatus status, const LocIpcSender& refSender) {
        struct onHalServiceStatusChangeHandler : public LocMsg {
            onHalServiceStatusChangeHandler(HalDaemonQrtrWatcher& watcher,
                                            LocIpcQrtrWatcher::ServiceStatus status,
                                            const LocIpcSender& refSender) :
                mWatcher(watcher), mStatus(status), mRefSender(refSender) {}

            virtual ~onHalServiceStatusChangeHandler() {}
            void proc() const {
                if (LocIpcQrtrWatcher::ServiceStatus::UP == mStatus) {
                    LOC_LOGi("LocIpcQrtrWatcher:: HAL Daemon ServiceStatus::UP");
                    auto sender = mWatcher.mIpcSender.lock();
                    if (nullptr != sender && sender->copyDestAddrFrom(mRefSender)) {
                        sleep(2);
                        auto listener = mWatcher.mIpcListener.lock();
                        if (nullptr != listener) {
                            LocAPIHalReadyIndMsg msg(SERVICE_NAME);
                            listener->onReceive((const char*)&msg, sizeof(msg), nullptr);
                        }
                    }
                }
                mWatcher.mKnownStatus = mStatus;
            }

            HalDaemonQrtrWatcher& mWatcher;
            LocIpcQrtrWatcher::ServiceStatus mStatus;
            const LocIpcSender& mRefSender;
        };

        if (LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID == serviceId &&
            LOCATION_CLIENT_API_QSOCKET_HALDAEMON_INSTANCE_ID == instanceId) {
            mMsgTask.sendMsg(new (nothrow)
                     onHalServiceStatusChangeHandler(*this, status, refSender));
        }
    }
};

/******************************************************************************
LocationIntegrationApiImpl
******************************************************************************/
mutex LocationIntegrationApiImpl::mMutex;
bool LocationIntegrationApiImpl::mClientRunning = false;

/******************************************************************************
LocationIntegrationApiImpl - constructors
******************************************************************************/
LocationIntegrationApiImpl::LocationIntegrationApiImpl(LocIntegrationCbs& integrationCbs) :
        mHalRegistered(false),
        mIntegrationCbs(integrationCbs),
        mTuncConfigInfo{},
        mPaceConfigInfo{},
        mSVConfigInfo{},
        mLeverArmConfigInfo{},
        mRobustLocationConfigInfo{},
        mDreConfigInfo{},
        mGtpUserConsentConfigInfo{},
        mRequestLocationInjectionCb(nullptr),
        mNmeaConfigInfo{},
        mRegisterXtraUpdate(false),
        mXtraUpdateUponRegisterPending(false) {

    if (integrationClientAllowed() == false) {
        return;
    }

    // read configuration file
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, gConfigTable);

    mMsgTask = new MsgTask("IntegrationApiImpl", false);
    // get pid to generate sokect name
    uint32_t pid = (uint32_t)getpid();

#ifdef FEATURE_EXTERNAL_AP
    SockNodeEap sock(LOCATION_CLIENT_API_QSOCKET_CLIENT_SERVICE_ID,
                     pid * 100);
    size_t pathNameLength = strlcpy(mSocketName, sock.getNodePathname().c_str(),
                                    sizeof(mSocketName));
    if (pathNameLength >= sizeof(mSocketName)) {
        LOC_LOGe("socket name length exceeds limit of %d bytes", sizeof(mSocketName));
        return;
    }
    // establish an ipc sender to the hal daemon
    mIpcSender = LocIpc::getLocIpcQrtrSender(LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID,
                                             LOCATION_CLIENT_API_QSOCKET_HALDAEMON_INSTANCE_ID);
    if (mIpcSender == nullptr) {
        LOC_LOGe("create sender socket failed for service id: %d instance id: %d",
                 LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID,
                 LOCATION_CLIENT_API_QSOCKET_HALDAEMON_INSTANCE_ID);
        return;
    }
    shared_ptr<IpcListener> listener(make_shared<IpcListener>(*this, *mMsgTask, SockNode::Eap));
    unique_ptr<LocIpcRecver> recver = LocIpc::getLocIpcQrtrRecver(listener,
            sock.getId1(), sock.getId2(),
            make_shared<HalDaemonQrtrWatcher>(listener, mIpcSender, *mMsgTask));
#else
    SockNodeLocal sock(LOCATION_INTEGRATION_API, pid, 0);
    size_t pathNameLength = strlcpy(mSocketName, sock.getNodePathname().c_str(),
                                    sizeof(mSocketName));
    if (pathNameLength >= sizeof(mSocketName)) {
        LOC_LOGe("socket name length exceeds limit of %d bytes", sizeof(mSocketName));
        return;
    }

    LOC_LOGd("create sender socket: %s", mSocketName);
    // establish an ipc sender to the hal daemon
    mIpcSender = LocIpc::getLocIpcLocalSender(SOCKET_TO_LOCATION_HAL_DAEMON);
    if (mIpcSender == nullptr) {
        LOC_LOGe("create sender socket failed %s", SOCKET_TO_LOCATION_HAL_DAEMON);
        return;
    }
    unique_ptr<LocIpcRecver> recver = LocIpc::getLocIpcLocalRecver(
            make_shared<IpcListener>(*this, *mMsgTask, SockNode::Local), mSocketName);
#endif //  FEATURE_EXTERNAL_AP

    LOC_LOGd("listen on socket: %s", mSocketName);
    mIpc.startNonBlockingListening(recver);
}

LocationIntegrationApiImpl::~LocationIntegrationApiImpl() {
}

void LocationIntegrationApiImpl::destroy() {

    struct DestroyReq : public LocMsg {
        DestroyReq(LocationIntegrationApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~DestroyReq() {}
        void proc() const {
            // deregister
            if (mApiImpl->mHalRegistered && (nullptr != mApiImpl->mIpcSender)) {
                LocAPIClientDeregisterReqMsg msg(mApiImpl->mSocketName);
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
                LOC_LOGd(">>> DeregisterReq rc=%d", rc);
                mApiImpl->mIpcSender = nullptr;
            }

            if (mApiImpl->mMsgTask) {
                mApiImpl->mMsgTask->destroy();
            }

            // delete the integration client, so another integration client can be set
            {
                lock_guard<mutex> lock(mMutex);
                mApiImpl->mClientRunning = false;
            }
            delete mApiImpl;
        }
        LocationIntegrationApiImpl* mApiImpl;
    };

    mMsgTask->sendMsg(new (nothrow) DestroyReq(this));
}

bool LocationIntegrationApiImpl::integrationClientAllowed() {
    lock_guard<mutex> lock(mMutex);

    // allow only one instance of integration client running per process
    if (!mClientRunning) {
        mClientRunning = true;
        return true;
    } else {
         LOC_LOGe("there is already a running location integration api client in the process");
         return false;
    }
}

/******************************************************************************
LocationIntegrationApiImpl -ILocIpcListener
******************************************************************************/
void IpcListener::onListenerReady() {
    struct ClientRegisterReq : public LocMsg {
        ClientRegisterReq(LocationIntegrationApiImpl& apiImpl) : mApiImpl(apiImpl) {}
        void proc() const {
            mApiImpl.sendClientRegMsgToHalDaemon();
        }
        LocationIntegrationApiImpl& mApiImpl;
    };
    if (SockNode::Local == mSockTpye) {
        if (0 != chown(mApiImpl.mSocketName, getuid(), GID_LOCCLIENT)) {
            LOC_LOGe("chown to group locclient failed %s", strerror(errno));
        }
    }
    mMsgTask.sendMsg(new (nothrow) ClientRegisterReq(mApiImpl));
}

void IpcListener::onReceive(const char* data, uint32_t length,
                            const LocIpcRecver* recver) {
    struct OnReceiveHandler : public LocMsg {
        OnReceiveHandler(LocationIntegrationApiImpl& apiImpl, IpcListener& listener,
                         const char* data, uint32_t length) :
                mApiImpl(apiImpl), mListener(listener), mMsgData(data, length) {}


        virtual ~OnReceiveHandler() {}
        void proc() const {
            LocAPIMsgHeader *pMsg = (LocAPIMsgHeader *)(mMsgData.data());

            // throw away message that does not come from location hal daemon
            if (false == pMsg->isValidServerMsg(mMsgData.length())) {
                return;
            }

            switch (pMsg->msgId) {
            case E_LOCAPI_HAL_READY_MSG_ID:
                LOC_LOGd("<<< HAL ready");
                if (sizeof(LocAPIHalReadyIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                // location hal daemon crashed and re-started
                mApiImpl.processHalReadyMsg();
                break;

            case E_INTAPI_CONFIG_CONSTRAINTED_TUNC_MSG_ID:
            case E_INTAPI_CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR_MSG_ID:
            case E_INTAPI_CONFIG_SV_CONSTELLATION_MSG_ID:
            case E_INTAPI_CONFIG_AIDING_DATA_DELETION_MSG_ID:
            case E_INTAPI_CONFIG_LEVER_ARM_MSG_ID:
            case E_INTAPI_CONFIG_ROBUST_LOCATION_MSG_ID:
            case E_INTAPI_CONFIG_MIN_GPS_WEEK_MSG_ID:
            case E_INTAPI_CONFIG_DEAD_RECKONING_ENGINE_MSG_ID:
            case E_INTAPI_CONFIG_MIN_SV_ELEVATION_MSG_ID:
            case E_INTAPI_CONFIG_OUTPUT_NMEA_TYPES_MSG_ID:
            case E_INTAPI_CONFIG_USER_CONSENT_TERRESTRIAL_POSITIONING_MSG_ID:
            case E_INTAPI_CONFIG_ENGINE_INTEGRITY_RISK_MSG_ID:
            case E_INTAPI_CONFIG_XTRA_PARAMS_MSG_ID:
            case E_INTAPI_GET_ROBUST_LOCATION_CONFIG_REQ_MSG_ID:
            case E_INTAPI_GET_MIN_GPS_WEEK_REQ_MSG_ID:
            case E_INTAPI_GET_MIN_SV_ELEVATION_REQ_MSG_ID:
            case E_INTAPI_GET_XTRA_STATUS_REQ_MSG_ID:
            case E_INTAPI_REGISTER_XTRA_STATUS_UPDATE_REQ_MSG_ID:
            case E_INTAPI_DEREGISTER_XTRA_STATUS_UPDATE_REQ_MSG_ID:
            {
                if (sizeof(LocAPIGenericRespMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                mApiImpl.processConfigRespCb((LocAPIGenericRespMsg*)pMsg);
                break;
            }

            case E_INTAPI_GET_MIN_GPS_WEEK_RESP_MSG_ID:
            {
                if (sizeof(LocConfigGetMinGpsWeekRespMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                mApiImpl.processGetMinGpsWeekRespCb((LocConfigGetMinGpsWeekRespMsg*)pMsg);
                break;
            }

            case E_INTAPI_GET_ROBUST_LOCATION_CONFIG_RESP_MSG_ID:
            {
                if (sizeof(LocConfigGetRobustLocationConfigRespMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                mApiImpl.processGetRobustLocationConfigRespCb(
                        (LocConfigGetRobustLocationConfigRespMsg*)pMsg);
                break;
            }

            case E_INTAPI_GET_MIN_SV_ELEVATION_RESP_MSG_ID:
            {
                if (sizeof(LocConfigGetMinSvElevationRespMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                mApiImpl.processGetMinSvElevationRespCb((LocConfigGetMinSvElevationRespMsg*)pMsg);
                break;
            }

            case E_INTAPI_CONFIG_ODCPI_INJECT_CB_MSG_ID:
            {
                if (sizeof(LocConfigOdcpiInjectReqCBMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                LocConfigOdcpiInjectReqCBMsg* pOdcpiInjectReqCBMsg =
                        (LocConfigOdcpiInjectReqCBMsg*)pMsg;
                mApiImpl.odcpiRequestCb(pOdcpiInjectReqCBMsg->mRequestInfo);
                break;
            }

            case E_INTAPI_GET_XTRA_STATUS_RESP_MSG_ID:
            {
                if (sizeof(LocConfigGetXtraStatusRespMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                    return;
                }

                mApiImpl.processGetXtraStatusRespCb((LocConfigGetXtraStatusRespMsg*)pMsg);
                break;
            }

            default:
                LOC_LOGw("<<< unknown message %d", pMsg->msgId);
                break;
            }
        }
        LocationIntegrationApiImpl& mApiImpl;
        IpcListener& mListener;
        const string mMsgData;
    };

    mMsgTask.sendMsg(new (nothrow) OnReceiveHandler(mApiImpl, *this, data, length));
}

/******************************************************************************
LocationIntegrationApi - integration API implementation
******************************************************************************/
uint32_t LocationIntegrationApiImpl::resetConstellationConfig() {
    struct ResetConstellationConfigReq : public LocMsg {
        ResetConstellationConfigReq(LocationIntegrationApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~ResetConstellationConfigReq() {}
        void proc() const {
            GnssSvTypeConfig svTypeConfig = {};
            GnssSvIdConfig   svIdConfig = {};
            LocConfigSvConstellationReqMsg msg(mApiImpl->mSocketName,
                                               true, // resetToDefault: true
                                               svTypeConfig,
                                               svIdConfig);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_CONSTELLATIONS,
                                                   reinterpret_cast<uint8_t*>(&msg),
                                                   sizeof(msg))) {
                // cache the last config to be used when hal daemon restarts
                mApiImpl->mSVConfigInfo.isValid = true;
                mApiImpl->mSVConfigInfo.resetToDeFault = true;
                mApiImpl->mSVConfigInfo.svTypeConfig = {};
                mApiImpl->mSVConfigInfo.svIdConfig = {};
            }
        }
        LocationIntegrationApiImpl* mApiImpl;
    };
    mMsgTask->sendMsg(new (nothrow) ResetConstellationConfigReq(this));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configConstellations(
        const GnssSvTypeConfig& svTypeConfig,
        const GnssSvIdConfig& svIdConfig) {

    struct ConfigConstellationsReq : public LocMsg {
        ConfigConstellationsReq(LocationIntegrationApiImpl* apiImpl,
                                const GnssSvTypeConfig& svTypeConfig,
                                const GnssSvIdConfig& svIdConfig) :
                mApiImpl(apiImpl),
                mSvTypeConfig(svTypeConfig),
                mSvIdConfig(svIdConfig) {}
        virtual ~ConfigConstellationsReq() {}
        void proc() const {
            LocConfigSvConstellationReqMsg msg(mApiImpl->mSocketName,
                                               false, // resetToDefault: false
                                               mSvTypeConfig,
                                               mSvIdConfig);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_CONSTELLATIONS,
                                                   reinterpret_cast<uint8_t*>(&msg),
                                                   sizeof(msg))) {
                // cache the last config to be used when hal daemon restarts
                mApiImpl->mSVConfigInfo.isValid = true;
                mApiImpl->mSVConfigInfo.resetToDeFault = false;
                mApiImpl->mSVConfigInfo.svTypeConfig = mSvTypeConfig;
                mApiImpl->mSVConfigInfo.svIdConfig = mSvIdConfig;
            }
        }

        LocationIntegrationApiImpl* mApiImpl;
        GnssSvTypeConfig mSvTypeConfig;
        GnssSvIdConfig mSvIdConfig;
    };
    mMsgTask->sendMsg(new (nothrow) ConfigConstellationsReq(
            this, svTypeConfig, svIdConfig));
    return 0;
}

uint32_t LocationIntegrationApiImpl::configConstrainedTimeUncertainty(
        bool enable, float tuncThreshold, uint32_t energyBudget) {

    struct ConfigConstrainedTuncReq : public LocMsg {
        ConfigConstrainedTuncReq(LocationIntegrationApiImpl* apiImpl,
                                 bool enable,
                                 float tuncThreshold,
                                 uint32_t energyBudget) :
                mApiImpl(apiImpl),
                mEnable(enable),
                mTuncThreshold(tuncThreshold),
                mEnergyBudget(energyBudget) {}
        virtual ~ConfigConstrainedTuncReq() {}
        void proc() const {
            LocConfigConstrainedTuncReqMsg msg(mApiImpl->mSocketName,
                                               mEnable, mTuncThreshold, mEnergyBudget);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_CONSTRAINED_TIME_UNCERTAINTY,
                                                   reinterpret_cast<uint8_t*>(&msg),
                                                   sizeof(msg))) {
               // cache the last config to be used when hal daemon restarts
               mApiImpl->mTuncConfigInfo.isValid = true;
               mApiImpl->mTuncConfigInfo.enable = mEnable;
               mApiImpl->mTuncConfigInfo.tuncThresholdMs = mTuncThreshold;
               mApiImpl->mTuncConfigInfo.energyBudget = mEnergyBudget;
            }
        }
        LocationIntegrationApiImpl* mApiImpl;
        bool mEnable;
        float mTuncThreshold;
        uint32_t mEnergyBudget;
    };
    mMsgTask->sendMsg(new (nothrow)ConfigConstrainedTuncReq(
            this, enable, tuncThreshold, energyBudget));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configPositionAssistedClockEstimator(bool enable) {

    struct ConfigPositionAssistedClockEstimatorReq : public LocMsg {
        ConfigPositionAssistedClockEstimatorReq(LocationIntegrationApiImpl* apiImpl,
                                                bool enable) :
                mApiImpl(apiImpl),
                mEnable(enable) {}
        virtual ~ConfigPositionAssistedClockEstimatorReq() {}
        void proc() const {
            LocConfigPositionAssistedClockEstimatorReqMsg msg(mApiImpl->mSocketName,
                                                              mEnable);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR,
                                                   reinterpret_cast<uint8_t*>(&msg),
                                                   sizeof(msg))) {
                // cache the last config to be used when hal daemon restarts
                mApiImpl->mPaceConfigInfo.isValid = true;
                mApiImpl->mPaceConfigInfo.enable = mEnable;
            }
        }
        LocationIntegrationApiImpl* mApiImpl;
        bool mEnable;
    };
    mMsgTask->sendMsg(new (nothrow)
            ConfigPositionAssistedClockEstimatorReq(this, enable));

    return 0;
}

uint32_t LocationIntegrationApiImpl::gnssDeleteAidingData(
        GnssAidingData& aidingData) {
    struct DeleteAidingDataReq : public LocMsg {
        DeleteAidingDataReq(LocationIntegrationApiImpl* apiImpl,
                            GnssAidingData& aidingData) :
                mApiImpl(apiImpl),
                mAidingData(aidingData) {}
        virtual ~DeleteAidingDataReq() {}
        void proc() const {
            LocConfigAidingDataDeletionReqMsg msg(mApiImpl->mSocketName,
                                                  const_cast<GnssAidingData&>(mAidingData));
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_AIDING_DATA_DELETION,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
        }
        LocationIntegrationApiImpl* mApiImpl;
        GnssAidingData mAidingData;
    };
    mMsgTask->sendMsg(new (nothrow) DeleteAidingDataReq(this, aidingData));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configLeverArm(
        const LeverArmConfigInfo& configInfo) {
    struct ConfigLeverArmReq : public LocMsg {
        ConfigLeverArmReq(LocationIntegrationApiImpl* apiImpl,
                          const LeverArmConfigInfo& configInfo) :
                mApiImpl(apiImpl),
                mConfigInfo(configInfo) {}
        virtual ~ConfigLeverArmReq() {}
        void proc() const {
            LocConfigLeverArmReqMsg msg(mApiImpl->mSocketName, mConfigInfo);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_LEVER_ARM,
                                                   reinterpret_cast<uint8_t*>(&msg),
                                                   sizeof(msg))) {
                // cache the last config to be used when hal daemon restarts
                mApiImpl->mLeverArmConfigInfo = mConfigInfo;
            }
        }
        LocationIntegrationApiImpl* mApiImpl;
        LeverArmConfigInfo mConfigInfo;
    };

    if (configInfo.leverArmValidMask) {
        mMsgTask->sendMsg(new (nothrow) ConfigLeverArmReq(this, configInfo));
    }

    return 0;
}

uint32_t LocationIntegrationApiImpl::configRobustLocation(
        bool enable, bool enableForE911) {
    struct ConfigRobustLocationReq : public LocMsg {
        ConfigRobustLocationReq(LocationIntegrationApiImpl* apiImpl,
                                bool enable,
                                bool enableForE911) :
                mApiImpl(apiImpl),
                mEnable(enable),
                mEnableForE911(enableForE911){}
        virtual ~ConfigRobustLocationReq() {}
        void proc() const {
            LocConfigRobustLocationReqMsg msg(mApiImpl->mSocketName, mEnable, mEnableForE911);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_ROBUST_LOCATION,
                                                   reinterpret_cast<uint8_t*>(&msg),
                                                   sizeof(msg))) {
                mApiImpl->mRobustLocationConfigInfo.isValid = true;
                mApiImpl->mRobustLocationConfigInfo.enable = mEnable;
                mApiImpl->mRobustLocationConfigInfo.enableForE911 = mEnableForE911;
            }
        }
        LocationIntegrationApiImpl* mApiImpl;
        bool mEnable;
        bool mEnableForE911;
    };

    mMsgTask->sendMsg(new (nothrow)
                      ConfigRobustLocationReq(this, enable, enableForE911));

    return 0;
}

uint32_t LocationIntegrationApiImpl::getRobustLocationConfig() {

    struct GetRobustLocationConfigReq : public LocMsg {
        GetRobustLocationConfigReq(LocationIntegrationApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~GetRobustLocationConfigReq() {}
        void proc() const {
            LocConfigGetRobustLocationConfigReqMsg msg(mApiImpl->mSocketName);
            mApiImpl->sendConfigMsgToHalDaemon(GET_ROBUST_LOCATION_CONFIG,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
        }
        LocationIntegrationApiImpl* mApiImpl;
    };

    if (mIntegrationCbs.getRobustLocationConfigCb == nullptr) {
        LOC_LOGe("no callback passed in constructor to receive robust location config");
        // return 1 to signal error
        return 1;
    }
    mMsgTask->sendMsg(new (nothrow) GetRobustLocationConfigReq(this));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configMinGpsWeek(uint16_t minGpsWeek) {
    struct ConfigMinGpsWeekReq : public LocMsg {
        ConfigMinGpsWeekReq(LocationIntegrationApiImpl* apiImpl,
                            uint16_t minGpsWeek) :
                mApiImpl(apiImpl),
                mMinGpsWeek(minGpsWeek) {}
        virtual ~ConfigMinGpsWeekReq() {}
        void proc() const {
            LocConfigMinGpsWeekReqMsg msg(mApiImpl->mSocketName, mMinGpsWeek);
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_MIN_GPS_WEEK,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
        }
        LocationIntegrationApiImpl* mApiImpl;
        uint16_t mMinGpsWeek;
    };

    mMsgTask->sendMsg(new (nothrow) ConfigMinGpsWeekReq(this, minGpsWeek));
    return 0;
}

uint32_t LocationIntegrationApiImpl::getMinGpsWeek() {

    struct GetMinGpsWeekReq : public LocMsg {
        GetMinGpsWeekReq(LocationIntegrationApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~GetMinGpsWeekReq() {}
        void proc() const {
            LocConfigGetMinGpsWeekReqMsg msg(mApiImpl->mSocketName);
            mApiImpl->sendConfigMsgToHalDaemon(GET_MIN_GPS_WEEK,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
        }
        LocationIntegrationApiImpl* mApiImpl;
    };

    if (mIntegrationCbs.getMinGpsWeekCb == nullptr) {
        LOC_LOGe("no callback passed in constructor to receive gps week info");
        // return 1 to signal error
        return 1;
    }
    mMsgTask->sendMsg(new (nothrow) GetMinGpsWeekReq(this));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configDeadReckoningEngineParams(
        const ::DeadReckoningEngineConfig& dreConfig) {
    struct ConfigDrEngineParamsReq : public LocMsg {
        ConfigDrEngineParamsReq(LocationIntegrationApiImpl* apiImpl,
                                ::DeadReckoningEngineConfig dreConfig) :
                mApiImpl(apiImpl),
                mDreConfig(dreConfig){}
        virtual ~ConfigDrEngineParamsReq() {}
        void proc() const {
            LocConfigDrEngineParamsReqMsg msg(mApiImpl->mSocketName,
                                              mDreConfig);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_DEAD_RECKONING_ENGINE,
                                                   reinterpret_cast<uint8_t*>(&msg),
                                                   sizeof(msg))) {
                mApiImpl->mDreConfigInfo.isValid = true;
                mApiImpl->mDreConfigInfo.dreConfig = mDreConfig;
            }
        }
        LocationIntegrationApiImpl* mApiImpl;
        ::DeadReckoningEngineConfig mDreConfig;
    };

    mMsgTask->sendMsg(new (nothrow) ConfigDrEngineParamsReq(this, dreConfig));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configMinSvElevation(uint8_t minSvElevation) {

        struct ConfigMinSvElevationReq : public LocMsg {
        ConfigMinSvElevationReq(LocationIntegrationApiImpl* apiImpl,
                                uint8_t minSvElevation) :
                mApiImpl(apiImpl), mMinSvElevation(minSvElevation){}
        virtual ~ConfigMinSvElevationReq() {}
        void proc() const {
            LocConfigMinSvElevationReqMsg msg(mApiImpl->mSocketName, mMinSvElevation);
            mApiImpl->sendConfigMsgToHalDaemon(CONFIG_MIN_SV_ELEVATION,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
        }
        LocationIntegrationApiImpl* mApiImpl;
        uint8_t mMinSvElevation;
    };

    mMsgTask->sendMsg(new (nothrow) ConfigMinSvElevationReq(this, minSvElevation));
    return 0;
}

uint32_t LocationIntegrationApiImpl::getMinSvElevation() {

    struct GetMinSvElevationReq : public LocMsg {
        GetMinSvElevationReq(LocationIntegrationApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~GetMinSvElevationReq() {}
        void proc() const {
            LocConfigGetMinSvElevationReqMsg msg(mApiImpl->mSocketName);
            mApiImpl->sendConfigMsgToHalDaemon(GET_MIN_SV_ELEVATION,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
        }
        LocationIntegrationApiImpl* mApiImpl;
    };

    if (mIntegrationCbs.getMinSvElevationCb == nullptr) {
        LOC_LOGe("no callback passed in constructor to receive min sv elevation info");
        // return 1 to signal error
        return 1;
    }
    mMsgTask->sendMsg(new (nothrow) GetMinSvElevationReq(this));

    return 0;
}

uint32_t LocationIntegrationApiImpl::configOutputNmeaTypes(
        GnssNmeaTypesMask enabledNmeaTypes,
        GnssGeodeticDatumType nmeaDatumType) {
    struct ConfigOutputNmeaReq : public LocMsg {
        ConfigOutputNmeaReq(LocationIntegrationApiImpl* apiImpl,
                            GnssNmeaTypesMask enabledNmeaTypes,
                            GnssGeodeticDatumType nmeaDatumType) :
                mApiImpl(apiImpl), mEnabledNmeaTypes(enabledNmeaTypes),
                mNmeaDatumType(nmeaDatumType) {}
        virtual ~ConfigOutputNmeaReq() {}
        void proc() const {
            LocConfigOutputNmeaTypesReqMsg msg(mApiImpl->mSocketName,
                                               mEnabledNmeaTypes, mNmeaDatumType);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_OUTPUT_NMEA_TYPES,
                                                   reinterpret_cast<uint8_t*>(&msg),
                                                   sizeof(msg))) {
                mApiImpl->mNmeaConfigInfo.isValid = true;
                mApiImpl->mNmeaConfigInfo.enabledNmeaTypes = mEnabledNmeaTypes;
                mApiImpl->mNmeaConfigInfo.nmeaDatumType = mNmeaDatumType;
            }
        }

        LocationIntegrationApiImpl* mApiImpl;
        GnssNmeaTypesMask mEnabledNmeaTypes;
        GnssGeodeticDatumType mNmeaDatumType;
    };

    mMsgTask->sendMsg(new (nothrow) ConfigOutputNmeaReq(this, enabledNmeaTypes,
                                                        nmeaDatumType));
    return 0;
}

uint32_t LocationIntegrationApiImpl::setUserConsentForTerrestrialPositioning(bool userConsent) {
    struct SetUserConsentReq : public LocMsg {
        SetUserConsentReq(LocationIntegrationApiImpl* apiImpl,
                          bool userConsent) :
                mApiImpl(apiImpl), mUserConsent(userConsent) {}
        virtual ~SetUserConsentReq() {}
        void proc() const {
            string pbStr;
            LocConfigUserConsentTerrestrialPositioningReqMsg msg(
                    mApiImpl->mSocketName, mUserConsent);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_USER_CONSENT_TERRESTRIAL_POSITIONING,
                                                   reinterpret_cast<uint8_t*>(&msg), sizeof(msg))) {
                mApiImpl->mGtpUserConsentConfigInfo.isValid = true;
                mApiImpl->mGtpUserConsentConfigInfo.userConsent = mUserConsent;
            }
        }

        LocationIntegrationApiImpl* mApiImpl;
        bool mUserConsent;
    };

    mMsgTask->sendMsg(new (nothrow) SetUserConsentReq(this, userConsent));
    return 0;
}

void LocationIntegrationApiImpl::odcpiRequestCb(const OdcpiRequestInfo& request) {

    LOC_LOGd("request type %d tbf %d", request.type, request.tbfMillis);
    LocationInjectRequestType requestType;
    switch (request.type) {
        case OdcpiRequestType::ODCPI_REQUEST_TYPE_START:
            requestType = LocationInjectRequestType::LOCATION_INJECT_REQUEST_TYPE_START;
            break;
        case OdcpiRequestType::ODCPI_REQUEST_TYPE_STOP:
            requestType = LocationInjectRequestType::LOCATION_INJECT_REQUEST_TYPE_STOP;
            break;
        default:
            LOC_LOGe("Undefined request type %d", request.type);
            return;
    }

    if (mRequestLocationInjectionCb) {
        mRequestLocationInjectionCb(requestType, request.tbfMillis);
    } else {
        LOC_LOGe("odcpiRequestCb received with null mRequestLocationInjectionCb");
    }
}

uint32_t LocationIntegrationApiImpl::registerLocationInjector(
        LocRequestLocationInjectionCb requestLocationInjectionCb) {

    mRequestLocationInjectionCb = requestLocationInjectionCb;

    odcpiRequestCallback cb = [this](const OdcpiRequestInfo& odcpiRequest) {
        odcpiRequestCb(odcpiRequest);
    };
    odcpiInit(cb, OdcpiPrioritytype::ODCPI_HANDLER_PRIORITY_DEFAULT);
    return 0;
}

void LocationIntegrationApiImpl::odcpiInit(const odcpiRequestCallback& callback,
                                           OdcpiPrioritytype priority) {

    struct RegisterLocationInjectorReq : public LocMsg {
        RegisterLocationInjectorReq(
                LocationIntegrationApiImpl* apiImpl) : mApiImpl(apiImpl){}
        virtual ~RegisterLocationInjectorReq() {}
        void proc() const {
            bool registerOdcpiInit =
                (nullptr != mApiImpl->mRequestLocationInjectionCb) ? true : false;
            LOC_LOGd("registerOdcpiInit %d", registerOdcpiInit);
            LocConfigOdcpiInitReqMsg msg(mApiImpl->mSocketName, registerOdcpiInit);
                // no ResponseCb on purpose
                mApiImpl->sendConfigMsgToHalDaemon(
                        (LocConfigTypeEnum)0,
                        reinterpret_cast<uint8_t*>(&msg),
                        sizeof(msg), false);

        }

        LocationIntegrationApiImpl* mApiImpl;
    };

    (void)priority;
    mMsgTask->sendMsg(new (nothrow) RegisterLocationInjectorReq(this));
}

void LocationIntegrationApiImpl::odcpiInject(const ::Location &location) {

    struct InjectBestLocationReq : public LocMsg {
        InjectBestLocationReq(LocationIntegrationApiImpl* apiImpl,
                const ::Location &location) : mApiImpl(apiImpl), mLocation(location) {}
        virtual ~InjectBestLocationReq() {}
        void proc() const {
            LocConfigOdcpiInjectReqMsg msg(mApiImpl->mSocketName, mLocation);
            // no ResponseCb on purpose
            mApiImpl->sendConfigMsgToHalDaemon(
                    (LocConfigTypeEnum)0,
                    reinterpret_cast<uint8_t*>(&msg),
                    sizeof(msg), false);
        }

        LocationIntegrationApiImpl* mApiImpl;
        const ::Location            mLocation;
    };

    mMsgTask->sendMsg(new (nothrow) InjectBestLocationReq(this, location));
}

uint32_t LocationIntegrationApiImpl::configEngineIntegrityRisk(
        PositioningEngineMask engType, uint32_t integrityRisk) {

    struct ConfigEngineIntegrityRiskReq : public LocMsg {
        ConfigEngineIntegrityRiskReq(LocationIntegrationApiImpl* apiImpl,
                                     PositioningEngineMask engType,
                                     uint32_t integrityRisk) :
                mApiImpl(apiImpl), mEngType(engType), mIntegrityRisk(integrityRisk) {}
        virtual ~ConfigEngineIntegrityRiskReq() {}
        void proc() const {
            LOC_LOGd("eng type %d, integrity risk %u", mEngType, mIntegrityRisk);

            LocConfigEngineIntegrityRiskReqMsg msg(mApiImpl->mSocketName,
                                                   mEngType, mIntegrityRisk);
            if (mApiImpl->sendConfigMsgToHalDaemon(CONFIG_ENGINE_INTEGRITY_RISK,
                                                   reinterpret_cast<uint8_t*>(&msg),
                                                   sizeof(msg))) {
                if (mApiImpl->mEngIntegrityRiskConfigMap.find(mEngType) ==
                            std::end(mApiImpl->mEngIntegrityRiskConfigMap)) {
                    mApiImpl->mEngIntegrityRiskConfigMap.emplace(mEngType, mIntegrityRisk);
                } else {
                    // change the state for the eng
                    mApiImpl->mEngIntegrityRiskConfigMap[mEngType] = mIntegrityRisk;
                }
            }
        }

        LocationIntegrationApiImpl* mApiImpl;
        PositioningEngineMask mEngType;
        uint32_t mIntegrityRisk;
    };

    mMsgTask->sendMsg(new (nothrow) ConfigEngineIntegrityRiskReq(this, engType, integrityRisk));
    return 0;
}

uint32_t LocationIntegrationApiImpl::configXtraParams(
        bool enable, const ::XtraConfigParams& configParams) {
    struct ConfigXtraReq : public LocMsg {
        ConfigXtraReq(LocationIntegrationApiImpl* apiImpl,
                      bool enable, const ::XtraConfigParams& configParams) :
                mApiImpl(apiImpl), mEnable(enable), mConfigParams(configParams) {}
        virtual ~ConfigXtraReq() {}
        void proc() const {
            if (1 != sXtraTestEnabled) {
                // download interval: 48 hours and 168 hours
                if (mConfigParams.xtraDownloadIntervalMinute != 0) {
                    if (mConfigParams.xtraDownloadIntervalMinute < 48 * 60) {
                        mConfigParams.xtraDownloadIntervalMinute = 48 * 60;
                    } else if (mConfigParams.xtraDownloadIntervalMinute > 168 * 60) {
                        mConfigParams.xtraDownloadIntervalMinute = 168 * 60;
                    }
                }

                // download timeout: maximum of 300 secs and minimum of 3 secs
                if (mConfigParams.xtraDownloadTimeoutSec != 0) {
                    if (mConfigParams.xtraDownloadTimeoutSec < 3) {
                        mConfigParams.xtraDownloadTimeoutSec = 3;
                    } else if (mConfigParams.xtraDownloadTimeoutSec > 300) {
                        mConfigParams.xtraDownloadTimeoutSec = 300;
                    }
                }

                // retry interval: a maximum of 1 day and a minimum of 3 minutes.
                if (mConfigParams.xtraDownloadRetryIntervalMinute != 0) {
                    if (mConfigParams.xtraDownloadRetryIntervalMinute < 3) {
                        mConfigParams.xtraDownloadRetryIntervalMinute = 3;
                    } else if (mConfigParams.xtraDownloadRetryIntervalMinute > 24 * 60) {
                        mConfigParams.xtraDownloadRetryIntervalMinute = 24 * 60;
                    }
                }

                // retry attempts: maximum number of allowed retry is 6 per
                // download interval.
                if (mConfigParams.xtraDownloadRetryAttempts > 6) {
                    mConfigParams.xtraDownloadRetryAttempts = 6;
                }

                // integrity file download interval: min is 6 hours, max is 48 hours
                if (mConfigParams.xtraIntegrityDownloadIntervalMinute < 360) {
                    mConfigParams.xtraIntegrityDownloadIntervalMinute = 360;
                } else if (mConfigParams.xtraIntegrityDownloadIntervalMinute > 2880) {
                    mConfigParams.xtraIntegrityDownloadIntervalMinute = 2880;
                }
            }

            LocConfigXtraReqMsg msg(mApiImpl->mSocketName, mEnable, mConfigParams);
            mApiImpl->sendConfigMsgToHalDaemon(
                        CONFIG_XTRA_PARAMS, reinterpret_cast<uint8_t*>(&msg),
                        sizeof(msg));
        }

        LocationIntegrationApiImpl* mApiImpl;
        bool mEnable;
        mutable ::XtraConfigParams mConfigParams;
    };

    mMsgTask->sendMsg(new (nothrow) ConfigXtraReq(this, enable, configParams));
    return 0;
}

uint32_t LocationIntegrationApiImpl::getXtraStatus() {

    struct GetXtraStatusReq : public LocMsg {
        GetXtraStatusReq(LocationIntegrationApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~GetXtraStatusReq() {}
        void proc() const {
            string pbStr;
            LocConfigGetXtraStatusReqMsg msg(mApiImpl->mSocketName);
            mApiImpl->sendConfigMsgToHalDaemon(GET_XTRA_STATUS,
                                               reinterpret_cast<uint8_t*>(&msg),
                                               sizeof(msg));
        }
        LocationIntegrationApiImpl* mApiImpl;
    };

    if (mIntegrationCbs.getXtraStatusCb == nullptr) {
        LOC_LOGe("no callback passed in constructor to receive xtra status");
        // return 1 to signal error
        return 1;
    }
    mMsgTask->sendMsg(new (nothrow) GetXtraStatusReq(this));
    return 0;
}

uint32_t LocationIntegrationApiImpl::registerXtraStatusUpdate(bool registerUpdate) {

    struct RegisterXtraStatusUpdateReq : public LocMsg {
        RegisterXtraStatusUpdateReq(LocationIntegrationApiImpl* apiImpl,
                      bool registerUpdate) :
                mApiImpl(apiImpl), mRegisterUpdate(registerUpdate) {}
        virtual ~RegisterXtraStatusUpdateReq() {}
        void proc() const {
            LOC_LOGe("registerXtraStatusUpdate: %d", mRegisterUpdate);

            if (mRegisterUpdate == true) {
                LocConfigRegisterXtraStatusUpdateReqMsg msg(mApiImpl->mSocketName);
                if (mApiImpl->sendConfigMsgToHalDaemon(REGISTER_XTRA_STATUS_UPDATE,
                            reinterpret_cast<uint8_t*>(&msg), sizeof(msg))) {
                    mApiImpl->mXtraUpdateUponRegisterPending = true;
                    mApiImpl->mRegisterXtraUpdate = mRegisterUpdate;
                }
            } else {
                LocConfigDeregisterXtraStatusUpdateReqMsg msg(mApiImpl->mSocketName);
                if (mApiImpl->sendConfigMsgToHalDaemon(REGISTER_XTRA_STATUS_UPDATE,
                            reinterpret_cast<uint8_t*>(&msg), sizeof(msg))) {
                    mApiImpl->mXtraUpdateUponRegisterPending = false;
                    mApiImpl->mRegisterXtraUpdate = mRegisterUpdate;
                }
            }
        }

        LocationIntegrationApiImpl* mApiImpl;
        bool mRegisterUpdate;
    };

    if (mIntegrationCbs.getXtraStatusCb == nullptr) {
        LOC_LOGe("no callback passed in constructor to receive xtra status");
        // return 1 to signal error
        return 1;
    }
    mMsgTask->sendMsg(new (nothrow) RegisterXtraStatusUpdateReq(this, registerUpdate));
    return 0;
}

bool LocationIntegrationApiImpl::sendConfigMsgToHalDaemon(
        LocConfigTypeEnum configType, uint8_t* pMsg,
        size_t msgSize, bool invokeResponseCb) {
    bool rc = false;
    LOC_LOGd(">>> sendConfigMsgToHalDaemon, mHalRegistered %d, config type=%d, "
             "msg size %d, config cb %d",
             mHalRegistered, configType, msgSize, invokeResponseCb);

    if (!mHalRegistered) {
        ConfigMsg msgInfo;
        msgInfo.msgType = configType;
        msgInfo.pMsg = (uint8_t*) malloc(msgSize);
        if (NULL != msgInfo.pMsg) {
            memcpy(msgInfo.pMsg, pMsg, msgSize);
            msgInfo.msgSize = msgSize;
            mQueuedMsg.emplace(std::move(msgInfo));
        }
        LOC_LOGi(">>> sendConfigMsgToHalDaemon mHal not yet ready, message queued");
        // set rc to trigger saving the configuration
        rc = true;
    } else {
        bool messageSentToHal = false;
        rc = sendMessage(pMsg, msgSize);
        LOC_LOGd(">>> sendConfigMsgToHalDaemon, msg type=%d, rc=%d", configType, rc);
        if (true == rc) {
            messageSentToHal = true;
        } else {
            LOC_LOGe(">>> sendConfigMsgToHalDaemon failed for msg type=%d", configType);
        }

        if (invokeResponseCb && mIntegrationCbs.configCb) {
            if (true == messageSentToHal) {
                addConfigReq(configType);
            } else {
                mIntegrationCbs.configCb(configType, LOC_INT_RESPONSE_FAILURE);
            }
        }
    }
    return rc;
}

bool LocationIntegrationApiImpl::sendClientRegMsgToHalDaemon(){
    bool retVal = false;
    LocAPIClientRegisterReqMsg msg(mSocketName, LOCATION_INTEGRATION_API);
    bool rc = sendMessage(reinterpret_cast<uint8_t *>(&msg), sizeof(msg));
    LOC_LOGd(">>> onListenerReady::ClientRegisterReqMsg rc=%d", rc);
    if (true == rc) {
        mHalRegistered = true;
        retVal = true;
    }
    return retVal;
}

void LocationIntegrationApiImpl::processHalReadyMsg() {

    // first, send registration msg to hal daemon
    if (sendClientRegMsgToHalDaemon() == false) {
        LOC_LOGe("failed to register with HAL, return");
        return;
    }


    // process the requests that are queued before hal daemon was first ready
    if (processQueuedReqs()) {
        // hal is first time ready, no more item to process
        return;
    }

    // when location hal daemon crashes and restarts,
    // we flush out all pending requests and notify each client
    flushConfigReqs();

    // send cached configuration to hal daemon
    if (mSVConfigInfo.isValid) {
        LocConfigSvConstellationReqMsg msg(mSocketName,
                                           mSVConfigInfo.resetToDeFault,
                                           mSVConfigInfo.svTypeConfig,
                                           mSVConfigInfo.svIdConfig);
        sendConfigMsgToHalDaemon(CONFIG_CONSTELLATIONS,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg), false);
    }
    if (mTuncConfigInfo.isValid) {
        LocConfigConstrainedTuncReqMsg msg(mSocketName,
                                           mTuncConfigInfo.enable,
                                           mTuncConfigInfo.tuncThresholdMs,
                                           mTuncConfigInfo.energyBudget);
        sendConfigMsgToHalDaemon(CONFIG_CONSTRAINED_TIME_UNCERTAINTY,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg), false);
    }
    if (mPaceConfigInfo.isValid) {
        LocConfigPositionAssistedClockEstimatorReqMsg msg(mSocketName,
                                                          mPaceConfigInfo.enable);
        sendConfigMsgToHalDaemon(CONFIG_POSITION_ASSISTED_CLOCK_ESTIMATOR,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg), false);
    }
    if (mLeverArmConfigInfo.leverArmValidMask) {
        LocConfigLeverArmReqMsg msg(mSocketName, mLeverArmConfigInfo);
        sendConfigMsgToHalDaemon(CONFIG_LEVER_ARM,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg));
    }
    if (mRobustLocationConfigInfo.isValid) {
        LocConfigRobustLocationReqMsg msg(mSocketName,
                                          mRobustLocationConfigInfo.enable,
                                          mRobustLocationConfigInfo.enableForE911);
        sendConfigMsgToHalDaemon(CONFIG_ROBUST_LOCATION,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg));
    }
    // Do not reconfigure min gps week, as min gps week setting
    // can be overwritten by modem over  time

    if (mDreConfigInfo.isValid) {
        LocConfigDrEngineParamsReqMsg msg(mSocketName, mDreConfigInfo.dreConfig);
        sendConfigMsgToHalDaemon(CONFIG_DEAD_RECKONING_ENGINE,
                                 reinterpret_cast<uint8_t*>(&msg),
                                 sizeof(msg));
    }

    if (mNmeaConfigInfo.isValid) {
         LocConfigOutputNmeaTypesReqMsg msg(mSocketName, mNmeaConfigInfo.enabledNmeaTypes,
                                            mNmeaConfigInfo.nmeaDatumType);
         sendConfigMsgToHalDaemon(CONFIG_OUTPUT_NMEA_TYPES,
                                  reinterpret_cast<uint8_t*>(&msg),
                                  sizeof(msg));
    }

    if (mGtpUserConsentConfigInfo.isValid) {
        string pbStr;
        LocConfigUserConsentTerrestrialPositioningReqMsg msg(
                    mSocketName, mGtpUserConsentConfigInfo.userConsent);
        sendConfigMsgToHalDaemon(CONFIG_USER_CONSENT_TERRESTRIAL_POSITIONING,
                                  reinterpret_cast<uint8_t*>(&msg),
                                  sizeof(msg));
    }

    if (mRequestLocationInjectionCb) {
        LocConfigOdcpiInitReqMsg msg(mSocketName, true);
        // no ResponseCb on purpose
        sendConfigMsgToHalDaemon(
                    (LocConfigTypeEnum)0,
                    reinterpret_cast<uint8_t*>(&msg),
                    sizeof(msg), false);
    }

    // resend integrity risk config request
    for (auto it = mEngIntegrityRiskConfigMap.begin();
            it != mEngIntegrityRiskConfigMap.end(); ++it) {
        LocConfigEngineIntegrityRiskReqMsg msg(mSocketName, it->first, it->second);
        sendConfigMsgToHalDaemon(CONFIG_ENGINE_INTEGRITY_RISK,
                                  reinterpret_cast<uint8_t*>(&msg),
                                  sizeof(msg));
    }

    // resend XTRA status registration message request
    if (mRegisterXtraUpdate) {
        LocConfigRegisterXtraStatusUpdateReqMsg msg(mSocketName);
        sendConfigMsgToHalDaemon(REGISTER_XTRA_STATUS_UPDATE,
                                 reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
    }
}

void LocationIntegrationApiImpl::addConfigReq(LocConfigTypeEnum configType) {
    LOC_LOGv("add configType %d", configType);

    auto configReqData = mConfigReqCntMap.find(configType);
    if (configReqData == std::end(mConfigReqCntMap)) {
        mConfigReqCntMap.emplace(configType, 1);
    } else{
        int newCnt = configReqData->second+1;
        mConfigReqCntMap.erase(configReqData);
        mConfigReqCntMap.emplace(configType, newCnt);
    }
}

void LocationIntegrationApiImpl::processConfigRespCb(const LocAPIGenericRespMsg* pRespMsg) {
    LocConfigTypeEnum configType = getLocConfigTypeFromMsgId(pRespMsg->msgId);
    LocIntegrationResponse intResponse = getLocIntegrationResponse(pRespMsg->err);
    LOC_LOGd("<<< response message id: %d, msg err: %d, "
             "config type: %d, int response %d",
             pRespMsg->msgId, pRespMsg->err, configType, intResponse);

    if (mIntegrationCbs.configCb) {
        auto configReqData = mConfigReqCntMap.find(configType);
        if (configReqData != std::end(mConfigReqCntMap)) {
            int reqCnt = configReqData->second;
            if (reqCnt > 0) {
                mIntegrationCbs.configCb(configType, intResponse);
            }
            mConfigReqCntMap.erase(configReqData);
            // there are still some request pending for this config type
            // need to store it in the map
            if (--reqCnt > 0) {
                mConfigReqCntMap.emplace(configType, reqCnt);
            }
        }
    }
}

// process queued reqs that are not able to sent to location hal daemon
bool LocationIntegrationApiImpl::processQueuedReqs() {
    bool queueNotEmpty = (mQueuedMsg.size() > 0);
    while (mQueuedMsg.size() > 0) {
        ConfigMsg msgInfo = mQueuedMsg.front();
        mQueuedMsg.pop();
        sendConfigMsgToHalDaemon(msgInfo.msgType, msgInfo.pMsg, msgInfo.msgSize);
        free(msgInfo.pMsg);
    }
    return queueNotEmpty;
}

// flush all the pending config request if location hal daemon has crashed
// and restarted
void LocationIntegrationApiImpl::flushConfigReqs() {
    for (auto itr=mConfigReqCntMap.begin(); itr!=mConfigReqCntMap.end(); ++itr) {
         int reqCnt = itr->second;
         while (reqCnt-- > 0) {
             if (itr->first <= CONFIG_ENUM_MAX) {
                 // config command are cached, and will be re-issued when
                 // hal daemon crashed and then restarted
                 mIntegrationCbs.configCb(itr->first, LOC_INT_RESPONSE_SUCCESS);
             } else {
                 // get command are not cached, and will not be re-issued when
                 // hal daemon crashed and then restarted
                 mIntegrationCbs.configCb(itr->first, LOC_INT_RESPONSE_FAILURE);
             }
         }
    }
    mConfigReqCntMap.clear();
}

void LocationIntegrationApiImpl::processGetRobustLocationConfigRespCb(
        const LocConfigGetRobustLocationConfigRespMsg* pRespMsg) {

    LOC_LOGd("<<< response message id: %d, mask 0x%x, enabled: %d, enabledFor911: %d",
             pRespMsg->msgId,
             pRespMsg->mRobustLoationConfig.validMask,
             pRespMsg->mRobustLoationConfig.enabled,
             pRespMsg->mRobustLoationConfig.enabledForE911);

    if (mIntegrationCbs.getRobustLocationConfigCb) {
        // conversion between the enums
        RobustLocationConfig robustConfig = {};
        uint32_t validMask = 0;;
        if (pRespMsg->mRobustLoationConfig.validMask &
                GNSS_CONFIG_ROBUST_LOCATION_ENABLED_VALID_BIT) {
            validMask |= ROBUST_LOCATION_CONFIG_VALID_ENABLED;
            robustConfig.enabled = pRespMsg->mRobustLoationConfig.enabled;
        }
        if (pRespMsg->mRobustLoationConfig.validMask &
                GNSS_CONFIG_ROBUST_LOCATION_ENABLED_FOR_E911_VALID_BIT) {
            validMask |= ROBUST_LOCATION_CONFIG_VALID_ENABLED_FOR_E911;
            robustConfig.enabledForE911 = pRespMsg->mRobustLoationConfig.enabledForE911;
        }
        if (pRespMsg->mRobustLoationConfig.validMask &
                GNSS_CONFIG_ROBUST_LOCATION_VERSION_VALID_BIT) {
            validMask |= ROBUST_LOCATION_CONFIG_VALID_VERSION;
            robustConfig.version.major = pRespMsg->mRobustLoationConfig.version.major;
            robustConfig.version.minor = pRespMsg->mRobustLoationConfig.version.minor;
        }

        robustConfig.validMask = (RobustLocationConfigValidMask) validMask;
        mIntegrationCbs.getRobustLocationConfigCb(robustConfig);
    }
}

void LocationIntegrationApiImpl::processGetMinGpsWeekRespCb(
        const LocConfigGetMinGpsWeekRespMsg* pRespMsg) {

    LOC_LOGd("<<< response message id: %d, min gps week: %d",
             pRespMsg->msgId, pRespMsg->mMinGpsWeek);
    if (mIntegrationCbs.getMinGpsWeekCb) {
        mIntegrationCbs.getMinGpsWeekCb(pRespMsg->mMinGpsWeek);
    }
}

void LocationIntegrationApiImpl::processGetMinSvElevationRespCb(
        const LocConfigGetMinSvElevationRespMsg* pRespMsg) {

    LOC_LOGd("<<< response message id: %d, min sv elevation: %d",
             pRespMsg->msgId, pRespMsg->mMinSvElevation);
    if (mIntegrationCbs.getMinSvElevationCb) {
        mIntegrationCbs.getMinSvElevationCb(pRespMsg->mMinSvElevation);
    }
}

void LocationIntegrationApiImpl::processGetXtraStatusRespCb(
        const LocConfigGetXtraStatusRespMsg* pRespMsg) {

    if (!mIntegrationCbs.getXtraStatusCb) {
        return;
    }

    XtraStatus xtraStatus = {};
    XtraStatusUpdateTrigger updateTrigger = (XtraStatusUpdateTrigger) 0;
    switch (pRespMsg->mUpdateType) {
    case ::XTRA_STATUS_UPDATE_UPON_QUERY:
        updateTrigger = XTRA_STATUS_UPDATE_UPON_QUERY;
        break;
    case ::XTRA_STATUS_UPDATE_UPON_REGISTRATION:
        updateTrigger = XTRA_STATUS_UPDATE_UPON_REGISTRATION;
        break;
    case ::XTRA_STATUS_UPDATE_UPON_STATUS_CHANGE:
        updateTrigger = XTRA_STATUS_UPDATE_UPON_STATUS_CHANGE;
        break;
    default:
        break;
    }

    LOC_LOGi("update type %d, register for update %d, register for update pending %d",
             updateTrigger, mRegisterXtraUpdate, mXtraUpdateUponRegisterPending);
    if (updateTrigger == XTRA_STATUS_UPDATE_UPON_REGISTRATION) {
        if ((mRegisterXtraUpdate == false) ||
            (mXtraUpdateUponRegisterPending == false)) {
            // if client has de-registered update or this is due to hal daemon restart
            return;
        }
        mXtraUpdateUponRegisterPending = false;
    }

    xtraStatus.featureEnabled = pRespMsg->mXtraStatus.featureEnabled;
    if (xtraStatus.featureEnabled == true) {
        xtraStatus.xtraDataStatus = (XtraDataStatus) XTRA_DATA_STATUS_UNKNOWN;
        switch (pRespMsg->mXtraStatus.xtraDataStatus) {
        case ::XTRA_DATA_STATUS_NOT_AVAIL:
            xtraStatus.xtraDataStatus = XTRA_DATA_STATUS_NOT_AVAIL;
            break;
        case ::XTRA_DATA_STATUS_NOT_VALID:
            xtraStatus.xtraDataStatus = XTRA_DATA_STATUS_NOT_VALID;
            break;
        case ::XTRA_DATA_STATUS_VALID:
            xtraStatus.xtraDataStatus = XTRA_DATA_STATUS_VALID;
            break;
        }

        if (xtraStatus.xtraDataStatus == XTRA_DATA_STATUS_VALID) {
            xtraStatus.xtraValidForHours = pRespMsg->mXtraStatus.xtraValidForHours;
        }
    }

    LOC_LOGd("send out xtra status: %d %d %d %d", updateTrigger, xtraStatus.featureEnabled,
             xtraStatus.xtraDataStatus, xtraStatus.xtraValidForHours);
    mIntegrationCbs.getXtraStatusCb(updateTrigger, xtraStatus);
}

/******************************************************************************
LocationIntegrationApiImpl - Not implemented ILocationControlAPI functions
******************************************************************************/
uint32_t* LocationIntegrationApiImpl::gnssUpdateConfig(const GnssConfig& config) {
    return nullptr;
}

} // namespace location_integration
