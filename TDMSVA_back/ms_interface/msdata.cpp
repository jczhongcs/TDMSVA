//
// Created by Xsakura on 2023/4/17.
//

#include "msdata.h"

#include <algorithm>

namespace massdb {

// 浮点类型数组转换为字符串
template <class T>
void val_vec_to_str(vector<T>& from, string& to) {
  size_t n = from.size();
  int byte_size = sizeof(T);
  to.resize(n * byte_size);
  size_t index = 0;
  // 这里应该是 T
  for (T val : from) {
    for (int i = 0; i < byte_size; ++i) {
      to[index++] = (reinterpret_cast<char*>(&val))[i];
    }
  }
}

template <class T>
string val_str(T val) {
  int byte_size = sizeof(val);
  string res(byte_size, 0);
  for (int i = 0; i < byte_size; i++) {
    res[i] = (reinterpret_cast<char*>(&val))[i];
  }
  return res;
}

// 头部信息转换为字符串
string MsData::header_to_string(const MsHeaderPtr& ms_header_ptr) {
  string head_string;
  int title_size = (int)ms_header_ptr->getTitle().size();
  // 头文件长度
  head_string += val_str(87 + title_size);
  // int
  // start_index = 0
  head_string += val_str(ms_header_ptr->getMsLevel());
  head_string += val_str(ms_header_ptr->getId());
  head_string += val_str(ms_header_ptr->getFractionId());
  head_string += val_str(ms_header_ptr->getPrecId());
  head_string += val_str(ms_header_ptr->getMsOneId());
  head_string += val_str((int)-1);  // feature_id
  head_string += val_str(ms_header_ptr->getMsOneScan());
  head_string += val_str(ms_header_ptr->getFirstScanNum());
  head_string += val_str(ms_header_ptr->getPrecCharge());
  // double
  // start_index = 36
  head_string += val_str(ms_header_ptr->getPrecSpMz());
  head_string += val_str(ms_header_ptr->getPrecMonoMz());
  head_string += val_str(ms_header_ptr->getPrecInte());
  head_string += val_str((double)-1);  // feature_inte_
  head_string += val_str(ms_header_ptr->getRetentionTime());

  // string
  // start_index = 76
  // 激活方式
  if (ms_header_ptr->getMsLevel() == 1) {
    head_string += "   ";
  } else {
    head_string += ms_header_ptr->getActivationPtr()->getName();
  }

  // start_index = 79
  head_string += val_str(title_size);
  // start_index = 83
  head_string += ms_header_ptr->getTitle();
  return head_string;
}

// 质谱数据转换为字符串
string MsData::peak_list_to_string(const vector<PeakPtr>& peak_list) {
  size_t peak_list_size = peak_list.size();
  vector<double> mass_vec(peak_list_size);
  vector<double> inte_vec(peak_list_size);
  for (size_t i = 0; i < peak_list_size; i++) {
    mass_vec[i] = peak_list[i]->getPosition();
    inte_vec[i] = peak_list[i]->getIntensity();
  }
  string mass_str;
  string inte_str;
  val_vec_to_str(mass_vec, mass_str);
  val_vec_to_str(inte_vec, inte_str);
  return val_str(peak_list_size) + mass_str + inte_str;
}
// Msalgin 数据转换为字符串
string MsData::peak_list_to_string(const vector<DeconvPeakPtr>& peak_list) {
  size_t peak_list_size = peak_list.size();
  vector<double> mass_vec(peak_list_size);
  vector<double> inte_vec(peak_list_size);
  vector<int> charge(peak_list_size);
  for (size_t i = 0; i < peak_list_size; i++) {
    mass_vec[i] = peak_list[i]->getPosition();
    inte_vec[i] = peak_list[i]->getIntensity();
    charge[i] = peak_list[i]->getCharge();
  }
  string mass_str;
  string inte_str;
  string charge_str;
  val_vec_to_str(mass_vec, mass_str);
  val_vec_to_str(inte_vec, inte_str);
  val_vec_to_str(charge, charge_str);
  return val_str(peak_list_size) + mass_str + inte_str + charge_str;
}

PeakPtrVec MsData::string_to_peak_list(const string& data) {
  toppic::PeakPtrVec peak_list;
  const char* p = data.c_str();
  int peak_list_index = *(reinterpret_cast<const int*>(p));
  size_t peak_list_size = *(reinterpret_cast<const size_t*>(p + peak_list_index));
  size_t shift = peak_list_index + sizeof(size_t);
  int double_size = sizeof(double);
  for (size_t i = 0; i < peak_list_size; i++) {
    double mass = *(reinterpret_cast<const double*>(p + shift + i * double_size));
    double intensity = *(reinterpret_cast<const double*>(p + shift + i * double_size + peak_list_size * 8));
    PeakPtr peak_ptr = std::make_shared<Peak>(mass, intensity);
    peak_list.push_back(peak_ptr);
  }
  return peak_list;
}
PeakPtrVec MsData::string_to_peak_list(const char* data) {
  toppic::PeakPtrVec peak_list;
  const char* p = data;
  int peak_list_index = *(reinterpret_cast<const int*>(p));
  size_t peak_list_size = *(reinterpret_cast<const size_t*>(p + peak_list_index));
  size_t shift = peak_list_index + sizeof(size_t);
  int double_size = sizeof(double);
  for (size_t i = 0; i < peak_list_size; i++) {
    double mass = *(reinterpret_cast<const double*>(p + shift + i * double_size));
    double intensity = *(reinterpret_cast<const double*>(p + shift + i * double_size + peak_list_size * 8));
    PeakPtr peak_ptr = std::make_shared<Peak>(mass, intensity);
    peak_list.push_back(peak_ptr);
  }
  return peak_list;
}
// 字符串转换为解卷积数据
DeconvPeakPtrVec MsData::string_to_deconv_ms_peak_list(const string& data) {
  toppic::DeconvPeakPtrVec ms_peak_list;
  const char* p = data.c_str();
  int peak_list_index = *(reinterpret_cast<const int*>(p));
  size_t peak_list_size = *(reinterpret_cast<const size_t*>(p + peak_list_index));
  size_t shift = peak_list_index + sizeof(size_t);
  int double_size = sizeof(double);
  int int_size = sizeof(int);
  for (size_t i = 0; i < peak_list_size; i++) {
    double mass = *(reinterpret_cast<const double*>(p + shift + i * double_size));
    double intensity = *(reinterpret_cast<const double*>(p + shift + i * double_size + peak_list_size * 8));
    int charge = *(reinterpret_cast<const int*>(p + shift + i * int_size + peak_list_size * 8 * 2));
    DeconvPeakPtr peak_ptr = std::make_shared<DeconvPeak>(0, 0, mass, intensity, charge);
    ms_peak_list.push_back(peak_ptr);
  }
  return ms_peak_list;
}
// TODO 如果这里使用的不是 const char* data 似乎有点问题
// get_deconv_ms_data(data.c_str());
// char* 传递给 string 类型遇到 0 会截断
DeconvPeakPtrVec MsData::string_to_deconv_ms_peak_list(const char* data) {
  toppic::DeconvPeakPtrVec ms_peak_list;
  const char* p = data;
  int peak_list_index = *(reinterpret_cast<const int*>(p));
  size_t peak_list_size = *(reinterpret_cast<const size_t*>(p + peak_list_index));
  size_t shift = peak_list_index + sizeof(size_t);
  int double_size = sizeof(double);
  int int_size = sizeof(int);
  for (size_t i = 0; i < peak_list_size; i++) {
    double mass = *(reinterpret_cast<const double*>(p + shift + i * double_size));
    double intensity = *(reinterpret_cast<const double*>(p + shift + i * double_size + peak_list_size * 8));
    int charge = *(reinterpret_cast<const int*>(p + shift + i * int_size + peak_list_size * 8 * 2));
    DeconvPeakPtr peak_ptr = std::make_shared<DeconvPeak>(0, 0, mass, intensity, charge);
    ms_peak_list.push_back(peak_ptr);
  }
  return ms_peak_list;
}

MsHeaderPtr MsData::string_to_header(const string& data) {
  toppic::MsHeaderPtr header_ptr;
  int size_int = sizeof(int);
  int size_double = sizeof(double);
  const char* p = data.c_str();
  int shift = 4;
  header_ptr = std::make_shared<toppic::MsHeader>();
  header_ptr->setMsLevel(*(reinterpret_cast<const int*>(shift + p + size_int * 0)));
  header_ptr->setId(*reinterpret_cast<const int*>(shift + p + size_int * 1));
  header_ptr->setFractionId(*reinterpret_cast<const int*>(shift + p + size_int * 2));
  header_ptr->setPrecId(*reinterpret_cast<const int*>(shift + p + size_int * 3));
  header_ptr->setMsOneId(*reinterpret_cast<const int*>(shift + p + size_int * 4));
  header_ptr->setMsOneScan(*reinterpret_cast<const int*>(shift + p + size_int * 6));
  header_ptr->setScan(*reinterpret_cast<const int*>(shift + p + size_int * 7));
  header_ptr->setPrecCharge(*reinterpret_cast<const int*>(shift + p + size_int * 8));

  header_ptr->setPrecSpMz(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 0));
  header_ptr->setPrecMonoMz(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 1));
  header_ptr->setPrecInte(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 2));
  header_ptr->setRetentionTime(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 4));
  if (header_ptr->getMsLevel() == 2) {
    header_ptr->setActivationPtr(
        ActivationBase::getActivationPtrByName(data.substr(shift + 76, 3)));
  }
  header_ptr->setTitle(
      string(p + 83, *reinterpret_cast<const int*>(shift + p + 79)));
  return header_ptr;
}
MsHeaderPtr MsData::string_to_header(const char* data) {
  toppic::MsHeaderPtr header_ptr;
  int size_int = sizeof(int);
  int size_double = sizeof(double);
  const char* p = data;
  int shift = 4;
  header_ptr = std::make_shared<toppic::MsHeader>();
  header_ptr->setMsLevel(*(reinterpret_cast<const int*>(shift + p + size_int * 0)));
  header_ptr->setId(*reinterpret_cast<const int*>(shift + p + size_int * 1));
  header_ptr->setFractionId(*reinterpret_cast<const int*>(shift + p + size_int * 2));
  header_ptr->setPrecId(*reinterpret_cast<const int*>(shift + p + size_int * 3));
  header_ptr->setMsOneId(*reinterpret_cast<const int*>(shift + p + size_int * 4));
  header_ptr->setMsOneScan(*reinterpret_cast<const int*>(shift + p + size_int * 6));
  header_ptr->setScan(*reinterpret_cast<const int*>(shift + p + size_int * 7));
  header_ptr->setPrecCharge(*reinterpret_cast<const int*>(shift + p + size_int * 8));

  header_ptr->setPrecSpMz(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 0));
  header_ptr->setPrecMonoMz(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 1));
  header_ptr->setPrecInte(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 2));
  header_ptr->setRetentionTime(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 4));
  if (header_ptr->getMsLevel() == 2) {
    header_ptr->setActivationPtr(ActivationBase::getActivationPtrByName(std::string(data + shift + 76, 3)));
  }
  header_ptr->setTitle(
      string(p + 83, *reinterpret_cast<const int*>(shift + p + 79)));
  return header_ptr;
}

// 字符串转换为质谱数据
// bool MsData::string_to_peak_list(const string& data) {
//  const char* p = data.c_str();
//  int peak_list_index = *(reinterpret_cast<const int*>(p));
//  size_t peak_list_size = *(reinterpret_cast<const size_t*>(p + peak_list_index));
//  size_t shift = peak_list_index + sizeof(size_t);
//  int double_size = sizeof(double);
//  for (size_t i = 0; i < peak_list_size; i++) {
//    double mass = *(reinterpret_cast<const double*>(p + shift + i * double_size));
//    double intensity = *(reinterpret_cast<const double*>(p + shift + i * double_size + peak_list_size * 8));
//    PeakPtr peak_ptr = std::make_shared<Peak>(mass, intensity);
//    peak_list_.push_back(peak_ptr);
//  }
//  return true;
//}

//// 字符串转换为头部信息
// bool MsData::string_to_header(const string& data) {
//   int size_int = sizeof(int);
//   int size_double = sizeof(double);
//   const char* p = data.c_str();
//   int shift = 4;
//   header_ptr_ = std::make_shared<toppic::MsHeader>();
//   header_ptr_->setMsLevel(*(reinterpret_cast<const int*>(shift + p + size_int * 0)));
//   header_ptr_->setId(*reinterpret_cast<const int*>(shift + p + size_int * 1));
//   header_ptr_->setFractionId(*reinterpret_cast<const int*>(shift + p + size_int * 2));
//   header_ptr_->setPrecId(*reinterpret_cast<const int*>(shift + p + size_int * 3));
//   header_ptr_->setMsOneId(*reinterpret_cast<const int*>(shift + p + size_int * 4));
//   header_ptr_->setMsOneScan(*reinterpret_cast<const int*>(shift + p + size_int * 6));
//   header_ptr_->setScan(*reinterpret_cast<const int*>(shift + p + size_int * 7));
//   header_ptr_->setPrecCharge(*reinterpret_cast<const int*>(shift + p + size_int * 8));
//
//   header_ptr_->setPrecSpMz(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 0));
//   header_ptr_->setPrecMonoMz(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 1));
//   header_ptr_->setPrecInte(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 2));
//   header_ptr_->setRetentionTime(*reinterpret_cast<const double*>(shift + p + 36 + size_double * 4));
//   if (header_ptr_->getMsLevel() == 2) {
//     header_ptr_->setActivationPtr(
//         ActivationBase::getActivationPtrByName(data.substr(shift + 76, 3)));
//   }
//   header_ptr_->setTitle(
//       string(p + 83, *reinterpret_cast<const int*>(shift + p + 79)));
//   return true;
// }
//// 字符串转换为质谱数据
// bool MsData::string_to_peak_list(const string& data) {
//   const char* p = data.c_str();
//   int peak_list_index = *(reinterpret_cast<const int*>(p));
//   size_t peak_list_size = *(reinterpret_cast<const size_t*>(p + peak_list_index));
//   size_t shift = peak_list_index + sizeof(size_t);
//   int double_size = sizeof(double);
//   for (size_t i = 0; i < peak_list_size; i++) {
//     double mass = *(reinterpret_cast<const double*>(p + shift + i * double_size));
//     double intensity = *(reinterpret_cast<const double*>(p + shift + i * double_size + peak_list_size * 8));
//     PeakPtr peak_ptr = std::make_shared<Peak>(mass, intensity);
//     peak_list_.push_back(peak_ptr);
//   }
//   return true;
// }

}  // namespace massdb