/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include <vector>
#include <sns_client.pb.h>
#include <sns_suid.pb.h>
#include <qsh_utils_suid_util.h>
#include <log_util.h>
#include <condition_variable>
#include <mutex>

#undef LOG_NDEBUG
#define LOG_NDEBUG 0

#undef LOG_TAG
#define LOG_TAG "QSH_LOCATION_UTILS"

void LocIsession::getSuid(std::string& sensorName) {
    static condition_variable eventCV;
    static mutex eventMutex;

    ISession::eventCallBack suidEvent = [&](const uint8_t* data, size_t size, uint64_t timeStamp) {
        sns_client_event_msg pb_event_msg;
        /* Parse the pb encoded event buffer*/
        pb_event_msg.ParseFromArray(data, size);
        LOC_LOGd("LocIsession::suidEvent");

        /* Iterate over all events that are received */
        for (int i = 0; i < pb_event_msg.events_size(); i++) {
            auto& pb_event = pb_event_msg.events(i);
            /* parse SUID event packet */
            if (pb_event.msg_id() == SNS_SUID_MSGID_SNS_SUID_EVENT) {
                sns_suid_event pb_suid_event;
                pb_suid_event.ParseFromString(pb_event.payload());
                const string& dataType = pb_suid_event.data_type();
                LOC_LOGd("Received SNS_SUID_MSGID_SNS_SUID_EVENT %s", dataType.c_str());
                printf("\nReceived SNS_SUID_MSGID_SNS_SUID_EVENT %s\n", dataType.c_str());

                /* create a list of all suids found for this sensorType */
                vector<suid> suids(pb_suid_event.suid_size());
                LOC_LOGd("Received SUIDs for %s, number of suids received = %d\n",
                        dataType.c_str(), pb_suid_event.suid_size());
                for (int j = 0; j < pb_suid_event.suid_size(); j++) {
                    LOC_LOGd("SUID received - suid_low=%" PRIu64 " suid_high=%" PRIu64,
                            pb_suid_event.suid(j).suid_low(), pb_suid_event.suid(j).suid_high());
                    suids[j] = suid(pb_suid_event.suid(j).suid_low(),
                            pb_suid_event.suid(j).suid_high());
                }
                mGetSuidCb(dataType, suids);
                break;
            }
            else {
                LOC_LOGe("Invalid event msg_id = %d", pb_event.msg_id());
            }
        }

        /* notify the waiting thread when all events are parsed */
        unique_lock<mutex> lock(eventMutex);
        eventCV.notify_one();
    };

    /*  -----------------------------------------------------------------------------------
        send suid discovery request
        -----------------------------------------------------------------------------------  */
    /* using the suid of SUID Sensor for suid discovery request */
    suid uid;
    sns_suid_sensor suid_sensor;
    uid.low = (uint64_t)suid_sensor.suid_low();
    uid.high = (uint64_t)suid_sensor.suid_high();

    /* create mSuidSession to get the availability of the qsh driver */
    unique_ptr<sessionFactory> factory = make_unique<sessionFactory>();
    if (nullptr == factory){
        LOC_LOGe("failed to create factory instance");
        return;
    }

    mSuidSession = unique_ptr<ISession>(factory->getSession());
    if (nullptr == mSuidSession){
        LOC_LOGe("failed to create session for suid discovery");
        return;
    }

    /* open the mSuidSession */
    int ret = mSuidSession->open();
    if (-1 == ret){
        LOC_LOGe("failed to open ISession for suid discovery");
        return;
    }

    /* set suidEvent call back for the mSuidSession for uid */
    ret = mSuidSession->setCallBacks(uid, nullptr, nullptr, suidEvent);
    if (0 != ret)
        LOC_LOGe("all callbacks are null, no need to register it");

    /*
     * Create SUID request message
     * (Please refer sns_client.proto and sns_suid.proto for more details)
     * */
    string pb_req_encoded = "";
    sns_suid_req pb_suid_req;
    pb_suid_req.set_data_type(sensorName);
    pb_suid_req.set_register_updates(true);
    pb_suid_req.set_default_only(true);
    pb_suid_req.SerializeToString(&pb_req_encoded);

    sns_client_request_msg pb_req_msg;
    pb_req_msg.set_msg_id(SNS_SUID_MSGID_SNS_SUID_REQ);
    pb_req_msg.mutable_request()->set_payload(pb_req_encoded);
    pb_req_msg.mutable_suid()->set_suid_high(uid.high);
    pb_req_msg.mutable_suid()->set_suid_low(uid.low);
    pb_req_msg.mutable_susp_config()->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
    pb_req_msg.mutable_susp_config()->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);

    string pb_req_msg_encoded;
    pb_req_msg.set_client_tech(SNS_TECH_LOCATION);
    pb_req_msg.SerializeToString(&pb_req_msg_encoded);

    /* send proto encoded message to sensing-hub using the mSuidSession */
    ret = mSuidSession->sendRequest(uid, pb_req_msg_encoded);
    if (0 != ret){
        LOC_LOGe("Error in sending suid discovery request");
        return;
    }

    /* wait until all suids are received */
    unique_lock<mutex> eventLock(eventMutex);
    eventCV.wait(eventLock);

    LOC_LOGw("Sensor %s suid request returned", sensorName.c_str());
}

void LocIsession::getAttributes(unique_ptr<ISession>& session, suid& uid) {
    string pb_req_msg_encoded;
    sns_client_request_msg pb_req_msg;

    LOC_LOGd();

    if (nullptr == session) {
        LOC_LOGe("session is nullptr!");
        return;
    }

    pb_req_msg.set_msg_id(SNS_STD_MSGID_SNS_STD_ATTR_REQ);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->clear_payload();
    } else {
        LOC_LOGe("mutable_request failed!");
        return;
    }
    sns_std_suid* pStdSuid = pb_req_msg.mutable_suid();
    if (pStdSuid) {
        pStdSuid->set_suid_high(uid.high);
        pStdSuid->set_suid_low(uid.low);
    } else {
        pb_req_msg.clear_request();
        LOC_LOGe("mutable_s_uid failed!");
        return;
    }
    sns_client_request_msg_suspend_config* pSuspendCfg = pb_req_msg.mutable_susp_config();
    if (pSuspendCfg) {
        pSuspendCfg->set_delivery_type(SNS_CLIENT_DELIVERY_WAKEUP);
        pSuspendCfg->set_client_proc_type(SNS_STD_CLIENT_PROCESSOR_APSS);
    } else {
        pb_req_msg.clear_request();
        pb_req_msg.clear_suid();
        LOC_LOGe("mutable_susp_config failed!");
        return;
    }

    pb_req_msg.set_client_tech(SNS_TECH_LOCATION);
    pb_req_msg.SerializeToString(&pb_req_msg_encoded);
    session->sendRequest(uid, pb_req_msg_encoded);
    pb_req_msg.clear_request();
    pb_req_msg.clear_suid();
    pb_req_msg.clear_susp_config();
}
