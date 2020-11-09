/* Copyright (c) 2020, The Linux Foundation. All rights reserved.
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
#ifndef LOC_NET_EXT_IFACE_H
#define LOC_NET_EXT_IFACE_H
#include <string>
#include "msg_q.h"
#include <MsgTask.h>

using namespace std;

/** @brief
    Response indication from StatusCb<br/>
*/
typedef enum {
    /* Connection is setup and ready for download,
      * unsolicited or some time after calling connectBackhaul()
      * Valid IP is expected from StatusCb
    */
    LOCNET_CONNECTED = 1,
    /* Connection is not ready or disconnected */
    LOCNET_DISCONNECTED,
    /* General Error, For anyother issues during connection/disconnection or timeout*/
    LOCNET_GENERAL_ERROR
} LocNetStatusType;

/** @brief
    StatusCb is for receiving status indications from network manager <br/>
    @param userDataPtr: userdata handler
    @param Status: status indication defined in LocNetStatusType
    @param ip: IP address
          If Status is "LOCNET_CONNECTED", valid IP address is expected.
          For other status values, IP shall be NULL<br/>
*/
typedef void (*StatusCb) (void* userDataPtr, LocNetStatusType Status, const char* ip);

typedef enum {
    LOC_NET_MSG_INIT_NETWORK_MANAGER = 1,
    LOC_NET_MSG_CONNECT_BACKHAUL,
    LOC_NET_MSG_DISCONNECT_BACKHAUL,
    LOC_NET_MSG_STATUS_RESP_CB,
} LocNetExtMsgId;

struct LocNetExtMsgHdr {
    LocNetExtMsgId msgId;
    inline LocNetExtMsgHdr(LocNetExtMsgId id): msgId(id) {}
};

// defintion for message with msg id of LOC_NET_MSG_INIT_NETWORK_MANAGER
struct LocNetExtMsgInitNetworkManager: public LocNetExtMsgHdr {
    void* mUserHandler;
    inline LocNetExtMsgInitNetworkManager(void* userHandler) :
        LocNetExtMsgHdr(LOC_NET_MSG_INIT_NETWORK_MANAGER),
        mUserHandler(userHandler){}
};

// defintion for message with msg id of LOC_NET_MSG_CONNECT_BACKHAUL
struct LocNetExtMsgConnectBackhaul: public LocNetExtMsgHdr {

    inline LocNetExtMsgConnectBackhaul() :
        LocNetExtMsgHdr(LOC_NET_MSG_CONNECT_BACKHAUL){}
};

// defintion for message with msg id of LOC_NET_MSG_DISCONNECT_BACKHAUL
struct LocNetExtMsgDisconnectBackhaul: public LocNetExtMsgHdr {

    inline LocNetExtMsgDisconnectBackhaul() :
        LocNetExtMsgHdr(LOC_NET_MSG_DISCONNECT_BACKHAUL){}
};

typedef struct {
    void* userHandler;
    LocNetStatusType status;
    string ip;
} StatusResp;

// defintion for message with msg id of LOC_NET_MSG_STATUS_RESP_CB
struct LocNetExtMsgRespCb: public LocNetExtMsgHdr {
    StatusResp mResp;
    inline LocNetExtMsgRespCb(const StatusResp &in_resp) :
        LocNetExtMsgHdr(LOC_NET_MSG_STATUS_RESP_CB),
        mResp(in_resp){}
};

class LocNetExtIface {

public:
    /** @brief
        get singleton instance for LocNetExtIface object. <br/>
    */
    static LocNetExtIface* getInstance();
    /** @brief
        Initialize network manager. <br/>
        @param statusCb: status callback function
        @param userHandler: pointer to user handler
        @return true, on success
        @return false, on failure
    */
    bool init(StatusCb statusCb, void* userHandler);

    /** @brief
        delete singleton instance for LocNetExtIface object. <br/>
    */
    void deleteInstance();

    /** @brief
        Setup backhaul  <br/>
        @return true, on success
        @return false, on failure
    */
    bool connectBackhaul();

    /** @brief
        Disconnects the backhaul <br/>
        @return true, on success
        @return false, on failure
    */
    bool disconnectBackhaul();
    /** @brief
        Handle connection status indications from connection manager<br/>
    */
    static void handleStatusCb(void* userDataPtr, LocNetStatusType status, const char* ip);
    void queueLocNetExtMsg(const LocNetExtMsgHdr* msg);
    void processLocNetExtMsg(const LocNetExtMsgHdr* msg);

private:
    static LocNetExtIface *mInstance;
    const MsgTask* mMsgTask;
    StatusCb mStatusCb;
    /** @brief
        Creates an instance of LocNetExtIface object. <br/>
    */
    LocNetExtIface();
    /** @brief
        Delete an instance of LocNetExtIface object. <br/>
    */
    ~LocNetExtIface();

};

#endif /* #ifndef LOC_NET_EXT_IFACE_H */
