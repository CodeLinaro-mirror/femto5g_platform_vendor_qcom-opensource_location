/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#ifndef LOC_IDL_DIAG_LOG_TYPE_H
#define LOC_IDL_DIAG_LOG_TYPE_H

#include <stdint.h>
#include <limits.h>
#include <time.h>
#include <inttypes.h>
#include <errno.h>
#include <LocDiagIface.h>

#define LOC_IDL_CLIENT_DIAG_GNSS_SV_MAX            (176)
#define LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ  (90)
#define LOC_IDL_CLIENT_DIAG_DGNSS_STATION_ID_MAX    (3)
#define LOC_IDL_CLIENT_DIAG_MAX_NUMBER_OF_SIGNAL_TYPES (22)
#define LOC_IDL_CLIENT_DIAG_MSG_VERSION        (0)

#ifndef LOC_IDL_CLIENT_API_PACKET_C
#define LOC_IDL_CLIENT_API_PACKET_C (0x1E6B)
#endif

namespace loc_idl_diag {

/** Flags to indicate which values are valid in a Location */
typedef uint16_t locIdlClientDiagLocationFlagsMask;
typedef enum {
    /** location has valid latitude and longitude */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_LAT_LONG_BIT          = (1<<0),
    /** location has valid altitude */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ALTITUDE_BIT          = (1<<1),
    /** location has valid speed */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_SPEED_BIT             = (1<<2),
    /** location has valid bearing */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_BEARING_BIT           = (1<<3),
    /** location has valid accuracy */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ACCURACY_BIT          = (1<<4),
    /** location has valid vertical accuracy */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_VERTICAL_ACCURACY_BIT = (1<<5),
    /** location has valid speed accuracy */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_SPEED_ACCURACY_BIT    = (1<<6),
    /** location has valid bearing accuracy */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_BEARING_ACCURACY_BIT  = (1<<7),
    /** location has valid valid timestamp  */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_TIMESTAMP_BIT         = (1<<8),
    /** Location has valid Location::elapsedRealTime. <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_BIT = (1<<9),
    /** Location has valid Location::elapsedRealTimeUnc. <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_ELAPSED_REAL_TIME_UNC_BIT = (1<<10),
    /** Location has valid Location::timeUncMs. <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_HAS_TIME_UNC_BIT = (1<<11),
} locIdlClientDiagLocationFlagsBits;

typedef uint32_t locIdlClientDiagPosDataMask;
typedef enum {
    /** Navigation data has Forward Acceleration */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT  = (1<<0),
    /** Navigation data has Sideward Acceleration */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT   = (1<<1),
    /** Navigation data has Vertical Acceleration */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT  = (1<<2),
    /** Navigation data has Heading Rate */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_RATE_BIT    = (1<<3),
    /** Navigation data has Body pitch */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_BIT       = (1<<4),
    /** Navigation data has Forward Acceleration uncertainty */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT = (1<<5),
    /** Navigation data has Sideward Acceleration uncertainty */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT  = (1<<6),
    /** Navigation data has Vertical Acceleration uncertainty */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT = (1<<7),
    /** Navigation data has Heading Rate uncertainty */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT   = (1<<8),
    /** Navigation data has Body pitch uncertainty */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT      = (1<<9),
    /** Navigation data has pitch rate */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT     = (1<<10),
    /** Navigation data has body pitch rate uncertainty */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT = (1<<11),
    /** Navigation data has body roll */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_BIT           = (1<<12),
    /** Navigation data has body roll uncertainty */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT       = (1<<13),
    /** Navigation data has body rate roll */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT      = (1<<14),
    /** Navigation data has body roll rate uncertainty */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT  = (1<<15),
    /** Navigation data has body yaw */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_BIT            = (1<<16),
    /** Navigation data has body roll uncertainty */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_DATA_HAS_YAW_UNC_BIT        = (1<<17)
} locIdlClientDiagPosDataBits;

typedef enum {
    /** GPS satellite. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GPS                    = 1,
    /** GALILEO satellite. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GALILEO                = 2,
    /** SBAS satellite. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_SBAS                   = 3,
    /** GLONASS satellite. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GLONASS                = 4,
    /** BDS satellite. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_BDS                    = 5,
    /** QZSS satellite. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_QZSS                   = 6,
    /** NAVIC satellite. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_NAVIC                  = 7
} locIdlClientDiagGnss_LocSvSystemEnumType;

typedef uint32_t locIdlClientDiagGnssSystemTimeStructTypeFlags;
typedef enum {
    LOC_IDL_CLIENT_DIAG_GNSS_SYSTEM_TIME_WEEK_VALID             = (1 << 0),
    LOC_IDL_CLIENT_DIAG_GNSS_SYSTEM_TIME_WEEK_MS_VALID          = (1 << 1),
    LOC_IDL_CLIENT_DIAG_GNSS_SYSTEM_CLK_TIME_BIAS_VALID         = (1 << 2),
    LOC_IDL_CLIENT_DIAG_GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID     = (1 << 3),
    LOC_IDL_CLIENT_DIAG_GNSS_SYSTEM_REF_FCOUNT_VALID            = (1 << 4),
    LOC_IDL_CLIENT_DIAG_GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID      = (1 << 5)
} locIdlClientDiagGnssSystemTimeTypeBits;

typedef uint32_t locIdlClientDiagGnssGloTimeStructTypeFlags;
typedef enum {
    LOC_IDL_CLIENT_DIAG_GNSS_CLO_DAYS_VALID                     = (1 << 0),
    LOC_IDL_CLIENT_DIAG_GNSS_GLOS_MSEC_VALID                    = (1 << 1),
    LOC_IDL_CLIENT_DIAG_GNSS_GLO_CLK_TIME_BIAS_VALID            = (1 << 2),
    LOC_IDL_CLIENT_DIAG_GNSS_GLO_CLK_TIME_BIAS_UNC_VALID        = (1 << 3),
    LOC_IDL_CLIENT_DIAG_GNSS_GLO_REF_FCOUNT_VALID               = (1 << 4),
    LOC_IDL_CLIENT_DIAG_GNSS_GLO_NUM_CLOCK_RESETS_VALID         = (1 << 5),
    LOC_IDL_CLIENT_DIAG_GNSS_GLO_FOUR_YEAR_VALID                = (1 << 6)
} locIdlClientDiagGnssGloTimeTypeBits;

typedef PACKED struct PACKED_POST {
    /** Validity mask for below fields */
    locIdlClientDiagGnssSystemTimeStructTypeFlags validityMask;
    /** Extended week number at reference tick.
    Unit: Week.
    Set to 65535 if week number is unknown.
    For GPS:
      Calculated from midnight, Jan. 6, 1980.
      OTA decoded 10 bit GPS week is extended to map between:
      [NV6264 to (NV6264 + 1023)].
      NV6264: Minimum GPS week number configuration.
      Default value of NV6264: 1738
    For BDS:
      Calculated from 00:00:00 on January 1, 2006 of Coordinated Universal Time (UTC).
    For GAL:
      Calculated from 00:00 UT on Sunday August 22,1999 (midnight between August 21 and August 22).
   */
    uint16_t systemWeek;
    /** Time in to the current week at reference tick.
       Unit: Millisecond. Range: 0 to 604799999.
       Check for systemClkTimeUncMs before use */
    uint32_t systemMsec;
    /** System clock time bias (sub-millisecond)
        Units: Millisecond
        Note: System time (TOW Millisecond) = systemMsec - systemClkTimeBias.
        Check for systemClkTimeUncMs before use. */
    float systemClkTimeBias;
    /** Single sided maximum time bias uncertainty
        Units: Millisecond */
    float systemClkTimeUncMs;
    /** FCount (free running HW timer) value. Don't use for relative time purpose
         due to possible discontinuities.
         Unit: Millisecond */
    uint32_t refFCount;
    /** Number of clock resets/discontinuities detected,
        affecting the local hardware counter value. */
    uint32_t numClockResets;
} locIdlClientDiagGnssSystemTimeStructType;

typedef PACKED struct PACKED_POST {
    /** GLONASS day number in four years. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 65535 */
    uint16_t gloDays;
    /** Validity mask for below fields */
    locIdlClientDiagGnssGloTimeStructTypeFlags validityMask;
    /** GLONASS time of day in Millisecond. Refer to GLONASS ICD.
        Units: Millisecond
        Check for gloClkTimeUncMs before use */
    uint32_t gloMsec;
    /** GLONASS clock time bias (sub-millisecond)
        Units: Millisecond
        Note: GLO time (TOD Millisecond) = gloMsec - gloClkTimeBias.
        Check for gloClkTimeUncMs before use. */
    float gloClkTimeBias;
    /** Single sided maximum time bias uncertainty
        Units: Millisecond */
    float gloClkTimeUncMs;
    /** FCount (free running HW timer) value. Don't use for relative time purpose
        due to possible discontinuities.
        Unit: Millisecond */
    uint32_t  refFCount;
    /** Number of clock resets/discontinuities detected,
        affecting the local hardware counter value. */
    uint32_t numClockResets;
    /** GLONASS four year number from 1996. Refer to GLONASS ICD.
        Applicable only for GLONASS and shall be ignored for other constellations.
        If unknown shall be set to 255 */
    uint8_t gloFourYear;
} locIdlClientDiagGnssGloTimeStructType;

typedef PACKED union PACKED_POST {
    locIdlClientDiagGnssSystemTimeStructType gpsSystemTime;
    locIdlClientDiagGnssSystemTimeStructType galSystemTime;
    locIdlClientDiagGnssSystemTimeStructType bdsSystemTime;
    locIdlClientDiagGnssSystemTimeStructType qzssSystemTime;
    locIdlClientDiagGnssGloTimeStructType gloSystemTime;
    locIdlClientDiagGnssSystemTimeStructType navicSystemTime;
} locIdlClientDiagSystemTimeStructUnion;

typedef PACKED struct PACKED_POST {
    /** Contains Body frame LocPosDataMask bits. */
    locIdlClientDiagPosDataMask  bodyFrameDataMask;
    /** Forward Acceleration in body frame (m/s2)*/
    float           longAccel;
    /** Sideward Acceleration in body frame (m/s2)*/
    float           latAccel;
    /** Vertical Acceleration in body frame (m/s2)*/
    float           vertAccel;
    /** Heading Rate (Radians/second) */
    float           yawRate;
    /** Body pitch (Radians) */
    float           pitch;
    /** Uncertainty of Forward Acceleration in body frame */
    float           longAccelUnc;
    /** Uncertainty of Side-ward Acceleration in body frame */
    float           latAccelUnc;
    /** Uncertainty of Vertical Acceleration in body frame */
    float           vertAccelUnc;
    /** Uncertainty of Heading Rate */
    float           yawRateUnc;
    /** Uncertainty of Body pitch */
    float           pitchUnc;
    /** Body pitch rate (Radians/second) */
    float pitchRate;
    /** Uncertainty of pitch rate (Radians/second) */
    float pitchRateUnc;
    /** Roll of body frame. Clockwise positive. (Radian) */
    float roll;
    /** Uncertainty of Roll, 68% confidence level (radian) */
    float rollUnc;
    /** Roll rate of body frame. Clockwise positive. (radian/second)
    */
    float rollRate;
    /** Uncertainty of Roll rate, 68% confidence level (radian/second) */
    float rollRateUnc;
    /** Yaw of body frame. Clockwise positive (radian) */
    float yaw;
    /** Uncertainty of Yaw, 68% confidence level (radian) */
    float yawUnc;
} locIdlClientDiagPositionDynamics;

/** @struct
    Time applicability of PVT report
*/
typedef PACKED struct PACKED_POST {
    /** Specifies GNSS system time reported. Mandatory field */
    locIdlClientDiagGnss_LocSvSystemEnumType gnssSystemTimeSrc;
    /** Reporting of GPS system time is recommended.
      If GPS time is unknown & other satellite system time is known,
      it should be reported.
      Mandatory field
     */
    locIdlClientDiagSystemTimeStructUnion u;
} locIdlClientDiagGnssSystemTime;

/** GNSS Signal Type and RF Band */
typedef uint32_t locIdlClientDiagGnssSignalTypeMask;
typedef enum {
    /** GPS L1CA Signal */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_GPS_L1CA            = (1<<0),
    /** GPS L1C Signal */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_GPS_L1C             = (1<<1),
    /** GPS L2 RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_GPS_L2              = (1<<2),
    /** GPS L5 RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_GPS_L5              = (1<<3),
    /** GLONASS G1 (L1OF) RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G1          = (1<<4),
    /** GLONASS G2 (L2OF) RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G2          = (1<<5),
    /** GALILEO E1 RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E1          = (1<<6),
    /** GALILEO E5A RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5A         = (1<<7),
    /** GALILEO E5B RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5B         = (1<<8),
    /** BEIDOU B1 RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1           = (1<<9),
    /** BEIDOU B2 RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2           = (1<<10),
    /** QZSS L1CA RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1CA           = (1<<11),
    /** QZSS L1S RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1S            = (1<<12),
    /** QZSS L2 RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_QZSS_L2             = (1<<13),
    /** QZSS L5 RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_QZSS_L5             = (1<<14),
    /** SBAS L1 RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_SBAS_L1             = (1<<15),
    /** BEIDOU B1I RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1I          = (1<<16),
    /** BEIDOU B1C RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1C          = (1<<17),
    /** BEIDOU B2I RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2I          = (1<<18),
    /** BEIDOU B2AI RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AI         = (1<<19),
    /** NAVIC L5 RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_NAVIC_L5            = (1<<20),
    /** BEIDOU B2AQ RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AQ         = (1<<21),
    /** BEIDOU B2BI RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2BI         = (1<<22),
    /** BEIDOU B2BQ RF Band */
    LOC_IDL_CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2BQ         = (1<<23)
} locIdlClientDiagGnssSignalTypeBits;

typedef PACKED struct PACKED_POST {
    /** GnssSignalType mask */
    locIdlClientDiagGnssSignalTypeMask gnssSignalType;
   /** Specifies GNSS Constellation Type */
    locIdlClientDiagGnss_LocSvSystemEnumType gnssConstellation;
    /**  GNSS SV ID.
     For GPS:      1 to 32
     For GLONASS:  65 to 96. When slot-number to SV ID mapping is unknown, set as 255.
     For SBAS:     120 to 151
     For QZSS-L1CA:193 to 197
     For BDS:      201 to 263
     For GAL:      301 to 336
     For NAVIC:    401 to 414 */
    uint16_t gnssSvId;
} locIdlClientDiagGnssMeasUsageInfo;

typedef enum {
    LOC_IDL_CLIENT_DIAG_GNSS_SV_TYPE_UNKNOWN = 0,
    LOC_IDL_CLIENT_DIAG_GNSS_SV_TYPE_GPS,
    LOC_IDL_CLIENT_DIAG_GNSS_SV_TYPE_SBAS,
    LOC_IDL_CLIENT_DIAG_GNSS_SV_TYPE_GLONASS,
    LOC_IDL_CLIENT_DIAG_GNSS_SV_TYPE_QZSS,
    LOC_IDL_CLIENT_DIAG_GNSS_SV_TYPE_BEIDOU,
    LOC_IDL_CLIENT_DIAG_GNSS_SV_TYPE_GALILEO,
    LOC_IDL_CLIENT_DIAG_GNSS_SV_TYPE_NAVIC
} locIdlClientDiagGnssSvType;

typedef uint16_t locIdlClientDiagGnssSvOptionsMask;
typedef enum {
    LOC_IDL_CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_EPHEMER_BIT           = (1<<0),
    LOC_IDL_CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_ALMANAC_BIT           = (1<<1),
    LOC_IDL_CLIENT_DIAG_GNSS_SV_OPTIONS_USED_IN_FIX_BIT           = (1<<2),
    LOC_IDL_CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT = (1<<3),
    LOC_IDL_CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_GNSS_SIGNAL_TYPE_BIT  = (1<<4),
    LOC_IDL_CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_BASEBAND_CARRIER_TO_NOISE_BIT = (1<<5),
    LOC_IDL_CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_ELEVATION_BIT         = (1<<6),
    LOC_IDL_CLIENT_DIAG_GNSS_SV_OPTIONS_HAS_AZIMUTH_BIT           = (1<<7),
} locIdlClientDiagGnssSvOptionsBits;

typedef PACKED struct PACKED_POST {
    /** Unique Identifier */
    uint16_t svId;
    /** type of SV (GPS, SBAS, GLONASS, QZSS, BEIDOU, GALILEO, NAVIC) */
    locIdlClientDiagGnssSvType type;
    /** signal strength */
    float cN0Dbhz;
    /** elevation of SV (in degrees) */
    float elevation;
    /** azimuth of SV (in degrees) */
    float azimuth;
    /** Bitwise OR of GnssSvOptionsBits */
    locIdlClientDiagGnssSvOptionsMask gnssSvOptionsMask;
    /** carrier frequency of the signal tracked */
    float carrierFrequencyHz;
    /** Specifies GNSS signal type */
    locIdlClientDiagGnssSignalTypeMask gnssSignalTypeMask;
    /** baseband signal strength */
    double basebandCarrierToNoiseDbHz;
    /** Glonass frequency number */
    uint16_t gloFrequency;
} locIdlClientDiagGnssSvData;

typedef uint16_t locIdlClientDiagLocationTechnologyMask;
typedef enum {
    /** location was calculated using GNSS */
    LOC_IDL_CLIENT_DIAG_LOCATION_TECHNOLOGY_GNSS_BIT     = (1<<0),
    /** location was calculated using Cell */
    LOC_IDL_CLIENT_DIAG_LOCATION_TECHNOLOGY_CELL_BIT     = (1<<1),
    /** location was calculated using WiFi */
    LOC_IDL_CLIENT_DIAG_LOCATION_TECHNOLOGY_WIFI_BIT     = (1<<2),
    /** location was calculated using Sensors */
    LOC_IDL_CLIENT_DIAG_LOCATION_TECHNOLOGY_SENSORS_BIT  = (1<<3),
    /** location was calculated using reference station */
    LOC_IDL_CLIENT_DIAG_LOCATION_REFERENCE_LOCATION_BIT  = (1<<4),
    /** location was calculated using CPI */
    LOC_IDL_CLIENT_DIAG_LOCATION_INJECTED_COARSE_POSITION_BIT  = (1<<5),
    /** location was calculated using AFLT */
    LOC_IDL_CLIENT_DIAG_LOCATION_AFLT_BIT               = (1<<6),
    /** location was calculated using HYBRID */
    LOC_IDL_CLIENT_DIAG_LOCATION_HYBRID_BIT             = (1<<7),
    /** location was calculated using PPE */
    LOC_IDL_CLIENT_DIAG_LOCATION_PPE_BIT                = (1<<8),
    /** location was calculated using VEH */
    LOC_IDL_CLIENT_DIAG_LOCATION_VEH_BIT                = (1<<9),
    /** location was calculated using VIS */
    LOC_IDL_CLIENT_DIAG_LOCATION_VIS_BIT                = (1<<10),
    /** location was calculated using cached measurements */
    LOC_IDL_CLIENT_DIAG_LOCATION_PROPAGATED_BIT         = (1<<11)
} locIdlClientDiagLocationTechnologyBits;

typedef uint32_t locIdlClientDiagDrCalibrationStatusMask;
typedef enum {
    /** Indicate that roll calibration is needed. Need to take more
     *  turns on level ground */
    LOC_IDL_CLIENT_DIAG_DR_ROLL_CALIBRATION_NEEDED  = (1<<0),
    /** Indicate that pitch calibration is needed. Need to take more
     *  turns on level ground */
    LOC_IDL_CLIENT_DIAG_DR_PITCH_CALIBRATION_NEEDED = (1<<1),
    /** Indicate that yaw calibration is needed. Need to accelerate
     *  in a straight line  */
    LOC_IDL_CLIENT_DIAG_DR_YAW_CALIBRATION_NEEDED   = (1<<2),
    /** Indicate that odo calibration is needed. Need to accelerate
     *  in a straight line  */
    LOC_IDL_CLIENT_DIAG_DR_ODO_CALIBRATION_NEEDED   = (1<<3),
    /** Indicate that gyro calibration is needed. Need to take more
     *  turns on level ground */
    LOC_IDL_CLIENT_DIAG_DR_GYRO_CALIBRATION_NEEDED  = (1<<4),
    /** Lot more turns on level ground needed */
    LOC_IDL_CLIENT_DIAG_DR_TURN_CALIBRATION_LOW  = (1<<5),
    /*8 Some more turns on level ground needed */
    LOC_IDL_CLIENT_DIAG_DR_TURN_CALIBRATION_MEDIUM  = (1<<6),
    /** Sufficient turns on level ground observed */
    LOC_IDL_CLIENT_DIAG_DR_TURN_CALIBRATION_HIGH  = (1<<7),
    /** Lot more accelerations in straight line needed */
    LOC_IDL_CLIENT_DIAG_DR_LINEAR_ACCEL_CALIBRATION_LOW  = (1<<8),
    /** Some more accelerations in straight line needed */
    LOC_IDL_CLIENT_DIAG_DR_LINEAR_ACCEL_CALIBRATION_MEDIUM  = (1<<9),
    /** Sufficient acceleration events in straight line observed */
    LOC_IDL_CLIENT_DIAG_DR_LINEAR_ACCEL_CALIBRATION_HIGH  = (1<<10),
    /** Lot more motion in straight line needed */
    LOC_IDL_CLIENT_DIAG_DR_LINEAR_MOTION_CALIBRATION_LOW  = (1<<11),
    /** Some more motion in straight line needed */
    LOC_IDL_CLIENT_DIAG_DR_LINEAR_MOTION_CALIBRATION_MEDIUM  = (1<<12),
    /** Sufficient motion events in straight line observed */
    LOC_IDL_CLIENT_DIAG_DR_LINEAR_MOTION_CALIBRATION_HIGH  = (1<<13),
    /** Lot more stationary events on level ground needed */
    LOC_IDL_CLIENT_DIAG_DR_STATIC_CALIBRATION_LOW  = (1<<14),
    /** Some more stationary events on level ground needed */
    LOC_IDL_CLIENT_DIAG_DR_STATIC_CALIBRATION_MEDIUM  = (1<<15),
    /** Sufficient stationary events on level ground observed */
    LOC_IDL_CLIENT_DIAG_DR_STATIC_CALIBRATION_HIGH  = (1<<16)
} locIdlClientDiagDrCalibrationStatusBits;

typedef uint64_t locIdlClientDiagInfoFlagMask;
typedef enum {
    /** valid altitude mean sea level */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT      = (1<<0),
    /** valid pdop, hdop, and vdop */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_DOP_BIT                          = (1<<1),
    /** valid magnetic deviation */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT           = (1<<2),
    /** valid horizontal reliability */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT              = (1<<3),
    /** valid vertical reliability */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_VER_RELIABILITY_BIT              = (1<<4),
    /** valid elipsode semi major */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT = (1<<5),
    /** valid elipsode semi minor */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MINOR_BIT = (1<<6),
    /** valid accuracy elipsode azimuth */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT    = (1<<7),
    /** valid gnss sv used in pos data */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT            = (1<<8),
    /** valid navSolutionMask */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT            = (1<<9),
    /** valid LocPosTechMask */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_POS_TECH_MASK_BIT                = (1<<10),
    /** valid LocSvInfoSource */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT               = (1<<11),
    /** valid position dynamics data */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT            = (1<<12),
    /** valid gdop, tdop */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EXT_DOP_BIT                      = (1<<13),
    /** valid North standard deviation */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_STD_DEV_BIT                = (1<<14),
    /** valid East standard deviation */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_STD_DEV_BIT                 = (1<<15),
    /** valid North Velocity */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_VEL_BIT                    = (1<<16),
    /** valid East Velocity */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_VEL_BIT                     = (1<<17),
    /** valid Up Velocity */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_UP_VEL_BIT                       = (1<<18),
    /** valid North Velocity Uncertainty */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NORTH_VEL_UNC_BIT                = (1<<19),
    /** valid East Velocity Uncertainty */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_EAST_VEL_UNC_BIT                 = (1<<20),
    /** valid Up Velocity Uncertainty */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_UP_VEL_UNC_BIT                   = (1<<21),
    /** valid leap seconds */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_LEAP_SECONDS_BIT                 = (1<<22),
    /** valid time uncertainty */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_TIME_UNC_BIT                     = (1<<23),
    /** valid numSvUsedInPosition */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_NUM_SV_USED_IN_POSITION_BIT      = (1<<24),
    /** valid calibrationConfidencePercent */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CALIBRATION_CONFIDENCE_PERCENT_BIT = (1<<25),
    /** valid calibrationStatus */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CALIBRATION_STATUS_BIT           = (1<<26),
    /** valid output engine type */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_OUTPUT_ENG_TYPE_BIT              = (1<<27),
    /** valid output engine mask */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_OUTPUT_ENG_MASK_BIT              = (1<<28),
    /** valid output conformityIndex */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CONFORMITY_INDEX_BIT             = (1<<29),
    /** valid llaVRPBased.  */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_LLA_VRP_BASED_BIT                = (1<<30),
    /** valid enuVelocityVRPBased. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_ENU_VELOCITY_VRP_BASED_BIT       = (1ULL<<31),
    /** valid drSolutionStatusMask. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_DR_SOLUTION_STATUS_MASK_BIT      = (1ULL<<32),
    /** valid altitudeAssumed  */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_ALTITUDE_ASSUMED_BIT             = (1ULL<<33),
    /** valid sessionStatus  */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_SESSION_STATUS_BIT               = (1ULL<<34),
    /** integrity risk used */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_INTEGRITY_RISK_USED_BIT          = (1ULL<<35),
    /** along-track protection level */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_ALONG_TRACK_BIT          = (1ULL<<36),
    /** Cross-track protection level */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_CROSS_TRACK_BIT          = (1ULL<<37),
    /** vertical component protection level */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_PROTECT_VERTICAL_BIT             = (1ULL<<38),
    /** GnssLocation has valid GnssLocation::dgnssStationId */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_DGNSS_STATION_ID_BIT             = (1ULL<<39),
    /** GnssLocation has valid GnssLocation::elapsedgPTPTime */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_GPTP_TIME_BIT                    = (1ULL<<40),
    /** GnssLocation has valid GnssLocation::elapsedgPTPTimeUnc */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_GPTP_TIME_UNC_BIT               = (1ULL<<41),
    /** GnssLocation has valid IDLLocationReport::baseLineLength. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_BASE_LINE_LENGTH_BIT               = (1ULL<<42),
    /** GnssLocation has valid IDLLocationReport::ageMsecOfCorrections. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_AGE_OF_CORRECTION_BIT              = (1ULL<<43),
    /** IDLLocationReport has valid IDLLocationReport::currReportingRate. */
    LOC_IDL_CLIENT_DIAG_GNSS_LOCATION_INFO_CURR_REPORT_RATE_BIT               = (1ULL<<44)
} locIdlClientDiagInfoFlagBits;

typedef enum {
    LOC_IDL_CLIENT_DIAG_LOCATION_RELIABILITY_NOT_SET = 0,
    LOC_IDL_CLIENT_DIAG_LOCATION_RELIABILITY_VERY_LOW,
    LOC_IDL_CLIENT_DIAG_LOCATION_RELIABILITY_LOW,
    LOC_IDL_CLIENT_DIAG_LOCATION_RELIABILITY_MEDIUM,
    LOC_IDL_CLIENT_DIAG_LOCATION_RELIABILITY_HIGH,
} locIdlClientDiagLocationReliability;

typedef PACKED struct PACKED_POST {
    uint64_t gpsSvUsedIdsMask;
    uint64_t gloSvUsedIdsMask;
    uint64_t galSvUsedIdsMask;
    uint64_t bdsSvUsedIdsMask;
    uint64_t qzssSvUsedIdsMask;
    uint64_t navicSvUsedIdsMask;
} locIdlClientDiagSvUsedInPosition;

typedef uint32_t locIdlClientDiagNavSolutionMask;
typedef enum {
    /** SBAS ionospheric correction is used*/
    LOC_IDL_CLIENT_DIAG_LOCATION_SBAS_CORRECTION_IONO_BIT  = (1<<0),
    /** SBAS fast correction is used*/
    LOC_IDL_CLIENT_DIAG_LOCATION_SBAS_CORRECTION_FAST_BIT  = (1<<1),
    /** SBAS long-tem correction is used*/
    LOC_IDL_CLIENT_DIAG_LOCATION_SBAS_CORRECTION_LONG_BIT  = (1<<2),
    /** SBAS integrity information is used*/
    LOC_IDL_CLIENT_DIAG_LOCATION_SBAS_INTEGRITY_BIT        = (1<<3),
    /** Position Report is DGNSS corrected*/
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_DGNSS_BIT  = (1<<4),
    /** Position Report is RTK corrected*/
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_RTK_BIT    = (1<<5),
    /** Position Report is PPP corrected*/
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_PPP_BIT    = (1<<6),
    /** Posiiton Report is RTF fixed corrected */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_RTK_FIXED_BIT  = (1<<7),
    /** Position report is computed with only SBAS corrected SVs */
    LOC_IDL_CLIENT_DIAG_LOCATION_NAV_CORRECTION_ONLY_SBAS_CORRECTED_SV_USED_BIT = (1<<8)
} locIdlClientDiagNavSolutionBits;

typedef uint32_t locIdlClientDiagPosTechMask;
typedef enum {
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_DEFAULT_BIT                  = 0,
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_SATELLITE_BIT                = (1<<0),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_CELLID_BIT                   = (1<<1),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_WIFI_BIT                     = (1<<2),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_SENSORS_BIT                  = (1<<3),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_REFERENCE_LOCATION_BIT       = (1<<4),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_INJECTED_COARSE_POSITION_BIT = (1<<5),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_AFLT_BIT                     = (1<<6),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_HYBRID_BIT                   = (1<<7),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_PPE_BIT                      = (1<<8),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_VEH_BIT                      = (1<<9),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_VIS_BIT                      = (1<<10),
    LOC_IDL_CLIENT_DIAG_LOCATION_POS_TECH_PROPAGATED_BIT               = (1<<11)
} locIdlClientDiagPosTechBits;

typedef enum {
    /** This is the propagated/aggregated reports from all engines
        running on the system (e.g.: DR/SPE/PPE). */
    LOC_IDL_CLIENT_DIAG_LOC_OUTPUT_ENGINE_FUSED = 0,
    /** This fix is the unmodified fix from modem GNSS engine */
    LOC_IDL_CLIENT_DIAG_LOC_OUTPUT_ENGINE_SPE   = 1,
    /** This is the unmodified fix from PPP/RTK correction engine */
    LOC_IDL_CLIENT_DIAG_LOC_OUTPUT_ENGINE_PPE   = 2,
    /** This is unmodified fix from VP Positioning Engine */
    LOC_IDL_CLIENT_DIAG_LOC_OUTPUT_ENGINE_VPE   = 3,
    LOC_IDL_CLIENT_DIAG_LOC_OUTPUT_ENGINE_COUNT,
} locIdlClientDiagLocOutputEngineType;

typedef uint32_t locIdlClientDiagPositioningEngineMask;
typedef enum {
    LOC_IDL_CLIENT_DIAG_STANDARD_POSITIONING_ENGINE = (1 << 0),
    LOC_IDL_CLIENT_DIAG_DEAD_RECKONING_ENGINE       = (1 << 1),
    LOC_IDL_CLIENT_DIAG_PRECISE_POSITIONING_ENGINE  = (1 << 2),
    LOC_IDL_CLIENT_DIAG_VP_POSITIONING_ENGINE       = (1 << 3)
} locIdlClientDiagPositioningEngineBits;

/** Specify various status that contributes to the DR poisition
 *  engine. <br/> */
typedef uint64_t locIdlClientDiagDrSolutionStatusMask;
typedef enum {
    /** Vehicle sensor speed input was detected by the DR position
     *  engine. <br/> */
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_VEHICLE_SENSOR_SPEED_INPUT_DETECTED = (1<<0),
    /** Vehicle sensor speed input was used by the DR position
     *  engine. <br/> */
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_VEHICLE_SENSOR_SPEED_INPUT_USED     = (1<<1),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_UNCALIBRATED                = (1<<2),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_GNSS_QUALITY_INSUFFICIENT   = (1<<3),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_FERRY_DETECTED              = (1<<4),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_6DOF_SENSOR_UNAVAILABLE       = (1<<5),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_VEHICLE_SPEED_UNAVAILABLE     = (1<<6),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_GNSS_EPH_UNAVAILABLE          = (1<<7),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_GNSS_MEAS_UNAVAILABLE         = (1<<8),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_INIT_POSITION_INVALID       = (1<<9),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_INIT_POSITION_UNRELIABLE    = (1<<10),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_POSITON_UNRELIABLE          = (1<<11),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_ERROR_GENERIC                       = (1<<12),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_SENSOR_TEMP_OUT_OF_RANGE    = (1<<13),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_USER_DYNAMICS_INSUFFICIENT  = (1<<14),
    LOC_IDL_CLIENT_DIAG_DR_SOLUTION_STATUS_WARNING_FACTORY_DATA_INCONSISTENT   = (1<<15)
} locIdlClientDiagDrSolutionStatusBits;

typedef PACKED struct PACKED_POST {
    /**  Latitude, in unit of degrees, range [-90.0, 90.0]. */
    double latitude;

    /**  Longitude, in unit of degrees, range [-180.0, 180.0]. */
    double longitude;

    /** Altitude above the WGS 84 reference ellipsoid, in unit
    of meters.  */
    float altitude;
} locIdlClientDiagLLAInfo;

typedef uint64_t locIdlClientDiagLocationCapabilitiesMask;
typedef enum {
    /** LocationClientApi can support time-based tracking session
     *  via LocationClientApi::startPositionSession(uint32_t,
     *  LocReqEngineTypeMask, const EngineReportCbs&, ResponseCb)
     *  and LocationClientApi::startPositionSession(uint32_t, const
     *  GnssReportCbs&, ResponseCb) and
     *  LocationClientApi::startPositionSession(uint32_t, uint32_t,
     *  LocationCb, ResponseCb) with distanceInMeters set to 0.
     *  <br/> */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_TIME_BASED_TRACKING_BIT           = (1<<0),
    /** LocationClientApi can support time-based batching session
     *  via LocationClientApi::startRoutineBatchingSession() with
     *  minInterval specified. <br/> */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_TIME_BASED_BATCHING_BIT           = (1<<1),
    /** LocationClientApi can support distance based tracking
     *  session via
     *  LocationClientApi::startPositionSession(uint32_t, uint32_t,
     *  LocationCb, ResponseCb) with distanceInMeters specified.
     *  <br/> */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_DISTANCE_BASED_TRACKING_BIT       = (1<<2),
    /** LocationClientApi can support distance-based batching via
     *  LocationClientApi::startRoutineBatchingSession() with
     *  minDistance set to none-zero value. <br/> */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_DISTANCE_BASED_BATCHING_BIT       = (1<<3),
    /** LocationClientApi can support geo fence via
     *  LocationClientApi::addGeofences(). <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_GEOFENCE_BIT                      = (1<<4),
    /** LocationClientApi can support trip batching via
     *  LocationClientApi::startTripBatchingSession(). <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_OUTDOOR_TRIP_BATCHING_BIT         = (1<<5),
    /** LocationClientApi can support receiving GnssMeasurements
     *  data in GnssMeasurementsCb when LocationClientApi is in
     *  a positioning session.. <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_GNSS_MEASUREMENTS_BIT             = (1<<6),
    /** LocationIntegrationApi can support configure constellations
     *  via LocationIntegrationApi::configConstellations.  <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_CONSTELLATION_ENABLEMENT_BIT      = (1<<7),
    /** Modem supports Carrier Phase for Precise Positioning
     *  Measurement Engine (PPME).
     *  This is a Standalone Feature.  <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_CARRIER_PHASE_BIT                 = (1<<8),
    /** Modem supports SV Polynomial for tightly coupled
     *  external DR support.
     *  This is a Standalone Feature.  <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_SV_POLYNOMIAL_BIT                 = (1<<9),
    /** Modem supports GNSS Single Frequency feature.
     *  This is a Standalone Feature.  <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_QWES_GNSS_SINGLE_FREQUENCY        = (1<<10),
    /** Modem supports GNSS Multi Frequency feature. Multi
     *  Frequency enables Single frequency also.  <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_QWES_GNSS_MULTI_FREQUENCY         = (1<<11),
    /** This mask indicates VEPP license bundle is enabled.
     *  VEPP bundle include Carrier Phase and SV Polynomial
     *  features.  <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_QWES_VPE                          = (1<<12),
    /** This mask indicates support for CV2X Location basic
     *  features. This bundle includes features for GTS Time
     *  & Freq, C-TUNC (Constrained Time uncertainity.
     *  LocationIntegrationApi can support setting of C-TUNC
     *  via configConstrainedTimeUncertainty. <br/> */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_QWES_CV2X_LOCATION_BASIC          = (1<<13),
    /** This mask indicates support for CV2X Location premium
     *  features. This bundle includes features for CV2X Location
     *  Basic features, QDR3 feature, and PACE. (Position
     *  Assisted Clock Estimator.
     *  LocationIntegrationApi can support setting of C-TUNC
     *  via configPositionAssistedClockEstimator. <br/> */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_QWES_CV2X_LOCATION_PREMIUM        = (1<<14),
    /** This mask indicates that PPE (Precise Positioning Engine)
     *  library is enabled or Precise Positioning Framework (PPF)
     *  is available. This bundle includes features for Carrier
     *  Phase and SV Ephermeris.  <br/>   */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_QWES_PPE                          = (1<<15),
    /** This mask indicates QDR2_C license bundle is enabled.
     *  This bundle includes features for SV Polynomial. <br/> */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_QWES_QDR2                         = (1<<16),
    /** This mask indicates QDR3_C license bundle is enabled.
     *  This bundle includes features for SV Polynomial. <br/> */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_QWES_QDR3                         = (1<<17),
    /** This mask indicates QWES license for ML inference of Pseudo Range meas is present <br/> */
    LOC_IDL_CLIENT_DIAG_LOCATION_CAPS_NLOS_ML20                         = (1<<18),

} locIdlClientDiagLocationCapabilitiesBits;

enum locIdlClientDiagLocSessionStatus {
    LOC_IDL_CLIENT_DIAG_LOC_SESS_SUCCESS      = 0,
    LOC_IDL_CLIENT_DIAG_LOC_SESS_INTERMEDIATE = 1,
    LOC_IDL_CLIENT_DIAG_LOC_SESS_FAILURE      = 2,
};

// Specify the LCA API type
// Used by LCA diag location report to indicate the
// API that is called to generate the location report
enum locIdlClientDiagLocReportTriggerType {
    LOC_IDL_CLIENT_DIAG_LOC_REPORT_TRIGGER_UNSPECIFIED               = 0,
    LOC_IDL_CLIENT_DIAG_LOC_REPORT_TRIGGER_SIMPLE_TRACKING_SESSION   = 1,
    LOC_IDL_CLIENT_DIAG_LOC_REPORT_TRIGGER_DETAILED_TRACKING_SESSION = 2,
    LOC_IDL_CLIENT_DIAG_LOC_REPORT_TRIGGER_ENGINE_TRACKING_SESSION   = 3,
    LOC_IDL_CLIENT_DIAG_LOC_REPORT_TRIGGER_SINGLE_TERRESTRIAL_FIX    = 4,
    LOC_IDL_CLIENT_DIAG_LOC_REPORT_TRIGGER_SINGLE_FIX                = 5,
    LOC_IDL_CLIENT_DIAG_LOC_REPORT_TRIGGER_TRIP_BATCHING_SESSION     = 6,
    LOC_IDL_CLIENT_DIAG_LOC_REPORT_TRIGGER_ROUTINE_BATCHING_SESSION  = 7,
    LOC_IDL_CLIENT_DIAG_LOC_REPORT_TRIGGER_GEOFENCE_SESSION          = 8,
};

typedef PACKED struct PACKED_POST {
    /** bitwise OR of LocationFlagsBits to mark which params are valid */
    locIdlClientDiagLocationFlagsMask flags;
    /** UTC timestamp for location fix, milliseconds since January 1, 1970 */
    uint64_t timestamp;
    /** in degrees; range [-90.0, 90.0] */
    double latitude;
    /** in degrees; range [-180.0, 180.0] */
    double longitude;
    /** in meters above the WGS 84 reference ellipsoid */
    double altitude;
    /** in meters per second */
    float speed;
    /** in degrees; range [0, 360) */
    float bearing;
    /** in meters */
    float horizontalAccuracy;
    /** in meters */
    float verticalAccuracy;
    /** in meters/second */
    float speedAccuracy;
    /** in degrees (0 to 359.999) */
    float bearingAccuracy;
    locIdlClientDiagLocationTechnologyMask techMask;
    /** bitwise OR of GnssLocationInfoBits for param validity */
    locIdlClientDiagInfoFlagMask gnssInfoFlags;
    /** altitude wrt mean sea level */
    float altitudeMeanSeaLevel;
    /** position dilusion of precision */
    float pdop;
    /** horizontal dilusion of precision */
    float hdop;
    /** vertical dilusion of precision */
    float vdop;
    /** geometric  dilution of precision */
    float gdop;
    /** time dilution of precision */
    float tdop;
    /** magnetic deviation */
    float magneticDeviation;
    /** horizontal reliability */
    locIdlClientDiagLocationReliability horReliability;
    /** vertical reliability */
    locIdlClientDiagLocationReliability verReliability;
    /** horizontal elliptical accuracy semi-major axis */
    float horUncEllipseSemiMajor;
    /** horizontal elliptical accuracy semi-minor axis */
    float horUncEllipseSemiMinor;
    /** horizontal elliptical accuracy azimuth */
    float horUncEllipseOrientAzimuth;
    /** North standard deviation Unit: Meters */
    float northStdDeviation;
    /** East standard deviation. Unit: Meters */
    float eastStdDeviation;
    /** North Velocity.Unit: Meters/sec */
    float northVelocity;
    /** East Velocity  Unit: Meters/sec */
    float eastVelocity;
    /** Up Velocity  Unit: Meters/sec */
    float upVelocity;
    float northVelocityStdDeviation;
    float eastVelocityStdDeviation;
    float upVelocityStdDeviation;
    /** Gnss sv used in position data */
    locIdlClientDiagSvUsedInPosition svUsedInPosition;
    /** Nav solution mask to indicate sbas corrections */
    locIdlClientDiagNavSolutionMask  navSolutionMask;
    /** Position technology used in computing this fix */
    locIdlClientDiagPosTechMask      posTechMask;
    /** Body Frame Dynamics: 4wayAcceleration and pitch set with validity */
    locIdlClientDiagPositionDynamics bodyFrameData;
    /**  GNSS System Time */
    locIdlClientDiagGnssSystemTime               gnssSystemTime;
    /** Number of measurements received for use in fix */
    uint8_t                                numOfMeasReceived;
    /** Gnss measurement usage info  */
    locIdlClientDiagGnssMeasUsageInfo measUsageInfo[LOC_IDL_CLIENT_DIAG_GNSS_SV_MAX];
    /** Leap Seconds */
    uint8_t leapSeconds;
    /** Time uncertainty in milliseconds   */
    float timeUncMs;
    /** number of SV used in position report */
    uint16_t numSvUsedInPosition;
    /** Sensor calibration confidence percent, valid range [0,
     *  100] */
    uint8_t calibrationConfidencePercent;
    /** Sensor calibration status */
    locIdlClientDiagDrCalibrationStatusMask calibrationStatus;
    /** boot timestamp in nano-second when this diag log packet is
     *  created and filled with the info at location client api
     *  layer */
    uint64_t bootTimestampNs;
    /** location engine type. When the fix. when the type is set to
        LOC_ENGINE_SRC_FUSED, the fix is the propagated/aggregated
        reports from all engines running on the system (e.g.:
        DR/SPE/PPE). To check which location engine contributes to
        the fused output, check for locOutputEngMask. */
    locIdlClientDiagLocOutputEngineType locOutputEngType;
    /** when loc output eng type is set to fused, this field
        indicates the set of engines contribute to the fix. */
    locIdlClientDiagPositioningEngineMask locOutputEngMask;
    /** When robust location is enabled, this field
     * will indicate how well the various input data considered for
     * navigation solution conform to expectations.
     * Range: [0.0, 1.0], with 0.0 for least conforming and 1.0 for
     * most conforming. */
    float conformityIndex;
    /** VRR-based latitude/longitude/altitude. */
    locIdlClientDiagLLAInfo llaVRPBased;
    /** VRR-based east, north, and up velocity */
    float enuVelocityVRPBased[3];
    /** DR fix status mask */
    locIdlClientDiagDrSolutionStatusMask drSolutionStatusMask;
    /** When this field is valid, it will indicates whether altitude
     *  is assumed or calculated.
     *  false: Altitude is calculated.
     *  true:  Altitude is assumed; there may not be enough
     *         satellites to determine the precise altitude. */
    bool altitudeAssumed;
    locIdlClientDiagLocationCapabilitiesMask capabilitiesMask;
    /** session status */
    locIdlClientDiagLocSessionStatus sessionStatus;
    /** integrity risk used for protection level parameters. */
    uint32_t integrityRiskUsed;
    /** along-track protection level */
    float    protectAlongTrack;
    /** cross-track protection level */
    float    protectCrossTrack;
    /** vertical component protection level */
    float    protectVertical;
    /** Boot timestamp corresponding to the UTC timestamp for
     *  location fix. */
    uint64_t elapsedRealTimeNs;
    /** Uncertainty for the boot timestamp. */
    uint64_t elapsedRealTimeUncNs;
    /** session start boot timestamp in ns */
    uint64_t sessionStartBootTimestampNs;
    /** lca req api type */
    locIdlClientDiagLocReportTriggerType reportTriggerType;
    /** number of dgnss station id that is valid in dgnssStationId */
    uint16_t  numOfDgnssStationId;
    /** dgnss station id */
    uint16_t dgnssStationId[LOC_IDL_CLIENT_DIAG_DGNSS_STATION_ID_MAX];
    /** PTP time corresponding to the UTC timestamp for
     *  location fix.
     *  In unit in nanoseconds <br/>
     *  This field may not always be available. Please check for the
     *  presence of IDL_LOC_INFO_GPTP_TIME_BIT in
     *  IDLLocationReport::locationInfoFlags before retrieving this field.   */
    uint64_t elapsedgPTPTime;
    uint64_t elapsedgPTPTimeUnc;
    /** Distance between the base station and the receiver
     *  Unit- meters */
    double baseLineLength;
    /** Difference in time between the fix timestamp using the
     *  correction and the time of the correction
     *  Unit - milli-seconds */
    double ageMsecOfCorrections;
    /** Current Reporting Rate of GNSS engine
     *  Unit - milli-seconds */
    double currReportingRate;
    /** Time difference between packet generation time
     *  and sending over SOMEIP
     *  Unit- milli-seconds */
    uint16_t reportingLatency;
} locIdlClientDiagPosition;

typedef uint32_t locIdlClientDiagGnssMeasurementsDataFlagsMask;
typedef enum {
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_ID_BIT = (1 << 0),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT = (1 << 1),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_STATE_BIT = (1 << 2),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_BIT = (1 << 3),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_UNCERTAINTY_BIT = (1 << 4),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT = (1 << 5),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT = (1 << 6),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT = (1 << 7),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT = (1 << 8),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_BIT = (1 << 9),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT = (1 << 10),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT = (1 << 11),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT = (1 << 12),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT = (1 << 13),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT = (1 << 14),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT = (1 << 15),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT = (1 << 16),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT = (1 << 17),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_GNSS_SIGNAL_TYPE_BIT       = (1 << 18),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_BASEBAND_CARRIER_TO_NOISE_BIT = (1<<19),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT                  = (1 << 20),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT      = (1 << 21)
} locIdlClientDiagGnssMeasurementsDataFlagsBits;

typedef uint32_t locIdlClientDiagGnssMeasurementsStateMask;
typedef enum {
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_UNKNOWN_BIT = 0,
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_CODE_LOCK_BIT = (1 << 0),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_BIT_SYNC_BIT = (1 << 1),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_SUBFRAME_SYNC_BIT = (1 << 2),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_TOW_DECODED_BIT = (1 << 3),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_MSEC_AMBIGUOUS_BIT = (1 << 4),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_SYMBOL_SYNC_BIT = (1 << 5),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GLO_STRING_SYNC_BIT = (1 << 6),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GLO_TOD_DECODED_BIT = (1 << 7),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_BDS_D2_BIT_SYNC_BIT = (1 << 8),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_BDS_D2_SUBFRAME_SYNC_BIT = (1 << 9),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GAL_E1BC_CODE_LOCK_BIT = (1 << 10),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GAL_E1C_2ND_CODE_LOCK_BIT = (1 << 11),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_GAL_E1B_PAGE_SYNC_BIT = (1 << 12),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_STATE_SBAS_SYNC_BIT = (1 << 13),
} locIdlClientDiagGnssMeasurementsStateBits;

typedef uint16_t locIdlClientDiagGnssMeasurementsAdrStateMask;
typedef enum {
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_UNKNOWN = 0,
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_VALID_BIT = (1 << 0),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_RESET_BIT = (1 << 1),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_CYCLE_SLIP_BIT = (1 << 2),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_HALF_CYCLE_RESOLVED_BIT = (1<<3),
} locIdlClientDiagGnssMeasurementsAdrStateBits;

typedef enum {
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_UNKNOWN = 0,
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_PRESENT,
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_NOT_PRESENT,
} locIdlClientDiagGnssMeasurementsMultipathIndicator;

typedef uint32_t locIdlClientDiagGnssMeasurementsClockFlagsMask;
typedef enum {
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_LEAP_SECOND_BIT = (1 << 0),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_BIT = (1 << 1),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_UNCERTAINTY_BIT = (1 << 2),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_FULL_BIAS_BIT = (1 << 3),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_BIT = (1 << 4),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_UNCERTAINTY_BIT = (1 << 5),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_BIT = (1 << 6),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_UNCERTAINTY_BIT = (1 << 7),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_HW_CLOCK_DISCONTINUITY_COUNT_BIT = (1 << 8),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_BIT             = (1 << 9),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_REAL_TIME_UNC_BIT         = (1 << 10),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_GPTP_TIME_BIT             = (1 << 11),
    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_CLOCK_FLAGS_ELAPSED_GPTP_TIME_UNC_BIT         = (1 << 12)
} locIdlClientDiagGnssMeasurementsClockFlagsBits;

typedef uint32_t GnssDiagDataMask;
typedef enum {
     // Jammer Indicator is available
     LOC_IDL_CLIENT_DIAG_GNSS_LOC_DATA_JAMMER_IND_BIT = (1ULL << 0),
     // AGC is available
     LOC_IDL_CLIENT_DIAG_GNSS_LOC_DATA_AGC_BIT = (1ULL << 1)
} GnssDiagDataBits;

typedef PACKED struct PACKED_POST {
    locIdlClientDiagGnssMeasurementsDataFlagsMask flags;  // bitwise OR of GnssMeasurementsDataFlagsBits
    int16_t svId;
    locIdlClientDiagGnssSvType svType;
    double timeOffsetNs;
    locIdlClientDiagGnssMeasurementsStateMask stateMask;  // bitwise OR of GnssMeasurementsStateBits
    int64_t receivedSvTimeNs;
    float receivedSvTimeSubNs;
    int64_t receivedSvTimeUncertaintyNs;
    double carrierToNoiseDbHz;
    double pseudorangeRateMps;
    double pseudorangeRateUncertaintyMps;
    locIdlClientDiagGnssMeasurementsAdrStateMask adrStateMask;
    double adrMeters;
    double adrUncertaintyMeters;
    float carrierFrequencyHz;
    int64_t carrierCycles;
    double carrierPhase;
    double carrierPhaseUncertainty;
    locIdlClientDiagGnssMeasurementsMultipathIndicator multipathIndicator;
    double signalToNoiseRatioDb;
    double agcLevelDb;
    double basebandCarrierToNoiseDbHz;
    locIdlClientDiagGnssSignalTypeMask gnssSignalType;
    double fullInterSignalBiasNs;
    double fullInterSignalBiasUncertaintyNs;
    /** Increments when a cycle slip is detected.  */
    uint8_t cycleSlipCount;
} locIdlClientDiagGnssMeasurementsData;

typedef PACKED struct PACKED_POST {
    // bitwise OR of GnssMeasurementsClockFlagsBits
    locIdlClientDiagGnssMeasurementsClockFlagsMask flags;
    int16_t leapSecond;
    int64_t timeNs;
    double timeUncertaintyNs;
    int64_t fullBiasNs;
    double biasNs;
    double biasUncertaintyNs;
    double driftNsps;
    double driftUncertaintyNsps;
    uint32_t hwClockDiscontinuityCount;
    /** elapsed time since boot, in unit of nonoseconds */
    uint64_t elapsedRealTime;
    /** uncertainty of elapsedRealTime, in unit of nonoseconds */
    uint64_t elapsedRealTimeUnc;
    /** gPTP since boot, in unit of nonoseconds */
    uint64_t elapsedgPTPTime;
    /** uncertainty of elapsedgPTPTime, in unit of nonoseconds */
    uint64_t elapsedgPTPTimeUnc;
} locIdlClientDiagGnssMeasurementsClock;

/* This structure is too large to be sent in one shot,
   therefore we segment using maxSequence and sequenceNumber
*/
typedef PACKED struct PACKED_POST {
    uint8_t maxSequence;
    uint8_t sequenceNumber;
    uint16_t reportingLatency;
    uint32_t count;        // number of items in GnssMeasurementsData array
    bool isNhz;            // NHz measurement indicator
    locIdlClientDiagGnssMeasurementsClock clock; // clock
    locIdlClientDiagGnssMeasurementsData measurements[LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ];
} locIdlClientDiagGnssMeas;

typedef PACKED struct PACKED_POST {
    uint32_t count;        // number of items in GnssMeasurementsData array
    uint16_t reportingLatency;
    bool isNhz;            // NHz measurement indicator
    locIdlClientDiagGnssMeasurementsClock clock; // clock
    locIdlClientDiagGnssMeasurementsData measurements[LOC_IDL_CLIENT_DIAG_GNSS_SV_MAX];
} locIdlClientDiagGnssMeasPacket;

typedef PACKED struct PACKED_POST {
    /** number of SVs in the GnssSvs array */
    uint32_t count;
    locIdlClientDiagGnssSvData gnssSvs[LOC_IDL_CLIENT_DIAG_GNSS_SV_MAX];
} locIdlClientDiagGnssSv;

typedef PACKED struct PACKED_POST {
/** Timestamp of NMEA generation */
    uint64_t timestamp;
/** Length of nmea data */
    uint32_t nmeaLength;
/** One or more nmea strings separated by \n charachter */
    uint8_t nmea[1];
} locIdlClientDiagGnssNmea;

typedef PACKED struct PACKED_POST {
    uint32_t size;                 // set to sizeof(GnssDataNotification)
    // bitwise OR of GnssDataBits
    GnssDiagDataMask  gnssDiagDataMask[LOC_IDL_CLIENT_DIAG_MAX_NUMBER_OF_SIGNAL_TYPES];
     // Jammer Indication
    double        jammerInd[LOC_IDL_CLIENT_DIAG_MAX_NUMBER_OF_SIGNAL_TYPES];
     // Automatic gain control
    double        agc[LOC_IDL_CLIENT_DIAG_MAX_NUMBER_OF_SIGNAL_TYPES];
} locIdlClientDiagGnssData;

typedef PACKED struct PACKED_POST {
    uint32_t interval;
    uint32_t mask;
} locIdlClientDiagStartReq;

typedef PACKED struct PACKED_POST {
    uint32_t delMask;
} locIdlClientDiagDelReq;

typedef enum {
    LOC_IDL_CLIENT_DIAG_POSITION_INFO = 1,
    LOC_IDL_CLIENT_DIAG_MEASUREMENT_INFO = 2,
    LOC_IDL_CLIENT_DIAG_SV_INFO = 3,
    LOC_IDL_CLIENT_DIAG_NMEA_INFO = 4,
    LOC_IDL_CLIENT_DIAG_GNSS_DATA_INFO = 5,
    LOC_IDL_CLIENT_DIAG_START_REQ = 6,
    LOC_IDL_CLIENT_DIAG_STOP_REQ = 7,
    LOC_IDL_CLIENT_DIAG_DEL_REQ = 8,
} locIdlClientMsgType;

/** @brief EP API Diag logger header for all messages. */
typedef PACKED struct PACKED_POST{
    /** Used by Logging Module */
    log_hdr_type z_LogHeader;
    /** Loc Idl Message Version */
    uint32_t u_Version;
    /** locIdlClientMsgType */
    uint8_t msgType;
    /** return value of gptpGetSyncStatus */
    bool ptpSyncStatus;
    /** Lateny in Transmitter end*/
    uint16_t lattencyInTxEndMsec;
    /** Lateny in Receiver end*/
    float lattencyInRxEndMsec;
    /**  size of packet it filled */
    uint32_t msgLength;
    /** Process identification */
    uint32_t u_Process_id;
    /** time at logging  (clock boot time)*/
    uint64_t t_TimeTickNsec;
    /** PTP time of  packet generation*/
    uint64_t t_TxTimePtpNsec;
    /** PTP time in Receiver End*/
    uint64_t t_RxTimePtpNsec;
} locIdlGenericHeader;

typedef PACKED struct PACKED_POST{
    locIdlGenericHeader header;
    locIdlClientDiagPosition location;
}locIdlClientDiagLocationRptLog;

typedef PACKED struct PACKED_POST{
    locIdlGenericHeader header;
    locIdlClientDiagGnssSv sv;
}locIdlClientDiagSvRptLog;

typedef PACKED struct PACKED_POST{
    locIdlGenericHeader header;
    locIdlClientDiagGnssMeas meas;
}locIdlClientDiagMeasRptLog;

typedef PACKED struct PACKED_POST{
    locIdlGenericHeader header;
    locIdlClientDiagGnssNmea nmea;
}locIdlClientDiagNmeaRptLog;

typedef PACKED struct PACKED_POST{
    locIdlGenericHeader header;
    locIdlClientDiagGnssData gnssData;
}locIdlClientDiagGnssDataRptLog;

typedef PACKED struct PACKED_POST{
    locIdlGenericHeader header;
    locIdlClientDiagStartReq start;
}locIdlClientDiagStartReqLog;

typedef PACKED struct PACKED_POST{
    locIdlGenericHeader header;
}locIdlClientDiagStopReqLog;

typedef PACKED struct PACKED_POST{
    locIdlGenericHeader header;
    locIdlClientDiagDelReq del;
}locIdlClientDiagDelReqLog;

class LocIdlclientDiag
{
public:
    LocDiagIface* mDiagIface;

    LocIdlclientDiag();

    void diagLogPosInfo
    (
        locIdlClientDiagPosition &gnssPos,
        uint64_t clk_bootTime,
        uint64_t gptp_time_ns,
        bool ptpSyncStatus,
        float latency
    );
    void diagLogSvInfo
    (
        locIdlClientDiagGnssSv &gnssSv,
        uint64_t clk_bootTime,
        uint64_t gptp_time_ns,
        bool ptpSyncStatus
    );
    void diagLogMeasInfo
    (
        locIdlClientDiagGnssMeasPacket &gnssMeas,
        uint64_t clk_bootTime,
        uint64_t gptp_time_ns,
        bool ptpSyncStatus
    );
    void diagLogNmeaInfo
    (
        const uint64_t timestamp,
        const string nmea,
        uint64_t clk_bootTime,
        uint64_t gptp_time_ns,
        bool ptpSyncStatus
    );
    void diagLogGnssDataInfo
    (
        locIdlClientDiagGnssData &gnssData,
        uint64_t clk_bootTime,
        uint64_t gptp_time_ns,
        bool ptpSyncStatus
    );
    void diagLogGnssStartReq
    (
        locIdlClientDiagStartReq &startReq,
        uint64_t clk_bootTime,
        uint64_t gptp_time_ns,
        bool ptpSyncStatus
    );
    void diagLogGnssStopReq
    (
        uint64_t clk_bootTime,
        uint64_t gptp_time_ns,
        bool ptpSyncStatus
    );
    void diagLogGnssDelReq
    (
        locIdlClientDiagDelReq &DelReq,
        uint64_t clk_bootTime,
        uint64_t gptp_time_ns,
        bool ptpSyncStatus
    );
private:
    void fillGenericHeader(locIdlGenericHeader &header,
                                uint8_t u_Version,
                                uint8_t msgType,
                                uint32_t msgLength,
                                uint16_t lattencyInTxEndMsec,
                                uint64_t t_TxTimePtpNsec,
                                uint64_t clk_bootTime,
                                uint64_t gptp_time_ns,
                                bool ptpSyncStatus,
                                float latency
    );
    void populateDiagGnssSv(locIdlClientDiagGnssSv &dst, locIdlClientDiagGnssSv &src);
    void populateDiagGnssPositon(locIdlClientDiagPosition &dst, locIdlClientDiagPosition &src);
    void populateDiagGnssMeas(
            locIdlClientDiagGnssMeas &dst,
            locIdlClientDiagGnssMeasPacket &src);
};

}

#endif //LOC_IDL_DIAG_LOG_TYPE_H
