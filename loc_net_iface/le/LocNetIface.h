/* Copyright (c) 2017, 2021 The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation, nor the names of its
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
#ifndef LOC_NET_IFACE_H
#define LOC_NET_IFACE_H

#include <LocNetIfaceBase.h>
#include <dsi_netctrl.h>
#include <QCMAP_Client.h>
#include <mutex>
#include <unordered_set>

using namespace std;

// Storing details of backhaul client requests
typedef unordered_set<string> ClientBackhaulRequest;

//Timeout to wait for wds service notification from qmi
#define DS_CLIENT_SERVICE_TIMEOUT (4000)
//Max timeout for the service to come up
#define DS_CLIENT_SERVICE_TIMEOUT_TOTAL (40000)
//Timeout for the service to respond to sync msg
#define DS_CLIENT_SYNC_MSG_TIMEOUT (5000)
/*Request messages the WDS client can send to the WDS service*/
typedef union {
    /*Requests the service for a list of all profiles present*/
    wds_get_profile_list_req_msg_v01 *p_get_profile_list_req;
    /*Requests the service for a profile's settings*/
    wds_get_profile_settings_req_msg_v01 *p_get_profile_settings_req;
} ds_client_req_union_type;

/*Response indications that are sent by the WDS service*/
typedef union {
    wds_get_profile_list_resp_msg_v01 *p_get_profile_list_resp;
    wds_get_profile_settings_resp_msg_v01 *p_get_profile_setting_resp;
} ds_client_resp_union_type;

typedef enum {
    E_DS_CLIENT_SUCCESS                              = 0,
    /**< Request was successful. */

    E_DS_CLIENT_FAILURE_GENERAL                      = 1,
    /**< Failed because of a general failure. */

    E_DS_CLIENT_FAILURE_UNSUPPORTED                  = 2,
    /**< Failed because the service does not support the command. */

    E_DS_CLIENT_FAILURE_INVALID_PARAMETER            = 3,
    /**< Failed because the request contained invalid parameters. */

    E_DS_CLIENT_FAILURE_ENGINE_BUSY                  = 4,
    /**< Failed because the engine is busy. */

    E_DS_CLIENT_FAILURE_PHONE_OFFLINE                = 5,
    /**< Failed because the phone is offline. */

    E_DS_CLIENT_FAILURE_TIMEOUT                      = 6,
    /**< Failed because of a timeout. */

    E_DS_CLIENT_FAILURE_SERVICE_NOT_PRESENT          = 7,
    /**< Failed because the service is not present. */

    E_DS_CLIENT_FAILURE_SERVICE_VERSION_UNSUPPORTED  = 8,
    /**< Failed because the service version is unsupported. */

    E_DS_CLIENT_FAILURE_CLIENT_VERSION_UNSUPPORTED  =  9,
    /**< Failed because the service does not support client version. */

    E_DS_CLIENT_FAILURE_INVALID_HANDLE               = 10,
    /**< Failed because an invalid handle was specified. */

    E_DS_CLIENT_FAILURE_INTERNAL                     = 11,
    /**< Failed because of an internal error in the service. */

    E_DS_CLIENT_FAILURE_NOT_INITIALIZED              = 12,
    /**< Failed because the service has not been initialized. */

    E_DS_CLIENT_FAILURE_NOT_ENOUGH_MEMORY             = 13,
    /**< Failed because not rnough memory to do the operation.*/

    E_DS_CLIENT_SERVICE_ALREADY_STARTED               = 14,
    /*Service is already started*/

    E_DS_CLIENT_DATA_CALL_CONNECTED                   = 15,

    E_DS_CLIENT_DATA_CALL_DISCONNECTED                = 16,

    E_DS_CLIENT_RETRY_LATER                           = 17
} ds_client_status_enum_type;

/*--------------------------------------------------------------------
 *  LE platform specific implementation for LocNetIface
 *-------------------------------------------------------------------*/
class LocNetIface : public LocNetIfaceBase {

public:
    /* Constructor */
    LocNetIface(LocNetConnType connType) :
        LocNetIfaceBase(connType), mQcmapClientPtr(NULL), mIsConnectReqSent(false),
        mIsConnectBackhaulPending(false), mIsDisconnectBackhaulPending(false),
        mLocNetBackHaulState(LOC_NET_CONN_STATE_INVALID),
        mLocNetBackHaulType(LOC_NET_CONN_TYPE_INVALID),
        mLocNetWlanState(LOC_NET_CONN_STATE_INVALID),
        mIsRoaming(false),
        mIsDsiInitDone(false), mDsiHandle(NULL), mIsDsiCallUp(false),
        mIsDsiStartCallPending(false), mIsDsiStopCallPending(false),
        mIsMobileApEnabled(false),
        mMutex() {}
    LocNetIface() : LocNetIface(LOC_NET_CONN_TYPE_WWAN_INTERNET) {}

    /* Override base class pure virtual methods */
    bool setupWwanCall();
    bool stopWwanCall();
    void subscribe(const std::list<DataItemId>& itemListToSubscribe);
    void unsubscribe(const std::list<DataItemId>& itemListToUnsubscribe);
    void unsubscribeAll();
    void requestData(const std::list<DataItemId>& itemListToRequestData);

    /* Setup WWAN backhaul via QCMAP
     * This sets up IP routes as well for any AP socket */
    bool connectBackhaul(const string& clientName);
    /* Disconnects the WWANbackhaul, only if it was setup by us */
    bool disconnectBackhaul(const string& clientName);

    /* APIs to fetch current WWAN status */
    bool isWwanConnected();
    /* APIs to fetch current Backhaul Network Interface status */
    bool isAnyBackHaulConnected();
    /* API to check if any non-metered backhaul type (eg: wifi, ethernet etc) status*/
    bool isNonMeteredBackHaulTypeConnected();
    /* API to check wwan roaming status */
    bool isWwanRoaming();
    qcmap_msgr_wwan_call_type_v01 getWwanCallType();

    recursive_mutex& getMutex(){ return mMutex; }

private:
    /* QCMAP client handle
     * This will be set only for static sQcmapInstance. */
    QCMAP_Client* mQcmapClientPtr;

    /* Keep track of backhaul client requests */
    ClientBackhaulRequest mClientBackhaulReq;

    /* Flag to track whether we've setup QCMAP backhaul */
    bool mIsConnectReqSent;
    bool mIsConnectBackhaulPending;
    bool mIsDisconnectBackhaulPending;

    /* Maintain an exclusive instance for QCMAP interaction.
     * QCMAP does NOT support passing in/out a void user data pointer,
     * Hence we need to track the instance used internally. */
    static LocNetIface* sLocNetIfaceInstance;

    /* Current connection status */
    LocNetConnState mLocNetBackHaulState;
    /* Current Backhaul type include wwan, wifi, BT, USB cradle, Ethernet etc*/
    LocNetConnType  mLocNetBackHaulType;
    /* Check wifi hardware state */
    LocNetConnState mLocNetWlanState;
    /* Roaming status */
    bool mIsRoaming;
    /* Keep track of whether EnableMobileAP is done */
    bool mIsMobileApEnabled;

    ds_client_status_enum_type ds_client_qmi_ctrl_point_init(qmi_client_type *p_wds_qmi_client);
    ds_client_status_enum_type ds_client_convert_qmi_response(
            uint32_t req_id,
            ds_client_resp_union_type *resp_union);
    ds_client_status_enum_type ds_client_send_qmi_sync_req(
            qmi_client_type *ds_client_handle,
            uint32_t req_id,
            ds_client_resp_union_type *resp_union,
            ds_client_req_union_type *req_union);
    ds_client_status_enum_type ds_client_get_profile_list(
            qmi_client_type *ds_client_handle,
            ds_client_resp_union_type *profile_list_resp_msg,
            wds_profile_type_enum_v01 profile_type);
    ds_client_status_enum_type ds_client_get_profile_settings(
            qmi_client_type *ds_client_handle,
            ds_client_resp_union_type *profile_settings_resp_msg,
            wds_profile_identifier_type_v01 *profile_identifier);
    ds_client_status_enum_type getEsProfileIndex(uint8_t& esProfileIndex);
    /* Private APIs to interact with QCMAP module */
    void subscribeWithQcmap();
    void unsubscribeWithQcmap();
    void handleQcmapCallback(
            qcmap_msgr_wlan_status_ind_msg_v01 &wlanStatusIndData);
    void handleQcmapCallback(
            qcmap_msgr_backhaul_status_ind_msg_v01 &backhaulStatusIndData);
    void handleQcmapCallback(
            qcmap_msgr_bring_up_wwan_ind_msg_v01 &bringUpWwanIndData);
    void handleQcmapCallback(
            qcmap_msgr_tear_down_wwan_ind_msg_v01 &teardownWwanIndData);
    void handleQcmapCallback(
            qcmap_msgr_wwan_roaming_status_ind_msg_v01 &roamingStatusIndData);
    void notifyObserverForWlanStatus(bool isWlanEnabled);
    void notifyObserverForNetworkInfo(boolean isConnected, LocNetConnType connType);
    void notifyCurrentNetworkInfo(bool queryQcmap,
            LocNetConnType connType = LOC_NET_CONN_TYPE_INVALID);
    void notifyCurrentWifiHardwareState(bool queryQcmap);
    void setCurrentBackHaulStatus(qcmap_msgr_backhaul_type_enum_v01  backhaulType,
            boolean backhaulIPv4Available, boolean backhaulIPv6Available);

    /* Callback registered with QCMAP */
    static void qcmapClientCallback
    (
      qmi_client_type user_handle,   /* QMI user handle. */
      unsigned int msg_id,           /* Indicator message ID. */
      void *ind_buf,                 /* Raw indication data. */
      unsigned int ind_buf_len,      /* Raw data length. */
      void *ind_cb_data              /* User callback handle. */
    );

    /* Data call setup specific members */
    bool mIsDsiInitDone;
    dsi_hndl_t mDsiHandle;
    bool mIsDsiCallUp;
    bool mIsDsiStartCallPending;
    bool mIsDsiStopCallPending;

    /* Callback registered with DSI */
    static void dsiNetEventCallback(
            dsi_hndl_t dsiHandle, void* userDataPtr, dsi_net_evt_t event,
            dsi_evt_payload_t* eventPayloadPtr);
    void handleDSCallback(dsi_hndl_t dsiHandle, bool isNetConnected);

    /* Mutex for synchronization */
    recursive_mutex mMutex;
};

#endif /* #ifndef LOC_NET_IFACE_H */
