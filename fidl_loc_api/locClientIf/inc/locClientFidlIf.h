/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#ifndef LOC_CLIENT_FIDL_INTERFACE_H
#define LOC_CLIENT_FIDL_INTERFACE_H


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

#define FIDL_HARDWARE_KNOW (0)
#define FIDL_HARDWARE_UP   (1)
#define FIDL_HARDWARE_DOWN (2)

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
} locClientFidlPosMode;


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
} locClientFidlFeatureList;


/** LocClient FIDL module’s Event Interface.
    These Event shall be implement by LocClient FIDL API module.
    This module shall be store and call these Event APIs.
 */
typedef struct {

    /**
       The FIDL event to indicate as PVT report. This interface API should be call by
       Loc Client FIDL. The Event processing is done in FIDL context and FIDL need to
       support multiple execution contexts.
       The execution context will identify using ‘void *context’ variable.

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
    void (*locClientFidlReportPosition)(UlpLocation& location,
                        GpsLocationExtended& locationExtended,
                        enum loc_sess_status status,
                        LocPosTechMask loc_technology_mask,
                        GnssDataNotification* pDataNotify,
                        int msInWeek, void *context);

    /**
       The FIDL event to indicate as GNSS SV Notification. This interface API should
       call by Loc Client FIDL. The Event processing is done in FIDL context and
       FIDL need to support multiple execution contexts.
       The execution context will identify using ‘void *context’ variable.

       @datatypes
       #GnssSvNotification \n

       @param GnssSvNotification[Input]  GNSS SV Data.
       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*locClientFidlReportSv)(GnssSvNotification& svNotify, void *context);

    /**
       The FIDL event to report as GNSS SV Measurement. This interface API should
       call by Loc Client FIDL. The Event processing is done in FIDL context and
       FIDL need to support multiple execution contexts.
       The execution context will identify using ‘void *context’ variable.

       @datatypes
       #GnssSvMeasurementSet \n

       @param GnssSvMeasurementSet[Input]  GNSS SV Measurement Data.
       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*locClientFidlReportSvMeasurement)(GnssMeasurements &svMeasurementSet, void *context);

    /**
       The FIDL event to report NMEA Sentences. This interface API should
       call by FIDL. The Event processing is done in Loc Client FIDL context and
       FIDL need to support multiple execution contexts.
       The execution context will identify using ‘void *context’ variable.

       @param nmea[Input]  NMEA Sentence.

       @param length[Input]  Length of NMEA Sentence.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
           None.

       @dependencies
           None.
    */
    void (*locClientFidlReportNmea)(const char* nmea, int length, void *context);

    /**
       The FIDL event to report GNSS Measurement Data. This interface API should be call by FIDL.
       The Event processing is done in FIDL context and same context Adapter API
       will called. FIDL need to support multiple execution contexts.
       The execution context will identify using ‘void *context’ variable.

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
    void (*locClientFidlReportGnssMeasurementData)(GnssMeasurements& measurements,
            int msInWeek, void *context);
    /* */
    void (*locClientFidlReportGnssConfig)(uint32_t sessionId, const GnssConfig& gnssConfig, void *context);
    /* */
    void (*locClientFidlHardWareStatus)(uint32_t hardwareStatus, void *context);
    /* */
    void (*locClientFidlCapabilities)(uint32_t hwCapabilities, void *context);

} locClientFidlInterfaceEvent;


/** FIDL module’s Interface commands.
    These commands should implement by FIDL module.
    FIDL Location API module should store and call these command APIs.
 */
typedef struct {
   /**
       The FIDL Open interface function request to open/active device.
       This interface API should be call by Loc Client FIDL API.
       This API will be called multiple times in different execution contexts.
       The execution context will identify using ‘void *context’ variable.

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
               none

       @dependencies
           None.
   */
   void (*locClientFidlOpen)(uint64_t requestedMask, bool isMaster,
                uint64_t *supportedMask, locClientFidlFeatureList * fidlFeatures,
                void *context);

   /**
       The FIDL Close interface function request to close/de-active device.
       This interface API should be call by Loc Client FIDL API.
       This API will be called multiple times in different execution contexts.
       The execution context will identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
               none

       @dependencies
           None.
   */
   void (*locClientFidlClose)(void *context);

   /**
       The FIDL Start fix interface function request to start a session of FIDL.
       This interface API should be call by Loc Client FIDL API.
       This API will be called multiple times in different execution contexts.
       The execution context will identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param locClientFidlPosMode[Input] Provide Session parameters to a start session.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
               none

       @dependencies
           None.
   */
   void (*locClientFidlStartFix)(locClientFidlPosMode& fixCriteria, void *context);

   /**
       The FIDL Stop fix interface function request to stop a ongoing session of FIDL.
       This interface API should be call by Loc Client FIDL API.
       This API will be called multiple times in different execution contexts.
       The execution context will identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
               none

       @dependencies
           None.
   */
   void (*locClientFidlStopFix)(void *context);

   /**
       The FIDL Delete Aiding Data interface function request delete assistance data of FIDL.
       This interface API should be call by Loc Client FIDL API.
       This API will be called multiple times in different execution contexts.
       The execution context will identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param GnssAidingData[Input] Provide Session parameters to a start session.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
               none

       @dependencies
           None.
   */
   void (*locClientFidlDeleteAidingData)(const GnssAidingData& data, void *context);

   /**
       The FIDL Set Position Mode interface function to update session parameters of
       ongoing session or upcoming session. This interface API should be call by
       Loc Client FIDL API. This API will be called multiple times in differenti
       execution contexts.
       The execution context will identify using ‘void *context’ variable.

       @datatypes
       #loc_api_adapter_err

       @param locClientFidlPosMode[Input] Provide Session parameters to a start session.

       @param context[Input]    Context Pointer of FIDL Location API.

       @return
               none

       @dependencies
           None.
   */
   void (*locClientFidlSetPositionMode)(locClientFidlPosMode& posMode, void *context);

   /**
       The FIDL interface to request GPS Lock incase of SUPL when GNSS disabled. This interface
       API should be call by Loc Client FIDL API. This API will be called multiple times in
       different execution contexts.
       The execution context will identify using ‘void *context’ variable.

       @datatypes
        #GnssConfigGpsLock
        #loc_api_adapter_err

       @param context[Input]    Context Pointer of FIDL Location API. FIDL should store
                                and use any future interaction with FIDL location API.

       @return
               none

       @dependencies
           None.
   */
   void (*locClientFidlSetGpsLockSync)(GnssConfigGpsLock lock, void *context);

   void (*locClientFidlStartTimeBasedTracking)(const TrackingOptions& options,
        void *context);
   void (*locClientFidlStopTimeBasedTracking)(void *context);

} locClientFidlInterfaceReq;


#ifdef __cplusplus
extern "C" {
#endif
    /**
      The FIDL location layer(FIDL) interface function provide commands and events callbacks to
      interact with FIDL module.
      This interface API should be call by FIDL LOC API module and it should implement by FIDL.
      This API will be called multiple times in different execution contexts.
      The execution context will identify using ‘void *context’ variable.

      @datatypes
      #locClientFidlInterfaceReq \n
      #locClientFidlInterfaceEvent

      @param eventCallback[Input]   These are event callbacks of FIDL module. These events should
                                    implement by FIDL Location API.
                                    FIDL module should store and call these event APIs.
      @param context[Input]         Context Pointer of FIDL Location API. FIDL should store and
                                    use any future interaction with FIDL location API.

      @return
           locClientFidlInterfaceReq[Output]  These are Interface commands of FIDL module. These commands
                                    should implement by FIDL module.
                                    FIDL Location API module should store and call these
                                    command APIs.

      @dependencies
      None.
    */
    const locClientFidlInterfaceReq* get_locClientFidl_if_api(locClientFidlInterfaceEvent* eventCallback,
                                     void *context);

    /**
      The FIDL location layer(FIDL) interface function pointer provide commands and events
      callbacks to interact with FIDL module. This interface API should be call by FIDL LOC
      API module, and it should implement by FIDL.
      This API will be called multiple times in different execution contexts.
      The execution context will identify using ‘void *context’ variable.

      @datatypes
      #locClientFidlInterfaceReq \n
      #locClientFidlInterfaceEvent

      @param eventCallback[Input]   These are event callbacks of FIDL module. These events
                                    should implement by FIDL Location API.
                                    FIDL module should store and call these event APIs.
      @param context[Input]         Context Pointer of FIDL Location API. FIDL should store
                                    and use any future interaction with FIDL location API.

      @return
           locClientFidlInterfaceReq[Output]  These are Interface commands of FIDL module. These commands
                                    should implement by FIDL module.
                                    FIDL Location API module should store and call these
                                    command APIs.

      @dependencies
           None.
    */
    typedef const locClientFidlInterfaceReq* (*get_locClientFidl_if_api_t)
                                       (const locClientFidlInterfaceEvent* eventCallback, void *context);

#ifdef __cplusplus
}
#endif

#endif /* LOC_CLIENT_FIDL_INTERFACE_H */
