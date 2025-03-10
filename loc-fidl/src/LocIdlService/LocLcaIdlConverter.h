/*
Copyright (c) 2024-2025 Qualcomm Innovation Center, Inc. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause-Clear
*/

/************************************************************************
 * File contains all the conversion functions from LCA reports type
 * to IDL format
 **********************************************************************/
#ifndef LOCLCAIDLCONVERTER_H
#define LOCLCAIDLCONVERTER_H

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <cstring>
#include "LocationClientApi.h"
#include <v1/com/qualcomm/qti/location/LocationTypes.hpp>
#include <v1/com/qualcomm/qti/location/LocationStub.hpp>
#include <gptp_helper.h>

using namespace v1::com::qualcomm::qti::location;
using namespace std;
using namespace location_client;

class LocLcaIdlConverter {

public:

    LocationTypes::LocationT parseBasicLocationInfo
    (
        const ::GnssLocation &basicLoc
    );

    uint8_t parseIDLLocReliability
    (
        ::LocationReliability locReliability
    );

    LocationTypes::LocationReportSvUsedInPositionT parseIDLSvUsedInPosition
    (
        ::GnssLocationSvUsedInPosition halSv
    );

    uint32_t parseIDLNavSolutionMask
    (
        ::GnssLocationNavSolutionMask   navSolMask
    );

    uint32_t parseIDLPosTechMask
    (
        location_client::LocationTechnologyMask   posTechMask
    );

    LocationTypes::LocationReportPositionDynamicsT parseIDLBodyFrameData
    (
        location_client::GnssLocationPositionDynamics bodyFrameData
    );

    LocationTypes::GnssSystemTimeT parseGnssSystemTime
    (
        location_client::GnssSystemTime gnssSystemTime
    );

    LocationTypes::GnssSystemTimeStructTypeT parseIDLGnssTime
    (
        location_client::GnssSystemTimeStructType gnssTime
    );

    LocationTypes::GnssGloTimeStructTypeT parseIDLGloTime
    (
        location_client::GnssGloTimeStructType gloTime
    );

    LocationTypes::GnssConstellationTypeT  parseIDLGnssConstellation
    (
        location_client::Gnss_LocSvSystemEnumType constellation
    );

    vector< LocationTypes::GnssMeasUsageInfoT > parseIDLMeasUsageInfo
    (
        vector<location_client::GnssMeasUsageInfo> measUsageInfo
    );

    uint32_t parseIDLCalibrationStatus
    (
        location_client::DrCalibrationStatusMask statusMask
    );

    uint32_t parseIDLEngMask
    (
        location_client::PositioningEngineMask locOutputEngMask
    );

    LocationTypes::LlaInfoT parseIDLLatLongAltInfo
    (
       location_client::LLAInfo llaVRPBased
    );

    uint32_t parseIDLDrSolStatusMask
    (
        location_client::DrSolutionStatusMask drSolutionStatusMask
    );

    LocationTypes::LocationReportT parseLocReport
    (
        const location_client::GnssLocation &lcaLoc
    );

    LocationTypes::GnssSvTypeT parseIDLSvType
    (
        location_client::GnssSvType  svType
    );

    uint32_t  parseIDLSvOptionMask
    (
        location_client:: GnssSvOptionsMask optionMask
    );

    LocationTypes::GnssSvDataT parseSvReport
    (
        const location_client::GnssSv& gnssSvs
    );

    uint32_t parseGnssClkFlags (
        location_client::GnssMeasurementsClockFlagsMask clkFlags
    );


    LocationTypes::GnssMeasurementsClockT parseIDLMeasClockInfo
    (
        location_client::GnssMeasurementsClock gnssClock
    );

    uint32_t parseIDLMeasFlags(
        location_client::GnssMeasurementsDataFlagsMask gnssFlags
    );

    uint32_t parseIDLStateMask(
        location_client::GnssMeasurementsStateMask stateMask
    );

    uint32_t parseIDLAdrStateMask(
        location_client::GnssMeasurementsAdrStateMask adrStatemask
    );

    uint32_t parseIDLMultiPathIndicator(
        location_client::GnssMeasurementsMultipathIndicator multipathIndicator
    );

    vector<LocationTypes::GnssMeasurementsDataT > parseIDLMeasData
    (
        vector<location_client::GnssMeasurementsData> gnssMeasData
    );

    LocationTypes::GnssMeasurementsT parseMeasurements
    (
        const location_client::GnssMeasurements& gnssMeasurements
    );

    uint32_t parseIDLDataMask
    (
        const location_client::GnssDataMask& mask
    );

    LocationTypes::GnssDataT parseGnssData
    (
        const location_client::GnssData& gnssData
    );

    LocationTypes::GnssSignalTypeT parseIDLSignalType (
        location_client::GnssSignalTypeMask lcaSignalType
    );

     LocLcaIdlConverter();

     ~LocLcaIdlConverter();
};

#endif
