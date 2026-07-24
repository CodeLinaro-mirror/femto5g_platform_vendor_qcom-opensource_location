/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#include <cstring>
#include "gps_extended_c.h"
#include "log_util.h"
#include "LocApiV02Utils.h"

#ifdef USE_GLIB

static inline bool isGloSvId(uint16_t svId) {
    return (svId >= 65 && svId <= 96);
}

static inline GnssSignalTypeMask toGnssSignalTypeMask(uint8_t extSignalType) {
    switch (static_cast<GnssExtended_SignalEnumType>(extSignalType)) {
        case GNSS_EXTENDED_SIGNAL_TYPE_GPS_L1CA:   return GNSS_SIGNAL_GPS_L1CA;
        case GNSS_EXTENDED_SIGNAL_TYPE_GLO_G1:     return GNSS_SIGNAL_GLONASS_G1;
        case GNSS_EXTENDED_SIGNAL_TYPE_BDS_B1:     return GNSS_SIGNAL_BEIDOU_B1I;
        case GNSS_EXTENDED_SIGNAL_TYPE_GAL_E1:     return GNSS_SIGNAL_GALILEO_E1;
        case GNSS_EXTENDED_SIGNAL_TYPE_QZSS_L1CA:  return GNSS_SIGNAL_QZSS_L1CA;
        case GNSS_EXTENDED_SIGNAL_TYPE_GPS_L2C:    return GNSS_SIGNAL_GPS_L2;
        case GNSS_EXTENDED_SIGNAL_TYPE_GPS_L5Q:    return GNSS_SIGNAL_GPS_L5;
        case GNSS_EXTENDED_SIGNAL_TYPE_GLO_G2:     return GNSS_SIGNAL_GLONASS_G2;
        case GNSS_EXTENDED_SIGNAL_TYPE_GAL_E5A:    return GNSS_SIGNAL_GALILEO_E5A;
        case GNSS_EXTENDED_SIGNAL_TYPE_QZSS_L2C:   return GNSS_SIGNAL_QZSS_L2;
        case GNSS_EXTENDED_SIGNAL_TYPE_QZSS_L5Q:   return GNSS_SIGNAL_QZSS_L5;
        case GNSS_EXTENDED_SIGNAL_TYPE_BDS_B2A:    return GNSS_SIGNAL_BEIDOU_B2AI;
        case GNSS_EXTENDED_SIGNAL_TYPE_NAVIC_L5:   return GNSS_SIGNAL_NAVIC_L5;
        case GNSS_EXTENDED_SIGNAL_TYPE_BDS_B1CQ:   return GNSS_SIGNAL_BEIDOU_B1C;
        case GNSS_EXTENDED_SIGNAL_TYPE_GAL_E5B:    return GNSS_SIGNAL_GALILEO_E5B;

        default:
            return static_cast<GnssSignalTypeMask>(0);
    }
}

void decodeSvResidualDataFromExtendedBinaryData(const GnssExtended_FixInfoStructType& inFix,
        GnssSvResidualReport &outReport) {
    outReport.locOutputEngType = LOC_OUTPUT_ENGINE_SPE;
    LOC_LOGd("u_Major %d u_Minor %d", inFix.z_Revision.u_Major, inFix.z_Revision.u_Minor);
    if (inFix.z_Revision.u_Major != GNSS_EXTENDED_MAJOR_REVISION) {
        LOC_LOGe("Unsupported GnssExtended major revision %d, expected %d",
                inFix.z_Revision.u_Major, GNSS_EXTENDED_MAJOR_REVISION);
        return;
    }
    // ------------------ GNSS System Time (GPS) ------------------

    outReport.gnssSystemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_GPS;
    GnssSystemTimeStructType& gpsTime = outReport.gnssSystemTime.u.gpsSystemTime;

    gpsTime.validityMask = 0;
    gpsTime.systemWeek   = inFix.z_NavPos.w_GpsWeek;
    gpsTime.systemMsec   = inFix.z_NavPos.q_GpsTimeMs;
    gpsTime.refFCount    = inFix.z_NavPos.q_RefFCount;

    if (inFix.z_NavPos.z_SmPosFlags.b_IsGpsTimeValid) {
        gpsTime.validityMask |= GNSS_SYSTEM_TIME_WEEK_VALID;
        gpsTime.validityMask |= GNSS_SYSTEM_TIME_WEEK_MS_VALID;
    }
    gpsTime.validityMask |= GNSS_SYSTEM_REF_FCOUNT_VALID;

    // ------------------ Residual PVT Data ------------------
    GnssResidualPVTData& pvt = outReport.residualPvtData;
    pvt.validityMask = 0;

    // Always copy raw values
    pvt.posLla[0] = inFix.z_NavPos.d_PosLla[0];
    pvt.posLla[1] = inFix.z_NavPos.d_PosLla[1];
    pvt.posLla[2] = inFix.z_NavPos.d_PosLla[2];

    pvt.velEnu[0] = inFix.z_NavPos.f_VelEnu[0];
    pvt.velEnu[1] = inFix.z_NavPos.f_VelEnu[1];
    pvt.velEnu[2] = inFix.z_NavPos.f_VelEnu[2];

    pvt.headingRad     = inFix.z_NavPos.f_HeadingRad;
    pvt.headingUncRad  = inFix.z_NavPos.f_HeadingUncRad;

    pvt.puncLatLonMeters[0] = inFix.z_NavPos.f_PuncLatLonMeters[0];
    pvt.puncLatLonMeters[1] = inFix.z_NavPos.f_PuncLatLonMeters[1];
    pvt.puncVertMeters      = inFix.z_NavPos.f_PuncVertMeters;

    pvt.vuncEastNorthMps[0] = inFix.z_NavPos.f_VuncEastNorthMps[0];
    pvt.vuncEastNorthMps[1] = inFix.z_NavPos.f_VuncEastNorthMps[1];
    pvt.vuncVertMps         = inFix.z_NavPos.f_VuncVertMps;

    pvt.clockBias           = inFix.z_NavPos.f_ClockBias;
    pvt.clockBiasUncMs      = inFix.z_NavPos.f_ClockBiasUncMs;
    pvt.clockDriftRate      = inFix.z_NavPos.f_ClockDriftRate;
    pvt.clockDriftRateUncMps= inFix.z_NavPos.f_ClockDriftRateUncMps;
    pvt.pdop                = inFix.z_NavPos.f_PDOP;

    const GnssExtended_NavSlnFlagsStructType& posFlags = inFix.z_NavPos.z_PosFlags;
    const GnssExtended_NavPosFlags& smFlags            = inFix.z_NavPos.z_SmPosFlags;

    // LLA validity: treat as valid when solution is valid.
    if (posFlags.b_IsValid) {
        pvt.validityMask |= GNSS_RESIDUAL_POS_LLA_VALID_BIT;
        pvt.validityMask |= GNSS_RESIDUAL_POS_CLOCK_BIAS_VALID_BIT;
        pvt.validityMask |= GNSS_RESIDUAL_POS_CLOCK_DRIFT_VALID_BIT;
        pvt.validityMask |= GNSS_RESIDUAL_POS_PDOP_VALID_BIT;
    }

    // Velocity + velocity uncertainty
    if (posFlags.b_IsVelValid) {
        pvt.validityMask |= GNSS_RESIDUAL_POS_VEL_ENU_VALID_BIT;
        pvt.validityMask |= GNSS_RESIDUAL_POS_VUNC_EAST_NORTH_VALID_BIT;
        pvt.validityMask |= GNSS_RESIDUAL_POS_VUNC_VERT_VALID_BIT;
    }

    // Heading + heading uncertainty
    if (posFlags.b_IsHeadingValid) {
        pvt.validityMask |= GNSS_RESIDUAL_POS_HEADING_VALID_BIT;
        pvt.validityMask |= GNSS_RESIDUAL_POS_HEADING_UNC_VALID_BIT;
    }

    // Position uncertainty (punc*)
    if (smFlags.b_IsPuncValid) {
        pvt.validityMask |= GNSS_RESIDUAL_POS_PUNC_LAT_LON_VALID_BIT;
        pvt.validityMask |= GNSS_RESIDUAL_POS_PUNC_VERT_VALID_BIT;
    }

    // Time uncertainty / clock uncertainty: best match is smFlags.b_IsTuncValid
    if (smFlags.b_IsTuncValid) {
        pvt.validityMask |= GNSS_RESIDUAL_POS_CLOCK_BIAS_UNC_VALID_BIT;
        pvt.validityMask |= GNSS_RESIDUAL_POS_CLOCK_DRIFT_UNC_VALID_BIT;
    }

    // ------------------ SV Residual Array ------------------

    uint16_t svCount = 0;
    for (int i = 0; i < GNSS_EXTENDED_SYSTEM_IDX_GNSS_TOTAL; ++i) {
        svCount = static_cast<uint16_t>(svCount + inFix.z_NavSvInfo.u_GnssNumSvInfo[i]);
    }
    svCount = std::min<uint16_t>(svCount, GNSS_EXTENDED_N_ACTIVE_MAX);
    svCount = std::min<uint16_t>(svCount, GNSS_MEASUREMENTS_MAX);

    outReport.numSvs = svCount;

    for (uint16_t i = 0; i < svCount; ++i) {
        const GnssExtended_GnssSvInfoType& inSv = inFix.z_NavSvInfo.z_GnssSvInfo[i];
        GnssSvResidualInfo& outSv = outReport.svResidualInfo[i];

        std::memset(&outSv, 0, sizeof(outSv));

        outSv.svId = inSv.w_SvId;
        outSv.signalTypeMask = toGnssSignalTypeMask(inSv.u_UsedMeasSignalType);
        outSv.validityMask = 0;


        if (inSv.q_SvInfoMask & GNSS_EXTENDED_SV_INFO_USED_IN_POS_FIX) {
            outSv.prRes = inSv.f_PrResMeters;
            outSv.prUnc = inSv.f_PrUncMeters;
            outSv.validityMask |= GNSS_SV_RES_PR_RES_VALID_BIT;
            outSv.validityMask |= GNSS_SV_RES_PR_UNC_VALID_BIT;

            // IODE vs GLO Tb: populate the right field and validity bit
            if (isGloSvId(inSv.w_SvId)) {
                outSv.gloTb = inSv.u_GloTb;
                outSv.validityMask |= GNSS_SV_RES_GLO_TB_VALID_BIT;
            } else {
                // In GnssExtended, w_IODE is uint16; LocationDataTypes uses uint8 for iode.
                outSv.iode = static_cast<uint8_t>(inSv.w_IODE & 0xFF);
                outSv.validityMask |= GNSS_SV_RES_IODE_VALID_BIT;
            }
        }

        if (inSv.q_SvInfoMask & GNSS_EXTENDED_SV_INFO_USED_IN_VEL_FIX) {
            outSv.dopplerRes = inSv.f_PrrResMps;
            outSv.dopplerUnc = inSv.f_PrrUncMps;
            outSv.validityMask |= GNSS_SV_RES_DOPPLER_RES_VALID_BIT;
            outSv.validityMask |= GNSS_SV_RES_DOPPLER_UNC_VALID_BIT;
        }

        // GLONASS freqNum: comment says valid only for GLO and only when MEAS_VALID
        if (isGloSvId(inSv.w_SvId) &&
            (inSv.q_SvInfoMask & GNSS_EXTENDED_SV_INFO_MEAS_VALID) &&
            (inSv.q_SvInfoMask & GNSS_EXTENDED_SV_INFO_USED_IN_POS_FIX)) {
            outSv.freqNum = inSv.b_FreqNum;
            outSv.validityMask |= GNSS_SV_RES_FREQ_NUM_VALID_BIT;
        }

        // The following fields do not exist in GnssExtended_GnssSvInfoType:
        // cpRes/cpUnc, cNo, azim/elev -> leave unset, validity bits remain 0.
    }
    // ------------------ SV Available/Used Info ------------------
    GnssSvAvailableUsedInfo& au = outReport.svAvailableUsedInfo;
    au.validityMask = 0;

    if (inFix.z_NavSvInfo.u_SvInfoValid) {
        // GPS
        au.gpsNumSvMeas   = inFix.z_NavSvInfo.u_GpsNumSvMeas;
        au.gpsNumSvPosFix = inFix.z_NavSvInfo.u_GpsNumSvPosFix;
        au.gpsNumSvVelFix = inFix.z_NavSvInfo.u_GpsNumSvVelFix;
        au.gpsSvMaskUsed  = inFix.z_NavSvInfo.q_GpsSvMaskUsed;

        au.validityMask |= GNSS_SV_AVAIL_GPS_NUM_SV_MEAS_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_GPS_NUM_SV_POS_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_GPS_NUM_SV_VEL_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_GPS_SV_MASK_USED_VALID_BIT;

        // GLO
        au.gloNumSvMeas   = inFix.z_NavSvInfo.u_GloNumSvMeas;
        au.gloNumSvPosFix = inFix.z_NavSvInfo.u_GloNumSvPosFix;
        au.gloNumSvVelFix = inFix.z_NavSvInfo.u_GloNumSvVelFix;
        au.gloSvMaskUsed  = inFix.z_NavSvInfo.q_GloSvMaskUsed;

        au.validityMask |= GNSS_SV_AVAIL_GLO_NUM_SV_MEAS_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_GLO_NUM_SV_POS_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_GLO_NUM_SV_VEL_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_GLO_SV_MASK_USED_VALID_BIT;

        // BDS
        au.bdsNumSvMeas   = inFix.z_NavSvInfo.u_BdsNumSvMeas;
        au.bdsNumSvPosFix = inFix.z_NavSvInfo.u_BdsNumSvPosFix;
        au.bdsNumSvVelFix = inFix.z_NavSvInfo.u_BdsNumSvVelFix;
        au.bdsSvMaskUsed  = inFix.z_NavSvInfo.t_BdsSvMaskUsed;

        au.validityMask |= GNSS_SV_AVAIL_BDS_NUM_SV_MEAS_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_BDS_NUM_SV_POS_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_BDS_NUM_SV_VEL_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_BDS_SV_MASK_USED_VALID_BIT;

        // GAL
        au.galNumSvMeas   = inFix.z_NavSvInfo.u_GalNumSvMeas;
        au.galNumSvPosFix = inFix.z_NavSvInfo.u_GalNumSvPosFix;
        au.galNumSvVelFix = inFix.z_NavSvInfo.u_GalNumSvVelFix;
        au.galSvMaskUsed  = inFix.z_NavSvInfo.t_GalSvMaskUsed;

        au.validityMask |= GNSS_SV_AVAIL_GAL_NUM_SV_MEAS_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_GAL_NUM_SV_POS_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_GAL_NUM_SV_VEL_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_GAL_SV_MASK_USED_VALID_BIT;

        // QZSS
        au.qzssNumSvMeas   = inFix.z_NavSvInfo.u_QzssNumSvMeas;
        au.qzssNumSvPosFix = inFix.z_NavSvInfo.u_QzssNumSvPosFix;
        au.qzssNumSvVelFix = inFix.z_NavSvInfo.u_QzssNumSvVelFix;
        au.qzssSvMaskUsed  = static_cast<uint16_t>(inFix.z_NavSvInfo.u_QzssSvMaskUsed);

        au.validityMask |= GNSS_SV_AVAIL_QZSS_NUM_SV_MEAS_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_QZSS_NUM_SV_POS_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_QZSS_NUM_SV_VEL_FIX_VALID_BIT;
        au.validityMask |= GNSS_SV_AVAIL_QZSS_SV_MASK_USED_VALID_BIT;


    }

}
#endif //USE_GLIB


