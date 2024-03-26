/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/
#ifndef LOC_IDL_CLIENT_IF_V02_H
#define LOC_IDL_CLIENT_IF_V02_H

#include "gps_extended_c.h"
#include "gps_extended.h"
#include "LocationDataTypes.h"
#include <iostream>

#define FIDL_LOC_SUPPORTED_FEATURE_LENGTH_V021 (100)
#define MAX_NMEA_STRING_SIZE    201

#define LOC_IDL_SOCKET_DIR       "/dev/socket/location/"
#define LOC_IDL_SOCKET_Device    "/dev/socket/location/socket_fidl_device"
#define LOC_IDL_SOCKET_HAL       "/dev/socket/location/socket_fidl_hal"

typedef struct {
    LocPositionMode mode;
    LocGpsPositionRecurrence recurrence;
    uint32_t min_interval;
    uint32_t preferred_accuracy;
    uint32_t preferred_time;
    bool share_position;
    char credentials[14];
    char provider[8];
    GnssPowerMode powerMode;
    uint32_t timeBetweenMeasurements;
} locClientFidlPosMode1;

/* FIDL Supported Features */
typedef struct {
    /**< Must be set to # of elements in feature */
    uint32_t feature_len;
/**<   This field describes which features are supported in the running
     QMI_LOC service. The array of unit8 is the bitmask where each bit
     represents a feature enum. Bit 0 represents feature enum ID 0,
     bit 1 represents feature enum ID 1, etc.
     For example, if QMI_LOC supports feature enum 0,1,2,8,
     feature_len is 2, and
     feature array is [7,1]. \n
     - Type: Array of uint8
     - Maximum array length: 100
     - enum is "loc_supported_feature_enum" which declared in gps_extended_c.h file.
*/
    uint8_t feature[FIDL_LOC_SUPPORTED_FEATURE_LENGTH_V021];
} locClientFidlFeatureList1;

/* Request */
enum class IdlClinetRequests
{
    CLNT_REQ_UNDEFINED = 0,
    CLNT_REQ_OPEN,
    CLNT_REQ_CLOSE,
    CLNT_REQ_START_FIX,
    CLNT_REQ_STOP_FIX,
    CLNT_REQ_DEL_AIDING,
    CLNT_REQ_SET_POS_MODE,
    CLNT_REQ_SET_GPS_LOCK,
    CLNT_REQ_START_TIME_BASED_TRK,
    CLNT_REQ_STOP_TIME_BASED_TRK
};

typedef struct
{
    uint64_t    requestedMask;
    bool        isMaster;
    uint64_t    supportedMask;
    locClientFidlFeatureList1    fidlFeatures;
}ReqOpen;

typedef struct
{
    char placeHolder;
}ReqClose;

typedef struct
{
    locClientFidlPosMode1    fixCriteria;
}ReqStartFix;

typedef struct
{
    char placeHolder;
}ReqStopFix;

typedef struct
{
    GnssAidingData data;
}ReqDelAiding;

typedef struct
{
    locClientFidlPosMode1 posMode;
}ReqSetPosMode;


typedef struct
{
    GnssConfigGpsLock    lock;
}ReqSetGpsLock;

typedef struct
{
    TrackingOptions    options;
}ReqStartTimeBasedTrk;

typedef struct
{
    char placeHolder;
}ReqStopTimeBasedTrk;

struct ReqMsgBase
{
    IdlClinetRequests reqID;
    uint32_t        uniqueId; /* Future use */
    uint32_t            size;
    inline ReqMsgBase(IdlClinetRequests inReqID, uint32_t inSize):
                reqID(inReqID),
                uniqueId(0xFF),
                size(inSize)
                {}
    virtual ~ReqMsgBase() = default;
};

struct ReqMsgOpenPkt : ReqMsgBase
{
    ReqOpen       reqOpen;
    inline ReqMsgOpenPkt(): ReqMsgBase(IdlClinetRequests::CLNT_REQ_OPEN, sizeof(ReqOpen))
    {
    }
};

struct ReqMsgClosePkt : ReqMsgBase
{
    ReqClose       closeOpen;
    inline ReqMsgClosePkt(): ReqMsgBase(IdlClinetRequests::CLNT_REQ_CLOSE, sizeof(ReqClose))
    {
    }
};

struct ReqMsgStartFix : ReqMsgBase
{
    ReqStartFix     reqStart;
    inline ReqMsgStartFix(): ReqMsgBase(IdlClinetRequests::CLNT_REQ_START_FIX,
                                                        sizeof(ReqStartFix))
    {
    }
};

struct ReqMsgStopFix : ReqMsgBase
{
    ReqStopFix       reqStop;
    inline ReqMsgStopFix(): ReqMsgBase(IdlClinetRequests::CLNT_REQ_STOP_FIX, sizeof(ReqStopFix))
    {
    }
};

struct ReqMsgDelAiding : ReqMsgBase
{
    ReqDelAiding       reqDel;
    inline ReqMsgDelAiding(): ReqMsgBase(IdlClinetRequests::CLNT_REQ_DEL_AIDING,
                                                    sizeof(ReqDelAiding))
    {
    }
};

struct ReqMsgSetPosMode : ReqMsgBase
{
    ReqSetPosMode       reqSetPos;
    inline ReqMsgSetPosMode(): ReqMsgBase(IdlClinetRequests::CLNT_REQ_SET_POS_MODE,
                                                            sizeof(ReqSetPosMode))
    {
    }
};

struct ReqMsgSetGpsLock : ReqMsgBase
{
    ReqSetGpsLock       reqSetGps;
    inline ReqMsgSetGpsLock(): ReqMsgBase(IdlClinetRequests::CLNT_REQ_SET_GPS_LOCK,
                                                            sizeof(ReqSetGpsLock))
    {
    }
};

struct ReqMsgStartTimerBasedTrk : ReqMsgBase
{
    ReqStartTimeBasedTrk       reqStartTimerTrk;
    inline ReqMsgStartTimerBasedTrk(): ReqMsgBase(IdlClinetRequests::CLNT_REQ_START_TIME_BASED_TRK,
                                                                    sizeof(ReqStartTimeBasedTrk))
    {
    }
};

struct ReqMsgStopTimerBasedTrk : ReqMsgBase
{
    ReqStopTimeBasedTrk        reqStopTimerTrk;
    inline ReqMsgStopTimerBasedTrk(): ReqMsgBase(IdlClinetRequests::CLNT_REQ_STOP_TIME_BASED_TRK,
                                                        sizeof(ReqStopTimeBasedTrk))
    {
    }
};

/* Events */
enum class IdlClientEvents
{
    CLNT_EVT_UNDEFINED = 0,
    CLNT_EVT_DEVICE_UP,
    CLNT_EVT_DEVICE_DOWN,
    CLNT_EVT_RESPONSE,
    CLNT_EVT_POS_RPT,
    CLNT_EVT_SV_RPT,
    CLNT_EVT_SV_MEAS_RPT,
    CLNT_EVT_NMEA_RPT,
    CLNT_EVT_GNSS_MEAS_RPT,
    CLNT_EVT_CONFIG_RPT
};

enum class ResponseStatus
{
    CLNT_RESONSE_FAILURE = 0,
    CLNT_RESONSE_SUCCESS
};

typedef struct
{
    IdlClinetRequests    reqID;
    ResponseStatus       respStat;
    char                 reserved[32];
}EventResponse;

typedef struct
{
    UlpLocation                    location;
    GpsLocationExtended            locationExtended;
    enum loc_sess_status           status;
    LocPosTechMask                 loc_technology_mask;
    GnssDataNotification           dataNotify;
    int                            msInWeek;
    uint64_t                       gptp_time_ns;
    uint64_t                       elapsedgPTPTime;
    char                           reserved[32];
}EventPosRpt;

typedef struct
{
    GnssSvNotification    svNotify;
    char                  reserved[32];
}EventSvRpt;

typedef struct
{
    GnssMeasurements     svMeasurementSet;
    char                 reserved[32];
}EventSvMeasRpt;

typedef struct
{
    char        nmea[MAX_NMEA_STRING_SIZE];
    int         length;
    uint64_t    timestamp;
    char        reserved[32];
}EventNmeaRpt;

typedef struct
{
    GnssMeasurements        measurements;
    int                       msInWeek;
    char                     reserved[32];
}EventGnssMeasRpt;

typedef struct
{
    uint32_t        sessionId;
    GnssConfig        gnssConfig;
    char             reserved[32];
}EventGnssConfig;

struct EventMsgBase
{
    IdlClientEvents     eventID;
    uint32_t            uniqueId; /* Future use */
    uint32_t            size;
    inline EventMsgBase(IdlClientEvents inEventID, uint32_t inSize):
                eventID(inEventID),
                uniqueId(0xFF),
                size(inSize)
                {}
    virtual ~EventMsgBase() = default;
};

struct EventMsgResponsePkt : EventMsgBase
{
    EventResponse       rspEvt;
    inline EventMsgResponsePkt(): EventMsgBase(IdlClientEvents::CLNT_EVT_RESPONSE,
                                                                sizeof(EventResponse))
    {
    }
};

struct EventMsgEngineUpPkt : EventMsgBase
{
    inline EventMsgEngineUpPkt(): EventMsgBase(IdlClientEvents::CLNT_EVT_DEVICE_UP, 0)
    {
    }
};

struct EventMsgEngineDownPkt : EventMsgBase
{
    inline EventMsgEngineDownPkt(): EventMsgBase(IdlClientEvents::CLNT_EVT_DEVICE_DOWN, 0)
    {
    }
};

struct EventMsgPosPkt : EventMsgBase
{
    EventPosRpt       posRpt;
    inline EventMsgPosPkt(): EventMsgBase(IdlClientEvents::CLNT_EVT_POS_RPT, sizeof(EventPosRpt))
    {
    }
};

struct EventMsgGnssMeasPkt : EventMsgBase
{
    EventGnssMeasRpt       measRpt;
    inline EventMsgGnssMeasPkt(): EventMsgBase(IdlClientEvents::CLNT_EVT_GNSS_MEAS_RPT,
                                                                    sizeof(EventGnssMeasRpt))
    {
    }
};

struct EventMsgSvPkt : EventMsgBase
{
    EventSvRpt       svRpt;
    inline EventMsgSvPkt(): EventMsgBase(IdlClientEvents::CLNT_EVT_SV_RPT, sizeof(EventSvRpt))
    {
    }
};

struct EventMsgNmeaPkt : EventMsgBase
{
    EventNmeaRpt       nmeaRpt;
    inline EventMsgNmeaPkt(): EventMsgBase(IdlClientEvents::CLNT_EVT_NMEA_RPT,
                                                            sizeof(EventNmeaRpt))
    {
    }
};

 #endif /* LOC_IDL_CLIENT_IF_V02_H */