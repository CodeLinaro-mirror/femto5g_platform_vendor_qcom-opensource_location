/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "qsh_utils_suid_util.h"
#include "sns_std.pb.h"
#include "qsh_loc_test.pb.h"
#include "sns_client.pb.h"
#include <loc_pla.h>
#include <loc_cfg.h>
#include <log_util.h>
using namespace std;
using com::quic::sensinghub::session::V1_0::sessionFactory;

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#undef LOG_NDEBUG
#define LOG_NDEBUG 0

#undef LOG_TAG
#define LOG_TAG "QSH_SLPI_LOC_TEST"

#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

#define SENSOR_NAME "qsh_loc_test"

#define BUFFERSIZE              200
#define GNSS_MAX_GEOFENCES      30

typedef struct {
    uint32_t breach_type_mask;
    uint32_t responsiveness;
    uint32_t dwell_time;
    int32_t latitude;
    int32_t longitude;
    float radius;
} storedGeofences;

#define OPEN_REQUEST            0
#define LOCATION_START_REQUEST  1
#define LOCATION_STOP_REQUEST   2
#define DATA_START_REQUEST      3
#define DATA_STOP_REQUEST       4
#define QUIT_REQUEST            5

storedGeofences myGeofences[GNSS_MAX_GEOFENCES];
int myRequest = 0;
uint32_t myInterval = 0;
uint32_t myCount = 0;

static unique_ptr<ISession> _qmiSession;
static suid mySuid;
static bool isInitialized = false;
static void suid_cb(const std::string& datatype, const std::vector<suid>& suids);
static LocIsession lookup(suid_cb);

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/**
 * Send an loc_test config request for the specified SUID.
 */
static void send_config_req(unique_ptr<ISession>& session, suid s_uid)
{
    string pb_req_msg_encoded;
    string loc_test_config_encoded;
    sns_client_request_msg pb_req_msg;
    qsh_loc_test_config loc_test_config;
    qsh_loc_test_geofence_specification* loc_test_spec[myCount];

    LOC_LOGd("Send config request, myInterval: %d ", myInterval);
    if (nullptr == session) {
        LOC_LOGe("session is nullptr!");
        return;
    }

    loc_test_config.set_loc_test_request((qsh_loc_test_request)myRequest);
    loc_test_config.set_interval(myInterval);

    for (uint32_t i = 0; i < myCount; i++) {
        loc_test_spec[i] = loc_test_config.add_specs();
        loc_test_spec[i]->clear_id();
        loc_test_spec[i]->set_breach_type_mask(myGeofences[i].breach_type_mask);
        loc_test_spec[i]->set_responsiveness(myGeofences[i].responsiveness);
        loc_test_spec[i]->set_dwell_time(myGeofences[i].dwell_time);
        loc_test_spec[i]->set_latitude(myGeofences[i].latitude);
        loc_test_spec[i]->set_longitude(myGeofences[i].longitude);
        loc_test_spec[i]->set_radius(myGeofences[i].radius);
    }

    loc_test_config.SerializeToString(&loc_test_config_encoded);

    pb_req_msg.set_msg_id(QSH_LOC_TEST_MSGID_QSH_LOC_TEST_CONFIG);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->set_payload(loc_test_config_encoded);
    } else {
        LOC_LOGe("mutable_request failed!");
        return;
    }
    sns_std_suid* pStdSuid = pb_req_msg.mutable_suid();
    if (pStdSuid) {
        pStdSuid->set_suid_high(s_uid.high);
        pStdSuid->set_suid_low(s_uid.low);
    } else {
        pb_req_msg.clear_request();
        LOC_LOGe("mutable_suid failed!");
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
    session->sendRequest(s_uid, pb_req_msg_encoded);
    pb_req_msg.clear_request();
    pb_req_msg.clear_suid();
    pb_req_msg.clear_susp_config();
}

/**
* Event callback function, as registered with ssc_interface.
*/
static void event_cb(const uint8_t *data, size_t size, uint64_t ts) {
    sns_client_event_msg pb_event_msg;
    UNUSED_VAR(ts);

    LOC_LOGd("Received QMI indication with length %zu", size);

    if (nullptr == data) {
        LOC_LOGe("data is nullptr!");
        return;
    }

    pb_event_msg.ParseFromArray(data, size);
    for (int i = 0; i < pb_event_msg.events_size(); i++) {
        const sns_client_event_msg_sns_client_event &pb_event = pb_event_msg.events(i);
        LOC_LOGd("Event[%i] msg_id=%i, ts=%llu", i, pb_event.msg_id(),
                 (unsigned long long)pb_event.timestamp());

        if (SNS_STD_MSGID_SNS_STD_ERROR_EVENT == pb_event.msg_id()) {
            sns_std_error_event error;
            error.ParseFromString(pb_event.payload());

            LOC_LOGe("Received error event %i", error.error());
        }
        else if (SNS_STD_MSGID_SNS_STD_ATTR_EVENT == pb_event.msg_id()) {
            sns_std_attr_event attr_event;
            int32_t attr_id;

            attr_event.ParseFromString(pb_event.payload());
            attr_id = attr_event.attributes(0).attr_id();
            LOC_LOGd("Received attr_event attr_id %d", attr_id);
            printf("Received attr_event attr_id %d", attr_id);
            send_config_req(_qmiSession, mySuid);
        }
        else {
            LOC_LOGe("Received unknown message ID %i", pb_event.msg_id());
        }
    }
}


/**
* SUID callback as registered with suid_lookup.
*/
static void suid_cb(const std::string& datatype, const std::vector<suid>& suids) {
    LOC_LOGd("Received SUID event with length %zu", suids.size());
    if (suids.size() > 0) {
        mySuid = suids[0];
        /* create a new ISession for attribute query */
        unique_ptr<sessionFactory> factory = make_unique<sessionFactory>();
        if (nullptr == factory){
            printf("failed to create factory instance");
            exit(-1);
        }

        _qmiSession = unique_ptr<ISession>(factory->getSession());
        if (nullptr == _qmiSession){
            printf("failed to create session for attribute query");
            exit(-1);
        }

        /* open the _qmiSession session */
        int ret = _qmiSession->open();
        if (-1 == ret){
            printf("failed to open ISession for attribute query");
            return;
        }

        /* set callbacks for the session for 'uid' */
        ret = _qmiSession->setCallBacks(mySuid, nullptr, nullptr, event_cb);
        if (0 == ret) {
            printf("\nrequesting attributes for - suid_low=%" PRIu64 " suid_high=%" PRIu64 "\n",
                    mySuid.low, mySuid.high);
            lookup.getAttributes(_qmiSession, mySuid);
        }

    } else {
        LOC_LOGe("%s sensor is not available", SENSOR_NAME);
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    UNUSED_VAR(argc);
    UNUSED_VAR(argv);

    int ret = -1;
    int ret2 = -1;
    char buf[BUFFERSIZE], *gfStr;
    char* tmp = NULL;
    char* substr;
    uint32_t i = 0;
    uint32_t k = 0;
    FILE *gf_fp = NULL;
    UTIL_READ_CONF_DEFAULT(LOC_PATH_GPS_CONF);

    LOC_LOGd("Begin Location Test threadid = %ld", pthread_self());
    memset(myGeofences, 0, sizeof(myGeofences));
    k = 0;

    if ((gf_fp = fopen("/vendor/etc/loc_test.txt", "r")) != NULL) {
        while (1) {
            gfStr = fgets(buf, BUFFERSIZE, gf_fp);
            if (NULL == gfStr) {
                break;
            } else if ('#' != gfStr[0]) {
                if (0 == myRequest) {
                    i = 0;
                    for (substr = strtok_r(gfStr, " ", &tmp);
                         substr != NULL;
                         substr = strtok_r(NULL, " ", &tmp)) {
                        if (0 == strncmp(substr, "QSH_LOC_TEST_POSITION_MEAS_AND_CLK",
                                         strlen("QSH_LOC_TEST_POSITION_MEAS_AND_CLK"))) {
                            myRequest = QSH_LOC_TEST_POSITION_MEAS_AND_CLK;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_MEAS_AND_CLK",
                                         strlen("QSH_LOC_TEST_MEAS_AND_CLK"))) {
                            myRequest = QSH_LOC_TEST_MEAS_AND_CLK;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_POSITION",
                                         strlen("QSH_LOC_TEST_POSITION"))) {
                            myRequest = QSH_LOC_TEST_POSITION;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_GEOFENCE",
                                        strlen("QSH_LOC_TEST_GEOFENCE"))) {
                            myRequest = QSH_LOC_TEST_GEOFENCE;
                            break;
                        }
                    }
                    continue;
                } else if (QSH_LOC_TEST_GEOFENCE != myRequest) {
                    // get the interval
                    substr = strtok_r(gfStr, " ", &tmp);
                    if (NULL != substr) {
                        myInterval = atoi(substr);
                    } else {
                        myInterval = 0;
                    }
                    break;
                } else {
                    i = 0;
                    for (substr = strtok_r(gfStr, " ", &tmp);
                         substr != NULL;
                         substr = strtok_r(NULL, " ", &tmp)) {
                        switch (i) {
                            case 0:
                                myGeofences[k].breach_type_mask = atoi(substr);
                                break;
                            case 1:
                                myGeofences[k].responsiveness = atoi(substr);
                                break;
                            case 2:
                                myGeofences[k].dwell_time = atoi(substr);
                                break;
                            case 3:
                                myGeofences[k].latitude = (int32_t)(atof(substr)*1e7);
                                break;
                            case 4:
                                myGeofences[k].longitude = (int32_t)(atof(substr)*1e7);
                                break;
                            case 5:
                                myGeofences[k].radius = atof(substr);
                                break;
                            default:
                                break;
                        }
                        i++;
                        if (6 == i) {
                            break;
                        }
                    }
                    if (i) {
                        k++;
                    }
                    if (GNSS_MAX_GEOFENCES == k) {
                        break;
                    }
                }
            }
        }
        myCount = k;
        fclose(gf_fp);
    } else {
        LOC_LOGe("Cannot open loc_test.txt for read!");
        printf("Cannot open loc_test.txt for read!\n");
        return -1;
    }
    std::string sensorName = SENSOR_NAME;
    lookup.getSuid(sensorName);
    printf("myRequest = %d myInterval = %d myCount = %d\n",
           myRequest, myInterval, myCount);
    if (0 == myRequest) {
        printf("Error (must select and option), please re-configure!\n");
        exit(0);
    }
    if (QSH_LOC_TEST_GEOFENCE != myRequest && 0 == myInterval) {
        printf("Error (interval must be non zero), please re-configure!\n");
        exit(0);
    }
    if (QSH_LOC_TEST_GEOFENCE == myRequest && 0 == myCount) {
        printf("Error (must have at least one geofence), please re-configure!\n");
        exit(0);
    }
    while (true) {
        sleep(1);
    }

    return ret;
}
