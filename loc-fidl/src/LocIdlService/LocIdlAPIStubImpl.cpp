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
#define LOG_TAG "LOC_IDL_SERVICE"
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
    LOC_LOGe("==== Client %"PRIu64" intervalInMs %d gnssReportCallbackMask %d",
            client->hashCode(), intervalInMs, gnssReportCallbackMask);
    if (mApiService) {
        mApiService->startPositionSession(client, intervalInMs, gnssReportCallbackMask, reply);
    }
}

void LocIdlAPIStubImpl::StartPositionSessionEngineSpecificLocation(
            const std::shared_ptr<CommonAPI::ClientId> client,
            uint32_t intervalInMs, uint32_t locReqEngMask, uint32_t engReportCallbackMask,
            StartPositionSessionEngineSpecificLocationReply_t reply)

{
    LOC_LOGe("==== Client %"PRIu64" intervalInMs %d locReqEngMask %d",
            client->hashCode(), intervalInMs, locReqEngMask);
    if (mApiService) {
        mApiService->startPositionSession(client, intervalInMs, locReqEngMask,
                engReportCallbackMask, reply);
    }
}

void LocIdlAPIStubImpl::StopPositionSession(
            const std::shared_ptr<CommonAPI::ClientId> client,
            StopPositionSessionReply_t reply)
{
    LOC_LOGi("==== Client %"PRIu64" STOP Session request",
            client->hashCode());
    if (mApiService) {
        mApiService->stopPositionSession(client, reply);
        if (mApiService->mEnableStopSession) {
            reply(LocationTypes::LocationStatusT::LOCATION_STATUS_T_SUCCESS);
            LOC_LOGi("==== Client %"PRIu64" response SUCCESS", client->hashCode());
        } else {
            reply(LocationTypes::LocationStatusT::LOCATION_STATUS_T_NOT_SUPPORTED);
            LOC_LOGi("==== Client %"PRIu64" response NOT_SUPPORTED", client->hashCode());
        }
    }
}

void LocIdlAPIStubImpl::DeleteAidingData(const std::shared_ptr<CommonAPI::ClientId> client,
            uint32_t deleteMask, DeleteAidingDataReply_t reply)
{
    LOC_LOGi("==== Client %"PRIu64" ", client->hashCode());
    if (mApiService) {
        mApiService->deleteAidingDataRequest(client, deleteMask, reply);
    }
}

void LocIdlAPIStubImpl::ConfigConstellations(
        const std::shared_ptr<CommonAPI::ClientId> client,
        std::vector<LocationTypes::GnssSvIdInfoT > svList, ConfigConstellationsReply_t reply)
{
    LOC_LOGi("==== Client %"PRIu64" ", client->hashCode());
    // This API is currently not supported.
    reply(LocationTypes::LocationStatusT::LOCATION_STATUS_T_NOT_SUPPORTED);
}

void LocIdlAPIStubImpl::InjectMapMatchedFeedbackData(
        const std::shared_ptr<CommonAPI::ClientId> client,
        LocationTypes::MapMatchingFeedbackDataT mmfData)
{
    LOC_LOGi("==== Client %"PRIu64" ", client->hashCode());
    if (mApiService) {
        mApiService->injectMapMatchedFeedbackData(client, mmfData);
    }
}
void LocIdlAPIStubImpl::GetLocationCapabilities(const std::shared_ptr<CommonAPI::ClientId> client,
            GetLocationCapabilitiesReply_t reply) {
    LOC_LOGi("==== Client %"PRIu64" ", client->hashCode());
    if (mApiService) {
        uint32_t mask = mApiService->getLocationCapabilitiesMask();
        reply(mask);
        LOC_LOGi("==== Client %"PRIu64" response mask 0x%x",
                client->hashCode(), mask);
    } else {
        reply(0);
    }

}
