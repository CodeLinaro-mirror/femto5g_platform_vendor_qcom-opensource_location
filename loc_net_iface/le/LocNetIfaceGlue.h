/* Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation, nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef LOC_NET_IFACE_GLUE_H
#define LOC_NET_IFACE_GLUE_H

#include <string>
#include "LocNetExtIface.h"
#include "LocNetIface.h"

using namespace std;

class LocNetIfaceGlue {
public:
    /** @brief
        get singleton instance for LocNetIfaceGlue object. <br/>
    */
    static LocNetIfaceGlue* getInstance();

    /** @brief
        delete singleton instance for LocNetIfaceGlue object. <br/>
    */
    static void deleteInstance();

    /** @brief
        Initialize locNetIface. <br/>
        @param statusCb: status callback function
        @param userHandler: pointer to user handler
        @return true, on success
        @return false, on failure
    */
    bool init(StatusCb statusCb, void* userHandler);

    /** @brief
        Setup backhaul  <br/>
        @return true, on success
        @return false, on failure
    */
    bool connectBackhaul();

    /** @brief
        Disconnects the backhaul <br/>
        @return true, on success
        @return false, on failure
    */
    bool disconnectBackhaul();

    /** @brief
        Wwan status callback to be registered with locnetiface <br/>
        @return void
    */
    static void wwanStatusCallback(void* userDataPtr, LocNetWwanCallEvent event,
            const char* apn, LocNetConnIpType apnIpType);

private:
    static LocNetIfaceGlue *mInstance;
    LocNetIface *mLocNetIfaceInstance;
    StatusCb mClientStatusCb;
    LocWwanCallStatusCb mWwanStatusCb;

    /** @brief
        Creates an instance of LocNetIfaceGlue object. <br/>
    */
    LocNetIfaceGlue();

    /** @brief
        Delete an instance of LocNetIfaceGlue object. <br/>
    */
    ~LocNetIfaceGlue();
};

#endif /* #ifndef LOC_NET_IFACE_GLUE_H */
