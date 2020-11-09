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
#define LOG_TAG "LocSvc_LocNetExtIfaceLE"

#include "LocNetExtIface.h"
#include "LocNetIfaceGlue.h"
#include <platform_lib_log_util.h>
#include <platform_lib_macros.h>
#include <unistd.h>

LocNetExtIface *LocNetExtIface::mInstance = nullptr;

LocNetExtIface *LocNetExtIface::getInstance()
{
    if (nullptr == mInstance) {
        mInstance = new LocNetExtIface();
    }
    return mInstance;
}

LocNetExtIface::LocNetExtIface() :
        mMsgTask(new MsgTask("LocNetExtIface", false)), mStatusCb(nullptr)
{
    ENTRY_LOG();
    // initialization
}

LocNetExtIface::~LocNetExtIface()
{
    ENTRY_LOG();
    // de-initialize connection manager object
}

bool LocNetExtIface::init(StatusCb statusCb, void* userHandler)
{
    ENTRY_LOG();
    mStatusCb = statusCb;
    LocNetExtMsgInitNetworkManager *initMsg = new LocNetExtMsgInitNetworkManager(userHandler);
    queueLocNetExtMsg(initMsg);
    return true;
}

bool LocNetExtIface::connectBackhaul()
{
    ENTRY_LOG();
    LocNetExtMsgConnectBackhaul *connectMsg = new LocNetExtMsgConnectBackhaul();
    queueLocNetExtMsg(connectMsg);
    return true;
}

bool LocNetExtIface::disconnectBackhaul()
{
    ENTRY_LOG();
    LocNetExtMsgDisconnectBackhaul *disConnectMsg = new LocNetExtMsgDisconnectBackhaul();
    queueLocNetExtMsg(disConnectMsg);
    return true;
}

void LocNetExtIface::handleStatusCb(void* userDataPtr, LocNetStatusType status, const char* ip)
{
    ENTRY_LOG();
    LOC_LOGD("%s: userDataPtr %p, status: %d ip: %s", __func__,
            userDataPtr, status, ip ? ip: "NULL");
    StatusResp respInfo = {};
    respInfo.userHandler = userDataPtr;
    respInfo.status = status;
    respInfo.ip = ip;
    LocNetExtMsgRespCb *respMsg = new LocNetExtMsgRespCb(respInfo);
    LocNetExtIface::getInstance()->queueLocNetExtMsg(respMsg);
}

void LocNetExtIface::deleteInstance()
{
    LocNetIfaceGlue::deleteInstance();
    if (nullptr != mInstance) {
        delete mInstance;
        mInstance = nullptr;
    }
}

void LocNetExtIface::queueLocNetExtMsg(const LocNetExtMsgHdr* msg)
{
    struct LocNetExtMsgType : public LocMsg {
        LocNetExtIface* mLocNetMgr;
        const LocNetExtMsgHdr* mMsg;
        inline LocNetExtMsgType(LocNetExtIface* locNetMgr,
                            const LocNetExtMsgHdr* msg) :
            LocMsg(),
            mLocNetMgr(locNetMgr),
            mMsg(msg) {};
        inline virtual void proc() const {
            mLocNetMgr->processLocNetExtMsg(mMsg);
            delete mMsg;
        }
    };
    mMsgTask->sendMsg(new LocNetExtMsgType(this, msg));
}

void LocNetExtIface::processLocNetExtMsg(const LocNetExtMsgHdr* msg)
{
    if (nullptr == msg) {
        LOC_LOGE("msg is NULL!!");
        return;
    }

    switch(msg->msgId) {
    case LOC_NET_MSG_INIT_NETWORK_MANAGER:
    {
        LOC_LOGD("LOC_NET_MSG_INIT_NETWORK_MANAGER");
        const LocNetExtMsgInitNetworkManager *initMsg = (const LocNetExtMsgInitNetworkManager *)msg;
        //Create Network Manager (NM) Object
        //Call NM init api
        // nm->init(initMsg->mUserHandler, handleStatusCb);
        LocNetIfaceGlue::getInstance()->init(LocNetExtIface::handleStatusCb, this);
        break;
    }
    case LOC_NET_MSG_CONNECT_BACKHAUL:
    {
        LOC_LOGD("LOC_NET_MSG_CONNECT_BACKHAUL");
        //Call NM connectBackhaul api
        //nm->connectBackhaul();
        LocNetIfaceGlue::getInstance()->connectBackhaul();
        break;
    }
    case LOC_NET_MSG_DISCONNECT_BACKHAUL:
    {
        LOC_LOGD("LOC_NET_MSG_DISCONNECT_BACKHAUL");
        //Call NM disconnectBackhaul api
        //nm->disconnectBackhaul();
        LocNetIfaceGlue::getInstance()->disconnectBackhaul();
        break;
    }
    case LOC_NET_MSG_STATUS_RESP_CB:
    {
        LOC_LOGD("LOC_NET_MSG_STATUS_RESP_CB");
        const LocNetExtMsgRespCb *respMsg = (const LocNetExtMsgRespCb *)msg;
        // Call Loc net iface status response callback function
        if (mStatusCb) {
            mStatusCb(respMsg->mResp.userHandler, respMsg->mResp.status, respMsg->mResp.ip.c_str());
        }
        break;
    }
    default:
        break;
    }
}
