/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#include "LocIdlClientDevice.h"

void LocIdlClientDevice::fillPosTechMask(unsigned int techmask, unsigned int &outMask)
{
    outMask = 0;
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_GNSS_BIT) {
        outMask |= LOC_POS_TECH_MASK_SATELLITE;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_CELL_BIT) {
        outMask |= LOC_POS_TECH_MASK_CELLID;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_WIFI_BIT) {
        outMask |= LOC_POS_TECH_MASK_WIFI;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_SENSORS_BIT) {
        outMask |= LOC_POS_TECH_MASK_SENSORS;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_REF_LOC_BIT) {
        outMask |= LOC_POS_TECH_MASK_REFERENCE_LOCATION;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_INJECTED_COARSE_POS_BIT) {
        outMask |= LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_AFLT_BIT) {
        outMask |= LOC_POS_TECH_MASK_AFLT;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_HYBRID_BIT) {
        outMask |= LOC_POS_TECH_MASK_HYBRID;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_PPE_BIT) {
        outMask |= LOC_POS_TECH_MASK_PPE;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_VEH_BIT) {
        outMask |= LOC_POS_TECH_MASK_VEH;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_VIS_BIT) {
        outMask |= LOC_POS_TECH_MASK_VIS;
    }
    if (techmask & LocIdlAPI::IDLLocationTechnologyMask::IDL_LOC_TECH_PROPAGATED_BIT) {
        outMask |= LOC_POS_TECH_MASK_PROPAGATED;
    }
}


void LocIdlClientDevice::getLocationRpt(const LocIdlAPI::IDLLocationReport &_locationReport,
                                UlpLocation &ulpLoc, locIdlClientDiagPosition    &gnssPosDiag)
{
    const LocIdlAPI::IDLLocation &location = _locationReport.getLocInfo();
    uint32_t flags = location.getFlags();

    memset(&ulpLoc, 0x00, sizeof(UlpLocation));
    ulpLoc.size = sizeof(UlpLocation);
    ulpLoc.gpsLocation.size = sizeof(LocGpsLocation);
    /** Contains LocGpsLocationFlags bits. */
    ulpLoc.gpsLocation.flags = 0;

    gnssPosDiag.flags = 0;
    gnssPosDiag.gnssInfoFlags = 0;

    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_LAT_LONG_BIT) {
        ulpLoc.gpsLocation.flags |=  LOC_GPS_LOCATION_HAS_LAT_LONG;
        ulpLoc.gpsLocation.latitude = location.getLatitude();
        ulpLoc.gpsLocation.longitude = location.getLongitude();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_LAT_LONG_BIT;
        gnssPosDiag.latitude = location.getLatitude();
        gnssPosDiag.longitude = location.getLongitude();
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_ALTITUDE_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_ALTITUDE;
        ulpLoc.gpsLocation.altitude = location.getAltitude();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ALTITUDE_BIT;
        gnssPosDiag.altitude = location.getAltitude();
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_SPEED_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_SPEED;
        ulpLoc.gpsLocation.speed = location.getSpeed();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_SPEED_BIT;
        gnssPosDiag.speed = location.getSpeed();
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_BEARING_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_BEARING;
        ulpLoc.gpsLocation.bearing = location.getBearing();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_BEARING_BIT;
        gnssPosDiag.bearing = location.getBearing();
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_ACCURACY_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_ACCURACY;
        ulpLoc.gpsLocation.accuracy = location.getHorizontalAccuracy();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ACCURACY_BIT;
        gnssPosDiag.horizontalAccuracy = location.getHorizontalAccuracy();
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_VERTICAL_ACCURACY_BIT) {
        ulpLoc.gpsLocation.flags |= LOC_GPS_LOCATION_HAS_VERT_UNCERTAINITY;
        ulpLoc.gpsLocation.vertUncertainity = location.getVerticalAccuracy();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_VERTICAL_ACCURACY_BIT;
        gnssPosDiag.verticalAccuracy = location.getVerticalAccuracy();
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_SPEED_ACCURACY_BIT) {
        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_SPEED_ACCURACY_BIT;
        gnssPosDiag.speedAccuracy = location.getSpeedAccuracy();
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_BEARING_ACCURACY_BIT) {
        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_BEARING_ACCURACY_BIT;
        gnssPosDiag.bearingAccuracy = location.getBearingAccuracy();
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_TIMESTAMP_BIT) {
        ulpLoc.gpsLocation.timestamp = location.getTimestamp();

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_TIMESTAMP_BIT;
        gnssPosDiag.timestamp = location.getTimestamp();
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_ELAPSED_REAL_TIME_BIT) {
        uint64_t boot_time_ns = 0;
        uint64_t rx_ptp_time_ns = 0;
        uint64_t  tx_ptp_time_ns = _locationReport.getElapsedgPTPTime();
        if (nullptr != gPTPReqIf) {
            gPTPReqIf->gptpGetBootTimeFromPtpTimeIf(&boot_time_ns, tx_ptp_time_ns);
            gPTPReqIf->gptpGetCurPtpTimeIf(&rx_ptp_time_ns);
        }
        ulpLoc.gpsLocation.elapsedRealTime = boot_time_ns + (rx_ptp_time_ns - tx_ptp_time_ns);

        LOC_LOGD("%s] --> elapsedRealTime:%ld, ", __func__, ulpLoc.gpsLocation.elapsedRealTime);

        gnssPosDiag.flags |=  LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_BIT;
        gnssPosDiag.elapsedRealTimeNs = ulpLoc.gpsLocation.elapsedRealTime;
    }
    if (flags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_TIME_UNC_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_TIME_UNC_BIT;
        gnssPosDiag.timeUncMs = location.getTimeUncMs();
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
                                const LocIdlAPI::IDLLocationReport &_locationReport,
                                GpsLocationExtended &gpsLocExt,
                                locIdlClientDiagPosition    &gnssPosDiag
                                )
{
    const LocIdlAPI::IDLLocation &location = _locationReport.getLocInfo();
    uint32_t lInfoflags = location.getFlags();
    memset(&gpsLocExt, 0x00, sizeof(GpsLocationExtended));
    gpsLocExt.size = sizeof(GpsLocationExtended);
    gpsLocExt.flags = 0;

    uint64_t lFlags = _locationReport.getLocationInfoFlags();
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_ALTITUDE_MEAN_SEA_LEVEL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL;
        gpsLocExt.altitudeMeanSeaLevel = _locationReport.getAltitudeMeanSeaLevel();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT;
        gnssPosDiag.altitudeMeanSeaLevel = _locationReport.getAltitudeMeanSeaLevel();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_DOP) {
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

    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_MAGNETIC_DEVIATION) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_MAG_DEV;
        gpsLocExt.magneticDeviation = _locationReport.getMagneticDeviation();

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT;
        gnssPosDiag.magneticDeviation = _locationReport.getMagneticDeviation();
    }
    if (lInfoflags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_VERTICAL_ACCURACY_BIT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_VERT_UNC;
        gpsLocExt.vert_unc = location.getVerticalAccuracy();
    }
    if (lInfoflags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_SPEED_ACCURACY_BIT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_SPEED_UNC;
        gpsLocExt.speed_unc = location.getSpeedAccuracy();
    }
    if (lInfoflags & LocIdlAPI::IDLLocationFlagsMask::IDL_HAS_BEARING_ACCURACY_BIT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_BEARING_UNC;
        gpsLocExt.bearing_unc = location.getBearingAccuracy ();
    }
    if (lFlags & LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_HOR_RELIABILITY) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_RELIABILITY;
        uint32_t hReliability = _locationReport.getHorReliability();
        gpsLocExt.horizontal_reliability = (LocReliability)hReliability;

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT;
        gnssPosDiag.horReliability = (locIdlClientDiagLocationReliability)hReliability;
    }
    if (lFlags & LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_VER_RELIABILITY) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_VERT_RELIABILITY;
        uint32_t vReliability = _locationReport.getVerReliability();
        gpsLocExt.vertical_reliability = (LocReliability)vReliability;

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_VER_RELIABILITY_BIT;
        gnssPosDiag.verReliability = (locIdlClientDiagLocationReliability)vReliability;
    }
    if (lFlags &
            LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MAJOR;
        gpsLocExt.horUncEllipseSemiMajor = _locationReport.getHorUncEllipseSemiMajor();

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT;
        gnssPosDiag.horUncEllipseSemiMajor = _locationReport.getHorUncEllipseSemiMajor();
    }
    if (lFlags &
            LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_HOR_ACCURACY_ELIP_SEMI_MINOR) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_MINOR;
        gpsLocExt.horUncEllipseSemiMinor = _locationReport.getHorUncEllipseSemiMinor();

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MINOR_BIT;
        gnssPosDiag.horUncEllipseSemiMinor = _locationReport.getHorUncEllipseSemiMinor();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_HOR_ACCURACY_ELIP_AZIMUTH) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_HOR_ELIP_UNC_AZIMUTH;
        gpsLocExt.horUncEllipseOrientAzimuth = _locationReport.getHorUncEllipseOrientAzimuth();

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT;
        gnssPosDiag.horUncEllipseOrientAzimuth = _locationReport.getHorUncEllipseOrientAzimuth();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_NUM_SV_USED_IN_POS) {
        gnssPosDiag.gnssInfoFlags |=
            LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NUM_SV_USED_IN_POSITION_BIT;
        gnssPosDiag.numSvUsedInPosition = _locationReport.getNumSvUsedInPosition();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_NUM_SV_USED_IN_POS) {
        const LocIdlAPI::IDLLocationReportSvUsedInPosition &svUsed =
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
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_NAV_SOLUTION_MASK_BIT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_NAV_SOLUTION_MASK;
        unsigned int navSolMask = _locationReport.getNavSolutionMask();

        gpsLocExt.navSolutionMask = 0;

        gnssPosDiag.gnssInfoFlags |= LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT;
        gnssPosDiag.navSolutionMask = (locIdlClientDiagNavSolutionMask)navSolMask;

        if (navSolMask & LocIdlAPI::IDLLocationReportNavSolutionMask::IDL_SBAS_CORR_IONO) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_SBAS_CORRECTION_IONO;
        }
        if (navSolMask & LocIdlAPI::IDLLocationReportNavSolutionMask::IDL_SBAS_CORR_FAST) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_SBAS_CORRECTION_FAST;
        }
        if (navSolMask & LocIdlAPI::IDLLocationReportNavSolutionMask::IDL_SBAS_CORR_LONG) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_SBAS_CORRECTION_LONG;
        }
        if (navSolMask & LocIdlAPI::IDLLocationReportNavSolutionMask::IDL_SBAS_INTEGRITY) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_SBAS_INTEGRITY;
        }
        if (navSolMask & LocIdlAPI::IDLLocationReportNavSolutionMask::IDL_NAV_CORR_DGNSS) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_DGNSS_CORRECTION;
        }
        if (navSolMask & LocIdlAPI::IDLLocationReportNavSolutionMask::IDL_NAV_CORR_RTK) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_RTK_CORRECTION;
        }
        if (navSolMask & LocIdlAPI::IDLLocationReportNavSolutionMask::IDL_NAV_CORR_PPP) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_PPP_CORRECTION;
        }
        if (navSolMask & LocIdlAPI::IDLLocationReportNavSolutionMask::IDL_NAV_CORR_RTK_FIX) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_RTK_FIXED_CORRECTION;
        }
        if (navSolMask &
                LocIdlAPI::IDLLocationReportNavSolutionMask::IDL_NAV_CORR_ONLY_SBAS_CORR_SV_USED) {
            gpsLocExt.navSolutionMask |= LOC_NAV_MASK_ONLY_SBAS_CORRECTED_SV_USED;
        }
    }
    if (lFlags &
            LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LCA_GNSS_LOCATION_INFO_POS_TECH_MASK) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_POS_TECH_MASK;
        fillPosTechMask(_locationReport.getPosTechMask(), gpsLocExt.tech_mask);

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_POS_TECH_MASK_BIT;
        gnssPosDiag.posTechMask = (locIdlClientDiagPosTechMask)_locationReport.getPosTechMask();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_POS_DYNAMICS_DATA) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_POS_DYNAMICS_DATA;

        const LocIdlAPI::IDLLocationReportPositionDynamics &posDynamics =
                                            _locationReport.getBodyFrameData();

        gpsLocExt.bodyFrameData.bodyFrameDataMask = 0;
        uint32_t bodyFrameDataMask = posDynamics.getBodyFrameDataMask();

        gnssPosDiag.gnssInfoFlags |= LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT;
        gnssPosDiag.bodyFrameData.bodyFrameDataMask = 0;

        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_LONG_ACCEL) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
            gpsLocExt.bodyFrameData.longAccel = posDynamics.getLongAccel();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
            gnssPosDiag.bodyFrameData.longAccel = posDynamics.getLongAccel();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_LAT_ACCEL) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
            gpsLocExt.bodyFrameData.latAccel = posDynamics.getLatAccel();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
            gnssPosDiag.bodyFrameData.latAccel = posDynamics.getLatAccel();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_VERT_ACCEL) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
            gpsLocExt.bodyFrameData.vertAccel = posDynamics.getVertAccel();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
            gnssPosDiag.bodyFrameData.vertAccel = posDynamics.getVertAccel();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_YAW_RATE) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
            gpsLocExt.bodyFrameData.yawRate = posDynamics.getYawRate();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
            gnssPosDiag.bodyFrameData.yawRate = posDynamics.getYawRate();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_PITCH) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_BIT;
            gpsLocExt.bodyFrameData.pitch = posDynamics.getPitch();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_BIT;
            gnssPosDiag.bodyFrameData.pitch = posDynamics.getPitch();
        }
        if (bodyFrameDataMask &
                        LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_LONG_ACCEL_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT;
            gpsLocExt.bodyFrameData.longAccelUnc = posDynamics.getLongAccelUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT;
            gnssPosDiag.bodyFrameData.longAccelUnc = posDynamics.getLongAccelUnc();
        }
        if (bodyFrameDataMask &
                LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_LAT_ACCEL_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |=
                                        LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT;
            gpsLocExt.bodyFrameData.latAccelUnc = posDynamics.getLatAccelUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT;
            gnssPosDiag.bodyFrameData.latAccelUnc = posDynamics.getLatAccelUnc();
        }
        if (bodyFrameDataMask &
                    LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_VERT_ACCEL_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT;
            gpsLocExt.bodyFrameData.vertAccelUnc = posDynamics.getVertAccelUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                            LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT;
            gnssPosDiag.bodyFrameData.vertAccelUnc = posDynamics.getVertAccelUnc();
        }
        if (bodyFrameDataMask &
                        LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_YAW_RATE_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT;
            gpsLocExt.bodyFrameData.yawRateUnc = posDynamics.getYawRateUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                            LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT;
            gnssPosDiag.bodyFrameData.yawRateUnc = posDynamics.getYawRateUnc();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_PITCH_UNC) {
            gpsLocExt.bodyFrameData.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT;
            gpsLocExt.bodyFrameData.pitchUnc = posDynamics.getPitchUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT;
            gnssPosDiag.bodyFrameData.pitchUnc = posDynamics.getPitchUnc();
        }

        gpsLocExt.bodyFrameDataExt.bodyFrameDataMask = 0;

        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_PITCH_RATE) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT;
            gpsLocExt.bodyFrameDataExt.pitchRate = posDynamics.getPitchRate();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT;
            gnssPosDiag.bodyFrameData.pitchRate = posDynamics.getPitchRate();
        }
        if (bodyFrameDataMask &
                        LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_PITCH_RATE_UNC) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |=
                                                    LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT;
            gpsLocExt.bodyFrameDataExt.pitchRateUnc = posDynamics.getPitchRateUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                            LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT;
            gnssPosDiag.bodyFrameData.pitchRateUnc = posDynamics.getPitchRateUnc();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_ROLL) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_BIT;
            gpsLocExt.bodyFrameDataExt.roll = posDynamics.getRoll();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                            LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_BIT;
            gnssPosDiag.bodyFrameData.roll = posDynamics.getRoll();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_ROLL_UNC) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT;
            gpsLocExt.bodyFrameDataExt.rollUnc = posDynamics.getRollUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT;
            gnssPosDiag.bodyFrameData.rollUnc = posDynamics.getRollUnc();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_ROLL_RATE) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT;
            gpsLocExt.bodyFrameDataExt.rollRate = posDynamics.getRollRate();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                        LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT;
            gnssPosDiag.bodyFrameData.rollRate = posDynamics.getRollRate();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_ROLL_RATE_UNC) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT;
            gpsLocExt.bodyFrameDataExt.rollRateUnc = posDynamics.getRollRateUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                        LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT;
            gnssPosDiag.bodyFrameData.rollRateUnc = posDynamics.getRollRateUnc();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_YAW) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_BIT;
            gpsLocExt.bodyFrameDataExt.yaw = posDynamics.getYaw();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_BIT;
            gnssPosDiag.bodyFrameData.yaw = posDynamics.getYaw();
        }
        if (bodyFrameDataMask & LocIdlAPI::IDLLocationReportPosDataMask::IDL_NAV_DATA_YAW_UNC) {
            gpsLocExt.bodyFrameDataExt.bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_UNC_BIT;
            gpsLocExt.bodyFrameDataExt.yawUnc = posDynamics.getYawUnc();

            gnssPosDiag.bodyFrameData.bodyFrameDataMask |=
                                    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_UNC_BIT;
            gnssPosDiag.bodyFrameData.yawUnc = posDynamics.getYawUnc();
        }
    }

    const LocIdlAPI::IDLGnssSystemTime &gnssTime = _locationReport.getGnssSystemTime();
    uint32_t timeSrc = gnssTime.getGnssSystemTimeSrc();
    gpsLocExt.gnssSystemTime.gnssSystemTimeSrc = (Gnss_LocSvSystemEnumType)timeSrc;
    const LocIdlAPI::IDLSystemTimeStructUnion &time = gnssTime.getTimeUnion();

    gnssPosDiag.gnssSystemTime.gnssSystemTimeSrc =
                        (locIdlClientDiagGnss_LocSvSystemEnumType)timeSrc;
    if(time.isType<LocIdlAPI::IDLGnssSystemTimeStructType>()) {
          const LocIdlAPI::IDLGnssSystemTimeStructType &systemTime =
                                            time.get<LocIdlAPI::IDLGnssSystemTimeStructType>();
          gpsLocExt.gnssSystemTime.u.gpsSystemTime.validityMask = systemTime.getValidityMask();
          gpsLocExt.gnssSystemTime.u.gpsSystemTime.systemWeek = systemTime.getSystemWeek();
          gpsLocExt.gnssSystemTime.u.gpsSystemTime.systemMsec = systemTime.getSystemMsec();
          gpsLocExt.gnssSystemTime.u.gpsSystemTime.systemClkTimeBias =
                                                        systemTime.getSystemClkTimeBias();
          gpsLocExt.gnssSystemTime.u.gpsSystemTime.systemClkTimeUncMs =
                                                        systemTime.getSystemClkTimeUncMs();
          gpsLocExt.gnssSystemTime.u.gpsSystemTime.refFCount = systemTime.getRefFCount();
          gpsLocExt.gnssSystemTime.u.gpsSystemTime.numClockResets = systemTime.getNumClockResets();

          gnssPosDiag.gnssSystemTime.u.gpsSystemTime.validityMask = systemTime.getValidityMask();
          gnssPosDiag.gnssSystemTime.u.gpsSystemTime.systemWeek = systemTime.getSystemWeek();
          gnssPosDiag.gnssSystemTime.u.gpsSystemTime.systemMsec = systemTime.getSystemMsec();
          gnssPosDiag.gnssSystemTime.u.gpsSystemTime.systemClkTimeBias =
                                                                systemTime.getSystemClkTimeBias();
          gnssPosDiag.gnssSystemTime.u.gpsSystemTime.systemClkTimeUncMs =
                                                                systemTime.getSystemClkTimeUncMs();
          gnssPosDiag.gnssSystemTime.u.gpsSystemTime.refFCount = systemTime.getRefFCount();
          gnssPosDiag.gnssSystemTime.u.gpsSystemTime.numClockResets = systemTime.getNumClockResets();

          if (timeSrc == LocIdlAPI::IDLGnssSvSystemEnumType::IDL_LOC_SV_SYSTEM_GPS) {
                gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_GPS_TIME;
                gpsLocExt.gpsTime.gpsWeek = systemTime.getSystemWeek();
                gpsLocExt.gpsTime.gpsTimeOfWeekMs =  systemTime.getSystemMsec();
          }
    } else if (time.isType<LocIdlAPI::IDLGnssGloTimeStructType>()) {
          const LocIdlAPI::IDLGnssGloTimeStructType &gloTime =
                                        time.get<LocIdlAPI::IDLGnssGloTimeStructType>();
          gpsLocExt.gnssSystemTime.u.gloSystemTime.validityMask = gloTime.getValidityMask();
          gpsLocExt.gnssSystemTime.u.gloSystemTime.gloFourYear = gloTime.getGloFourYear();
          gpsLocExt.gnssSystemTime.u.gloSystemTime.gloDays = gloTime.getGloDays();
          gpsLocExt.gnssSystemTime.u.gloSystemTime.gloMsec = gloTime.getGloMsec();
          gpsLocExt.gnssSystemTime.u.gloSystemTime.gloClkTimeBias = gloTime.getGloClkTimeBias();
          gpsLocExt.gnssSystemTime.u.gloSystemTime.gloClkTimeUncMs = gloTime.getGloClkTimeUncMs();
          gpsLocExt.gnssSystemTime.u.gloSystemTime.refFCount = gloTime.getRefFCount();
          gpsLocExt.gnssSystemTime.u.gloSystemTime.numClockResets = gloTime.getNumClockResets();

          gnssPosDiag.gnssSystemTime.u.gloSystemTime.validityMask = gloTime.getValidityMask();
          gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloFourYear = gloTime.getGloFourYear();
          gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloDays = gloTime.getGloDays();
          gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloMsec = gloTime.getGloMsec();
          gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloClkTimeBias = gloTime.getGloClkTimeBias();
          gnssPosDiag.gnssSystemTime.u.gloSystemTime.gloClkTimeUncMs =
                                                                gloTime.getGloClkTimeUncMs();
          gnssPosDiag.gnssSystemTime.u.gloSystemTime.refFCount = gloTime.getRefFCount();
          gnssPosDiag.gnssSystemTime.u.gloSystemTime.numClockResets = gloTime.getNumClockResets();
    }

    if (lFlags & LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_EXT_DOP ) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_EXT_DOP;
        gpsLocExt.extDOP.PDOP = _locationReport.getPdop();
        gpsLocExt.extDOP.HDOP = _locationReport.getHdop();
        gpsLocExt.extDOP.VDOP = _locationReport.getVdop();
        gpsLocExt.extDOP.GDOP = _locationReport.getGdop();
        gpsLocExt.extDOP.TDOP = _locationReport.getTdop();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_NORTH_STD_DEV) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_NORTH_STD_DEV;
        gpsLocExt.northStdDeviation = _locationReport.getNorthStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_STD_DEV_BIT;
        gnssPosDiag.northStdDeviation = _locationReport.getNorthStdDeviation();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_EAST_STD_DEV) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_EAST_STD_DEV;
        gpsLocExt.eastStdDeviation = _locationReport.getEastStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_STD_DEV_BIT;
        gnssPosDiag.eastStdDeviation = _locationReport.getEastStdDeviation();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_NORTH_VEL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_NORTH_VEL;
        gpsLocExt.northVelocity = _locationReport.getNorthVelocity();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_VEL_BIT;
        gnssPosDiag.northVelocity = _locationReport.getNorthVelocity();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_EAST_VEL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_EAST_VEL;
        gpsLocExt.eastVelocity = _locationReport.getEastVelocity();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_VEL_BIT;
        gnssPosDiag.eastVelocity = _locationReport.getEastVelocity();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_UP_VEL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_UP_VEL;
        gpsLocExt.upVelocity = _locationReport.getUpVelocity();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_UP_VEL_BIT;
        gnssPosDiag.upVelocity = _locationReport.getUpVelocity();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_NORTH_VEL_UNC) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_NORTH_VEL_UNC;
        gpsLocExt.northVelocityStdDeviation = _locationReport.getNorthVelocityStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_VEL_UNC_BIT;
        gnssPosDiag.northVelocityStdDeviation = _locationReport.getNorthVelocityStdDeviation();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_EAST_VEL_UNC) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_EAST_VEL_UNC;
        gpsLocExt.eastVelocityStdDeviation = _locationReport.getEastVelocityStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_VEL_UNC_BIT;
        gnssPosDiag.eastVelocityStdDeviation = _locationReport.getEastVelocityStdDeviation();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_UP_VEL_UNC) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_UP_VEL_UNC;
        gpsLocExt.upVelocityStdDeviation = _locationReport.getUpVelocityStdDeviation();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_UP_VEL_UNC_BIT;
        gnssPosDiag.upVelocityStdDeviation = _locationReport.getUpVelocityStdDeviation();
    }

    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_LEAP_SECONDS) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_LEAP_SECONDS;
        gpsLocExt.leapSeconds = _locationReport.getLeapSeconds();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_LEAP_SECONDS_BIT;
        gnssPosDiag.leapSeconds = _locationReport.getLeapSeconds();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_GNSS_SV_USED_DATA) {
        const std::vector<LocIdlAPI::IDLGnssMeasUsageInfo> &meas =
                                                _locationReport.getMeasUsageInfo();
        gpsLocExt.numOfMeasReceived = meas.size();

        gnssPosDiag.gnssInfoFlags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT;
        gnssPosDiag.numOfMeasReceived = meas.size();

        for (int idx = 0; idx < meas.size() && idx < GNSS_SV_MAX; idx++) {
            uint32_t constellation = meas[idx].getGnssConstellation();
            gpsLocExt.measUsageInfo[idx].gnssConstellation =
                                            (Gnss_LocSvSystemEnumType)constellation;
            gpsLocExt.measUsageInfo[idx].gnssSignalType = meas[idx].getGnssSignalType();

            gnssPosDiag.measUsageInfo[idx].gnssConstellation =
                                (locIdlClientDiagGnss_LocSvSystemEnumType)constellation;
            gnssPosDiag.measUsageInfo[idx].gnssSignalType = meas[idx].getGnssSignalType();
            gnssPosDiag.measUsageInfo[idx].gnssSvId = meas[idx].getGnssSvId();
        }
    }

    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_CALIB_CONFIDENCE_PERCENT) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_CALIBRATION_CONFIDENCE;
        gpsLocExt.calibrationConfidence = _locationReport.getCalibrationConfidencePercent();

        gnssPosDiag.gnssInfoFlags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CALIBRATION_CONFIDENCE_PERCENT_BIT;
        gnssPosDiag.calibrationConfidencePercent =
                    _locationReport.getCalibrationConfidencePercent();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_CALIB_STATUS) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_CALIBRATION_STATUS;
        unsigned int calStatus = _locationReport.getCalibrationStatus();
        gpsLocExt.calibrationStatus = 0;

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CALIBRATION_STATUS_BIT;
        gnssPosDiag.calibrationStatus = (locIdlClientDiagDrCalibrationStatusMask)calStatus;

        if (calStatus & LocIdlAPI::IDLDrCalibrationStatusMask::IDL_DR_ROLL_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_ROLL_CALIBRATION_NEEDED;
        }
        if (calStatus & LocIdlAPI::IDLDrCalibrationStatusMask::IDL_DR_PITCH_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_PITCH_CALIBRATION_NEEDED;
        }
        if (calStatus & LocIdlAPI::IDLDrCalibrationStatusMask::IDL_DR_YAW_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_YAW_CALIBRATION_NEEDED;
        }
        if (calStatus & LocIdlAPI::IDLDrCalibrationStatusMask::IDL_DR_ODO_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_ODO_CALIBRATION_NEEDED;
        }
        if (calStatus & LocIdlAPI::IDLDrCalibrationStatusMask::IDL_DR_GYRO_CALIB_NEEDED) {
            gpsLocExt.calibrationStatus |= DR_GYRO_CALIBRATION_NEEDED;
        }
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_OUTPUT_ENG_TYPE) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_OUTPUT_ENG_TYPE;
        uint32_t engTypeIdl = _locationReport.getLocOutputEngType();
        LocOutputEngineType engType = (LocOutputEngineType)0;
        if (engTypeIdl == LocIdlAPI::IDLLocOutputEngineType::IDL_LOC_OUTPUT_ENGINE_FUSED) {
            engType = LOC_OUTPUT_ENGINE_FUSED;
        } else if (engTypeIdl == LocIdlAPI::IDLLocOutputEngineType::IDL_LOC_OUTPUT_ENGINE_SPE) {
            engType = LOC_OUTPUT_ENGINE_SPE;
        } else if (engTypeIdl == LocIdlAPI::IDLLocOutputEngineType::IDL_LOC_OUTPUT_ENGINE_PPE) {
            engType = LOC_OUTPUT_ENGINE_PPE;
        } else if (engTypeIdl == LocIdlAPI::IDLLocOutputEngineType::IDL_LOC_OUTPUT_ENGINE_VPE) {
            engType = LOC_OUTPUT_ENGINE_VPE;
        }
        gpsLocExt.locOutputEngType = engType;

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_OUTPUT_ENG_TYPE_BIT;
        gnssPosDiag.locOutputEngType = (locIdlClientDiagLocOutputEngineType)engType;
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_OUTPUT_ENG_MASK) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_OUTPUT_ENG_MASK;
        gpsLocExt.locOutputEngMask = _locationReport.getLocOutputEngMask();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_OUTPUT_ENG_MASK_BIT;
        gnssPosDiag.locOutputEngMask =
                    (locIdlClientDiagPositioningEngineMask)_locationReport.getLocOutputEngMask();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_CONFORMITY_INDEX) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_CONFORMITY_INDEX;
        gpsLocExt.conformityIndex = _locationReport.getConformityIndex();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CONFORMITY_INDEX_BIT;
        gnssPosDiag.conformityIndex = _locationReport.getConformityIndex();
    }

    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_LLA_VRP_BASED) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_LLA_VRP_BASED;
        const LocIdlAPI::IDLLLAInfo &lla = _locationReport.getLlaVRPBased();
        gpsLocExt.llaVRPBased.latitude =  lla.getLatitude();
        gpsLocExt.llaVRPBased.longitude =  lla.getLongitude();
        gpsLocExt.llaVRPBased.altitude =  lla.getAltitude();

        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_LLA_VRP_BASED_BIT;
        gnssPosDiag.llaVRPBased.latitude =  lla.getLatitude();
        gnssPosDiag.llaVRPBased.longitude =  lla.getLongitude();
        gnssPosDiag.llaVRPBased.altitude =  lla.getAltitude();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_ENU_VEL_VRP_BASED) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_ENU_VELOCITY_LLA_VRP_BASED;
        const std::vector<float> &emu = _locationReport.getEnuVelocityVRPBased();

        gnssPosDiag.gnssInfoFlags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_ENU_VELOCITY_VRP_BASED_BIT;
        for (int k = 0; k < emu.size(); k++) { //emu.size() should be 3
            gpsLocExt.enuVelocityVRPBased[k] = emu[k];
            gnssPosDiag.enuVelocityVRPBased[k] = emu[k];
        }
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_DR_SOL_STATUS_MASK) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_DR_SOLUTION_STATUS_MASK;
        uint64_t mask = _locationReport.getDrSolutionStatusMask();
        gpsLocExt.drSolutionStatusMask = 0;

        gnssPosDiag.gnssInfoFlags |=
                LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_DR_SOLUTION_STATUS_MASK_BIT;
        gnssPosDiag.drSolutionStatusMask = (locIdlClientDiagDrSolutionStatusMask)mask;

        if (mask & LocIdlAPI::IDLDrSolutionStatusMask::
                    IDL_DR_SOLUTION_STATUS_VEHICLE_SENSOR_SPEED_INPUT_DETECTED)
            gpsLocExt.drSolutionStatusMask |= VEHICLE_SENSOR_SPEED_INPUT_DETECTED;
        if (mask & LocIdlAPI::IDLDrSolutionStatusMask::
                    IDL_DR_SOLUTION_STATUS_VEHICLE_SENSOR_SPEED_INPUT_USED)
            gpsLocExt.drSolutionStatusMask |= VEHICLE_SENSOR_SPEED_INPUT_USED;
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_ALTITUDE_ASSUMED) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_ALTITUDE_ASSUMED;
        gpsLocExt.altitudeAssumed = _locationReport.getAltitudeAssumed();

        gnssPosDiag.gnssInfoFlags |= LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_ALTITUDE_ASSUMED_BIT;
        gnssPosDiag.altitudeAssumed = _locationReport.getAltitudeAssumed();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_SESSION_STATUS) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_SESSION_STATUS_BIT;

        uint32_t st = _locationReport.getSessionStatus();
        locIdlClientDiagLocSessionStatus status;
        if (st == LocIdlAPI::IDLLocSessionStatus::IDL_LOC_SESS_SUCCESS) {
            status = LOC_IDL_CLIENT_DIAG_LOC_SESS_SUCCESS;
        } else if (st == LocIdlAPI::IDLLocSessionStatus::IDL_LOC_SESS_INTERMEDIATE) {
            status = LOC_IDL_CLIENT_DIAG_LOC_SESS_INTERMEDIATE;
        } else {
            status = LOC_IDL_CLIENT_DIAG_LOC_SESS_FAILURE;
        }
        gnssPosDiag.sessionStatus = status;
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_INTEGRITY_RISK_USED) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_INTEGRITY_RISK_USED;
        gpsLocExt.integrityRiskUsed = _locationReport.getIntegrityRiskUsed();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_INTEGRITY_RISK_USED_BIT;
        gnssPosDiag.integrityRiskUsed = _locationReport.getIntegrityRiskUsed();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_PROTECT_ALONG_TRACK) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_PROTECT_ALONG_TRACK;
        gpsLocExt.protectAlongTrack = _locationReport.getProtectAlongTrack();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_ALONG_TRACK_BIT;
        gnssPosDiag.protectAlongTrack = _locationReport.getProtectAlongTrack();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_PROTECT_CROSS_TRACK) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_PROTECT_CROSS_TRACK;
        gpsLocExt.protectCrossTrack = _locationReport.getProtectCrossTrack();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_CROSS_TRACK_BIT;
        gnssPosDiag.protectCrossTrack = _locationReport.getProtectCrossTrack();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_PROTECT_VERTICAL) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_PROTECT_VERTICAL;
        gpsLocExt.protectVertical = _locationReport.getProtectVertical();

        gnssPosDiag.gnssInfoFlags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_VERTICAL_BIT;
        gnssPosDiag.protectVertical = _locationReport.getProtectVertical();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_DGNSS_STATION_ID) {
        gpsLocExt.flags |= GPS_LOCATION_EXTENDED_HAS_DGNSS_STATION_ID;
        const std::vector<uint16_t> &dgnss = _locationReport.getDgnssStationId();
        gpsLocExt.numOfDgnssStationId = dgnss.size();

        gnssPosDiag.gnssInfoFlags |= LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_DGNSS_STATION_ID_BIT;
        gnssPosDiag.numOfDgnssStationId = dgnss.size();

        for (int l = 0; l < dgnss.size(); l++) {
               gpsLocExt.dgnssStationId[l] = dgnss[l];
               gnssPosDiag.dgnssStationId[l] = dgnss[l];
        }
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_GPTP_TIME_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_GPTP_TIME_BIT;
        gnssPosDiag.elapsedgPTPTime = _locationReport.getElapsedgPTPTime();
    }
    if (lFlags &  LocIdlAPI::IDLLCALocationInfoFlagMask::IDL_LOC_INFO_GPTP_TIME_UNC_BIT) {
        gnssPosDiag.gnssInfoFlags |=  LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_GPTP_TIME_UNC_BIT;
        gnssPosDiag.elapsedgPTPTimeUnc = _locationReport.getElapsedgPTPTimeUnc();
    }
}

void LocIdlClientDevice::getMeasurementSet(const LocIdlAPI::IDLGnssMeasurements& gnssMeasurement,
                                    GnssMeasurements &svMeasurementSet,
                                    locIdlClientDiagGnssMeasPacket &gnssMeasDiag)
{
    memset(&svMeasurementSet, 0x00, sizeof(GnssMeasurements));
    svMeasurementSet.size = sizeof(GnssMeasurements);
    memset(&gnssMeasDiag, 0x00, sizeof(locIdlClientDiagGnssMeasPacket));

    svMeasurementSet.gnssSvMeasurementSet.size = sizeof(GnssSvMeasurementSet);
    svMeasurementSet.gnssSvMeasurementSet.svMeasSetHeader.size = sizeof(GnssSvMeasurementHeader);
    svMeasurementSet.gnssSvMeasurementSet.svMeasSetHeader.flags = 0;
    svMeasurementSet.gnssMeasNotification.size = sizeof(GnssMeasurementsNotification);
    svMeasurementSet.gnssMeasNotification.isNhz = gnssMeasurement.getIsNHz();

    const vector<LocIdlAPI::IDLGnssMeasurementsData > &measData =
                                        gnssMeasurement.getMeasurements();
    svMeasurementSet.gnssMeasNotification.count = measData.size(); //WC: GNSS_MEASUREMENTS_MAX

    gnssMeasDiag.count = measData.size();
    gnssMeasDiag.isNhz = gnssMeasurement.getIsNHz();

    for (uint16_t idx = 0; idx < measData.size() && idx < GNSS_MEASUREMENTS_MAX; idx++) {
        svMeasurementSet.gnssMeasNotification.measurements[idx].size = sizeof(GnssMeasurementsData);

        unsigned int flags = measData[idx].getFlags();
        svMeasurementSet.gnssMeasNotification.measurements[idx].flags = 0;
        gnssMeasDiag.measurements[idx].flags = 0;

        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_SV_ID_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                            GNSS_MEASUREMENTS_DATA_SV_ID_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].svId = measData[idx].getSvId();

            gnssMeasDiag.measurements[idx].flags |=
                                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_ID_BIT;
            gnssMeasDiag.measurements[idx].svId = measData[idx].getSvId();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_SV_TYPE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                            GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT;
            uint32_t stype = measData[idx].getSvType();
            svMeasurementSet.gnssMeasNotification.measurements[idx].svType = (GnssSvType)stype;

            gnssMeasDiag.measurements[idx].flags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT;
            gnssMeasDiag.measurements[idx].svType = (locIdlClientDiagGnssSvType)stype;
        }
        svMeasurementSet.gnssMeasNotification.measurements[idx].timeOffsetNs =
                                                            measData[idx].getTimeOffsetNs();
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_STATE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                            GNSS_MEASUREMENTS_DATA_STATE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].stateMask =
                                                                measData[idx].getStateMask();

            gnssMeasDiag.measurements[idx].flags |=
                                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_STATE_BIT;
            gnssMeasDiag.measurements[idx].stateMask = measData[idx].getStateMask();
        }
        if (flags &
            LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_RECEIVED_SV_TIME_BIT) {
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
            LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_RECVD_SV_TIME_UNC_BIT) {
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
                LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_CARRIER_TO_NOISE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                    GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierToNoiseDbHz =
                                                        measData[idx].getCarrierToNoiseDbHz();

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT;
            gnssMeasDiag.measurements[idx].carrierToNoiseDbHz =
                                measData[idx].getCarrierToNoiseDbHz();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_PR_RATE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].pseudorangeRateMps =
                                                        measData[idx].getPseudorangeRateMps();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT;
            gnssMeasDiag.measurements[idx].pseudorangeRateMps =
                            measData[idx].getPseudorangeRateMps();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_PR_RATE_UNC_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                        GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].pseudorangeRateUncertaintyMps =
                                                measData[idx].getPseudorangeRateUncertaintyMps();

            gnssMeasDiag.measurements[idx].flags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT;
            gnssMeasDiag.measurements[idx].pseudorangeRateUncertaintyMps =
                    measData[idx].getPseudorangeRateUncertaintyMps();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_ADR_STATE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                            GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].adrStateMask =
                                                measData[idx].getAdrStateMask();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT;
            gnssMeasDiag.measurements[idx].adrStateMask = measData[idx].getAdrStateMask();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_ADR_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                        GNSS_MEASUREMENTS_DATA_ADR_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].adrMeters =
                                                        measData[idx].getAdrMeters();

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_BIT;
            gnssMeasDiag.measurements[idx].adrMeters = measData[idx].getAdrMeters();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_ADR_UNC_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                    GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].adrUncertaintyMeters =
                                                    measData[idx].getAdrUncertaintyMeters();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT;
            gnssMeasDiag.measurements[idx].adrUncertaintyMeters =
                            measData[idx].getAdrUncertaintyMeters();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_CARRIER_FREQ_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierFrequencyHz =
                                                measData[idx].getCarrierFrequencyHz();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT;
            gnssMeasDiag.measurements[idx].carrierFrequencyHz =
                            measData[idx].getCarrierFrequencyHz();
        }
        if (flags &
                LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_CARRIER_CYCLES_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierCycles =
                                                measData[idx].getCarrierCycles();

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT;
            gnssMeasDiag.measurements[idx].carrierCycles = measData[idx].getCarrierCycles();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_CARRIER_PHASE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                        GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierPhase =
                                                        measData[idx].getCarrierPhase();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT;
            gnssMeasDiag.measurements[idx].carrierPhase = measData[idx].getCarrierPhase();
        }
        if (flags &
                LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_CARRIER_PHASE_UNC_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                            GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].carrierPhaseUncertainty =
                                            measData[idx].getCarrierPhaseUncertainty();

            gnssMeasDiag.measurements[idx].flags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT;
            gnssMeasDiag.measurements[idx].carrierPhaseUncertainty =
                        measData[idx].getCarrierPhaseUncertainty();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_MULTIPATH_IND_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                    GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT;
            uint32_t mPathIndicator = measData[idx].getMultipathIndicator();
            svMeasurementSet.gnssMeasNotification.measurements[idx].multipathIndicator =
                                                (GnssMeasurementsMultipathIndicator)mPathIndicator;

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT;
            gnssMeasDiag.measurements[idx].multipathIndicator =
                                (locIdlClientDiagGnssMeasurementsMultipathIndicator)mPathIndicator;
        }
        if (flags &
                LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_SIGNAL_TO_NOISE_RATIO) {
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
            LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_AUTO_GAIN_CTRL_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                    GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].agcLevelDb =
                                                    measData[idx].getAgcLevelDb();

            gnssMeasDiag.measurements[idx].flags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT;
            gnssMeasDiag.measurements[idx].agcLevelDb = measData[idx].getAgcLevelDb();
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_FULL_ISB_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                            GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT;
        }
        if (flags & LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_FULL_ISB_UNC_BIT) {
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
                LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_CYCLE_SLIP_COUNT_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                        GNSS_MEASUREMENTS_DATA_CYCLE_SLIP_COUNT_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].cycleSlipCount =
                                        measData[idx].getCycleSlipCount();

            gnssMeasDiag.measurements[idx].flags |=
                        LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_BASEBAND_CARRIER_TO_NOISE_BIT;
            gnssMeasDiag.measurements[idx].cycleSlipCount = measData[idx].getCycleSlipCount();
        }
        if (flags &
            LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::IDL_MEAS_DATA_GNSS_SIGNAL_TYPE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                                    GNSS_MEASUREMENTS_DATA_GNSS_SIGNAL_TYPE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].gnssSignalType =
                                                    measData[idx].getGnssSignalType();

            gnssMeasDiag.measurements[idx].flags |=
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT;
            gnssMeasDiag.measurements[idx].gnssSignalType = measData[idx].getGnssSignalType();
        }
        if (flags &
            LocIdlAPI::IDLGnssMeasurementsDataFlagsMask::
            IDL_MEAS_DATA_BASEBAND_CARRIER_TO_NOISE_BIT) {
            svMeasurementSet.gnssMeasNotification.measurements[idx].flags |=
                                        GNSS_MEASUREMENTS_DATA_BASEBAND_CARRIER_TO_NOISE_BIT;
            svMeasurementSet.gnssMeasNotification.measurements[idx].basebandCarrierToNoiseDbHz =
                                        measData[idx].getBasebandCarrierToNoiseDbHz();

            gnssMeasDiag.measurements[idx].flags |=
                            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT;
            gnssMeasDiag.measurements[idx].basebandCarrierToNoiseDbHz =
                                    measData[idx].getBasebandCarrierToNoiseDbHz();
        }
    }

    const LocIdlAPI::IDLGnssMeasurementsClock &clk = gnssMeasurement.getClock();
    svMeasurementSet.gnssMeasNotification.clock.size = sizeof(GnssMeasurementsClock);
    unsigned int flag = clk.getFlags();

    svMeasurementSet.gnssMeasNotification.clock.flags = 0;

    if (flag & LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_LEAP_SECOND_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                    GNSS_MEASUREMENTS_CLOCK_FLAGS_LEAP_SECOND_BIT;
        svMeasurementSet.gnssMeasNotification.clock.leapSecond = clk.getLeapSecond();

        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_LEAP_SECOND_BIT;
        gnssMeasDiag.clock.leapSecond = clk.getLeapSecond();
    }
    if (flag & LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_TIME_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                    GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_BIT;
        svMeasurementSet.gnssMeasNotification.clock.timeNs = clk.getTimeNs();

        gnssMeasDiag.clock.flags |= LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_BIT;
        gnssMeasDiag.clock.timeNs = clk.getTimeNs();
    }
    if (flag &
        LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_TIME_UNCERTAINTY_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                            GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_UNCERTAINTY_BIT;
        svMeasurementSet.gnssMeasNotification.clock.timeUncertaintyNs =
                                            clk.getTimeUncertaintyNs();

        gnssMeasDiag.clock.flags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_UNCERTAINTY_BIT;
        gnssMeasDiag.clock.timeUncertaintyNs = clk.getTimeUncertaintyNs();
    }
    if (flag & LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_FULL_BIAS_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                            GNSS_MEASUREMENTS_CLOCK_FLAGS_FULL_BIAS_BIT;
        svMeasurementSet.gnssMeasNotification.clock.fullBiasNs = clk.getFullBiasNs();

        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_FULL_BIAS_BIT;
        gnssMeasDiag.clock.fullBiasNs = clk.getFullBiasNs();
    }
    if (flag & LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_BIAS_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                    GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_BIT;
        svMeasurementSet.gnssMeasNotification.clock.biasNs = clk.getBiasNs();

        gnssMeasDiag.clock.flags |= LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_BIT;
        gnssMeasDiag.clock.biasNs = clk.getBiasNs();
    }
    if (flag &
        LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_BIAS_UNCERTAINTY_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                    GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_UNCERTAINTY_BIT;
        svMeasurementSet.gnssMeasNotification.clock.biasUncertaintyNs =
                                    clk.getBiasUncertaintyNs();

        gnssMeasDiag.clock.flags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_UNCERTAINTY_BIT;
        gnssMeasDiag.clock.biasUncertaintyNs = clk.getBiasUncertaintyNs();
    }
    if (flag & LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_DRIFT_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |= GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_BIT;
        svMeasurementSet.gnssMeasNotification.clock.driftNsps = clk.getDriftNsps();

        gnssMeasDiag.clock.flags |= LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_BIT;
        gnssMeasDiag.clock.driftNsps = clk.getDriftNsps();
    }
    if (flag &
        LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_DRIFT_UNCERTAINTY_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                                                GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_UNCERTAINTY_BIT;
        svMeasurementSet.gnssMeasNotification.clock.driftUncertaintyNsps =
                                                clk.getDriftUncertaintyNsps();

        gnssMeasDiag.clock.flags |=
                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_UNCERTAINTY_BIT;
        gnssMeasDiag.clock.driftUncertaintyNsps = clk.getDriftUncertaintyNsps();
    }
    if (flag & LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::
                        IDL_MEAS_CLK_HW_CLOCK_DISCONTINUITY_COUNT_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                        GNSS_MEASUREMENTS_CLOCK_FLAGS_HW_CLOCK_DISCONTINUITY_COUNT_BIT;
        svMeasurementSet.gnssMeasNotification.clock.hwClockDiscontinuityCount =
                        clk.getHwClockDiscontinuityCount();

        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_HW_CLOCK_DISCONTINUITY_COUNT_BIT;
        gnssMeasDiag.clock.hwClockDiscontinuityCount = clk.getHwClockDiscontinuityCount();
    }
    if (flag &
        LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_ELAPSED_REAL_TIME_BIT) {
        svMeasurementSet.gnssMeasNotification.clock.flags |=
                    GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_BIT;

        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_BIT;
        gnssMeasDiag.clock.elapsedRealTime = clk.getElapsedRealTime();
    }
    if (flag &
    LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_ELAPSED_REAL_TIME_UNC_BIT) {
        gnssMeasDiag.clock.flags |=
            LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_UNC_BIT;
        gnssMeasDiag.clock.elapsedRealTimeUnc = clk.getElapsedRealTimeUnc();
    }
    if (flag &
    LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_ELAPSED_GPTP_TIME_BIT) {
        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_GPTP_TIME_BIT;
        gnssMeasDiag.clock.elapsedgPTPTime = clk.getElapsedgPTPTime();
    }
    if (flag &
    LocIdlAPI::IDLGnssMeasurementsClockFlagsMask::IDL_MEAS_CLK_FLAGS_ELAPSED_GPTP_TIME_UNC_BIT) {
        gnssMeasDiag.clock.flags |=
                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_GPTP_TIME_UNC_BIT;
        gnssMeasDiag.clock.elapsedgPTPTimeUnc = clk.getElapsedgPTPTimeUnc();
    }
}

void LocIdlClientDevice::getSvRpt(const std::vector<LocIdlAPI::IDLGnssSv> &gnssSvf,
                                    GnssSvNotification &svNotify,
                                    locIdlClientDiagGnssSv& gnssSVDiag)
{
    memset(&svNotify, 0x00, sizeof(GnssSvNotification));
    memset(&gnssSVDiag, 0x00, sizeof(locIdlClientDiagGnssSv));
    svNotify.size = sizeof(GnssSvNotification);
    svNotify.count = gnssSvf.size();
    gnssSVDiag.count = gnssSvf.size();
    svNotify.gnssSignalTypeMaskValid = true;

    for (uint16_t idx = 0; idx < GNSS_SV_MAX && idx < gnssSvf.size(); idx++) {
        svNotify.gnssSvs[idx].size = sizeof(GnssSv);
        svNotify.gnssSvs[idx].svId = gnssSvf[idx].getSvId();
        gnssSVDiag.gnssSvs[idx].svId = gnssSvf[idx].getSvId();
        uint32_t stype = gnssSvf[idx].getType();
        svNotify.gnssSvs[idx].type = (GnssSvType)stype;
        gnssSVDiag.gnssSvs[idx].type = (locIdlClientDiagGnssSvType)stype;
        svNotify.gnssSvs[idx].cN0Dbhz = gnssSvf[idx].getCN0Dbhz();
        gnssSVDiag.gnssSvs[idx].cN0Dbhz =  gnssSvf[idx].getCN0Dbhz();
        svNotify.gnssSvs[idx].elevation = gnssSvf[idx].getElevation();
        gnssSVDiag.gnssSvs[idx].elevation =  gnssSvf[idx].getElevation();
        svNotify.gnssSvs[idx].azimuth = gnssSvf[idx].getAzimuth();
        gnssSVDiag.gnssSvs[idx].azimuth =  gnssSvf[idx].getAzimuth();
        uint32_t optMask = gnssSvf[idx].getGnssSvOptionsMask();
        svNotify.gnssSvs[idx].gnssSvOptionsMask = (GnssSvOptionsMask)optMask;
        gnssSVDiag.gnssSvs[idx].gnssSvOptionsMask =  (locIdlClientDiagGnssSvOptionsMask)optMask;
        svNotify.gnssSvs[idx].carrierFrequencyHz = gnssSvf[idx].getCarrierFrequencyHz();
        gnssSVDiag.gnssSvs[idx].carrierFrequencyHz =  gnssSvf[idx].getCarrierFrequencyHz();
        uint32_t sigMask = gnssSvf[idx].getGnssSignalTypeMask();
        svNotify.gnssSvs[idx].gnssSignalTypeMask = (GnssSignalTypeMask)sigMask;
        gnssSVDiag.gnssSvs[idx].gnssSignalTypeMask =  (locIdlClientDiagGnssSignalTypeMask)sigMask;
        svNotify.gnssSvs[idx].basebandCarrierToNoiseDbHz =
                            gnssSvf[idx].getBasebandCarrierToNoiseDbHz();
        gnssSVDiag.gnssSvs[idx].basebandCarrierToNoiseDbHz =
                            gnssSvf[idx].getBasebandCarrierToNoiseDbHz();
        svNotify.gnssSvs[idx].gloFrequency = gnssSvf[idx].getGloFrequency();
        gnssSVDiag.gnssSvs[idx].gloFrequency =  gnssSvf[idx].getGloFrequency();
    }
}
