/*
* Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
* SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#include "LocIdlclientDiagLogTypes.h"

namespace loc_idl_diag {

    LocIdlclientDiag::LocIdlclientDiag() : mDiagIface(LocDiagIface::getInstance()) {}

    void LocIdlclientDiag::diagLogPosInfo(locIdlClientDiagPosition &gnssPos,
                uint64_t clk_bootTime, uint64_t gptp_time_ns, bool ptpSyncStatus, float latency)
    {
        if (!mDiagIface) {
            return;
        }

        diagBuffSrc bufferSrc = BUFFER_INVALID;
        locIdlClientDiagLocationRptLog* diagGnssPosPtr = nullptr;
        diagGnssPosPtr = (locIdlClientDiagLocationRptLog*)mDiagIface->logAlloc(
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagLocationRptLog), &bufferSrc);
        if (diagGnssPosPtr == NULL) {
            return;
        }
        memset((char *)diagGnssPosPtr + sizeof(log_hdr_type),
                0x00, sizeof(locIdlClientDiagLocationRptLog) - sizeof(log_hdr_type));
        fillGenericHeader(diagGnssPosPtr->header,
            LOC_IDL_CLIENT_DIAG_MSG_VERSION,
            LOC_IDL_CLIENT_DIAG_POSITION_INFO,
            sizeof(locIdlClientDiagPosition),
            gnssPos.reportingLatency,  //lat in Tx
            gnssPos.elapsedgPTPTime,
            clk_bootTime,
            gptp_time_ns,
            ptpSyncStatus,
            latency
        );
        populateDiagGnssPositon(diagGnssPosPtr->location, gnssPos);
        mDiagIface->logCommit(diagGnssPosPtr, bufferSrc,
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagLocationRptLog));
    }

    void LocIdlclientDiag::diagLogSvInfo(locIdlClientDiagGnssSv &gnssSv,
                    uint64_t clk_bootTime, uint64_t gptp_time_ns, bool ptpSyncStatus)
    {
        if (!mDiagIface) {
            return;
        }

        diagBuffSrc bufferSrc = BUFFER_INVALID;
        locIdlClientDiagSvRptLog* diagGnssSvPtr = nullptr;
        uint32_t packet_size = sizeof(locIdlClientDiagSvRptLog) -
                                (sizeof(locIdlClientDiagGnssSvData) *
                                LOC_IDL_CLIENT_DIAG_GNSS_SV_MAX) +
                                (gnssSv.count * sizeof(locIdlClientDiagGnssSvData));
        uint32_t msg_length = sizeof(locIdlClientDiagGnssSv) -
                            (sizeof(locIdlClientDiagGnssSvData) *
                            LOC_IDL_CLIENT_DIAG_GNSS_SV_MAX) +
                            (gnssSv.count * sizeof(locIdlClientDiagGnssSvData));
        diagGnssSvPtr = (locIdlClientDiagSvRptLog*)mDiagIface->logAlloc(
                LOC_IDL_CLIENT_API_PACKET_C,
                packet_size, &bufferSrc);
        if (diagGnssSvPtr == NULL) {
            return;
        }
        memset((char *)diagGnssSvPtr + sizeof(log_hdr_type),
                0x00, packet_size - sizeof(log_hdr_type));
        fillGenericHeader(diagGnssSvPtr->header,
            LOC_IDL_CLIENT_DIAG_MSG_VERSION,
            LOC_IDL_CLIENT_DIAG_SV_INFO,
            msg_length,
            0, //lat in Tx
            0,
            clk_bootTime,
            gptp_time_ns,
            ptpSyncStatus,
            0
        );
        populateDiagGnssSv(diagGnssSvPtr->sv, gnssSv);

        mDiagIface->logCommit(diagGnssSvPtr, bufferSrc,
                LOC_IDL_CLIENT_API_PACKET_C,
                packet_size);
    }

    void LocIdlclientDiag::diagLogMeasInfo(locIdlClientDiagGnssMeasPacket &gnssMeas,
                        uint64_t clk_bootTime, uint64_t gptp_time_ns, bool ptpSyncStatus)
    {
        if (!mDiagIface) {
            return;
        }

        uint8_t maxSequence = 1;
        uint32_t measCount = gnssMeas.count;
        if (0 < measCount) {
            maxSequence = (uint8)(((measCount-1) / LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ) + 1);
        }
        for (uint8 i = 0; i < maxSequence; i++) {
            diagBuffSrc bufferSrc = BUFFER_INVALID;
            uint32_t count = measCount - i * LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ;
            locIdlClientDiagMeasRptLog* diagGnssMeasPtr = nullptr;

            if (count > LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ) {
                count = LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ;
            }

            uint32_t packet_size = sizeof(locIdlClientDiagMeasRptLog);
            uint32_t msg_length = sizeof(locIdlClientDiagGnssMeas);

            if (count < LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ) {
                packet_size = sizeof(locIdlClientDiagMeasRptLog) -
                                (sizeof(locIdlClientDiagGnssMeasurementsData) *
                                LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ) +
                                (count * sizeof(locIdlClientDiagGnssMeasurementsData));
                msg_length = sizeof(locIdlClientDiagGnssMeas) -
                                (sizeof(locIdlClientDiagGnssMeasurementsData) *
                                    LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ) +
                                    (count * sizeof(locIdlClientDiagGnssMeasurementsData));
            }

            diagGnssMeasPtr =
                    (locIdlClientDiagMeasRptLog*)mDiagIface->logAlloc(
                            LOC_IDL_CLIENT_API_PACKET_C,
                            packet_size,
                            &bufferSrc);
            if (NULL == diagGnssMeasPtr) {
                //LOC_LOGv("memory alloc failed");
                return;
            }

            memset((char *)diagGnssMeasPtr + sizeof(log_hdr_type),
                    0x00, packet_size - sizeof(log_hdr_type));

            diagGnssMeasPtr->meas.maxSequence = maxSequence;
            diagGnssMeasPtr->meas.sequenceNumber = i + 1;
            diagGnssMeasPtr->meas.count = count;

            fillGenericHeader(diagGnssMeasPtr->header,
                LOC_IDL_CLIENT_DIAG_MSG_VERSION,
                LOC_IDL_CLIENT_DIAG_MEASUREMENT_INFO,
                msg_length,
                gnssMeas.reportingLatency, //lat in Tx
                gnssMeas.clock.elapsedgPTPTime,
                clk_bootTime,
                gptp_time_ns,
                ptpSyncStatus,
                0
            );

            populateDiagGnssMeas(diagGnssMeasPtr->meas, gnssMeas);
            mDiagIface->logCommit(diagGnssMeasPtr, bufferSrc,
                    LOC_IDL_CLIENT_API_PACKET_C,
                    packet_size);
        }
    }

    void LocIdlclientDiag::diagLogNmeaInfo(const uint64_t timestamp,
            const string nmea, uint64_t clk_bootTime, uint64_t gptp_time_ns, bool ptpSyncStatus)
    {
        if (!mDiagIface) {
            return;
        }

        diagBuffSrc bufferSrc = BUFFER_INVALID;
        locIdlClientDiagNmeaRptLog* diagNmeaPtr = nullptr;
        uint32_t len = nmea.length();

        uint32_t packet_size = sizeof(locIdlClientDiagNmeaRptLog) + len - 1;
        uint32_t msg_length = sizeof(locIdlClientDiagGnssNmea) + len - 1;

        diagNmeaPtr = (locIdlClientDiagNmeaRptLog*)mDiagIface->logAlloc(
                LOC_IDL_CLIENT_API_PACKET_C,
                packet_size, &bufferSrc);
        if (diagNmeaPtr == NULL) {
            return;
        }
        memset((char *)diagNmeaPtr + sizeof(log_hdr_type),
                0x00, packet_size - sizeof(log_hdr_type));
        fillGenericHeader(diagNmeaPtr->header,
            LOC_IDL_CLIENT_DIAG_MSG_VERSION,
            LOC_IDL_CLIENT_DIAG_NMEA_INFO,
            msg_length,
            0,  //lat in Tx
            0,
            clk_bootTime,
            gptp_time_ns,
            ptpSyncStatus,
            0
        );
        diagNmeaPtr->nmea.timestamp = timestamp;
        diagNmeaPtr->nmea.nmeaLength = len;
        memscpy(&diagNmeaPtr->nmea.nmea, len, nmea.c_str(), len);

        mDiagIface->logCommit(diagNmeaPtr, bufferSrc,
                LOC_IDL_CLIENT_API_PACKET_C,
                packet_size);
    }

    void LocIdlclientDiag::diagLogGnssDataInfo(locIdlClientDiagGnssData &gnssData,
                        uint64_t clk_bootTime, uint64_t gptp_time_ns, bool ptpSyncStatus)
    {
        if (!mDiagIface) {
            return;
        }

        diagBuffSrc bufferSrc = BUFFER_INVALID;
        locIdlClientDiagGnssDataRptLog* diagGnssDataPtr = nullptr;
        diagGnssDataPtr = (locIdlClientDiagGnssDataRptLog*)mDiagIface->logAlloc(
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagGnssDataRptLog), &bufferSrc);
        if (diagGnssDataPtr == NULL) {
            return;
        }
        memset((char *)diagGnssDataPtr + sizeof(log_hdr_type),
                0x00, sizeof(locIdlClientDiagGnssDataRptLog) - sizeof(log_hdr_type));
        fillGenericHeader(diagGnssDataPtr->header,
            LOC_IDL_CLIENT_DIAG_MSG_VERSION,
            LOC_IDL_CLIENT_DIAG_GNSS_DATA_INFO,
            sizeof(locIdlClientDiagGnssData),
            0,  //lat in Tx
            0,
            clk_bootTime,
            gptp_time_ns,
            ptpSyncStatus,
            0
        );

        diagGnssDataPtr->gnssData.size = gnssData.size;

        for  (int i = 0; i < gnssData.size; i++) {
            diagGnssDataPtr->gnssData.gnssDiagDataMask[i] = gnssData.gnssDiagDataMask[i];
            diagGnssDataPtr->gnssData.jammerInd[i] = gnssData.jammerInd[i];
            diagGnssDataPtr->gnssData.agc[i] = gnssData.agc[i];
        }

        mDiagIface->logCommit(diagGnssDataPtr, bufferSrc,
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagGnssDataRptLog));
    }

    void LocIdlclientDiag::diagLogGnssStartReq(locIdlClientDiagStartReq &startReq,
                            uint64_t clk_bootTime, uint64_t gptp_time_ns, bool ptpSyncStatus)
    {
        if (!mDiagIface) {
            return;
        }

        diagBuffSrc bufferSrc = BUFFER_INVALID;
        locIdlClientDiagStartReqLog* diagStartReqPtr = nullptr;
        diagStartReqPtr = (locIdlClientDiagStartReqLog*)mDiagIface->logAlloc(
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagStartReqLog), &bufferSrc);
        if (diagStartReqPtr == NULL) {
            return;
        }
        memset((char *)diagStartReqPtr + sizeof(log_hdr_type),
                0x00, sizeof(locIdlClientDiagStartReqLog) - sizeof(log_hdr_type));
        fillGenericHeader(diagStartReqPtr->header,
            LOC_IDL_CLIENT_DIAG_MSG_VERSION,
            LOC_IDL_CLIENT_DIAG_START_REQ,
            sizeof(locIdlClientDiagStartReq),
            0,  //lat in Tx
            0,
            clk_bootTime,
            gptp_time_ns,
            ptpSyncStatus,
            0
        );

        diagStartReqPtr->start.interval = startReq.interval;
        diagStartReqPtr->start.mask = startReq.mask;

        mDiagIface->logCommit(diagStartReqPtr, bufferSrc,
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagStartReqLog));
    }

    void LocIdlclientDiag::diagLogGnssStopReq(uint64_t clk_bootTime, uint64_t gptp_time_ns,
                                                                           bool ptpSyncStatus)
    {
        if (!mDiagIface) {
            return;
        }

        diagBuffSrc bufferSrc = BUFFER_INVALID;
        locIdlClientDiagStopReqLog* diagStopReqPtr = nullptr;
        diagStopReqPtr = (locIdlClientDiagStopReqLog*)mDiagIface->logAlloc(
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagStopReqLog), &bufferSrc);
        if (diagStopReqPtr == NULL) {
            return;
        }
        memset((char *)diagStopReqPtr + sizeof(log_hdr_type),
                0x00, sizeof(locIdlClientDiagStopReqLog) - sizeof(log_hdr_type));
        fillGenericHeader(diagStopReqPtr->header,
            LOC_IDL_CLIENT_DIAG_MSG_VERSION,
            LOC_IDL_CLIENT_DIAG_STOP_REQ,
            0,
            0,  //lat in Tx
            0,
            clk_bootTime,
            gptp_time_ns,
            ptpSyncStatus,
            0
        );

        mDiagIface->logCommit(diagStopReqPtr, bufferSrc,
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagStopReqLog));
    }

    void LocIdlclientDiag::diagLogGnssDelReq(locIdlClientDiagDelReq &DelReq,
                            uint64_t clk_bootTime, uint64_t gptp_time_ns, bool ptpSyncStatus)
    {
        if (!mDiagIface) {
            return;
        }

        diagBuffSrc bufferSrc = BUFFER_INVALID;
        locIdlClientDiagDelReqLog* diagDelReqPtr = nullptr;
        diagDelReqPtr = (locIdlClientDiagDelReqLog*)mDiagIface->logAlloc(
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagDelReqLog), &bufferSrc);
        if (diagDelReqPtr == NULL) {
            return;
        }
        memset((char *)diagDelReqPtr + sizeof(log_hdr_type),
                0x00, sizeof(locIdlClientDiagDelReqLog) - sizeof(log_hdr_type));
        fillGenericHeader(diagDelReqPtr->header,
            LOC_IDL_CLIENT_DIAG_MSG_VERSION,
            LOC_IDL_CLIENT_DIAG_DEL_REQ,
            sizeof(locIdlClientDiagDelReq),
            0,  //lat in Tx
            0,
            clk_bootTime,
            gptp_time_ns,
            ptpSyncStatus,
            0
        );

        diagDelReqPtr->del.delMask = DelReq.delMask;

        mDiagIface->logCommit(diagDelReqPtr, bufferSrc,
                LOC_IDL_CLIENT_API_PACKET_C,
                sizeof(locIdlClientDiagDelReqLog));
    }

    void LocIdlclientDiag::fillGenericHeader(locIdlGenericHeader &header,
            uint8_t u_Version, uint8_t msgType, uint32_t msgLength, uint16_t lattencyInTxEndMsec,
            uint64_t t_TxTimePtpNsec, uint64_t clk_bootTime, uint64_t gptp_time_ns,
            bool ptpSyncStatus, float latency)
    {
        header.u_Version = u_Version;
        header.msgType = msgType;
        header.msgLength = msgLength;
        header.lattencyInTxEndMsec = lattencyInTxEndMsec;
        header.t_TxTimePtpNsec = t_TxTimePtpNsec;
        header.t_TimeTickNsec = clk_bootTime;
        header.t_RxTimePtpNsec = gptp_time_ns;
        header.ptpSyncStatus = ptpSyncStatus;
        header.lattencyInRxEndMsec = latency;
        header.u_Process_id = (uint32_t)getpid();
        snprintf(header.processName, sizeof(header.processName),
            "%s%d", getprogname(), (uint32_t)getpid());
    }

    void LocIdlclientDiag::populateDiagGnssSv(locIdlClientDiagGnssSv &dst,
                                                locIdlClientDiagGnssSv &src)
    {
        dst.count = src.count;
        for (int i = 0; i < src.count; i++) {
            dst.gnssSvs[i].svId = src.gnssSvs[i].svId;
            dst.gnssSvs[i].type = src.gnssSvs[i].type;
            dst.gnssSvs[i].cN0Dbhz = src.gnssSvs[i].cN0Dbhz;
            dst.gnssSvs[i].elevation = src.gnssSvs[i].elevation;
            dst.gnssSvs[i].azimuth = src.gnssSvs[i].azimuth;
            dst.gnssSvs[i].gnssSvOptionsMask = src.gnssSvs[i].gnssSvOptionsMask;
            dst.gnssSvs[i].carrierFrequencyHz = src.gnssSvs[i].carrierFrequencyHz;
            dst.gnssSvs[i].gnssSignalTypeMask = src.gnssSvs[i].gnssSignalTypeMask;
            dst.gnssSvs[i].basebandCarrierToNoiseDbHz = src.gnssSvs[i].basebandCarrierToNoiseDbHz;
            dst.gnssSvs[i].gloFrequency = src.gnssSvs[i].gloFrequency;
        }
    }

    void LocIdlclientDiag::populateDiagGnssPositon(locIdlClientDiagPosition &dst,
                                                    locIdlClientDiagPosition &src)
    {
        dst.flags = src.flags;
        dst.timestamp = src.timestamp;
        dst.latitude = src.latitude;
        dst.longitude = src.longitude;
        dst.altitude = src.altitude;
        dst.speed = src.speed;
        dst.bearing = src.bearing;
        dst.horizontalAccuracy = src.horizontalAccuracy;
        dst.verticalAccuracy = src.verticalAccuracy;
        dst.speedAccuracy = src.speedAccuracy;
        dst.bearingAccuracy = src.bearingAccuracy;
        dst.techMask = src.techMask;
        dst.gnssInfoFlags = src.gnssInfoFlags;
        dst.altitudeMeanSeaLevel = src.altitudeMeanSeaLevel;
        dst.pdop = src.pdop;
        dst.hdop = src.hdop;
        dst.vdop = src.vdop;
        dst.gdop = src.gdop;
        dst.tdop = src.tdop;
        dst.magneticDeviation = src.magneticDeviation;
        dst.horReliability = src.horReliability;
        dst.verReliability = src.verReliability;
        dst.horUncEllipseSemiMajor = src.horUncEllipseSemiMajor;
        dst.horUncEllipseSemiMinor = src.horUncEllipseSemiMinor;
        dst.horUncEllipseOrientAzimuth = src.horUncEllipseOrientAzimuth;
        dst.northStdDeviation = src.northStdDeviation;
        dst.eastStdDeviation = src.eastStdDeviation;
        dst.northVelocity = src.northVelocity;
        dst.eastVelocity = src.eastVelocity;
        dst.upVelocity = src.upVelocity;
        dst.northVelocityStdDeviation = src.northVelocityStdDeviation;
        dst.eastVelocityStdDeviation = src.eastVelocityStdDeviation;
        dst.upVelocityStdDeviation = src.upVelocityStdDeviation;
        dst.svUsedInPosition = src.svUsedInPosition;
        dst.navSolutionMask = src.navSolutionMask;
        dst.posTechMask = src.posTechMask;
        dst.bodyFrameData = src.bodyFrameData;
        dst.gnssSystemTime = src.gnssSystemTime;
        dst.numOfMeasReceived = src.numOfMeasReceived;
        for (int i = 0; i < dst.numOfMeasReceived; i++)
            dst.measUsageInfo[i] = src.measUsageInfo[i];
        dst.leapSeconds = src.leapSeconds;
        dst.timeUncMs = src.timeUncMs;
        dst.numSvUsedInPosition = src.numSvUsedInPosition;
        dst.calibrationConfidencePercent = src.calibrationConfidencePercent;
        dst.calibrationStatus = src.calibrationStatus;
        dst.bootTimestampNs = src.bootTimestampNs;
        dst.locOutputEngType = src.locOutputEngType;
        dst.locOutputEngMask = src.locOutputEngMask;
        dst.conformityIndex = src.conformityIndex;
        src.llaVRPBased = src.llaVRPBased;
        for (int i = 0; i < 3; i++)
            dst.enuVelocityVRPBased[i] = src.enuVelocityVRPBased[i];
        dst.drSolutionStatusMask = src.drSolutionStatusMask;
        dst.altitudeAssumed = src.altitudeAssumed;
        dst.capabilitiesMask = src.capabilitiesMask;
        dst.sessionStatus = src.sessionStatus;
        dst.integrityRiskUsed = src.integrityRiskUsed;
        dst.protectAlongTrack = src.protectAlongTrack;
        dst.protectCrossTrack = src.protectCrossTrack;
        dst.protectVertical = src.protectVertical;
        dst.elapsedRealTimeNs = src.elapsedRealTimeUncNs;
        dst.elapsedRealTimeUncNs = src.elapsedRealTimeUncNs;
        dst.sessionStartBootTimestampNs = src.sessionStartBootTimestampNs;
        dst.reportTriggerType = src.reportTriggerType;
        dst.numOfDgnssStationId = src.numOfDgnssStationId;
        for (int i = 0; i < dst.numOfDgnssStationId; i++)
            dst.dgnssStationId[i] = src.dgnssStationId[i];
        dst.elapsedgPTPTime = src.elapsedgPTPTime;
        dst.elapsedgPTPTimeUnc = src.elapsedgPTPTimeUnc;
        dst.baseLineLength = src.baseLineLength;
        dst.ageMsecOfCorrections = src.ageMsecOfCorrections;
        dst.currReportingRate = src.currReportingRate;
        dst.reportingLatency = src.reportingLatency;
    }

    void LocIdlclientDiag::populateDiagGnssMeas(
            locIdlClientDiagGnssMeas &dst,
            locIdlClientDiagGnssMeasPacket &src) {

        uint8 adjust = 0;
        if (1 <= dst.sequenceNumber) {
            adjust = (dst.sequenceNumber - 1) * LOC_IDL_CLIENT_DIAG_GNSS_MEASUREMENTS_SEQ;
        }
        uint32_t count =  dst.count;

        for (uint32_t idx = 0; idx < count; ++idx) {
            uint32_t gIdx = idx + adjust;
            dst.measurements[idx].flags = src.measurements[idx].flags;
            dst.measurements[idx].svId = src.measurements[gIdx].svId;
            dst.measurements[idx].svType = src.measurements[gIdx].svType;
            dst.measurements[idx].timeOffsetNs = src.measurements[gIdx].timeOffsetNs;
            dst.measurements[idx].stateMask = src.measurements[gIdx].stateMask;
            dst.measurements[idx].receivedSvTimeNs = src.measurements[gIdx].receivedSvTimeNs;
            dst.measurements[idx].receivedSvTimeSubNs = src.measurements[gIdx].receivedSvTimeSubNs;
            dst.measurements[idx].receivedSvTimeUncertaintyNs =
                            src.measurements[gIdx].receivedSvTimeUncertaintyNs;
            dst.measurements[idx].carrierToNoiseDbHz = src.measurements[gIdx].carrierToNoiseDbHz;
            dst.measurements[idx].pseudorangeRateMps = src.measurements[gIdx].pseudorangeRateMps;
            dst.measurements[idx].pseudorangeRateUncertaintyMps =
                        src.measurements[gIdx].pseudorangeRateUncertaintyMps;
            dst.measurements[idx].adrStateMask = src.measurements[gIdx].adrStateMask;
            dst.measurements[idx].adrMeters = src.measurements[gIdx].adrMeters;
            dst.measurements[idx].adrUncertaintyMeters =
                                                    src.measurements[gIdx].adrUncertaintyMeters;
            dst.measurements[idx].carrierFrequencyHz = src.measurements[gIdx].carrierFrequencyHz;
            dst.measurements[idx].carrierCycles = src.measurements[gIdx].carrierCycles;
            dst.measurements[idx].carrierPhase = src.measurements[gIdx].carrierPhase;
            dst.measurements[idx].carrierPhaseUncertainty =
                                                src.measurements[gIdx].carrierPhaseUncertainty;
            dst.measurements[idx].multipathIndicator = src.measurements[gIdx].multipathIndicator;
            dst.measurements[idx].signalToNoiseRatioDb =
                                            src.measurements[gIdx].signalToNoiseRatioDb;
            dst.measurements[idx].agcLevelDb = src.measurements[gIdx].agcLevelDb;
            dst.measurements[idx].basebandCarrierToNoiseDbHz =
                                        src.measurements[gIdx].basebandCarrierToNoiseDbHz;
            dst.measurements[idx].gnssSignalType = src.measurements[gIdx].gnssSignalType;
            dst.measurements[idx].fullInterSignalBiasNs =
                            src.measurements[gIdx].fullInterSignalBiasNs;
            dst.measurements[idx].fullInterSignalBiasUncertaintyNs =
                        src.measurements[gIdx].fullInterSignalBiasUncertaintyNs;
            dst.measurements[idx].cycleSlipCount = src.measurements[gIdx].cycleSlipCount;
        }
        dst.clock.flags = src.clock.flags;
        dst.clock.leapSecond = src.clock.leapSecond;
        dst.clock.timeNs = src.clock.timeNs;
        dst.clock.timeUncertaintyNs = src.clock.timeUncertaintyNs;
        dst.clock.fullBiasNs = src.clock.fullBiasNs;
        dst.clock.biasNs = src.clock.biasNs;
        dst.clock.biasUncertaintyNs = src.clock.biasUncertaintyNs;
        dst.clock.driftNsps = src.clock.driftNsps;
        dst.clock.driftUncertaintyNsps = src.clock.driftUncertaintyNsps;
        dst.clock.hwClockDiscontinuityCount = src.clock.hwClockDiscontinuityCount;
        dst.clock.elapsedRealTime = src.clock.elapsedRealTime;
        dst.clock.elapsedRealTimeUnc = src.clock.elapsedRealTimeUnc;
        dst.clock.elapsedgPTPTime = src.clock.elapsedgPTPTime;
        dst.clock.elapsedgPTPTimeUnc = src.clock.elapsedgPTPTimeUnc;
    }
}
