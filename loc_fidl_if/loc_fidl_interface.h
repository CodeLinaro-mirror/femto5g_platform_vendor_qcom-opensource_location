/* Copyright (c) 2011-2014, 2016-2021, The Linux Foundation. All rights reserved.
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

/*
  Changes from Qualcomm Innovation Center, Inc. are provided under the following license:

  Copyright (c) 2022, 2024 Qualcomm Innovation Center, Inc. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted (subject to the limitations in the
  disclaimer below) provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

  * Neither the name of Qualcomm Innovation Center, Inc. nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

   NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
   GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
   HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
   GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
   IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/** @file loc-fidl-interface.h
*/

#ifndef LOC_FIDL_INTERFACE_H
#define LOC_FIDL_INTERFACE_H


/*=============================================================================
 *============================================================================*/
#include <stdbool.h>
#include <stdint.h>
#include "gps_extended_c.h"
#include "LocationDataTypes.h"
#include "LocApiBase.h"
#include "ContextBase.h"
#include "loc_gps.h"
#include "loc_pla.h"
#include "log_util.h"


#define FIDL_LOC_SUPPORTED_FEATURE_LENGTH_V02 (100)

typedef struct {
    LocPositionMode mode;
    LocGpsPositionRecurrence recurrence;
    uint32_t min_interval;
    uint32_t preferred_accuracy;
    uint32_t preferred_time;
    bool share_position;
    char credentials[14];
    char provider[8];
    GnssPowerMode powerMode;
    uint32_t timeBetweenMeasurements;
} fidlPosMode;


/* FIDL Supported Features */
typedef struct {
    /**< Must be set to # of elements in feature */
    uint32_t feature_len;
/**<   This field describes which features are supported in the running
     QMI_LOC service. The array of unit8 is the bitmask where each bit
     represents a feature enum. Bit 0 represents feature enum ID 0,
     bit 1 represents feature enum ID 1, etc.
     For example, if QMI_LOC supports feature enum 0,1,2,8,
     feature_len is 2, and
     feature array is [7,1]. \n
     - Type: Array of uint8
     - Maximum array length: 100
     - enum is "loc_supported_feature_enum" which declared in gps_extended_c.h file.
*/
    uint8_t feature[FIDL_LOC_SUPPORTED_FEATURE_LENGTH_V02];
} fidlFeatureList;


/** FIDL module’s Event Interface.
    These Event shall be implement by FIDL Location API module.
    FIDL module shall be store and call these Event APIs.
 */
typedef struct {
    /**
       Engine Up Event. FIDL shall indicate this event when FIDL engine is ready to accept
       command such as start session, delete assistance.
       FIDL context will be used to process this event and will call Adapter APIs in the
       same context. FIDL need to support multiple execution contexts. The execution context
       will be identify using ‘void *context’ variable.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlHandleEngineUpEvent)(void *context);

    /**
       Engine Down Event. FIDL shall indicate this event when FIDL is not accepting any command.
       The Event processing is done in FIDL context and same context Adapter API will called.
       FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlHandleEngineDownEvent)(void *context);

    /**
       PVT report Event. FIDL shall indicate this event when FIDL receive the Position Info.
       The Event processing is done in FIDL context and same context Adapter API will called.
       FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #UlpLocation \n
       #GpsLocationExtended \n
       #LocPosTechMask \n
       #GnssDataNotification

       @param location[Input]           Ulp Location Report.

       @param locationExtended[Input]   Extended location report.

       @param status                    Session Status.

       @param loc_technology_mask       Location Technology Mask.

       @param pDataNotify               GNSS Data Notification.

       @param msInWeek                  Week in milliseconds.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportPosition)(UlpLocation& location,
                        GpsLocationExtended& locationExtended,
                        enum loc_sess_status status,
                        LocPosTechMask loc_technology_mask,
                        GnssDataNotification* pDataNotify,
                        int msInWeek, void *context);

    /**
       GNSS SV Notification Event. FIDL shall indicate this event when FIDL receive
       Visible SV Info. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssSvNotification \n

       @param GnssSvNotification[Input]  GNSS SV Data.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportSv)(GnssSvNotification& svNotify, void *context);

    /**
       GNSS SV Measurement Event. FIDL shall indicate this event when FIDL receive
       SV Measurement. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssSvMeasurementSet \n

       @param GnssSvMeasurementSet[Input]  GNSS SV Measurement Data.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportSvMeasurement)(GnssMeasurements &svMeasurementSet, void *context);

    /**
       GNSS SV Polynomial Event. FIDL shall indicate this event when FIDL receive
       SV Polynomial. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssSvPolynomial \n

       @param svPolynomial[Input]  GNSS SV Polynomial Data.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportSvPolynomial)(GnssSvPolynomial &svPolynomial, void *context);

    /**
       GNSS SV Ephemeris Event. FIDL shall indicate this event when FIDL receive
       SV Ephemeris. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssSvEphemerisReport \n

       @param svEphemeris[Input]  GNSS SV Ephemeris Report.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportSvEphemeris)(GnssSvEphemerisReport &svEphemeris, void *context);

    /**
       GNSS Status Value Event. FIDL shall indicate this event when FIDL receive
       Gps Status Value. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #LocGpsStatusValue \n

       @param LocGpsStatusValue[Input]  GPS Status Value.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportStatus)(LocGpsStatusValue status, void *context);

    /**
       Report NMEA SentencesEvent. FIDL shall indicate this event when FIDL receive
       NMEA report. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @param nmea[Input]  NMEA Sentence.

       @param length[Input]  Length of NMEA Sentence.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportNmea)(const char* nmea, int length, void *context);

    /**
       GNSS Data NotificationEvent. FIDL shall indicate this event when FIDL report
       GNSS Data Notification. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssDataNotification

       @param dataNotify[Input]  GNSS Data Notification.

       @param msInWeek[Input]    Week in milliseconds.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportData)(GnssDataNotification& dataNotify, int msInWeek, void *context);

    /**
       XTRA Server details Event. FIDL shall indicate this event when FIDL Report
        Xtra Server. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @param url1[Input]     XTRA Server URL -1.

       @param url2[Input]     XTRA Server URL -2.

       @param url3[Input]     XTRA Server URL -3.

       @param maxlength[Input]  Max length of URLs.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportXtraServer)(const char* url1, const char* url2,
            const char* url3, const int maxlength, void *context);

    /**
       Report Location System Info Event. FIDL shall indicate this event when FIDL request
       Location System Info. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #LocationSystemInfo

       @param locationSystemInfo[Input]  location System Info.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportLocationSystemInfo)(const LocationSystemInfo& locationSystemInfo,
            void *context);

    /**
       Request XTRA DataEvent. FIDL shall indicate this event when FIDL request XTRA
       The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlRequestXtraData)(void *context);

    /**
       Request Time infoEvent. FIDL shall indicate this event when FIDL request time.
       The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlRequestTime)(void *context);

    /**
       Request Location info Event. FIDL shall indicate this event when FIDL request
       Location. The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlRequestLocation)(void *context);

    /**
       Request for ATL Event. FIDL shall indicate this event when FIDL request for
       ATL. The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #LocAGpsType \n
       #LocApnTypeMask

       @param connHandle[Input]   ATL Connection handle.

       @param agps_type[Input]    AGPS Type.

       @param apn_type_mask[Input] APN Type Mask.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlRequestATL)(int connHandle, LocAGpsType agps_type, LocApnTypeMask apn_type_mask,
            void *context);

    /**
       Request for release ATL Event. FIDL shall indicate this event when FIDL release
       ATL. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
        #LocAGpsType \n
        #LocApnTypeMask

       @param connHandle[Input]   ATL Connection handle.


       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReleaseATL)(int connHandle, void *context);

    /**
       NI Notification Event. FIDL shall indicate this event when FIDL receive NI Notification
       The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssNiNotification

       @param notify[Input]   GNSS Ni Notification.

       @param data[Input]     Ni Data Pointer.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlRequestNiNotify)(GnssNiNotification &notify, const void* data, void *context);

    /**
       Report GNSS Measurement Data Event. FIDL shall indicate this event when FIDL receive
       GNSS Measuement Data. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssMeasurementsNotification

       @param measurements[Input]   GNSS measurements Data.

       @param msInWeek[Input]     Week in milliseconds.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.

       @Note
           This API do not have intra-constellation information.
    */
    void (*fidlReportGnssMeasurementData)(GnssMeasurements& measurements,
            int msInWeek, void *context);
    /**
       WWAN ZPP Fix Event. FIDL shall indicate this event when FIDL receive WWAN ZPP fix
       The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #LocGpsLocation

       @param zppLoc[Input]    ZPP PVT report.

       @param msInWeek[Input]     Week in milliseconds.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportWwanZppFix)(LocGpsLocation &zppLoc, void *context);

    /**
       ZPP Best Available Fix Event. FIDL shall indicate this event when FIDL receive ZPP fix.
       The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #LocGpsLocation \n
       #GpsLocationExtended \n
       #LocPosTechMask

       @param zppLoc[Input]     ZPP PVT report.

       @param location_extended[Input]  Extended Location PVT report.

       @param tech_mask[Input]  Tech Mask.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportZppBestAvailableFix) (LocGpsLocation &zppLoc,
            GpsLocationExtended &location_extended, LocPosTechMask tech_mask, void *context);

    /**
       Report GNSS SV ID Configure Event. FIDL shall indicate this event when FIDL
       Gnss SvId Config. This interface API should be called by FIDL.
       The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssSvIdConfig

       @param config[Input]     GNSS SV ID configuration.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportGnssSvIdConfig)(const GnssSvIdConfig& config, void *context);

    /**
       Report GNSS SV Type Configure Event. FIDL shall indicate this event when FIDL
       Gnss Sv Type Config request. This interface API should be called by FIDL.
       The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssSvTypeConfig

       @param config[Input]     GNSS SV Type configuration.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportGnssSvTypeConfig)(const GnssSvTypeConfig& config, void *context);

    /**
       Request for ODCPI Event. FIDL shall indicate this event when FIDL needs ODCPI.
       The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #OdcpiRequestInfo

       @param request[Input]    Request for ODCPI.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlRequestOdcpi)(OdcpiRequestInfo& request, void *context);

    /**
       Report GNSS Engine Energy Consumed Event. FIDL shall indicate this event when FIDL
       reports  energy Consumed Since FirstBoot. The Event processing is done in FIDL context
       and same context Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @param energyConsumedSinceFirstBoot[Input] Energy Consumed
                   Since from first Boot.

       @param context[Input]   Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportGnssEngEnergyConsumedEvent)(uint64_t energyConsumedSinceFirstBoot,
            void *context);

    /**
       Deleted Aiding Data Event. FIDL shall indicate this event when FIDL request
       Gnss Aiding Data. The Event processing is done in FIDL context and same context
       Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssAidingData

       @param aidingData[Input] Deleted Aiding Data.

       @param context[Input]   Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportDeleteAidingDataEvent)(GnssAidingData& aidingData, void *context);

    /**
       GNSS Klobuchar Iono Model Event. FIDL shall indicate this event when FIDL
       provides Gnss Klobuchar Iono Model. The Event processing is done in FIDL context
       and same context Adapter API will called. FIDL need to support multiple execution
       contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssKlobucharIonoModel

       @param ionoModel[Input] GNSS Klobuchar Iono Model.

       @param context[Input]   Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportKlobucharIonoModel)(GnssKlobucharIonoModel& ionoModel, void *context);

    /**
       GNSS Additional System Info Event. FIDL shall indicate this event when FIDL provides
       Gnss Additional System Info. The Event processing is done in FIDL context and same
       context Adapter API will called. FIDL need to support multiple execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssAdditionalSystemInfo

       @param additionalSystemInfo[Input] Additional System Info.

       @param context[Input]   Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*fidlReportGnssAdditionalSystemInfo)(GnssAdditionalSystemInfo& additionalSystemInfo,
            void *context);

    void (*fidlSendNfwNotification)(GnssNfwNotification& notification,
            void *context);
    void (*fidlReportGnssConfig)(uint32_t sessionId, const GnssConfig& gnssConfig, void *context);
    void (*fidlReportLatencyInfo)(GnssLatencyInfo& gnssLatencyInfo, void *context);
    void (*fidlReportQwesCapabilities)(const std::unordered_map<LocationQwesFeatureType, bool> &featureMap,
            void *context);
    void (*fidlGeofenceBreach)(size_t count, uint32_t* hwIds, Location& location,
            GeofenceBreachType breachType, uint64_t timestamp, void *context);
    void (*fidlGeofenceStatus)(GeofenceStatusAvailable available, void *context);
    void (*fidlReportDBTPosition)(UlpLocation &location,
                           GpsLocationExtended &locationExtended,
                           enum loc_sess_status status,
                           LocPosTechMask loc_technology_mask,
                           void *context);
    void (*fidlReportLocations)(Location* locations, size_t count, BatchingMode batchingMode, void *context);
    void (*fidlReportCompletedTrips)(uint32_t accumulated_distance, void *context);
    void (*fidlHandleBatchStatusEvent)(BatchingStatus batchStatus, void *context);
} FidlInterfaceEvent;

/** FIDL module’s Interface commands.
    These commands should be implemented by by FIDL module.
    FIDL Location API module should store and call these command APIs.
 */
typedef struct {
   /**
       The FIDL Open interface function request to open/active device.
       This interface API should be called by FIDL LOC API.
       This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #LOC_API_ADAPTER_EVENT_MASK_T \n
       #loc_api_adapter_err

       @param requestedMask [Input]
                                This input variable indicates the requested
                                configuration from the GNSS hardware.

       @param isMaster [Input]   'isMaster' indicates as the master session.

       @param supportedMask[out] The GNSS HW provides its supported configuration.

       @param fidlFeatures[out]   The GNSS HW provides its supported feature list.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlOpen)(uint64_t requestedMask, bool isMaster,
                uint64_t *supportedMask, fidlFeatureList * fidlFeatures,
                void *context);

   /**
       The FIDL Close interface function request to close/de-active device.
       This interface API should be called by FIDL LOC API.
       This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlClose)(void *context);

   /**
       The FIDL Start fix interface function request to start a session of FIDL.
       This interface API should be called by FIDL LOC API.
       This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param fidlPosMode[Input] Provide Session parameters to a start session.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlStartFix)(fidlPosMode& fixCriteria, void *context);
   /**
       The FIDL Stop fix interface function request to stop a ongoing session of FIDL.
       This interface API should be called by FIDL LOC API.
       This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlStopFix)(void *context);

   /**
       The FIDL Delete Aiding Data interface function request delete assistance data of FIDL.
       This interface API should be called by FIDL LOC API.
       This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param GnssAidingData[Input] Provide Session parameters to a start session.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlDeleteAidingData)(const GnssAidingData& data, void *context);

   /**
       The interface function to inject Position to FIDL. This interface API should be called by
       FIDL LOC API. This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssLocationInfoNotification
       #loc_api_adapter_err

       @param locationInfo[Input] Position to inject.

       @param onDemandCpi[Input]  Flag to indicate on demand.

       @param context[Input]      Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlInjectPosition)(const GnssLocationInfoNotification &locationInfo,
            bool onDemandCpi, void *context);

   /**
       The interface function to set time of FIDL. This interface API should be called by
       FIDL LOC API. This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #LocGpsUtcTime
       #loc_api_adapter_err

       @param time[Input]           Time to inject.

       @param timeReference[Input]  Indicate time reference.

       @param uncertainty[Input]    Time uncertainty.

       @param onDemandCpi[Input]    Flag to indicate on demand.

       @param context[Input]        Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetTime)(LocGpsUtcTime time, int64_t timeReference,
            int uncertainty, void *context);

   /**
       The interface function to inject XTRA Data to FIDL. This interface API should be called by
       FIDL LOC API. This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param data[Input]     XTRA data.

       @param length[Input]   Indicate length of XTRA data.

       @param context[Input]  Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                              The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                              is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetXtraData)(char* data, int length, void *context);

   /**
       The interface function to request for XTRA Server to FIDL. This interface API should be called by
       FIDL LOC API. This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
    #loc_api_adapter_err

       @param context[Input]  Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                              The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                              is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlRequestXtraServer)(void *context);


   /**
       The interface function to indicate status of open ATL connection.
       This interface API should be called by FIDL LOC API. This API will be called multiple
       times in different execution contexts. The execution context will be identify using
       ‘void *context’ variable.

       @datatypes
       #AGpsBearerType
       #LocAGpsType
       #LocApnTypeMask
       #loc_api_adapter_err

       @param handle[Input]    handle of ATL connection status.

       @param is_succ[Input]   Indicate status of ATL Connection.

       @param apn[Input]       Provide APN details.

       @param apnLen[Input]    Length of APN data.

       @param bear[Input]      Indicate bearer type.

       @param agpsType[Input]  Indicate AGPS type.

       @param mask[Input]      Indicate APN type.

       @param context[Input]   Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlAtlOpenStatus)(int handle, int is_succ, char* apn,
            uint32_t apnLen, AGpsBearerType bear, LocAGpsType agpsType, LocApnTypeMask mask,
            void *context);

   /**
       The interface function to indicate status of close ATL connection.
       This interface API should be called by FIDL LOC API. This API will be called multiple
       times in different execution contexts. The execution context will be identify using
       ‘void *context’ variable.

       @datatypes
       #AGpsBearerType
       #LocAGpsType
       #LocApnTypeMask
       #loc_api_adapter_err

       @param handle[Input]    handle of ATL connection status.

       @param is_succ[Input]   Indicate status of ATL Connection.

       @param context[Input]   Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlAtlCloseStatus)(int handle, int is_succ, void *context);

   /**
       The FIDL Set Position Mode interface function to update session parameters of
       ongoing session or upcoming session. This interface API should be called by FIDL LOC API.
       This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param fidlPosMode[Input] Provide Session parameters to a start session.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetPositionMode)(fidlPosMode& posMode, void *context);

   /**
       The FIDL interface function to Set Sync Server URL. This interface API should be call
       by FIDL LOC API. This API will be called multiple times in different execution
       contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #LocServerType
       #loc_api_adapter_err

       @param url[Input]        Indicate URL.

       @param len[Input]        Indicate URL length.

       @param type[Input]       Indicate Location Server Type.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetServerSync)(const char* url, int len, LocServerType type,
            void *context);

   /**
       The FIDL interface function to inform NI Response. This interface API should be call
       by FIDL LOC API. This API will be called multiple times in different execution
       contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssNiResponse
       #loc_api_adapter_err

       @param userResponse[Input]     Ni User Response.

       @param passThroughData[Input]  Pass Through Data.

       @param context[Input]          Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlInformNiResponse)(GnssNiResponse userResponse,
            const void* passThroughData, void *context);

   /**
       The FIDL interface function to configure SUPL version. This interface API should be call
       by FIDL LOC API. This API will be called multiple times in different execution
       contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssConfigSuplVersion
       #loc_api_adapter_err

       @param version[Input]    Indicate SUPL version.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetSUPLVersionSync)(GnssConfigSuplVersion version, void *context);

   /**
       The FIDL interface function to configure NMEA Type. This interface API should be call
       by FIDL LOC API. This API will be called multiple times in different execution
       contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssConfigSuplVersion
       #loc_api_adapter_err

       @param typesMask[Input]  Configure of Type Mask.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetNMEATypesSync)(uint32_t typesMask, void *context);

   /**
       The FIDL interface function to configure LPP sync. This interface API should be called by
       FIDL LOC API. This API will be called multiple times in different execution
       contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssConfigLppProfileMask
       #loc_api_adapter_err

       @param profile[Input]    Configure of LPP Profile.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetLPPConfigSync)(GnssConfigLppProfileMask profileMask,
           void *context);

   /**
       The FIDL interface function to configure Sensor Properties. This interface API should
       called by FIDL LOC API. This API will be called multiple times in different execution
       contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param gyroBiasVarianceRandomWalk_valid[Input] Flag to indicate Gyro Bias.

       @param gyroBiasVarianceRandomWalk[Input] Gyro Bias.

       @param accelBiasVarianceRandomWalk_valid[Input] Flag to indicate ACCEL Bias.

       @param accelBiasVarianceRandomWalk[Input] ACCEL Bias.

       @param angleBiasVarianceRandomWalk_valid[Input] Flag to indicateAngle Bias.

       @param angleBiasVarianceRandomWalk[Input] Angle Bias.

       @param rateBiasVarianceRandomWalk_valid[Input] Flag to indicate Angle Bias.

       @param rateBiasVarianceRandomWalk[Input] Rate Bias.

       @param velocityBiasVarianceRandomWalk_valid[Input] Flag to indicate Velocity Bias.

       @param velocityBiasVarianceRandomWalk[Input] Velocity Bias.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetSensorPropertiesSync)
                           (bool gyroBiasVarianceRandomWalk_valid,
                            float gyroBiasVarianceRandomWalk,
                            bool accelBiasVarianceRandomWalk_valid,
                            float accelBiasVarianceRandomWalk,
                            bool angleBiasVarianceRandomWalk_valid,
                            float angleBiasVarianceRandomWalk,
                            bool rateBiasVarianceRandomWalk_valid,
                            float rateBiasVarianceRandomWalk,
                            bool velocityBiasVarianceRandomWalk_valid,
                            float velocityBiasVarianceRandomWalk, void *context);
   /**
       The FIDL interface function to configure Set Sensor Preference Control.
       This interface API should be called by FIDL LOC API. This API will be called
       multiple times in different execution contexts. The execution context will
       identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param accelSamplesPerBatch[Input] ACCEL Samples per batch.

       @param accelBatchesPerSec[Input]   ACCEL Batches per seconds.

       @param gyroSamplesPerBatch[Input]  Gyro Samples per batch.

       @param gyroBatchesPerSec[Input]    Gyro Batches per seconds.

       @param accelSamplesPerBatchHigh[Input] ACCEL Sample per batch High.

       @param accelBatchesPerSecHigh[Input]   ACCEL batches per Seconds High.

       @param gyroSamplesPerBatchHigh[Input]  Gyro Samples per Batch High.

       @param gyroBatchesPerSecHigh[Input]    Gyro Batch Per Sec High.

       @param algorithmConfig[Input] Algorithm Configure.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetSensorPerfControlConfigSync)(int controlMode,
                            int accelSamplesPerBatch,
                            int accelBatchesPerSec,
                            int gyroSamplesPerBatch,
                            int gyroBatchesPerSec,
                            int accelSamplesPerBatchHigh,
                            int accelBatchesPerSecHigh,
                            int gyroSamplesPerBatchHigh,
                            int gyroBatchesPerSecHigh,
                            int algorithmConfig, void *context);
   /**
       The FIDL interface function to configure AGLONASS Protocol Sync. This interface
       API should be called by FIDL LOC API. This API will be called multiple times in
       different execution contexts. The execution context will be identify using
       ‘void *context’ variable.

       @datatypes
       #GnssConfigAGlonassPositionProtocolMask
       #loc_api_adapter_err

       @param aGlonassProtocol[Input]    Configure AGLONASS Protocol sync.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetAGLONASSProtocolSync)
            (GnssConfigAGlonassPositionProtocolMask aGlonassProtocol, void *context);

   /**
       The FIDL interface function to Set LPP protocol Control Plan Sync Mask. This interface API
       should be called by FIDL LOC API. This API will be called multiple times in different
       execution contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssConfigLppeControlPlaneMask
       #loc_api_adapter_err

       @param lppeCP[Input]     Configure of LPP protocol of Control Plan Mask.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetLPPeProtocolCpSync)(GnssConfigLppeControlPlaneMask lppeCP,
            void *context);

   /**
       The FIDL interface function to Set LPP protocol User Plan Sync Mask. This interface API should
       called by FIDL LOC API. This API will be called multiple times in different
       execution contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #GnssConfigLppeUserPlaneMask
       #loc_api_adapter_err

       @param profile[Input]    Configure of LPP User Plan Mask Profile.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetLPPeProtocolUpSync)(GnssConfigLppeUserPlaneMask lppeUP,
            void *context);

   /**
       The FIDL interface function to get GNSS SUPL Version. This interface API should be called by
       FIDL LOC API. This API will be called multiple times in different execution contexts.
       The execution context will be identify using ‘void *context’ variable.
       This is blocking call.

       @datatypes
       #GnssConfigSuplVersion
       #loc_api_adapter_err

       @param suplVersion[Input] SUPL version.

       @param suplConfig[Output] SUPL Version configuration.

       @param context[Input]     Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlConvertSuplVersion)(const uint32_t suplVersion,
            GnssConfigSuplVersion *suplConfig, void *context);

   /**
       The FIDL interface function to get GNSS LPP profile. This interface API should
       called by FIDL LOC API. This API will be called multiple times in different
       execution contexts. The execution context will be identify using ‘void *context’ variable.
       This is blocking call.

       @datatypes
       #GnssConfigLppProfileMask
       #loc_api_adapter_err

       @param lppProfile[Input] LPP Profile.

       @param gnssLppProfile[Output] GNSS LPP Profile configuration.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlConvertLppProfile)(const uint32_t lppProfile,
            GnssConfigLppProfileMask *gnssLppProfileMask, void *context);

   /**
       The FIDL interface function to get GNSS LPP Control Plan Mask. This interface API should
       called by FIDL LOC API. This API will be called multiple times in different
       execution contexts. The execution context will be identify using ‘void *context’ variable.
       This is blocking call.

       @datatypes
       #GnssConfigLppeControlPlaneMask
       #loc_api_adapter_err

       @param lppeControlPlaneMask[Input] LPP Control Plan Mask.

       @param lppControlPlanMask[Output] GNSS LPP Control Plan Mask configuration.

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlConvertLppeCp)(const uint32_t lppeControlPlaneMask,
            GnssConfigLppeControlPlaneMask *gnssLppControlPlanMask, void *context);

   /**
       The FIDL interface function to get GNSS LPP User Plan Mask. This interface API should
       called by FIDL LOC API. This API will be called multiple times in different
       execution contexts. The execution context will be identify using ‘void *context’ variable.
       This is blocking call.

       @datatypes
       #GnssConfigLppeUserPlaneMask
       #loc_api_adapter_err

       @param lppeUserPlaneMask[Input] LPP User Plan Mask.

       @param lppControlPlanMask[Output] GNSS LPP User Plan Mask configuration.

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlConvertLppeUp)(const uint32_t lppeUserPlaneMask,
            GnssConfigLppeUserPlaneMask *gnssLppUpMask, void *context);

   LocationError (*fidlSetEmergencyExtensionWindowSync)(const uint32_t emergencyExtensionSeconds,
			void *context);
   void (*fidlSetMeasurementCorrections)(
            const GnssMeasurementCorrections& gnssMeasurementCorrections, void *context);
   /**
       The FIDL interface function to request for WWAN ZPP Fix. This interface API should
       called by FIDL LOC API. This API will be called multiple times in different
       execution contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlGetWwanZppFix)(void *context);

   /**
       The FIDL interface function to request for  Best Available ZPP Fix. This interface
       API should be called by FIDL LOC API. This API will be called multiple times in different
       execution contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlGetBestAvailableZppFix)(void *context);

   /**
       The FIDL interface to request GPS Lock incase of SUPL when GNSS disabled. This interface
       API should be called by FIDL LOC API. This API will be called multiple times in different
       execution contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
        #GnssConfigGpsLock
        #loc_api_adapter_err

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetGpsLockSync)(GnssConfigGpsLock lock, void *context);

   /**
       The FIDL interface to request For Aiding Data. This interface API should be called by
       FIDL LOC API. This API will be called multiple times in different execution
       contexts.The execution context will be identify using ‘void *context’ variable.

       @datatypes
        #GnssAidingDataSvMask
        #loc_api_adapter_err

       @GnssConfigGpsLock[Input]  GNSS Aiding Data SV Mask.

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlRequestForAidingData)(GnssAidingDataSvMask svDataMask,
            void *context);

   /**
       The FIDL interface to Get GPS Lock incase of SUPL. This interface API should be call
       by FIDL LOC API. This API will be called multiple times in different execution
       contexts. The execution context will be identify using 'void *context' variable.

       @datatypes
        #loc_api_adapter_err

       @param subType[Input]    Subscription that is associated with the engine lock state
                                being requested

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
           loc_api_adapter_err[Output]
                            The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                            is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlGetGpsLock)(uint8_t subType, void *context);


   /**
       The FIDL interface to Set XTRA Veresion Check. This interface API should be call
       by FIDL LOC API. This API will be called multiple times in different execution
       contexts. The execution context will be identify using 'void *context' variable.

       @datatypes
          #loc_api_adapter_err

       @param check[Input]      Check XTRA File Version.

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
           loc_api_adapter_err[Output]
                            The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                            is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlSetXtraVersionCheckSync)(uint32_t check, void *context);


   /**
       The FIDL interface function to Install AGPS Certificate. This interface API should be call
       by FIDL LOC API. This API will be called multiple times in different execution
       contexts. The execution context will be identify using ‘void *context’ variable.

       @datatypes
       #LocDerEncodedCertificate
       #loc_api_adapter_err

       @param pData[Input]      Encoded AGPS Certificate.

       @param length[Input]     Length of AGPS Certificate.

       @param slotBitMask[Input] Slot Bit mask.

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
           loc_api_adapter_err[Output]
                                The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                is successfully received and other status indicate as failure.

       @dependencies
           None.
   */
   enum loc_api_adapter_err (*fidlInstallAGpsCert)(const LocDerEncodedCertificate* pData,
                                 size_t length,
                                 uint32_t slotBitMask, void *context);

    /**
        The FIDL interface function to set constraint for time uncertainty and energy Budget.
        This interface API should be called by FIDL LOC API. This API will be called multiple
        times in different execution contexts. The execution context will be identify using
        'void *context' variable.

        @datatypes
        #loc_api_adapter_err

        @param enabled[Input]           Enabled or Disabled Constrained.

        @param tuncConstraint[Input]    If constraint is enabled, the engine maintains its
                                        time uncertainty below the specified constraint in
                                        tuncConstraint. The units are in ms.

        @param energyBudget[Input]      If constraint is enabled, and if energyBudget is
                                        specified, the engine uses this as the maximum energy
                                        to be used while keeping the engine in constrained tunc
                                        mode. Units: 0.1 milliwatt second.

        @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                 and use any future interaction with FIDL location API.

        @return
            loc_api_adapter_err[Output]
                                 The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                 is successfully received and other status indicate as failure.

        @dependencies
            None.
    */
    enum loc_api_adapter_err (*fidlSetConstrainedTuncMode)(bool enabled, float tuncConstraint,
                                  uint32_t energyBudget, void *context);

    /**
        The FIDL interface function to enable position assisted clock estimation mode.
        This interface API should be called by FIDL LOC API. This API will be called multiple
        times in different execution contexts. The execution context will be identify using
        'void *context' variable.

        @datatypes
        #loc_api_adapter_err

        @param enabled[Input]    Enabled or Disabled position assisted clock estimation mode.

        @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                 and use any future interaction with FIDL location API.

        @return
            loc_api_adapter_err[Output]
                                 The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                 is successfully received and other status indicate as failure.

        @dependencies
            None.
    */
    enum loc_api_adapter_err (*fidlSetPositionAssistedClockEstimatorMode)(bool enabled,
                                 void *context);

    /**
        The FIDL interface function to get GNSS Energy Consumed. This interface API should be call
        by FIDL LOC API. This API will be called multiple times in different execution
        contexts. The execution context will be identify using 'void *context' variable.

        @datatypes
        #loc_api_adapter_err

        @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                 and use any future interaction with FIDL location API.

        @return
            loc_api_adapter_err[Output]
                                 The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                 is successfully received and other status indicate as failure.

        @dependencies
            None.
    */
    enum loc_api_adapter_err (*fidlGetGnssEnergyConsumed)(void *context);

    /**
        This API used to indicate Blacklisted SV to the GNSS hardware.
        GNSS hardware can avoid these blacklisted SV in position report calculation.
        This interface API should be called by FIDL LOC API.
        This API will be called multiple times in different execution contexts.
        The execution context will be identify using ‘void *context’ variable.

        @datatypes
            #loc_api_adapter_err

        @param GnssSvIdConfig[Input] GNSS SV ID Configuration.

        @param context[Input]    Context Pointer of FIDL Location API.

        @return
            loc_api_adapter_err[Output]
                                 The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                 is successfully received and other status indicate as failure.

        @dependencies
            None.
    */
    enum loc_api_adapter_err (*fidlSetBlacklistSv)(const GnssSvIdConfig& config, void *context);

    /**
        This API used to request to get Blacklisted SV from GNSS hardware.
        GNSS hardware indicate blacklisted SV using fidlReportGnssSvIdConfig event.
        This interface API should be called by FIDL LOC API.
        This API will be called multiple times in different execution contexts.
        The execution context will be identify using ‘void *context’ variable.

        @datatypes
        #loc_api_adapter_err

        @param context[Input]    Context Pointer of FIDL Location API.

        @return
            loc_api_adapter_err[Output]
                                 The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                 is successfully received and other status indicate as failure.

        @dependencies
            None.
    */
    enum loc_api_adapter_err (*fidlGetBlacklistSv)(void *context);

    /**
        This API used to request to set supported constellation to GNSS hardware.
        This interface API should be called by FIDL LOC API.
        This API will be called multiple times in different execution contexts.
        The execution context will be identify using ‘void *context’ variable.

        @datatypes
            #loc_api_adapter_err

        @param context[Input]    Context Pointer of FIDL Location API.

        @return
            loc_api_adapter_err[Output]
                                 The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                 is successfully received and other status indicate as failure.

        @dependencies
            None.
    */
    enum loc_api_adapter_err (*fidlSetConstellationControl)(const GnssSvTypeConfig& config,
            void *context);

    /**
        This API used to request to get supported constellation from GNSS hardware.
        GNSS hardware indicate supported constellation using fidlReportGnssSvTypeConfig event.
        This interface API should be called by FIDL LOC API.
        This API will be called multiple times in different execution contexts.
        The execution context will be identify using ‘void *context’ variable.

        @datatypes
    #loc_api_adapter_err

        @param context[Input]    Context Pointer of FIDL Location API.

        @return
            loc_api_adapter_err[Output]
                                 The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                 is successfully received and other status indicate as failure.

        @dependencies
            None.
    */
    enum loc_api_adapter_err (*fidlGetConstellationControl)(void *context);

    /**
        This API used to request to reset supported constellation from GNSS hardware.
        This interface API should be called by FIDL LOC API.
        This API will be called multiple times in different execution contexts.
        The execution context will be identify using ‘void *context’ variable.

        @datatypes
            #loc_api_adapter_err

        @param context[Input]    Context Pointer of FIDL Location API.

        @return
            loc_api_adapter_err[Output]
                                 The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                 is successfully received and other status indicate as failure.

        @dependencies
            None.
    */
    enum loc_api_adapter_err (*fidlResetConstellationControl)(void *context);


    /**
        This API used to request to XTRA configuration to GNSS hardware.
        This interface API should be called by FIDL LOC API.
        This API will be called multiple times in different execution contexts.
        The execution context will be identify using ‘void *context’ variable.

        @datatypes
              #loc_api_adapter_err

        @param configMask[Input]      Configuration Mask.
        @param configReqSource[Input] Configuration Request Source.
        @param context[Input]         Context Pointer of FIDL Location API.

        @return
            loc_api_adapter_err[Output]
                                 The return status LOC_API_ADAPTER_ERR_SUCCESS indicate command
                                 is successfully received and other status indicate as failure.

        @dependencies
            None.
    */
    enum loc_api_adapter_err (*fidlRequestXtraConfigInfo)(const uint32_t configMask,
                                             const uint32_t configReqSource, void *context);

    void (*fidlAddGeofence)(uint32_t clientId, const GeofenceOption& options,
        const GeofenceInfo& info, void *context);
    void (*fidlRemoveGeofence)(uint32_t hwId, uint32_t clientId, void *context);
    void (*fidlPauseGeofence)(uint32_t hwId, uint32_t clientId, void *context);
    void (*fidlResumeGeofence)(uint32_t hwId, uint32_t clientId, void *context);
    void (*fidlModifyGeofence)(uint32_t hwId, uint32_t clientId, const GeofenceOption& options,
        void *context);

    void (*fidlStartTimeBasedTracking)(const TrackingOptions& options,
        void *context);
    void (*fidlStopTimeBasedTracking)(void *context);
    void (*fidlStartDistanceBasedTracking)(uint32_t sessionId, const LocationOptions& options,
        void *context);
    void (*fidlStopDistanceBasedTracking)(uint32_t sessionId,
        void *context);
    void (*fidlStartBatching)(uint32_t sessionId, const LocationOptions& options,
        uint32_t accuracy, uint32_t timeout, void *context);
    void (*fidlStopBatching)(uint32_t sessionId, void *context);
    LocationError (*fidlStartOutdoorTripBatchingSync)(uint32_t tripDistance,
        uint32_t tripTbf, uint32_t timeout, void *context);
    void (*fidlStartOutdoorTripBatching)(uint32_t tripDistance,
        uint32_t tripTbf, uint32_t timeout, void *context);
    void (*fidlReStartOutdoorTripBatching)(uint32_t ongoingTripDistance,
        uint32_t ongoingTripInterval, uint32_t batchingTimeout, void *context);
    LocationError (*fidlStopOutdoorTripBatchingSync)(bool deallocBatchBuffer, void *context);
    void (*fidlStopOutdoorTripBatching)(bool deallocBatchBuffer, void *context);
    LocationError (*fidlGetBatchedLocationsSync)(size_t count, void *context);
    void (*fidlGetBatchedLocations)(size_t count, void *context);
    LocationError (*fidlGetBatchedTripLocationsSync)(size_t count, uint32_t accumulatedDistance,
		void *context);
    void (*fidlGetBatchedTripLocations)(size_t count, uint32_t accumulatedDistance,
        void *context);
    LocationError (*fidlQueryAccumulatedTripDistanceSync)(uint32_t &accumulated_trip_distance,
        uint32_t &numOfBatchedPositions, void *context);
    void (*fidlQueryAccumulatedTripDistance)(void *context);
    void (*fidlSetBatchSize)(size_t size, void *context);
    void (*fidlSetTripBatchSize)(size_t size, void *context);
    void (*fidlAddToCallQueue)(void *context);

    void (*fidlUpdateEvtMask)(void *context);
    void (*fidlUpdateNmeaMask)(uint32_t mask, void *context);

    void (*fidlUpdateSystemPowerState)(PowerStateType systemPowerState, void *context);

    void (*fidlConfigRobustLocation)(bool enable, bool enableForE911,
        void *context);
    void (*fidlGetRobustLocationConfig)(uint32_t sessionId,
		void *context);
    void (*fidlConfigMinGpsWeek)(uint16_t minGpsWeek,
		void *context);
    void (*fidlGetMinGpsWeek)(uint32_t sessionId,
		void *context);

    LocationError (*fidlSetParameterSync)(const GnssConfig & gnssConfig, void *context);
    void (*fidlGetParameter)(uint32_t sessionId, GnssConfigFlagsMask flags,
		void *context);

    void (*fidlConfigConstellationMultiBand)(const GnssSvTypeConfig& secondaryBandConfig,
		void *context);
    void (*fidlGetConstellationMultiBandConfig)(uint32_t sessionId,
		void *context);

} FidlInterfaceReq;


#ifdef __cplusplus
extern "C" {
#endif
    /**
      The FIDL location layer(FIDL) interface function provide commands and events callbacks to
      interact with FIDL module.
      This interface API should be called by FIDL LOC API module and it should be implemented by by FIDL.
      This API will be called multiple times in different execution contexts.
      The execution context will be identify using ‘void *context’ variable.

      @datatypes
      #FidlInterfaceReq \n
      #FidlInterfaceEvent

      @param eventCallback[Input]   These are event callbacks of FIDL module. These events should
                                    implement by FIDL Location API.
                                    FIDL module should store and call these event APIs.
      @param context[Input]         Context Pointer of FIDL Location API. FIDL should store and
                                    use any future interaction with FIDL location API.

      @return
           FidlInterfaceReq[Output]  These are Interface commands of FIDL module. These commands
                                    should be implemented by by FIDL module.
                                    FIDL Location API module should store and call these
                                    command APIs.

      @dependencies
      None.
    */
    const FidlInterfaceReq* get_fidl_if_api(const FidlInterfaceEvent* eventCallback, void *context);

    /**
      The FIDL location layer(FIDL) interface function pointer provide commands and events
      callbacks to interact with FIDL module. This interface API should be called by FIDL LOC
      API module, and it should be implemented by by FIDL.
      This API will be called multiple times in different execution contexts.
      The execution context will be identify using ‘void *context’ variable.

      @datatypes
      #FidlInterfaceReq \n
      #FidlInterfaceEvent

      @param eventCallback[Input]   These are event callbacks of FIDL module. These events
                                    should be implemented by by FIDL Location API.
                                    FIDL module should store and call these event APIs.
      @param context[Input]         Context Pointer of FIDL Location API. FIDL should store
                                    and use any future interaction with FIDL location API.

      @return
           FidlInterfaceReq[Output]  These are Interface commands of FIDL module. These commands
                                    should be implemented by by FIDL module.
                                    FIDL Location API module should store and call these
                                    command APIs.

      @dependencies
           None.
    */
    typedef const FidlInterfaceReq* (*get_fidl_if_api_t)
                                       (const FidlInterfaceEvent* eventCallback, void *context);

#ifdef __cplusplus
}
#endif

#endif /* LOC_FIDL_INTERFACE_H */
