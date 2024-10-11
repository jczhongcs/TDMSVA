//
// Created by Xsakura on 2023/4/20.
//

#ifndef MASSDB_UTIL_H
#define MASSDB_UTIL_H

#include <string>
#include <vector>

namespace massdb {
struct Util {
  static std::string get_uuid();

  template <class T>
  static std::string val_str(T val);

  static std::vector<std::string> split(const std::string& s, const std::string& delim);

  static void trim(std::string& s);
  static bool start_with(const std::string& s,const std::string& prefix);
  static int str_int(std::string str);
  static bool isVarPtm(std::string mod);
  static bool checkMod(std::vector<std::string> cur_prsm_mod,std::vector<std::string> mod_vec);
  static std::vector<int> computeLPS(const std::string& A);
  static bool KMPSearch(const std::string& A, const std::string& B);
  static std::string processProFormSeq(std::string proFormSeq);
  static std::string processNterFormSeq(std::string proFormSeq,std::string nterFrom);
  static bool PPMCheck(double A, double B,double PPM);
};

template <class T>
std::string Util::val_str(T val) {
  int byte_size = sizeof(val);
  std::string res(byte_size, 0);
  for (int i = 0; i < byte_size; i++) {
    res[i] = (reinterpret_cast<char*>(&val))[i];
  }
  return res;
}

}  // namespace massdb

#endif  // MASSDB_UTIL_H
