/** Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
    SPDX-License-Identifier: BSD-3-Clause-Clear */
#ifndef LOC_QSOCK_API
#define LOC_QSOCK_API
/******************************************************************************
IMPORTANT WORKFLOW PROCESS NOTE
When adding a new Qsocket msg ID, you MUST strictly follow the following process,
1) Any new definitions here should map with definitions maintained on MPSS.
   No new changes should be made in this file, without first updating the MPSS
   master file.
2) Ensure that new definition you add do not create conflict with existing definitions
   based on MPSS master file.
3) All the definitions here are also mentioned with their corresponding definitions on
   APSS within comments.This is to ensure there is no conflicts in keeping the file
   consistent across APSS and MPSS.
   IT IS IMPORTANT TO NOTE THAT CONFLICTS OR INCONSISTENCIES IN THE QSOCKET DEFINITIONS
   ON APSS AND MPSS WILL BREAK THE SOFTWARE FEATURES RELYING ON QSOCKETS.
   IT IS VERY IMPORTANT AND ESSENTIAL TO FOLLOW THESE GUIDELINES.
******************************************************************************/

// 5025 is the offcially assigned ID
#define QSOCKET_LOC_SERVER_SERVICE_ID                   5025
/*Must match with MPSS definition of OS_GNSS_MPSS_SERVER_SOCKET_INSTANCE_ID*/
#define QSOCKET_LOC_SERVER_INSTANCE_ID                  1
#define QSOCKET_LOC_CLIENT_SERVICE_ID                   5025
/*Must match with MPSS definition of GNSS_APSS_WWAN_CLIENT_AP_WWAN_UEA_INSTANCE_ID*/
#define QSOCKET_LOC_CLIENT_AP_WWAN_UEA_INSTANCE_ID      6
/*Must match with MPSS definition of GNSS_APSS_WWAN_RIL_CLIENT_INSTANCE_ID*/
#define QSOCKET_LOC_CLIENT_AP_RIL_SERVICE_INSTANCE_ID   7
/*Must match with MPSS definition of GNSS_APSS_WWAN_CLIENT_AP_WWAN_INSTANCE_ID*/
#define QSOCKET_LOC_CLIENT_AP_WWAN_INSTANCE_ID          8
/*Must match with MPSS definition of GNSS_APSS_SMCHAL_CLIENT_INSTANCE_ID*/
#define QSOCKET_LOC_CLIENT_AP_SMCHAL_INSTANCE_ID        9
/*Must match with MPSS definition of GNSS_APSS_WWAN_MEAS_CLIENT_INSTANCE_ID*/
#define QSOCKET_LOC_CLIENT_AP_WWAN_MEAS_CLIENT_INSTANCE_ID    (10)
/*Must match with MPSS definition of GNSS_APSS_XTRA_AIDING_CLIENT_AP_INSTANCE_ID*/
#define QSOCKET_LOC_CLIENT_AP_GNSS_AIDING_SERVICE_INSTANCE_ID (11)


// Define the min and max msg id for each sub features

/*Must match with MPSS definition of GNSS_SERVER_SOCK_WWAN_MSG_ID_MIN*/
#define QSOCK_LOC_WWAN_MSG_ID_RANGE_MIN 1
/*Must match with MPSS definition of GNSS_SERVER_SOCK_WWAN_MSG_ID_MAX*/
#define QSOCK_LOC_WWAN_MSG_ID_RANGE_MAX 100
/*Must match with MPSS definition of GNSS_SERVER_SOCK_RIL_SERVICE_MSG_ID_MIN*/
#define QSOCK_LOC_RIL_SERVICE_MSG_ID_RANGE_MIN 101
/*Must match with MPSS definition of GNSS_SERVER_SOCK_RIL_SERVICE_MSG_ID_MAX*/
#define QSOCK_LOC_RIL_SERVICE_MSG_ID_RANGE_MAX 200
/*Must match with MPSS definition of GNSS_SERVER_SOCK_WWAN_UEB_MSG_ID_RANGE_MIN*/
#define QSOCK_LOC_WWAN_UEB_MSG_ID_RANGE_MIN 201
/*Must match with MPSS definition of GNSS_SERVER_SOCK_WWAN_UEB_MSG_ID_RANGE_MAX*/
#define QSOCK_LOC_WWAN_UEB_MSG_ID_RANGE_MAX 300
/*Must match with MPSS definition of GNSS_SERVER_SOCK_SMCHAL_MSG_ID_MIN*/
#define QSOCK_LOC_SMCHAL_MSG_ID_RANGE_MIN 301
/*Must match with MPSS definition of GNSS_SERVER_SOCK_SMCHAL_MSG_ID_MAX*/
#define QSOCK_LOC_SMCHAL_MSG_ID_RANGE_MAX 400
/*Must match with MPSS definition of GNSS_SERVER_SOCK_WWAN_MEAS_MSG_ID_MIN*/
#define QSOCK_LOC_WWAN_MEAS_MSG_ID_MIN (401)
/*Must match with MPSS definition of GNSS_SERVER_SOCK_WWAN_MEAS_MSG_ID_MAX*/
#define QSOCK_LOC_WWAN_MEAS_MSG_ID_MAX (500)
/*Must match with MPSS definition of GNSS_SERVER_SOCK_GNSS_AIDING_SERVICE_MSG_ID_MIN*/
#define QSOCK_LOC_GNSS_AIDING_MSG_ID_RANGE_MIN (501)
/*Must match with MPSS definition of GNSS_SERVER_SOCK_GNSS_AIDING_SERVICE_MSG_ID_MAX*/
#define QSOCK_LOC_GNSS_AIDING_MSG_ID_RANGE_MAX (600)

#endif // #define LOC_QSOCK_API
