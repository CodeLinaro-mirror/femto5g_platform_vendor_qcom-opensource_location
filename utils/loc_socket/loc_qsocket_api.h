/** Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
    SPDX-License-Identifier: BSD-3-Clause-Clear */
#ifndef LOC_QSOCK_API
#define LOC_QSOCK_API

// 5025 is the offcially assigned ID
#define QSOCKET_LOC_SERVER_SERVICE_ID                   5025
#define QSOCKET_LOC_SERVER_INSTANCE_ID                  1
#define QSOCKET_LOC_CLIENT_SERVICE_ID                   5025
#define QSOCKET_LOC_CLIENT_AP_WWAN_UEA_INSTANCE_ID      6
#define QSOCKET_LOC_CLIENT_AP_RIL_SERVICE_INSTANCE_ID   7
#define QSOCKET_LOC_CLIENT_AP_WWAN_INSTANCE_ID          8
#define QSOCKET_LOC_CLIENT_AP_SMCHAL_INSTANCE_ID        9
#define QSOCKET_LOC_CLIENT_AP_GNSS_AIDING_SERVICE_INSTANCE_ID        10


// Define the min and max msg id for each sub features
#define QSOCK_LOC_WWAN_MSG_ID_RANGE_MIN 1
#define QSOCK_LOC_WWAN_MSG_ID_RANGE_MAX 100

#define QSOCK_LOC_RIL_SERVICE_MSG_ID_RANGE_MIN 101
#define QSOCK_LOC_RIL_SERVICE_MSG_ID_RANGE_MAX 200

#define QSOCK_LOC_WWAN_UEB_MSG_ID_RANGE_MIN 201
#define QSOCK_LOC_WWAN_UEB_MSG_ID_RANGE_MAX 300

#define QSOCK_LOC_SMCHAL_MSG_ID_RANGE_MIN 301
#define QSOCK_LOC_SMCHAL_MSG_ID_RANGE_MAX 400

#endif // #define LOC_QSOCK_API
