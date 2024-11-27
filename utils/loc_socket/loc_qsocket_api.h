/** Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
    SPDX-License-Identifier: BSD-3-Clause-Clear */
#ifndef LOC_QSOCK_API
#define LOC_QSOCK_API

// 5025 is the offcially assigned ID
#define QSOCKET_LOC_SERVER_SERVICE_ID                   5025
#define QSOCKET_LOC_SERVER_INSTANCE_ID                  1
#define QSOCKET_LOC_CLIENT_SERVICE_ID                   5025
#define QSOCKET_LOC_CLIENT_AP_WWAN_INSTANCE_ID          6
#define QSOCKET_LOC_CLIENT_AP_RIL_SERVICE_INSTANCE_ID   7

// Define the min and max msg id for each sub features
#define QSOCK_LOC_WWAN_MSG_ID_RANGE_MIN 1
#define QSOCK_LOC_WWAN_MSG_ID_RANGE_MAX 100

#define QSOCK_LOC_RIL_SERVICE_MSG_ID_RANGE_MIN 101
#define QSOCK_LOC_RIL_SERVICE_MSG_ID_RANGE_MAX 200

// Other features msg id range to follow

#endif // #define LOC_QSOCK_API
