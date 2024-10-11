//
// Created by Xsakura on 2023/4/19.
//

#include "prsm_reader.h"

#include "util.h"

namespace massdb {

const std::unordered_map<std::string, int> PrsmReader::prsm_head_map = {
    {"Data file name", 0},
    {"Prsm ID", 1},
    {"Spectrum ID", 2},
    {"Fragmentation", 3},
    {"Scan(s)", 4},
    {"Retention time", 5},
    {"#peaks", 6},
    {"Charge", 7},
    {"Precursor mass", 8},
    {"Adjusted precursor mass", 9},
    {"Proteoform ID", 10},
    {"Feature intensity", 11},
    {"Feature score", 12},
    {"Feature apex time", 13},
    {"#Protein hits", 14},
    {"Protein accession", 15},
    {"Protein description", 16},
    {"First residue", 17},
    {"Last residue", 18},
    {"Special amino acids", 19},
    {"Database protein sequence", 20},
    {"Proteoform", 21},
    {"Proteoform mass", 22},
    {"Protein N-terminal form", 23},
    {"Fixed PTMs", 24},
    {"#unexpected modification", 25},
    {"unexpected modifications", 26},
    {"#variable PTMs", 27},
    {"variable PTMs", 28},
    {"MIScore", 29},
    {"#matched peaks", 30},
    {"#matched fragment ions", 31},
    {"E-value", 32},
    {"Spectrum-level Q-value", 33},
    {"Proteoform-level Q-value", 34},
};

const std::vector<std::string> PrsmReader::prsm_head = {
    "Data file name",
    "Prsm ID",
    "Spectrum ID",
    "Fragmentation",
    "Scan(s)",
    "Retention time",
    "#peaks",
    "Charge",
    "Precursor mass",
    "Adjusted precursor mass",
    "Proteoform ID",
    "Feature intensity",
    "Feature score",
    "Feature apex time",
    "#Protein hits",
    "Protein accession",
    "Protein description",
    "First residue",
    "Last residue",
    "Special amino acids",
    "Database protein sequence",
    "Proteoform",
    "Proteoform mass",
    "Protein N-terminal form",
    "Fixed PTMs",
    "#unexpected modification",
    "unexpected modifications",
    "#variable PTMs",
    "variable PTMs",
    "MIScore",
    "#matched peaks",
    "#matched fragment ions",
    "E-value",
    "Spectrum-level Q-value",
    "Proteoform-level Q-value"};

PrsmReader::PrsmReader(const std::string& file_name) : file_name_(file_name) {
  input_.open(file_name_.c_str(), std::ios::in);
  if (!input_.is_open()) {
    // LOG_ERROR("msalign file  " << file_name << " does not exist.");
    exit(EXIT_FAILURE);
  }
  std::string line;
  bool flag = false;
  while (std::getline(input_, line)) {
    Util::trim(line);
    if (line.empty()) continue;
    if (!flag) {
      if (Util::start_with(line, "Data file name")) flag = true;
    } else {
      prsms.push_back(line);
      prsms_split.push_back(Util::split(line, "\t"));
    }
  }
}

PrsmReader::~PrsmReader() {
  if (input_.is_open()) {
    input_.close();
  }
}
const std::vector<std::string>& PrsmReader::getPrsms() const {
  return prsms;
}
const std::vector<std::vector<std::string>>& PrsmReader::getPrsmsSplit() const {
  return prsms_split;
}

std::vector<std::string> PrsmReader::string_to_prsm(const char* data) {
  return Util::split(data, "\t");
}

std::vector<std::string> PrsmReader::string_to_prsm(const std::string& data) {
  return Util::split(data, "\t");
}

//// 从 tsv 文件中读取出来
// std::string PrsmReader::readOneLine() {
//   std::string line;
//   while (std::getline(input_, line)) {
//     Util::trim(line);
//     if (line.empty() || line[0] == '#' || line.find_first_of("Data file name")) {
//       continue;
//     }
//   }
//   return line;
// }
//
//// 解析 prsm 文件中的一行数据
// std::vector<std::string> PrsmReader::readOnePrsm() {
//   std::string res = readOneLine();
//   if (res.empty()) {
//     input_.close();
//     return {};
//   }
//   std::vector<std::string> prsms = Util::split(res, "\t");
//   return prsms;
// }

}  // namespace massdb