//
// Created by Xsakura on 2023/4/17.
//

#ifndef MASSDB_MSDATA_H
#define MASSDB_MSDATA_H

#include <string>
#include <vector>

#include "common/base/activation_base.hpp"
#include "ms/spec/deconv_ms.hpp"
#include "ms/spec/ms_header.hpp"
#include "ms/spec/peak.hpp"

namespace massdb {

using namespace std;
using namespace toppic;

class MsData {
public:
  static string peak_list_to_string(const vector<PeakPtr>& peak_list);
  static string peak_list_to_string(const vector<DeconvPeakPtr>& peak_list);
  static string header_to_string(const MsHeaderPtr& ms_header_ptr);

  static PeakPtrVec string_to_peak_list(const string& data);
  static PeakPtrVec string_to_peak_list(const char* data);

  static MsHeaderPtr string_to_header(const string& data);
  static MsHeaderPtr string_to_header(const char* data);

  //  bool string_to_header(const string& data);
  //  bool string_to_peak_list(const string& data);
  static DeconvPeakPtrVec string_to_deconv_ms_peak_list(const string& data);
  static DeconvPeakPtrVec string_to_deconv_ms_peak_list(const char* data);

  //  MsHeaderPtr header_ptr_;
  //  PeakPtrVec peak_list_;
  //  DeconvPeakPtrVec ms_peak_list_;
};

}  // namespace massdb

#endif  // MASSDB_MSDATA_H
