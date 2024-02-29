/*
Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.

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

#define LOG_TAG "LOC_IDL_CLIENT"
#include <iostream>
#include <string>

#ifndef _WIN32
#include <unistd.h>
#endif
#include <chrono>
#include <future>
#include <CommonAPI/CommonAPI.hpp>
#include <v0/com/qualcomm/qti/location/LocIdlAPIProxy.hpp>
#include <time.h>
#include "log_util.h"
#include <gptp_helper.h>

using namespace v0::com::qualcomm::qti::location;
using namespace std;
#define NSEC_IN_ONE_SEC       (1000000000ULL)   /* nanosec in a sec */

int main() {
    CommonAPI::Runtime::setProperty("LogContext", "LOCIDL");
    CommonAPI::Runtime::setProperty("LogApplication", "LOCIDL");
    CommonAPI::Runtime::setProperty("LibraryBase", "LocIdlAPI");

    std::shared_ptr < CommonAPI::Runtime > runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "com.qualcomm.qti.location.LocIdlAPI";
    std::string connection = "client-sample";

    std::shared_ptr<LocIdlAPIProxy<>> myProxy = runtime->buildProxy<LocIdlAPIProxy>(domain,
            instance, connection);

    LOC_LOGd("Checking IDL Service availability !!");
    while (!myProxy->isAvailable())
        usleep(10);
    LOC_LOGd(" IDL Service is now available !!");

    if (gptpInit()) {
        LOC_LOGd(" GPTP init success !!");
    } else {
        LOC_LOGe(" GPTP init failed !!");
    }
    LocIdlAPI::IDLLocationCapabilitiesMask capsMask;
    // Subscribe for receiving GNSS Reports
    myProxy->getGnssCapabilitiesMaskAttribute().getChangedEvent().subscribe([&](const uint32_t &val)
    {
            LOC_LOGd(" Received caps change event: %d", val);
    });

    //IDLLocationReport locationReport
    myProxy->getLocationReportEvent().subscribe(
        [&](const ::v0::com::qualcomm::qti::location::LocIdlAPI::\
            IDLLocationReport &locReport) {
            const LocIdlAPI::IDLLocation &location = locReport.getLocInfo();

            LOC_LOGi("TimeStamp %" PRIu64 ", Latitude %f Longitude %f Altitude %f ",
                    location.getTimestamp(), location.getLatitude(),
                    location.getLongitude(), location.getAltitude());

            const LocIdlAPI::IDLLocationReportSvUsedInPosition &svUsed =
                    locReport.getSvUsedInPosition();
            const LocIdlAPI::IDLLocationReportPositionDynamics &posDynamics =
                     locReport.getBodyFrameData();
            const LocIdlAPI::IDLGnssSystemTime &gnssTime = locReport.getGnssSystemTime();
            const LocIdlAPI::IDLSystemTimeStructUnion &time = gnssTime.getTimeUnion();

            if (time.isType<LocIdlAPI::IDLGnssSystemTimeStructType>()) {
                    const LocIdlAPI::IDLGnssSystemTimeStructType &systemTime =
                    time.get<LocIdlAPI::IDLGnssSystemTimeStructType>();
           }

            const std::vector<LocIdlAPI::IDLGnssMeasUsageInfo> &meas =
                    locReport.getMeasUsageInfo();

            const LocIdlAPI::IDLLLAInfo &lla = locReport.getLlaVRPBased();

            const std::vector<float> &emu = locReport.getEnuVelocityVRPBased();

            const std::vector<uint16_t> &dgnss = locReport.getDgnssStationId();

            /**For Latency related Caluculations */
            uint64_t currPTPtime = 0;
            gptpGetCurPtpTime(&currPTPtime);
            int64_t latency = 0;
            latency = currPTPtime - locReport.getElapsedgPTPTime();
            LOC_LOGv("TimeStamp %" PRIu64 ", PTP time Position %" PRIu64 ", "
                    " Current PTP time %" PRIu64 " Latency %" PRId64 " ",
                    location.getTimestamp(), locReport.getElapsedgPTPTime(),
                    currPTPtime, latency);
        });

    // Subscribe for Measurement reports
    myProxy->getGnssMeasurementsEvent().subscribe(
        [&](const LocIdlAPI::IDLGnssMeasurements& gnssMeasurements) {

        const LocIdlAPI::IDLGnssMeasurementsClock &clk = gnssMeasurements.getClock();
        /**For Latency related Caluculations */
        uint64_t currPTPtime = 0;
        gptpGetCurPtpTime(&currPTPtime);
        int64_t latency = 0;
        latency = currPTPtime - clk.getElapsedgPTPTime();
        LOC_LOGv("PTP time Meas %" PRIu64 ", "
                " Current PTP time %" PRIu64 " Latnecy %" PRId64 " ",
                clk.getElapsedgPTPTime(), currPTPtime, latency);

        const vector<LocIdlAPI::IDLGnssMeasurementsData > &measData =
                gnssMeasurements.getMeasurements();
        LOC_LOGd("Received gnssMeasurements isNhz -- %d", gnssMeasurements.getIsNHz());
    });

    myProxy->getGnssSvEvent().subscribe(
        [&](const std::vector<::v0::com::qualcomm::qti::location::LocIdlAPI::IDLGnssSv> &gnssSv) {
        LOC_LOGd("Number of SV's recevived -- %d", gnssSv.size());
    });

    myProxy->getGnssNmeaEvent().subscribe([&](const uint64_t timestamp, const std::string nmea){
        LOC_LOGv("NMEA timestamp: %" PRIu64 ", String %s ",
                timestamp, nmea);
    });

    myProxy->getGnssDataEvent().subscribe([&](const LocIdlAPI::IDLGnssData& gnssData){
       vector<uint32_t> dataMask = gnssData.getGnssDataMask();
       vector<double> jammerInd = gnssData.getJammerInd();
       vector<double> agc = gnssData.getAgc();
    });


    CommonAPI::CallStatus callStatus;
    std::string returnMessage;

    CommonAPI::CallInfo info(1000);
    info.sender_ = 1234;
    uint32_t _intervalInMs = 100;
    LocIdlAPI::IDLLocationResponse resp;
    uint32_t reportCbMask = \
        LocIdlAPI::IDLGnssReportCbInfoMask::IDL_LOC_CB_INFO_BIT |
        LocIdlAPI::IDLGnssReportCbInfoMask::IDL_SV_CB_INFO_BIT  |
        LocIdlAPI::IDLGnssReportCbInfoMask::IDL_NMEA_CB_INFO_BIT |
        LocIdlAPI::IDLGnssReportCbInfoMask::IDL_DATA_CB_INFO_BIT |
        LocIdlAPI::IDLGnssReportCbInfoMask::IDL_1HZ_MEAS_CB_INFO_BIT;

    myProxy->startPositionSession(_intervalInMs, reportCbMask, callStatus, resp, &info);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        return -1;
    }
/*
    //// TEST- CODE for LIA API's
    LocIdlAPI::IDLAidingDataDeletionMask aidMask = \
    LocIdlAPI::IDLAidingDataDeletionMask::IDL_AIDING_DATA_DELETION_DR_SENSOR_CALIBRATION;
    LocIdlAPI::IDLLocIntegrationResponse respLIA;
    std::cout << "Sending Mask" << std::endl;

    myProxy->deleteAidingData( aidMask, callStatus, respLIA, &info);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        return -1;
    }

   std::vector< LocIdlAPI::IDLGnssSvIdInfo > blacklist;
   for (int i = 0; i<2; i++) {
       LocIdlAPI::IDLGnssSvIdInfo sv ={};
       if (i == 0) {
           sv.setConstellation(LocIdlAPI::IDLGnssConstellationType::\
                   IDL_GNSS_CONSTELLATION_TYPE_NAVIC);
           sv.setSvId(407);
       } else {
           sv.setConstellation(LocIdlAPI::IDLGnssConstellationType::\
                   IDL_GNSS_CONSTELLATION_TYPE_GLONASS);
           sv.setSvId(67);
       }
     blacklist.push_back(sv);
   }

    myProxy->configConstellations(blacklist, callStatus, respLIA, &info);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        return -1;
    }
*/

    while (true) {
       sleep(1);
       /* Test code for stop session
       struct timespec bootTime;
       clock_gettime(CLOCK_BOOTTIME, &bootTime);
       if (120 < bootTime.tv_sec) {
       myProxy->stopPositionSession(callStatus, &info);
       } */
       info.timeout_ = info.timeout_ + 1000;
       sleep(1);
    }

    return 0;
}
