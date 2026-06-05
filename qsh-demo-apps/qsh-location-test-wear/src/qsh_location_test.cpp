/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <sns_client.pb.h>
#include <qsh_location.pb.h>
#include <loc_pla.h>
#include <loc_cfg.h>
#include <loc_log.h>
#include <log_util.h>
#include <map>
#include <qsh_utils_suid_util.h>

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

#undef LOG_NDEBUG
#define LOG_NDEBUG 0

#undef LOG_TAG
#define LOG_TAG "QSH_LOCATION_TEST"

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

#define SENSOR_NAME "location"

#define OPEN_REQUEST            0
#define LOCATION_START_REQUEST  1
#define LOCATION_STOP_REQUEST   2
#define DATA_START_REQUEST      3
#define DATA_STOP_REQUEST       4
#define QUIT_REQUEST            5
#define BUFFERSIZE 50

#define NO_OF_RETRIES 5

/*=============================================================================
  Static Data
  ===========================================================================*/

static unique_ptr<ISession> myIsession = nullptr;
static suid mySuid;
static bool isInitialized = false;
static int locInProgress = 0;
static int measInProgress = 0;
static void suid_cb(const std::string& dataType, const std::vector<suid>& suids);
static LocIsession lookup(suid_cb);

// Timer related
pid_t pid;
int signum;
struct timespec timeout;
sigset_t newmask;
siginfo_t info;
bool bVerboseMode = true;

//pdr mode related
static qsh_location_pdr_mode g_pdr_mode = QSH_LOCATION_PDR_MODE_CASUAL;
// Session‑mode handling
static qsh_location_session_mode g_session_mode = QSH_LOCATION_SESSION_MODE_NORMAL;
// Max measurement count
static uint32_t g_max_meas_count = 0;

#define GNSS_MAX_MEASUREMENT  128

typedef struct {
    int32_t sv_id;
    qsh_location_constellation_type constellation_type;
    int64_t time_offset;
    uint32_t state;
    int64_t received_sv_time;
    int64_t received_sv_time_uncertainty;
    float c_n0;
    float pseudorange_rate;
    float pseudorange_rate_uncertainty;
    bool has_carrier_phase;
    double carrier_phase;
    bool has_carrier_phase_uncertainty;
    float carrier_phase_uncertainty;
    bool has_cycle_slip_count;
    uint32_t cycle_slip_count;
    bool has_multipath_indicator;
    qsh_location_multipath_indicator multipath_indicator;
    bool has_snr;
    float snr;
    bool has_carrier_frequency;
    float carrier_frequency;
} qsh_measurements;

typedef struct {
    int64_t time;
    bool has_time_uncertainty;
    float time_uncertainty;
    bool has_full_bias;
    int64_t full_bias;
    bool has_bias;
    float bias;
    bool has_bias_uncertainty;
    float bias_uncertainty;
    bool has_drift;
    float drift;
    bool has_drift_uncertainty;
    float drift_uncertainty;
    bool has_hw_clock_discontinuity_count;
    uint32_t hw_clock_discontinuity_count;
} qsh_clock;

typedef struct {
    uint32_t numberOfMeasurements;
    qsh_clock clock;
    qsh_measurements measurements[GNSS_MAX_MEASUREMENT];
} qsh_location_meas_and_clk;

qsh_location_meas_and_clk allMeasAndClk;

static int measIndex = 0;

// Define the enum for qshLocationStatus
typedef enum {
    SERVICE_UNKNOWN = 0,
    SERVICE_AVAILABLE = 1,
    SERVICE_UNAVAILABLE = 2,
} qshLocationStatus;

// Define a variable of type qshLocationStatus
static qshLocationStatus locServiceStatus = SERVICE_UNKNOWN;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

static void send_open_req();
static void send_close_req();
static void send_update_req(bool start,
                            qsh_location_request request, uint32_t interval);

// Function to print the menu based on locServiceStatus
static void printMenu() {
    switch (locServiceStatus) {
        case SERVICE_UNKNOWN:
        case SERVICE_AVAILABLE:
            printf("\n\n"
                   "1: start/stop location test\n"
                   "2: start/stop measurement test\n"
                   "3: turn on/off screen output\n"
                   "q: quit\n"
                   "\nEnter Command:");
            break;
        case SERVICE_UNAVAILABLE:
            printf("\n\n"
                   "qsh location sensor is unavailable!\n"
                   "q: quit\n");
            break;
        default:
            break;
    }
}

static const char* event_cb_msg_string(uint32_t message_id) {
    switch (message_id) {
    case 128:
        return "SNS_STD_MSGID_SNS_STD_ATTR_EVENT";
    case 130:
        return "SNS_STD_MSGID_SNS_STD_ERROR_EVENT";
    case 512:
        return "QSH_LOCATION_MSGID_QSH_LOCATION_OPEN";
    case 513:
        return "QSH_LOCATION_MSGID_QSH_LOCATION_CLOSE";
    case 514:
        return "QSH_LOCATION_MSGID_QSH_LOCATION_UPDATE";
    case 768:
        return "QSH_LOCATION_MSGID_QSH_LOCATION_ACK";
    case 1024:
        return "QSH_LOCATION_MSGID_QSH_LOCATION_POSITION_EVENT";
    case 1025:
        return "QSH_LOCATION_MSGID_QSH_LOCATION_MEAS_AND_CLK_EVENT";
    case 1026:
        return "QSH_LOCATION_MSGID_QSH_LOCATION_ENGINE_STATE_EVENT";
    default:
        return "unsupprted MSGID";
    }
}

static void consume_meas_and_clk() {
    uint32_t i;

    if (bVerboseMode) {
        printf("clock: t=0x%" PRIx64" tu=%.2f fb=%" PRId64" b=%.2f bu=%.2f d=%.2f du=%.2f hd=%d\n",
            allMeasAndClk.clock.time, allMeasAndClk.clock.time_uncertainty,
            allMeasAndClk.clock.full_bias, allMeasAndClk.clock.bias,
            allMeasAndClk.clock.bias_uncertainty, allMeasAndClk.clock.drift,
            allMeasAndClk.clock.drift_uncertainty,
            allMeasAndClk.clock.hw_clock_discontinuity_count);
        for (i = 0; i < allMeasAndClk.numberOfMeasurements; i++) {
            printf("meas[%d] sv=%d cs=%d to=%" PRId64" st=0x%X rt=%" PRId64" ru=%" PRId64" cn=%.2f"
                " pr=%.2f pu=%.2f cf=%.2f cu=%.2f cs=%d mi=%d sn=%.2f fr=%.2f\n",
                i, allMeasAndClk.measurements[i].sv_id,
                allMeasAndClk.measurements[i].constellation_type,
                allMeasAndClk.measurements[i].time_offset, allMeasAndClk.measurements[i].state,
                allMeasAndClk.measurements[i].received_sv_time,
                allMeasAndClk.measurements[i].received_sv_time_uncertainty,
                allMeasAndClk.measurements[i].c_n0, allMeasAndClk.measurements[i].pseudorange_rate,
                allMeasAndClk.measurements[i].pseudorange_rate_uncertainty,
                allMeasAndClk.measurements[i].carrier_phase,
                allMeasAndClk.measurements[i].carrier_phase_uncertainty,
                allMeasAndClk.measurements[i].cycle_slip_count,
                allMeasAndClk.measurements[i].multipath_indicator,
                allMeasAndClk.measurements[i].snr,
                allMeasAndClk.measurements[i].carrier_frequency);
        }
    }
    LOC_LOGd("clock: t=0x%" PRIx64" tu=%.2f fb=%" PRId64" b=%.2f bu=%.2f d=%.2f du=%.2f hd=%d",
             allMeasAndClk.clock.time, allMeasAndClk.clock.time_uncertainty,
             allMeasAndClk.clock.full_bias, allMeasAndClk.clock.bias,
             allMeasAndClk.clock.bias_uncertainty, allMeasAndClk.clock.drift,
             allMeasAndClk.clock.drift_uncertainty,
             allMeasAndClk.clock.hw_clock_discontinuity_count);

    for (i = 0; i < allMeasAndClk.numberOfMeasurements; i++) {
        LOC_LOGd("meas[%d] sv=%d cs=%d to=%" PRId64" st=0x%X rt=%" PRId64" ru=%" PRId64" cn=%.2f"
                 " pr=%.2f pu=%.2f cf=%.2f cu=%.2f cs=%d mi=%d sn=%.2f fr=%.2f",
                 i, allMeasAndClk.measurements[i].sv_id,
                 allMeasAndClk.measurements[i].constellation_type,
                 allMeasAndClk.measurements[i].time_offset, allMeasAndClk.measurements[i].state,
                 allMeasAndClk.measurements[i].received_sv_time,
                 allMeasAndClk.measurements[i].received_sv_time_uncertainty,
                 allMeasAndClk.measurements[i].c_n0,
                 allMeasAndClk.measurements[i].pseudorange_rate,
                 allMeasAndClk.measurements[i].pseudorange_rate_uncertainty,
                 allMeasAndClk.measurements[i].carrier_phase,
                 allMeasAndClk.measurements[i].carrier_phase_uncertainty,
                 allMeasAndClk.measurements[i].cycle_slip_count,
                 allMeasAndClk.measurements[i].multipath_indicator,
                 allMeasAndClk.measurements[i].snr,
                 allMeasAndClk.measurements[i].carrier_frequency);
    }
}

static void parse_meas_and_clk(qsh_location_meas_and_clk_event* pMeasClkEvent) {
    int idx, tempIndex, tempSize, tempNoOfMeas;
    qsh_location_measurement tempMeas;
    qsh_location_clock tempClock;

    tempIndex = pMeasClkEvent->measurement_index();
    tempSize = pMeasClkEvent->measurements_size();
    tempNoOfMeas = pMeasClkEvent->number_of_measurements();
    LOC_LOGd("Received location meas and clock event size=%d noOfMeas=%d idx=%d",
             tempSize, tempNoOfMeas, tempIndex);

    if (1 == tempIndex) {
        memset(allMeasAndClk.measurements, 0,
               GNSS_MAX_MEASUREMENT*sizeof(qsh_measurements));
        allMeasAndClk.numberOfMeasurements = tempNoOfMeas;
        tempClock = pMeasClkEvent->clock();
        // get the clock information
        allMeasAndClk.clock.time = tempClock.time();
        allMeasAndClk.clock.has_time_uncertainty = tempClock.has_time_uncertainty();
        allMeasAndClk.clock.time_uncertainty = tempClock.time_uncertainty();
        allMeasAndClk.clock.has_full_bias = tempClock.has_full_bias();
        allMeasAndClk.clock.full_bias = tempClock.full_bias();
        allMeasAndClk.clock.has_bias = tempClock.has_bias();
        allMeasAndClk.clock.bias = tempClock.bias();
        allMeasAndClk.clock.has_bias_uncertainty = tempClock.has_bias_uncertainty();
        allMeasAndClk.clock.bias_uncertainty = tempClock.bias_uncertainty();
        allMeasAndClk.clock.has_drift = tempClock.has_drift();
        allMeasAndClk.clock.drift = tempClock.drift();
        allMeasAndClk.clock.has_drift_uncertainty = tempClock.has_drift_uncertainty();
        allMeasAndClk.clock.drift_uncertainty = tempClock.drift_uncertainty();
        allMeasAndClk.clock.has_hw_clock_discontinuity_count =
                tempClock.has_hw_clock_discontinuity_count();
        allMeasAndClk.clock.hw_clock_discontinuity_count =
                tempClock.hw_clock_discontinuity_count();
        measIndex = 0;
    }

    for (idx = 0; idx < tempSize; idx++) {
        tempMeas  = pMeasClkEvent->measurements(idx);
        allMeasAndClk.measurements[measIndex + idx].sv_id = tempMeas.sv_id();
        allMeasAndClk.measurements[measIndex + idx].constellation_type =
                tempMeas.constellation_type();
        allMeasAndClk.measurements[measIndex + idx].time_offset = tempMeas.time_offset();
        allMeasAndClk.measurements[measIndex + idx].state = tempMeas.state();
        allMeasAndClk.measurements[measIndex + idx].received_sv_time = tempMeas.received_sv_time();
        allMeasAndClk.measurements[measIndex + idx].received_sv_time_uncertainty =
                tempMeas.received_sv_time_uncertainty();
        allMeasAndClk.measurements[measIndex + idx].c_n0 = tempMeas.c_n0();
        allMeasAndClk.measurements[measIndex + idx].pseudorange_rate = tempMeas.pseudorange_rate();
        allMeasAndClk.measurements[measIndex + idx].pseudorange_rate_uncertainty =
                tempMeas.pseudorange_rate_uncertainty();
        allMeasAndClk.measurements[measIndex + idx].has_carrier_phase =
                tempMeas.has_carrier_phase();
        allMeasAndClk.measurements[measIndex + idx].carrier_phase = tempMeas.carrier_phase();
        allMeasAndClk.measurements[measIndex + idx].has_carrier_phase_uncertainty =
                tempMeas.has_carrier_phase_uncertainty();
        allMeasAndClk.measurements[measIndex + idx].carrier_phase_uncertainty =
                tempMeas.carrier_phase_uncertainty();
        allMeasAndClk.measurements[measIndex + idx].has_cycle_slip_count =
                tempMeas.has_cycle_slip_count();
        allMeasAndClk.measurements[measIndex + idx].cycle_slip_count = tempMeas.cycle_slip_count();
        allMeasAndClk.measurements[measIndex + idx].has_multipath_indicator =
                tempMeas.has_multipath_indicator();
        allMeasAndClk.measurements[measIndex + idx].multipath_indicator =
                tempMeas.multipath_indicator();
        allMeasAndClk.measurements[measIndex + idx].has_snr = tempMeas.has_snr();
        allMeasAndClk.measurements[measIndex + idx].snr = tempMeas.snr();
        allMeasAndClk.measurements[measIndex + idx].has_carrier_frequency =
                tempMeas.has_carrier_frequency();
        allMeasAndClk.measurements[measIndex + idx].carrier_frequency =
                tempMeas.carrier_frequency();
    }
    measIndex += tempSize;

    if (measIndex == tempNoOfMeas) {
        consume_meas_and_clk();
    }
}

/**
 * Event callback function, as registered with ssc_interface.
 */
static void event_cb(const uint8_t *data, size_t size, uint64_t ts) {
    sns_client_event_msg pb_event_msg;
    UNUSED_VAR(ts);

    union sigval sig_value;

    LOC_LOGd("Received sns_client_event_msg with length %zu", size);

    if (nullptr == data) {
        LOC_LOGe("data is nullptr!");
        return;
    }

    sig_value.sival_int = 1;

    pb_event_msg.ParseFromArray(data, size);
    for (int i = 0; i < pb_event_msg.events_size(); i++) {
        const sns_client_event_msg_sns_client_event &pb_event= pb_event_msg.events(i);
        LOC_LOGd("Event[%i] %s, ts=%llu", i, event_cb_msg_string(pb_event.msg_id()),
                 (unsigned long long)pb_event.timestamp());
        printf("\nReceived %s %s\n", event_cb_msg_string(pb_event.msg_id()), SENSOR_NAME);

        if (SNS_STD_MSGID_SNS_STD_ERROR_EVENT == pb_event.msg_id()) {
            sns_std_error_event error;
            error.ParseFromString(pb_event.payload());

            if (error.error() == SNS_STD_ERROR_INVALID_STATE) {
                LOC_LOGd("SNS_STD_ERROR_INVALID_STATE, %s is offline", SENSOR_NAME);
                printf("SNS_STD_ERROR_INVALID_STATE, %s is offline", SENSOR_NAME);
                // clean up myIsession
                if (myIsession) {
                    myIsession->close();
                    myIsession = nullptr;
                }
            } else if (error.error() == SNS_STD_ERROR_NOT_AVAILABLE) {
                // Subsystem recovery case
                LOC_LOGd("SNS_STD_ERROR_NOT_AVAILABLE), %s SSR", SENSOR_NAME);
                printf("SNS_STD_ERROR_NOT_AVAILABLE), %s recovery", SENSOR_NAME);
            } else {
                LOC_LOGe("Received error event %i", error.error());
                printf("Received error event %i\n", error.error());
            }
        } else if (SNS_STD_MSGID_SNS_STD_ATTR_EVENT == pb_event.msg_id()) {
            sns_std_attr_event attr_event;
            int32_t attr_id;

            attr_event.ParseFromString(pb_event.payload());
            attr_id = attr_event.attributes(0).attr_id();
            LOC_LOGd("Received attr_event attr_id %d", attr_id);
        } else if (QSH_LOCATION_MSGID_QSH_LOCATION_ACK == pb_event.msg_id()) {
            qsh_location_ack ack;
            ack.ParseFromString(pb_event.payload());

            LOC_LOGd("Received location ack err=%d cmd=%d "
                     "version=0x%X capabilities=0x%X",
                     ack.error(), ack.command(),
                     ack.version(), ack.capabilities());
            sigqueue(pid, signum, sig_value);

        } else if (QSH_LOCATION_MSGID_QSH_LOCATION_POSITION_EVENT == pb_event.msg_id()) {
            qsh_location_position_event posEvent;
            posEvent.ParseFromString(pb_event.payload());

            if (bVerboseMode) {
                printf("Received position event\n");
                printf("session status=%d clock: t=%" PRIu64" lat=%d lon=%d alt=%.2f "
                       "altWrtMeanSeaLevel=%.2f hacc=%.2f vacc=%.2f sp=%.2f "
                       "sacc=%.2f b=%.2f bacc=%.2f ci=%.2f "
                       "pDop = %.2f, hDop = %.2f, vDop = %.2f\n",
                       posEvent.session_status(),
                       posEvent.timestamp(),
                       posEvent.latitude(),
                       posEvent.longitude(),
                       posEvent.altitude(),
                       posEvent.altitudewrtmeansealevel(),
                       posEvent.horizontal_accuracy(),
                       posEvent.vertical_accuracy(),
                       posEvent.speed(),
                       posEvent.speed_accuracy(),
                       posEvent.bearing(),
                       posEvent.bearing_accuracy(),
                       posEvent.conformity_index(),
                       posEvent.pdop(),
                       posEvent.hdop(),
                       posEvent.vdop());

                printf("SV used count = %u\n", posEvent.sv_used_count());
                for (int i = 0; i < posEvent.sv_used_size(); ++i) {
                    const qsh_location_sv_used& sv = posEvent.sv_used(i);
                    printf("SV[%d]: id=%d, constellation=%d\n",
                        i, sv.sv_id(), sv.constellation_type());
                }
            }
            LOC_LOGd("timestamp=%" PRIu64" latitude=%d",
                     posEvent.timestamp(),
                     posEvent.latitude());
            LOC_LOGd("longitude=%d altitude=%f altitudeWrtMeanSeaLevel=%f",
                     posEvent.longitude(),
                     posEvent.altitude(),
                     posEvent.altitudewrtmeansealevel());
            LOC_LOGd("horizontal_accuracy=%f vertical_accuracy=%f",
                     posEvent.horizontal_accuracy(),
                     posEvent.vertical_accuracy());
            LOC_LOGd("speed=%f speed_accuracy=%f",
                     posEvent.speed(),
                     posEvent.speed_accuracy());
            LOC_LOGd("bearing=%f bearing_accuracy=%f",
                     posEvent.bearing(),
                     posEvent.bearing_accuracy());
            LOC_LOGd("conformity_index=%f", posEvent.conformity_index());
            LOC_LOGd("session_status=%d", posEvent.session_status());
            LOC_LOGd("PDOP = %.2f, HDOP = %.2f, VDOP = %.2f\n",
                        posEvent.pdop(), posEvent.hdop(), posEvent.vdop());
            LOC_LOGd("SV used count = %u\n", posEvent.sv_used_count());

            for (int i = 0; i < posEvent.sv_used_size(); ++i) {
                const qsh_location_sv_used& sv = posEvent.sv_used(i);
                LOC_LOGd("SV[%d]: id=%d, constellation=%d\n",
                    i, sv.sv_id(), sv.constellation_type());
            }
        } else if (QSH_LOCATION_MSGID_QSH_LOCATION_MEAS_AND_CLK_EVENT == pb_event.msg_id()) {
            qsh_location_meas_and_clk_event measClkEvent;
            measClkEvent.ParseFromString(pb_event.payload());
            parse_meas_and_clk(&measClkEvent);
        } else if (QSH_LOCATION_MSGID_QSH_LOCATION_ENGINE_STATE_EVENT == pb_event.msg_id()) {
            qsh_location_engine_state_event engStateEvent;
            engStateEvent.ParseFromString(pb_event.payload());

            if (bVerboseMode) {
                printf("Received engine state event\n");
                printf("engine state = %d\n",
                       engStateEvent.engine_state());
            }
            LOC_LOGd("Received engine state event, eng state = %d",
                     engStateEvent.engine_state());
        } else {
            LOC_LOGe("Unsupprted message ID: %d", pb_event.msg_id());
        }
    }
}

/**
* Send an location open request for the specified SUID.
*/
static void send_open_req() {
    string pb_req_msg_encoded;
    string location_open_encoded;
    sns_client_request_msg pb_req_msg;
    qsh_location_open location_open;

    LOC_LOGd("Send location open request");

    if (nullptr == myIsession) {
        LOC_LOGe("myIsession is nullptr!");
        return;
    }

    location_open.set_version(0x01000000);
    location_open.SerializeToString(&location_open_encoded);

    pb_req_msg.set_msg_id(QSH_LOCATION_MSGID_QSH_LOCATION_OPEN);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->set_payload(location_open_encoded);
    } else {
        LOC_LOGe("mutable_request failed!");
        return;
    }
    sns_std_suid* pStdSuid = pb_req_msg.mutable_suid();
    if (pStdSuid) {
        pStdSuid->set_suid_high(mySuid.high);
        pStdSuid->set_suid_low(mySuid.low);
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
    myIsession->sendRequest(mySuid, pb_req_msg_encoded);
    pb_req_msg.clear_request();
    pb_req_msg.clear_suid();
    pb_req_msg.clear_susp_config();
}

/**
* Send an location close request for the specified SUID.
*/
static void send_close_req() {
    string pb_req_msg_encoded;
    sns_client_request_msg pb_req_msg;

    LOC_LOGd("Send location close request");

    if (nullptr == myIsession) {
        LOC_LOGe("myIsession is nullptr!");
        return;
    }

    pb_req_msg.set_msg_id(QSH_LOCATION_MSGID_QSH_LOCATION_CLOSE);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->clear_payload();
    } else {
        LOC_LOGe("mutable_request failed!");
        return;
    }
    sns_std_suid* pStdSuid = pb_req_msg.mutable_suid();
    if (pStdSuid) {
        pStdSuid->set_suid_high(mySuid.high);
        pStdSuid->set_suid_low(mySuid.low);
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
    myIsession->sendRequest(mySuid, pb_req_msg_encoded);
    pb_req_msg.clear_request();
    pb_req_msg.clear_suid();
    pb_req_msg.clear_susp_config();
}

/**
* Send an location update request for the specified SUID.
*/
static void send_update_req(bool start,
                            qsh_location_request request, uint32_t interval) {
    string pb_req_msg_encoded;
    string location_update_encoded;
    sns_client_request_msg pb_req_msg;
    qsh_location_update location_update;

    LOC_LOGd("Send location update request");

    if (nullptr == myIsession) {
        LOC_LOGe("myIsession is nullptr!");
        return;
    }

    location_update.set_start(start);
    location_update.set_location_request(request);
    location_update.set_interval(interval);
    printf("Setting pdr mode to %d", g_pdr_mode);
    location_update.set_pdrmode(g_pdr_mode);
    printf("Setting session mode to %d", g_session_mode);
    location_update.set_sessionmode(g_session_mode);
    if (g_max_meas_count > 0) {
        printf("Setting max meas count to %d", g_max_meas_count);
        location_update.set_maxmeascount(g_max_meas_count);
    }

    location_update.SerializeToString(&location_update_encoded);

    pb_req_msg.set_msg_id(QSH_LOCATION_MSGID_QSH_LOCATION_UPDATE);
    sns_std_request* pStdReq = pb_req_msg.mutable_request();
    if (pStdReq) {
        pStdReq->set_payload(location_update_encoded);
    } else {
        LOC_LOGe("mutable_request failed!");
        return;
    }
    sns_std_suid* pStdSuid = pb_req_msg.mutable_suid();
    if (pStdSuid) {
        pStdSuid->set_suid_high(mySuid.high);
        pStdSuid->set_suid_low(mySuid.low);
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
    myIsession->sendRequest(mySuid, pb_req_msg_encoded);
    pb_req_msg.clear_request();
    pb_req_msg.clear_suid();
    pb_req_msg.clear_susp_config();
}

/**
 * SUID callback as registered with LocIsession.
*/
static void suid_cb(const std::string& dataType, const std::vector<suid>& suids) {
    LOC_LOGd("Received SUID event with length %zu", suids.size());
    qshLocationStatus prevStatus = locServiceStatus;

    if (suids.size() > 0) {
        mySuid = suids[0];
        // create myIsession to send QSH_LOCATION_MSGs
        unique_ptr<sessionFactory> factory = make_unique<sessionFactory>();
        if (nullptr == factory){
            printf("failed to create factory instance");
            exit(-1);
        }

        myIsession = unique_ptr<ISession>(factory->getSession());
        if (nullptr == myIsession){
            printf("failed to create session for attribute query");
            exit(-1);
        }

        // open the myIsession session
        int ret = myIsession->open();
        if (-1 == ret){
            printf("failed to open ISession for attribute query");
            return;
        }
        ISession::errorCallBack error_cb =[](ISession::error error) {
            LOC_LOGe("error occurred: %d!", error);
            printf("ADSP SSR occured ");
            if (error == com::quic::sensinghub::session::V1_0::ISession::RESET) {
                printf("\nPress ENTER to show the menu\n");
                isInitialized = false;
                locInProgress = 0;
                measInProgress = 0;
            }
        };
        // set event_cb for the myIsession for mySuid
        ret = myIsession->setCallBacks(mySuid, nullptr, error_cb, event_cb);
        if (0 == ret) {
            LOC_LOGd("Request attributes for - suid_low=%" PRIu64 " suid_high=%" PRIu64 "\n",
                    mySuid.low, mySuid.high);
            lookup.getAttributes(myIsession, mySuid);
            locServiceStatus = SERVICE_AVAILABLE;
        } else {
            LOC_LOGe("set event CallBacks failed");
            locServiceStatus = SERVICE_UNAVAILABLE;
        }
    } else {
        LOC_LOGe("%s sensor is not available", SENSOR_NAME);
        locServiceStatus = SERVICE_UNAVAILABLE;
    }
    if (prevStatus != locServiceStatus && prevStatus != SERVICE_UNKNOWN) {
        if (SERVICE_UNAVAILABLE == locServiceStatus) {
            printf("\n%s sensor is not available", SENSOR_NAME);
            isInitialized = false;
            locInProgress = 0;
            measInProgress = 0;
        }
        if (SERVICE_AVAILABLE == locServiceStatus) {
            printf("\n%s sensor is available", SENSOR_NAME);
        }
        printf("\nPress ENTER to show the menu\n");
    }
}

int wait_for_ack(uint8_t request) {
    int ret = -1;
    sigset_t oldmask;

    pthread_sigmask(SIG_BLOCK, &newmask, &oldmask);
    LOC_LOGd("Wait for ack request=%d pid=%d", request, pid);
    while (true) {
        ret = sigtimedwait(&newmask, &info, &timeout);
        if (-1 == ret) {
            LOC_LOGw("Timeout waiting for ack errno=%d", errno);
            if (EAGAIN == errno) {
                break;
            }
            else if (EINTR == errno) {
                continue;
            }
            LOC_LOGw("Timeout waiting for ack errno=%d", errno);
            break;
        }
        if (signum == info.si_signo) {
            LOC_LOGd("Got signal from ACK");
            break;
        }
    }
    pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
    return ret;
}

int test_loc_start(uint32_t request, uint32_t rate) {
    int ret = -1;
    int retries;

    LOC_LOGd("test_loc_start");
    if (locServiceStatus != SERVICE_AVAILABLE) {
        LOC_LOGd("No action for locServiceStatus %d", locServiceStatus);
        return 0;
    }

    if (!isInitialized) {
        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_open_req();
            ret = wait_for_ack(OPEN_REQUEST);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for OPEN_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        isInitialized = true;
    }

    switch (request) {
    case LOCATION_START_REQUEST:
        LOC_LOGd("Start Location");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_update_req(true, QSH_LOCATION_POSITION_REQUEST, rate);
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for LOCATION_START_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        break;
    case LOCATION_STOP_REQUEST:
        LOC_LOGd("Stop Location");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_update_req(false, QSH_LOCATION_POSITION_REQUEST, rate);
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for LOCATION_STOP_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        break;
    case DATA_START_REQUEST:
        LOC_LOGd("Start meas and clk");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_update_req(true, QSH_LOCATION_MEAS_AND_CLK_REQUEST, rate);
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for DATA_START_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        break;
    case DATA_STOP_REQUEST:
        LOC_LOGd("Stop meas and clk");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_update_req(false, QSH_LOCATION_MEAS_AND_CLK_REQUEST, rate);
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for DATA_STOP_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
            goto bail;
        }
        break;
    case QUIT_REQUEST:
        LOC_LOGd("Quit");

        for (retries = 0; retries < NO_OF_RETRIES; retries++) {
            send_close_req();
            ret = wait_for_ack(request);
            if (-1 != ret) {
                break;
            }
        }
        if (-1 == ret) {
            LOC_LOGw("Did not receive ACK for QUIT_REQUEST after %d tries! Give up!",
                     NO_OF_RETRIES);
        }
        goto bail;
        break;
    default:
        goto bail;
        break;
    }
    return 0;

bail:
    LOC_LOGd("Bye bye");
    if (-1 == ret) {
        send_close_req();
    }
    return ret;
}

// Helper to execute a test entry
static int executeTest(const std::vector<int>& values) {
    int ret = -1;
    // Apply PDR and session mode if provided
    if (values.size() >= 5) {
        int pdrMode = values[3];
        g_pdr_mode = (pdrMode == 1) ? QSH_LOCATION_PDR_MODE_CASUAL : QSH_LOCATION_PDR_MODE_FITNESS;
        int sessMode = values[4];
        switch (sessMode) {
            case 1: g_session_mode = QSH_LOCATION_SESSION_MODE_NORMAL; break;
            case 2: g_session_mode = QSH_LOCATION_SESSION_MODE_SWIM; break;
            case 3: g_session_mode = QSH_LOCATION_SESSION_MODE_STAMINA; break;
            default: g_session_mode = QSH_LOCATION_SESSION_MODE_NORMAL; break;
        }
    }
    int scenario = values[0];
    int rate = values[1];
    int duration = values[2];

    if (scenario == 1) {
        ret = test_loc_start(LOCATION_START_REQUEST, rate);
        if (0 == ret) {
            printf("success - start location request\n");
            sleep(duration);
            ret = test_loc_start(LOCATION_STOP_REQUEST, rate);
            if (0 == ret) {
                printf("success - stop location request\n");
            } else {
                printf("failed - stop location request\n");
            }
        } else {
            printf("failed - start location request\n");
        }
    } else if (scenario == 2) {
        ret = test_loc_start(DATA_START_REQUEST, rate);
        if (0 == ret) {
            printf("success - start data request\n");
            sleep(duration);
            ret = test_loc_start(DATA_STOP_REQUEST, rate);
            if (0 == ret) {
                printf("success - stop data request\n");
            } else {
                printf("failed - stop data request\n");
            }
        } else {
            printf("failed - start data request\n");
        }
    } else if (scenario == 10) {
        printf("scenario 10: going to sleep for %d seconds \n", duration);
        sleep(duration);
    } else if (scenario == 11) {
        ret = test_loc_start(LOCATION_START_REQUEST, rate);
        if (0 == ret) {
            printf("scenario 11: success - start location request\n");
        } else {
            printf("scenario 11: failed - start location request\n");
        }
    } else if (scenario == 12) {
        ret = test_loc_start(LOCATION_STOP_REQUEST, rate);
        if (0 == ret) {
            printf("scenario 12: success - stop location request\n");
        } else {
            printf("scenario12: failed - stop location request\n");
        }
    } else if (scenario == 21) {
        ret = test_loc_start(DATA_START_REQUEST, rate);
        if (0 == ret) {
            printf("scenario 21: success - start data request\n");
        } else {
            printf("scenario 21: failed - start data request\n");
        }
    } else if (scenario == 22) {
        ret = test_loc_start(DATA_STOP_REQUEST, rate);
        if (0 == ret) {
            printf("scenario 22: success - stop data request\n");
        } else {
            printf("scenario 22: failed - stop data request\n");
        }
    }

    return ret;
}

static int loadReqFromFile(string inputKey) {
    FILE *gf_fp;
    char line[256];
    std::map<std::string, std::vector<int>> data;
    std::vector<std::string> order; // preserve file order
    int ret = -1;

    if ((gf_fp = fopen("/vendor/etc/qsh_location_test.txt", "r")) != NULL) {
        while (fgets(line, sizeof(line), gf_fp)) {
            char key[50];
            int val1, val2, val3, val4, val5;

            //   File format:
            //   key = case,rate,duration,pdrMode,sessionMode
            //   pdrMode: 1 = CASUAL, 2 = FITNESS
            //   sessionMode: 1 = NORMAL, 2 = SWIM, 3 = STAMINA
            if (sscanf(line, "%49s = %d,%d,%d,%d,%d", key, &val1, &val2, &val3, &val4, &val5) == 6){
                std::string skey(key);
                data[skey] = {val1, val2, val3, val4, val5};
                order.push_back(skey);
            }
        }
        fclose(gf_fp);
        if (inputKey == "-ALL-") {
            // Execute synchronously in the same order as in qsh_location_test.txt
            for (const auto& key : order) {
                auto it = data.find(key);
                if (it != data.end()) {
                    executeTest(it->second);
                }
            }
        } else if (data.find(inputKey) != data.end()) {
            executeTest(data[inputKey]);
        } else {
            printf("test not defined\n");
        }
        test_loc_start(QUIT_REQUEST, 0);
    } else {
        LOC_LOGe("Cannot open qsh_location_test.txt for read!");
        printf("Cannot open qsh_location_test.txt for read!\n");
        return -1;
    }
    return ret;
}

int main(int argc, char *argv[]) {
    UNUSED_VAR(argc);
    UNUSED_VAR(argv);

    int ret = -1;
    int ret2 = -1;
    char buf[16], *p;
    int exit_loop = 0;
    uint locRate = 1000, measRate = 1000;
    char str[BUFFERSIZE];
    char* pch;

    pid = getpid();
    signum = SIGUSR2;
    timeout.tv_sec = 10;
    timeout.tv_nsec = 0;
    sigemptyset(&newmask);
    UTIL_READ_CONF_DEFAULT(LOC_PATH_GPS_CONF);

    LOC_LOGd("SIGRTMIN = %d, signum=%d", SIGRTMIN, signum);

    ret2 = sigaddset(&newmask, signum);

    LOC_LOGd("after sigaddset ret2=%d , errno=%d, signum=%d", ret2, errno, signum);
    pthread_sigmask(SIG_BLOCK, &newmask, NULL);
    sigfillset(&newmask);

    LOC_LOGd("Begin Location Test threadid = %ld", pthread_self());

    //retrieve suids for the specified sensor
    std::string sensorName = SENSOR_NAME;
    lookup.getSuid(sensorName);

    if (argc == 2) {
        ret = loadReqFromFile(argv[1]);
        exit_loop = 1;
    }

    while (0 == exit_loop) {
        printMenu();
        fflush(stdout);
        p = fgets(buf, 16, stdin);
        if (NULL == p) {
            printf("Error: fgets returned nullptr !!\n");
            continue;
        }

        if (p[0] == 'q') {
            if (locInProgress) {
                ret = test_loc_start(LOCATION_STOP_REQUEST, locRate);
            }
            if (measInProgress) {
                ret = test_loc_start(DATA_STOP_REQUEST, measRate);
            }
            test_loc_start(QUIT_REQUEST, 0);
            exit_loop = 1;
        }

        if (locServiceStatus == SERVICE_AVAILABLE) {
            switch (p[0]) {
            case '1':
                if (locInProgress) {
                    ret = test_loc_start(LOCATION_STOP_REQUEST, locRate);
                    locInProgress = 0;
                    if (0 == ret) {
                        printf("success\n");
                    } else {
                        printf("failed\n");
                        exit_loop = 1;
                    }
                }
                else {
                    printf("Enter rate in milliseconds (>=1000): ");
                    pch = fgets(str, BUFFERSIZE, stdin);
                    if (pch != NULL) {
                        locRate = atoi(pch);
                    }

                    //PDR mode selection
                    char pdrMode_buf[8];
                    printf("Enter PDR mode (c for casual, f for fitness): ");
                    if (fgets(pdrMode_buf, sizeof(pdrMode_buf), stdin) != NULL) {
                        if (pdrMode_buf[0] == 'c' || pdrMode_buf[0] == 'C') {
                            g_pdr_mode = QSH_LOCATION_PDR_MODE_CASUAL;
                            printf("PDR mode set to CASUAL\n");
                        } else if (pdrMode_buf[0] == 'f' || pdrMode_buf[0] == 'F') {
                            g_pdr_mode = QSH_LOCATION_PDR_MODE_FITNESS;
                            printf("PDR mode set to FITNESS\n");
                        } else {
                            printf("Invalid selection, PDR mode unchanged\n");
                        }
                    }

                    //Session mode selection
                    char sessionMode_buf[8];
                    printf("Enter session mode (n for normal, s for swim, t for stamina): ");
                    if (fgets(sessionMode_buf, sizeof(sessionMode_buf), stdin) != NULL) {
                        if (sessionMode_buf[0] == 'n' || sessionMode_buf[0] == 'N') {
                            g_session_mode = QSH_LOCATION_SESSION_MODE_NORMAL;
                            printf("Session mode set to NORMAL\n");
                        } else if (sessionMode_buf[0] == 's' || sessionMode_buf[0] == 'S') {
                            g_session_mode = QSH_LOCATION_SESSION_MODE_SWIM;
                            printf("Session mode set to SWIM\n");
                        } else if (sessionMode_buf[0] == 't' || sessionMode_buf[0] == 'T') {
                            g_session_mode = QSH_LOCATION_SESSION_MODE_STAMINA;
                            printf("Session mode set to STAMINA\n");
                        } else {
                            printf("Invalid selection, session mode unchanged\n");
                        }
                    }

                    ret = test_loc_start(LOCATION_START_REQUEST, locRate);
                    locInProgress = 1;
                    if (0 == ret) {
                        printf("success\n");
                    } else {
                        printf("failed\n");
                        exit_loop = 1;
                    }
                }
                break;
            case '2':
                if (measInProgress) {
                    ret = test_loc_start(DATA_STOP_REQUEST, measRate);
                    measInProgress = 0;
                    if (0 == ret) {
                        printf("success\n");
                    } else {
                        printf("failed\n");
                        exit_loop = 1;
                    }
                } else {
                    printf("Enter rate in milliseconds (>=1000): ");
                    pch = fgets(str, BUFFERSIZE, stdin);
                    if (pch != NULL) {
                        measRate = atoi(pch);
                    }

                    // Max Measurement Count
                    printf("Enter maxMeasCount (0 = not_specified): ");
                    pch = fgets(str, BUFFERSIZE, stdin);
                    if (pch != NULL) {
                        g_max_meas_count = atoi(pch);
                    }

                    ret = test_loc_start(DATA_START_REQUEST, measRate);
                    measInProgress = 1;
                    if (0 == ret) {
                        printf("success\n");
                    } else {
                        printf("failed\n");
                        exit_loop = 1;
                    }
                }
                break;
            case '3':
                bVerboseMode = !bVerboseMode;
                break;
            default:
                break;
            }
        }
    }

    if (myIsession){
        myIsession->close();
        myIsession.reset();
    }

    // Explicitly destroy lookup before exiting main to ensure any
    // internal resources/threads it owns are cleaned up deterministically.
    lookup.~LocIsession();

    LOC_LOGd("Sensors Location Test completed - Exiting");
    return ret;
}
