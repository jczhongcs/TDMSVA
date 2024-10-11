//
// Created by Xsakura on 2023/4/19.
//

#ifndef TOPPIC_PRSM_READER_H
#define TOPPIC_PRSM_READER_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace massdb {
class PrsmReader {
public:
  PrsmReader(const std::string& file_name);

  ~PrsmReader();

  const std::vector<std::string>& getPrsms() const;
  const std::vector<std::vector<std::string>>& getPrsmsSplit() const;


  static std::vector<std::string> string_to_prsm(const char* data);
  static std::vector<std::string> string_to_prsm(const std::string& data);

  static const std::unordered_map<std::string, int> prsm_head_map;
  static const std::vector<std::string> prsm_head;

//  std::string readOneLine();
//
//  std::vector<std::string> readOnePrsm();
//
//  std::string getPrsmValueByName(const std::string& name);

private:
  //  class Prsm {
  //
  //    };
  //    std::string data_file_name_;
  //    int prsm_id_;
  //    std::vector<int> spec_id_;
  //    std::vector<std::string> spec_activations_;
  //    std::vector<std::string> scans_;
  //    std::vector<double> retention_time_;
  //    int peak_num_;
  //    int charge_;
  //    double precursor_mass_;
  //    double adjusted_precursor_mass_;
  //    int proteoform_id_;
  //    double feature_intensity_;
  //    double feature_score_;
  //    double feature_apex_time_;
  //    int protein_hits;
  //    std::string protein_accession_;
  //    std::string protein_description;
  //    int first_residue_;
  //    int last_residue_;
  //    std::string special_amnio_acids_;
  //    std::string database_protein_sequence_;
  //    std::string proteoform_;
  //    double proteoform_mass_;
  //    std::string n_terminal_form_;
  //    int unexcepted_modifications_num_;
  //    std::string unexcepted_modifications_str_;
  //    int variable_ptms_num_;
  //    std::string variable_ptms_str_;
  //    std::string mi_score_;
  //    double matched_peaks_num_;
  //    double matched_fragment_ions_num_;
  //    double e_value_;
  //    double spectrum_level_q_value_;
  //    double proteoform_level_q_value_;

  //  std::string getPrsmValueByName(const std::name);

private:
  std::vector<std::string> prsms;
  std::vector<std::vector<std::string>> prsms_split;

  std::string file_name_;
  std::ifstream input_;
};
}  // namespace massdb

#endif  // TOPPIC_PRSM_READER_H
