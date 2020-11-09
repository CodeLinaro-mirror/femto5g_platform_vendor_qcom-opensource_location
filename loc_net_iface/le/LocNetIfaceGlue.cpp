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
#define LOG_TAG "LocSvc_LocNetIfaceGlueLE"

#include "LocNetIfaceGlue.h"
#include <platform_lib_log_util.h>
#include <platform_lib_macros.h>
#include "DataItemConcreteTypes.h"

LocNetIfaceGlue *LocNetIfaceGlue::mInstance = nullptr;

LocNetIfaceGlue *LocNetIfaceGlue::getInstance()
{
    if (nullptr == mInstance) {
        mInstance = new LocNetIfaceGlue();
    }
    return mInstance;
}

LocNetIfaceGlue::LocNetIfaceGlue() :
        mLocNetIfaceInstance(nullptr), mClientStatusCb(nullptr),
        mWwanStatusCb(nullptr)
{
    ENTRY_LOG();
    // get instance of LocNetIface
    mLocNetIfaceInstance = new LocNetIface();
}

void LocNetIfaceGlue::deleteInstance() {
    if (NULL != mInstance) {
        delete mInstance;
        mInstance = NULL;
    }
}

LocNetIfaceGlue::~LocNetIfaceGlue()
{
    ENTRY_LOG();
    // de-initialize connection manager object
}

bool LocNetIfaceGlue::init(StatusCb statusCb, void* userHandler)
{
    ENTRY_LOG();
    mClientStatusCb = statusCb;
    if (NULL != mLocNetIfaceInstance) {
        mLocNetIfaceInstance->registerWwanCallStatusCallback(LocNetIfaceGlue::wwanStatusCallback, NULL);
        std::list<DataItemId> itemListToSubscribe;
        itemListToSubscribe.push_back(NETWORKINFO_DATA_ITEM_ID);
        //Glue will subscribe with empty list with Qcmap
        mLocNetIfaceInstance->subscribe(itemListToSubscribe);
    }
    return true;
}

bool LocNetIfaceGlue::connectBackhaul()
{
    ENTRY_LOG();
    if (mLocNetIfaceInstance != NULL) {
        mLocNetIfaceInstance->connectBackhaul();
        return true;
    }
    return false;
}

bool LocNetIfaceGlue::disconnectBackhaul()
{
    ENTRY_LOG();
    if (mLocNetIfaceInstance != NULL) {
        mLocNetIfaceInstance->disconnectBackhaul();
        return true;
    }
    return false;
}

void LocNetIfaceGlue::wwanStatusCallback(void* userDataPtr, LocNetWwanCallEvent event,
            const char* apn, LocNetConnIpType apnIpType) {
    LocNetStatusType status;
    //Setting a default loopback ip here as Qcmap
    //does not return a Ip in Cb.
    string ip("127.0.0.1");

    switch (event) {
        case LOC_NET_WWAN_CALL_EVT_OPEN_SUCCESS:
            status = LOCNET_CONNECTED;
            //execute the clientstatusCb
            LocNetIfaceGlue::getInstance()->mClientStatusCb(NULL, status, ip.c_str());
            break;
        case LOC_NET_WWAN_CALL_EVT_CLOSE_SUCCESS:
            status = LOCNET_DISCONNECTED;
            //execute the clientstatusCb
            LocNetIfaceGlue::getInstance()->mClientStatusCb(NULL, status, ip.c_str());
            break;
        default:
            status = LOCNET_GENERAL_ERROR;
            LocNetIfaceGlue::getInstance()->mClientStatusCb(NULL, status, ip.c_str());
    }
}
