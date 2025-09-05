/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#include "LocIdlClientDevice.h"

#define DEFAULT_ELAPSED_REAL_TIMEUNC (100)

void LocIdlClientDevice::fillPosTechMask(unsigned int techmask, unsigned int &outMask)
{
    outMask = 0;
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_GNSS_BIT) {
        outMask |= LOC_POS_TECH_MASK_SATELLITE;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_CELL_BIT) {
        outMask |= LOC_POS_TECH_MASK_CELLID;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_WIFI_BIT) {
        outMask |= LOC_POS_TECH_MASK_WIFI;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_SENSORS_BIT) {
        outMask |= LOC_POS_TECH_MASK_SENSORS;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_REF_LOC_BIT) {
        outMask |= LOC_POS_TECH_MASK_REFERENCE_LOCATION;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_INJECTED_COARSE_POS_BIT) {
        outMask |= LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_AFLT_BIT) {
        outMask |= LOC_POS_TECH_MASK_AFLT;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_HYBRID_BIT) {
        outMask |= LOC_POS_TECH_MASK_HYBRID;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_PPE_BIT) {
        outMask |= LOC_POS_TECH_MASK_PPE;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_VEH_BIT) {
        outMask |= LOC_POS_TECH_MASK_VEH;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_VIS_BIT) {
        outMask |= LOC_POS_TECH_MASK_VIS;
    }
    if (techmask & LocationTypes::LocationTechnologyMaskT::LTMT_PROPAGATED_BIT) {
        outMask |= LOC_POS_TECH_MASK_PROPAGATED;
    }
}

uint32_t LocIdlClientDevice::parseConstellationType(LocationTypes::GnssConstellationTypeT idlConstellation)
{
    uint32_t res = 0;
    switch(idlConstellation) {
        case LocationTypes::GnssConstellationTypeT::GCTT_GLONASS:
            res = GNSS_LOC_SV_SYSTEM_GLONASS;
            break;
        case LocationTypes::GnssConstellationTypeT::GCTT_QZSS:
            res = GNSS_LOC_SV_SYSTEM_QZSS;
            break;
        case LocationTypes::GnssConstellationTypeT::GCTT_BEIDOU:
            res = GNSS_LOC_SV_SYSTEM_BDS;
            break;
        case LocationTypes::GnssConstellationTypeT::GCTT_GALILEO:
            res = GNSS_LOC_SV_SYSTEM_GALILEO;
            break;
        case LocationTypes::GnssConstellationTypeT::GCTT_SBAS:
            res = GNSS_LOC_SV_SYSTEM_SBAS;
            break;
        case LocationTypes::GnssConstellationTypeT::GCTT_NAVIC:
            res = GNSS_LOC_SV_SYSTEM_NAVIC;
            break;
        case LocationTypes::GnssConstellationTypeT::GCTT_GPS:
            res = GNSS_LOC_SV_SYSTEM_GPS;
            break;
    }
    return res;
}

uint32_t LocIdlClientDevice::parseSvType(LocationTypes::GnssSvTypeT svType)
{
    uint32_t res = 0;
    switch(svType) {
        case LocationTypes::GnssSvTypeT::GNSS_SV_TYPE_T_GPS:
            res = GNSS_SV_TYPE_GPS;
            break;
        case LocationTypes::GnssSvTypeT::GNSS_SV_TYPE_T_SBAS:
            res = GNSS_SV_TYPE_SBAS;
            break;
        case LocationTypes::GnssSvTypeT::GNSS_SV_TYPE_T_GLONASS:
            res = GNSS_SV_TYPE_GLONASS;
            break;
        case LocationTypes::GnssSvTypeT::GNSS_SV_TYPE_T_QZSS:
            res = GNSS_SV_TYPE_QZSS;
            break;
        case LocationTypes::GnssSvTypeT::GNSS_SV_TYPE_T_BEIDOU:
            res = GNSS_SV_TYPE_BEIDOU;
            break;
        case LocationTypes::GnssSvTypeT::GNSS_SV_TYPE_T_GALILEO:
            res = GNSS_SV_TYPE_GALILEO;
            break;
        case LocationTypes::GnssSvTypeT::GNSS_SV_TYPE_T_NAVIC:
            res = GNSS_SV_TYPE_NAVIC;
            break;
    }
    return res;
}

uint32_t LocIdlClientDevice::parseMeasStateMask(uint32_t stateMask)
{
    uint32_t resMask = 0;
    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_UNKNOWN & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_UNKNOWN_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_CODE_LOCK & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_CODE_LOCK_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_BIT_SYNC & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_BIT_SYNC_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_SUBFRAME_SYNC & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_SUBFRAME_SYNC_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_TOW_DECODED & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_TOW_DECODED_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_MSEC_AMBIGUOUS & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_MSEC_AMBIGUOUS_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_SYMBOL_SYNC & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_SYMBOL_SYNC_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_GLO_STRING_SYNC & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_GLO_STRING_SYNC_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_GLO_TOD_DECODED & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_GLO_TOD_DECODED_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_BDS_D2_BIT_SYNC & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_BDS_D2_BIT_SYNC_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_BDS_D2_SUBFRAME_SYNC & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_BDS_D2_SUBFRAME_SYNC_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_GAL_E1BC_CODE_LOCK & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_GAL_E1BC_CODE_LOCK_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_GAL_E1C_2ND_CODE_LOCK & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_GAL_E1C_2ND_CODE_LOCK_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_GAL_E1B_PAGE_SYNC & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_GAL_E1B_PAGE_SYNC_BIT;
    }

    if (LocationTypes::GnssMeasurementsStateMaskT::GMSMT_SBAS_SYNC & stateMask) {
      resMask |= GNSS_MEASUREMENTS_STATE_SBAS_SYNC_BIT;
    }

    return resMask;
}

uint32_t LocIdlClientDevice::parseAdrStateMask(uint32_t adrStatemask)
{
    uint32_t retAdrMask = 0;
    if (LocationTypes::GnssMeasurementsAdrStateMaskT::GMASMT_STATE_UNKNOWN & adrStatemask) {
        retAdrMask |= GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_UNKNOWN;
    }

    if (LocationTypes::GnssMeasurementsAdrStateMaskT::GMASMT_STATE_VALID_BIT & adrStatemask) {
        retAdrMask |= GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_VALID_BIT;
    }

    if (LocationTypes::GnssMeasurementsAdrStateMaskT::GMASMT_STATE_RESET_BIT & adrStatemask) {
        retAdrMask |= GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_RESET_BIT;
    }

    if (LocationTypes::GnssMeasurementsAdrStateMaskT::GMASMT_STATE_CYCLE_SLIP_BIT & adrStatemask) {
        retAdrMask |= GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_CYCLE_SLIP_BIT;
    }

    if (LocationTypes::GnssMeasurementsAdrStateMaskT::GMASMT_STATE_HALF_CYCLE_RESOLVED_BIT & adrStatemask) {
        retAdrMask |= GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_HALF_CYCLE_RESOLVED_BIT;
    }

    return retAdrMask;
}

uint32_t LocIdlClientDevice::parseMultiPathIndicator(uint32_t multipathIndicator)
{
    uint32_t resMultiPathInd = 0;
    switch (multipathIndicator) {
    case LocationTypes::GnssMeasurementsMultipathIndicatorT::GMMIT_PRESENT:
        resMultiPathInd = GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_PRESENT;
        break;
    case LocationTypes::GnssMeasurementsMultipathIndicatorT::GMMIT_NOT_PRESENT:
        resMultiPathInd = GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_NOT_PRESENT;
        break;
    }

    return resMultiPathInd;
}

uint32_t LocIdlClientDevice::parseDataMask(uint32_t mask)
{
    uint32_t retDataMask = 0;
    if (LocationTypes::GnssDataMaskT::GDMT_JAMMER_IND_BIT & mask) {
        retDataMask |= GNSS_LOC_DATA_JAMMER_IND_BIT;
    }
    if (LocationTypes::GnssDataMaskT::GDMT_AGC_BIT & mask) {
        retDataMask |= GNSS_LOC_DATA_AGC_BIT;
    }
    return retDataMask;
}

uint32_t LocIdlClientDevice::parseGnssSvOptionsMask(uint32_t idlOptionMask)
{
    uint32_t optionMask = 0;

    if (LocationTypes::GnssSvOptionsMaskT::GSOMT_HAS_EPHEMER_BIT & idlOptionMask) {
        optionMask |= GNSS_SV_OPTIONS_HAS_EPHEMER_BIT;
    }

    if (LocationTypes::GnssSvOptionsMaskT::GSOMT_HAS_ALMANAC_BIT & idlOptionMask) {
        optionMask |= GNSS_SV_OPTIONS_HAS_ALMANAC_BIT;
    }

    if (LocationTypes::GnssSvOptionsMaskT::GSOMT_HAS_USED_IN_FIX_BIT  & idlOptionMask) {
        optionMask |= GNSS_SV_OPTIONS_USED_IN_FIX_BIT;
    }

    if (LocationTypes::GnssSvOptionsMaskT::GSOMT_HAS_CARRIER_FREQ_BIT & idlOptionMask) {
        optionMask |= GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT;
    }

    if (LocationTypes::GnssSvOptionsMaskT::GSOMT_HAS_GNSS_SIGNAL_TYPE_BIT & idlOptionMask) {
        optionMask |= GNSS_SV_OPTIONS_HAS_GNSS_SIGNAL_TYPE_BIT;
    }

    if (LocationTypes::GnssSvOptionsMaskT::GSOMT_HAS_BASEBAND_CARRIER_TO_NOISE_BIT & idlOptionMask) {
        optionMask |= GNSS_SV_OPTIONS_HAS_BASEBAND_CARRIER_TO_NOISE_BIT;
    }

    if (LocationTypes::GnssSvOptionsMaskT::GSOMT_HAS_ELEVATION_BIT & idlOptionMask) {
        optionMask |= GNSS_SV_OPTIONS_HAS_ELEVATION_BIT;
    }

    if (LocationTypes::GnssSvOptionsMaskT::GSOMT_HAS_AZIMUTH_BIT & idlOptionMask) {
        optionMask |= GNSS_SV_OPTIONS_HAS_AZIMUTH_BIT;
    }
    return optionMask;
}


uint32_t LocIdlClientDevice::parseDrSolutionStatusMask(uint32_t idlDrSolMask)
{
    uint32_t drSolutionStatusMask = 0;
    if (LocationTypes::DrSolutionStatusMaskT::DSSMT_VEHICLE_SENSOR_SPEED_INPUT_DETECTED & idlDrSolMask) {
        drSolutionStatusMask |= VEHICLE_SENSOR_SPEED_INPUT_DETECTED;
    }
    if (LocationTypes::DrSolutionStatusMaskT::DSSMT_VEHICLE_SENSOR_SPEED_INPUT_USED & idlDrSolMask) {
        drSolutionStatusMask |= VEHICLE_SENSOR_SPEED_INPUT_USED;
    }
    return drSolutionStatusMask;
}

uint32_t LocIdlClientDevice::parsegnssSystemTime(uint8_t idlTimeSrc)
{
    uint32_t timeSrc= 0;
    switch (idlTimeSrc) {
        case LocationTypes::GnssLocSvSystemTypeT::GLSSTT_GPS:
            timeSrc = GNSS_LOC_SV_SYSTEM_GPS;
            break;
        case LocationTypes::GnssLocSvSystemTypeT::GLSSTT_GAL:
            timeSrc = GNSS_LOC_SV_SYSTEM_GALILEO;
            break;
        case LocationTypes::GnssLocSvSystemTypeT::GLSSTT_SBAS:
            timeSrc = GNSS_LOC_SV_SYSTEM_SBAS;
            break;
        case LocationTypes::GnssLocSvSystemTypeT::GLSSTT_GLO:
            timeSrc = GNSS_LOC_SV_SYSTEM_GLONASS;
            break;
        case LocationTypes::GnssLocSvSystemTypeT::GLSSTT_BDS:
            timeSrc = GNSS_LOC_SV_SYSTEM_BDS;
            break;
         case LocationTypes::GnssLocSvSystemTypeT::GLSSTT_QZSS:
            timeSrc = GNSS_LOC_SV_SYSTEM_QZSS;
            break;
        case LocationTypes::GnssLocSvSystemTypeT::GLSSTT_NAVIC:
            timeSrc = GNSS_LOC_SV_SYSTEM_NAVIC;
            break;
    }
    return timeSrc;
}

uint32_t LocIdlClientDevice::parseLocReliability(uint32_t idlReliability)
{
    uint32_t reliability = LOCATION_RELIABILITY_NOT_SET;
    switch (idlReliability) {
        case LocationTypes::LocationReliabilityT::LOCATION_RELIABILIT_T_VERY_LOW:
            reliability = LOCATION_RELIABILITY_VERY_LOW;
            break;
        case LocationTypes::LocationReliabilityT::LOCATION_RELIABILIT_T_LOW:
            reliability = LOCATION_RELIABILITY_LOW;
            break;
        case LocationTypes::LocationReliabilityT::LOCATION_RELIABILIT_T_MEDIUM:
            reliability = LOCATION_RELIABILITY_MEDIUM;
            break;
        case LocationTypes::LocationReliabilityT::LOCATION_RELIABILIT_T_HIGH:
            reliability = LOCATION_RELIABILITY_HIGH;
            break;
        default:
            reliability = LOCATION_RELIABILITY_NOT_SET;
            break;
    }
    return reliability;
}

uint32_t LocIdlClientDevice::parseSignalType (uint32_t idlSignalType)
{
    uint32_t gnssSignalType = 0;

    switch(idlSignalType) {
        case LocationTypes::GnssSignalTypeT::GSTT_GPS_L1CA_BIT:
            gnssSignalType = GNSS_SIGNAL_GPS_L1CA;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_GPS_L1C_BIT:
            gnssSignalType = GNSS_SIGNAL_GPS_L1C;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_GPS_L2_BIT:
            gnssSignalType = GNSS_SIGNAL_GPS_L2;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_GPS_L5_BIT:
            gnssSignalType = GNSS_SIGNAL_GPS_L5;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_GLONASS_G1_BIT:
            gnssSignalType = GNSS_SIGNAL_GLONASS_G1;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_GLONASS_G2_BIT:
            gnssSignalType = GNSS_SIGNAL_GLONASS_G2;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_GALILEO_E1_BIT:
            gnssSignalType = GNSS_SIGNAL_GALILEO_E1;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_GALILEO_E5A_BIT:
            gnssSignalType = GNSS_SIGNAL_GALILEO_E5A;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_GALILEO_E5B_BIT:
            gnssSignalType = GNSS_SIGNAL_GALILEO_E5B;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_BEIDOU_B1_BIT:
            gnssSignalType = GNSS_SIGNAL_BEIDOU_B1;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_BEIDOU_B2_BIT:
            gnssSignalType = GNSS_SIGNAL_BEIDOU_B2;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_QZSS_L1CA_BIT:
            gnssSignalType = GNSS_SIGNAL_QZSS_L1CA;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_QZSS_L1S_BIT:
            gnssSignalType = GNSS_SIGNAL_QZSS_L1S;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_QZSS_L2_BIT:
            gnssSignalType = GNSS_SIGNAL_QZSS_L2;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_QZSS_L5_BIT:
            gnssSignalType = GNSS_SIGNAL_QZSS_L5;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_SBAS_L1_BIT:
            gnssSignalType = GNSS_SIGNAL_SBAS_L1;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_BEIDOU_B1I_BIT:
            gnssSignalType = GNSS_SIGNAL_BEIDOU_B1I;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_BEIDOU_B1C_BIT:
            gnssSignalType = GNSS_SIGNAL_BEIDOU_B1C;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_BEIDOU_B2I_BIT:
            gnssSignalType = GNSS_SIGNAL_BEIDOU_B2I;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_BEIDOU_B2AI_BIT:
            gnssSignalType = GNSS_SIGNAL_BEIDOU_B2AI;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_NAVIC_L5_BIT:
            gnssSignalType = GNSS_SIGNAL_NAVIC_L5;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_BEIDOU_B2AQ_BIT:
            gnssSignalType = GNSS_SIGNAL_BEIDOU_B2AQ;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_BEIDOU_B2BI_BIT:
            gnssSignalType = GNSS_SIGNAL_BEIDOU_B2BI;
            break;
        case LocationTypes::GnssSignalTypeT::GSTT_BEIDOU_B2BQ_BIT:
            gnssSignalType = GNSS_SIGNAL_BEIDOU_B2BQ;
            break;
    }
    return gnssSignalType;
}

uint32_t LocIdlClientDevice::parseEngMask(uint32_t idlEngMask) {
    uint32_t engMask = 0;
    if (LocationTypes::PositioningEngineMaskT::PEMT_STANDARD_POSITIONING_ENGINE & idlEngMask) {
        engMask |= STANDARD_POSITIONING_ENGINE;
    }

    if (LocationTypes::PositioningEngineMaskT::PEMT_DEAD_RECKONING_ENGINE & idlEngMask) {
        engMask |= DEAD_RECKONING_ENGINE;
    }

    if (LocationTypes::PositioningEngineMaskT::PEMT_PRECISE_POSITIONING_ENGINE  & idlEngMask) {
        engMask |= PRECISE_POSITIONING_ENGINE;
    }

    if (LocationTypes::PositioningEngineMaskT::PEMT_VP_POSITIONING_ENGINE & idlEngMask) {
        engMask |= VP_POSITIONING_ENGINE;
    }
    return engMask;
}

GnssMeasurementsCodeType LocIdlClientDevice::parseMeasCodeType(uint32_t idlMeasCodeType) {
    GnssMeasurementsCodeType measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_A;

    switch(idlMeasCodeType) {
       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_A:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_A;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_B:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_B;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_I:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_I;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_L:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_L;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_M:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_M;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_P:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_P;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_Q:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_Q;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_S:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_S;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_W:
          measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_W;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_X:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_X;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_Y:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_Y;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_Z:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_Z;
       break;

       case LocationTypes::GnssMeasCodeTypeT::GNSS_MEAS_CODE_TYPE_OTHER:
           measCodeType = GNSS_MEASUREMENTS_CODE_TYPE_OTHER;
       break;
    }

    return measCodeType;
}

void LocIdlClientDevice::getLocationRpt(
                            const LocationTypes::LocationReportT &_locationReport,
                            UlpLocation &ulpLoc, locIdlClientDiagPosition    &gnssPosDiag)
{
    const LocationTypes::LocationT &location = _locationReport.getLocInfo();
    uint32_t flags = location.getFlags();

    memset(&ulpLoc, 0x00, sizeof(UlpLocation));
    ulpLoc.size = sizeof(UlpLocation);
    ulpLoc.gpsLocation.size = sizeof(LocGpsLocation);
    /** Contains LocGpsLocationFlags bits. */
    ulpLoc.gpsLocation.flags = 0;

    gnssPosDiag.flags = 0;
    gnssPosDiag.gnssInfoFlags = 0;

    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_LAT_LONG_BIT) {
        ulpLoc.gpsLocation.flags |=  LOC_GPS_LOCATION_HAS_LAT_LONG;
        ulpLoc.gpsLocation.latitude = location.getLatitude();
        ulpLoc.gpsLocation.longitude = location.getLongitude();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_LAT_LONG_BIT;
        gnssPosDiag.latitude = location.getLatitude();
        gnssPosDiag.longitude = location.getLongitude();
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_ALTITUDE_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_ALTITUDE;
        ulpLoc.gpsLocation.altitude = location.getAltitude();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ALTITUDE_BIT;
        gnssPosDiag.altitude = location.getAltitude();
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_SPEED_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_SPEED;
        ulpLoc.gpsLocation.speed = location.getSpeed();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_SPEED_BIT;
        gnssPosDiag.speed = location.getSpeed();
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_BEARING_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_BEARING;
        ulpLoc.gpsLocation.bearing = location.getBearing();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_BEARING_BIT;
        gnssPosDiag.bearing = location.getBearing();
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_ACCURACY_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_ACCURACY;
        ulpLoc.gpsLocation.accuracy = location.getHorizontalAccuracy();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ACCURACY_BIT;
        gnssPosDiag.horizontalAccuracy = location.getHorizontalAccuracy();
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_VERTICAL_ACCURACY_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_VERT_UNCERTAINITY;
        ulpLoc.gpsLocation.vertUncertainity = location.getVerticalAccuracy();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_VERTICAL_ACCURACY_BIT;
        gnssPosDiag.verticalAccuracy = location.getVerticalAccuracy();
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_SPEED_ACCURACY_BIT) {
        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_SPEED_ACCURACY_BIT;
        gnssPosDiag.speedAccuracy = location.getSpeedAccuracy();
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_BEARING_ACCURACY_BIT) {
        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_BEARING_ACCURACY_BIT;
        gnssPosDiag.bearingAccuracy = location.getBearingAccuracy();
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_TIMESTAMP_BIT) {
        ulpLoc.gpsLocation.timestamp = location.getTimestamp();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_TIMESTAMP_BIT;
        gnssPosDiag.timestamp = location.getTimestamp();
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_ELAPSED_REAL_TIME_BIT) {
        uint64_t boot_time_ns = 0;
        uint64_t  tx_ptp_time_ns = _locationReport.getElapsedgPtpTime();
        if ((nullptr == gPTPReqIf) ||
           (false == gPTPReqIf->gptpGetBootTimeFromPtpTimeIf(&boot_time_ns, tx_ptp_time_ns))) {
            boot_time_ns = tx_ptp_time_ns;
        }
        ulpLoc.gpsLocation.elapsedRealTime = boot_time_ns;
        ulpLoc.gpsLocation.elapsedRealTimeUnc = DEFAULT_ELAPSED_REAL_TIMEUNC;

        LOC_LOGD("%s] --> elapsedRealTime:%ld, ", __func__, ulpLoc.gpsLocation.elapsedRealTime);

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_BIT;
        gnssPosDiag.elapsedRealTimeNs = ulpLoc.gpsLocation.elapsedRealTime;
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_ELAPSED_REAL_TIME_UNC_BIT) {
        ulpLoc.gpsLocation.elapsedRealTimeUnc = _locationReport.getElapsedgPtpTimeUnc();
        if (0 == ulpLoc.gpsLocation.elapsedRealTimeUnc) {
            ulpLoc.gpsLocation.elapsedRealTimeUnc = DEFAULT_ELAPSED_REAL_TIMEUNC;
        }
        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_UNC_BIT;
        gnssPosDiag.elapsedRealTimeUncNs = ulpLoc.gpsLocation.elapsedRealTimeUnc;
    }
    if (flags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_TIME_UNC_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_TIME_UNC_BIT;
        gnssPosDiag.timeUncMs = location.getTimeUncMs();
        ulpLoc.gpsLocation.elapsedRealTimeUnc = location.getTimeUncMs();
        if (0 == ulpLoc.gpsLocation.elapsedRealTimeUnc) {
            ulpLoc.gpsLocation.elapsedRealTimeUnc = DEFAULT_ELAPSED_REAL_TIMEUNC;
        }
    }

    ulpLoc.gpsLocation.spoof_mask = 0;
    ulpLoc.position_source = ULP_LOCATION_IS_FROM_GNSS ;

    uint32_t techmask = location.getTechMask();
    ulpLoc.tech_mask = 0;
    fillPosTechMask(techmask, ulpLoc.tech_mask);
    gnssPosDiag.techMask = (locIdlClientDiagLocationTechnologyMask)techmask;
    ulpLoc.unpropagatedPosition = false;
}

void LocIdlClientDevice::getLocationExtendedRpt(
                                const LocationTypes::LocationReportT &_locationReport,
                                GpsLocationExtended &gpsLocExt,
                                locIdlClientDiagPosition    &gnssPosDiag
                                )
{
    const LocationTypes::LocationT &location = _locationReport.getLocInfo();
    uint32_t lInfoflags = location.getFlags();
    memset(&gpsLocExt, 0x00, sizeof(GpsLocationExtended));
    gpsLocExt.size = sizeof(GpsLocationExtended);
    gpsLocExt.flags = 0;
    gnssPosDiag.reportingLatency = _locationReport.getReportingLatency();
    gnssPosDiag.sessionStartBootTimestampNs = mSessionStartBootTimestampNs;

    uint64_t lFlags = _locationReport.getLocationInfoFlags();
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_ALTITUDE_MEAN_SEA_LEVEL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL;
        gpsLocExt.altitudeMeanSeaLevel = _locationReport.getAltitudeMeanSeaLevel();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT;
        gnssPosDiag.altitudeMeanSeaLevel = _locationReport.getAltitudeMeanSeaLevel();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_DOP) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_DOP;
        gpsLocExt.pdop = _locationReport.getPdop();
        gpsLocExt.hdop = _locationReport.getHdop();
        gpsLocExt.vdop =  _locationReport.getVdop();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_DOP_BIT;
        gnssPosDiag.pdop = _locationReport.getPdop();
        gnssPosDiag.hdop = _locationReport.getHdop();
        gnssPosDiag.vdop =  _locationReport.getVdop();
        gnssPosDiag.gdop = _locationReport.getGdop();
        gnssPosDiag.tdop = _locationReport.getTdop();
    }

    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_MAGNETIC_DEVIATION) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_MAG_DEV;
        gpsLocExt.magneticDeviation = _locationReport.getMagneticDeviation();

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT;
        gnssPosDiag.magneticDeviation = _locationReport.getMagneticDeviation();
    }
    if (lInfoflags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_VERTICAL_ACCURACY_BIT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_VERT_UNC;
        gpsLocExt.vert_unc = location.getVerticalAccuracy();
    }
    if (lInfoflags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_SPEED_ACCURACY_BIT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_SPEED_UNC;
        gpsLocExt.speed_unc = location.getSpeedAccuracy();
    }
    if (lInfoflags & LocationTypes::LocationFlagsMaskT::LFMT_HAS_BEARING_ACCURACY_BIT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_BEARING_UNC;
        gpsLocExt.bearing_unc = location.getBearingAccuracy ();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_HOR_RELIABILITY) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_RELIABILITY;
        uint32_t hReliability = parseLocReliability(_locationReport.getHorReliability());
        gpsLocExt.horizontal_reliability = (LocReliability)hReliability;

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT;
        gnssPosDiag.horReliability = (locIdlClientDiagLocationReliability)hReliability;
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_VER_RELIABILITY) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_VERT_RELIABILITY;
        uint32_t vReliability = parseLocReliability(_locationReport.getVerReliability());
        gpsLocExt.vertical_reliability = (LocReliability)vReliability;

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_VER_RELIABILITY_BIT;
        gnssPosDiag.verReliability = (locIdlClientDiagLocationReliability)vReliability;
    }
    if (lFlags &
            LocationTypes::LocationReportFlagMaskT::LRFMT_HOR_ACCURACY_ELIP_SEMI_MAJOR) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MAJOR;
        gpsLocExt.horUncEllipseSemiMajor = _locationReport.getHorUncEllipseSemiMajor();

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT;
        gnssPosDiag.horUncEllipseSemiMajor = _locationReport.getHorUncEllipseSemiMajor();
    }
    if (lFlags &
            LocationTypes::LocationReportFlagMaskT::LRFMT_HOR_ACCURACY_ELIP_SEMI_MINOR) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MINOR;
        gpsLocExt.horUncEllipseSemiMinor = _locationReport.getHorUncEllipseSemiMinor();

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MINOR_BIT;
        gnssPosDiag.horUncEllipseSemiMinor = _locationReport.getHorUncEllipseSemiMinor();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_HOR_ACCURACY_ELIP_AZIMUTH) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_AZIMUTH;
        gpsLocExt.horUncEllipseOrientAzimuth = _locationReport.getHorUncEllipseOrientAzimuth();

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT;
        gnssPosDiag.horUncEllipseOrientAzimuth = _locationReport.getHorUncEllipseOrientAzimuth();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_NUM_SV_USED_IN_POS) {
        gnssPosDiag.gnssInfoFlags |=
            LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NUM_SV_USED_IN_POSITION_BIT;
        gnssPosDiag.numSvUsedInPosition = _locationReport.getNumSvUsedInPosition();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_NUM_SV_USED_IN_POS) {
        const LocationTypes::LocationReportSvUsedInPositionT &svUsed =
                                                _locationReport.getSvUsedInPosition();
        gpsLocExt.gnss_sv_used_ids.gps_sv_used_ids_mask = svUsed.getGpsSvUsedIdsMask();
        gpsLocExt.gnss_sv_used_ids.glo_sv_used_ids_mask = svUsed.getGloSvUsedIdsMask();
        gpsLocExt.gnss_sv_used_ids.gal_sv_used_ids_mask = svUsed.getGalSvUsedIdsMask();
        gpsLocExt.gnss_sv_used_ids.bds_sv_used_ids_mask = svUsed.getBdsSvUsedIdsMask();
        gpsLocExt.gnss_sv_used_ids.qzss_sv_used_ids_mask = svUsed.getQzssSvUsedIdsMask();
        gpsLocExt.gnss_sv_used_ids.navic_sv_used_ids_mask = svUsed.getNavicSvUsedIdsMask();

        gnssPosDiag.gnssInfoFlags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NUM_SV_USED_IN_POSITION_BIT;
        gnssPosDiag.svUsedInPosition.gpsSvUsedIdsMask = svUsed.getGpsSvUsedIdsMask();
        gnssPosDiag.svUsedInPosition.gloSvUsedIdsMask = svUsed.getGloSvUsedIdsMask();
        gnssPosDiag.svUsedInPosition.galSvUsedIdsMask = svUsed.getGalSvUsedIdsMask();
        gnssPosDiag.svUsedInPosition.bdsSvUsedIdsMask = svUsed.getBdsSvUsedIdsMask();
        gnssPosDiag.svUsedInPosition.qzssSvUsedIdsMask = svUsed.getQzssSvUsedIdsMask();
        gnssPosDiag.svUsedInPosition.navicSvUsedIdsMask = svUsed.getNavicSvUsedIdsMask();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_NAV_SOLUTION_MASK_BIT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_NAV_SOLUTION_MASK;
        unsigned int navSolMask = _locationReport.getNavSolutionMask();

        gpsLocExt.navSolutionMask = 0;

        gnssPosDiag.gnssInfoFlags |= LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT;
        gnssPosDiag.navSolutionMask = 0;

        if (navSolMask & LocationTypes::LocationReportNavSolutionMaskT::LRNSMT_SBAS_CORR_IONO) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_SBAS_CORRECTION_IONO;
            gnssPosDiag.navSolutionMask |= LOC_IDL_CLIENT_DIAG_LOCATION_SBAS_CORRECTION_IONO_BIT;
        }
        if (navSolMask & LocationTypes::LocationReportNavSolutionMaskT::LRNSMT_SBAS_CORR_FAST) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_SBAS_CORRECTION_FAST;
            gnssPosDiag.navSolutionMask |= LOC_IDL_CLIENT_DIAG_LOCATION_SBAS_CORRECTION_FAST_BIT;
        }
        if (navSolMask & LocationTypes::LocationReportNavSolutionMaskT::LRNSMT_SBAS_CORR_LONG) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_SBAS_CORRECTION_LONG;
            gnssPosDiag.navSolutionMask |= LOC_IDL_CLIENT_DIAG_LOCATION_SBAS_CORRECTION_LONG_BIT;
        }
        if (navSolMask & LocationTypes::LocationReportNavSolutionMaskT::LRNSMT_SBAS_INTEGRITY) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_SBAS_INTEGRITY;
            gnssPosDiag.navSolutionMask |= LOC_IDL_CLIENT_DIAG_LOCATION_SBAS_INTEGRITY_BIT;
        }
        if (navSolMask & LocationTypes::LocationReportNavSolutionMaskT::LRNSMT_NAV_CORR_DGNSS) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_DGNSS_CORRECTION;
            gnssPosDiag.navSolutionMask |= LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_DGNSS_BIT;
        }
        if (navSolMask & LocationTypes::LocationReportNavSolutionMaskT::LRNSMT_NAV_CORR_RTK) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_RTK_CORRECTION;
            gnssPosDiag.navSolutionMask |= LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_RTK_BIT;
        }
        if (navSolMask & LocationTypes::LocationReportNavSolutionMaskT::LRNSMT_NAV_CORR_PPP) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_PPP_CORRECTION;
            gnssPosDiag.navSolutionMask |= LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_PPP_BIT;
        }
        if (navSolMask & LocationTypes::LocationReportNavSolutionMaskT::LRNSMT_NAV_CORR_RTK_FIX) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_RTK_FIXED_CORRECTION;
            gnssPosDiag.navSolutionMask |=
                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_RTK_FIXED_BIT;
        }
        if (navSolMask &
            LocationTypes::LocationReportNavSolutionMaskT::LRNSMT_NAV_CORR_ONLY_SBAS_CORR_SV_USED) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_ONLY_SBAS_CORRECTED_SV_USED;
            gnssPosDiag.navSolutionMask |=
                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_ONLY_SBAS_CORRECTED_SV_USED_BIT;
        }
    }
    if (lFlags &
            LocationTypes::LocationReportFlagMaskT::LRFMT_GNSS_LOCATION_INFO_POS_TECH_MASK) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK;
        fillPosTechMask(_locationReport.getPosTechMask(), gpsLocExt.tech_mask);

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_POS_TECH_MASK_BIT;
        gnssPosDiag.posTechMask = (locIdlClientDiagPosTechMask)_locationReport.getPosTechMask();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_POS_DYNAMICS_DATA) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_POS_DYNAMICS_DATA;

        const LocationTypes::LocationReportPositionDynamicsT &posDynamics =
                                            _locationReport.getBodyFrameData();

        gpsLocExt.bodyFrameData.bodyFrameDataMask = 0;
        uint32_t bodyFrameDataMask = posDynamics.getBodyFrameDataMask();

        gnssPosDiag.gnssInfoFlags |= LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT;
        gnssPosDiag.bodyFrameData.bodyFrameDataMask = 0;

        if (bodyFrameDataMask &
            LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_LONG_ACCEL) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
            gpsLocExt.bodyFrameData.longAccel = posDynamics.getLongAccel();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
            gnssPosDiag.bodyFrameData.longAccel = posDynamics.getLongAccel();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_LAT_ACCEL) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
            gpsLocExt.bodyFrameData.latAccel = posDynamics.getLatAccel();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
            gnssPosDiag.bodyFrameData.latAccel = posDynamics.getLatAccel();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_VERT_ACCEL) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
            gpsLocExt.bodyFrameData.vertAccel = posDynamics.getVertAccel();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
            gnssPosDiag.bodyFrameData.vertAccel = posDynamics.getVertAccel();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_YAW_RATE) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
            gpsLocExt.bodyFrameData.yawRate = posDynamics.getYawRate();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
            gnssPosDiag.bodyFrameData.yawRate = posDynamics.getYawRate();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_PITCH) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_BIT;
            gpsLocExt.bodyFrameData.pitch = posDynamics.getPitch();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_BIT;
            gnssPosDiag.bodyFrameData.pitch = posDynamics.getPitch();
        }
        if (bodyFrameDataMask &
               LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_LONG_ACCEL_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT;
            gpsLocExt.bodyFrameData.longAccelUnc = posDynamics.getLongAccelUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT;
            gnssPosDiag.bodyFrameData.longAccelUnc = posDynamics.getLongAccelUnc();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_LAT_ACCEL_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |=
                                        LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT;
            gpsLocExt.bodyFrameData.latAccelUnc = posDynamics.getLatAccelUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT;
            gnssPosDiag.bodyFrameData.latAccelUnc = posDynamics.getLatAccelUnc();
        }
        if (bodyFrameDataMask &
                 LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_VERT_ACCEL_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT;
            gpsLocExt.bodyFrameData.vertAccelUnc = posDynamics.getVertAccelUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                            LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT;
            gnssPosDiag.bodyFrameData.vertAccelUnc = posDynamics.getVertAccelUnc();
        }
        if (bodyFrameDataMask &
                  LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_YAW_RATE_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT;
            gpsLocExt.bodyFrameData.yawRateUnc = posDynamics.getYawRateUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                            LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT;
            gnssPosDiag.bodyFrameData.yawRateUnc = posDynamics.getYawRateUnc();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_PITCH_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT;
            gpsLocExt.bodyFrameData.pitchUnc = posDynamics.getPitchUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT;
            gnssPosDiag.bodyFrameData.pitchUnc = posDynamics.getPitchUnc();
        }

        gpsLocExt.bodyFrameDataExt.bodyFrameDataMask = 0;

        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_PITCH_RATE) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT;
            gpsLocExt.bodyFrameDataExt.pitchRate = posDynamics.getPitchRate();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT;
            gnssPosDiag.bodyFrameData.pitchRate = posDynamics.getPitchRate();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_PITCH_RATE_UNC) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |=
                                                    LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT;
            gpsLocExt.bodyFrameDataExt.pitchRateUnc = posDynamics.getPitchRateUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                            LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT;
            gnssPosDiag.bodyFrameData.pitchRateUnc = posDynamics.getPitchRateUnc();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_ROLL) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_BIT;
            gpsLocExt.bodyFrameDataExt.roll = posDynamics.getRoll();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                            LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_BIT;
            gnssPosDiag.bodyFrameData.roll = posDynamics.getRoll();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_ROLL_UNC) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT;
            gpsLocExt.bodyFrameDataExt.rollUnc = posDynamics.getRollUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT;
            gnssPosDiag.bodyFrameData.rollUnc = posDynamics.getRollUnc();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_ROLL_RATE) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT;
            gpsLocExt.bodyFrameDataExt.rollRate = posDynamics.getRollRate();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                        LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT;
            gnssPosDiag.bodyFrameData.rollRate = posDynamics.getRollRate();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_ROLL_RATE_UNC) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT;
            gpsLocExt.bodyFrameDataExt.rollRateUnc = posDynamics.getRollRateUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                        LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT;
            gnssPosDiag.bodyFrameData.rollRateUnc = posDynamics.getRollRateUnc();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_YAW) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_BIT;
            gpsLocExt.bodyFrameDataExt.yaw = posDynamics.getYaw();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_BIT;
            gnssPosDiag.bodyFrameData.yaw = posDynamics.getYaw();
        }
        if (bodyFrameDataMask &
                LocationTypes::LocationReportPosDataMaskT::LRPDMT_NAV_DATA_YAW_UNC) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_UNC_BIT;
            gpsLocExt.bodyFrameDataExt.yawUnc = posDynamics.getYawUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_UNC_BIT;
            gnssPosDiag.bodyFrameData.yawUnc = posDynamics.getYawUnc();
        }
    }

    const LocationTypes::GnssSystemTimeT &gnssTime = _locationReport.getGnssSystemTime();
    uint32_t timeSrc = parsegnssSystemTime(gnssTime.getGnssSystemTimeSrc());
    gpsLocExt.gnssSystemTime.gnssSystemTimeSrc = (Gnss_LocSvSystemEnumType)timeSrc;
    const LocationTypes::SystemTimeStructT &time = gnssTime.getSystemTime();

    gnssPosDiag.gnssSystemTime.gnssSystemTimeSrc =
                        (locIdlClientDiagGnss_LocSvSystemEnumType)timeSrc;
    if (gnssTime.getGnssSystemTimeSrc() != LocationTypes::GnssLocSvSystemTypeT::GLSSTT_GLO) {
           const LocationTypes::GnssSystemTimeStructTypeT &systemTime =
                                time.getGnssSystemTime();
          uint32_t validityMask = systemTime.getValidityMask();
          gpsLocExt.gnssSystemTime.u.gpsSystemTime.validityMask = validityMask;
          gnssPosDiag.gnssSystemTime.u.gpsSystemTime.validityMask = validityMask;
          if (validityMask & LocationTypes::GnssSystemTimeStructTypeFlagsT::GSTSTFT_WEEK_VALID) {
              gpsLocExt.gnssSystemTime.u.gpsSystemTime.systemWeek = systemTime.getSystemWeek();
              gnssPosDiag.gnssSystemTime.u.gpsSystemTime.systemWeek = systemTime.getSystemWeek();
          }
          if (validityMask &
                LocationTypes::GnssSystemTimeStructTypeFlagsT::GSTSTFT_WEEK_MS_VALID) {
              gpsLocExt.gnssSystemTime.u.gpsSystemTime.systemMsec = systemTime.getSystemMsec();
              gnssPosDiag.gnssSystemTime.u.gpsSystemTime.systemMsec = systemTime.getSystemMsec();
          }
          if (validityMask &
                LocationTypes::GnssSystemTimeStructTypeFlagsT::GSTSTFT_CLK_TIME_BIAS_VALID) {
              gpsLocExt.gnssSystemTime.u.gpsSystemTime.systemClkTimeBias =
                                                        systemTime.getSystemClkTimeBias();
              gnssPosDiag.gnssSystemTime.u.gpsSystemTime.systemClkTimeBias =
                                                        systemTime.getSystemClkTimeBias();
          }
          if (validityMask &
                LocationTypes::GnssSystemTimeStructTypeFlagsT::GSTSTFT_CLK_TIME_BIAS_UNC_VALID) {
              gpsLocExt.gnssSystemTime.u.gpsSystemTime.systemClkTimeUncMs =
                                                        systemTime.getSystemClkTimeUncMs();
              gnssPosDiag.gnssSystemTime.u.gpsSystemTime.systemClkTimeUncMs =
                                                        systemTime.getSystemClkTimeUncMs();
          }
          if (validityMask &
                LocationTypes::GnssSystemTimeStructTypeFlagsT::GSTSTFT_REF_FCOUNT_VALID) {
              gpsLocExt.gnssSystemTime.u.gpsSystemTime.refFCount = systemTime.getRefFcount();
              gnssPosDiag.gnssSystemTime.u.gpsSystemTime.refFCount = systemTime.getRefFcount();
          }
          if (validityMask &
                LocationTypes::GnssSystemTimeStructTypeFlagsT::GSTSTFT_NUM_CLOCK_RESETS_VALID) {
              gpsLocExt.gnssSystemTime.u.gpsSystemTime.numClockResets =
                                                    systemTime.getNumClockResets();
              gnssPosDiag.gnssSystemTime.u.gpsSystemTime.numClockResets =
                                                systemTime.getNumClockResets();
          }

          if (timeSrc == LocationTypes::GnssLocSvSystemTypeT::GLSSTT_GPS) {
                gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_GPS_TIME;
                gpsLocExt.gpsTime.gpsWeek = systemTime.getSystemWeek();
                gpsLocExt.gpsTime.gpsTimeOfWeekMs =  systemTime.getSystemMsec();
          }
    } else if (gnssTime.getGnssSystemTimeSrc() ==
                LocationTypes::GnssLocSvSystemTypeT::GLSSTT_GLO) {
          const LocationTypes::GnssGloTimeStructTypeT &gloTime =
                            time.getGloSystemTime();
          uint32_t validityMask = gloTime.getValidityMask();
          gpsLocExt.gnssSystemTime.u.gloSystemTime.validityMask = validityMask;
          gnssPosDiag.gnssSystemTime.u.gloSystemTime.validityMask = validityMask;

          if (validityMask &
                LocationTypes::GnssGloTimeStructTypeFlagsT::GGTSTFT_DAYS_VALID) {
              gpsLocExt.gnssSystemTime.u.gloSystemTime.gloDays = gloTime.getGloDays();
              gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloDays = gloTime.getGloDays();
          }
          if (validityMask &
                LocationTypes::GnssGloTimeStructTypeFlagsT::GGTSTFT_MSEC_VALID) {
              gpsLocExt.gnssSystemTime.u.gloSystemTime.gloMsec = gloTime.getGloMsec();
              gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloMsec = gloTime.getGloMsec();
          }
          if (validityMask &
                LocationTypes::GnssGloTimeStructTypeFlagsT::GGTSTFT_CLK_TIME_BIAS_VALID) {
              gpsLocExt.gnssSystemTime.u.gloSystemTime.gloClkTimeBias =
                                                        gloTime.getGloClkTimeBias();
              gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloClkTimeBias =
                                                        gloTime.getGloClkTimeBias();
          }
          if (validityMask &
                LocationTypes::GnssGloTimeStructTypeFlagsT::GGTSTFT_CLK_TIME_BIAS_UNC_VALID) {
              gpsLocExt.gnssSystemTime.u.gloSystemTime.gloClkTimeUncMs =
                                                                gloTime.getGloClkTimeUncMs();
              gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloClkTimeUncMs =
                                                                gloTime.getGloClkTimeUncMs();
          }
          if (validityMask &
                LocationTypes::GnssGloTimeStructTypeFlagsT::GGTSTFT_REF_FCOUNT_VALID) {
              gpsLocExt.gnssSystemTime.u.gloSystemTime.refFCount = gloTime.getRefFCount();
              gnssPosDiag.gnssSystemTime.u.gloSystemTime.refFCount = gloTime.getRefFCount();
          }
          if (validityMask &
                LocationTypes::GnssGloTimeStructTypeFlagsT::GGTSTFT_NUM_CLK_RESETS_VALID) {
              gpsLocExt.gnssSystemTime.u.gloSystemTime.numClockResets =
                                                    gloTime.getNumClockResets();
              gnssPosDiag.gnssSystemTime.u.gloSystemTime.numClockResets =
                                                    gloTime.getNumClockResets();
          }
          if (validityMask &
                LocationTypes::GnssGloTimeStructTypeFlagsT::GGTSTFT_FOUR_YEAR_VALID) {
                gpsLocExt.gnssSystemTime.u.gloSystemTime.gloFourYear = gloTime.getGloFourYear();
                gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloFourYear = gloTime.getGloFourYear();
          }
    }

    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_EXT_DOP ) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_EXT_DOP;
        gpsLocExt.extDOP.PDOP = _locationReport.getPdop();
        gpsLocExt.extDOP.HDOP = _locationReport.getHdop();
        gpsLocExt.extDOP.VDOP = _locationReport.getVdop();
        gpsLocExt.extDOP.GDOP = _locationReport.getGdop();
        gpsLocExt.extDOP.TDOP = _locationReport.getTdop();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_NORTH_STD_DEV) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_NORTH_STD_DEV;
        gpsLocExt.northStdDeviation = _locationReport.getNorthStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_STD_DEV_BIT;
        gnssPosDiag.northStdDeviation = _locationReport.getNorthStdDeviation();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_EAST_STD_DEV) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_EAST_STD_DEV;
        gpsLocExt.eastStdDeviation = _locationReport.getEastStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_STD_DEV_BIT;
        gnssPosDiag.eastStdDeviation = _locationReport.getEastStdDeviation();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_NORTH_VEL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_NORTH_VEL;
        gpsLocExt.northVelocity = _locationReport.getNorthVelocity();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_VEL_BIT;
        gnssPosDiag.northVelocity = _locationReport.getNorthVelocity();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_EAST_VEL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_EAST_VEL;
        gpsLocExt.eastVelocity = _locationReport.getEastVelocity();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_VEL_BIT;
        gnssPosDiag.eastVelocity = _locationReport.getEastVelocity();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_UP_VEL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_UP_VEL;
        gpsLocExt.upVelocity = _locationReport.getUpVelocity();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_UP_VEL_BIT;
        gnssPosDiag.upVelocity = _locationReport.getUpVelocity();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_NORTH_VEL_UNC) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_NORTH_VEL_UNC;
        gpsLocExt.northVelocityStdDeviation = _locationReport.getNorthVelocityStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_VEL_UNC_BIT;
        gnssPosDiag.northVelocityStdDeviation = _locationReport.getNorthVelocityStdDeviation();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_EAST_VEL_UNC) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_EAST_VEL_UNC;
        gpsLocExt.eastVelocityStdDeviation = _locationReport.getEastVelocityStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_VEL_UNC_BIT;
        gnssPosDiag.eastVelocityStdDeviation = _locationReport.getEastVelocityStdDeviation();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_UP_VEL_UNC) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_UP_VEL_UNC;
        gpsLocExt.upVelocityStdDeviation = _locationReport.getUpVelocityStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_UP_VEL_UNC_BIT;
        gnssPosDiag.upVelocityStdDeviation = _locationReport.getUpVelocityStdDeviation();
    }

    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_LEAP_SECONDS) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_LEAP_SECONDS;
        gpsLocExt.leapSeconds = _locationReport.getLeapSeconds();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_LEAP_SECONDS_BIT;
        gnssPosDiag.leapSeconds = _locationReport.getLeapSeconds();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_GNSS_SV_USED_DATA) {
        const vector<LocationTypes::GnssMeasUsageInfoT> &meas =
                                                _locationReport.getMeasUsageInfo();
        gpsLocExt.numOfMeasReceived = meas.size();

        gnssPosDiag.gnssInfoFlags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT;
        gnssPosDiag.numOfMeasReceived = meas.size();

        for (int idx = 0; idx < meas.size() && idx < GNSS_SV_MAX; idx++) {
            uint32_t constellation = parseConstellationType(meas[idx].getGnssConstellation());
            uint32_t sigType = parseSignalType(meas[idx].getGnssSignalType());
            gpsLocExt.measUsageInfo[idx].gnssConstellation =
                                            (Gnss_LocSvSystemEnumType)constellation;
            gpsLocExt.measUsageInfo[idx].gnssSignalType = sigType;
            gpsLocExt.measUsageInfo[idx].gnssSvId = meas[idx].getGnssSvId();

            gnssPosDiag.measUsageInfo[idx].gnssConstellation =
                                (locIdlClientDiagGnss_LocSvSystemEnumType)constellation;
            gnssPosDiag.measUsageInfo[idx].gnssSignalType = sigType;
            gnssPosDiag.measUsageInfo[idx].gnssSvId = meas[idx].getGnssSvId();
        }
    }

    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_CALIB_CONFIDENCE_PERCENT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_CALIBRATION_CONFIDENCE;
        gpsLocExt.calibrationConfidence = _locationReport.getCalibrationConfidencePercent();

        gnssPosDiag.gnssInfoFlags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CALIBRATION_CONFIDENCE_PERCENT_BIT;
        gnssPosDiag.calibrationConfidencePercent =
                    _locationReport.getCalibrationConfidencePercent();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_CALIB_STATUS) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_CALIBRATION_STATUS;
        unsigned int calStatus = _locationReport.getCalibrationStatus();
        gpsLocExt.calibrationStatus = 0;

        gnssPosDiag.gnssInfoFlags |= LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CALIBRATION_STATUS_BIT;
        gnssPosDiag.calibrationStatus = (locIdlClientDiagDrCalibrationStatusMask)calStatus;

        if (calStatus & LocationTypes::DrCalibrationStatusMaskT::DCSMT_ROLL_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_ROLL_CALIBRATION_NEEDED;
        }
        if (calStatus & LocationTypes::DrCalibrationStatusMaskT::DCSMT_PITCH_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_PITCH_CALIBRATION_NEEDED;
        }
        if (calStatus & LocationTypes::DrCalibrationStatusMaskT::DCSMT_YAW_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_YAW_CALIBRATION_NEEDED;
        }
        if (calStatus & LocationTypes::DrCalibrationStatusMaskT::DCSMT_ODO_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_ODO_CALIBRATION_NEEDED;
        }
        if (calStatus & LocationTypes::DrCalibrationStatusMaskT::DCSMT_GYRO_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_GYRO_CALIBRATION_NEEDED;
        }
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_OUTPUT_ENG_TYPE) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_OUTPUT_ENG_TYPE;
        uint32_t engTypeIdl = _locationReport.getLocOutputEngType();
        LocOutputEngineType engType = (LocOutputEngineType)0;
        if (engTypeIdl == LocationTypes::LocOutputEngineTypeT::LOETT_FUSED) {
            engType = LOC_OUTPUT_ENGINE_FUSED;
        } else if (engTypeIdl == LocationTypes::LocOutputEngineTypeT::LOETT_SPE) {
            engType = LOC_OUTPUT_ENGINE_SPE;
        } else if (engTypeIdl == LocationTypes::LocOutputEngineTypeT::LOETT_PPE) {
            engType = LOC_OUTPUT_ENGINE_PPE;
        } else if (engTypeIdl == LocationTypes::LocOutputEngineTypeT::LOETT_VPE) {
            engType = LOC_OUTPUT_ENGINE_VPE;
        }
        gpsLocExt.locOutputEngType = engType;

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_OUTPUT_ENG_TYPE_BIT;
        gnssPosDiag.locOutputEngType = (locIdlClientDiagLocOutputEngineType)engType;
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_OUTPUT_ENG_MASK) {
        uint32_t engMask = parseEngMask(_locationReport.getLocOutputEngMask());
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_OUTPUT_ENG_MASK;
        gpsLocExt.locOutputEngMask = engMask;

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_OUTPUT_ENG_MASK_BIT;
        gnssPosDiag.locOutputEngMask =
                    (locIdlClientDiagPositioningEngineMask)engMask;
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_CONFORMITY_INDEX) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_CONFORMITY_INDEX;
        gpsLocExt.conformityIndex = _locationReport.getConformityIndex();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CONFORMITY_INDEX_BIT;
        gnssPosDiag.conformityIndex = _locationReport.getConformityIndex();
    }

    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_LLA_VRP_BASED) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_LLA_VRP_BASED;
        const LocationTypes::LlaInfoT &lla = _locationReport.getLlaVrpBased();
        gpsLocExt.llaVRPBased.latitude =  lla.getLatitude();
        gpsLocExt.llaVRPBased.longitude =  lla.getLongitude();
        gpsLocExt.llaVRPBased.altitude =  lla.getAltitude();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_LLA_VRP_BASED_BIT;
        gnssPosDiag.llaVRPBased.latitude =  lla.getLatitude();
        gnssPosDiag.llaVRPBased.longitude =  lla.getLongitude();
        gnssPosDiag.llaVRPBased.altitude =  lla.getAltitude();
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_ENU_VEL_VRP_BASED) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_ENU_VELOCITY_LLA_VRP_BASED;
        const std::vector<float> &emu = _locationReport.getEnuVelocityVrpBased();

        gnssPosDiag.gnssInfoFlags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_ENU_VELOCITY_VRP_BASED_BIT;
        for (int k = 0; k < emu.size(); k++) { //emu.size() should be 3
            gpsLocExt.enuVelocityVRPBased[k] = emu[k];
            gnssPosDiag.enuVelocityVRPBased[k] = emu[k];
        }
    }
    if (lFlags & LocationTypes::LocationReportFlagMaskT::LRFMT_DR_SOL_STATUS_MASK) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_DR_SOLUTION_STATUS_MASK;
        uint32_t mask = parseDrSolutionStatusMask(_locationReport.getDrSolutionStatusMask());
        gpsLocExt.drSolutionStatusMask = mask;

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_DR_SOLUTION_STATUS_MASK_BIT;
        gnssPosDiag.drSolutionStatusMask = (locIdlClientDiagDrSolutionStatusMask)mask;
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_ALTITUDE_ASSUMED) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_ALTITUDE_ASSUMED;
        gpsLocExt.altitudeAssumed = _locationReport.getAltitudeAssumed();

        gnssPosDiag.gnssInfoFlags |= LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_ALTITUDE_ASSUMED_BIT;
        gnssPosDiag.altitudeAssumed = _locationReport.getAltitudeAssumed();
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_SESSION_STATUS) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_SESSION_STATUS_BIT;

        uint32_t st = _locationReport.getSessionStatus();
        locIdlClientDiagLocSessionStatus status;
        if (st == LocationTypes::LocSessionStatusT::LSS_SUCCESS) {
            status = LOC_IDL_CLIENT_DIAG_LOC_SESS_SUCCESS;
        } else if (st == LocationTypes::LocSessionStatusT::LSS_INTERMEDIATE) {
            status = LOC_IDL_CLIENT_DIAG_LOC_SESS_INTERMEDIATE;
        } else {
            status = LOC_IDL_CLIENT_DIAG_LOC_SESS_FAILURE;
        }
        gnssPosDiag.sessionStatus = status;
    }
    if (lFlags &
            LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_INTEGRITY_RISK_USED) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_INTEGRITY_RISK_USED;
        gpsLocExt.integrityRiskUsed = _locationReport.getIntegrityRiskUsed();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_INTEGRITY_RISK_USED_BIT;
        gnssPosDiag.integrityRiskUsed = _locationReport.getIntegrityRiskUsed();
    }
    if (lFlags &
            LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_PROTECT_ALONG_TRACK) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_PROTECT_ALONG_TRACK;
        gpsLocExt.protectAlongTrack = _locationReport.getProtectAlongTrack();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_ALONG_TRACK_BIT;
        gnssPosDiag.protectAlongTrack = _locationReport.getProtectAlongTrack();
    }
    if (lFlags &
        LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_PROTECT_CROSS_TRACK) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_PROTECT_CROSS_TRACK;
        gpsLocExt.protectCrossTrack = _locationReport.getProtectCrossTrack();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_CROSS_TRACK_BIT;
        gnssPosDiag.protectCrossTrack = _locationReport.getProtectCrossTrack();
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_PROTECT_VERTICAL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_PROTECT_VERTICAL;
        gpsLocExt.protectVertical = _locationReport.getProtectVertical();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_VERTICAL_BIT;
        gnssPosDiag.protectVertical = _locationReport.getProtectVertical();
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_DGNSS_STATION_ID) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_DGNSS_STATION_ID;
        const std::vector<uint32_t> &dgnss = _locationReport.getDgnssStationId();
        gpsLocExt.numOfDgnssStationId = dgnss.size();

        gnssPosDiag.gnssInfoFlags |= LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_DGNSS_STATION_ID_BIT;
        gnssPosDiag.numOfDgnssStationId = dgnss.size();

        for (int l = 0; l < dgnss.size(); l++) {
               gpsLocExt.dgnssStationId[l] = dgnss[l];
               gnssPosDiag.dgnssStationId[l] = dgnss[l];
        }
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_GPTP_TIME_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_GPTP_TIME_BIT;
        gnssPosDiag.elapsedgPTPTime = _locationReport.getElapsedgPtpTime();
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_GPTP_TIME_UNC_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_GPTP_TIME_UNC_BIT;
        gnssPosDiag.elapsedgPTPTimeUnc = _locationReport.getElapsedgPtpTimeUnc();
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_BASE_LINE_LENGTH_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_BASE_LINE_LENGTH_BIT;
        gnssPosDiag.baseLineLength = _locationReport.getBaseLineLength();
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_AGE_OF_CORRECTION_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_AGE_OF_CORRECTION_BIT;
        gnssPosDiag.ageMsecOfCorrections = _locationReport.getAgeMsecOfCorrections();
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_REPORT_INTERVAL_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CURR_REPORT_RATE_BIT;
        gnssPosDiag.currReportingRate = _locationReport.getPosReportingInterval();
    }
    if (lFlags & LocationTypes::LocationReportExtendedFlagMaskT::LREFMT_LEAP_SECONDS_UNC_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_LREFM_LEAP_SECONDS_UNC_BIT;
        gnssPosDiag.leapSecondsUnc = _locationReport.getLeapSecondsUnc();
    }
}

void LocIdlClientDevice::getMeasurementSet(const LocationTypes::GnssMeasurementsT& gnssMeasurement,
                                    GnssMeasurements &svMeasurementSet,
                                    locIdlClientDiagGnssMeasPacket &gnssMeasDiag)
{
    /* Each index represents the corresponding SV type */
    GnssMeasurementsAgc measAgc[GNSS_SV_TYPE_NAVIC + 1];
    memset(&measAgc, 0x00, sizeof(measAgc));
    memset(&svMeasurementSet, 0x00, sizeof(GnssMeasurements));
    svMeasurementSet.size = sizeof(GnssMeasurements);
    memset(&gnssMeasDiag, 0x00, sizeof(locIdlClientDiagGnssMeasPacket));

    svMeasurementSet.gnssSvMeasurementSet.size = sizeof(GnssSvMeasurementSet);
    svMeasurementSet.gnssSvMeasurementSet.svMeasSetHeader.size = sizeof(GnssSvMeasurementHeader);
    svMeasurementSet.gnssSvMeasurementSet.svMeasSetHeader.flags = 0;
    svMeasurementSet.gnssMeasNotification.size = sizeof(GnssMeasurementsNotification);
    svMeasurementSet.gnssMeasNotification.isNhz = gnssMeasurement.getIsNHz();

    const vector<LocationTypes::GnssMeasurementsDataT > &measData =
                                        gnssMeasurement.getMeasurements();
    svMeasurementSet.gnssMeasNotification.count = measData.size(); //WC: GNSS_MEASUREMENTS_MAX

    gnssMeasDiag.count = measData.size();
    gnssMeasDiag.isNhz = gnssMeasurement.getIsNHz();
    gnssMeasDiag.reportingLatency = gnssMeasurement.getReportingLatency();

    for (uint16_t idx = 0; idx < measData.size() && idx < GNSS_MEASUREMENTS_MAX; idx++) {
        GnssMeasurementsAgc tempAgc = {0.0, (GnssSvType)0, 0.0};
        svMeasurementSet.gnssMeasNotification.measurements[idx].size = sizeof(GnssMeasurementsData);

        unsigned int flags = measData[idx].getFlags();
        svMeasurementSet.gnssMeasNotification.measurements[idx].flags = 0;
        gnssMeasDiag.measurements[idx].flags = 0;

        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_SV_ID_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                            GNSS_MEASUREMENTS_DATA_SV_ID_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].svId = measData[idx].getSvId();

            gnssMeasDiag.measurements[idx].flags |=
                                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_ID_BIT;
            gnssMeasDiag.measurements[idx].svId = measData[idx].getSvId();
        }
        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_SV_TYPE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                            GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT;
            uint32_t stype = parseSvType(measData[idx].getSvType());
            svMeasurementSet.gnssMeasNotification.measurements[idx].svType = (GnssSvType)stype;
            tempAgc.svType = (GnssSvType)stype;
            gnssMeasDiag.measurements[idx].flags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT;
            gnssMeasDiag.measurements[idx].svType = (locIdlClientDiagGnssSvType)stype;
        }
        svMeasurementSet.gnssMeasNotification.measurements[idx].timeOffsetNs =
                                                            measData[idx].getTimeOffsetNs();
        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_STATE_BIT) {
            uint32_t stateMask = parseMeasStateMask(measData[idx].getStateMask());
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                            GNSS_MEASUREMENTS_DATA_STATE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].stateMask = stateMask;

            gnssMeasDiag.measurements[idx].flags |=
                                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_STATE_BIT;
            gnssMeasDiag.measurements[idx].stateMask = measData[idx].getStateMask();
        }
        if (flags &
            LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_RECEIVED_SV_TIME_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].receivedSvTimeNs =
                                                            measData[idx].getReceivedSvTimeNs();
            svMeasurementSet.gnssMeasNotification.measurements[idx].receivedSvTimeSubNs =
                                                            measData[idx].getReceivedSvTimeSubNs();

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_BIT;
            gnssMeasDiag.measurements[idx].receivedSvTimeNs = measData[idx].getReceivedSvTimeNs();
            gnssMeasDiag.measurements[idx].receivedSvTimeSubNs =
                                                        measData[idx].getReceivedSvTimeSubNs();
        }
        if (flags &
            LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_RECVD_SV_TIME_UNC_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                        GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_UNCERTAINTY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].receivedSvTimeUncertaintyNs =
                                            measData[idx].getReceivedSvTimeUncertaintyNs();

            gnssMeasDiag.measurements[idx].flags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_UNCERTAINTY_BIT;
            gnssMeasDiag.measurements[idx].receivedSvTimeUncertaintyNs =
                    measData[idx].getReceivedSvTimeUncertaintyNs();
        }
        if (flags &
                LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_CARRIER_TO_NOISE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                    GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierToNoiseDbHz =
                                                        measData[idx].getCarrierToNoiseDbHz();

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT;
            gnssMeasDiag.measurements[idx].carrierToNoiseDbHz =
                                measData[idx].getCarrierToNoiseDbHz();
        }
        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_PR_RATE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].pseudorangeRateMps =
                                                        measData[idx].getPseudorangeRateMps();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT;
            gnssMeasDiag.measurements[idx].pseudorangeRateMps =
                            measData[idx].getPseudorangeRateMps();
        }
        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_PR_RATE_UNC_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                        GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].pseudorangeRateUncertaintyMps =
                                                measData[idx].getPseudorangeRateUncertaintyMps();

            gnssMeasDiag.measurements[idx].flags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT;
            gnssMeasDiag.measurements[idx].pseudorangeRateUncertaintyMps =
                    measData[idx].getPseudorangeRateUncertaintyMps();
        }
        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_ADR_STATE_BIT) {
            uint32_t adrStateMask = parseAdrStateMask(measData[idx].getAdrStateMask());
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                            GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].adrStateMask = adrStateMask;

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT;
            gnssMeasDiag.measurements[idx].adrStateMask = adrStateMask;
        }
        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_ADR_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                        GNSS_MEASUREMENTS_DATA_ADR_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].adrMeters =
                                                        measData[idx].getAdrMeters();

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_BIT;
            gnssMeasDiag.measurements[idx].adrMeters = measData[idx].getAdrMeters();
        }
        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_ADR_UNC_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                    GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].adrUncertaintyMeters =
                                                    measData[idx].getAdrUncertaintyMeters();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT;
            gnssMeasDiag.measurements[idx].adrUncertaintyMeters =
                            measData[idx].getAdrUncertaintyMeters();
        }
        if (flags &
                LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_CARRIER_FREQ_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierFrequencyHz =
                                                measData[idx].getCarrierFrequencyHz();
            tempAgc.carrierFrequencyHz = measData[idx].getCarrierFrequencyHz();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT;
            gnssMeasDiag.measurements[idx].carrierFrequencyHz =
                            measData[idx].getCarrierFrequencyHz();
        }
        if (flags &
                LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_CARRIER_CYCLES_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierCycles =
                                                measData[idx].getCarrierCycles();

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT;
            gnssMeasDiag.measurements[idx].carrierCycles = measData[idx].getCarrierCycles();
        }
        if (flags &
                LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_CARRIER_PHASE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                        GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierPhase =
                                                        measData[idx].getCarrierPhase();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT;
            gnssMeasDiag.measurements[idx].carrierPhase = measData[idx].getCarrierPhase();
        }
        if (flags &
              LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_CARRIER_PHASE_UNC_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                            GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierPhaseUncertainty =
                                            measData[idx].getCarrierPhaseUncertainty();

            gnssMeasDiag.measurements[idx].flags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT;
            gnssMeasDiag.measurements[idx].carrierPhaseUncertainty =
                        measData[idx].getCarrierPhaseUncertainty();
        }
        if (flags &
                LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_MULTIPATH_IND_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                    GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT;
            uint32_t mPathIndicator = parseMultiPathIndicator(measData[idx].getMultipathIndicator());
            svMeasurementSet.gnssMeasNotification.measurements[idx].multipathIndicator =
                                                (GnssMeasurementsMultipathIndicator)mPathIndicator;

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT;
            gnssMeasDiag.measurements[idx].multipathIndicator =
                                (locIdlClientDiagGnssMeasurementsMultipathIndicator)mPathIndicator;
        }
        if (flags &
              LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_SIGNAL_TO_NOISE_RATIO) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                        GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].signalToNoiseRatioDb =
                                        measData[idx].getSignalToNoiseRatioDb();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT;
            gnssMeasDiag.measurements[idx].signalToNoiseRatioDb =
                            measData[idx].getSignalToNoiseRatioDb();
        }
        if (flags &
            LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_AUTO_GAIN_CTRL_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                    GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].agcLevelDb =
                                                    measData[idx].getAgcLevelDb();
            tempAgc.agcLevelDb = measData[idx].getAgcLevelDb();

            gnssMeasDiag.measurements[idx].flags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT;
            gnssMeasDiag.measurements[idx].agcLevelDb = measData[idx].getAgcLevelDb();
        }
        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_FULL_ISB_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                            GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT;
        }
        if (flags & LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_FULL_ISB_UNC_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                        GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].fullInterSignalBiasNs =
                                        measData[idx].getFullInterSignalBiasNs();
            svMeasurementSet.gnssMeasNotification.measurements[idx].fullInterSignalBiasUncertaintyNs =
                                        measData[idx].getFullInterSignalBiasUncertaintyNs();

            gnssMeasDiag.measurements[idx].flags |=
                                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_GNSS_SIGNAL_TYPE_BIT;
            gnssMeasDiag.measurements[idx].fullInterSignalBiasNs =
                                    measData[idx].getFullInterSignalBiasNs();
            gnssMeasDiag.measurements[idx].fullInterSignalBiasUncertaintyNs =
                                    measData[idx].getFullInterSignalBiasUncertaintyNs();
        }
        if (flags &
                LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_CYCLE_SLIP_COUNT_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                        GNSS_MEASUREMENTS_DATA_CYCLE_SLIP_COUNT_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].cycleSlipCount =
                                        measData[idx].getCycleSlipCount();

            gnssMeasDiag.measurements[idx].flags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_BASEBAND_CARRIER_TO_NOISE_BIT;
            gnssMeasDiag.measurements[idx].cycleSlipCount = measData[idx].getCycleSlipCount();
        }
        if (flags &
            LocationTypes::GnssMeasurementsDataFlagsMaskT::GMDFMT_GNSS_SIGNAL_TYPE_BIT) {
            uint32_t sigType = parseSignalType(measData[idx].getGnssSignalType());
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                    GNSS_MEASUREMENTS_DATA_GNSS_SIGNAL_TYPE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].gnssSignalType = sigType;

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT;
            gnssMeasDiag.measurements[idx].gnssSignalType = sigType;
        }
        if (flags &
            LocationTypes::GnssMeasurementsDataFlagsMaskT::
            GMDFMT_BASEBAND_CARRIER_TO_NOISE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                        GNSS_MEASUREMENTS_DATA_BASEBAND_CARRIER_TO_NOISE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].basebandCarrierToNoiseDbHz =
                                        measData[idx].getBasebandCarrierToNoiseDbHz();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT;
            gnssMeasDiag.measurements[idx].basebandCarrierToNoiseDbHz =
                                    measData[idx].getBasebandCarrierToNoiseDbHz();
        }

        if (flags &
            LocationTypes::GnssMeasurementsDataFlagsMaskT::
                GMDFMT_CODE_TYPE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].codeType =
                                        parseMeasCodeType(measData[idx].getMeasCodeType());
        }

        if (flags &
            LocationTypes::GnssMeasurementsDataFlagsMaskT::
                GMDFMT_OTHER_MEAS_CODE_TYPE_BIT) {
            uint32_t sizeInBuff =
                sizeof(svMeasurementSet.gnssMeasNotification.measurements[idx].otherCodeTypeName);
            strlcpy(svMeasurementSet.gnssMeasNotification.measurements[idx].otherCodeTypeName,
                                        measData[idx].getOtherCodeTypeName().c_str(), sizeInBuff);
        }

        if (tempAgc.svType && tempAgc.agcLevelDb && tempAgc.carrierFrequencyHz) {
            if (measAgc[tempAgc.svType].agcLevelDb < tempAgc.agcLevelDb) {
                measAgc[tempAgc.svType].agcLevelDb = tempAgc.agcLevelDb;
                measAgc[tempAgc.svType].carrierFrequencyHz = tempAgc.carrierFrequencyHz;
            }
        }
    }
    svMeasurementSet.gnssMeasNotification.agcCount = 0;
    for (uint32_t i = 1; i < (GNSS_SV_TYPE_NAVIC + 1); i++) {
        if (0 != measAgc[i].agcLevelDb && 0 != measAgc[i].carrierFrequencyHz) {
            uint32_t count = svMeasurementSet.gnssMeasNotification.agcCount;
            svMeasurementSet.gnssMeasNotification.gnssAgc[count].agcLevelDb =
                                                        measAgc[i].agcLevelDb;
            svMeasurementSet.gnssMeasNotification.gnssAgc[count].svType = (GnssSvType)i;
            svMeasurementSet.gnssMeasNotification.gnssAgc[count].carrierFrequencyHz =
                                                        measAgc[i].carrierFrequencyHz;
            svMeasurementSet.gnssMeasNotification.agcCount += 1;
        } else {
            LOC_LOGD("%s]--> Measurement agcLevelDb: %f, carrierFrequencyHz: %f",__func__,
                 measAgc[i].agcLevelDb, measAgc[i].carrierFrequencyHz);
        }
    }

    const LocationTypes::GnssMeasurementsClockT &clk = gnssMeasurement.getClock();
    svMeasurementSet.gnssMeasNotification.clock.size = sizeof(GnssMeasurementsClock);
    unsigned int flag = clk.getFlags();

    svMeasurementSet.gnssMeasNotification.clock.flags = 0;

    if (flag & LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_LEAP_SECOND_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                    GNSS_MEASUREMENTS_CLOCK_FLAGS_LEAP_SECOND_BIT;
        svMeasurementSet.gnssMeasNotification.clock.leapSecond = clk.getLeapSecond();

        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_LEAP_SECOND_BIT;
        gnssMeasDiag.clock.leapSecond = clk.getLeapSecond();
    }
    if (flag & LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_TIME_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                    GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_BIT;
        svMeasurementSet.gnssMeasNotification.clock.timeNs = clk.getTimeNs();

        gnssMeasDiag.clock.flags |= LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_BIT;
        gnssMeasDiag.clock.timeNs = clk.getTimeNs();
    }
    if (flag &
        LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_TIME_UNCERTAINTY_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                            GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_UNCERTAINTY_BIT;
        svMeasurementSet.gnssMeasNotification.clock.timeUncertaintyNs =
                                            clk.getTimeUncertaintyNs();

        gnssMeasDiag.clock.flags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_UNCERTAINTY_BIT;
        gnssMeasDiag.clock.timeUncertaintyNs = clk.getTimeUncertaintyNs();
    }
    if (flag & LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_FULL_BIAS_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                            GNSS_MEASUREMENTS_CLOCK_FLAGS_FULL_BIAS_BIT;
        svMeasurementSet.gnssMeasNotification.clock.fullBiasNs = clk.getFullBiasNs();

        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_FULL_BIAS_BIT;
        gnssMeasDiag.clock.fullBiasNs = clk.getFullBiasNs();
    }
    if (flag & LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_BIAS_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                    GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_BIT;
        svMeasurementSet.gnssMeasNotification.clock.biasNs = clk.getBiasNs();

        gnssMeasDiag.clock.flags |= LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_BIT;
        gnssMeasDiag.clock.biasNs = clk.getBiasNs();
    }
    if (flag &
        LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_BIAS_UNCERTAINTY_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                    GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_UNCERTAINTY_BIT;
        svMeasurementSet.gnssMeasNotification.clock.biasUncertaintyNs =
                                    clk.getBiasUncertaintyNs();

        gnssMeasDiag.clock.flags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_UNCERTAINTY_BIT;
        gnssMeasDiag.clock.biasUncertaintyNs = clk.getBiasUncertaintyNs();
    }
    if (flag & LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_DRIFT_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                    GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_BIT;
        svMeasurementSet.gnssMeasNotification.clock.driftNsps = clk.getDriftNsps();

        gnssMeasDiag.clock.flags |= LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_BIT;
        gnssMeasDiag.clock.driftNsps = clk.getDriftNsps();
    }
    if (flag &
        LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_DRIFT_UNCERTAINTY_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                              GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_UNCERTAINTY_BIT;
        svMeasurementSet.gnssMeasNotification.clock.driftUncertaintyNsps =
                                                clk.getDriftUncertaintyNsps();

        gnssMeasDiag.clock.flags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_UNCERTAINTY_BIT;
        gnssMeasDiag.clock.driftUncertaintyNsps = clk.getDriftUncertaintyNsps();
    }
    if (flag & LocationTypes::GnssMeasurementsClockFlagsMaskT::
                        GMCFMT_HW_CLOCK_DISCONTINUITY_COUNT_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                        GNSS_MEASUREMENTS_CLOCK_FLAGS_HW_CLOCK_DISCONTINUITY_COUNT_BIT;
        svMeasurementSet.gnssMeasNotification.clock.hwClockDiscontinuityCount =
                        clk.getHwClockDiscontinuityCount();

        gnssMeasDiag.clock.flags |=
               LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_HW_CLOCK_DISCONTINUITY_COUNT_BIT;
        gnssMeasDiag.clock.hwClockDiscontinuityCount = clk.getHwClockDiscontinuityCount();
    }
    if (flag &
        LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_ELAPSED_REAL_TIME_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                    GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_BIT;
        svMeasurementSet.gnssMeasNotification.clock.elapsedRealTime = clk.getElapsedRealTime();
        svMeasurementSet.gnssMeasNotification.clock.elapsedRealTimeUnc =
                    DEFAULT_ELAPSED_REAL_TIMEUNC;

        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_BIT;
        gnssMeasDiag.clock.elapsedRealTime = clk.getElapsedRealTime();
    }
    if (flag &
    LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_ELAPSED_REAL_TIME_UNC_BIT) {
        gnssMeasDiag.clock.flags |=
            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_UNC_BIT;
        gnssMeasDiag.clock.elapsedRealTimeUnc = clk.getElapsedRealTimeUnc();
        svMeasurementSet.gnssMeasNotification.clock.elapsedRealTimeUnc =
                clk.getElapsedRealTimeUnc();
        if (0 == svMeasurementSet.gnssMeasNotification.clock.elapsedRealTimeUnc) {
            svMeasurementSet.gnssMeasNotification.clock.elapsedRealTimeUnc
                = DEFAULT_ELAPSED_REAL_TIMEUNC;
        }
    }
    if (flag &
       LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_ELAPSED_GPTP_TIME_BIT) {
        uint64_t boot_time_ns = 0;
        uint64_t  tx_ptp_time_ns = clk.getElapsedgPtpTime();
        if ((nullptr == gPTPReqIf) ||
           (false == gPTPReqIf->gptpGetBootTimeFromPtpTimeIf(&boot_time_ns, tx_ptp_time_ns))) {
           boot_time_ns = tx_ptp_time_ns;
        }

        svMeasurementSet.gnssMeasNotification.clock.flags |=
                    GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_BIT;
        svMeasurementSet.gnssMeasNotification.clock.elapsedRealTime = boot_time_ns;
        svMeasurementSet.gnssMeasNotification.clock.elapsedRealTimeUnc =
                    DEFAULT_ELAPSED_REAL_TIMEUNC;

        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_BIT;
        gnssMeasDiag.clock.elapsedRealTime = boot_time_ns;
        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_GPTP_TIME_BIT;
        gnssMeasDiag.clock.elapsedgPTPTime = clk.getElapsedgPtpTime();
    }
    if (flag &
    LocationTypes::GnssMeasurementsClockFlagsMaskT::GMCFMT_ELAPSED_GPTP_TIME_UNC_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.elapsedRealTimeUnc =
                        clk.getElapsedgPtpTimeUnc();
        if (0 == svMeasurementSet.gnssMeasNotification.clock.elapsedRealTimeUnc) {
            svMeasurementSet.gnssMeasNotification.clock.elapsedRealTimeUnc
                = DEFAULT_ELAPSED_REAL_TIMEUNC;
        }

        gnssMeasDiag.clock.flags |=
            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_UNC_BIT;
        gnssMeasDiag.clock.elapsedRealTimeUnc =    clk.getElapsedgPtpTimeUnc();
        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_GPTP_TIME_UNC_BIT;
        gnssMeasDiag.clock.elapsedgPTPTimeUnc = clk.getElapsedgPtpTimeUnc();
    }

    LOC_LOGD("%s] --> Measurement count %d, elapsedRealtime: %" PRIi64 ", uncertainty: %" PRIi64 "",
             __func__, svMeasurementSet.gnssMeasNotification.count,
             svMeasurementSet.gnssMeasNotification.clock.elapsedRealTime,
             svMeasurementSet.gnssMeasNotification.clock.elapsedRealTimeUnc);
}

void LocIdlClientDevice::getSvRpt(const vector<LocationTypes::GnssSvDataT> &gnssSvf,
                                    GnssSvNotification &svNotify,
                                    locIdlClientDiagGnssSv& gnssSVDiag)
{
    memset(&svNotify, 0x00, sizeof(GnssSvNotification));
    memset(&gnssSVDiag, 0x00, sizeof(locIdlClientDiagGnssSv));
    svNotify.size = sizeof(GnssSvNotification);
    svNotify.count = gnssSvf.size();
    gnssSVDiag.count = gnssSvf.size();
    svNotify.gnssSignalTypeMaskValid = true;

    if (gnssSvf.size() > GNSS_SV_MAX)
        svNotify.count = GNSS_SV_MAX;

    if (gnssSvf.size() > LOC_IDL_CLIENT_DIAG_GNSS_SV_MAX)
        gnssSVDiag.count = LOC_IDL_CLIENT_DIAG_GNSS_SV_MAX;

    for (uint16_t idx = 0; idx < gnssSvf.size(); idx++) {
        uint32_t stype = parseSvType(gnssSvf[idx].getType());
        uint32_t optMask = parseGnssSvOptionsMask(gnssSvf[idx].getGnssSvOptionsMask());
        uint32_t sigMask = parseSignalType(gnssSvf[idx].getGnssSignalType());

        if (idx < GNSS_SV_MAX) {
            svNotify.gnssSvs[idx].size = sizeof(GnssSv);
            svNotify.gnssSvs[idx].svId = gnssSvf[idx].getSvId();
            svNotify.gnssSvs[idx].type = (GnssSvType)stype;
            svNotify.gnssSvs[idx].cN0Dbhz = gnssSvf[idx].getCN0DbHz();
            svNotify.gnssSvs[idx].elevation = gnssSvf[idx].getElevation();
            svNotify.gnssSvs[idx].azimuth = gnssSvf[idx].getAzimuth();
            svNotify.gnssSvs[idx].gnssSvOptionsMask = (GnssSvOptionsMask)optMask;
            svNotify.gnssSvs[idx].carrierFrequencyHz = gnssSvf[idx].getCarrierFrequencyHz();
            svNotify.gnssSvs[idx].gnssSignalTypeMask = (GnssSignalTypeMask)sigMask;
            svNotify.gnssSvs[idx].basebandCarrierToNoiseDbHz =
                                gnssSvf[idx].getBasebandCarrierToNoiseDbHz();
            svNotify.gnssSvs[idx].gloFrequency = gnssSvf[idx].getGloFrequency();
        }
        if (idx < LOC_IDL_CLIENT_DIAG_GNSS_SV_MAX) {
            gnssSVDiag.gnssSvs[idx].svId = gnssSvf[idx].getSvId();
            gnssSVDiag.gnssSvs[idx].type = (locIdlClientDiagGnssSvType)stype;
            gnssSVDiag.gnssSvs[idx].cN0Dbhz =  gnssSvf[idx].getCN0DbHz();
            gnssSVDiag.gnssSvs[idx].elevation =  gnssSvf[idx].getElevation();
            gnssSVDiag.gnssSvs[idx].azimuth =  gnssSvf[idx].getAzimuth();
            gnssSVDiag.gnssSvs[idx].gnssSvOptionsMask =  (locIdlClientDiagGnssSvOptionsMask)optMask;
            gnssSVDiag.gnssSvs[idx].carrierFrequencyHz =  gnssSvf[idx].getCarrierFrequencyHz();
            gnssSVDiag.gnssSvs[idx].gnssSignalTypeMask =  (locIdlClientDiagGnssSignalTypeMask)sigMask;
            gnssSVDiag.gnssSvs[idx].basebandCarrierToNoiseDbHz =
                                gnssSvf[idx].getBasebandCarrierToNoiseDbHz();
            gnssSVDiag.gnssSvs[idx].gloFrequency =  gnssSvf[idx].getGloFrequency();
        }
    }
}
