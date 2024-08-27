/*
 * Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#ifndef FIDL_LOC_API_H
#define FIDL_LOC_API_H

#include <stdint.h>
#include <stdbool.h>
#include <LocApiBase.h>
#include <ContextBase.h>
#include <loc_fidl_interface.h>
#include <vector>
#include <functional>


#define LOC_CLIENT_INVALID_HANDLE_VALUE (NULL)
#define FIDL_HW_ENGINE_ON  (true)
#define FIDL_HW_ENGINE_OFF (false)

using Resender = std::function<void()>;

using namespace std;
using namespace loc_core;


/* This class derives from the LocApiBase class.
   The members of this class are responsible for converting
   the Fidl Location API data structures into Loc Adapter data structures.
   This class also implements some of the virtual functions that
   handle the requests from loc engine. */
class FidlLocApi : public LocApiBase {
protected:


private:
    bool mInSession;
    GnssPowerMode mPowerMode;
    bool mEngineOn;
    bool mMeasurementsStarted;
    std::vector<Resender> mResenders;
    bool mIsMasterRegistered;
    bool mMasterRegisterNotSupported;
    GnssSvMeasurementSet*  mSvMeasurementSet;
    LOC_API_ADAPTER_EVENT_MASK_T mFdlMask;

    const FidlInterfaceReq *fidlReqIf;

    void registerEventMask(LOC_API_ADAPTER_EVENT_MASK_T adapterMask);


protected:
    virtual enum loc_api_adapter_err
        open(LOC_API_ADAPTER_EVENT_MASK_T mask);
    virtual enum loc_api_adapter_err
        close();

    FidlLocApi(LOC_API_ADAPTER_EVENT_MASK_T exMask,
            ContextBase *context = NULL);
    virtual ~FidlLocApi();

public:
    ElapsedRealtimeEstimator mPositionElapsedRealTimeCal;
    uint32_t mMinInterval;

    static LocApiBase* createFidlLocApi(LOC_API_ADAPTER_EVENT_MASK_T exMask,
                                     ContextBase* context);


    virtual void startFix(const LocPosMode& posMode,
        LocApiResponse *adapterResponse);

    virtual void stopFix(LocApiResponse *adapterResponse);

    virtual void setPositionMode(const LocPosMode& mode);

    virtual void
        setTime(LocGpsUtcTime time, int64_t timeReference, int uncertainty);

    virtual int getNumSvUsed (uint64_t svUsedIdsMask, int totalSvCntInOneConstellation);

    virtual void
        injectPosition(const Location& location, bool onDemandCpi);

    virtual void
        injectPosition(const GnssLocationInfoNotification &locationInfo, bool onDemandCpi);

    virtual void
        deleteAidingData(const GnssAidingData& data, LocApiResponse *adapterResponse);

    virtual void
        informNiResponse(GnssNiResponse userResponse, const void* passThroughData);

    virtual LocationError
        setServerSync(const char* url, int len, LocServerType type);

    virtual LocationError
        setServerSync(unsigned int ip, int port, LocServerType type);

    virtual void
        atlOpenStatus(int handle, int is_succ, char* apn, uint32_t apnLen, AGpsBearerType bear,
                   LocAGpsType agpsType, LocApnTypeMask mask);
    virtual void atlCloseStatus(int handle, int is_succ);
    virtual LocationError setSUPLVersionSync(GnssConfigSuplVersion version);

    virtual enum loc_api_adapter_err setNMEATypesSync(uint32_t typesMask);

    virtual LocationError setLPPConfigSync(GnssConfigLppProfileMask profileMask);


    virtual enum loc_api_adapter_err
        setSensorPropertiesSync(bool gyroBiasVarianceRandomWalk_valid,
                            float gyroBiasVarianceRandomWalk,
                            bool accelBiasVarianceRandomWalk_valid,
                            float accelBiasVarianceRandomWalk,
                            bool angleBiasVarianceRandomWalk_valid,
                            float angleBiasVarianceRandomWalk,
                            bool rateBiasVarianceRandomWalk_valid,
                            float rateBiasVarianceRandomWalk,
                            bool velocityBiasVarianceRandomWalk_valid,
                            float velocityBiasVarianceRandomWalk);

    virtual enum loc_api_adapter_err
        setSensorPerfControlConfigSync(int controlMode, int accelSamplesPerBatch,
            int accelBatchesPerSec, int gyroSamplesPerBatch, int gyroBatchesPerSec,
            int accelSamplesPerBatchHigh, int accelBatchesPerSecHigh,
            int gyroSamplesPerBatchHigh, int gyroBatchesPerSecHigh, int algorithmConfig);
    virtual LocationError
        setAGLONASSProtocolSync(GnssConfigAGlonassPositionProtocolMask aGlonassProtocol);
    virtual LocationError setLPPeProtocolCpSync(GnssConfigLppeControlPlaneMask lppeCP);
    virtual LocationError setLPPeProtocolUpSync(GnssConfigLppeUserPlaneMask lppeUP);
    virtual void getWwanZppFix();
    virtual void getBestAvailableZppFix();
    virtual LocationError setGpsLockSync(GnssConfigGpsLock lock);
    virtual void setConstrainedTuncMode(bool enabled,
                                        float tuncConstraint,
                                        uint32_t energyBudget,
                                        LocApiResponse* adapterResponse=nullptr);
    virtual void setPositionAssistedClockEstimatorMode(bool enabled,
                                                       LocApiResponse* adapterResponse=nullptr);
    virtual void getGnssEnergyConsumed();
    virtual void requestForAidingData(GnssAidingDataSvMask svDataMask);

    virtual LocationError setXtraVersionCheckSync(uint32_t check);

    virtual void startTimeBasedTracking(const TrackingOptions& options,
             LocApiResponse* adapterResponse);
    virtual void stopTimeBasedTracking(LocApiResponse* adapterResponse);
    virtual void startDistanceBasedTracking(uint32_t sessionId, const LocationOptions& options,
             LocApiResponse* adapterResponse);
    virtual void stopDistanceBasedTracking(uint32_t sessionId,
             LocApiResponse* adapterResponse = nullptr);

    virtual GnssConfigSuplVersion convertSuplVersion(const uint32_t suplVersion);
    virtual GnssConfigLppeControlPlaneMask convertLppeCp(const uint32_t lppeControlPlaneMask);
    virtual GnssConfigLppeUserPlaneMask convertLppeUp(const uint32_t lppeUserPlaneMask);

    /* Requests for SV/Constellation Control */
    virtual LocationError setBlacklistSvSync(const GnssSvIdConfig& config);
    virtual void setBlacklistSv(const GnssSvIdConfig& config,
                                LocApiResponse* adapterResponse=nullptr);
    virtual void getBlacklistSv();
    virtual void setConstellationControl(const GnssSvTypeConfig& config,
                                         LocApiResponse *adapterResponse=nullptr);
    virtual void getConstellationControl();
    virtual void resetConstellationControl(LocApiResponse *adapterResponse=nullptr);
    virtual void updateEngineState(bool currentEngineState);
};

extern "C" LocApiBase* getLocApi(LOC_API_ADAPTER_EVENT_MASK_T exMask,
                                 ContextBase *context);

#endif //FIDL_LOC_API_H
