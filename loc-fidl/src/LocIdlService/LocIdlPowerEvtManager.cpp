/*
Copyright (c) 2024-2025 Qualcomm Innovation Center, Inc. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "IDL_POWER_HANDLER"
#ifdef POWER_DAEMON_MGR_ENABLED
#include <unistd.h>
#include "LocIdlPowerEvtManager.h"

#define ACK_TIMEOUT_US 300000 // 300 msec

LocIdlAPIService* LocIdlPowerEvtHandler::mLocationApiService = nullptr;

LocIdlPowerEvtHandler* LocIdlPowerEvtHandler::getPwrEvtHandler(LocIdlAPIService* locServiceApiObj) {
    mLocationApiService = locServiceApiObj;
    static LocIdlPowerEvtHandler instance;
    return &instance;
}

LocIdlPowerEvtHandler::LocIdlPowerEvtHandler() {
    int ret = pwr_state_notification_register(LocIdlPowerEvtHandler::pwrMngrLibStateCb);
}

LocIdlPowerEvtHandler::~LocIdlPowerEvtHandler() {
}

int LocIdlPowerEvtHandler::pwrMngrLibStateCb(power_state_t pwr_state) {
    client_ack_t client_ack;
    client_ack.ack = ERR;
    IDLPowerStateType powerState = IDL_POWER_STATE_UNKNOWN;
    LOC_LOGe("Received powerState %d", pwr_state);
    switch (pwr_state.sys_state) {
        case SYS_SUSPEND:
            client_ack.ack = SUSPEND_ACK;
            powerState = IDL_POWER_STATE_SUSPEND;
            break;
        case SYS_RESUME:
            client_ack.ack = RESUME_ACK;
            powerState = IDL_POWER_STATE_RESUME;
            break;
        case SYS_SHUTDOWN:
            client_ack.ack = SHUTDOWN_ACK;
            powerState = IDL_POWER_STATE_SHUTDOWN;
            break;
    }

    if (powerState != IDL_POWER_STATE_UNKNOWN) {
        if (mLocationApiService) {
                mLocationApiService->onPowerEvent(powerState);
        }
    }

    //Allow some time to stop the session and write calibration data NVM.
    usleep(ACK_TIMEOUT_US);
    LOC_LOGd("LocIdlPowerEvtHandler: pwrStateCb sending ack");
    send_acknowledgement(client_ack);

    return 0;
}
#endif
