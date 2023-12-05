/*
* Copyright (c) 2023 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/

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
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <gptp_helper.h>

using namespace v0::com::qualcomm::qti::location;
using namespace std;
#define NSEC_IN_ONE_SEC       (1000000000ULL)   /* nanosec in a sec */
#define GPTP_IF_LIB_NAME      "libgptp.so"

void gptpUpdateNotification(struct gptp_update update);
const char * libName = GPTP_IF_LIB_NAME;
void *gPTPLibHandle = nullptr;
std::shared_ptr<LocIdlAPIProxy<>> myProxy;
CommonAPI::CallInfo info(1000);
bool     sessionStarted;
uint32_t mask;
uint32_t pvtSubscription;
uint32_t svSubscription;
uint32_t nmeaSubscription;
uint32_t measSubscription;
uint32_t nHzmeasSubscription;
uint32_t dataSubscription;

const static gPTPLibInterfaceEvent gPTPEvent = {
   gptpUpdateNotification,
};
const static gPTPLibInterfaceReq  *gPTPReqIf = nullptr;

void gptpUpdateNotification(struct gptp_update update)
{
    cout << "GPTP Update Notification" << endl;
}

void loadGptpLibFile(void)
{
    char *errorDll;

    if ((gPTPLibHandle = dlopen(libName, RTLD_NOW)) != nullptr) {
        printf("%s is present", libName);
        get_gPTPLib_if_t getter = (get_gPTPLib_if_t)dlsym(gPTPLibHandle, "get_gPTPLib_if");

        if ((errorDll = dlerror()) != nullptr) {
            printf("dlsym for %s get_gPTPLib_if failed, error = %s", libName, errorDll);
            getter = nullptr;
        }

        if (getter != nullptr) {
            gPTPReqIf = (getter)(&gPTPEvent);
            if (gPTPReqIf != nullptr) {
                return;
            } else {
                printf("%s lib provided Command Interface as NULL", libName);
            }
        }
    } else {
        errorDll = dlerror();

        printf("dlopen for %s failed, handle %p error: %s", libName, gPTPLibHandle,
             ((nullptr != errorDll) ? errorDll : "No Error"));
    }
}

void ToolUsage()
{
    cout << " Usage : " << endl;
    cout << " LocIdlAPIClient -m <interested reports in decimal> -d "
                                            "<test duration in seconds>" << endl;
    cout << " ===========================================" << endl<<endl;
    cout << " Example Case 1: No argument - deafult values will be used(-m 3 -d 60)" << endl;
    cout << " LocIdlAPIClient" <<endl;
    cout << " ===========================================" << endl<<endl;
    cout << " Example : For all the reports and test duration 300sec" << endl;
    cout << " LocIdlAPIClient -m 31 -d 300" <<endl<<endl;
    cout << " Example : For Position, SV, NMEA reports and test duration 300sec" << endl;
    cout << " LocIdlAPIClient -m 7 -d 300" <<endl;
    cout << " Bit mask definition" << endl ;
    cout << " REPORT_NHZ_PVT    0x01   1" <<endl;
    cout << " REPORT_SV         0x02   2" <<endl;
    cout << " REPORT_NMEA       0x04   4" <<endl;
    cout << " REPORT_GNSSDATA   0x08   8" <<endl;
    cout << " REPORT_1HZ_MEAS   0x10   16" <<endl;
    cout << " ===========================================" << endl<<endl;
    return;
}

void printMeasurement(const LocIdlAPI::IDLGnssMeasurements& gnssMeasurements)
{
    static unsigned int measCount;
    uint64_t gptp_time_ns = 0;
    const LocIdlAPI::IDLGnssMeasurementsClock &clk = gnssMeasurements.getClock();

    measCount += 1;
     if ((nullptr != gPTPReqIf) && (nullptr != gPTPReqIf->gptpGetCurPtpTimeIf))
        gPTPReqIf->gptpGetCurPtpTimeIf(&gptp_time_ns);

    cout << "MeasurementReport Cnt ----------  :  " <<measCount << " Latency is: "
            ""<< gptp_time_ns - clk.getElapsedgPTPTime() << endl;

    cout << "Clk Flags     " << clk.getFlags() << endl;
    cout << "LeapSecond    " << clk.getLeapSecond() << endl;
    cout << "TimeNs        " << clk.getTimeNs() << endl;
    cout << "TimeUncNs     " << clk.getTimeUncertaintyNs() << endl;
    cout << "FullBiasNs    " << clk.getFullBiasNs() << endl;
    cout << "BiasNs        " << clk.getBiasNs() << endl;
    cout << "BiasUncNs     " << clk.getBiasUncertaintyNs() << endl;
    cout << "DriftNsps     " << clk.getDriftNsps() << endl;
    cout << "DriftUncNsps  " << clk.getDriftUncertaintyNsps() << endl;
    cout << "HwClockCount  " << clk.getHwClockDiscontinuityCount() << endl;

    const vector<LocIdlAPI::IDLGnssMeasurementsData > &measData =
                                    gnssMeasurements.getMeasurements();
    for (uint16_t idx = 0; idx < measData.size(); idx++) {
        cout <<"Idx  "<< idx << endl;

        cout <<"MeasFlags "<< measData[idx].getFlags() << endl;
        cout <<"svId "<< measData[idx].getSvType() << endl;

        cout <<"svType "<< measData[idx].getSvType() << endl;
        cout <<"timeOffsetNs"<< measData[idx].getTimeOffsetNs() << endl;
        cout <<"stateMask "<< measData[idx].getStateMask() << endl;
        cout <<"receivedSvTimeNs "<< measData[idx].getReceivedSvTimeNs() << endl;
        cout <<"receivedSvTimeSubNs "<< measData[idx].getReceivedSvTimeSubNs() << endl;
        cout <<"receivedSvTimeUncertaintyNs "
                "" << measData[idx].getReceivedSvTimeUncertaintyNs() << endl;

        cout <<"carrierToNoiseDbHz "<< measData[idx].getCarrierToNoiseDbHz() << endl;
        cout <<"pseudorangeRateMps "<< measData[idx].getPseudorangeRateMps() << endl;
        cout <<"pseudorangeRateUncertaintyMps "
                "" << measData[idx].getPseudorangeRateUncertaintyMps() << endl;
        cout <<"adrStateMask "<< measData[idx].getAdrStateMask() << endl;
        cout <<"adrMeters "<< measData[idx].getAdrMeters() << endl;
        cout <<"adrUncertaintyMeters "<< measData[idx].getAdrUncertaintyMeters() << endl;
        cout <<"carrierFrequencyHz "<< measData[idx].getCarrierFrequencyHz()  << endl;

        cout <<"carrierCycles "<< measData[idx].getCarrierCycles() << endl;
        cout <<"carrierPhaseUncertainty "<< measData[idx].getCarrierPhase() << endl;
        cout <<"carrierPhaseUncertainty "<< measData[idx].getCarrierPhaseUncertainty() << endl;
        cout <<"multipathIndicator "<< measData[idx].getMultipathIndicator() << endl;
        cout <<"signalToNoiseRatioDb "<< measData[idx].getSignalToNoiseRatioDb() << endl;
        cout <<"agcLevelDb "<< measData[idx].getAgcLevelDb() << endl;

        cout <<"basebandCarrierToNoiseDbHz "
                "" << measData[idx].getBasebandCarrierToNoiseDbHz() << endl;
        cout <<"gnssSignalType "<< measData[idx].getGnssSignalType() << endl;
        cout <<"fullInterSignalBiasNs "<< measData[idx].getFullInterSignalBiasNs() << endl;
        cout <<"fullInterSignalBiasUncertaintyNs "
                ""<< measData[idx].getFullInterSignalBiasUncertaintyNs() << endl;
        cout <<"cycleSlipCount "<< static_cast<int>(measData[idx].getCycleSlipCount()) << endl;
    }

    cout << "----- Received gnssMeasurements isNhz --" << gnssMeasurements.getIsNHz()<<  endl;
    cout << "-------" << endl;
}

void printPosResport(const LocIdlAPI::IDLLocationReport &_locationReport)
{
    const LocIdlAPI::IDLLocation &location = _locationReport.getLocInfo();

    static unsigned int posCount;
    uint64_t gptp_time_ns = 0;

    posCount += 1;
     if ((nullptr != gPTPReqIf) && (nullptr != gPTPReqIf->gptpGetCurPtpTimeIf))
        gPTPReqIf->gptpGetCurPtpTimeIf(&gptp_time_ns);
    cout << "PositionReport Cnt ----------  :  " <<posCount << " Latency is: "
        ""  << gptp_time_ns - _locationReport.getElapsedgPTPTime() << " ns ptp time: "
        "" << gptp_time_ns << " ns ElapsedgPTPTime: "
        "" << _locationReport.getElapsedgPTPTime() <<" ns GPS Timestamp: "
        "" << location.getTimestamp() << " ms " << endl;

    cout << "getLocationCbEvent  ----------   " << endl;
    cout << "TimeStamp      " << location.getTimestamp() << endl;
    cout << "Latitude       " << location.getLatitude() << endl;
    cout << "Longitude      " << location.getLongitude() << endl;
    cout << "Altitude       " << location.getAltitude() << endl;
    cout << "Speed          " << location.getSpeed() << endl;

    cout << "Bearing        " << location.getBearing() << endl;
    cout << "HorizontalAccuracy" << location.getHorizontalAccuracy() << endl;
    cout << "VerticalAccuracy  " << location.getVerticalAccuracy() << endl;
    cout << "SpeedAccuracy     " << location.getSpeedAccuracy() << endl;
    cout << "BearingAccuracy   " << location.getBearingAccuracy() << endl;

    cout << "TechMask          " << location.getTechMask() << endl;
    cout << "ElapsedRealTimeNs " << location.getElapsedRealTimeNs() << endl;
    cout << "ElapsedRealTimeUncNs " << location.getElapsedRealTimeUncNs() << endl;
    cout << "TimeUncMs         " << location.getTimeUncMs() << endl;

    cout << "Flags             " << location.getFlags() << endl;
    cout << "AltitudeMeanSeaLevel  "<< _locationReport.getAltitudeMeanSeaLevel() << endl;
    cout << "pDop             " << _locationReport.getPdop() << endl;
    cout << "HDop  " << _locationReport.getHdop() << endl;
    cout << "VDop  " << _locationReport.getVdop() << endl;
    cout << "GDop  " << _locationReport.getGdop() << endl;
    cout << "TDop  " << _locationReport.getTdop() << endl;

    cout << "MagneticDeviation       "<< _locationReport.getMagneticDeviation() << endl;
    cout << "HorReliability          "<< _locationReport.getHorReliability() << endl;
    cout << "VerReliability          "<< _locationReport.getVerReliability() << endl;
    cout << "HorUncEllipseSemiMajor  "<< _locationReport.getHorUncEllipseSemiMajor() << endl;
    cout << "HorUncEllipseSemiMinor  " << _locationReport.getHorUncEllipseSemiMinor() << endl;
    cout << "HorUncEllipseOrientAzimuth  "
        "" << _locationReport.getHorUncEllipseOrientAzimuth() << endl;

    cout << "NorthStdDeviation       "<< _locationReport.getNorthStdDeviation() << endl;
    cout << "EastStdDeviation        "<< _locationReport.getEastStdDeviation() << endl;
    cout << "NorthVelocity           "<< _locationReport.getNorthVelocity() << endl;
    cout << "EastVelocity            "<< _locationReport.getEastVelocity() << endl;
    cout << "UpVelocity              "<< _locationReport.getUpVelocity() << endl;

    cout << "NorthVelocityStdDeviation"<< _locationReport.getNorthVelocityStdDeviation() << endl;
    cout << "EastVelocityStdDeviation "<< _locationReport.getEastVelocityStdDeviation() << endl;
    cout << "UpVelocityStdDeviatio    "<< _locationReport.getUpVelocityStdDeviation() << endl;
    cout << "NumSvUsedInPosition      " << _locationReport.getNumSvUsedInPosition() << endl;

    const LocIdlAPI::IDLLocationReportSvUsedInPosition &svUsed =
                            _locationReport.getSvUsedInPosition();
    cout << "GpsSvUsedIdsMask     "<< svUsed.getGpsSvUsedIdsMask() << endl;
    cout << "GalSvUsedIdsMask     "<< svUsed.getGalSvUsedIdsMask() << endl;
    cout << "BdsSvUsedIdsMask     "<< svUsed.getBdsSvUsedIdsMask() << endl;
    cout << "QzssSvUsedIdsMask    "<< svUsed.getQzssSvUsedIdsMask() << endl;
    cout << "NavicSvUsedIdsMask   "<< svUsed.getNavicSvUsedIdsMask() << endl;
    cout << "GloSvUsedIdsMask     "<< svUsed.getGloSvUsedIdsMask() << endl;


    cout << "NavSolutionMask      "<< _locationReport.getNavSolutionMask() << endl;
    cout << "PosTechMask          "<< _locationReport.getPosTechMask() << endl;

    const LocIdlAPI::IDLLocationReportPositionDynamics &posDynamics =
                                _locationReport.getBodyFrameData();
    cout << "BodyFrameDataMask    "<< posDynamics.getBodyFrameDataMask() << endl;
    cout << "LongAccel            "<< posDynamics.getLongAccel() << endl;
    cout << "LatAccel             "<< posDynamics.getLatAccel() << endl;
    cout << "VertAccel            "<< posDynamics.getVertAccel() << endl;
    cout << "LongAccelUnc         "<< posDynamics.getLongAccelUnc() << endl;
    cout << "LatAccelUnc          "<< posDynamics.getLatAccelUnc() << endl;
    cout << "VertAccelUnc         "<< posDynamics.getVertAccelUnc() << endl;
    cout << "Pitch                "<< posDynamics.getPitch() << endl;
    cout << "PitchUnc             "<< posDynamics.getPitchUnc() << endl;
    cout << "PitchRateUnc         "<< posDynamics.getPitchRateUnc() << endl;
    cout << "PitchRate            "<< posDynamics.getPitchRate() << endl;
    cout << "Roll                 "<< posDynamics.getRoll() << endl;
    cout << "Roll Unc             "<< posDynamics.getRollUnc() << endl;
    cout << "Roll Rate            "<< posDynamics.getRollRate() << endl;
    cout << "Roll Rate Unc        "<< posDynamics.getRollRateUnc() << endl;
    cout << "Yaw                  "<< posDynamics.getYaw() << endl;
    cout << "YawUnc               "<< posDynamics.getYawUnc() << endl;
    cout << "YawRate              "<< posDynamics.getYawRate() << endl;
    cout << "YawRateUnc           "<< posDynamics.getYawRateUnc() << endl;

    const LocIdlAPI::IDLGnssSystemTime &gnssTime = _locationReport.getGnssSystemTime();
    cout << "getLocationCbEvent GnssSystemTimeSrc    "
          "" << gnssTime.getGnssSystemTimeSrc() << endl;
    const LocIdlAPI::IDLSystemTimeStructUnion &time = gnssTime.getTimeUnion();
    if(time.isType<LocIdlAPI::IDLGnssSystemTimeStructType>()) {
          const LocIdlAPI::IDLGnssSystemTimeStructType &systemTime =
                            time.get<LocIdlAPI::IDLGnssSystemTimeStructType>();
          cout <<"SystemWeek      " <<systemTime.getSystemWeek() <<endl ;
          cout <<"SystemWeekMs    " <<systemTime.getSystemMsec() <<endl ;
          cout <<"SysClkTimeBias  " <<systemTime.getSystemClkTimeBias() <<endl ;
          cout <<"SysClkTimeUncMs " <<systemTime.getSystemClkTimeUncMs() <<endl ;
          cout <<"RefFCount       " <<systemTime.getRefFCount() <<endl ;
          cout <<"NumClockResets  " <<systemTime.getNumClockResets() <<endl ;
    }

    const vector<LocIdlAPI::IDLGnssMeasUsageInfo> &meas = _locationReport.getMeasUsageInfo();
    for (int idx = 0; idx < meas.size() && idx < 176; idx++) {
        cout << "GnssConstellation    "<< meas[idx].getGnssConstellation()<<endl ;
        cout << "GnssSignalType       "<< meas[idx].getGnssSignalType()<<endl ;
        cout << "GnssSvId             "<< meas[idx].getGnssSvId()<<endl ;
    }

    cout << "LeapSeconds          " << static_cast<int>(_locationReport.getLeapSeconds()) << endl;
    cout << "CalibrationConfidence "
        "" << static_cast<int>(_locationReport.getCalibrationConfidencePercent()) << endl;
    cout << "CalibrationStatus    " << _locationReport.getCalibrationStatus() << endl;
    cout << "LocOutputEngType     " << _locationReport.getLocOutputEngType() << endl;
    cout << "LocOutputEngMask     " << _locationReport.getLocOutputEngMask() << endl;
    cout << "ConformityIndex      " << _locationReport.getConformityIndex() << endl;

    const LocIdlAPI::IDLLLAInfo &lla = _locationReport.getLlaVRPBased();
    cout << "Latitude             " << lla.getLatitude() << endl;
    cout << "Longitude            " << lla.getLongitude() << endl;
    cout << "Altitude             " << lla.getAltitude() << endl;

    const vector<float> &emu = _locationReport.getEnuVelocityVRPBased();
    for (int k = 0; k < emu.size(); k++) {
        cout << "Emu                  " << emu[k] << endl;
    }

    cout << "DrSolutionStatusMask " << _locationReport.getDrSolutionStatusMask() << endl;
    cout << "AltitudeAssumed      " << _locationReport.getAltitudeAssumed() << endl;
    cout << "SessionStatus        " << _locationReport.getSessionStatus() << endl;
    cout << "IntegrityRiskUsed    " << _locationReport.getIntegrityRiskUsed() << endl;
    cout << "ProtectAlongTrack    " << _locationReport.getProtectAlongTrack() << endl;
    cout << "ProtectCrossTrack    " << _locationReport.getProtectCrossTrack() << endl;
    cout << "ProtectVertical      " << _locationReport.getProtectVertical() << endl;
    cout << "LocationInfoFlags    " << _locationReport.getLocationInfoFlags() << endl;

    const vector<uint16_t> &dgnss = _locationReport.getDgnssStationId();
    for (int l = 0; l < dgnss.size(); l++) {
           cout << "DgnssStationId        " << dgnss[l] << endl;
    }
    cout << "ElapsedPTPTimeNs  " << _locationReport.getElapsedgPTPTime() << endl;
    cout << "-------" << endl;
}

void printGnssData(const LocIdlAPI::IDLGnssData& gnssData)
{
   vector<uint32_t> dataMask = gnssData.getGnssDataMask();
   vector<double> jammerInd = gnssData.getJammerInd();
   vector<double> agc = gnssData.getAgc();
   static unsigned int dataCount;

   dataCount += 1;
   cout << "DataCb Cnt ----------  :  " <<dataCount <<endl;
   for  (int i = 0; i < dataMask.size(); i++) {
       cout << "Mask " << dataMask[i] << " " << "JammeInd "
            "" << jammerInd[i] << " " << "Agc " << agc[i] << endl;
   }
}

void printSVInfo(const vector<LocIdlAPI::IDLGnssSv> &gnssSv)
{
    static unsigned int svCount;

    svCount += 1;
    cout << "SvReport Cnt ----------  :  " <<svCount << endl;

    cout << "No of SV " << gnssSv.size() << endl;
    for (uint16_t idx = 0; idx < gnssSv.size(); idx++) {
        cout << "svId  " << gnssSv[idx].getSvId() << endl;
        cout << "Type    " << gnssSv[idx].getType() << endl;
        cout << "CN0Dbhz  " << gnssSv[idx].getCN0Dbhz() << endl;
        cout << "setElevation    " << gnssSv[idx].getElevation() << endl;
        cout << "Azimuth  " << gnssSv[idx].getAzimuth() << endl;
        cout << "CarrierFrequencyHz    " << gnssSv[idx].getCarrierFrequencyHz() << endl;
        cout << "GnssSignalTypeMask" << gnssSv[idx].getGnssSignalTypeMask() << endl;
        cout << "BasebandCarrierToNoiseDbHz    "
            "" << gnssSv[idx].getBasebandCarrierToNoiseDbHz() << endl;
        cout << "GloFrequency" << gnssSv[idx].getGloFrequency() << endl;
    }
}

void printNmea(const uint64_t timestamp, const string &nmea)
{
    static unsigned int nmeaCount;

    nmeaCount += 1;
    cout << "NMEAReport Cnt ----------  :  " <<nmeaCount << endl;
    cout << "NMEA timestamp" << timestamp << endl;
    cout << "NMEA String" << nmea << endl;
}

void DeInitHandles()
{
    CommonAPI::CallStatus callStatus;

    if (sessionStarted) {
        if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_DATA_CB_INFO_BIT) {
            myProxy->getGnssDataEvent().unsubscribe(dataSubscription);
        }
        if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_LOC_CB_INFO_BIT) {
            myProxy->getLocationReportEvent().unsubscribe(pvtSubscription);
        }
        if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_1HZ_MEAS_CB_INFO_BIT) {
            myProxy->getGnssMeasurementsEvent().unsubscribe(measSubscription);
        }
        if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_SV_CB_INFO_BIT) {
            myProxy->getGnssSvEvent().unsubscribe(svSubscription);
        }
        if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_NMEA_CB_INFO_BIT) {
            myProxy->getGnssNmeaEvent().unsubscribe(nmeaSubscription);
        }

        myProxy->stopPositionSession(callStatus, &info);
    }

    usleep(5000);

    if ((nullptr != gPTPReqIf) && (nullptr != gPTPReqIf->gptpDeinitIf)) {
       gPTPReqIf->gptpDeinitIf();
       gPTPReqIf = nullptr;
    }
    usleep(5000);
    if (nullptr != gPTPLibHandle)
        dlclose(gPTPLibHandle);
}

void signalHandler(int signal)
{
    cout << "signalHandler " <<endl;
    DeInitHandles();
    exit(0);
    return;
}

void parseCommandLine(int argc, char* argv[], int &delay)
{
    extern char *optarg;
    int opt;
    bool flag = false;

    /*
    REPORT_NHZ_PVT    0x01
    REPORT_SV         0x02
    REPORT_NMEA       0x04
    REPORT_GNSSDATA   0x08
    REPORT_1HZ_MEAS   0x10
    */
    /*PVT and SV enabled by default */
    mask = 0x03;

    /*60sec / 1 min */
    delay = 60;

    if (argc > 1) {
        while ((opt = getopt(argc, argv,
                  "m:d:h")) != -1) {
             switch (opt) {
                 case 'm':
                    mask = atoi(optarg);
                    flag = true;
                    break;
                 case 'd':
                    delay = atoi(optarg);
                    flag = true;
                    break;
                 case 'h':
                 default:
                     ToolUsage();
                     return;
             }
        }
        if (!flag) {
             ToolUsage();
             return;
        }
    }
}

void regSigHandler()
{
    struct sigaction mySigAction = {};

    mySigAction.sa_handler = signalHandler;
    sigemptyset(&mySigAction.sa_mask);
    sigaction(SIGHUP, &mySigAction, NULL);
    sigaction(SIGTERM, &mySigAction, NULL);
    sigaction(SIGINT, &mySigAction, NULL);
    sigaction(SIGPIPE, &mySigAction, NULL);
}

void subscribeGnssResports()
{
    // Subscribe for receiving values
    myProxy->getGnssCapabilitiesMaskAttribute().getChangedEvent().subscribe(
        [&](const uint32_t &val) {
                cout << "Received caps change event: " << val << endl;
            });

    if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_DATA_CB_INFO_BIT) {
        dataSubscription = myProxy->getGnssDataEvent().subscribe(
        [&](const LocIdlAPI::IDLGnssData& gnssData){
            printGnssData(gnssData);
        });
    }

    if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_LOC_CB_INFO_BIT) {
        pvtSubscription = myProxy->getLocationReportEvent().subscribe(
        [&](const LocIdlAPI::IDLLocationReport &_locationReport) {
            printPosResport(_locationReport);
        });
    }

    if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_1HZ_MEAS_CB_INFO_BIT) {
        measSubscription = myProxy->getGnssMeasurementsEvent().subscribe(
        [&](const LocIdlAPI::IDLGnssMeasurements& gnssMeasurements) {
            printMeasurement(gnssMeasurements);
        });
    }

    if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_SV_CB_INFO_BIT) {
        svSubscription = myProxy->getGnssSvEvent().subscribe(
        [&](const vector<LocIdlAPI::IDLGnssSv> &gnssSv) {
            printSVInfo(gnssSv);
        });
    }

    if (mask & LocIdlAPI::IDLGnssReportCbInfoMask::IDL_NMEA_CB_INFO_BIT) {
        nmeaSubscription = myProxy->getGnssNmeaEvent().subscribe(
        [&](const uint64_t timestamp, const string nmea){
            printNmea(timestamp, nmea);
        });
    }
}

void sessionStart()
{
    uint32_t _intervalInMs = 100;
    LocIdlAPI::IDLLocationResponse resp;
    CommonAPI::CallStatus callStatus;
    info.sender_ = 1234;

    sleep(1);
    myProxy->startPositionSession(_intervalInMs, mask, callStatus, resp, &info);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        cout << "startPositionSession() Remote call failed! callStatus "
        "" << (int)callStatus << endl;
        sessionStarted = false;
    } else {
        sessionStarted = true;
    }
}

int main(int argc, char* argv[])
{
    int delay;

    /* Command Line parsing*/
    parseCommandLine(argc, argv, delay);

    /* signal Handler */
    regSigHandler();

    /* GPTP */
    loadGptpLibFile();
    if ((nullptr != gPTPReqIf) && (nullptr != gPTPReqIf->gptpInitIf)) {
       gPTPReqIf->gptpInitIf();
    }

    CommonAPI::Runtime::setProperty("LogContext", "E01C");
    CommonAPI::Runtime::setProperty("LogApplication", "E01C");
    CommonAPI::Runtime::setProperty("LibraryBase", "LocIdlAPI");

    shared_ptr < CommonAPI::Runtime > runtime = CommonAPI::Runtime::get();

    string domain = "local";
    string instance = "com.qualcomm.qti.location.LocIdlAPI";
    string connection = "client-sample";

    myProxy = runtime->buildProxy<LocIdlAPIProxy>(domain,
            instance, connection);

    cout << "Checking availability!" << endl;
    while (!myProxy->isAvailable())
        usleep(10);
    cout << "Available..." << endl;

    subscribeGnssResports();
    sessionStart();
    if (sessionStarted)
        sleep(delay);
    DeInitHandles();
    return 0;
}
