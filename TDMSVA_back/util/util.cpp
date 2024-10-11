//
// Created by Xsakura on 2023/4/20.
//

#include "util.h"

#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
namespace massdb {

std::string Util::get_uuid() {
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  return boost::uuids::to_string(uuid);
}

std::vector<std::string> Util::split(const std::string& s, const std::string& delim) {
  std::vector<std::string> strs;
  boost::split(strs, s, boost::is_any_of(delim));
  return strs;
}

void Util::trim(std::string& s) {
  boost::algorithm::trim(s);
}

bool Util::start_with(const std::string& s, const std::string& prefix) {
  if (s.length() < prefix.size()) {
    return false;
  }
  return s.substr(0, prefix.size()) == prefix;
}

int Util::str_int(std::string str) {
  const char* p = str.data();
  int val = *(reinterpret_cast<const int*>(p));
  return val;
}

bool Util::isVarPtm(std::string mod){
    const std::vector<std::string> varPTM = {"Acetyl","Methyl","Dimethyl","Trimethyl","Phospho"};
    for(auto x:varPTM){
        if(strcmp(mod.c_str(),x.c_str())==0){
            return true;
        }
    }
    return false;
}


bool Util::checkMod(std::vector<std::string> cur_prsm_mod,std::vector<std::string> mod_vec){
    int count = 0;
    for(auto x:mod_vec){
        for(auto y:cur_prsm_mod){
            if(strcmp(x.c_str(),y.c_str())==0){
                count++;
                break;
            }
        }
    }
    if(count == mod_vec.size())
        return true;
    else
        return false;
}


std::vector<int> Util::computeLPS(const std::string& A) {
    int m = A.length();
    std::vector<int> lps(m, 0);
    int len = 0;
    int i = 1;
    while (i < m) {
        if (A[i] == A[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

bool Util::KMPSearch(const std::string& A, const std::string& B) {
    std::vector<int> lps = computeLPS(A);
    int i = 0; // B的索引
    int j = 0; // A的索引
    while (i < B.length()) {
        if (A[j] == B[i]) {
            j++;
            i++;
        }
        if (j == A.length()) {
            return true;
        }
        if (i < B.length() && A[j] != B[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    return false;
}

std::string Util::processProFormSeq(std::string proFormSeq){
    std::string res_seq;
    int i=0;
    while(proFormSeq[i] != '.')
        i++;
    i++;
    while(i < proFormSeq.size()){
        if(proFormSeq[i] == '.' ){
            break;
        }else if(proFormSeq[i] == '-' || proFormSeq[i] == '(' || proFormSeq[i] == ')'){
            i++;
            continue;
        }else if(proFormSeq[i] == '['){
            i++;
            while(proFormSeq[i] != ']' && i < proFormSeq.size()){
                i++;
            }
            if (proFormSeq[i] == ']'){
                i++;
                continue;
            }else{
                break;
            }

        }else{
            res_seq += proFormSeq[i];
            i++;
        }
    }
    return res_seq;
}

std::string Util::processNterFormSeq(std::string proFormSeq,std::string nterFrom){
    const std::string NME_A = "NME_ACETYLATION";
    const std::string M_A = "M_ACETYLATION";
    if(strcmp(nterFrom.c_str(),NME_A.c_str())==0 || strcmp(nterFrom.c_str(),M_A.c_str())==0){
        std::string res_seq;
        bool processFlag = true;
        for(int i=0;i<proFormSeq.size();i++){
            if(proFormSeq[i] == '.' && processFlag){
                res_seq = res_seq + ".(" + proFormSeq[i+10] + ")[Acetyl]";
                i+=10;
                processFlag = false;
            }else{
                res_seq += proFormSeq[i];
            }
        }

        return res_seq;
    }else{
        return proFormSeq;
    }


}

bool Util::PPMCheck(double A, double B,double PPM){
    double difference = std::fabs(A - B);
    // 计算15ppm的阈值
    double ppm_threshold = std::max(A, B) * PPM / 1000000; // 15ppm转换为整数形式
    // 如果差值小于15ppm的阈值，则返回true
    return difference <= ppm_threshold;


//    double difference = std::fabs(A - B);
//
//    return difference <= 2;
}

}  // namespace massdb
