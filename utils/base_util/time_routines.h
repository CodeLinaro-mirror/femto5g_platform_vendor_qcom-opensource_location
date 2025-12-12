/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 Time related routines

 GENERAL DESCRIPTION
 This component implements portable time related routines

 Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
 SPDX-License-Identifier: BSD-3-Clause-Clear
 =============================================================================*/

#ifndef __QC_LOC_FW_TIME_H__
#define __QC_LOC_FW_TIME_H__

#include <time.h>
#include <base_util/postcard.h>
#include <list>

namespace qc_loc_fw
{
using std::list;
class TimeDiff
{
public:
  // validity: false to prevent any further operation unless it's reset with true
  // or being copied/assigned with = to a valid object
  explicit TimeDiff (const bool validity = true);
  int add_sec(const int sec);
  int add_msec(const int msec);
  float get_total_sec() const;
  float get_total_msec() const;
  const timespec * getTimeDiffPtr() const;
  bool is_valid() const;
  void reset(const bool validity = false);
private:
  static const char * const TAG;
  bool m_is_valid;
  timespec m_timediff;
};

class Timestamp
{
public:
  // set to false if you want to use reset_to_XXX functions to initialize it to
  // non-default clock, or you simply want to set it later, instead of at time of construction
  explicit Timestamp (const bool set_to_default_clock = true);

  // use this if you want to set it to some specific clock type using clock id (defined in time.h)
  explicit Timestamp (int clock_id);
  // use this if you want to initialize it with pre-existing timespec acquired using some clock type
  Timestamp (const int clock_id, const timespec & src);
  // copy constructor
  Timestamp (const Timestamp & rhs);

  const timespec * getTimestampPtr() const;
  bool is_valid() const;
  void invalidate();
  Timestamp operator + (const TimeDiff & rhs) const;
  Timestamp operator - (const TimeDiff & rhs) const;
  TimeDiff operator - (const Timestamp & rhs) const;
  bool operator >= (const Timestamp & rhs) const;
  bool operator < (const Timestamp & rhs) const;
  Timestamp & operator = (const Timestamp & rhs);

private:
  static const char * const TAG;
  bool m_is_valid;
  int m_clock_id;
  timespec m_timestamp;
};

#ifdef __cplusplus
extern "C"
{
#endif

// Function Name: get_time_rtc_ms
// Description:   return rtc time in milliseconds
//                represents time since the Epoch
long long get_time_rtc_ms();

// Function Name: get_time_boot_ms
// Description:   Returns monotonic time from boot
long long get_time_boot_ms();

#ifdef __cplusplus
}
#endif

} // namespace qc_loc_fw

#endif //#ifndef __QC_LOC_FW_TIME_H__
