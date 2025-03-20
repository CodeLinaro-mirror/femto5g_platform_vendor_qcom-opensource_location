/*
Copyright (c) 2024-2025 Qualcomm Innovation Center, Inc. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#ifndef LOCIDLAPISTUBIMPL_HPP
#define LOCIDLAPISTUBIMPL_HPP

#include <iostream>
#include <thread>
#include <stdio.h>
#include <CommonAPI/CommonAPI.hpp>
#include <v1/com/qualcomm/qti/location/Location.hpp>
#include <v1/com/qualcomm/qti/location/LocationStub.hpp>
#include <v1/com/qualcomm/qti/location/LocationTypes.hpp>
#include "LocIdlAPIService.h"

using namespace std;
using namespace v1::com::qualcomm::qti::location;

class LocIdlAPIService;
class LocIdlAPIStubImpl: public v1::com::qualcomm::qti::location::LocationStub {

public:

    virtual const CommonAPI::Version& getInterfaceVersion(
            std::shared_ptr<CommonAPI::ClientId> client)
    {
        return CommonAPI::Version(0, 1);
    }

    /// This is the method that will be called on remote calls on the method StartPositionSessionLocationReport.
    virtual void StartPositionSessionLocationReport(
            const std::shared_ptr<CommonAPI::ClientId> client,
            uint32_t intervalInMs, uint32_t gnssReportCallbackMask,
            StartPositionSessionLocationReportReply_t reply);

    /// This is the method that will be called on remote calls on the method StartPositionSessionEngineSpecificLocation.
    virtual void StartPositionSessionEngineSpecificLocation(
            const std::shared_ptr<CommonAPI::ClientId> client,
            uint32_t intervalInMs, uint32_t locReqEngMask, uint32_t engReportCallbackMask,
            StartPositionSessionEngineSpecificLocationReply_t reply);

    /// This is the method that will be called on remote calls on the method StopPositionSession.
    virtual void StopPositionSession(const std::shared_ptr<CommonAPI::ClientId> client,
            StopPositionSessionReply_t reply);

    /// This is the method that will be called on remote calls on the method DeleteAidingData.
    virtual void DeleteAidingData(const std::shared_ptr<CommonAPI::ClientId> client,
            uint32_t deleteMask, DeleteAidingDataReply_t reply);

    /// This is the method that will be called on remote calls on the method ConfigConstellations.
    virtual void ConfigConstellations(const std::shared_ptr<CommonAPI::ClientId> client,
            std::vector<LocationTypes::GnssSvIdInfoT > svList, ConfigConstellationsReply_t reply);

    /// This is the method that will be called on remote calls on the method InjectMapMatchedFeedbackData.
    virtual void InjectMapMatchedFeedbackData(const std::shared_ptr<CommonAPI::ClientId> client,
            LocationTypes::MapMatchingFeedbackDataT mmfData);

    virtual void GetLocationCapabilities(const std::shared_ptr<CommonAPI::ClientId> client,
            GetLocationCapabilitiesReply_t reply);

    const LocIdlAPIService* mApiService;
    LocIdlAPIStubImpl(const LocIdlAPIService* apiService);
    ~LocIdlAPIStubImpl();
};

#endif // LOCIDLAPISTUBIMPL_HPP
