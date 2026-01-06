/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */
#include <IzatRemoteApi.h>
#include <loc_cfg.h>
#include <thread>
#include <unistd.h>
#include <log_util.h>
#include <inttypes.h>
#include <stdlib.h>
#include <NLPApi.h>
#include <cstdlib>

#define  LOG_TAG "ZaxisTest"

using namespace std;
using namespace qc_loc_fw;


static const AltitudeReceiver* altReceiver  = nullptr;
static const SystemRequester*  sysRequester = nullptr;

static void onAlititudeLookupRequest(const LocationExt* location, bool isEmergency,
                                     const void* clientData) {
    LOC_LOGi("onAlititudeLookupRequest:latitude:%lf, longitude:%lf, altitude:%lf, accuracy:%lf,"
            "isEmergency:%d, elapsedRealTimeInMs: %lld",
            location->loc.latitude, location->loc.longitude,
            location->loc.altitude, location->loc.accuracy, isEmergency,
            location->elapsedRealTimeInMs);
        LocationExt locReport;
        //For a valid altitude pushed from ZProvider, below fields are required
        //latitude/longitude/timestamp/elapsedRealTimeInMs shall be copied from location
        //received in onAlititudeLookupRequest
        /********* required fields *************/
        locReport.loc.latitude = location->loc.latitude;
        locReport.loc.longitude = location->loc.longitude;
        locReport.loc.altitude = 30.03;
        locReport.loc.verticalAccuracy = 1.002;
        locReport.loc.locationFlagsMask =
            (NlpLocationFlagsBits)(LAT_LONG_BIT | ALTITUDE_BIT | VERTICAL_ACCURACY_BIT);
        locReport.elapsedRealTimeInMs = location->elapsedRealTimeInMs;
        /********* required fiels end *************/

        locReport.loc.positionSource = location->loc.positionSource;
        locReport.loc.timestamp = location->loc.timestamp;
        locReport.loc.speed = location->loc.speed;
        locReport.loc.bearing = location->loc.bearing;
        locReport.loc.accuracy = location->loc.accuracy;
        locReport.loc.speedAccuracy = location->loc.speedAccuracy;
        locReport.loc.bearingAccuracy = location->loc.speedAccuracy;

    thread t([=](){

        //Process time
        int processTime[] = {200, 300, 2100, 500, 6000, 7000, 3200, 4000};
        int idx = std::rand() % 8;
        usleep(processTime[idx] * 1000);
        if (altReceiver) {
            LOC_LOGi("processTime: %d, elapsedRealTimeInMs:%" PRIi64 ", pushAltitude",
                    processTime[idx],
                    locReport.elapsedRealTimeInMs);
            altReceiver->pushAltitude(&locReport);
        }

    });
    t.detach();
}

static void onLocationOptInUpdate(OptInStatus optInStatus, const void* clientData) {

}

static void onNetworkStatusUpdate(bool isConected, const NlpNetwork* networksAvailable,
        uint8_t networksAvailableCount, const void* clientData) {
}

int main() {
    UTIL_READ_CONF_DEFAULT(LOC_PATH_GPS_CONF);
    LOC_LOGd("ZAxis test app starts!");
    AltitudeReceiverResponseListener altListener;
    SystemStatusListener sysListener;
    altListener.onAltitudeLookupRequest = onAlititudeLookupRequest;
    sysListener.onLocationOptInUpdate = onLocationOptInUpdate;
    sysListener.onNetworkStatusUpdate = onNetworkStatusUpdate;
    const NLPApi* nlpApi = nullptr;
    nlpApi = linkGetNLPApi();
    if (nullptr != nlpApi) {
        altReceiver = nlpApi->connectToAltitudeReceiver(&altListener, nullptr);
        nlpApi->connectToSystemStatus(&sysListener);
    } else {
        LOC_LOGe("NLP API is nullptr");
    }

    sleep(10000000);

    return 0;
}
