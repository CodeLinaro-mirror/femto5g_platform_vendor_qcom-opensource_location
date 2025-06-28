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
#include <iomanip>
#include <sstream>
#include <string>
#include <queue>
#include <thread>
#include <chrono>
#include <fstream>
#include <CommonAPI/CommonAPI.hpp>
#include <v1/com/qualcomm/qti/location/LocationProxy.hpp>
#include <time.h>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <gptp_helper.h>

using namespace v1::com::qualcomm::qti::location;
using namespace std;
#define NSEC_IN_ONE_SEC       (1000000000ULL)   /* nanosec in a sec */
#define GPTP_IF_LIB_NAME      "libgptp.so"
#define IDLTESTCLIENT_NAME    "LocIdlAPIClient"

void gptpUpdateNotification(struct gptp_update update);
const char * libName = GPTP_IF_LIB_NAME;
void *gPTPLibHandle = nullptr;
std::shared_ptr<LocationProxy<>> myProxy;
bool verbose = false;
bool fileReadDone = false;

static const char MMF_TRUTH_FILE[] = "/data/vendor/location/";
#define MAX_LINE_TO_READ 100
#define NUM_SEC_IN_WEEK       604800

uint32_t readThreadSleep = 25;

CommonAPI::CallInfo info(1000);
bool     sessionStarted = false;
uint32_t mask;
uint32_t pvtSubscription;
uint32_t svSubscription;
uint32_t nmeaSubscription;
uint32_t measSubscription;
uint32_t nHzmeasSubscription;
uint32_t dataSubscription;
uint32_t capsSubscription;

enum BdsFields {
    BDS_INSTEST_GPS_WEEK = 1,
    BDS_INSTEST_GPS_TOW_MS = 2,
    BDS_INSTEST_LAT = 4,
    BDS_INSTEST_LONG = 5,
    BDS_INSTEST_ALTITUDE = 6,
    BDS_INSTEST_NORTH_ACC = 7,
    BDS_INSTEST_EAST_ACC = 8,
    BDS_INSTEST_ALTITUDE_ACC = 9,
    BDS_INSTEST_BEARING = 21,
    BDS_INSTEST_BEARING_ACC = 24
};

enum MmfPosInfoValidityMAsk {
    DATA_INVALID            = 0x00000000,
    DATA_VALID_UTC_TIME     = 0x00000001,
    DATA_VALID_GPS_WEEK     = 0x00000002,
    DATA_VALID_GPS_MSEC     = 0x00000004,
    DATA_VALID_LAT          = 0x00000008,
    DATA_VALID_LONG         = 0x00000010,
    DATA_VALID_TUNNEL       = 0x00000020,
    DATA_VALID_BEARING      = 0x00000040,
    DATA_VALID_ALTITUDE     = 0x00000080,
    DATA_VALID_HOR_ACC      = 0x00000100,
    DATA_VALID_ALT_ACC      = 0x00000200,
    DATA_VALID_BEARING_ACC  = 0x00000400
};

typedef struct {
    uint64_t validityMask;
    uint64_t utcTimestampMs;
    uint32_t gpsWeek;
    uint32_t gpsWeekMsec;
    uint64_t gpsTimeMsec;
    double lat;
    double lon;
    double alt;
    float horizontalAccuracy;
    float altitudeAccuracy;
    float bearing;
    float bearingAccuracy;
    bool isTunnel;
}MMF_POSITION_INFO;


std::queue<MMF_POSITION_INFO> posInfoQueue;
std::queue<std::vector<MMF_POSITION_INFO>> truthInfoQueue;

MMF_POSITION_INFO liveTruthInfo;
bool liveSignal = false;

string truthFile;
string serverIp;
uint32_t serverPort;

std::condition_variable cv_posCb;
std::mutex cv_m_posCb;
unsigned int gotPosCb = 0;

std::condition_variable cv_mmfTerminate;
std::mutex cv_m_mmfTerminate;
unsigned int mmfTerminate = 0;

std::thread t[2];

bool mmfON = false;

void mmfDataInjection(LocationTypes::MapMatchingFeedbackDataT  &mapData);

const static gPTPLibInterfaceEvent gPTPEvent = {
   gptpUpdateNotification,
};
const static gPTPLibInterfaceReq  *gPTPReqIf = nullptr;

void gptpUpdateNotification(struct gptp_update update)
{
    cout << "GPTP Update Notification" << endl;
}

uint64_t getSystemTimeInmSecFromBoot(void)
{
   struct timespec curt_ts;
   uint64_t curt_time_ms = 0;

   memset(&curt_ts, 0x00, sizeof(struct timespec));
   clock_gettime(CLOCK_BOOTTIME, &curt_ts);
   curt_time_ms = (curt_ts.tv_sec * 1000LL);
   curt_time_ms += (curt_ts.tv_nsec / 1000000LL);
   return curt_time_ms;
}

bool isIdlTestClientRunning(void)
{
   bool isIdlTestPrst = false;
   FILE *fptr = NULL;
   char processName[1024];
   char *ptrPresent = NULL;
   char *nxtPresent = NULL;
   unsigned int count = 0;

   fptr = popen("ps -A |grep LocIdlAPIClient", "r");
   if (NULL == fptr) {
       printf("pgrep is NULL");
       return isIdlTestPrst;
   }

   memset(&processName, 0x00, (1024 * sizeof(char)));
   while (fgets(processName, (1024 * sizeof(char)), fptr) != NULL) {
       std::string buff(processName);
       std::string subs(IDLTESTCLIENT_NAME);
       cout << "Output:" << buff << endl;
       auto pos = buff.find(subs);
       if (pos != std::string::npos) {
           if (0 == count) {
               count++;
           } else {
               isIdlTestPrst = true;
           }
       }
   }

   return isIdlTestPrst;
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
                              "-f <mmf options> <test duration in seconds> -v" << endl;
    cout << " -v represents verbose output " << endl;
    cout << " -f represents Map Matching feedback " << endl;
    cout << " mmf options:" << endl;
    cout << " -f 1,lat,long,alt (For live location)" << endl;
    cout << " -f 2,Truth_file_name (file should be present in /data/vendor/location/)" << endl;
    cout << " ===========================================" << endl<<endl;
    cout << " Example 1: No argument - deafult values will be used(-m 1 -d 60)" << endl;
    cout << " LocIdlAPIClient" <<endl;
    cout << " ===========================================" << endl<<endl;
    cout << " Example 2: For all the reports and test duration 300sec with verbose output" << endl;
    cout << " LocIdlAPIClient -m 31 -d 300 -v" <<endl<<endl;
    cout << " Example 3: For Position, SV reports and test duration 300sec " << endl;
    cout << " LocIdlAPIClient -m 3 -d 300" <<endl;
    cout << " Bit mask definition" << endl ;
    cout << " REPORT_NHZ_PVT    0x01   1" <<endl;
    cout << " REPORT_SV         0x02   2" <<endl;
    cout << " REPORT_NMEA       0x04   4" <<endl;
    cout << " REPORT_GNSSDATA   0x08   8" <<endl;
    cout << " REPORT_1HZ_MEAS   0x10   16" <<endl;
    cout << " ===========================================" << endl<<endl;
    cout << " Example 4: Map Matching feedback with live location" << endl;
    cout << " LocIdlAPIClient -f 1,12.98226941,77.6985102,866.776" <<endl;
    cout << " ===========================================" << endl<<endl;
    cout << " Example 5: Map Matching feedback based on truth file" << endl;
    cout << " LocIdlAPIClient -f 2,Truth_10Hz.txt" <<endl;
    cout << " ===========================================" << endl<<endl;
    return;
}

void printMeasurement(const LocationTypes::GnssMeasurementsT& gnssMeasurements)
{
    static unsigned int measCount;
    uint64_t gptp_time_ns = 0;
    const LocationTypes::GnssMeasurementsClockT &clk = gnssMeasurements.getClock();
    static bool printMeasHeader = true;

    if (printMeasHeader) {
        cout << "Type, LeapSecond, TimeNs, TimeUncNs, No.Of SV" << endl;
        printMeasHeader = false;
    }

    measCount += 1;
    //if ((nullptr != gPTPReqIf) && (nullptr != gPTPReqIf->gptpGetCurPtpTimeIf))
    //    gPTPReqIf->gptpGetCurPtpTimeIf(&gptp_time_ns);

    cout << "MEAS, " << clk.getLeapSecond()<<", "<< clk.getTimeNs()<< ""
        ", "<< clk.getTimeUncertaintyNs()<<", "
        "" << (gnssMeasurements.getMeasurements()).size()<< endl;

    if (verbose) {
        cout << "-------" << endl;
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

        const vector<LocationTypes::GnssMeasurementsDataT > &measData =
                                        gnssMeasurements.getMeasurements();
        for (uint16_t idx = 0; idx < measData.size(); idx++) {
            cout <<"Idx  "<< idx << endl;

            cout <<"MeasFlags "<< measData[idx].getFlags() << endl;
            cout <<"svId "<< measData[idx].getSvId() << endl;

            cout <<"svType "<< static_cast<int>(measData[idx].getSvType()) << endl;
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
            cout <<"carrierPhase "<< measData[idx].getCarrierPhase() << endl;
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
        cout <<"ReportingLatency "<< gnssMeasurements.getReportingLatency() << endl;
        cout << "-------" << endl;
    }
}

void TerminateApp()
{
    std::unique_lock<std::mutex> lk(cv_m_mmfTerminate);
    mmfTerminate = 1;
    cv_mmfTerminate.notify_all();
    lk.unlock();
    cout<<"TerminateApp!"<<endl;
}

void readThread()
{
    if (!liveSignal) {
        std::string filepath(MMF_TRUTH_FILE);
        std::ifstream file((filepath+truthFile));
        uint32_t nLines = 0;
        uint32_t nBlob = 0;
        if (!file.is_open()) {
            std::cout << "Could not open file" << std::endl;
            TerminateApp();
            fileReadDone = true;
            return;
        }

        std::string line;
        vector<MMF_POSITION_INFO> truthInfoVec;
        while (std::getline(file, line) && mmfON) {
            MMF_POSITION_INFO truthInfo;
            std::istringstream lineStream(line);
            std::string token;

            // Extract the first token delimited by a comma
            if (std::getline(lineStream, token, ',')) {
                if (token == string("#INSTEST")) {
                    // Extract remaining tokens
                    std::vector<std::string> remainingTokens;
                    while (std::getline(lineStream, token, ',')) {
                            remainingTokens.push_back(token);
                    }
                    truthInfo.validityMask = 0;
                    truthInfo.gpsWeek =
                        static_cast<uint32_t>(std::stoul(remainingTokens[BDS_INSTEST_GPS_WEEK]));
                    truthInfo.validityMask |= DATA_VALID_GPS_WEEK;
                    truthInfo.gpsWeekMsec =
                    static_cast<uint32_t>((std::stod(remainingTokens[BDS_INSTEST_GPS_TOW_MS])) * 1000);
                    truthInfo.gpsTimeMsec =
                        (truthInfo.gpsWeek * NUM_SEC_IN_WEEK * 1000) + truthInfo.gpsWeekMsec;
                    truthInfo.validityMask |= DATA_VALID_GPS_MSEC;
                    truthInfo.lat = (std::stod(remainingTokens[BDS_INSTEST_LAT]));
                    truthInfo.validityMask |= DATA_VALID_LAT;
                    truthInfo.lon = (std::stod(remainingTokens[BDS_INSTEST_LONG]));
                    truthInfo.validityMask |= DATA_VALID_LONG;
                    truthInfo.alt = (std::stod(remainingTokens[BDS_INSTEST_ALTITUDE])); //H-Ell
                    truthInfo.validityMask |= DATA_VALID_ALTITUDE;
                    float northAcc = std::stof(remainingTokens[BDS_INSTEST_NORTH_ACC]); //SDNorth
                    float eastAcc = std::stof(remainingTokens[BDS_INSTEST_EAST_ACC]); //SDEast
                    truthInfo.horizontalAccuracy =
                        static_cast<float>( sqrt((northAcc * northAcc) + (eastAcc * eastAcc)) );
                    truthInfo.validityMask |= DATA_VALID_HOR_ACC;
                    truthInfo.altitudeAccuracy =
                        std::stof(remainingTokens[BDS_INSTEST_ALTITUDE_ACC]); //SDHeight
                    truthInfo.validityMask |= DATA_VALID_ALT_ACC;
                    truthInfo.bearing = std::stof(remainingTokens[BDS_INSTEST_BEARING]);
                    truthInfo.validityMask |= DATA_VALID_BEARING;
                    truthInfo.bearingAccuracy =
                        std::stof(remainingTokens[BDS_INSTEST_BEARING_ACC]);
                    truthInfo.validityMask |= DATA_VALID_BEARING_ACC;
                    truthInfoVec.push_back(truthInfo);
                    nLines++;
                } else {
                    continue;
                }
                /*when 100 records reached we will push the vector into queue */
                if (nLines == MAX_LINE_TO_READ) {
                    truthInfoQueue.push(truthInfoVec);
                    truthInfoVec.clear();
                    nLines = 0;
                    truthInfoVec.push_back(truthInfo);
                    nLines++;
                    nBlob++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(readThreadSleep));
                }
            }
        }
        if (file.eof() && nLines > 1) {
            nBlob++;
            truthInfoQueue.push(truthInfoVec);
            truthInfoVec.clear();
        }
        fileReadDone = true;
        file.close();
    }
    cout<<" readThread terminate " <<endl;
    return;
}

void sendMmfInfo(MMF_POSITION_INFO &truth, MMF_POSITION_INFO &pos)
{
    uint32_t mask = 0;
    LocationTypes::MapMatchingFeedbackDataT  mapData = {};

    mask |= LocationTypes::MapMatchingFeedbackDataValidityT::MMFDVT_UTC_TIME;
    mapData.setUtcTimestampMs(pos.utcTimestampMs);
    mask |= LocationTypes::MapMatchingFeedbackDataValidityT::MMFDVT_TUNNEL;
    mapData.setIsTunnel(pos.isTunnel);
    if (((truth.validityMask & DATA_VALID_LAT) == DATA_VALID_LAT)) {
        mask |= LocationTypes::MapMatchingFeedbackDataValidityT::MMFDVT_LAT_DIFF;
        mapData.setMapMatchedLatitudeDifference(truth.lat - pos.lat);
    }
    if (((truth.validityMask & DATA_VALID_LONG) == DATA_VALID_LONG)) {
        mask |= LocationTypes::MapMatchingFeedbackDataValidityT::MMFDVT_LONG_DIFF;
        mapData.setMapMatchedLongitudeDifference(truth.lon - pos.lon);
    }
    if (((truth.validityMask & DATA_VALID_ALTITUDE) == DATA_VALID_ALTITUDE)) {
        mask |= LocationTypes::MapMatchingFeedbackDataValidityT::MMFDVT_ALTITUDE;
        mapData.setAltitude(truth.alt);
    }
    if (((truth.validityMask & DATA_VALID_BEARING) == DATA_VALID_BEARING)) {
        mask |= LocationTypes::MapMatchingFeedbackDataValidityT::MMFDVT_BEARING;
        mapData.setBearing(truth.bearing);
    }
    if (((truth.validityMask & DATA_VALID_HOR_ACC) == DATA_VALID_HOR_ACC)) {
        mask |= LocationTypes::MapMatchingFeedbackDataValidityT::MMFDVT_HOR_ACC;
        mapData.setHorizontalAccuracy(truth.horizontalAccuracy);
    }
    if (((truth.validityMask & DATA_VALID_ALT_ACC) == DATA_VALID_ALT_ACC)) {
        mask |= LocationTypes::MapMatchingFeedbackDataValidityT::MMFDVT_ALT_ACC;
        mapData.setAltitudeAccuracy(truth.altitudeAccuracy);
    }
    if (((truth.validityMask & DATA_VALID_BEARING_ACC) == DATA_VALID_BEARING_ACC)) {
        mask |= LocationTypes::MapMatchingFeedbackDataValidityT::MMFDVT_BEARING_ACC;
        mapData.setBearingAccuracy(truth.bearingAccuracy);
    }
    mapData.setValidityMask(mask);
    mmfDataInjection(mapData);
}

void mmfComputation()
{
    std::string filepath(MMF_TRUTH_FILE);

    uint32_t truthPos = 0;
    MMF_POSITION_INFO prevTruth;
    bool runThread = true;
    while (runThread) {
        MMF_POSITION_INFO pos;
        MMF_POSITION_INFO matchingTruth;

        if (!posInfoQueue.size() && mmfON) {
            std::unique_lock<std::mutex> lock(cv_m_posCb);
            cv_posCb.wait(lock, [] { return gotPosCb; });
            gotPosCb = 0;
            lock.unlock();
        }
        if (!mmfON)
            break;
        /* To handle the case where more than one position report reached before finding the truth
        entry from truth file. This will happen only for first report. */
        if (posInfoQueue.size()) {
            pos = posInfoQueue.front();
            posInfoQueue.pop();
        } else {
            continue;
        }

        if (liveSignal) {
            matchingTruth = liveTruthInfo;
            sendMmfInfo(matchingTruth, pos);
        } else if (truthInfoQueue.size()){
            std::vector<MMF_POSITION_INFO> truth = truthInfoQueue.front();
            uint32_t tSize = truth.size();
            if ((truth[0].gpsWeek + 1) < pos.gpsWeek) {
                cout << " TRUTH FILE is TOO OLD!!!!"<<endl;
                runThread = false;
                break;
            }
            for (int i = truthPos; i < tSize; i++) {
                /*Find the right vector from the queue which has the matching timestamp*/
                if ((truth[tSize - 1].gpsTimeMsec <= pos.gpsTimeMsec && tSize == MAX_LINE_TO_READ)
                                                    || (pos.gpsTimeMsec < truth[0].gpsTimeMsec)) {
                    truthInfoQueue.pop();
                    while (!truthInfoQueue.size() && !fileReadDone && mmfON ) {
                        readThreadSleep = 5; /*aggressive read */
                        std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    }
                    if (truthInfoQueue.size()) {
                        truth = truthInfoQueue.front();
                        tSize = truth.size();
                        i = -1;
                        continue;
                    } else {
                        cout<< "truth[tSize - 1].gpsWeekMsec: "<< truth[tSize - 1].gpsWeekMsec <<""
                                  ""<< " pos.gpsWeekMsec: " << pos.gpsWeekMsec <<endl;
                        cout << " No More entry in Truth file!!!!"<<endl;
                        runThread = false;
                        break;
                    }
                }
                else if (tSize < MAX_LINE_TO_READ && truth[tSize - 1].gpsTimeMsec < pos.gpsTimeMsec) {
                    cout << " Last vector batch, No matching timestamp going to terminate!!!!"<<endl;
                    cout<< "truth[tSize - 1].gpsWeekMsec: "<< truth[tSize - 1].gpsWeekMsec <<""
                        ""<< " pos.gpsWeekMsec: " << pos.gpsWeekMsec <<endl;
                    truthInfoQueue.pop();
                    runThread = false;
                    break;
                }
                else if (truth[i].gpsTimeMsec == pos.gpsTimeMsec) {
                    matchingTruth = truth[i];
                    truthPos = i;
                    sendMmfInfo(matchingTruth, pos);
                    break;
                }
                else  if (i < (tSize -1) && truth[i+1].gpsTimeMsec == pos.gpsTimeMsec) {
                    matchingTruth = truth[i+1];
                    truthPos = i+1;
                    sendMmfInfo(matchingTruth, pos);
                    break;
                }
                else if (i < (tSize -1) && truth[i].gpsTimeMsec < pos.gpsTimeMsec &&
                                            truth[i+1].gpsTimeMsec > pos.gpsTimeMsec){
                    MMF_POSITION_INFO intPolTruth;
                    intPolTruth.validityMask = truth[i].validityMask;
                    intPolTruth.gpsWeek = pos.gpsWeek;
                    intPolTruth.gpsWeekMsec = pos.gpsWeekMsec;
                    intPolTruth.gpsTimeMsec = pos.gpsTimeMsec;
                    intPolTruth.utcTimestampMs = pos.utcTimestampMs;
                    float factor = static_cast<float>((float)(pos.gpsTimeMsec -
                        truth[i].gpsTimeMsec) / (float)(truth[i+1].gpsTimeMsec -
                                                        truth[i].gpsTimeMsec));
                    intPolTruth.lat = truth[i].lat + ( factor * (truth[i+1].lat - truth[i].lat));
                    intPolTruth.lon = truth[i].lon + ( factor * (truth[i+1].lon - truth[i].lon));
                    intPolTruth.alt = truth[i].alt + ( factor * (truth[i+1].alt - truth[i].alt));
                    intPolTruth.horizontalAccuracy = truth[i].horizontalAccuracy +
                      (factor * (truth[i + 1].horizontalAccuracy - truth[i].horizontalAccuracy));
                    intPolTruth.altitudeAccuracy = truth[i].altitudeAccuracy +
                          (factor * (truth[i + 1].altitudeAccuracy - truth[i].altitudeAccuracy));
                    {
                        double delta2Use;
                        double adj1 = (truth[i+1].bearing > truth[i].bearing) ? -360.0 : 0;
                        double adj2 = (truth[i+1].bearing > truth[i].bearing) ? 0 : -360.0;

                        // the straight forward slice: Sample N+1 - Sample N
                        double deltaA = truth[i+1].bearing - truth[i].bearing;

                        // the other slice, where it be within 360 degrees, thus the adjustment factors
                        double deltaB = (truth[i+1].bearing + adj1) - (truth[i].bearing + adj2);

                        // use the small slice as delta for interpolation
                        delta2Use = ( std::abs(deltaA) < std::abs(deltaB) ) ? deltaA : deltaB;
                        double interpbearing = truth[i].bearing + ( factor * delta2Use );

                        if (interpbearing < 0)
                           interpbearing += 360;
                        if (interpbearing >= 360)
                           interpbearing -= 360;
                        intPolTruth.bearing = static_cast<float>( interpbearing );
                    }
                    intPolTruth.bearingAccuracy = truth[i].bearingAccuracy +
                        (factor * (truth[i + 1].bearingAccuracy - truth[i].bearingAccuracy));
                    matchingTruth = intPolTruth;
                    truthPos = i;
                    sendMmfInfo(matchingTruth, pos);
                    break;
                } else if (i == (tSize -1)) {
                    cout<< "Invalid TS in PVT, pos.gpsWeek:: " << pos.gpsWeek <<""
                        ""<< " pos.gpsWeekMsec: " << pos.gpsWeekMsec<<endl;
                    cout<< "truthInfoQueue.size(): " << truthInfoQueue.size()<<endl;
                    truthPos = i;
                }else {
                    ;
                }
                if (truthInfoQueue.size() > 5)
                    readThreadSleep = 1000; /*Reduce read frequency*/
                else
                    readThreadSleep = 500;
            }
            if (truthPos == (tSize - 1)) {
                truthPos = 0;
                truthInfoQueue.pop();
            }
        } else {
            cout<<" TRUTH QUEUE EMPTY: fileReadDone status: " << fileReadDone<<endl;
            while (!truthInfoQueue.size() && !fileReadDone && mmfON) {
                readThreadSleep = 5; /*aggressive read */
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
            if (!truthInfoQueue.size() && fileReadDone) {
                cout << " No More entry in Truth file!!!!"<<endl;
                runThread = false;
            }
        }
    }
    cout<<" mmfComputation terminate " <<endl;
    TerminateApp();
    return;
}

string navTechMaskToString(unsigned int techmask)
{
    string s1("");

    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_GNSS_BIT) {
        s1.append("SATELLITE");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_CELL_BIT) {
        s1.append("|CELLID");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_WIFI_BIT) {
        s1.append("|WIFI");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_SENSORS_BIT) {
        s1.append("|SENSORS");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_REF_LOC_BIT) {
        s1.append("|REFERENCE_LOCATION");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_INJECTED_COARSE_POS_BIT) {
        s1.append("|INJECTED_COARSE_POSITION");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_AFLT_BIT) {
        s1.append("|AFLT");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_HYBRID_BIT) {
        s1.append("|HYBRID");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_PPE_BIT) {
        s1.append("|PPE");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_VEH_BIT) {
        s1.append("|VEH");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_VIS_BIT) {
        s1.append("|VIS");
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_PROPAGATED_BIT) {
        s1.append("|PROPAGATED");
    }

    return s1;
}

void printPosResport(const LocationTypes::LocationReportT &_locationReport)
{
    const LocationTypes::LocationT &location = _locationReport.getLocInfo();

    static unsigned int posCount;
    uint64_t gptp_time_ns = 0;
    bool retSyncStatus = false;
    bool retPtp = false;
    static bool printPvtHeader = true;

    if (printPvtHeader) {
        cout << "Type, UTCTimestamp(ms), Latitude, Longitude, RxTimeStampPTP(ns), "
                "TxTimestampPTP(ns), Latency(ms), Latency@Source(ms), LocationTechMask, "
                "NavSolutionMask, gPTPSyncStatus, Boot Time(ms) " << endl;
        printPvtHeader = false;
    }

    posCount += 1;

    if (mmfON)
    {
        MMF_POSITION_INFO posInfo;
        static uint32_t tunnelTracker;
        uint32_t posMask =  _locationReport.getPosTechMask();
        if (posMask & LocationTypes::LocationTechnologyMaskT::LTMT_GNSS_BIT) {
            tunnelTracker = 0;
        } else if ((posMask & LocationTypes::LocationTechnologyMaskT::LTMT_SENSORS_BIT) &&
            !(posMask & LocationTypes::LocationTechnologyMaskT::LTMT_GNSS_BIT)) {
            tunnelTracker++;
        }
        posInfo.utcTimestampMs = location.getTimestamp();
        posInfo.lat = location.getLatitude();
        posInfo.lon = location.getLongitude();
        posInfo.alt = location.getAltitude();
        if (tunnelTracker > 30) { /*3s = 30 reports in 10Hz case */
            posInfo.isTunnel = true;
        } else {
            posInfo.isTunnel = false;
        }
        const LocationTypes::GnssSystemTimeT &gnssTime = _locationReport.getGnssSystemTime();
        const LocationTypes::SystemTimeStructT &time = gnssTime.getSystemTime();

         if (gnssTime.getGnssSystemTimeSrc() != LocationTypes::GnssLocSvSystemTypeT::GLSSTT_GLO) {
            const LocationTypes::GnssSystemTimeStructTypeT &systemTime =
                                time.getGnssSystemTime();
            posInfo.gpsWeek = systemTime.getSystemWeek();
            posInfo.gpsWeekMsec = systemTime.getSystemMsec();
            posInfo.gpsTimeMsec = (posInfo.gpsWeek * NUM_SEC_IN_WEEK * 1000) + posInfo.gpsWeekMsec;
        }
        if (posInfo.gpsWeek > 0) {
            std::unique_lock<std::mutex> lock(cv_m_posCb);
            posInfoQueue.push(posInfo);
            gotPosCb = 1;
            cv_posCb.notify_all();
            lock.unlock();
        }
    }

    uint64_t lFlags = _locationReport.getLocationInfoFlags();
    if (lFlags &  LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_GPTP_TIME_BIT){
        if ((nullptr != gPTPReqIf) && (nullptr != gPTPReqIf->gptpGetCurPtpTimeIf)) {
            retPtp = gPTPReqIf->gptpGetCurPtpTimeIf(&gptp_time_ns);
            if (nullptr != gPTPReqIf->gptpGetSyncStatusIf) {
               retSyncStatus = gPTPReqIf->gptpGetSyncStatusIf();
            }
        }
        if     (retPtp) {
            cout <<"PVT, "
                "" << location.getTimestamp()<< ", "
                "" <<location.getLatitude() << ", " << location.getLongitude() << ", "
                "" << gptp_time_ns << ", "
                "" << _locationReport.getElapsedgPtpTime()<<", "
                "" << fixed << setprecision(3) << ""
                "" <<(float)(gptp_time_ns -
                            _locationReport.getElapsedgPtpTime()) / (float)1000000 <<", "
                "" << _locationReport.getReportingLatency()<<","
                "" << navTechMaskToString(location.getTechMask())  <<","
                "" << (_locationReport.getNavSolutionMask()) <<","
                "" << retSyncStatus <<","
                "" << getSystemTimeInmSecFromBoot() << ","
                ""  << endl;
        } else {
            cout <<"PVT, "
                "" << location.getTimestamp()<< ", "
                "" <<location.getLatitude() << ", " << location.getLongitude() << ", "
                "" << "NA" << ", "
                "" << _locationReport.getElapsedgPtpTime() <<", "
                "" << fixed << setprecision(3) << ""
                "" <<"NA" <<", "
                "" << _locationReport.getReportingLatency()<<","
                "" << navTechMaskToString(location.getTechMask())  <<","
                "" << (_locationReport.getNavSolutionMask())<<","
                "" << retSyncStatus <<","
                "" << getSystemTimeInmSecFromBoot() << ","
                ""  << endl;
        }
    } else {
            cout <<"PVT, "
                "" << location.getTimestamp()<< ", "
                "" <<location.getLatitude() << ", " << location.getLongitude() << ", "
                "" << "NA" << ", "
                "" << "NA" <<", "
                "" << fixed << setprecision(3) << ""
                "" <<"NA" <<","
                "" <<"NA" <<","
                "" << navTechMaskToString(location.getTechMask())  <<","
                "" << (_locationReport.getNavSolutionMask()) <<","
                "" << retSyncStatus <<","
                "" << getSystemTimeInmSecFromBoot() << ","
                ""  << endl;
    }

    if (verbose) {
        cout << "-------" << fixed << setprecision(8) << endl;
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

        cout << "NorthVelocityStdDeviation"
            "" << _locationReport.getNorthVelocityStdDeviation() << endl;
        cout << "EastVelocityStdDeviation "
            "" << _locationReport.getEastVelocityStdDeviation() << endl;
        cout << "UpVelocityStdDeviatio    "<< _locationReport.getUpVelocityStdDeviation() << endl;
        cout << "NumSvUsedInPosition      " << _locationReport.getNumSvUsedInPosition() << endl;

        const LocationTypes::LocationReportSvUsedInPositionT &svUsed =
                                _locationReport.getSvUsedInPosition();
        cout << "GpsSvUsedIdsMask     "<< svUsed.getGpsSvUsedIdsMask() << endl;
        cout << "GalSvUsedIdsMask     "<< svUsed.getGalSvUsedIdsMask() << endl;
        cout << "BdsSvUsedIdsMask     "<< svUsed.getBdsSvUsedIdsMask() << endl;
        cout << "QzssSvUsedIdsMask    "<< svUsed.getQzssSvUsedIdsMask() << endl;
        cout << "NavicSvUsedIdsMask   "<< svUsed.getNavicSvUsedIdsMask() << endl;
        cout << "GloSvUsedIdsMask     "<< svUsed.getGloSvUsedIdsMask() << endl;

        cout << "NavSolutionMask      "<< _locationReport.getNavSolutionMask() << endl;
        cout << "PosTechMask          "<< _locationReport.getPosTechMask() << endl;

        const LocationTypes::LocationReportPositionDynamicsT &posDynamics =
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

        const LocationTypes::GnssSystemTimeT &gnssTime = _locationReport.getGnssSystemTime();
        cout << "getLocationCbEvent GnssSystemTimeSrc " << static_cast<int>(gnssTime.getGnssSystemTimeSrc()) << endl;
        const LocationTypes::SystemTimeStructT &time = gnssTime.getSystemTime();
        if (gnssTime.getGnssSystemTimeSrc() == LocationTypes::GnssLocSvSystemTypeT::GLSSTT_GLO) {
              const LocationTypes::GnssGloTimeStructTypeT &systemTime =
                                time.getGloSystemTime();
              cout <<"ValidityMask    " <<systemTime.getValidityMask() <<endl ;
              cout <<"GloDays         " <<systemTime.getGloDays() <<endl ;
              cout <<"GloFourYear     " <<systemTime.getGloFourYear() <<endl ;
              cout <<"ClkTimeBias     " <<systemTime.getGloClkTimeBias() <<endl ;
              cout <<"ClkTimeUncMs    " <<systemTime.getGloClkTimeUncMs() <<endl ;
              cout <<"GloMsec         " <<systemTime.getGloMsec() <<endl ;
              cout <<"NumClockResets  " <<systemTime.getNumClockResets() <<endl ;
              cout <<"RefCount        " <<systemTime.getRefFCount() <<endl ;
        } else {
              const LocationTypes::GnssSystemTimeStructTypeT &systemTime =
                                time.getGnssSystemTime();
              cout <<"ValidityMask    " <<systemTime.getValidityMask() <<endl ;
              cout <<"SystemWeek      " <<systemTime.getSystemWeek() <<endl ;
              cout <<"SystemMsec      " <<systemTime.getSystemMsec() <<endl ;
              cout <<"ClkTimeBias     " <<systemTime.getSystemClkTimeBias() <<endl ;
              cout <<"ClkTimeUncMs    " <<systemTime.getSystemClkTimeUncMs() <<endl ;
              cout <<"RefFcount       " <<systemTime.getRefFcount() <<endl ;
              cout <<"NumClockResets  " <<systemTime.getNumClockResets() <<endl ;
        }

        const vector<LocationTypes::GnssMeasUsageInfoT> &meas = _locationReport.getMeasUsageInfo();
        for (uint8_t idx = 0; idx < meas.size() && idx < 176; idx++) {
            cout << "GnssConstellation    "<< static_cast<int>(meas[idx].getGnssConstellation())<<endl ;
            cout << "GnssSignalType       "<< meas[idx].getGnssSignalType()<<endl ;
            cout << "GnssSvId             "<< meas[idx].getGnssSvId()<<endl ;
        }

        cout << "LeapSeconds      " << static_cast<int>(_locationReport.getLeapSeconds()) << endl;
        cout << "CalibrationConfidence "
            "" << static_cast<int>(_locationReport.getCalibrationConfidencePercent()) << endl;
        cout << "CalibrationStatus    " << _locationReport.getCalibrationStatus() << endl;
        cout << "LocOutputEngType     " << static_cast<int>(_locationReport.getLocOutputEngType()) << endl;
        cout << "LocOutputEngMask     " << _locationReport.getLocOutputEngMask() << endl;
        cout << "ConformityIndex      " << _locationReport.getConformityIndex() << endl;

        const LocationTypes::LlaInfoT &lla = _locationReport.getLlaVrpBased();
        cout << "Latitude             " << lla.getLatitude() << endl;
        cout << "Longitude            " << lla.getLongitude() << endl;
        cout << "Altitude             " << lla.getAltitude() << endl;

        const vector<float> &emu = _locationReport.getEnuVelocityVrpBased();
        for (int k = 0; k < emu.size(); k++) {
            cout << "Emu                  " << emu[k] << endl;
        }

        cout << "DrSolutionStatusMask " << _locationReport.getDrSolutionStatusMask() << endl;
        cout << "AltitudeAssumed      " << _locationReport.getAltitudeAssumed() << endl;
        cout << "SessionStatus        " << static_cast<int>(_locationReport.getSessionStatus()) << endl;
        cout << "IntegrityRiskUsed    " << _locationReport.getIntegrityRiskUsed() << endl;
        cout << "ProtectAlongTrack    " << _locationReport.getProtectAlongTrack() << endl;
        cout << "ProtectCrossTrack    " << _locationReport.getProtectCrossTrack() << endl;
        cout << "ProtectVertical      " << _locationReport.getProtectVertical() << endl;
        cout << "LocationInfoFlags    " << _locationReport.getLocationInfoFlags() << endl;

        const vector<uint32_t> &dgnss = _locationReport.getDgnssStationId();
        for (int idx = 0; idx < dgnss.size(); idx++) {
               cout << "DgnssStationId        " << dgnss[idx] << endl;
        }
        cout << "ElapsedPTPTimeNs  " << _locationReport.getElapsedgPtpTime() << endl;
        cout << "ReportingLatency  " << _locationReport.getReportingLatency() << endl;
        cout << "LeapSecondsUnc    " << _locationReport.getLeapSecondsUnc() << endl;
        cout << "BaseLineLength    " << _locationReport.getBaseLineLength() << endl;
        cout << "AgeMsecOfCorrections " << _locationReport.getAgeMsecOfCorrections() << endl;
        cout << "CurrReportingRate " << _locationReport.getPosReportingInterval() << endl;
        cout << "-------" << endl;
    }
}

void printGnssData(const LocationTypes::GnssDataT& gnssData)
{
   vector<uint32_t> dataMask = gnssData.getGnssDataMask();
   vector<double> jammerInd = gnssData.getJammerInd();
   vector<double> agc = gnssData.getAgc();

   if (verbose) {
       cout << "Type, SignalType, Mask, JammeInd, Agc" << endl;
       cout << "-------" << endl;
       for  (int i = 0; i < (dataMask.size() - 1); i++) {
           cout << "GNSSDATA, " << i << " , "<< dataMask[i] << " , "
                "" << jammerInd[i] << " , " << agc[i] << endl;
       }
       cout << "-------" << endl;
   }
}

void printSVInfo(const vector<LocationTypes::GnssSvDataT > &gnssSv)
{
    static unsigned int svCount;
    static bool printSVHeader = true;

    if (printSVHeader) {
        cout << "Type, No.of SV" << endl;
        printSVHeader = false;
    }
    svCount += 1;
    cout << "SV, " << gnssSv.size() << endl;

    if (verbose) {
        cout << "-------" << endl;
        for (uint16_t idx = 0; idx < gnssSv.size(); idx++) {
            cout << "svId  " << gnssSv[idx].getSvId() << endl;
            cout << "Type    " << static_cast<int>(gnssSv[idx].getType()) << endl;
            cout << "CN0Dbhz  " << gnssSv[idx].getCN0DbHz() << endl;
            cout << "setElevation    " << gnssSv[idx].getElevation() << endl;
            cout << "Azimuth  " << gnssSv[idx].getAzimuth() << endl;
            cout << "CarrierFrequencyHz    " << gnssSv[idx].getCarrierFrequencyHz() << endl;
            cout << "GnssSignalTypeMask" << gnssSv[idx].getGnssSignalType() << endl;
            cout << "BasebandCarrierToNoiseDbHz    "
                    "" << gnssSv[idx].getBasebandCarrierToNoiseDbHz() << endl;
            cout << "GloFrequency" << gnssSv[idx].getGloFrequency() << endl;
        }
        cout << "-------" << endl;
    }
}

void printNmea(const uint64_t timestamp, const string &nmea)
{
    static bool printNmeaHeader = true;
    string segment;
    stringstream ss(nmea);
    string nmeaStr;

    while (std::getline(ss, nmeaStr, '\n')) {
        nmeaStr += '\n';

        if (verbose) {
            cout << "NMEA, " << timestamp <<", "<< nmeaStr<< endl;
        } else {
            if (printNmeaHeader) {
                cout << "Type, Timestamp, ID" << endl;
                printNmeaHeader = false;
            }
            stringstream stnmea(nmeaStr);
            getline(stnmea, segment, ',');
            cout << "NMEA, "<< timestamp <<", "<< segment<< endl;
        }
    }
}

void DeInitHandles()
{
    CommonAPI::CallStatus callStatus;
    LocationTypes::LocationStatusT sessStatus = LocationTypes::LocationStatusT::LOCATION_STATUS_T_SUCCESS;
    if (sessionStarted && myProxy) {
        if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_DATA_CB_INFO_BIT) {
            myProxy->getGnssDataReportEvent().unsubscribe(dataSubscription);
        }
        if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_LOCATION_CB_INFO_BIT) {
            myProxy->getGnssLocationReportEvent().unsubscribe(pvtSubscription);
        }
        if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_MEAS_CB_INFO_BIT) {
            myProxy->getGnssMeasurementReportEvent().unsubscribe(measSubscription);
        }
        if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_SV_CB_INFO_BIT) {
            myProxy->getGnssSvReportEvent().unsubscribe(svSubscription);
        }
        if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_NMEA_CB_INFO_BIT) {
            myProxy->getGnssNmeaEvent().unsubscribe(nmeaSubscription);
        }
        myProxy->getGnssCapabilitiesEvent().unsubscribe(capsSubscription);
        myProxy->StopPositionSession(callStatus, sessStatus, &info);
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

void terminateThreads()
{
    mmfON = false;
    std::unique_lock<std::mutex> lock(cv_m_posCb);
    gotPosCb = 1;
    cv_posCb.notify_all();
    lock.unlock();

    t[0].join();
    t[1].join();
    cout << "terminateThreads" <<endl;

}

void signalHandler(int signal)
{
    cout << "signalHandler " <<endl;
    if (mmfON) {
        terminateThreads();
        return;
    }
    DeInitHandles();
    if (myProxy) {
        myProxy.reset();
    }
    exit(0);
    return;
}

void getLocationCapabilities() {
    if (myProxy) {
        CommonAPI::CallStatus callStatus;
        uint32_t capsMask = 0;
        myProxy->GetLocationCapabilities(callStatus, capsMask, &info);
        cout << "caps received " << capsMask << endl;
    }

}

bool parseCommandLine(int argc, char* argv[], int &delay)
{
    extern char *optarg;
    int opt;
    bool flag = false;

    /*
    Valid mask values:
    REPORT_NHZ_PVT    0x01
    REPORT_SV         0x02
    REPORT_NMEA       0x04
    REPORT_GNSSDATA   0x08
    REPORT_1HZ_MEAS   0x10
    */
    /*PVT enabled by default */
    mask = 0x01;

    /*60sec / 1 min */
    delay = 60;

    if (argc > 1) {
        while ((opt = getopt(argc, argv,
                  "m:d:f:hv")) != -1) {
             switch (opt) {
                 case 'm':
                    mask = atoi(optarg);
                    flag = true;
                    break;
                 case 'd':
                    delay = atoi(optarg);
                    flag = true;
                    break;
                 case 'v':
                    verbose = true;
                    break;
                 case 'f':
                     {
                        string inp(optarg);
                        stringstream stream(inp);
                        string token;
                        getline(stream, token, ',');

                        if (token == string("1")) {
                            liveSignal = true;
                            liveTruthInfo.validityMask = 0;
                            if (getline(stream, token, ',')) {
                                liveTruthInfo.validityMask |= DATA_VALID_LAT;
                                liveTruthInfo.lat = (stod(token));
                            } else {
                                cout << "Truth LATITUDE MISSING!!"<<endl;
                                ToolUsage();
                                return false;
                            }
                            if (getline(stream, token, ',')) {
                                liveTruthInfo.validityMask |= DATA_VALID_LONG;
                                liveTruthInfo.lon = (stod(token));
                            } else {
                                cout << "Truth LONGTITUDE MISSING!!"<<endl;
                                ToolUsage();
                                return false;
                            }
                            if (getline(stream, token, ',')) {
                                liveTruthInfo.validityMask |= DATA_VALID_ALTITUDE;
                                liveTruthInfo.alt = (stod(token));
                            } else {
                                cout << "Truth ALTITUDE MISSING!!"<<endl;
                                ToolUsage();
                                return false;
                            }
                            cout << "LAT: " << liveTruthInfo.lat << " LON:" << liveTruthInfo.lon <<" ALT:" << liveTruthInfo.alt<<endl;

                        } else if (token == string("2")) {
                            if (getline(stream, token, ',')) {
                                truthFile = token;
                                cout << "Truth File: " << truthFile << endl;
                            } else {
                                cout << "Truth File MISSING!!"<<endl;
                                ToolUsage();
                                return false;
                            }
                        } else {
                                cout << "Invalid Argument!!"<<endl;
                                ToolUsage();
                                return false;
                            /*getline(stream, token, ',');
                            serverIp = token;
                            getline(stream, token, ',');
                            serverPort = static_cast<uint32_t>(std::stoul(token));*/
                        }
                        mmfON = true;
                        flag = true;
                     }
                    break;
                 case 'h':
                 default:
                     ToolUsage();
                     return false;
             }
        }
        if (!flag) {
             ToolUsage();
             return false;
        }
    }
    return true;
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
    if (!myProxy) {
      cout << " mProxy is NULL !! "<< endl;
      return;
    }
       myProxy->getProxyStatusEvent().subscribe([&] (const CommonAPI::AvailabilityStatus status) {
           switch (status) {
               case CommonAPI::AvailabilityStatus::UNKNOWN:
                   std::cout << "Unkown" << endl;
               break;
               case CommonAPI::AvailabilityStatus::NOT_AVAILABLE:
                   std::cout << "NOT_AVAILABLE" << endl;
               break;
               case CommonAPI::AvailabilityStatus::AVAILABLE:
                   std::cout << "AVAILABLE" << endl;
               break;
           }
       });

       capsSubscription = myProxy->getGnssCapabilitiesEvent().subscribe(
               [&](const uint32_t &val) {
                   cout << "Received caps change event: " << val << endl;
               });

       if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_DATA_CB_INFO_BIT) {
               dataSubscription = myProxy->getGnssDataReportEvent().subscribe(
               [&](const LocationTypes::GnssDataT& gnssData) {
                   printGnssData(gnssData);
               });
       }

       if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_LOCATION_CB_INFO_BIT) {
               pvtSubscription = myProxy->getGnssLocationReportEvent().subscribe(
               [&](const LocationTypes::LocationReportT &locationReportInfo) {
                   printPosResport(locationReportInfo);
               });
       }

       if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_MEAS_CB_INFO_BIT) {
               measSubscription = myProxy->getGnssMeasurementReportEvent().subscribe(
               [&](const LocationTypes::GnssMeasurementsT& gnssMeasurements) {
                   printMeasurement(gnssMeasurements);
               });
       }

       if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_SV_CB_INFO_BIT) {
               svSubscription = myProxy->getGnssSvReportEvent().subscribe(
               [&](const vector<LocationTypes::GnssSvDataT> &gnssSv) {
                   printSVInfo(gnssSv);
               });
       }

       if (mask & LocationTypes::GnssReportCbInfoMaskT::GRCIMT_NMEA_CB_INFO_BIT) {
               nmeaSubscription = myProxy->getGnssNmeaEvent().subscribe(
               [&](const uint64_t timestamp, const string nmea) {
                   printNmea(timestamp, nmea);
               });
       }
}

void sessionStart()
{
    uint32_t _intervalInMs = 100;
    LocationTypes::LocationStatusT resp;
    CommonAPI::CallStatus callStatus;
    info.sender_ = 0xEE00;

    if (!myProxy) {
      cout << " mProxy is NULL !! "<< endl;
      return;
    }

    sleep(1);
    myProxy->StartPositionSessionLocationReport(_intervalInMs, mask, callStatus, resp, &info);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
            cout << "StartPositionSessionLocationReport() Remote call failed! callStatus "
        "" << (int)callStatus << endl;
        sessionStarted = false;
    } else {
        sessionStarted = true;
    }
}

void mmfDataInjection(LocationTypes::MapMatchingFeedbackDataT  &mapData)
{
    CommonAPI::CallStatus callStatus;

    myProxy->InjectMapMatchedFeedbackData(mapData, callStatus);
    if (callStatus != CommonAPI::CallStatus::SUCCESS) {
        cout << "mmf not sent" << endl;
    }
}

int main(int argc, char* argv[])
{
    int delay;

    setenv("VSOMEIP_CONFIGURATION", "/vendor/etc/vsomeip_vlan1500.json", 1);
    setenv("COMMONAPI_CONFIG", "/vendor/etc/commonapi4someip.ini" ,1);

    if (isIdlTestClientRunning()) {
        cout<<"previous LocIdlAPIClient is Running, Closing this one!!!!"<<endl;
        return 0;
    }

    /* Command Line parsing*/
    if (!parseCommandLine(argc, argv, delay)) {
        return 0;
    }
    /* signal Handler */
    regSigHandler();

    if (mmfON) {
        t[0] = std::thread(readThread);
        t[1] = std::thread(mmfComputation);
    }

    /* GPTP */
    loadGptpLibFile();
    if ((nullptr != gPTPReqIf) && (nullptr != gPTPReqIf->gptpInitIf)) {
       gPTPReqIf->gptpInitIf();
    }

    CommonAPI::Runtime::setProperty("LogContext", "Location");
    CommonAPI::Runtime::setProperty("LogApplication", "Location");
    CommonAPI::Runtime::setProperty("LibraryBase", "Location");

    shared_ptr < CommonAPI::Runtime > runtime = CommonAPI::Runtime::get();

    string domain = "local";
    string instance = "com.qualcomm.qti.location.Location";
    string connection = "location-fidl-test-client";

    myProxy = runtime->buildProxy<LocationProxy>(domain,
            instance, connection);
    cout << "Checking availability!" << endl;
    while (!myProxy->isAvailable())
        usleep(10);
    cout << "Available..." << endl;

    getLocationCapabilities();
    subscribeGnssResports();
    sessionStart();

    if (mmfON) {
        std::unique_lock<std::mutex> lock(cv_m_mmfTerminate);
        cv_mmfTerminate.wait(lock, [] { return mmfTerminate; });
        mmfTerminate = 0;
        delay = 0;
        cout<<"MMF Computation Terminated!!!!"<<endl;
        lock.unlock();
    }

    if (sessionStarted) {
        sleep(delay);
    }

    DeInitHandles();
    if (myProxy) {
        myProxy.reset();
    }

    if (mmfON) {
        terminateThreads();
    }

    return 0;
}
