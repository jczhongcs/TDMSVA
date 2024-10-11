//
// Created by Xsakura on 2023/5/1.
//

#ifndef MASSDB_MS_WRITER_H
#define MASSDB_MS_WRITER_H

#include <map>

#include "ms/spec/deconv_ms.hpp"
#include "ms/spec/raw_ms.hpp"
#include "mod_total.h"
#include "deMs1.h"
#include "PrSM.h"

namespace massdb::data2json {

// void write(const std::string& file_name, toppic::RawMsPtr ms_ptr);
std::string json_ms(toppic::RawMsPtr ms_ptr);

std::string json_package_list(std::map<std::string, std::string>* package_list);

std::string json_ms_file_list(const std::vector<bool>& exist_file);

std::string json_rms_key_list(const std::vector<std::string>& rms_key_list);
std::string json_dms_key_list(const std::vector<std::string>& dms_key_list);
std::string json_ims_key_list(const std::vector<std::string>& ims_key_list);
std::string json_dm1_key_list(const std::vector<std::string>& dms_key_list);

std::string json_rms_single_data(const toppic::RawMsPtr& rms_single_data);
std::string json_dms_single_data(const toppic::DeconvMsPtr& ims_single_data);
std::string json_ims_single_data(const std::string& ims_single_data);
std::string json_dm1_single_data(const toppic::DeconvMsPtr& ims_single_data);

std::string json_mtpVec_list(const modTotalPtrVec &mtpVec);
std::string json_dm1Vec_list(const dm1PtrVec &dm1PVec);
std::string json_prsmVec_list(const PrsmPtrVec &prsmPtrVec);


}  // namespace massdb::data2json

#endif  // MASSDB_MS_WRITER_H
