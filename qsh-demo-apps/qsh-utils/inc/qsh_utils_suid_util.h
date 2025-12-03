/*
 * Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause-Clear
 */

#pragma once

#include "ISession.h"
#include "SessionFactory.h"

using namespace std;
using namespace ::com::quic::sensinghub::session::V1_0;

/**
 * @brief Type alias for a get suid callback function
 *
 * param dataType: Name of the sensor associated with the event
 * param suids: Vector of suids available for the dataType
 */
using LocIsessionGetSuidCb =
        std::function<void(const std::string& dataType, const std::vector<suid>& suids)>;

/**
 * @brief Utility class for define common function to use ISession
 */
class LocIsession
{
public:
    /**
     * @brief Constructor
     *
     * @param cb Callback function for getSuid
     */
    LocIsession(LocIsessionGetSuidCb cb): mGetSuidCb(cb) {}

    /**
     *  @brief look up the suid for a given sensor driver, registered
     *         callback will be called when suid is available for
     *         this datatype
     *
     *  @param sensorName The sensor driver for which suid is requested
     */
    void getSuid(std::string& sensorName);

    /**
     * Send an attribute request for the specified uid.
     *
     *  @param session The session listens to the event callback
     *  @param uid     The uid to get the attributes
     */
    void getAttributes(unique_ptr<ISession>& session, suid& uid);

private:
    LocIsessionGetSuidCb mGetSuidCb;
    //The session listens to the SNS_SUID_MSGID_SNS_SUID_REQ callback
    unique_ptr<ISession> mSuidSession;
};
