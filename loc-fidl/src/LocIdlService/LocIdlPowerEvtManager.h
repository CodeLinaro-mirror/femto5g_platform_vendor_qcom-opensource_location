/*
Copyright (c) 2024-2025 Qualcomm Innovation Center, Inc. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause-Clear
*/

#ifndef LOC_IDL_POWER_EVENT_MANAGER_H_
#define LOC_IDL_POWER_EVENT_MANAGER_H_

#ifdef POWER_DAEMON_MGR_ENABLED
#include <power_state.h>
#endif

#include <log_util.h>
#include "LocIdlAPIService.h"

class LocIdlAPIService;

class LocIdlPowerEvtHandler
{
public:
    static LocIdlPowerEvtHandler* getPwrEvtHandler(LocIdlAPIService*);
    ~LocIdlPowerEvtHandler();
    static int pwrMngrLibStateCb(power_state_t pwr_state);
private:
    LocIdlPowerEvtHandler();
    static LocIdlAPIService* mLocationApiService;
};

#endif //LOC_IDL_POWER_EVENT_MANAGER_H_
