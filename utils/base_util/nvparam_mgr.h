/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 GTP storage manager for persistent parameters

 GENERAL DESCRIPTION
 This declares the public interface of storage manager for
 permanent parameters.

 Copyright (c) 2024-2025 Qualcomm Innovation Center, Inc. All rights reserved.
 SPDX-License-Identifier: BSD-3-Clause-Clear
 =============================================================================*/
#ifndef __XTRAT_WIFI_NVPARAM_MGR_H__
#define __XTRAT_WIFI_NVPARAM_MGR_H__
#include <stdint.h>

namespace qc_loc_fw
{
// Below is the list of parameter names to be shared among modules
// for 32-bit pseudo client ID, this will be 32 bit signed,
// for 64 bit  signed or unsigned
#define PARAM_NAME_PSEUDO_CLIENT_ID       "PSEUDO_CLIENT_ID"
#define PARAM_NAME_PSEUDO_CLIENT_ID_64BIT "PSEUDO_CLIENT_ID_64BIT"

// list of error code currently supported by nv param manager
// for saveXXXParam and getXXXParam
typedef enum
{
  NV_PARAM_ERR_NO_ERR           = 0,
  // the parameter is not available
  NV_PARAM_ERR_PARAM_NOT_FOUND  = 1,
  NV_PARAM_ERR_PRRAM_NOT_VALID  = 2,
  // sqlite is locked and operation can not be performed now,
  // if the operation is critical, suggest application to
  // wait a few seconds and try again.
  NV_PARAM_ERR_SQLITE_BUSY      = 3,
  // other sqlite error
  NV_PARAM_ERR_SQLITE_OTHER_ERR = 4,
  // leave some range for extension
  NV_PARAM_ERR_OTHER_ERR        = 100
} nv_param_err_code;

// NOTE: to use this class, it is user's responsibility to make sure
// that nv param name is unique. It is recommended nv param name to
// start with your module name to avoid collision.
class NvParamMgr
{
public:
  // returns singleton NvParamMgr implementation class
  // NOTE: If the returned poiner is non-NULL, please
  //       call NvParamMgr::releaseInstance after usage.
  static NvParamMgr* getInstance();
  // releases the refence to NvParamMgr implementation class
  // NOTE: calling this function need to be paired when getInstance()
  //       is returning non-NULL pointer.
  static void releaseInstance();

  // Below APIs are thread safe with current sqlite based implementation.
  //
  // The APIs will save the given param value into the permanent storage file.
  // Return value is defined in nv_param_err_code.
  //
  // Note: If returned error code is NV_PARAM_ERR_SQLITE_BUSY, the sqlite file
  // is locked and operation can not be performed now,
  // if the operation is critical, suggest application to
  // wait a few seconds and try again.
  nv_param_err_code saveUInt64Param (const char* param_name,
                                     uint64_t param_value);

  nv_param_err_code saveBlobParam (const char* param_name,
                                   const unsigned char* param_value,
                                   unsigned int param_size_in_bytes);

  // Below APIs are thread safe with current sqlite based implementation.
  //
  // The APIs will retrive the given param value from the permanent storage file
  // If the param can not be retrieved, param_value will not be
  // modified.
  //
  // Return value is defined in nv_param_err_code, 0 for success.
  //
  // Note: If returned error code is NV_PARAM_ERR_SQLITE_BUSY,
  // the sqlite file is locked and operation can not be performed now,
  // if the operation is critical, suggest application to
  // wait a few seconds and try again.
  nv_param_err_code getUInt64Param (const char* param_name,
                                    uint64_t & param_value);

  // 0 or NV_PARAM_ERR_NO_ERR: when successful
  // NOTE: memory returned via param_value need to be freed by caller
  // via delete [] when return code is 0 (NV_PARAM_ERR_NO_ERROR)
  nv_param_err_code getBlobParam (const char* param_name,
                                  unsigned char* &param_value,
                                  unsigned int &param_size_in_bytes);

protected:
  NvParamMgr (const char* const filename);

  // allow constructor to be made only via getInstance
  NvParamMgr() {};

  // prohibit calling delete on the pointer returned from
  // NvParamMgr::getInstance()
  virtual ~NvParamMgr() {};

private:
  nv_param_err_code init();
  nv_param_err_code deinit();
  nv_param_err_code openDB ();

  nv_param_err_code saveNvDataBlob (const char* param_name,
                                    const void* data,
                                    uint32_t data_len);

  nv_param_err_code getNvDataBlob (const char* param_name,
                                   void* data,
                                   int &data_len);

  // internal functions
  int generatePseudoClientIdIfNeeded ();

  // internal functions
  int wrapper_mprintf (char* &output_buf, unsigned int &output_size,
                       const char *sentence, ...);

  // Variables
  // sqlite file and etc
  const char* m_filename;
  void*    m_db;

  char*        m_sqlite_sentence;
  unsigned int m_sqlite_sentence_size;

  bool     m_pseudo_client_id_exist;
  uint64_t m_pseudo_client_id_64;
};

} // namespace qc_loc_fw

#endif // #ifndef __XTRAT_WIFI_NVPARAM_MGR_H__
