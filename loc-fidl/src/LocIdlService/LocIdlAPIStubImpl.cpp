/*
Copyright (c) 2024-2025 Qualcomm Innovation Center, Inc. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#include <functional>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <LocationClientApi.h>
#include "LocIdlAPIStubImpl.hpp"
#include <time.h>
#define NSEC_IN_ONE_SEC       (1000000000ULL)   /* nanosec in a sec */
using namespace v1::com::qualcomm::qti::location;

using namespace location_client;


LocIdlAPIStubImpl::LocIdlAPIStubImpl(const LocIdlAPIService* apiService):
    mApiService(apiService){
}

LocIdlAPIStubImpl::~LocIdlAPIStubImpl() {
}

void LocIdlAPIStubImpl::StartPositionSessionLocationReport(
            const std::shared_ptr<CommonAPI::ClientId> client,
            uint32_t intervalInMs, uint32_t gnssReportCallbackMask,
            StartPositionSessionLocationReportReply_t reply)
{
    LOC_LOGe("==== Start Fused report Session intervalInMs %d gnssReportCallbackMask %d",
            intervalInMs, gnssReportCallbackMask);
    if (mApiService) {
        mApiService->startPositionSession(client, intervalInMs, gnssReportCallbackMask, reply);
    }
}

void LocIdlAPIStubImpl::StartPositionSessionEngineSpecificLocation(
            const std::shared_ptr<CommonAPI::ClientId> client,
            uint32_t intervalInMs, uint32_t locReqEngMask, uint32_t engReportCallbackMask,
            StartPositionSessionEngineSpecificLocationReply_t reply)

{
    LOC_LOGe("==== Start Engine Specific Session intervalInMs %d locReqEngMask %d",
            intervalInMs, locReqEngMask);
    if (mApiService) {
        mApiService->startPositionSession(client, intervalInMs, locReqEngMask,
                engReportCallbackMask, reply);
    }
}

void LocIdlAPIStubImpl::StopPositionSession(
            const std::shared_ptr<CommonAPI::ClientId> client,
            StopPositionSessionReply_t reply)
{
    if (mApiService) {
        LOC_LOGi("==== STOP Session request");
        mApiService->stopPositionSession(client, reply);
        if (mApiService->mEnableStopSession) {
            reply(LocationTypes::LocationStatusT::LOCATION_STATUS_T_SUCCESS);
        } else {
            reply(LocationTypes::LocationStatusT::LOCATION_STATUS_T_NOT_SUPPORTED);
        }
    }
}

void LocIdlAPIStubImpl::DeleteAidingData(const std::shared_ptr<CommonAPI::ClientId> client,
            uint32_t deleteMask, DeleteAidingDataReply_t reply)
{
    if (mApiService) {
        mApiService->deleteAidingDataRequest(client, deleteMask, reply);
    }
}

void LocIdlAPIStubImpl::ConfigConstellations(
        const std::shared_ptr<CommonAPI::ClientId> client,
        std::vector<LocationTypes::GnssSvIdInfoT > svList, ConfigConstellationsReply_t reply)
{
    // This API is currently not supported.
    reply(LocationTypes::LocationStatusT::LOCATION_STATUS_T_NOT_SUPPORTED);
}

void LocIdlAPIStubImpl::InjectMapMatchedFeedbackData(
        const std::shared_ptr<CommonAPI::ClientId> client,
        LocationTypes::MapMatchingFeedbackDataT mmfData)
{
    if (mApiService) {
        mApiService->injectMapMatchedFeedbackData(client, mmfData);
    }
}
void LocIdlAPIStubImpl::GetLocationCapabilities(const std::shared_ptr<CommonAPI::ClientId> client,
            GetLocationCapabilitiesReply_t reply) {
    if (mApiService) {
        uint32_t mask = mApiService->getLocationCapabilitiesMask();
        reply(mask);
    } else {
        reply(0);
    }

}
