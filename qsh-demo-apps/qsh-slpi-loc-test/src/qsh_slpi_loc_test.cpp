/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "qsh_utils_suid_util.h"
#include <atomic>
#include <string.h>
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
#define GNSS_MAX_CLIENTS        2

typedef struct {
    uint32_t breach_type_mask;
    uint32_t responsiveness;
    uint32_t dwell_time;
    int32_t latitude;
    int32_t longitude;
    float radius;
    bool has_id;
    uint32_t id;
} storedGeofences;

typedef struct {
    uint32_t sessionMode;
    uint32_t pdrMode;
    uint32_t maxMeasCount;
    uint32_t interval;
} storedClientParams;

#define OPEN_REQUEST            0
#define LOCATION_START_REQUEST  1
#define LOCATION_STOP_REQUEST   2
#define DATA_START_REQUEST      3
#define DATA_STOP_REQUEST       4
#define QUIT_REQUEST            5

storedGeofences myGeofences[GNSS_MAX_GEOFENCES];
storedClientParams myClientParams[GNSS_MAX_CLIENTS];
int myRequest = 0;
uint32_t myInterval = 0;
uint32_t myCount = 0;
uint32_t myTimeout = 0;
uint32_t myDuration = 0;
uint32_t myIterations = 0;
uint32_t mySessionMode = 0;
uint32_t myPdrMode = 0;
uint32_t myMaxMeasCount = 0;
uint32_t myClientCount = 0;

static unique_ptr<ISession> _qmiSession;
static suid mySuid;
static bool isInitialized = false;
static std::atomic<bool> test_completed(false);
static std::atomic<bool> test_passed(false);
static bool config_sent = false;
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
    qsh_loc_test_client_params* loc_test_client_param[myClientCount];

    LOC_LOGd("Send config request, myRequest: %d, myInterval: %d, myCount: %d, myClientCount: %d",
             myRequest, myInterval, myCount, myClientCount);
    if (nullptr == session) {
        LOC_LOGe("session is nullptr!");
        return;
    }

    loc_test_config.set_loc_test_request((qsh_loc_test_request)myRequest);
    loc_test_config.set_interval(myInterval);

    for (uint32_t i = 0; i < myCount; i++) {
        loc_test_spec[i] = loc_test_config.add_specs();
        if (myGeofences[i].has_id) {
            loc_test_spec[i]->set_id(myGeofences[i].id);
        } else {
            loc_test_spec[i]->clear_id();
        }
        loc_test_spec[i]->set_breach_type_mask(myGeofences[i].breach_type_mask);
        loc_test_spec[i]->set_responsiveness(myGeofences[i].responsiveness);
        loc_test_spec[i]->set_dwell_time(myGeofences[i].dwell_time);
        loc_test_spec[i]->set_latitude(myGeofences[i].latitude);
        loc_test_spec[i]->set_longitude(myGeofences[i].longitude);
        loc_test_spec[i]->set_radius(myGeofences[i].radius);
    }

    if (mySessionMode > 0) {
        loc_test_config.set_sessionmode(mySessionMode);
    }
    if (myPdrMode > 0) {
        loc_test_config.set_pdrmode(myPdrMode);
    }
    if (myMaxMeasCount > 0) {
        loc_test_config.set_maxmeascount(myMaxMeasCount);
    }
    if (myTimeout > 0) {
        loc_test_config.set_timeout(myTimeout);
    }
    if (myDuration > 0) {
        loc_test_config.set_duration(myDuration);
    }
    if (myIterations > 0) {
        loc_test_config.set_iterations(myIterations);
    }

    for (uint32_t i = 0; i < myClientCount; i++) {
        loc_test_client_param[i] = loc_test_config.add_client_params();
        if (myClientParams[i].sessionMode > 0) {
            loc_test_client_param[i]->set_sessionmode(myClientParams[i].sessionMode);
        }
        if (myClientParams[i].pdrMode > 0) {
            loc_test_client_param[i]->set_pdrmode(myClientParams[i].pdrMode);
        }
        if (myClientParams[i].maxMeasCount > 0) {
            loc_test_client_param[i]->set_maxmeascount(myClientParams[i].maxMeasCount);
        }
        if (myClientParams[i].interval > 0) {
            loc_test_client_param[i]->set_interval(myClientParams[i].interval);
        }
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
            printf("Received attr_event attr_id %d\n", attr_id);
            /* Only send config once per SUID, to avoid double-triggering. */
            if (!config_sent) {
                config_sent = true;
                send_config_req(_qmiSession, mySuid);
            } else {
                LOC_LOGd("config already sent, ignoring duplicate attr_event");
            }
        }
        else if (QSH_LOC_TEST_MSGID_QSH_LOC_TEST_ACK == pb_event.msg_id()) {
            qsh_loc_test_ack ack;
            if (!ack.ParseFromString(pb_event.payload()) || !ack.has_error()) {
                LOC_LOGe("Malformed QSH_LOC_TEST_ACK payload, treating as failure");
                printf("Malformed QSH_LOC_TEST_ACK payload\n");
                test_passed = false;
                test_completed = true;
                continue;
            }

            LOC_LOGd("Received QSH_LOC_TEST_ACK: error=%d", ack.error());
            if (ack.has_command()) {
                LOC_LOGd("  command=%d", ack.command());
            }
            if (ack.ids_size() > 0) {
                LOC_LOGd("  geofence ids count=%d", ack.ids_size());
                for (int j = 0; j < ack.ids_size(); j++) {
                    LOC_LOGd("    id[%d]=%d", j, ack.ids(j));
                }
            }

            /* Ignore ACKs for a different command (stale/spurious). */
            if (ack.has_command() && (int)ack.command() != myRequest) {
                LOC_LOGd("Ignoring ACK for command=%d (waiting on %d)",
                         ack.command(), myRequest);
            } else {
                printf("Received QSH_LOC_TEST_ACK: error=%d\n", ack.error());
                test_passed = !ack.error();
                test_completed = true;
            }
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
            LOC_LOGe("ISession open failed");
            _qmiSession->close();
            _qmiSession.reset();
            return;
        }

        /* set callbacks for the session for 'uid' */
        ret = _qmiSession->setCallBacks(mySuid, nullptr, nullptr, event_cb);
        if (0 != ret) {
            printf("failed to set callbacks for ISession");
            LOC_LOGe("setCallBacks failed: %d", ret);
            _qmiSession->close();
            _qmiSession.reset();
            return;
        }
        printf("\nrequesting attributes for - suid_low=%" PRIu64 " suid_high=%" PRIu64 "\n",
                mySuid.low, mySuid.high);
        lookup.getAttributes(_qmiSession, mySuid);

    } else {
        LOC_LOGe("%s sensor is not available", SENSOR_NAME);
        exit(-1);
    }
}

/**
 * Closes/releases the session and destroys lookup before exiting.
 * Use this instead of a bare exit()/return once past lookup.getSuid().
 */
static void cleanup_and_exit(int code)
{
    if (_qmiSession) {
        _qmiSession->close();
        _qmiSession.reset();
    }
    lookup.~LocIsession();
    exit(code);
}

/**
 * Maps a test name string to its qsh_loc_test_request value, or 0 if unknown.
 */
static int test_name_to_request(const char *name)
{
    if (0 == strcmp(name, "QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF"))
        return QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF"))
        return QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF"))
        return QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF"))
        return QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF"))
        return QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF"))
        return QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF"))
        return QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF"))
        return QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF"))
        return QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF"))
        return QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF"))
        return QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF"))
        return QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF"))
        return QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF"))
        return QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF"))
        return QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF"))
        return QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF"))
        return QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF"))
        return QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF"))
        return QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF"))
        return QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF"))
        return QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF"))
        return QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF"))
        return QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF"))
        return QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF"))
        return QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF"))
        return QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF"))
        return QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_TC8_POSITION_CLIENT_REF"))
        return QSH_LOC_TEST_TC8_POSITION_CLIENT_REF;
    if (0 == strcmp(name, "QSH_LOC_TEST_POSITION_MEAS_AND_CLK"))
        return QSH_LOC_TEST_POSITION_MEAS_AND_CLK;
    if (0 == strcmp(name, "QSH_LOC_TEST_MEAS_AND_CLK"))
        return QSH_LOC_TEST_MEAS_AND_CLK;
    if (0 == strcmp(name, "QSH_LOC_TEST_POSITION"))
        return QSH_LOC_TEST_POSITION;
    if (0 == strcmp(name, "QSH_LOC_TEST_GEOFENCE"))
        return QSH_LOC_TEST_GEOFENCE;
    return 0;
}

/**
 * Parses one --geofence "breach,responsiveness,dwell,lat,lon,radius[,id]"
 * value into myGeofences[k]. The 7th field (id) is optional. Returns true
 * on success.
 */
static bool parse_cli_geofence(const char *value, uint32_t k)
{
    char copy[BUFFERSIZE];
    char *tmp = NULL;
    char *substr;
    uint32_t i = 0;

    strlcpy(copy, value, sizeof(copy));

    myGeofences[k].has_id = false;
    myGeofences[k].id = 0;

    for (substr = strtok_r(copy, ",", &tmp);
         substr != NULL && i < 7;
         substr = strtok_r(NULL, ",", &tmp)) {
        switch (i) {
            case 0: myGeofences[k].breach_type_mask = atoi(substr); break;
            case 1: myGeofences[k].responsiveness = atoi(substr); break;
            case 2: myGeofences[k].dwell_time = atoi(substr); break;
            case 3: myGeofences[k].latitude = (int32_t)(atof(substr)*1e7); break;
            case 4: myGeofences[k].longitude = (int32_t)(atof(substr)*1e7); break;
            case 5: myGeofences[k].radius = atof(substr); break;
            case 6: myGeofences[k].has_id = true; myGeofences[k].id = (uint32_t)atoi(substr); break;
            default: break;
        }
        i++;
    }
    return (6 == i || 7 == i);
}

/**
 * Parses one --client "sessionMode,pdrMode,maxMeasCount,interval" value
 * into myClientParams[client_idx] (TC1 only). Returns true on success.
 */
static bool parse_cli_client_param(const char *value, uint32_t client_idx)
{
    char copy[BUFFERSIZE];
    char *tmp = NULL;
    char *substr;
    uint32_t i = 0;

    strlcpy(copy, value, sizeof(copy));

    for (substr = strtok_r(copy, ",", &tmp);
         substr != NULL && i < 4;
         substr = strtok_r(NULL, ",", &tmp)) {
        switch (i) {
            case 0: myClientParams[client_idx].sessionMode = atoi(substr); break;
            case 1: myClientParams[client_idx].pdrMode = atoi(substr); break;
            case 2: myClientParams[client_idx].maxMeasCount = atoi(substr); break;
            case 3: myClientParams[client_idx].interval = atoi(substr); break;
            default: break;
        }
        i++;
    }
    return (4 == i);
}

/**
 * Parses CLI-mode arguments into the same module-level variables file mode
 * populates. Returns false on a parse error (message already printed).
 */
static bool parse_cli_args(int argc, char *argv[])
{
    uint32_t k = 0;
    uint32_t client_idx = 0;

    for (int argi = 1; argi < argc; argi++) {
        if (0 == strcmp(argv[argi], "--test") && argi + 1 < argc) {
            myRequest = test_name_to_request(argv[++argi]);
            if (0 == myRequest) {
                printf("CLI: unknown --test value '%s'\n", argv[argi]);
                return false;
            }
        } else if (0 == strcmp(argv[argi], "--interval") && argi + 1 < argc) {
            myInterval = atoi(argv[++argi]);
        } else if (0 == strcmp(argv[argi], "--timeout") && argi + 1 < argc) {
            myTimeout = atoi(argv[++argi]);
        } else if (0 == strcmp(argv[argi], "--duration") && argi + 1 < argc) {
            myDuration = atoi(argv[++argi]);
        } else if (0 == strcmp(argv[argi], "--iterations") && argi + 1 < argc) {
            myIterations = atoi(argv[++argi]);
        } else if (0 == strcmp(argv[argi], "--sessionMode") && argi + 1 < argc) {
            mySessionMode = atoi(argv[++argi]);
        } else if (0 == strcmp(argv[argi], "--pdrMode") && argi + 1 < argc) {
            myPdrMode = atoi(argv[++argi]);
        } else if (0 == strcmp(argv[argi], "--maxMeasCount") && argi + 1 < argc) {
            myMaxMeasCount = atoi(argv[++argi]);
        } else if (0 == strcmp(argv[argi], "--geofence") && argi + 1 < argc) {
            if (k >= GNSS_MAX_GEOFENCES) {
                printf("CLI: max %d geofences exceeded, ignoring extra --geofence\n",
                       GNSS_MAX_GEOFENCES);
            } else if (!parse_cli_geofence(argv[++argi], k)) {
                printf("CLI: malformed --geofence value '%s' "
                       "(need breach,responsiveness,dwell,lat,lon,radius)\n", argv[argi]);
                return false;
            } else {
                k++;
            }
        } else if (0 == strcmp(argv[argi], "--client") && argi + 1 < argc) {
            if (client_idx >= GNSS_MAX_CLIENTS) {
                printf("CLI: max %d --client entries exceeded\n", GNSS_MAX_CLIENTS);
            } else if (!parse_cli_client_param(argv[++argi], client_idx)) {
                printf("CLI: malformed --client value '%s' "
                       "(need sessionMode,pdrMode,maxMeasCount,interval)\n", argv[argi]);
                return false;
            } else {
                client_idx++;
            }
        } else if (0 == strcmp(argv[argi], "--help") || 0 == strcmp(argv[argi], "-h")) {
            continue;  // already handled before this function is called
        } else {
            printf("CLI: unknown argument '%s'\n", argv[argi]);
            return false;
        }
    }

    myCount = k;
    myClientCount = client_idx;
    return true;
}

static void print_cli_usage() {
    printf("\nUsage: qsh_slpi_loc_test [--help | -h]\n"
           "       qsh_slpi_loc_test --test <NAME> [options]      (CLI mode)\n"
           "       qsh_slpi_loc_test                               (file mode, reads "
           "loc_test.txt)\n\n"
           "========================================================================\n"
           " OVERVIEW\n"
           "========================================================================\n\n"
           "Two ways to configure a test — pick whichever is more convenient:\n\n"
           "  CLI MODE   qsh_slpi_loc_test --test <NAME> [options]\n"
           "             Everything on one command line. No file to edit. Best for\n"
           "             quick manual runs and one-off checks.\n\n"
           "  FILE MODE  qsh_slpi_loc_test               (no arguments)\n"
           "             Reads exactly one test config from /vendor/etc/loc_test.txt\n"
           "             (fixed path). Best for scripted/automated regression runs\n"
           "             where the config is checked into a file.\n\n"
           "Both modes are 100%% equivalent — same 32 test cases, same parameters,\n"
           "same underlying request sent to the 'qsh_loc_test' sensor. Below is a\n"
           "full, separate example reference for EACH mode — all 32 tests, file mode\n"
           "first, then the equivalent CLI mode commands.\n\n"
           "Exit code: 0 = PASS, -1 = FAIL / TIMEOUT / config error.\n\n"

           "------------------------------------------------------------------------\n\n"

           " QUICK INDEX — all 32 test cases\n"
           "------------------------------------------------------------------------\n"
           "  BASIC (driver-only, no reference app)\n"
           "    1  QSH_LOC_TEST_POSITION\n"
           "    2  QSH_LOC_TEST_MEAS_AND_CLK\n"
           "    3  QSH_LOC_TEST_POSITION_MEAS_AND_CLK\n"
           "    4  QSH_LOC_TEST_GEOFENCE\n"
           "  SINGLE-CLIENT REFERENCE APP\n"
           "    5  QSH_LOC_TEST_TC8_POSITION_CLIENT_REF\n"
           "    6  QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF\n"
           "    7  QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF\n"
           "  MULTI-CLIENT / LIFECYCLE (TC1-TC7)\n"
           "    8  QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF\n"
           "    9  QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF\n"
           "   10  QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF\n"
           "   11  QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF\n"
           "   12  QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF\n"
           "   13  QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF\n"
           "   14  QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF\n"
           "  GEOFENCE REFERENCE APP\n"
           "   15  QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF\n"
           "   16  QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF\n"
           "   17  QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF\n"
           "   18  QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF\n"
           "   19  QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF\n"
           "   20  QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF\n"
           "   21  QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF\n"
           "  LOCATION REF-FUNCTION COVERAGE (TC18-TC23)\n"
           "   22  QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF\n"
           "   23  QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF\n"
           "   24  QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF\n"
           "   25  QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF\n"
           "   26  QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF\n"
           "   27  QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF\n"
           "  GEOFENCE REF-FUNCTION COVERAGE (TC24-TC28)\n"
           "   28  QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF\n"
           "   29  QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF\n"
           "   30  QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF\n"
           "   31  QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF\n"
           "   32  QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF\n\n"

           "------------------------------------------------------------------------\n\n"

           " CLI MODE — FLAG REFERENCE\n"
           "------------------------------------------------------------------------\n"
           "   --test <NAME>          Required. Exact name from the index above.\n"
           "   --interval <ms>        ms between fixes/measurements.\n"
           "   --timeout <sec>        max wait for result. Optional; if a test needs\n"
           "                          one and it's omitted, defaults to 60s.\n"
           "   --duration <sec>       Multi-purpose; meaning depends on --test:\n"
           "                            [5][6][7]     hold the session open this long,\n"
           "                                          collecting every fix/epoch, instead\n"
           "                                          of stopping after the first one.\n"
           "                            [20]          per-leg hold time (see test [20]).\n"
           "                            [21]          inter-step delay between ADD/MODIFY/\n"
           "                                          PAUSE/RESUME (see test [21]).\n"
           "                            [24][25]      sustained-tracking window (see\n"
           "                                          tests [24][25]).\n"
           "                          Optional in every case; each has its own built-in\n"
           "                          default when omitted. Ignored by every other test.\n"
           "   --iterations <n>       cycle count for TC2/TC3. Required > 0 for those.\n"
           "   --sessionMode <1|2|3>  1=NORMAL  2=SWIM  3=STAMINA.\n"
           "   --pdrMode <1|2>        1=CASUAL  2=FITNESS.\n"
           "   --maxMeasCount <n>     cap on measurement reports per epoch.\n"
           "   --geofence "
           "\"<breach_type>,<responsiveness_ms>,<dwell_time_s>,<lat>,<lon>,<radius_m>[,<id>]\"\n"
           "                          One geofence per occurrence. Repeat the flag\n"
           "                          for each additional geofence (up to %d). The\n"
           "                          optional 7th field (id) is required for\n"
           "                          TC12-14_REMOVE/MODIFY/PAUSE/RESUME_CLIENT_REF and\n"
           "                          TC25-27_GEOFENCE_REF_MODIFY/PAUSE/RESUME_CLIENT_REF and\n"
           "                          TC28_GEOFENCE_REF_REMOVE_CLIENT_REF — use the\n"
           "                          driver-assigned id a prior ADD's ack returned.\n"
           "                          Omit it for TC11_GEOFENCE_ADD_CLIENT_REF,\n"
           "                          TC24_GEOFENCE_REF_ADD_CLIENT_REF, and "
           "QSH_LOC_TEST_GEOFENCE.\n"
           "   --client \"<sessionMode>,<pdrMode>,<maxMeasCount>,<interval_ms>\"\n"
           "                          One concurrent client per occurrence. TC1\n"
           "                          needs exactly 2 --client flags.\n\n"

           "========================================================================\n"
           " FILE MODE — ALL 32 TEST CASES  (/vendor/etc/loc_test.txt)\n"
           "========================================================================\n"
           "   Lines starting with '#' are comments. Uncomment exactly ONE test\n"
           "   block below. Line 1 selects the test by name; the following\n"
           "   line(s) supply its parameters.\n\n"

           "------------------------------------------------------------------------\n"
           " BASIC TEST MODES  (driver-only, no reference app)\n"
           "------------------------------------------------------------------------\n\n"

           "[1] QSH_LOC_TEST_POSITION — position fixes only\n"
           "      QSH_LOC_TEST_POSITION\n"
           "      2000\n\n"

           "[2] QSH_LOC_TEST_MEAS_AND_CLK — measurements + clock data only\n"
           "      QSH_LOC_TEST_MEAS_AND_CLK\n"
           "      4000\n\n"

           "[3] QSH_LOC_TEST_POSITION_MEAS_AND_CLK — both, chained on one session\n"
           "      QSH_LOC_TEST_POSITION_MEAS_AND_CLK\n"
           "      1000\n\n"

           "[4] QSH_LOC_TEST_GEOFENCE — geofence breach detection (up to %d geofences)\n"
           "    Fields per line: breach_type responsiveness_ms dwell_time_s lat lon radius_m\n"
           "      QSH_LOC_TEST_GEOFENCE\n"
           "         5               150             2           37.3394     -121.8939       5000\n"
           "         10              200             3           37.775      -122.4183       "
           "300\n\n"

           "------------------------------------------------------------------------\n"
           " SINGLE-CLIENT REFERENCE APP TESTS  (via qsh_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n"
           "   [options] tokens (space-separated, any order, all optional):\n"
           "     timeout=<sec>  duration=<sec>  sessionMode=<1|2|3>  pdrMode=<1|2>  "
           "maxMeasCount=<n>\n\n"

           "[5] QSH_LOC_TEST_TC8_POSITION_CLIENT_REF — position fixes via reference app "
           "(one-shot: stops right after the first fix)\n"
           "      QSH_LOC_TEST_TC8_POSITION_CLIENT_REF\n"
           "      2000 timeout=30 sessionMode=1 pdrMode=1\n\n"
           "    Sustained tracking for 30s instead of one-shot:\n"
           "      QSH_LOC_TEST_TC8_POSITION_CLIENT_REF\n"
           "      1000 duration=30 sessionMode=2 pdrMode=2\n\n"

           "[6] QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF — measurements via reference app "
           "(one-shot: stops right after the first epoch)\n"
           "      QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF\n"
           "      4000 timeout=30 maxMeasCount=10\n\n"
           "    Sustained tracking for 60s instead of one-shot:\n"
           "      QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF\n"
           "      4000 duration=60 maxMeasCount=10\n\n"

           "[7] QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF — both, via reference app "
           "(one-shot: first fix, then first epoch)\n"
           "      QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF\n"
           "      1000 timeout=30 sessionMode=2 pdrMode=2\n\n"
           "    Sustained tracking for 30s per request type (~60s total session):\n"
           "      QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF\n"
           "      1000 duration=30 sessionMode=2 pdrMode=2\n\n"

           "------------------------------------------------------------------------\n"
           " MULTI-CLIENT / LIFECYCLE TESTS  (TC1-TC7, via qsh_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n\n"

           "[8] QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF — 2 concurrent clients (each holds a "
           "fixed 3s; PASS = both open+start acks succeed)\n"
           "    Requires EXACTLY 2 client lines, each: sessionMode pdrMode maxMeasCount "
           "interval_ms\n"
           "      QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF\n"
           "      1 1 0 2000\n"
           "      1 1 0 3000\n\n"

           "[9] QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF — repeated open/close cycling (no "
           "start/stop sent; PASS = every OPEN ack)\n"
           "    Requires iterations > 0.\n"
           "      QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF\n"
           "      1000 iterations=10 timeout=30\n\n"

           "[10] QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF — start/stop within one session (PASS = "
           "every ack; no event required)\n"
           "    Requires iterations > 0.\n"
           "      QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF\n"
           "      1000 iterations=5 timeout=30\n\n"

           "[11] QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF — measure first-fix latency (PASS requires"
           " an actual fix within timeout)\n"
           "      QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF\n"
           "      1000 timeout=60\n\n"

           "[12] QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF — measure first-meas-epoch latency "
           "(PASS requires a complete epoch within timeout)\n"
           "      QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF\n"
           "      1000 timeout=60\n\n"

           "[13] QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF — capabilities + version check (OPEN "
           "only; no start/stop/event)\n"
           "    No timeout used.\n"
           "      QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF\n"
           "      1000\n\n"

           "[14] QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF — switch request type mid-session "
           "(PASS = every ack; no event required)\n"
           "      QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF\n"
           "      1000 timeout=30\n\n"

           "------------------------------------------------------------------------\n"
           " GEOFENCE REFERENCE APP TESTS  (via qsh_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n"
           "   All 5 share this field layout — the 7th field (id) is optional:\n"
           "     breach_type responsiveness_ms dwell_time_s lat lon radius_m [id]\n"
           "   IMPORTANT: REMOVE/MODIFY/PAUSE/RESUME are STATEFUL — they act on a\n"
           "   geofence added earlier in the SAME test run by a prior ADD. Run ADD\n"
           "   first, note the driver-assigned id(s) printed in its ACK log, and\n"
           "   supply that id as the 7th field for the follow-up call (see\n"
           "   qsh_loc_test_ack.ids). ADD itself never sets id — the driver assigns\n"
           "   it and returns it in the ack.\n\n"

           "[15] QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF — add geofence(s); session stays open "
           "logging breaches until a later REMOVE\n"
           "    (one line per geofence, at least 1; no id field — driver assigns one)\n"
           "      QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF\n"
           "         5               150             2           37.3394     -121.8939       5000\n"
           "         10              200             3           37.775      -122.4183       "
           "300\n\n"

           "[16] QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF — remove a previously-added "
           "geofence (closes the ADD session; PASS = ack)\n"
           "    (fields besides id are ignored for REMOVE; id must be the value ADD's ack returned,"
           " e.g. 1)\n"
           "      QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF\n"
           "         5               150             2           37.3394     -121.8939       5000"
           "       1\n\n"

           "[17] QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF — change responsiveness/dwell_time "
           "(session stays open; PASS = ack)\n"
           "      QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF\n"
           "         5               300             5           37.3394     -121.8939       5000"
           "       1\n\n"

           "[18] QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF — pause breach detection (session "
           "stays open; PASS = ack)\n"
           "      QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF\n"
           "         5               150             2           37.3394     -121.8939       5000"
           "       1\n\n"

           "[19] QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF — resume breach detection (session "
           "stays open; PASS = ack)\n"
           "      QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF\n"
           "         5               150             2           37.3394     -121.8939       5000"
           "       1\n\n"

           "------------------------------------------------------------------------\n"
           " SESSION-OPTIONS / FULL-LIFECYCLE TESTS  (TC16-TC17)\n"
           "------------------------------------------------------------------------\n\n"

           "[20] QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF — chains OPEN, ack "
           "introspection, set_ind_cb, then POSITION/MEAS_AND_CLK/session-options legs, each "
           "run+tracked for duration seconds so QXDM logs are visible per leg, all in one request "
           "(PASS = all legs succeed)\n"
           "      QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF\n"
           "      4000 timeout=30 duration=15 sessionMode=2 pdrMode=2 maxMeasCount=10\n\n"

           "[21] QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF — "
           "ADD->MODIFY->PAUSE->RESUME->REMOVE chained in one self-contained run (no id "
           "carry-forward needed; PASS = all 5 steps succeed)\n"
           "    (one line per geofence, at least 1; no id field — ids stay internal to the "
           "chained call)\n"
           "    Inter-step delay is fixed at the 3s built-in default in file mode (not\n"
           "    configurable per-line here — use CLI mode's --duration to override).\n"
           "      QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF\n"
           "         5               150             2           37.3394     -121.8939       "
           "5000\n\n"

           "------------------------------------------------------------------------\n"
           " LOCATION REF-FUNCTION COVERAGE TESTS  (TC18-TC23, via "
           "qsh_location_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n"
           "   Each calls its matching qsh_location_sensor_client_ref_*() function\n"
           "   directly, forwarding sessionMode/pdrMode/maxMeasCount as given.\n"
           "   [options] tokens (space-separated, any order, all optional):\n"
           "     timeout=<sec>  duration=<sec>  sessionMode=<1|2|3>  pdrMode=<1|2>  "
           "maxMeasCount=<n>\n\n"

           "[22] QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF — one-shot position fix via "
           "qsh_location_sensor_client_ref_one_shot_position()\n"
           "      QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF\n"
           "      1000 timeout=30 sessionMode=2 pdrMode=1\n\n"

           "[23] QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF — one-shot measurement epoch "
           "via qsh_location_sensor_client_ref_one_shot_meas_and_clk()\n"
           "      QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF\n"
           "      4000 timeout=30 maxMeasCount=8\n\n"

           "[24] QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF — position fixes over a"
           " fixed window via qsh_location_sensor_client_ref_sustained_position_tracking() (PASS = "
           "at least one fix collected)\n"
           "      QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF\n"
           "      1000 timeout=30 duration=20 sessionMode=3 pdrMode=2\n\n"

           "[25] QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF — measurement epochs over a"
           " fixed window via qsh_location_sensor_client_ref_sustained_meas_tracking() (PASS = at "
           "least one epoch collected)\n"
           "      QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF\n"
           "      4000 timeout=30 duration=20 maxMeasCount=6\n\n"

           "[26] QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF — switch between POSITION "
           "and MEAS_AND_CLK requests via qsh_location_sensor_client_ref_request_switch()\n"
           "      QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF\n"
           "      1000 timeout=30 sessionMode=1 pdrMode=1 maxMeasCount=5\n\n"

           "[27] QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF — exercise "
           "sessionMode/pdrMode/maxMeasCount together via "
           "qsh_location_sensor_client_ref_session_options()\n"
           "      QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF\n"
           "      1000 timeout=30 sessionMode=2 pdrMode=2 maxMeasCount=10\n\n"

           "------------------------------------------------------------------------\n"
           " GEOFENCE REF-FUNCTION COVERAGE TESTS  (TC24-TC28, via "
           "qsh_geofence_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n"
           "   Each calls its matching qsh_geofence_sensor_client_ref_*() function\n"
           "   directly. Same field layout as TC11-15/17, own independent session\n"
           "   state — does not interfere with a TC11-15 run.\n"
           "     breach_type responsiveness_ms dwell_time_s lat lon radius_m [id]\n"
           "   IMPORTANT: REF_MODIFY/REF_PAUSE/REF_RESUME/REF_REMOVE are STATEFUL —\n"
           "   they act on a geofence added earlier in the SAME test run by a prior\n"
           "   REF_ADD. Run REF_ADD first, note the driver-assigned id(s) printed in\n"
           "   its ACK log, and supply that id as the 7th field for the follow-up call.\n\n"

           "[28] QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF — add geofence(s) via "
           "qsh_geofence_sensor_client_ref_add(); session stays open until a later REF_REMOVE\n"
           "    (one line per geofence, at least 1; no id field — driver assigns one)\n"
           "      QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF\n"
           "         5               150             2           37.3394     -121.8939       "
           "5000\n\n"

           "[29] QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF — change "
           "responsiveness/dwell_time via qsh_geofence_sensor_client_ref_modify() (session stays "
           "open; PASS = ack)\n"
           "      QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF\n"
           "         5               300             5           37.3394     -121.8939       5000"
           "       1\n\n"

           "[30] QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF — pause breach detection via "
           "qsh_geofence_sensor_client_ref_pause() (session stays open; PASS = ack)\n"
           "      QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF\n"
           "         5               150             2           37.3394     -121.8939       5000"
           "       1\n\n"

           "[31] QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF — resume breach detection via "
           "qsh_geofence_sensor_client_ref_resume() (session stays open; PASS = ack)\n"
           "      QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF\n"
           "         5               150             2           37.3394     -121.8939       5000"
           "       1\n\n"

           "[32] QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF — remove a previously-added "
           "geofence via qsh_geofence_sensor_client_ref_remove() (always closes the session; PASS ="
           " ack)\n"
           "    (fields besides id are ignored for REF_REMOVE; id must be the value REF_ADD's ack "
           "returned, e.g. 1)\n"
           "      QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF\n"
           "         5               150             2           37.3394     -121.8939       5000"
           "       1\n\n"

           "========================================================================\n"
           " CLI MODE — ALL 32 TEST CASES  (--test <NAME> [options])\n"
           "========================================================================\n"
           "   Every test below is fully expressible on one command line — no file\n"
           "   to edit. See CLI MODE FLAG REFERENCE above for what each flag means.\n\n"

           "------------------------------------------------------------------------\n"
           " BASIC TEST MODES  (driver-only, no reference app)\n"
           "------------------------------------------------------------------------\n\n"

           "[1] QSH_LOC_TEST_POSITION — position fixes only\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_POSITION --interval 2000\n\n"

           "[2] QSH_LOC_TEST_MEAS_AND_CLK — measurements + clock data only\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_MEAS_AND_CLK --interval 4000\n\n"

           "[3] QSH_LOC_TEST_POSITION_MEAS_AND_CLK — both, chained on one session\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_POSITION_MEAS_AND_CLK --interval 1000\n\n"

           "[4] QSH_LOC_TEST_GEOFENCE — geofence breach detection (repeat --geofence, up to %d)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_GEOFENCE \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000\" \\\n"
           "        --geofence \"10,200,3,37.775,-122.4183,300\"\n\n"
           "    Single geofence, minimal form:\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_GEOFENCE \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000\"\n\n"

           "------------------------------------------------------------------------\n"
           " SINGLE-CLIENT REFERENCE APP TESTS  (via qsh_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n\n"

           "[5] QSH_LOC_TEST_TC8_POSITION_CLIENT_REF — position fixes via reference app "
           "(one-shot: stops right after the first fix)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC8_POSITION_CLIENT_REF \\\n"
           "        --interval 2000 --timeout 30 --sessionMode 1 --pdrMode 1\n\n"
           "    Minimal form (timeout defaults to 60s, sessionMode/pdrMode to 1):\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC8_POSITION_CLIENT_REF --interval 2000\n\n"
           "    Sustained tracking session for 30s (PDR/SWIM mode) instead of one-shot:\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC8_POSITION_CLIENT_REF \\\n"
           "        --interval 1000 --duration 30 --sessionMode 2 --pdrMode 2\n\n"

           "[6] QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF — measurements via reference app "
           "(one-shot: stops right after the first epoch)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF \\\n"
           "        --interval 4000 --timeout 30 --maxMeasCount 10\n\n"
           "    Sustained tracking session for 60s instead of one-shot:\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF \\\n"
           "        --interval 4000 --duration 60 --maxMeasCount 10\n\n"

           "[7] QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF — both, via reference app "
           "(one-shot: first fix, then first epoch)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF \\\n"
           "        --interval 1000 --timeout 30 --sessionMode 2 --pdrMode 2\n\n"
           "    SWIM session, FITNESS PDR mode, 90s timeout:\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF \\\n"
           "        --interval 1000 --timeout 90 --sessionMode 2 --pdrMode 2\n\n"
           "    Sustained tracking, 30s per request type (~60s total session):\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF \\\n"
           "        --interval 1000 --duration 30 --sessionMode 2 --pdrMode 2\n\n"

           "------------------------------------------------------------------------\n"
           " MULTI-CLIENT / LIFECYCLE TESTS  (TC1-TC7, via qsh_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n\n"

           "[8] QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF — 2 concurrent clients, each holds a "
           "fixed 3s (exactly 2 --client flags)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF \\\n"
           "        --client \"1,1,0,2000\" --client \"1,1,0,3000\"\n\n"
           "    Different session modes per client (client 1 SWIM, client 2 STAMINA):\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF \\\n"
           "        --client \"2,1,0,1000\" --client \"3,2,5,2000\"\n\n"

           "[9] QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF — repeated open/close cycling, no "
           "start/stop sent (iterations > 0 required)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF \\\n"
           "        --interval 1000 --iterations 10 --timeout 30\n\n"

           "[10] QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF — start/stop within one session, no event"
           " required (iterations > 0 required)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF \\\n"
           "        --interval 1000 --iterations 5 --timeout 30\n\n"

           "[11] QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF — measure first-fix latency (PASS requires"
           " an actual fix within timeout)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF \\\n"
           "        --interval 1000 --timeout 60\n\n"

           "[12] QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF — measure first-meas-epoch latency "
           "(PASS requires a complete epoch within timeout)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF \\\n"
           "        --interval 1000 --timeout 60\n\n"

           "[13] QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF — capabilities + version check, OPEN "
           "only (no timeout used)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF --interval "
           "1000\n\n"

           "[14] QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF — switch request type mid-session "
           "(PASS = every ack; no event required)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF \\\n"
           "        --interval 1000 --timeout 30\n\n"

           "------------------------------------------------------------------------\n"
           " GEOFENCE REFERENCE APP TESTS  (via qsh_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n"
           "   Same stateful contract as file mode: run ADD_CLIENT_REF first, note\n"
           "   the driver-assigned id printed in its ACK log, then append it as an\n"
           "   optional 7th comma field on '--geofence' for a later REMOVE/MODIFY/\n"
           "   PAUSE/RESUME_CLIENT_REF call. ADD itself never takes an id.\n\n"

           "[15] QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF — add geofence(s); session stays open "
           "logging breaches until a later REMOVE\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000\" \\\n"
           "        --geofence \"10,200,3,37.775,-122.4183,300\"\n\n"

           "[16] QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF — remove a previously-added "
           "geofence (closes the ADD session; PASS = ack)\n"
           "    (fields besides id are ignored for REMOVE; id must be the value ADD's ack returned,"
           " e.g. 1)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000,1\"\n\n"

           "[17] QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF — change responsiveness/dwell_time "
           "(session stays open; PASS = ack)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF \\\n"
           "        --geofence \"5,300,5,37.3394,-121.8939,5000,1\"\n\n"

           "[18] QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF — pause breach detection (session "
           "stays open; PASS = ack)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000,1\"\n\n"

           "[19] QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF — resume breach detection (session "
           "stays open; PASS = ack)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000,1\"\n\n"

           "------------------------------------------------------------------------\n"
           " SESSION-OPTIONS / FULL-LIFECYCLE TESTS  (TC16-TC17)\n"
           "------------------------------------------------------------------------\n\n"

           "[20] QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF — chains OPEN, ack "
           "introspection, set_ind_cb, then POSITION/MEAS_AND_CLK/session-options legs, each "
           "run+tracked for --duration seconds so QXDM logs are visible per leg, all in one request"
           " (PASS = all legs succeed)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF "
           "\\\n"
           "        --interval 4000 --timeout 30 --duration 15 --sessionMode 2 --pdrMode 2 "
           "--maxMeasCount 10\n\n"

           "[21] QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF — "
           "ADD->MODIFY->PAUSE->RESUME->REMOVE chained in one self-contained run (no id "
           "carry-forward needed; PASS = all 5 steps succeed)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF "
           "\\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000\"\n\n"
           "    With an explicit 5s delay between each step (default is 3s) so QXDM logs are easy "
           "to follow:\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF "
           "\\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000\" --duration 5\n\n"

           "------------------------------------------------------------------------\n"
           " LOCATION REF-FUNCTION COVERAGE TESTS  (TC18-TC23, via "
           "qsh_location_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n\n"

           "[22] QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF — one-shot position fix via "
           "qsh_location_sensor_client_ref_one_shot_position()\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF \\\n"
           "        --interval 1000 --timeout 30 --sessionMode 2 --pdrMode 1\n\n"

           "[23] QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF — one-shot measurement epoch "
           "via qsh_location_sensor_client_ref_one_shot_meas_and_clk()\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF \\\n"
           "        --interval 4000 --timeout 30 --maxMeasCount 8\n\n"

           "[24] QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF — position fixes over a"
           " fixed window via qsh_location_sensor_client_ref_sustained_position_tracking() (PASS = "
           "at least one fix collected)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF"
           " \\\n"
           "        --interval 1000 --timeout 30 --duration 20 --sessionMode 3 --pdrMode 2\n\n"

           "[25] QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF — measurement epochs over a"
           " fixed window via qsh_location_sensor_client_ref_sustained_meas_tracking() (PASS = at "
           "least one epoch collected)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF "
           "\\\n"
           "        --interval 4000 --timeout 30 --duration 20 --maxMeasCount 6\n\n"

           "[26] QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF — switch between POSITION "
           "and MEAS_AND_CLK requests via qsh_location_sensor_client_ref_request_switch()\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF "
           "\\\n"
           "        --interval 1000 --timeout 30 --sessionMode 1 --pdrMode 1 --maxMeasCount 5\n\n"

           "[27] QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF — exercise "
           "sessionMode/pdrMode/maxMeasCount together via "
           "qsh_location_sensor_client_ref_session_options()\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF \\\n"
           "        --interval 1000 --timeout 30 --sessionMode 2 --pdrMode 2 --maxMeasCount 10\n\n"

           "------------------------------------------------------------------------\n"
           " GEOFENCE REF-FUNCTION COVERAGE TESTS  (TC24-TC28, via "
           "qsh_geofence_sensor_client_ref)\n"
           "------------------------------------------------------------------------\n"
           "   Same stateful contract as TC11-15: run REF_ADD first, note the\n"
           "   driver-assigned id printed in its ACK log, then append it as an\n"
           "   optional 7th comma field on '--geofence' for a later REF_MODIFY/\n"
           "   REF_PAUSE/REF_RESUME/REF_REMOVE call. REF_ADD itself never takes an id.\n\n"

           "[28] QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF — add geofence(s) via "
           "qsh_geofence_sensor_client_ref_add(); session stays open until a later REF_REMOVE\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000\"\n\n"

           "[29] QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF — change "
           "responsiveness/dwell_time via qsh_geofence_sensor_client_ref_modify() (session stays "
           "open; PASS = ack)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF \\\n"
           "        --geofence \"5,300,5,37.3394,-121.8939,5000,1\"\n\n"

           "[30] QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF — pause breach detection via "
           "qsh_geofence_sensor_client_ref_pause() (session stays open; PASS = ack)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000,1\"\n\n"

           "[31] QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF — resume breach detection via "
           "qsh_geofence_sensor_client_ref_resume() (session stays open; PASS = ack)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000,1\"\n\n"

           "[32] QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF — remove a previously-added "
           "geofence via qsh_geofence_sensor_client_ref_remove() (always closes the session; PASS ="
           " ack)\n"
           "    (fields besides id are ignored for REF_REMOVE; id must be the value REF_ADD's ack "
           "returned, e.g. 1)\n"
           "      qsh_slpi_loc_test --test QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF \\\n"
           "        --geofence \"5,150,2,37.3394,-121.8939,5000,1\"\n\n"

           "========================================================================\n"
           " PARAMETER REFERENCE\n"
           "========================================================================\n\n"
           "   interval     ms between fixes/measurements. Required for every test\n"
           "                except GEOFENCE tests and TC1/TC6 (which don't use it\n"
           "                the same way).\n"
           "   timeout      sec to wait for a result before FAIL. Optional; falls\n"
           "                back to a 60s default (with a warning) if a test that\n"
           "                needs one doesn't specify it. Not used by GEOFENCE_* or TC6.\n"
           "   duration     multi-purpose; meaning depends on the test (see --duration\n"
           "                above): sustained-tracking window for [5][6][7][24][25],\n"
           "                per-leg hold time for [20], inter-step delay for [21].\n"
           "                Optional in every case; each has its own built-in default\n"
           "                when unset.\n"
           "   iterations   cycle count for TC2/TC3. Required > 0 for those two tests.\n"
           "   sessionMode  1=NORMAL  2=SWIM  3=STAMINA  (default 1 if unset)\n"
           "   pdrMode      1=CASUAL  2=FITNESS          (default 1 if unset)\n"
           "   maxMeasCount cap on measurement reports per epoch (0 = unlimited)\n\n"
           "   Geofence breach_type values:\n"
           "      1  = ENTER              2  = EXIT\n"
           "      5  = ENTER + DWELL_IN   10 = EXIT + DWELL_OUT\n"
           "   Geofence responsiveness : ms (lower = more responsive, higher power)\n"
           "   Geofence dwell_time     : seconds\n"
           "   Geofence radius         : meters\n"
           "   Geofence lat / lon      : decimal degrees\n"
           "   Geofence id             : optional 7th field, required for REMOVE/\n"
           "                             MODIFY/PAUSE/RESUME_CLIENT_REF and\n"
           "                             GEOFENCE_REF_MODIFY/PAUSE/RESUME/REMOVE_CLIENT_REF\n"
           "                             — use the driver-assigned id a prior ADD's\n"
           "                             ack returned. Not used by ADD, REF_ADD, or\n"
           "                             QSH_LOC_TEST_GEOFENCE.\n\n"

           "========================================================================\n"
           " EXIT STATUS\n"
           "========================================================================\n\n"
           "   0    test completed and PASSED (ACK received with error=false)\n"
           "   -1   test FAILED (ACK received with error=true), config/argument\n"
           "        error, or timed out\n\n"

           "========================================================================\n"
           " RUNNING ON DEVICE\n"
           "========================================================================\n\n"
           "   CLI mode (no file edit needed):\n"
           "     adb shell qsh_slpi_loc_test --test QSH_LOC_TEST_POSITION --interval 2000\n\n"
           "   File mode:\n"
           "     1. Edit /vendor/etc/loc_test.txt on the target (uncomment one test block)\n"
           "     2. adb shell qsh_slpi_loc_test\n\n"
           "   Watch progress either way:\n"
           "     adb logcat | grep QSH_SLPI_LOC_TEST\n\n"
           "========================================================================\n\n",
           GNSS_MAX_GEOFENCES, GNSS_MAX_GEOFENCES, GNSS_MAX_GEOFENCES);
}

int main(int argc, char *argv[]) {
    bool cli_mode = false;

    for (int argi = 1; argi < argc; argi++) {
        if (0 == strcmp(argv[argi], "--help") || 0 == strcmp(argv[argi], "-h")) {
            print_cli_usage();
            return 0;
        }
        if (0 == strcmp(argv[argi], "--test")) {
            cli_mode = true;
        }
    }

    int ret = -1;
    char buf[BUFFERSIZE], *gfStr;
    char* tmp = NULL;
    char* substr;
    uint32_t i = 0;
    uint32_t k = 0;
    uint32_t client_idx = 0;
    FILE *gf_fp = NULL;
    UTIL_READ_CONF_DEFAULT(LOC_PATH_GPS_CONF);

    LOC_LOGd("Begin Location Test threadid = %ld", pthread_self());
    memset(myGeofences, 0, sizeof(myGeofences));
    memset(myClientParams, 0, sizeof(myClientParams));
    k = 0;
    client_idx = 0;

    if (cli_mode) {
        if (!parse_cli_args(argc, argv)) {
            return -1;
        }
    } else if ((gf_fp = fopen("/vendor/etc/loc_test.txt", "r")) != NULL) {
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
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr,
                                "QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr,
                                "QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr,
                            "QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF",
                            strlen("QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr,
                                "QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr,
                                "QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr,
                                "QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC7_REQUEST_SWITCH_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr,
                                "QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF",
                                strlen("QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF;
                            break;
                        }
                        if (0 == strncmp(substr, "QSH_LOC_TEST_TC8_POSITION_CLIENT_REF",
                                         strlen("QSH_LOC_TEST_TC8_POSITION_CLIENT_REF"))) {
                            myRequest = QSH_LOC_TEST_TC8_POSITION_CLIENT_REF;
                            break;
                        }
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
                } else if (QSH_LOC_TEST_GEOFENCE != myRequest &&
                           QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF != myRequest &&
                           QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF != myRequest) {
                    i = 0;
                    for (substr = strtok_r(gfStr, " ", &tmp);
                         substr != NULL;
                         substr = strtok_r(NULL, " ", &tmp)) {
                        if (0 == strncmp(substr, "timeout=", strlen("timeout="))) {
                            myTimeout = atoi(substr + strlen("timeout="));
                        } else if (0 == strncmp(substr, "duration=", strlen("duration="))) {
                            myDuration = atoi(substr + strlen("duration="));
                        } else if (0 == strncmp(substr, "iterations=", strlen("iterations="))) {
                            myIterations = atoi(substr + strlen("iterations="));
                        } else if (0 == strncmp(substr, "sessionMode=", strlen("sessionMode="))) {
                            mySessionMode = atoi(substr + strlen("sessionMode="));
                        } else if (0 == strncmp(substr, "pdrMode=", strlen("pdrMode="))) {
                            myPdrMode = atoi(substr + strlen("pdrMode="));
                        } else if (0 == strncmp(substr, "maxMeasCount=", strlen("maxMeasCount="))) {
                            myMaxMeasCount = atoi(substr + strlen("maxMeasCount="));
                        } else if (i == 0) {
                            myInterval = atoi(substr);
                        }
                        i++;
                    }
                    break;
                } else if (QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF == myRequest) {
                    i = 0;
                    for (substr = strtok_r(gfStr, " ", &tmp);
                         substr != NULL;
                         substr = strtok_r(NULL, " ", &tmp)) {
                        if (client_idx >= GNSS_MAX_CLIENTS) {
                            break;
                        }
                        switch (i) {
                            case 0:
                                myClientParams[client_idx].sessionMode = atoi(substr);
                                break;
                            case 1:
                                myClientParams[client_idx].pdrMode = atoi(substr);
                                break;
                            case 2:
                                myClientParams[client_idx].maxMeasCount = atoi(substr);
                                break;
                            case 3:
                                myClientParams[client_idx].interval = atoi(substr);
                                break;
                            default:
                                break;
                        }
                        i++;
                        if (4 == i) {
                            client_idx++;
                            i = 0;
                        }
                    }
                    if (i > 0) {
                        client_idx++;
                    }
                    if (client_idx >= GNSS_MAX_CLIENTS) {
                        break;
                    }
                    continue;
                } else {
                    if (k >= GNSS_MAX_GEOFENCES) {
                        LOC_LOGe("Max geofences (%d) exceeded, ignoring remaining geofences",
                                 GNSS_MAX_GEOFENCES);
                        break;
                    }
                    i = 0;
                    myGeofences[k].has_id = false;
                    myGeofences[k].id = 0;
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
                            case 6:
                                /* optional id column, from a prior ADD's ack */
                                myGeofences[k].has_id = true;
                                myGeofences[k].id = (uint32_t)atoi(substr);
                                break;
                            default:
                                break;
                        }
                        i++;
                        if (7 == i) {
                            break;
                        }
                    }
                    if (i) {
                        k++;
                    }
                }
            }
        }
        myCount = k;
        myClientCount = client_idx;
        fclose(gf_fp);
    } else {
        LOC_LOGe("Cannot open loc_test.txt for read!");
        printf("Cannot open loc_test.txt for read!\n");
        return -1;
    }
    std::string sensorName = SENSOR_NAME;
    lookup.getSuid(sensorName);
    printf("myRequest = %d myInterval = %d myCount = %d myClientCount = %d\n",
           myRequest, myInterval, myCount, myClientCount);
    printf("  timeout = %d iterations = %d sessionMode = %d pdrMode = %d maxMeasCount = %d"
           " duration = %d\n",
           myTimeout, myIterations, mySessionMode, myPdrMode, myMaxMeasCount, myDuration);

    if (0 == myRequest) {
        printf("Error (must select an option), please re-configure!\n");
        cleanup_and_exit(-1);
    }

    bool requires_interval = (QSH_LOC_TEST_GEOFENCE != myRequest &&
                              QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF != myRequest &&
                              QSH_LOC_TEST_TC6_CAPS_VERSION_CLIENT_REF != myRequest);

    if (requires_interval && 0 == myInterval) {
        printf("Error (interval must be non zero for this test), please re-configure!\n");
        cleanup_and_exit(-1);
    }

    bool is_geofence_test = (QSH_LOC_TEST_GEOFENCE == myRequest ||
                             QSH_LOC_TEST_TC11_GEOFENCE_ADD_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC12_GEOFENCE_REMOVE_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC13_GEOFENCE_MODIFY_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC14_GEOFENCE_PAUSE_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC15_GEOFENCE_RESUME_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC24_GEOFENCE_REF_ADD_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC25_GEOFENCE_REF_MODIFY_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC26_GEOFENCE_REF_PAUSE_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC27_GEOFENCE_REF_RESUME_CLIENT_REF == myRequest ||
                             QSH_LOC_TEST_TC28_GEOFENCE_REF_REMOVE_CLIENT_REF == myRequest);

    if (is_geofence_test && 0 == myCount) {
        printf("Error (must have at least one geofence), please re-configure!\n");
        cleanup_and_exit(-1);
    }

    if (QSH_LOC_TEST_TC1_MULTI_CLIENT_CLIENT_REF == myRequest && myClientCount < 2) {
        printf("Error (TC1 requires at least 2 clients), please re-configure!\n");
        cleanup_and_exit(-1);
    }

    if ((QSH_LOC_TEST_TC2_OPEN_CLOSE_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC3_START_STOP_CLIENT_REF == myRequest) && 0 == myIterations) {
        printf("Error (TC2/TC3 require iterations > 0), please re-configure!\n");
        cleanup_and_exit(-1);
    }

    if ((QSH_LOC_TEST_TC8_POSITION_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC9_MEAS_AND_CLK_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC4_FIRST_FIX_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC5_FIRST_MEAS_EPOCH_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC18_ONE_SHOT_POSITION_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC19_ONE_SHOT_MEAS_AND_CLK_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC20_SUSTAINED_POSITION_TRACKING_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC21_SUSTAINED_MEAS_TRACKING_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC22_LOCATION_REQUEST_SWITCH_CLIENT_REF == myRequest ||
         QSH_LOC_TEST_TC23_SESSION_OPTIONS_CLIENT_REF == myRequest) && 0 == myTimeout) {
        printf("Warning: timeout not specified for test %d, using default 60s\n", myRequest);
        myTimeout = 60;
    }

    uint32_t timeout_seconds = myTimeout ? myTimeout : 60;

    /* Scale the wait bound for tests that hold the session open multiple
     * times per run (TC10 x2, TC16 x3, TC17 x4), so we don't time out
     * while the M55 side is still legitimately working. */
    if (myDuration > 0) {
        uint32_t hold_seconds = myDuration;
        if (QSH_LOC_TEST_TC10_POSITION_MEAS_AND_CLK_CLIENT_REF == myRequest) {
            hold_seconds = myDuration * 2;
        } else if (QSH_LOC_TEST_TC16_LOCATION_FULL_LIFECYCLE_CLIENT_REF == myRequest) {
            hold_seconds = myDuration * 3;
        } else if (QSH_LOC_TEST_TC17_GEOFENCE_FULL_LIFECYCLE_CLIENT_REF == myRequest) {
            hold_seconds = myDuration * 4;
        }
        uint32_t min_needed = hold_seconds + 10;
        if (timeout_seconds < min_needed) {
            timeout_seconds = min_needed;
        }
    }

    time_t start_time = time(NULL);
    printf("Test started. Waiting for completion (timeout: %ds)...\n", timeout_seconds);

    while (true) {
        if (test_completed) {
            printf("Test completed: %s\n", test_passed ? "PASS" : "FAIL");
            ret = test_passed ? 0 : -1;
            break;
        }

        time_t elapsed = difftime(time(NULL), start_time);
        if (elapsed > (time_t)timeout_seconds) {
            printf("Test timeout after %ld seconds\n", elapsed);
            LOC_LOGe("Test timeout");
            ret = -1;
            break;
        }

        sleep(1);
    }

    if (_qmiSession) {
        _qmiSession->close();
        _qmiSession.reset();
    }

    // Explicitly destroy lookup before exiting main to ensure any
    // internal resources/threads it owns are cleaned up deterministically.
    lookup.~LocIsession();

    return ret;
}
