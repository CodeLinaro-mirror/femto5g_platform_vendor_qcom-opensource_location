/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#ifndef LOC_APIV02_UTILS_H
#define LOC_APIV02_UTILS_H

#ifdef USE_GLIB
/**************************** REVISION NUMBER *********************************
GNSS Extended file version is defined using major and minor version number.
A Major version could be associated with non-backward compatible update.
A change to only Minor version is generally backward compatible to
header file having the same Major version number.

Note: Client shall verify version number for compatibility.
***************************** REVISION NUMBER ********************************/

#define GNSS_EXTENDED_MAJOR_REVISION ((uint8_t)1)
#define GNSS_EXTENDED_MINOR_REVISION ((uint8_t)0)

#if defined (__GNUC__) || defined (__linux__)|| defined (USE_GLIB) || defined (__ANDROID__)
#define PACKED
#define PACKED_POST    __attribute__((__packed__))
#else
#define PACKED
#define PACKED_POST
#endif /* defined (__GNUC__) || defined (__linux__)|| defined (USE_GLIB) || defined (__ANDROID__) */

/******************************************************************************
************************** Position Report Data *******************************
******************************************************************************/
typedef PACKED struct PACKED_POST
{
    /* GPS position report validity flags */
    uint64_t b_IsValid : 1;        /* Solution is/is not valid */
    uint64_t b_IsItarViolated : 1; /* Solution violates ITAR regulations */
    uint64_t b_IsGpsTimeValid : 1; /* GPS Time (week and Msec valid or not) */
    uint64_t b_IsPuncValid : 1;    /* Position uncertainties valid or not */
    uint64_t b_IsTuncValid : 1;    /* Time uncertainties valid or not */
    uint64_t b_IsAltValid : 1;     /* Altitude valid or not */

    /* Technology used in computing the position fix flags */
    uint64_t b_Reserved1  : 1;         /* Reserved bit */
    uint64_t b_Reserved2  : 1;         /* Reserved bit */
    uint64_t b_Reserved3  : 1;         /* Reserved bit */
    uint64_t b_Reserved4  : 1;         /* Reserved bit */
    uint64_t b_Reserved5  : 1;         /* Reserved bit */
    uint64_t b_Reserved6  : 1;         /* Reserved bit */
    uint64_t b_Reserved7  : 1;         /* Reserved bit */
    uint64_t b_Reserved8  : 1;         /* Reserved bit */
    uint64_t b_Reserved9  : 1;         /* Reserved bit */
    uint64_t b_Reserved10 : 1;         /* Reserved bit */
    uint64_t b_Reserved11 : 1;         /* Reserved bit */
    uint64_t b_Reserved12 : 1;         /* Reserved bit */
    uint64_t : 46;  /* Padding to align with 8 byte boundary */
} GnssExtended_NavPosFlags;


/* Navigation solution position flags. Each field is binary. The TRUE/FALSE
state is described below. */
typedef PACKED struct PACKED_POST
{
    uint64_t b_IsValid : 1;       /* Solution is/is not valid */
    uint64_t b_IsPropagated : 1;  /* Solution is/is not propagated */
    uint64_t b_Reserved1 : 1;         /* Reserved bit */
    uint64_t b_Reserved2 : 1;         /* Reserved bit */
    uint64_t b_Reserved3 : 1;         /* Reserved bit */
    uint64_t b_Reserved4 : 1;         /* Reserved bit */
    uint64_t b_Reserved5 : 1;         /* Reserved bit */
    uint64_t b_Reserved6 : 1;         /* Reserved bit */
    uint64_t b_Reserved7 : 1;         /* Reserved bit */
    uint64_t b_Reserved8 : 1;         /* Reserved bit */
    uint64_t b_IsGpsUsed : 1;     /* GPS is/is not used in solution */
    uint64_t b_IsGloUsed : 1;     /* GLO is/is not used in solution */
    uint64_t b_IsBdsUsed : 1;     /* BDS is/is not used in solution */
    uint64_t b_IsGalUsed : 1;     /* GAL is/is not used in solution */
    uint64_t b_Reserved9 : 1;         /* Reserved bit */
    uint64_t b_Reserved10 : 1;         /* Reserved bit */
    uint64_t b_Reserved11 : 1;         /* Reserved bit */
    uint64_t b_Reserved12 : 1;         /* Reserved bit */
    uint64_t b_Reserved13 : 1;         /* Reserved bit */
    uint64_t b_Reserved14 : 1;         /* Reserved bit */
    uint64_t b_Reserved15 : 1;         /* Reserved bit */
    uint64_t b_IsFinal : 1;       /* Solution is/is not final */
    uint64_t b_Reserved16 : 1;         /* Reserved bit */
    uint64_t b_Reserved17 : 1;         /* Reserved bit */
    uint64_t b_Reserved18 : 1;         /* Reserved bit */
    uint64_t b_Reserved19 : 1;         /* Reserved bit */
    uint64_t b_Reserved20 : 1;         /* Reserved bit */
    uint64_t b_Reserved21 : 1;         /* Reserved bit */
    uint64_t b_Reserved22 : 1;         /* Reserved bit */
    uint64_t b_Reserved23 : 1;         /* Reserved bit */
    uint64_t b_Reserved24 : 1;         /* Reserved bit */
    uint64_t b_Reserved25 : 1;         /* Reserved bit */
    uint64_t b_IsHeadingValid : 1;   /* Heading is/is not valid */
    uint64_t b_IsVelValid : 1;    /* Velocity is/is not valid */
    uint64_t b_Reserved26 : 1;         /* Reserved bit */
    uint64_t b_Reserved27 : 1;         /* Reserved bit */
    uint64_t b_IsStatic : 1;      /* Solution is/is not in KF static mode */
    uint64_t b_Reserved28 : 1;         /* Reserved bit */
    uint64_t b_Reserved29 : 1;         /* Reserved bit */
    uint64_t b_Reserved30 : 1;         /* Reserved bit */
    uint64_t b_Reserved31 : 1;         /* Reserved bit */
    uint64_t b_Reserved32 : 1;         /* Reserved bit */
    uint64_t b_Reserved33 : 1;         /* Reserved bit */
    uint64_t b_Reserved34 : 1;         /* Reserved bit */
    uint64_t b_Reserved35 : 1;         /* Reserved bit */
    uint64_t b_Reserved36 : 1;         /* Reserved bit */
    uint64_t b_Reserved37 : 1;         /* Reserved bit */
    uint64_t b_Reserved38 : 1;         /* Reserved bit */
    uint64_t b_Reserved39 : 1;         /* Reserved bit */
    uint64_t b_Reserved40 : 1;         /* Reserved bit */
    uint64_t b_Reserved41 : 1;         /* Reserved bit */
    uint64_t b_Reserved42 : 1;         /* Reserved bit */
    uint64_t b_Reserved43 : 1;         /* Reserved bit */
    uint64_t b_Reserved44 : 1;         /* Reserved bit */
    uint64_t b_Reserved45 : 1;         /* Reserved bit */
    uint64_t b_Reserved46 : 1;         /* Reserved bit */
    uint64_t b_Reserved47 : 1;         /* Reserved bit */
    uint64_t b_Reserved48 : 1;         /* Reserved bit */
    uint64_t b_Reserved49 : 1;         /* Reserved bit */
    uint64_t b_Reserved50 : 1;         /* Reserved bit */
    uint64_t b_Reserved51 : 1;         /* Reserved bit */
    uint64_t b_Reserved52 : 1;         /* Reserved bit */
    uint64_t b_Reserved53 : 1;         /* Reserved bit */
    uint64_t b_Reserved54 : 1;         /* Reserved bit */
    uint64_t b_Reserved55 : 1;         /* Reserved bit */
    uint64_t b_Reserved56 : 1;         /* Reserved bit */
    uint64_t b_IsQzssUsed : 1;     /* QZSS is/is not used in solution */
    uint64_t b_Reserved57 : 1;         /* Reserved bit */
    uint64_t b_Reserved58 : 1;         /* Reserved bit */
    uint64_t b_Reserved59 : 1;         /* Reserved bit */
    uint64_t b_Reserved60 : 1;         /* Reserved bit */
    uint64_t b_Reserved61 : 1;         /* Reserved bit */
    uint64_t b_Reserved62 : 1;         /* Reserved bit */
    uint64_t b_Reserved63 : 1;         /* Reserved bit */
    uint64_t b_Reserved64 : 1;         /* Reserved bit */
    uint64_t b_Reserved65 : 1;         /* Reserved bit */
    uint64_t b_Reserved66 : 1;         /* Reserved bit */
    uint64_t b_Reserved67 : 1;         /* Reserved bit */
    uint64_t b_Reserved68 : 1;         /* Reserved bit */
    uint64_t b_Reserved69 : 1;         /* Reserved bit */
    uint64_t b_Reserved70 : 1;         /* Reserved bit */
    uint64_t b_Reserved71 : 1;         /* Reserved bit */
    uint64_t b_Reserved72 : 1;         /* Reserved bit */
    uint64_t b_Reserved73 : 1;         /* Reserved bit */
    uint64_t b_Reserved74 : 1;         /* Reserved bit */
    uint64_t b_Reserved75 : 1;         /* Reserved bit */
    uint64_t b_Reserved76 : 1;         /* Reserved bit */
    uint64_t b_Reserved77 : 1;         /* Reserved bit */
    uint64_t b_Reserved78 : 1;         /* Reserved bit */
    uint64_t b_Reserved79 : 1;         /* Reserved bit */
    uint64_t b_Reserved80 : 1;         /* Reserved bit */
    uint64_t b_Reserved81 : 1;         /* Reserved bit */
    uint64_t b_Reserved82 : 1;         /* Reserved bit */
    uint64_t b_Reserved83 : 1;         /* Reserved bit */
    uint64_t b_Reserved84 : 1;         /* Reserved bit */
    uint64_t b_Reserved85 : 1;         /* Reserved bit */
    uint64_t b_Reserved86 : 1;         /* Reserved bit */
    uint64_t b_Reserved87 : 1;         /* Reserved bit */
    uint64_t b_Reserved88 : 1;         /* Reserved bit */
    uint64_t b_Reserved89 : 1;         /* Reserved bit */
    uint64_t b_Reserved90 : 1;         /* Reserved bit */
    uint64_t b_Reserved91 : 1;         /* Reserved bit */
    uint64_t b_Reserved92 : 1;         /* Reserved bit */
    uint64_t b_Reserved93 : 1;         /* Reserved bit */
    uint64_t : 24;  /* Padding to align with 8 byte boundary */
} GnssExtended_NavSlnFlagsStructType;

/**   struct GnssExtended_NavPositionStructType describes the current PVT */
typedef PACKED struct PACKED_POST
{
    /* Bit flags */
    GnssExtended_NavSlnFlagsStructType z_PosFlags;

    /* Additional bit flags specific to SM */
    GnssExtended_NavPosFlags             z_SmPosFlags;

    /* GNSS Fix Time Information */
    /* GPS week number of fix
      (Current GPS week as calculated from midnight, Jan. 6, 1980)*/
    uint16_t  w_GpsWeek;
    /* GPS time in week (milliseconds) of fix
      (Amount of time into the current GPS week) */
    uint32_t  q_GpsTimeMs;
    /* Receiver's ms counter value */
    uint32_t  q_RefFCount;
    /* Lat (radians), Long (radians), Alt (meters)*/
    double    d_PosLla[3];
    /* East, North, Up velocity (m/s) */
    float    f_VelEnu[3];
    /* Heading angle, [0,2*pi] (rad). Referenced to North (0 rad) */
    float    f_HeadingRad;
    /* Heading uncertainty (rad) */
    float    f_HeadingUncRad;
    /* Position (Lat, Long) uncertainty (m) */
    float    f_PuncLatLonMeters[2];
    /* Vertical position uncertainty (m) */
    float    f_PuncVertMeters;
    /* East, North velocity uncertainty (m/s) */
    float    f_VuncEastNorthMps[2];
    /* Vertical velocity uncertainty (m/s) */
    float    f_VuncVertMps;
    /* Receiver's clock bias (m) */
    float    f_ClockBias;
    /* Receiver's clock uncertainty (m) */
    float    f_ClockBiasUncMs;
    /* Receiver's clock drift (m/s) */
    float    f_ClockDriftRate;
    /* Receiver's clock frequency bias uncertainty (m/s) */
    float    f_ClockDriftRateUncMps;
    /* Position Dilution of Precision (unitless) */
    float    f_PDOP;
} GnssExtended_NavPositionStructType; /* position-related info */


#define GNSS_EXTENDED_N_ACTIVE_MAX (112)
/* If set, GNSS SV measurement is valid (detected by searcher) */
#define GNSS_EXTENDED_SV_INFO_MEAS_VALID       0x00000001
/* If set, GNSS SV is usable: {GPS|BDS} PRNs or {GLO} Slot IDs */
#define GNSS_EXTENDED_SV_INFO_USABLE           0x00000002
/* If set, GNSS SV is used for pos fix:   {GPS|BDS} PRNs or {GLO} Slot IDs */
#define GNSS_EXTENDED_SV_INFO_USED_IN_POS_FIX  0x00000004
/* If set, GNSS SV is used for velocity fix: {GPS|BDS} PRNs or {GLO} Slot IDs */
#define GNSS_EXTENDED_SV_INFO_USED_IN_VEL_FIX  0x00000008

/* If set, GNSS SV is used for position and velocity fix: {GPS|BDS} PRNs or {GLO} Slot IDs */
#define GNSS_EXTENDED_SV_INFO_USED_IN_POS_VEL_FIX \
    (GNSS_EXTENDED_SV_INFO_USED_IN_POS_FIX | GNSS_EXTENDED_SV_INFO_USED_IN_VEL_FIX)

/* If set, GNSS SV is corrected with SBAS IONO correction */
#define GNSS_EXTENDED_SV_INFO_SBAS_IONO_CORR  0x00000010
/* If set, GNSS SV is corrected with SBAS FAST correction */
#define GNSS_EXTENDED_SV_INFO_SBAS_FAST_CORR  0x00000020
/* If set, GNSS SV is corrected with SBAS Long-term correction */
#define GNSS_EXTENDED_SV_INFO_SBAS_LONG_CORR  0x00000040
/* If set, GNSS SV is selected with SBAS RTI */
#define GNSS_EXTENDED_SV_INFO_SBAS_RTI_CORR  0x00000080

#define GNSS_EXTENDED_SV_INFO_SBAS_INFO_VALID 0x00000100

typedef enum
{
    GNSS_EXTENDED_SIGNAL_TYPE_GPS_L1CA = 0,
    GNSS_EXTENDED_SIGNAL_TYPE_GLO_G1,
    GNSS_EXTENDED_SIGNAL_TYPE_BDS_B1,
    GNSS_EXTENDED_SIGNAL_TYPE_GAL_E1,
    GNSS_EXTENDED_SIGNAL_TYPE_QZSS_L1CA,
    GNSS_EXTENDED_SIGNAL_TYPE_LEGACY_TOTAL,
    GNSS_EXTENDED_SIGNAL_TYPE_GPS_L2C = GNSS_EXTENDED_SIGNAL_TYPE_LEGACY_TOTAL,
    GNSS_EXTENDED_SIGNAL_TYPE_GPS_L5Q,
    GNSS_EXTENDED_SIGNAL_TYPE_GLO_G2,
    GNSS_EXTENDED_SIGNAL_TYPE_BDS_B2_Deprecated,
    GNSS_EXTENDED_SIGNAL_TYPE_GAL_E5A,
    GNSS_EXTENDED_SIGNAL_TYPE_QZSS_L2C,
    GNSS_EXTENDED_SIGNAL_TYPE_QZSS_L5Q,
    GNSS_EXTENDED_SIGNAL_TYPE_BDS_B2A,
    GNSS_EXTENDED_SIGNAL_TYPE_NAVIC_L5,
    GNSS_EXTENDED_SIGNAL_TYPE_BDS_B1CQ,
    GNSS_EXTENDED_SIGNAL_TYPE_GAL_E5B,
    GNSS_EXTENDED_SIGNAL_TYPE_MAX,
} GnssExtended_SignalEnumTypeVal;

typedef uint8_t GnssExtended_SignalEnumType;

typedef PACKED struct PACKED_POST
{
    /* See GNSS_EXTENDED_SV_INFO_XXX definition bits above.
     * For XXX_VALID bits, if set, then corresponding data is valid, otherwise not valid */
    uint32_t  q_SvInfoMask;
    /* GNSS SV: {GPS|BDS|GAL} PRN or {GLO} SlotID.
               GPS -- 1 to 32 \n
               GLONASS -- 65 to 96 \n
               QZSS -- 193 to 197 \n
               BDS -- 201 to 263 \n
               Galileo -- 301 to 336 */

    uint16_t  w_SvId;
    /* Measurement signal type used in fix.
       Refer GnssExtended_SignalEnumTypeVal above*/

    uint8_t  u_UsedMeasSignalType;
    /* GNSS SV-specific data for SVs used in Position Fix (valid if
        GNSS_EXTENDED_SV_INFO_USED_IN_POS_FIX is set) */
    /* Pseudo-range measurement residuals of GNSS SV */
    float  f_PrResMeters;
    /* Pseudo-range weights (uncertainties) of GNSS SV */
    float  f_PrUncMeters;
    /* GNSS SV-specific data for SVs used in Velocity Fix (valid if
        GNSS_EXTENDED_SV_INFO_USED_IN_VEL_FIX is set) */
    /* Pseudo-range rate (doppler) measurement residuals of GNSS SV */
    float  f_PrrResMps;
   /* Pseudo-range rate (doppler) uncertainties of GNSS SV */
    float  f_PrrUncMps;
    union {
        // IODE {GPS|BDS|GAL}
        uint16_t  w_IODE;
        // Tb for {GLO} SV
        uint8_t  u_GloTb;
    };

    /* GNSS SV-specific data for SVs detected by searcher (valid if
       GNSS_EXTENDED_SV_INFO_MEAS_VALID is set) */
    /* Frequency number(-7~+6) of the Glonass SV, invalid for other constellations.
       valid if GNSS_EXTENDED_SV_INFO_USED_IN_POS_FIX is set as well*/
    int8_t   b_FreqNum;
} GnssExtended_GnssSvInfoType;

typedef enum
{
    GNSS_EXTENDED_SYSTEM_IDX_GPS = 0,
    GNSS_EXTENDED_SYSTEM_IDX_GLO = 1,
    GNSS_EXTENDED_SYSTEM_IDX_BDS = 2,
    GNSS_EXTENDED_SYSTEM_IDX_GAL = 3,
    GNSS_EXTENDED_SYSTEM_IDX_QZSS = 4,
    GNSS_EXTENDED_SYSTEM_IDX_GNSS_TOTAL = 5,
} GnssExtended_SystemIdxEnumType;

typedef PACKED struct PACKED_POST
{
    /* TRUE if all info in this struct valid, otherwise FALSE */
    uint8_t               u_SvInfoValid;
    /* Number of GPS SVs with valid measurement (detected by searcher) */
    uint8_t               u_GpsNumSvMeas;
    /* Number of GPS SVs used in Position Fix */
    uint8_t               u_GpsNumSvPosFix;
    /* Number of GPS SVs used in Velocity Fix */
    uint8_t               u_GpsNumSvVelFix;
    /* Bitmask for GPS SVs used for calculating position fix */
    uint32_t               q_GpsSvMaskUsed;
    /* Number of GLO SVs with valid measurement (detected by searcher) */
    uint8_t               u_GloNumSvMeas;
    /* Number of GLO SVs used in Position Fix */
    uint8_t               u_GloNumSvPosFix;
    /* Number of GLO SVs used in Velocity Fix */
    uint8_t               u_GloNumSvVelFix;
    /* Bitmask for GLONASS SVs used for calculating position fix */
    uint32_t               q_GloSvMaskUsed;
    /* Number of BDS SVs with valid measurement (detected by searcher) */
    uint8_t               u_BdsNumSvMeas;
    /* Number of BDS SVs used in Position Fix */
    uint8_t               u_BdsNumSvPosFix;
    /* Number of BDS SVs used in Velocity Fix */
    uint8_t               u_BdsNumSvVelFix;
    /* Bitmask for BDS SVs used for calculating position fix */
    uint64_t              t_BdsSvMaskUsed;
    /* Number of GAL SVs with valid measurement (detected by searcher) */
    uint8_t               u_GalNumSvMeas;
    /* Number of GAL SVs used in Position Fix */
    uint8_t               u_GalNumSvPosFix;
    /* Number of GAL SVs used in Velocity Fix */
    uint8_t               u_GalNumSvVelFix;
    /* Bitmask for GAL SVs used for calculating position fix */
    uint64_t              t_GalSvMaskUsed;
    /* Number of QZSS SVs with valid measurement (detected by searcher) */
    uint8_t               u_QzssNumSvMeas;
    /* Number of QZSS SVs used in Position Fix */
    uint8_t               u_QzssNumSvPosFix;
    /* Number of QZSS SVs used in Velocity Fix */
    uint8_t               u_QzssNumSvVelFix;
    /* Bitmask for QZSS SVs used for calculating position fix */
    uint8_t               u_QzssSvMaskUsed;
    /* Specific num SV for each constellation in z_GnssSvInfo
     Cumulative sum of values at all indices <= GNSS_EXTENDED_N_ACTIVE_MAX */
    uint8_t               u_GnssNumSvInfo[GNSS_EXTENDED_SYSTEM_IDX_GNSS_TOTAL];
    /* SV-specific data for GNSS supplemental SV information.
     SVs shall be sequenced as per the GNSS_EXTENDED_SYSTEM_IDX_...*/
    GnssExtended_GnssSvInfoType z_GnssSvInfo[GNSS_EXTENDED_N_ACTIVE_MAX];
} GnssExtended_GnssNavSvInfo;

typedef PACKED struct PACKED_POST
{
    uint8_t u_Major; /* Major Revision */
    uint8_t u_Minor; /* Minor Revision */
} GnssExtended_RevisionStructT;

/******************************************************************************
************** Position Report Information to be parsed by client *************
******************************************************************************/
typedef PACKED struct PACKED_POST
{
    GnssExtended_RevisionStructT z_Revision; /* Major and minor revisions */
    GnssExtended_NavPositionStructType    z_NavPos;     /* position-related info */
    GnssExtended_GnssNavSvInfo            z_NavSvInfo;  /* SV-related info */
} GnssExtended_FixInfoStructType;

#define SIZEOF_GNSS_EXTENDED_POSBLOB sizeof(GnssExtended_FixInfoStructType)
#define SIZEOF_VALID_GNSS_EXTENDED_POSBLOB(numSvs) \
         ((numSvs <GNSS_EXTENDED_N_ACTIVE_MAX) ? (SIZEOF_GNSS_EXTENDED_POSBLOB - \
          ((GNSS_EXTENDED_N_ACTIVE_MAX - numSvs)*sizeof(GnssExtended_GnssSvInfoType))) : \
         SIZEOF_GNSS_EXTENDED_POSBLOB)

void decodeSvResidualDataFromExtendedBinaryData(const GnssExtended_FixInfoStructType& inFix,
        GnssSvResidualReport &outReport);
#endif //USE_GLIB

#endif //LOC_APIV02_UTILS_H
