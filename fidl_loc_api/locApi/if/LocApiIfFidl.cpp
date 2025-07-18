/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */


#define LOG_NDEBUG 0
#define LOG_TAG "FidlLoc_Api"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include <dlfcn.h>
#include <algorithm>
#include <cutils/sched_policy.h>

#include <gps_extended.h>
#include "loc_pla.h"
#include <loc_cfg.h>
#include <LocApiBase.h>
#include <LocContext.h>
#include <fild_util.h>
#include <LocApiIfFidl.h>

using namespace std;
using namespace loc_core;


#define SL_MAX_SV_CNT_SUPPORTED_IN_ONE_CONSTELLATION (64)
#define SL_NO_FEATURE_SUPPORTED (0)
#define LOCAPI_FIDL_CORE_LIB_NAME       "libloc_fidl_core.so"
#define LOCAPI_FIDL_SIM_LIB_NAME        "libloc_fidl_sim.so"

#define FIDL_DEFAULT_IMPL()                                    \
{                                                             \
    LOC_LOGd("%s: default implementation invoked", __func__); \
    return LOC_API_ADAPTER_ERR_UNSUPPORTED;                   \
}

typedef const FidlInterfaceReq* (*get_fidl_if_api_t)
            (const FidlInterfaceEvent* eventCallback, void *context);

/**
   Engine Up Event, this is receive from FIDL Interface.
   This event indicates Engine is ready to accept command
   such as start, delete assistance data.

   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlEngineUp(void *context) {

    if (nullptr != context) {
        LOC_LOGD("%s:%d",__func__,__LINE__);
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->updateEngineState(FIDL_HW_ENGINE_ON);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}


/**
   Engine Down Event, this is receive from FIDL Interface.
   This event indicates Engine is not ready.

   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlEngineDown(void *context) {

    if (nullptr != context) {
        LOC_LOGD("%s:%d",__func__,__LINE__);
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->updateEngineState(FIDL_HW_ENGINE_OFF);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   Position report Event, this is receive from FIDL Interface.
   This event sends Position report of SSL Hardware.

   @param UlpLocation[Input]   Position reprot.
   @param GpsLocationExtended[Input]   Extended Position reprot.
   @param loc_sess_status[Input]   Indicates Position report status
                                   as Final or intermediate
   @param LocPosTechMask[Input]         Indiacte Fix type.
   @param GnssDataNotification[Input]   Indicate Jammer and AGC info.
   @param msInWeek[Input]   Indicate time in milliseconds.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportPosition(UlpLocation& location,
                    GpsLocationExtended& locationExtended,
                    enum loc_sess_status status,
                    LocPosTechMask loc_technology_mask,
                    GnssDataNotification* pDataNotify,
                    int msInWeek, void *context) {
    LOC_LOGD("%s:%d",__func__,__LINE__);
    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportPosition(location, locationExtended,
                            status, loc_technology_mask, pDataNotify, msInWeek);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   SV Report Event, this is receive from FIDL Interface.
   This event indicates SV Info.

   @param GnssSvNotification[Input]    SV Info in track.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportSv(GnssSvNotification& svNotify, void *context) {

    if (nullptr != context) {
        LOC_LOGD("%s:%d",__func__,__LINE__);
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportSv(svNotify);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   SV Measurement Event, this is receive from FIDL Interface.
   This event indicates SV Measurement.

   @param GnssSvMeasurementSet[Input]    SV Measurement
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportSvMeasurement(GnssMeasurements &svMeasurementSet, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportGnssMeasurements(svMeasurementSet, 0);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   SV Polynomial Event, this is receive from FIDL Interface.
   This event indicates SV Polynomial.

   @param GnssSvPolynomial[Input]    SV Polynomial
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void hanldeFidlReportSvPolynomial(GnssSvPolynomial &svPolynomial, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportSvPolynomial(svPolynomial);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   SV Ephemeris Event, this is receive from FIDL Interface.
   This event indicates SV Ephemeris.

   @param GnssSvEphemerisReport[Input]    SV Ephemeris
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportSvEphemeris(GnssSvEphemerisReport &svEphemeris, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportSvEphemeris(svEphemeris);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   Engine and Session Status Event, this is receive from FIDL Interface.
   This event indicates Engine and Session Status.

   @param LocGpsStatusValue[Input]    SV Ephemeris
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void hanldeFidlReportStatus(LocGpsStatusValue status, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportStatus(status);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   Report NMEA String, this is receive from FIDL Interface.
   This event indicates NMEA string generated by Hardware.

   @param nmea[Input]       NMEA Strings
   @param nmea[Input]       NMEA Strings Length
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void hanldeFidlReportNmea(const char* nmea, int length, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportNmea(nmea, length);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Report AGC and Jammer Info, this is received from FIDL Interface.
   This event indicates AGC and Jammer Info generated by Hardware.

   @param GnssDataNotification[Input]   GNSS Data Notification.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportData(GnssDataNotification& dataNotify, int msInWeek,
    void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportData(dataNotify, msInWeek);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Report XTRA Server Info, this is received from FIDL Interface.
   This event indicates XTRA Server URL info.

   @param url1[Input]    XTRA Server URL.
   @param url2[Input]    XTRA Server URL.
   @param url3[Input]    XTRA Server URL.
   @param maxlength[Input]    Max length of URL.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void hanldeFidlReportXtraServer(const char* url1, const char* url2,
    const char* url3, const int maxlength, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportXtraServer(url1, url2, url3, maxlength);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Report Location System Info, this is received from FIDL Interface.
   This event indicates LEAP second related Info.

   @param LocationSystemInfo[Input]    Leap Second related info.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportLocationSystemInfo(const LocationSystemInfo& locationSystemInfo,
    void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportLocationSystemInfo(locationSystemInfo);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Request for XTRA Server Info, this is received from FIDL Interface.
   This event is to request to provide XTRA Server URL info.

   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlRequestXtraData(void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->requestXtraData();
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Request for Time Info, this is received from FIDL Interface.
   This event is to request to provide Time info.

   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlRequestTime(void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->requestTime();
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Request for Location Info, this is received from FIDL Interface.
   This event is to request to provide Location info.

   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlRequestLocation(void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->requestLocation();
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   Request for ATL Info, this is received from FIDL Interface.
   This event is to request to provide ATL info.

   @param connHandle[Input]   Connection Handle of ATL.
   @param agps_type[Input]    AGPS Type such as SUPL/Wifi/SUPL ES
   @param apn_type_mask[Input]  APN Type.

   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlRequestATL(int connHandle, LocAGpsType agps_type,
        LocApnTypeMask apn_type_mask, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->requestATL(connHandle, agps_type, apn_type_mask);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Request to release ATL Connection, this is received from FIDL Interface.
   This event is to request to release ATL connection.

   @param connHandle[Input]   Connection Handle of ATL.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReleaseATL(int connHandle, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->releaseATL(connHandle);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   Request to indicate NI-SUPL User notification, this is received from FIDL Interface.
   This event is to indicate NI-SUPL User notification.

   @param notify[Input]   NI SUPL User indication.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlRequestNiNotify(GnssNiNotification &notify, const void* data,
        void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        /* Ni is not support  */
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   Request to indicate measurement report, this is received from FIDL Interface.
   This event is to indicate measurement reprot.

   @param GnssMeasurementsNotification[Input]   measurement report.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportGnssMeasurementData(GnssMeasurements &measurements,
    int msInWeek, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportGnssMeasurements(measurements, msInWeek);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Request to report WWAN ZPP Position report, this is received from FIDL Interface.
   This event is to report ZPP Position report.

   @param LocGpsLocation[Input]   Position report.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportWwanZppFix(LocGpsLocation &zppLoc, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportWwanZppFix(zppLoc);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   Report the Best ZPP Position, this is received from FIDL Interface.
   This event is to report Best ZPP Position.

   @param LocGpsLocation[Input]   Position report.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportZppBestAvailableFix(LocGpsLocation &zppLoc,
    GpsLocationExtended &location_extended, LocPosTechMask tech_mask, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportZppBestAvailableFix(zppLoc,
                    location_extended, tech_mask);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Report the Black listed SV configuration, this is received from FIDL Interface.
   This event is to report Black listed SV ID.

   @param GnssSvIdConfig[Input]   Black listed SV configuration.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportGnssSvIdConfig(const GnssSvIdConfig& config, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportGnssSvIdConfig(config);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Report the Blacklisted Constellation, this is received from FIDL Interface.
   This event is to report Blacklisted Constellation.

   @param GnssSvIdConfig[Input]   Blacklisted Constellation.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportGnssSvTypeConfig(const GnssSvTypeConfig& config, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportGnssSvTypeConfig(config);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Request the ODCPI, this is received from FIDL Interface.
   This event is to request ODCPI.

   @param OdcpiRequestInfo[Input]   Request for ODCPI.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlRequestOdcpi(OdcpiRequestInfo& request, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->requestOdcpi(request);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Report the Energy Consumed, this is received from FIDL Interface.
   This event is to report Engery Consumed by HW from Boot.

   @param GnssSvIdConfig[Input]   Engery Consumed Since First Boot,
                                 in units of 0.1 milli watt seconds
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportGnssEngEnergyConsumedEvent(uint64_t energyConsumedSinceFirstBoot,
    void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportGnssEngEnergyConsumedEvent(energyConsumedSinceFirstBoot);
    } else {
        LOC_LOGw ("Context is NULL");
    }

}

/**
   Report the Delete Aiding Data Event, this is received from FIDL Interface.
   This event is to report delete aiding data event.

   @param GnssAidingData[Input]   Indicate GNSS Aiding Data,
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportDeleteAidingDataEvent(GnssAidingData& aidingData, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportDeleteAidingDataEvent(aidingData);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   Report the IONO model, this is received from FIDL Interface.
   This event is to report IONO Model.

   @param GnssKlobucharIonoModel[Input]   Report IONO Model,
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportKlobucharIonoModel(GnssKlobucharIonoModel& ionoModel, void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportKlobucharIonoModel(ionoModel);

    } else {
        LOC_LOGw ("Context is NULL");
    }
}

/**
   Report the Additional System Info, this is received from FIDL Interface.
   This event is to report Additional System Info which includes system time.

   @param GnssAdditionalSystemInfo[Input]   Report Additional System Info
                                            which includes system time.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       None.

   @dependencies
       None.
*/
void handleLocApiFidlReportGnssAdditionalSystemInfo(GnssAdditionalSystemInfo& additionalSystemInfo,
    void *context) {

    if (nullptr != context) {
        FidlLocApi *fidlLocApiInstance = (FidlLocApi*)context;
        fidlLocApiInstance->reportGnssAdditionalSystemInfo(additionalSystemInfo);
    } else {
        LOC_LOGw ("Context is NULL");
    }
}

void handleLocApiFidlRequestEedGpsData(void *context) {
   (void *)(context);
   LOC_LOGw ("Default Implementation");
}

void handleLocApiFidlRequestEedGloData(void *context) {
   (void *)(context);
   LOC_LOGw ("Default Implementation");
}


void handleLocApiFidlSendNfwNotification(GnssNfwNotification& notification,
   void *context) {
   (void *)(context);
   LOC_LOGw ("Default Implementation");
}

void  handleLocApiFidlReportGnssConfig(uint32_t sessionId, const GnssConfig& gnssConfig,
   void *context) {
   (void *)(context);
   LOC_LOGw ("Default Implementation");
}

void handleLocApiFidlReportLatencyInfo(GnssLatencyInfo& gnssLatencyInfo,
   void *context) {
   (void *)(context);
   LOC_LOGw ("Default Implementation");
}

void handleLocApiFidlReportQwesCapabilities(
    const std::unordered_map<LocationQwesFeatureType, bool> &featureMap,
   void *context) {
   (void *)(context);
   LOC_LOGw ("Default Implementation");
}

void handleLocApiFidlGeofenceBreach (size_t count, uint32_t* hwIds, Location& location,
    GeofenceBreachType breachType, uint64_t timestamp,
   void *context) {
   (void *)(context);
   LOC_LOGw ("Default Implementation");
}

void handleLocApiFidlGeofenceStatus(GeofenceStatusAvailable available,
   void *context) {
   (void *)(context);
   LOC_LOGw ("Default Implementation");
}

void handleLocApiFidlReportDBTPosition(UlpLocation &location,
   GpsLocationExtended &locationExtended,
   enum loc_sess_status status,
   LocPosTechMask loc_technology_mask,
   void *context) {
        (void *)(context);
        LOC_LOGw ("Default Implementation");
}

void handleLocApiFidlReportLocations(Location* locations, size_t count, BatchingMode batchingMode,
   void *context) {
         (void *)(context);
         LOC_LOGw ("Default Implementation");
}

void handleLocApiFidlReportCompletedTrips(uint32_t accumulated_distance,
   void *context) {
       (void *)(context);
       LOC_LOGw ("Default Implementation");
}

void handleLocApiFidlHandleBatchStatusEvent(BatchingStatus batchStatus,
   void *context) {
   (void *)(context);
   LOC_LOGw ("Default Implementation");
}



/* Init FIDL Event Callbacks */
const FidlInterfaceEvent fidlEventCb = {
    handleLocApiFidlEngineUp,
    handleLocApiFidlEngineDown,
    handleLocApiFidlReportPosition,
    handleLocApiFidlReportSv,
    handleLocApiFidlReportSvMeasurement,
    hanldeFidlReportSvPolynomial,
    handleLocApiFidlReportSvEphemeris,
    hanldeFidlReportStatus,
    hanldeFidlReportNmea,
    handleLocApiFidlReportData,
    hanldeFidlReportXtraServer,
    handleLocApiFidlReportLocationSystemInfo,
    handleLocApiFidlRequestXtraData,
    handleLocApiFidlRequestTime,
    handleLocApiFidlRequestLocation,
    handleLocApiFidlRequestATL,
    handleLocApiFidlReleaseATL,
    handleLocApiFidlRequestNiNotify,
    handleLocApiFidlReportGnssMeasurementData,
    handleLocApiFidlReportWwanZppFix,
    handleLocApiFidlReportZppBestAvailableFix,
    handleLocApiFidlReportGnssSvIdConfig,
    handleLocApiFidlReportGnssSvTypeConfig,
    handleLocApiFidlRequestOdcpi,
    handleLocApiFidlReportGnssEngEnergyConsumedEvent,
    handleLocApiFidlReportDeleteAidingDataEvent,
    handleLocApiFidlReportKlobucharIonoModel,
    handleLocApiFidlReportGnssAdditionalSystemInfo,
    handleLocApiFidlSendNfwNotification,
    handleLocApiFidlReportGnssConfig,
    handleLocApiFidlReportLatencyInfo,
    handleLocApiFidlReportQwesCapabilities,
    handleLocApiFidlGeofenceBreach,
    handleLocApiFidlGeofenceStatus,
    handleLocApiFidlReportDBTPosition,
    handleLocApiFidlReportLocations,
    handleLocApiFidlReportCompletedTrips,
    handleLocApiFidlHandleBatchStatusEvent
};

/**
    Default Implantation of Open Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlOpen(uint64_t requestedMask, bool isMaster,
             uint64_t *supportedMask, fidlFeatureList * fidlFeatures,
             void *context) {
    (void*)supportedMask;
    (void*)fidlFeatures;
    (void*)context;
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Close Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlClose(void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Start Session Command;
    to indicate the open command is not supported.
*/
enum loc_api_adapter_err defaultFidlStartFix(fidlPosMode& fixCriteria, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Stop Session Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlStopFix(void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Delet Aiding Data Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlDeleteAidingData(const GnssAidingData& data, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Inject Position Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlInjectPosition(const GnssLocationInfoNotification &locationInfo,
        bool onDemandCpi, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Set Time Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetTime(LocGpsUtcTime time, int64_t timeReference,
        int uncertainty, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Set XTRA Data Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetXtraData(char* data, int length, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Request XTRA Server Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlRequestXtraServer(void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of ATL Open Status Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlAtlOpenStatus(int handle, int is_succ, char* apn,
            uint32_t apnLen, AGpsBearerType bear, LocAGpsType agpsType, LocApnTypeMask mask,
            void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of ATL Close Status Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlAtlCloseStatus(int handle, int is_succ, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Set Position Mode Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetPositionMode(fidlPosMode& posMode, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Set Server Sync Command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetServerSync(const char* url, int len, LocServerType type,
        void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of inform NI reponse command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlInformNiResponse(GnssNiResponse userResponse,
        const void* passThroughData, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of inform NI reponse commnad;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetSUPLVersionSync
        (GnssConfigSuplVersion version, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set NMEA Types command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetNMEATypesSync(uint32_t typesMask, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set LPP Configuration command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetLPPConfigSync(GnssConfigLppProfileMask profileMask,
        void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set Sensor Properties command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetSensorPropertiesSync
                       (bool gyroBiasVarianceRandomWalk_valid,
                        float gyroBiasVarianceRandomWalk,
                        bool accelBiasVarianceRandomWalk_valid,
                        float accelBiasVarianceRandomWalk,
                        bool angleBiasVarianceRandomWalk_valid,
                        float angleBiasVarianceRandomWalk,
                        bool rateBiasVarianceRandomWalk_valid,
                        float rateBiasVarianceRandomWalk,
                        bool velocityBiasVarianceRandomWalk_valid,
                        float velocityBiasVarianceRandomWalk, void *context) {
    FIDL_DEFAULT_IMPL();

}

/**
    Default Implantation of set Sensor Preferred Control configure command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetSensorPerfControlConfigSync(int controlMode,
                        int accelSamplesPerBatch,
                        int accelBatchesPerSec,
                        int gyroSamplesPerBatch,
                        int gyroBatchesPerSec,
                        int accelSamplesPerBatchHigh,
                        int accelBatchesPerSecHigh,
                        int gyroSamplesPerBatchHigh,
                        int gyroBatchesPerSecHigh,
                        int algorithmConfig, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set AGLONASS Protocol Sync command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetAGLONASSProtocolSync
        (GnssConfigAGlonassPositionProtocolMask aGlonassProtocol, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set LPPe Protocol Control Sync command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetLPPeProtocolCpSync(GnssConfigLppeControlPlaneMask lppeCP,
        void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set LPPe Protocol UP Sync command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetLPPeProtocolUpSync(GnssConfigLppeUserPlaneMask lppeUP,
        void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of convert SUPL Version command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlConvertSuplVersion(const uint32_t suplVersion,
        GnssConfigSuplVersion *suplConfig, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of convert LPP Profile command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlConvertLppProfile(const uint32_t lppProfile,
        GnssConfigLppProfileMask *gnssLppProfileMask, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of convert LPPe Control Plan command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlConvertLppeCp(const uint32_t lppeControlPlaneMask,
        GnssConfigLppeControlPlaneMask *gnssLppControlPlanMask, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of convert LPPe User Plan command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlConvertLppeUp(const uint32_t lppeUserPlaneMask,
        GnssConfigLppeUserPlaneMask *gnssLppUpMask, void *context) {
    FIDL_DEFAULT_IMPL();
}


LocationError defaultFidlSetEmergencyExtensionWindowSync(const uint32_t emergencyExtensionSeconds,
  void *context) {
    LOC_LOGd("%s: default implementation invoked", __func__);
    return LOCATION_ERROR_ID_UNKNOWN;
}

void defaultFidlSetMeasurementCorrections(
   const GnssMeasurementCorrections& gnssMeasurementCorrections, void *context) {
    LOC_LOGd("%s: default implementation invoked", __func__);
}

/**
    Default Implantation of get WWAN ZPP Fix command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlGetWwanZppFix(void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of best Available ZPP Fix command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlGetBestAvailableZppFix(void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Set GPS Lock Sync command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetGpsLockSync(GnssConfigGpsLock lock, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Request for aiding data command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlRequestForAidingData(GnssAidingDataSvMask svDataMask,
        void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Request for get GPS Lock command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlGetGpsLock(uint8_t subType, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of Request for get GPS Lock command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetXtraVersionCheckSync(uint32_t check, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set AGPS certificate command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlInstallAGpsCert(const LocDerEncodedCertificate* pData,
                             size_t length,
                             uint32_t slotBitMask, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set Constrained Tunc Mode command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetConstrainedTuncMode(bool enabled, float tuncConstraint,
                              uint32_t energyBudget, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set Position Assisted Clock Estimator Mode command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlSetPositionAssistedClockEstimatorMode(bool enabled,
                             void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of get GNSS Energy Consumed command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultFidlGetGnssEnergyConsumed(void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set Black List SV command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultfidlSetBlacklistSv(const GnssSvIdConfig& config, void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of get Black List SV command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultfidlGetBlacklistSv(void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of set constellation Control command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultfidlSetConstellationControl(const GnssSvTypeConfig& config,
        void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of get constellation Control command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultfidlGetConstellationControl(void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of reset constellation Control command;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultfidlResetConstellationControl(void *context) {
    FIDL_DEFAULT_IMPL();
}

/**
    Default Implantation of request XTRA configuration Info;
    to indicate the command is not supported.
*/
enum loc_api_adapter_err defaultfidlRequestXtraConfigInfo(const uint32_t configMask,
                                         const uint32_t configReqSource, void *context) {
    FIDL_DEFAULT_IMPL();
}

/* Default Implantation of FIDL Commands */
const FidlInterfaceReq fidlDefultReq = {
    defaultFidlOpen,
    defaultFidlClose,
    defaultFidlStartFix,
    defaultFidlStopFix,
    defaultFidlDeleteAidingData,
    defaultFidlInjectPosition,
    defaultFidlSetTime,
    defaultFidlSetXtraData,
    defaultFidlRequestXtraServer,
    defaultFidlAtlOpenStatus,
    defaultFidlAtlCloseStatus,
    defaultFidlSetPositionMode,
    defaultFidlSetServerSync,
    defaultFidlInformNiResponse,
    defaultFidlSetSUPLVersionSync,
    defaultFidlSetNMEATypesSync,
    defaultFidlSetLPPConfigSync,
    defaultFidlSetSensorPropertiesSync,
    defaultFidlSetSensorPerfControlConfigSync,
    defaultFidlSetAGLONASSProtocolSync,
    defaultFidlSetLPPeProtocolCpSync,
    defaultFidlSetLPPeProtocolUpSync,
    defaultFidlConvertSuplVersion,
    defaultFidlConvertLppProfile,
    defaultFidlConvertLppeCp,
    defaultFidlConvertLppeUp,
    defaultFidlSetEmergencyExtensionWindowSync,
    defaultFidlSetMeasurementCorrections,
    defaultFidlGetWwanZppFix,
    defaultFidlGetBestAvailableZppFix,
    defaultFidlSetGpsLockSync,
    defaultFidlRequestForAidingData,
    defaultFidlGetGpsLock,
    defaultFidlSetXtraVersionCheckSync,
    defaultFidlInstallAGpsCert,
    defaultFidlSetConstrainedTuncMode,
    defaultFidlSetPositionAssistedClockEstimatorMode,
    defaultFidlGetGnssEnergyConsumed,
    defaultfidlSetBlacklistSv,
    defaultfidlGetBlacklistSv,
    defaultfidlSetConstellationControl,
    defaultfidlGetConstellationControl,
    defaultfidlResetConstellationControl,
    defaultfidlRequestXtraConfigInfo
};

/* Constructor for FidlLocApi */
FidlLocApi::FidlLocApi(LOC_API_ADAPTER_EVENT_MASK_T exMask,
                       ContextBase* context) :
    LocApiBase(exMask, context),
    mInSession(false), mPowerMode(GNSS_POWER_MODE_INVALID), mEngineOn(false),
    mMeasurementsStarted(false), mIsMasterRegistered(false),
    mMasterRegisterNotSupported(false), mSvMeasurementSet(nullptr),
    mFdlMask(0), mPositionElapsedRealTimeCal(30000000), mMinInterval(1000)
{
    const char * libName = nullptr;
    void *handle = nullptr;
    unsigned int isFidlSimEnabled = 0;
    const char *error = nullptr;

    loc_param_s_type gps_conf_param_table[] =
    {
        {"IS_FIDL_SIM_ENABLED", &isFidlSimEnabled, NULL, 'n'}
    };

    UTIL_READ_CONF(LOC_PATH_GPS_CONF, gps_conf_param_table);

    if (isFidlSimEnabled) {
        libName = LOCAPI_FIDL_SIM_LIB_NAME;
    } else {
        libName = LOCAPI_FIDL_CORE_LIB_NAME;
    }

    if ((handle = dlopen(libName, RTLD_NOW)) != nullptr) {
        LOC_LOGv("%s is present", libName);
        get_fidl_if_api_t getter = (get_fidl_if_api_t)dlsym(handle, "get_fidl_if_api");

        if ((error = dlerror()) != nullptr) {
            LOC_LOGe("%s get_fidl_if_api failed, error = %s", libName, error);
            getter = nullptr;
        }

        if (getter != nullptr) {
            fidlReqIf = (getter)(&fidlEventCb, ((void *)this));
            if (fidlReqIf != nullptr) {
                return;
            } else {
                LOC_LOGe("%s FIDL lib provided Command Interface as NULL", libName);
            }
        }
    } else {
        char *errorDll = dlerror();

        LOC_LOGe("%s failed to load, handle %p error: %s", libName, handle,
             ((nullptr != errorDll) ? errorDll : "No Error"))
    }

    fidlReqIf = &fidlDefultReq;

}

/* Destructor for FidlLocApi */
FidlLocApi::~FidlLocApi()
{
    close();
}

/**
   The API to get Fidl Location API object.

   @param LOC_API_ADAPTER_EVENT_MASK_T[Input]  Configure Event Mask.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       LocApiBase: Pointer of Constructor of Fidl Location API.

   @dependencies
       None.
*/
LocApiBase* getLocApi(LOC_API_ADAPTER_EVENT_MASK_T exMask,
                      ContextBase* context)
{
    return (LocApiBase*)FidlLocApi::createFidlLocApi(exMask, context);
}


/**
   The API to call Constructor of Fidl Location API.

   @param LOC_API_ADAPTER_EVENT_MASK_T[Input]  Configure Event Mask.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       LocApiBase: Pointer of Constructor of Fidl Location API.

   @dependencies
       None.
*/
LocApiBase* FidlLocApi::createFidlLocApi(LOC_API_ADAPTER_EVENT_MASK_T exMask,
                                       ContextBase* context)
{
    return new FidlLocApi(exMask, context);
}


/**
   Initialize a fidl loc api client AND
   check which loc message are supported by FIDL.

   @param LOC_API_ADAPTER_EVENT_MASK_T[Input]  Configure Event Mask.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
       loc_api_adapter_err.

   @dependencies
       None.
*/
enum loc_api_adapter_err
FidlLocApi::open(LOC_API_ADAPTER_EVENT_MASK_T mask) {
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_UNKNOWN;
    uint64_t supportedMask = 0;
    uint64_t requestedMask = (uint64_t)mask;
    bool gnssMeasurementSupported = false;
    bool isMasterSession = false;
    fidlFeatureList  getFidlFeatures;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlOpen)) {
            //Check for Master and Measurement
        if (isMaster()) {
            isMasterSession = true;
        }

        memset(&getFidlFeatures, 0, sizeof(getFidlFeatures));
        rtv = fidlReqIf->fidlOpen(requestedMask, isMasterSession, &supportedMask,
                    &getFidlFeatures, (void *)this);
        if (((LOC_API_ADAPTER_ERR_SUCCESS == rtv) ||
            (LOC_API_ADAPTER_ERR_UNSUPPORTED == rtv)) &&
            (SL_NO_FEATURE_SUPPORTED != supportedMask)) {
                LOC_LOGd ("FIDL Requested Configuration 0x%" PRIx64 ","
                        " Supported Configuration 0x%" PRIx64 "",
                        mask, supportedMask);
                if ((LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT ==
                        (supportedMask & LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT)) ||
                    (LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT_REPORT ==
                        (supportedMask & LOC_API_ADAPTER_BIT_GNSS_MEASUREMENT))) {
                    gnssMeasurementSupported = true;
                }
            mContext->setEngineCapabilities(supportedMask, (getFidlFeatures.feature_len != 0 ?
                getFidlFeatures.feature : NULL), gnssMeasurementSupported);
        }
    }
    LOC_LOGd("%s:%d rtv = %" PRIx64 "",__func__,__LINE__,rtv);
    return rtv;
}

/**
   Register Event Mask

   @param LOC_API_ADAPTER_EVENT_MASK_T[Input]  Configure Event Mask.
   @param context[Input]    Context Pointer of Fidl Location API.

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::registerEventMask(LOC_API_ADAPTER_EVENT_MASK_T adapterMask) {

    mMask = adapterMask;
    return;
}

/**
   Request to Close FIDL Interface

   @param
        none

   @return
       loc_api_adapter_err.

   @dependencies
       None.
*/
enum loc_api_adapter_err FidlLocApi::close() {

    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlClose)) {
        rtv = fidlReqIf->fidlClose((void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
            LOC_LOGw ("Close Error 0x%x:", rtv);
        }
    } else {
        rtv = LOC_API_ADAPTER_ERR_UNKNOWN;
    }
    return rtv;
}


/**
   start positioning session

   @param LocPosMode[Input]  Session configuration.
   @param LocApiResponse[Input]  LOC API Response API.

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::startFix(const LocPosMode& fixCriteria, LocApiResponse *adapterResponse) {

    mMinInterval = fixCriteria.min_interval;

    sendMsg(new LocApiMsg([this, fixCriteria, adapterResponse] () {
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
        fidlPosMode posMode;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlStartFix)) {
            posMode.mode = fixCriteria.mode;
            posMode.recurrence = fixCriteria.recurrence;
            posMode.min_interval = fixCriteria.min_interval;
            posMode.preferred_accuracy = fixCriteria.preferred_accuracy;
            posMode.preferred_time = fixCriteria.preferred_time;
            posMode.share_position = fixCriteria.share_position;
            posMode.powerMode = fixCriteria.powerMode;
            posMode.timeBetweenMeasurements = fixCriteria.timeBetweenMeasurements;

            rtv = fidlReqIf->fidlStartFix(posMode, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            }
            /* Return the State based on Engine */
            if (false == mEngineOn) {
                err = LOCATION_ERROR_SYSTEM_NOT_READY;
            }

        } else {
            err = LOCATION_ERROR_NOT_SUPPORTED;
        }
        if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }
    }));

}

/**
   stop a positioning session

   @param LocApiResponse[Input]  LOC API Response API.

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::stopFix(LocApiResponse *adapterResponse) {

    mMinInterval = 0;

    sendMsg(new LocApiMsg([this, adapterResponse] () {
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlStopFix)) {

            rtv = fidlReqIf->fidlStopFix((void *)this);
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            }
        } else {
            err = LOCATION_ERROR_NOT_SUPPORTED;
        }

        if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }
    }));
}


/**
   set the positioning fix criteria

   @param LocPosMode[Input]  Session configuration paramter.

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::setPositionMode(const LocPosMode& posMode) {

    mMinInterval = posMode.min_interval;

    sendMsg(new LocApiMsg([this, posMode] () {
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
        fidlPosMode LocSlPosMode;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetPositionMode)) {
            LocSlPosMode.mode = posMode.mode;
            LocSlPosMode.recurrence = posMode.recurrence;
            LocSlPosMode.min_interval = posMode.min_interval;
            LocSlPosMode.preferred_accuracy = posMode.preferred_accuracy;
            LocSlPosMode.preferred_time = posMode.preferred_time;
            LocSlPosMode.share_position = posMode.share_position;
            LocSlPosMode.powerMode = posMode.powerMode;
            LocSlPosMode.timeBetweenMeasurements = posMode.timeBetweenMeasurements;

            rtv = fidlReqIf->fidlSetPositionMode(LocSlPosMode, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            }
        } else {
            err = LOCATION_ERROR_NOT_SUPPORTED;
        }
    }));
}

/**
   Inject time into the position engine

   @param LocGpsUtcTime[Input]  Location GPS UTC Time.
   @param timeReference[Input]  Reference Time.
   @param uncertainty[Input]    Time Uncertainty.
                                Units: Milliseconds.

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::
    setTime(LocGpsUtcTime time, int64_t timeReference, int uncertainty) {

    sendMsg(new LocApiMsg([this, time, timeReference, uncertainty] () {

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetTime)) {
            enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
            rtv = fidlReqIf->fidlSetTime(time, timeReference, uncertainty, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
                LOC_LOGe ("Error: %d", rtv);
            }
        }

    }));

}

/**
   inject position into the position engine

   @param Location[Input]   Position report.
   @param onDemandCpi[Input]  Indicate CPI demanded or not.

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::injectPosition(const Location& location, bool onDemandCpi) {

    sendMsg(new LocApiMsg([this, location, onDemandCpi] () {

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlInjectPosition)) {
            enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
            GnssLocationInfoNotification locInfo;
            locInfo.location = location;
            rtv = fidlReqIf->fidlInjectPosition(locInfo, onDemandCpi, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
                LOC_LOGe ("Error: %d", rtv);
            }
        }

    }));

}

/**
    This API provides number of SV used in the fix

   @param LocGpsUtcTime[Input]  Location GPS UTC Time.
   @param timeReference[Input]  Reference Time.
   @param uncertainty[Input]    Time Uncertainty.
                                Units: Milliseconds.

   @return
        None.

   @dependencies
       None.
*/
int FidlLocApi::getNumSvUsed (uint64_t svUsedIdsMask, int totalSvCntInOneConstellation) {

    if (totalSvCntInOneConstellation > SL_MAX_SV_CNT_SUPPORTED_IN_ONE_CONSTELLATION) {
        LOC_LOGe ("error: total SV count in one constellation %d exceeded limit of 64",
                  totalSvCntInOneConstellation);
        return 0;
    }

    int numSvUsed = 0;
    uint64_t mask = 0x1;
    for (int i = 0; i < totalSvCntInOneConstellation; i++) {
        if (svUsedIdsMask & mask) {
            numSvUsed++;
        }
        mask <<= 1;
    }

    return numSvUsed;
}


/**
    This API provides Position report

   @param GnssLocationInfoNotification[Input]  recevied Position report.
   @param onDemandCpi[Input]  Indicate CPI demanded or not.

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::injectPosition(const GnssLocationInfoNotification &locationInfo,
    bool onDemandCpi) {

    sendMsg(new LocApiMsg([this, locationInfo, onDemandCpi] () {
        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlInjectPosition)){
            enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
            rtv = fidlReqIf->fidlInjectPosition(locationInfo, onDemandCpi, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
                LOC_LOGe ("Error: %d", rtv);
            }
        }

    }));
}



/**
    This API provides delete assistance data

   @param GnssAidingData[Input]  GNSS Aiding Data.
   @param LocApiResponse[Input]  Adapter Response.

   @return
        None.

   @dependencies
       None.
*/
void
FidlLocApi::deleteAidingData(const GnssAidingData& data, LocApiResponse *adapterResponse) {

    sendMsg(new LocApiMsg([this, data, adapterResponse] () {
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlDeleteAidingData)) {
            enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
            rtv = fidlReqIf->fidlDeleteAidingData(data, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            }

        } else {
            err = LOCATION_ERROR_NOT_SUPPORTED;
        }
        if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }
    }));
}


/**
   This API provides NI user response to the engine

   @param GnssNiResponse[Input]  GNSS NI Response.
   @param LocApiResponse[Input]  Pass Through Data.

   @return
        None.

   @dependencies
       None.
*/
void
FidlLocApi::informNiResponse(GnssNiResponse userResponse, const void* passThroughData) {

    sendMsg(new LocApiMsg([this, userResponse, passThroughData] () {

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlInformNiResponse)) {
            enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
            rtv = fidlReqIf->fidlInformNiResponse(userResponse, passThroughData, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
                LOC_LOGe ("Error: %d", rtv);
            }
        }
        free((void *)passThroughData);
    }));

}


/**
   This API provides Set UMTs SLP server URL

   @param const char*[Input]   Server URL Configuration.
   @param Length[Input]        Server URL Lenght
   @param LocServerType[Input] Server Type

   @return
        LocationError.

   @dependencies
       None.
*/
LocationError
FidlLocApi::setServerSync(const char* url, int len, LocServerType type) {

    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
    LocationError err = LOCATION_ERROR_SUCCESS;
    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetServerSync)) {
        rtv = fidlReqIf->fidlSetServerSync (url, len, type, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
            err = LOCATION_ERROR_GENERAL_FAILURE;
        }
    } else {
        err = LOCATION_ERROR_CALLBACK_MISSING;
    }

    return err;
}

/**
   This API configure Server using IP Address and Port Number

   @param unsigned int ip[Input]    Server IP Configuration.
   @param int port[Input]           Port number
   @param LocServerType[Input]      Server Type

   @return
        LocationError.

   @dependencies
       None.
*/
LocationError
FidlLocApi::setServerSync(unsigned int ip, int port, LocServerType type) {

    return LOCATION_ERROR_CALLBACK_MISSING;
}


/**
   This API request to OPEN ATL Status

   @param handle[Input]     ATL Connection handle
   @param is_succ[Input]    ATL connection status
   @param APN[Input]        ATL APN buffer
   @param AGpsBearerType[Input]   APN Type
   @param LocAGpsType[Input]   AGPS Type
   @param LocApnTypeMask[Input]   AGPS Configuration Mask

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::atlOpenStatus(
  int handle, int is_succ, char* apn, uint32_t apnLen, AGpsBearerType bear,
  LocAGpsType agpsType, LocApnTypeMask apnTypeMask) {

    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlAtlOpenStatus)) {

        rtv= fidlReqIf->fidlAtlOpenStatus(handle, is_succ, apn, apnLen, bear, agpsType,
                apnTypeMask, (void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
             LOC_LOGe ("Error: %d", rtv);
        }

    } else {
        rtv = LOC_API_ADAPTER_ERR_UNSUPPORTED;
    }

    LOC_LOGd("Error: %d", rtv);

    return;
}


/**
   This API request to close ATL connection

   @param handle[Input]     ATL Connection handle
   @param is_succ[Input]    ATL connection status

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::atlCloseStatus(int handle, int is_succ) {
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlAtlCloseStatus)) {

        rtv= fidlReqIf->fidlAtlCloseStatus(handle, is_succ, (void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
             LOC_LOGe ("Error: %d", rtv);
        }
    }

    return;
}


/**
   set the SUPL version

   @param GnssConfigSuplVersion[Input]     ATL Connection handle
   @param is_succ[Input]    ATL connection status

   @return
        LocationError.

   @dependencies
       None.
*/
LocationError
FidlLocApi::setSUPLVersionSync(GnssConfigSuplVersion version) {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetSUPLVersionSync)) {

        rtv = fidlReqIf->fidlSetSUPLVersionSync(version, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }

    return err;
}

/**
   set the NMEA types mask

   @param typesMask[Input]    Set NMEA Mask

   @return
        enum loc_api_adapter_err

   @dependencies
       None.
*/
enum loc_api_adapter_err FidlLocApi::setNMEATypesSync(uint32_t typesMask) {

    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetNMEATypesSync)) {
        rtv= fidlReqIf->fidlSetNMEATypesSync(typesMask, (void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
             LOC_LOGe ("Error: %d", rtv);
        }

    } else {
        rtv = LOC_API_ADAPTER_ERR_UNSUPPORTED;
    }

    return rtv;
}

/**
   set the configuration for LTE positioning profile (LPP).

   @param GnssConfigLppProfileMask[Input]    Configur LPP Profile.

   @return
        LocationError

   @dependencies
       None.
*/
LocationError
FidlLocApi::setLPPConfigSync(GnssConfigLppProfileMask profileMask) {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetLPPConfigSync)) {

        rtv = fidlReqIf->fidlSetLPPConfigSync(profileMask, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }

    return err;
}


/**
   Set the Sensor Properties configuration.

   @param gyroBiasVarianceRandomWalk_valid[Input]   Is vaild Gyro Bias Varianace
   @param gyroBiasVarianceRandomWalk[Input]         Gyro Bias Variance.
   @param accelBiasVarianceRandomWalk_valid[Input]  Is vaild Accel Bias Varianace
   @param accelBiasVarianceRandomWalk[Input]        Accel Bias Variance.
   @param angleBiasVarianceRandomWalk_valid[Input]  Is vaild Angle Bias Varianace
   @param angleBiasVarianceRandomWalk[Input]        Angle Bias Variance.
   @param velocityBiasVarianceRandomWalk_valid[Input]   Is vaild Velocity Bias Varianace
   @param velocityBiasVarianceRandomWalk[Input]         Velocity Bias Variance.

   @return
        enum loc_api_adapter_err

   @dependencies
       None.
*/
enum loc_api_adapter_err FidlLocApi::setSensorPropertiesSync(
        bool gyroBiasVarianceRandomWalk_valid, float gyroBiasVarianceRandomWalk,
        bool accelBiasVarianceRandomWalk_valid, float accelBiasVarianceRandomWalk,
        bool angleBiasVarianceRandomWalk_valid, float angleBiasVarianceRandomWalk,
        bool rateBiasVarianceRandomWalk_valid, float rateBiasVarianceRandomWalk,
        bool velocityBiasVarianceRandomWalk_valid, float velocityBiasVarianceRandomWalk) {

    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetSensorPropertiesSync)) {

        rtv= fidlReqIf->fidlSetSensorPropertiesSync(gyroBiasVarianceRandomWalk_valid,
                            gyroBiasVarianceRandomWalk,
                            accelBiasVarianceRandomWalk_valid,
                            accelBiasVarianceRandomWalk,
                            angleBiasVarianceRandomWalk_valid,
                            angleBiasVarianceRandomWalk,
                            rateBiasVarianceRandomWalk_valid,
                            rateBiasVarianceRandomWalk,
                            velocityBiasVarianceRandomWalk_valid,
                            velocityBiasVarianceRandomWalk,
                            (void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
             LOC_LOGe ("Error: %d", rtv);
        }
    } else {
        rtv = LOC_API_ADAPTER_ERR_UNSUPPORTED;
    }
    return rtv;

}


/**
   Set the Sensor Performance Configuration.

   @param controlMode[Input]   Control Mode
   @param accelSamplesPerBatch[Input]       Accel Sample Per Batch.
   @param accelBatchesPerSec[Input]         Accel Batch Per Seconds.
   @param gyroSamplesPerBatch[Input]        Gyro Sample Per Batch.
   @param gyroBatchesPerSec[Input]          Gyro Batch Per Seconds.
   @param accelSamplesPerBatchHigh[Input]   Max Accel Sample Per Batches.
   @param accelBatchesPerSecHigh[Input]     Max Accel Batches Per Second.
   @param gyroSamplesPerBatchHigh[Input]    Max Gyro Sample Per Batches.
   @param gyroBatchesPerSecHigh[Input]      Max Gyro Batches Per Second.
   @param algorithmConfig[Input]            Configure Algorithm.

   @return
        enum loc_api_adapter_err

   @dependencies
       None.
*/
enum loc_api_adapter_err FidlLocApi::setSensorPerfControlConfigSync(int controlMode,
        int accelSamplesPerBatch, int accelBatchesPerSec,
        int gyroSamplesPerBatch, int gyroBatchesPerSec,
        int accelSamplesPerBatchHigh, int accelBatchesPerSecHigh,
        int gyroSamplesPerBatchHigh, int gyroBatchesPerSecHigh,
        int algorithmConfig) {

    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetSensorPerfControlConfigSync)) {

        rtv= fidlReqIf->fidlSetSensorPerfControlConfigSync(controlMode,
                            accelSamplesPerBatch,
                            accelBatchesPerSec,
                            gyroSamplesPerBatch,
                            gyroBatchesPerSec,
                            accelSamplesPerBatchHigh,
                            accelBatchesPerSecHigh,
                            gyroSamplesPerBatchHigh,
                            gyroBatchesPerSecHigh,
                            algorithmConfig,
                            (void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
             LOC_LOGe ("Error: %d", rtv);
        }
    } else {
        rtv = LOC_API_ADAPTER_ERR_UNSUPPORTED;
    }
    return rtv;

}


/**
   Set the Positioning Protocol on A-GLONASS system.

   @param GnssConfigAGlonassPositionProtocolMask[Input]   AGLONASS Protocol configure

   @return
        LocationError

   @dependencies
       None.
*/
LocationError
FidlLocApi::setAGLONASSProtocolSync(GnssConfigAGlonassPositionProtocolMask aGlonassProtocol) {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetAGLONASSProtocolSync)) {

        rtv = fidlReqIf->fidlSetAGLONASSProtocolSync(aGlonassProtocol, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }

    return err;
}


/**
   Set the LPPe Protocol control plan synchronize.

   @param GnssConfigLppeControlPlaneMask[Input]   GNSS Configure Control Plan Mask.

   @return
        LocationError

   @dependencies
       None.
*/
LocationError
FidlLocApi::setLPPeProtocolCpSync(GnssConfigLppeControlPlaneMask lppeCP) {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;


    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetLPPeProtocolCpSync)) {

        rtv = fidlReqIf->fidlSetLPPeProtocolCpSync(lppeCP, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }

    return err;
}


/**
   Set the LPPe Protocol User plan synchronize.

   @param GnssConfigLppeUserPlaneMask[Input]   GNSS Configure User Plan Mask.

   @return
        LocationError

   @dependencies
       None.
*/
LocationError
FidlLocApi::setLPPeProtocolUpSync(GnssConfigLppeUserPlaneMask lppeUP) {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetLPPeProtocolUpSync)) {

        rtv = fidlReqIf->fidlSetLPPeProtocolUpSync(lppeUP, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }

    return err;

}


/**
   Request for WWAN ZPP Fix- Position reprot.

   @param
        None

   @return
        None

   @dependencies
       None.
*/
void FidlLocApi::getWwanZppFix() {

    sendMsg(new LocApiMsg([this] () {
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlGetWwanZppFix)) {
            rtv= fidlReqIf->fidlGetWwanZppFix((void *)this);
            if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
                 LOC_LOGe("Error: %d", rtv);
            }
        } else {
            rtv = LOC_API_ADAPTER_ERR_UNSUPPORTED;
        }
        LOC_LOGd("Status: %d", rtv);
    }));

}

/**
   Request for Best Available ZPP Fix- Position reprot.

   @param
        None

   @return
        None

   @dependencies
       None.
*/
void FidlLocApi::getBestAvailableZppFix() {

    sendMsg(new LocApiMsg([this] () {
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlGetBestAvailableZppFix)) {
            rtv= fidlReqIf->fidlGetBestAvailableZppFix((void *)this);
            if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
                 LOC_LOGe("Error: %d", rtv);
            }
        } else {
            rtv = LOC_API_ADAPTER_ERR_UNSUPPORTED;
        }
        LOC_LOGd("Status: %d", rtv);
    }));

}


/**
   Set GPS Lock.

   @param GnssConfigGpsLock[Input]  Configure GPS Lock

   @return
        LocationError

   @dependencies
       None.
*/
LocationError FidlLocApi::setGpsLockSync(GnssConfigGpsLock lock) {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetGpsLockSync)) {

        rtv = fidlReqIf->fidlSetGpsLockSync(lock, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }

    return err;
}


/**
   Request Aiding Data

   @param GnssAidingDataSvMask[Input]  GNSS Aiding Data SV Mask

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::requestForAidingData(GnssAidingDataSvMask svDataMask) {

    sendMsg(new LocApiMsg([this, svDataMask] () {

        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlRequestForAidingData)) {

            rtv = fidlReqIf->fidlRequestForAidingData(svDataMask, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
               LOC_LOGe ("Error: %d", rtv);
            }
        } else {
            rtv = LOC_API_ADAPTER_ERR_UNSUPPORTED;
        }
    }));
}


/**
   Check XTRA Version Check

   @param uint32_t[Input]  XTRA Version Check

   @return
        LocationError.

   @dependencies
       None.
*/
LocationError
FidlLocApi::setXtraVersionCheckSync(uint32_t check) {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetXtraVersionCheckSync)) {

        rtv = fidlReqIf->fidlSetXtraVersionCheckSync(check, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }
    return err;
}


/**
   Set Constrained Tunc Mode

   @param enabled[Input]                     Enabled OR Disabled
   @param tuncConstraint[Input]              Number of AGPS Certificates
   @param energyBudget[Input]                Bit mask of Certificates

   @return
        None.

   @dependencies
       None.
*/
void FidlLocApi::setConstrainedTuncMode(bool enabled,
                                           float tuncConstraint,
                                           uint32_t energyBudget,
                                           LocApiResponse* adapterResponse) {

    sendMsg(new LocApiMsg([this, enabled, tuncConstraint, energyBudget, adapterResponse] () {
    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetConstrainedTuncMode)) {

        rtv = fidlReqIf->fidlSetConstrainedTuncMode(enabled, tuncConstraint,
                    energyBudget, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }
    if (adapterResponse != NULL) {
        adapterResponse->returnToSender(err);
    }
    }));
}


/**
   Enable Position Assisted Clock Estimator Mode

   @param enabled[Input]    Enable or disable Clock estimator.

   @return
        LocationError.

   @dependencies
       None.
*/
void FidlLocApi::setPositionAssistedClockEstimatorMode(bool enabled,
                                                          LocApiResponse* adapterResponse) {
    sendMsg(new LocApiMsg([this, enabled, adapterResponse] () {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) &&
            (nullptr != fidlReqIf->fidlSetPositionAssistedClockEstimatorMode)) {
        rtv = fidlReqIf->fidlSetPositionAssistedClockEstimatorMode(enabled, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }

    if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }

    }));
}


/**
   Get GNSS Engery Consumed

   @param
        None.

   @return
        LocationError.

   @dependencies
        None.
*/
void FidlLocApi::getGnssEnergyConsumed() {

    sendMsg(new LocApiMsg([this] {

    LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlGetGnssEnergyConsumed)) {
        rtv = fidlReqIf->fidlGetGnssEnergyConsumed((void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
            err = LOCATION_ERROR_SUCCESS;
        }
    } else {
        err = LOCATION_ERROR_NOT_SUPPORTED;
    }

    }));
}


/**
    Convert SUPL Version into GNSS SUPL Version format

    @param suplVersion[Input]     SUPL Version

    @return
        GnssConfigSuplVersion.

    @dependencies
        None.
*/
GnssConfigSuplVersion
FidlLocApi::convertSuplVersion(const uint32_t suplVersion) {

    GnssConfigSuplVersion configSuplVersion = GNSS_CONFIG_SUPL_VERSION_1_0_0;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlConvertSuplVersion)) {
        rtv = fidlReqIf->fidlConvertSuplVersion(suplVersion, &configSuplVersion, (void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
            configSuplVersion = GNSS_CONFIG_SUPL_VERSION_1_0_0;
        }
    }

    return configSuplVersion;
}


/**
    Convert LPPe Control Plan Profile vesrion in GNSS LPP Control Plan profile format


    @param lppeControlPlaneMask[Input]     LPPe Control Plan Mask


    @return
        GnssConfigLppeControlPlaneMask.

    @dependencies
        None.
*/
GnssConfigLppeControlPlaneMask
FidlLocApi::convertLppeCp(const uint32_t lppeControlPlaneMask) {
    GnssConfigLppeControlPlaneMask configLppeCp = 0;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlConvertLppeCp)) {
        rtv = fidlReqIf->fidlConvertLppeCp(lppeControlPlaneMask, &configLppeCp, (void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
            configLppeCp = 0;
        }
    }

    return configLppeCp;
}


/**
   Convert LPPe Control Plan Profile vesrion in GNSS LPP User Plan profile format

    @param lppeUserPlaneMask[Input]     LPPe User Plan Mask


    @return
        GnssConfigLppeUserPlaneMask.

    @dependencies
        None.
*/
GnssConfigLppeUserPlaneMask
FidlLocApi::convertLppeUp(const uint32_t lppeUserPlaneMask) {
    GnssConfigLppeUserPlaneMask configLppeUp = 0;
    enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlConvertLppeUp)) {
        rtv = fidlReqIf->fidlConvertLppeUp(lppeUserPlaneMask, &configLppeUp, (void *)this);
        if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
            configLppeUp = 0;
        }
    }
    return configLppeUp;
}


/**
   Set Black listed SV.

    @param GnssSvIdConfig[Input]     Black listed SV ID


    @return
        LocationError.

    @dependencies
        None.
*/
LocationError
FidlLocApi::setBlacklistSvSync(const GnssSvIdConfig& config) {

    LocationError rtv = LOCATION_ERROR_SUCCESS;
    enum loc_api_adapter_err adapRtv = LOC_API_ADAPTER_ERR_SUCCESS;

    if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetBlacklistSv)) {
        adapRtv = fidlReqIf->fidlSetBlacklistSv(config, ((void *)this));
        if (LOC_API_ADAPTER_ERR_SUCCESS != adapRtv) {
           LOC_LOGe ("Error: %d", adapRtv);
           rtv = LOCATION_ERROR_GENERAL_FAILURE;
        }
    } else {
        rtv = LOCATION_ERROR_NOT_SUPPORTED;
    }

    return rtv;
}


/**
   Set Black listed SV.

    @param GnssSvIdConfig[Input]     Black listed SV ID


    @return
        None.

    @dependencies
        None.
*/
void
FidlLocApi::setBlacklistSv(const GnssSvIdConfig& config, LocApiResponse* adapterResponse) {
    sendMsg(new LocApiMsg([this, config, adapterResponse] () {
        LocationError err = setBlacklistSvSync(config);
        if (adapterResponse) {
            adapterResponse->returnToSender(err);
        }
    }));
}


/**
   Get Black listed SV.

   @param
        None.


   @return
        None.

   @dependencies
        None.
*/
void FidlLocApi::getBlacklistSv() {
    sendMsg(new LocApiMsg([this] () {
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlGetBlacklistSv)) {
            rtv = fidlReqIf->fidlGetBlacklistSv((void *)this);
            if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
               LOC_LOGe ("Error: %d", rtv);
            }
        }
    }));
}


/**
   Configure Constellation Control.

   @param   GnssSvTypeConfig[Input]  GNSS Constellation configuration.


   @return
        None.

   @dependencies
        None.
*/
void
FidlLocApi::setConstellationControl(const GnssSvTypeConfig& config,
                                       LocApiResponse *adapterResponse) {

    sendMsg(new LocApiMsg([this, config, adapterResponse] () {
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlSetConstellationControl)) {
            rtv = fidlReqIf->fidlSetConstellationControl(config, (void *)this);
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            } else {
               LOC_LOGe ("Error: %d", rtv);
            }
        }
        if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }
    }));
}

/**
   Request get configured Constellation from Hardware.

   @param
        None.

   @return
        None.

   @dependencies
        None.
*/
void
FidlLocApi::getConstellationControl() {

    sendMsg(new LocApiMsg([this] () {
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlGetConstellationControl)) {
            rtv = fidlReqIf->fidlGetConstellationControl((void *)this);
            if (LOC_API_ADAPTER_ERR_SUCCESS != rtv) {
               LOC_LOGe ("Error: %d", rtv);
            }
        }
    }));
}


/**
   Request to reset configured Constellation from Hardware.

   @param
        None.

   @return
        None.

   @dependencies
        None.
*/
void
FidlLocApi::resetConstellationControl(LocApiResponse *adapterResponse) {
    sendMsg(new LocApiMsg([this, adapterResponse] () {
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlResetConstellationControl)) {
            rtv = fidlReqIf->fidlResetConstellationControl((void *)this);
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            } else {
               LOC_LOGe ("Error: %d", rtv);
            }
        }
        if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }
    }));
}

void
FidlLocApi::startTimeBasedTracking(const TrackingOptions& options,
         LocApiResponse* adapterResponse) {

    sendMsg(new LocApiMsg([this, options, adapterResponse] () {
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
        fidlPosMode posMode;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlStartFix)) {

            posMode.mode = LOC_POSITION_MODE_STANDALONE;
            posMode.recurrence = LOC_GPS_POSITION_RECURRENCE_PERIODIC;
            posMode.min_interval = options.minInterval;
            posMode.preferred_accuracy = 100;
            posMode.preferred_time = 120000;
            posMode.share_position = true;
            posMode.powerMode = GNSS_POWER_MODE_M2;
            posMode.timeBetweenMeasurements = options.tbm;

            rtv = fidlReqIf->fidlStartFix(posMode, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            }
        } else {
            err = LOCATION_ERROR_NOT_SUPPORTED;
        }
        if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }
    }));

}

void
FidlLocApi::stopTimeBasedTracking(LocApiResponse* adapterResponse){

    sendMsg(new LocApiMsg([this, adapterResponse] () {
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlStopFix)) {

            rtv = fidlReqIf->fidlStopFix((void *)this);
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            }
        } else {
            err = LOCATION_ERROR_NOT_SUPPORTED;
        }

        if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }
    }));
}

void
FidlLocApi::startDistanceBasedTracking(uint32_t sessionId,
        const LocationOptions& options, LocApiResponse* adapterResponse) {

    sendMsg(new LocApiMsg([this, options, adapterResponse] () {
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;
        fidlPosMode posMode;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlStartFix)) {

            posMode.mode = LOC_POSITION_MODE_STANDALONE;
            posMode.recurrence = LOC_GPS_POSITION_RECURRENCE_PERIODIC;
            posMode.min_interval = options.minInterval;
            posMode.preferred_accuracy = 100;
            posMode.preferred_time = 120000;
            posMode.share_position = true;
            posMode.powerMode = GNSS_POWER_MODE_M2;
            posMode.timeBetweenMeasurements = 1000;

            rtv = fidlReqIf->fidlStartFix(posMode, ((void *)this));
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            }
        } else {
            err = LOCATION_ERROR_NOT_SUPPORTED;
        }
        if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }
    }));
}

void
FidlLocApi::stopDistanceBasedTracking(uint32_t sessionId,
         LocApiResponse* adapterResponse) {

    sendMsg(new LocApiMsg([this, adapterResponse] () {
        LocationError err = LOCATION_ERROR_GENERAL_FAILURE;
        enum loc_api_adapter_err rtv = LOC_API_ADAPTER_ERR_SUCCESS;

        if ((nullptr != fidlReqIf) && (nullptr != fidlReqIf->fidlStopFix)) {

            rtv = fidlReqIf->fidlStopFix((void *)this);
            if (LOC_API_ADAPTER_ERR_SUCCESS == rtv) {
                err = LOCATION_ERROR_SUCCESS;
            }
        } else {
            err = LOCATION_ERROR_NOT_SUPPORTED;
        }

        if (adapterResponse != NULL) {
            adapterResponse->returnToSender(err);
        }
    }));
}

void
FidlLocApi::updateEngineState(bool currentEngineState) {
    mEngineOn = currentEngineState;
}

