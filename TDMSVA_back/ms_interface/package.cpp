//
// Created by Xsakura on 2023/4/20.
//

#include "package.h"

#include <iostream>

#include "ms/spec/msalign_reader.hpp"
#include "msdata.h"
#include "prsm_reader.h"
#include "topfd/msreader/raw_ms_writer.hpp"
#include "util.h"

namespace massdb {
// TODO: 封装 levedldb 遍历

const std::unordered_map<std::string, int> Package::file_type_map = {
    {"raw", 0},
    {"deconvoluted", 1},
    {"identified", 2},
    {"rms", 0},
    {"dms", 1},
    {"ims", 2},
    {"dm1", 3},
};

Package::Package(leveldb::DB** db, const leveldb::WriteOptions& writeOptions, const leveldb::ReadOptions& readOptions, const string& uuid) {
  db_ = db;
  write_options_ = writeOptions;
  read_options_ = readOptions;
  uuid_ = uuid;
}

// 创建包
void Package::CreatePackge(const std::string& package_name) {
  //  // 包的实际位置
  //  (*db_)->Put(write_options_, "package_list_01_" + uuid_, "");
  //  // 通过别名访问相应的包
  //  (*db_)->Put(write_options_, "package_index_01_" + package_name, uuid_);
}
// TODO: 打开包，并加载里面的内容
void Package::OpenPackage() {
  //  leveldb::Status status = db_->Get();
}

toppic::RawMsPtr get_ms_data(const char* data) {
  toppic::PeakPtrVec peak_list = MsData::string_to_peak_list(data);
  toppic::MsHeaderPtr header = MsData::string_to_header(data);
  toppic::RawMsPtr ms_ptr = std::make_shared<toppic::Ms<PeakPtr>>(header, peak_list);
  return ms_ptr;
}
toppic::DeconvMsPtr get_deconv_ms_data(const char* data) {
  toppic::DeconvPeakPtrVec peak_list = MsData::string_to_deconv_ms_peak_list(data);
  toppic::MsHeaderPtr header = MsData::string_to_header(data);
  toppic::DeconvMsPtr ms_ptr = std::make_shared<toppic::Ms<DeconvPeakPtr>>(header, peak_list);
  return ms_ptr;
}

// 存放和读取 MS 数据
void Package::PutMSFile(const std::string& file_name, bool missing_level_one) {
  // TODO: 实现不固定的 fraction_id
  // toppic::RawMsGroupReaderPtr reader_ptr = std::make_shared<toppic::RawMsGroupReader>(file_name, missing_level_one, nullptr, 3.0, 0);
  toppic::RawMsGroupReaderPtr reader_ptr = std::make_shared<toppic::RawMsGroupReader>(file_name, missing_level_one, 0);
  // 由于字符串在加 "\0" 并不会算入长度，所以目前是以 "\1" 开始
  // 第一个 1 表示这是一个 ms 文件
  // 第二个 1 表示这是这个 ms 文件的默认索引，可能会存在其他的索引
  // 第三个 1 表示这个文件索引开始
  (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\1" + "\1" + "\1", "");

  // 获取一二级质谱组
  toppic::RawMsGroupPtr ms_group_ptr = reader_ptr->getNextMsGroupPtr();
  while (ms_group_ptr != nullptr) {
    toppic::RawMsPtr ms_one_ptr = ms_group_ptr->getMsOnePtr();
    if (ms_one_ptr) {
      toppic::MsHeaderPtr ms_one_header_ptr = ms_one_ptr->getMsHeaderPtr();
      std::string ms_one_str;
      // 转换 ms1 头部数据
      ms_one_str += massdb::MsData::header_to_string(ms_one_header_ptr);
      // 转换 ms1 峰值数据
      ms_one_str += massdb::MsData::peak_list_to_string(ms_one_ptr->getPeakPtrVec());
      // 2 表示现在写入的数据部分
      // id 等于 ms 文件中的 scan 数据，类型为 int
      (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\1" + "\1" + "\2" + Util::val_str(ms_one_header_ptr->getFirstScanNum()), ms_one_str);
    }
    // 读取二级质谱
    toppic::RawMsPtrVec ms_two_ptr_vec = ms_group_ptr->getMsTwoPtrVec();
    for (auto& ms_two_ptr : ms_two_ptr_vec) {
      std::string ms_two_str;
      ms_two_str += massdb::MsData::header_to_string(ms_two_ptr->getMsHeaderPtr());
      ms_two_str += massdb::MsData::peak_list_to_string(ms_two_ptr->getPeakPtrVec());
      (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\1" + "\1" + "\2" + Util::val_str(ms_two_ptr->getMsHeaderPtr()->getFirstScanNum()),
                  ms_two_str);
    }
    ms_group_ptr = reader_ptr->getNextMsGroupPtr();
  }

  //  else {
  //    // 处理二级质谱
  //  }
  (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\1" + "\1" + "\3", "");
}
std::vector<toppic::RawMsGroupPtr> Package::GetMSFile(bool missing_level_one) {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\1" + "\1" + "\1");
  it->Next();
  if (it->Valid() && it->key().ToString() == "package_list_01_" + uuid_ + "\1" + "\1" + "\3") {
    return {};
  }
  std::vector<toppic::RawMsGroupPtr> ms_group_ptr_vec;
  if (missing_level_one) {
    // 读取二级质谱
    while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\1" + "\1" + "\3") {
      // 只读取二级质谱
      // topfd 存储质谱的逻辑是 1 个一级质谱对于着多个二级质谱
      // 一级质谱存储在 RawMsGroup 的第一个参数，但是如果一级质谱缺失的话则这里为 nullptr
      // 二级质谱数组存放在 RawMsGroup 的第二个参数，如果一级质谱缺失的话那么二级质谱数组中只会有一个二级质谱
      // 即表示这是一个相对独立的质谱数据
      toppic::RawMsPtr null_ms_one_ptr(nullptr);
      toppic::RawMsPtr ms_two_ptr = get_ms_data(it->value().data());
      toppic::RawMsPtrVec ms_two_ptr_vec = {ms_two_ptr};
      toppic::RawMsGroupPtr ms_group_ptr = std::make_shared<RawMsGroup>(null_ms_one_ptr, ms_two_ptr_vec);
      ms_group_ptr_vec.push_back(ms_group_ptr);
      it->Next();
    }
  } else {
    // 读取一级质谱
    toppic::RawMsPtr ms_one_ptr = get_ms_data(it->value().data());
    it->Next();
    toppic::RawMsPtrVec ms_two_ptr_vec;
    while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\1" + "\1" + "\3") {
      toppic::RawMsPtr ms_ptr = get_ms_data(it->value().data());
      if (ms_ptr->getMsHeaderPtr()->getMsLevel() == 1) {
        toppic::RawMsGroupPtr ms_group_ptr = std::make_shared<RawMsGroup>(ms_one_ptr, ms_two_ptr_vec);
        ms_group_ptr_vec.push_back(ms_group_ptr);
        ms_two_ptr_vec.clear();
        ms_one_ptr = ms_ptr;
      } else {
        ms_two_ptr_vec.push_back(ms_ptr);
      }
      it->Next();
    }

    toppic::RawMsGroupPtr ms_group_ptr = std::make_shared<RawMsGroup>(ms_one_ptr, ms_two_ptr_vec);
    ms_group_ptr_vec.push_back(ms_group_ptr);
  }
  return ms_group_ptr_vec;
}
std::vector<std::string> Package::GetMSFileKey() {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\1" + "\1" + "\1");
  it->Next();
  std::vector<string> key_list;
  // 16 + 36 + 1 + 1 + 1 = 55
  while (it->Valid()) {
    if (it->key().compare("package_list_01_" + uuid_ + "\1" + "\1" + "\3") >= 0) break;
    key_list.push_back(to_string(Util::str_int(it->key().ToString().substr(55, it->key().size() - 55))));
    it->Next();
  }
  sort(key_list.begin(), key_list.end(), [](const string& a, const string& b) { return stoi(a) < stoi(b); });
  return key_list;
}
toppic::RawMsPtr Package::GetMSFileSingleData(int id) {
  string data;
  leveldb::Status status = (*db_)->Get(read_options_, "package_list_01_" + uuid_ + "\1" + "\1" + "\2" + Util::val_str(id), &data);
  if (status.ok()) {
    return get_ms_data(data.c_str());
  }
  return nullptr;
}
void Package::DeleteMSFile() {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\1" + "\1" + "\1");
  while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\1" + "\1" + "\3") {
    (*db_)->Delete(write_options_, it->key());
    it->Next();
  }
  (*db_)->Delete(write_options_, "package_list_01_" + uuid_ + "\1" + "\1" + "\3");
}

// 存放和读取 DeconvolutedMS 数据
void Package::PutDeconvolutedMSFile(const std::string& file_name) {
  toppic::MsAlignReader msalign_reader(file_name);
  toppic::DeconvMsPtr deconv_ms;
  (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\2" + "\1" + "\1", "");
  while ((deconv_ms = msalign_reader.getNextMs()) != nullptr) {
    std::string dms_str;
    dms_str += massdb::MsData::header_to_string(deconv_ms->getMsHeaderPtr());
    dms_str += massdb::MsData::peak_list_to_string(deconv_ms->getPeakPtrVec());
    (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\2" + "\1" + "\2" + Util::val_str(deconv_ms->getMsHeaderPtr()->getFirstScanNum()), dms_str);
  }
  (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\2" + "\1" + "\3", "");
}
void Package::GetDeconvolutedMSFile(bool missing_level_one) {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\2" + "\1" + "\1");
  it->Next();
  std::vector<toppic::DeconvMsPtr> deconv_ms_vec;
  while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\2" + "\1" + "\3") {
    toppic::MsHeaderPtr header = MsData::string_to_header(it->value().data());
    toppic::DeconvPeakPtrVec peak_list = MsData::string_to_deconv_ms_peak_list(it->value().data());
    toppic::DeconvMsPtr ms_ptr = std::make_shared<toppic::Ms<DeconvPeakPtr>>(header, peak_list);
  }
  //  return deconv_ms_vec;
}
std::vector<std::string> Package::GetDeconvolutedMSFileKey() {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\2" + "\1" + "\1");
  it->Next();
  std::vector<string> key_list;
  // 16 + 36 + 1 + 1 + 1 = 55
  while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\2" + "\1" + "\3") {
    // std::cout << it->value().ToString() << endl;
    // 这里使用 data() 似乎有点问题
    key_list.push_back(to_string(Util::str_int(it->key().ToString().substr(55, it->key().size() - 55))));
    it->Next();
  }
  sort(key_list.begin(), key_list.end(), [](const string& a, const string& b) { return stoi(a) < stoi(b); });
  return key_list;
}
toppic::DeconvMsPtr Package::GetDeconvolutedMSFileSingleData(int id) {
  string data;
  leveldb::Status status = (*db_)->Get(read_options_, "package_list_01_" + uuid_ + "\2" + "\1" + "\2" + Util::val_str(id), &data);
  if (status.ok()) {
    return get_deconv_ms_data(data.c_str());
  }
  return nullptr;
}
void Package::DeleteDeconvolutedMSFile() {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\2" + "\1" + "\1");
  while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\2" + "\1" + "\3") {
    (*db_)->Delete(write_options_, it->key());
    it->Next();
  }
  (*db_)->Delete(write_options_, "package_list_01_" + uuid_ + "\2" + "\1" + "\3");
}

// 存放和读取 DeconMS1 数据
void Package::PutDeconMS1File(const std::string& file_name) {
        toppic::MsAlignReader msalign_reader(file_name);
        toppic::DeconvMsPtr deconv_ms;
        (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\4" + "\1" + "\1", "");
        while ((deconv_ms = msalign_reader.getNextMs()) != nullptr) {
            std::string dms_str;
            dms_str += massdb::MsData::header_to_string(deconv_ms->getMsHeaderPtr());
            dms_str += massdb::MsData::peak_list_to_string(deconv_ms->getPeakPtrVec());
            (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\4" + "\1" + "\2" + Util::val_str(deconv_ms->getMsHeaderPtr()->getFirstScanNum()), dms_str);
        }
        (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\4" + "\1" + "\3", "");
}
std::vector<std::string> Package::GetDeconMS1Key() {
    leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
    it->Seek("package_list_01_" + uuid_ + "\4" + "\1" + "\1");
    it->Next();
    std::vector<string> key_list;
    // 16 + 36 + 1 + 1 + 1 = 55
    while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\4" + "\1" + "\3") {
        // std::cout << it->value().ToString() << endl;
        // 这里使用 data() 似乎有点问题
        key_list.push_back(to_string(Util::str_int(it->key().ToString().substr(55, it->key().size() - 55))));
        it->Next();
    }
    sort(key_list.begin(), key_list.end(), [](const string& a, const string& b) { return stoi(a) < stoi(b); });
    return key_list;
}
void Package::DeleteDeconMS1File() {
    leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
    it->Seek("package_list_01_" + uuid_ + "\4" + "\1" + "\1");
    while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\4" + "\1" + "\3") {
        (*db_)->Delete(write_options_, it->key());
        it->Next();
    }
    (*db_)->Delete(write_options_, "package_list_01_" + uuid_ + "\4" + "\1" + "\3");
}
toppic::DeconvMsPtr Package::GetDeconMS1FileSingleData(int id) {
    string data;
    leveldb::Status status = (*db_)->Get(read_options_, "package_list_01_" + uuid_ + "\4" + "\1" + "\2" + Util::val_str(id), &data);
    if (status.ok()) {
        return get_deconv_ms_data(data.c_str());
    }
    return nullptr;
}

// 存放和读取 IdentifiedMS 数据
void Package::PutIdentifiedMSFile(const std::string& file_name) {
  massdb::PrsmReader prsm_reader(file_name);
  (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\3" + "\1" + "\1", "");
  // 索引
  (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\3" + "\2" + "Protein accession" + "\1", "");
  std::vector<std::vector<std::string>> prsm = prsm_reader.getPrsmsSplit();
  int prsm_num = prsm.size();
  for (int i = 0; i < prsm_num; i++) {
    std::string prsm_id = prsm[i][PrsmReader::prsm_head_map.at("Prsm ID")];              // 1
    std::string accession = prsm[i][PrsmReader::prsm_head_map.at("Protein accession")];  // 15

    (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\3" + "\1" + "\2" + Util::val_str(stoi(prsm_id)), prsm_reader.getPrsms()[i]);
    (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\3" + "\2" + "Protein accession" + "\2" + accession + "\xff" + Util::val_str(stoi(prsm_id)), prsm_reader.getPrsms()[i]);
  }
  (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\3" + "\2" + "Protein accession" + "\3", "");
  (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\3" + "\1" + "\3", "");
}
std::vector<std::vector<std::string>> Package::GetIdentifiedMSFile() {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\3" + "\1" + "\1");
  it->Next();
  std::vector<std::vector<string>> prsm_vec;
  while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\3" + "\1" + "\3") {
    // std::cout << it->value().ToString() << endl;
    // 这里使用 data() 似乎有点问题
    prsm_vec.push_back(PrsmReader::string_to_prsm(it->value().ToString()));
    it->Next();
  }
  return prsm_vec;
}
std::vector<std::string> Package::GetIdentifiedMSFileKey() {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\3" + "\1" + "\1");
  it->Next();
  std::vector<string> prsm_id_vec;
  // 16 + 36 + 1 + 1 + 1 = 55
  while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\3" + "\1" + "\3") {
    prsm_id_vec.push_back(to_string(Util::str_int(it->key().ToString().substr(55, it->key().size() - 55))));
    it->Next();
  }
    sort(prsm_id_vec.begin(), prsm_id_vec.end(),compareStringsAsIntegers);
  return prsm_id_vec;
}
std::vector<std::vector<std::string>> Package::GetIdentifiedMSFileKeyByIndex() {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\3" + "\2" + "Protein accession" + "\1");
  it->Next();
  std::vector<std::vector<std::string>> prsm_id_vec;

  while (it->Valid()) {
    // 72
    if (it->key().starts_with("package_list_01_" + uuid_ + "\3" + "\2" + "Protein accession" + "\3")) {
      break;
    }
    string key = it->key().ToString();
    int index = 72;
    while (key[index] != '\xff') index++;
    string accession = key.substr(72, index - 72);
    string prsm_id = to_string(Util::str_int(key.substr(index + 1, key.size() - index - 1)));
    prsm_id_vec.push_back({prsm_id, accession});
    it->Next();
  }
  return prsm_id_vec;
}
std::string Package::GetIdentifiedMSFileSingleData(int id) {
  string data;
  leveldb::Status status = (*db_)->Get(read_options_, "package_list_01_" + uuid_ + "\3" + "\1" + "\2" + Util::val_str(id), &data);
  return data;
}
void Package::DeleteIdentifiedMSFile() {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek("package_list_01_" + uuid_ + "\3" + "\1" + "\1");
  while (it->Valid()) {
    if (!it->key().starts_with("package_list_01_" + uuid_ + "\3")) break;
    (*db_)->Delete(write_options_, it->key());
    it->Next();
  }
}

// 根据 attribute 创建索引
void Package::CreateIndex(Package::PackageType type, const std::string& attribute) {
  switch (type) {
    case Package::RAW:
      break;
    case Package::DECONVOLUTED:
      break;
    case Package::IDENTIFIED:
      // TODO: 检查存在的索引
      // "\3" 表示这是一个鉴定后的 tsv 文件
      // "\2" 表示这是一个索引
      if (PrsmReader::prsm_head_map.count(attribute)) {
        (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\3" + "\2" + attribute + "\1", "");
        (*db_)->Put(write_options_, "package_list_01_" + uuid_ + "\3" + "\2" + attribute + "\3", "");
        leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
        it->Seek("package_list_01_" + uuid_ + "\3" + "\1" + "\1");  // 找原始的数据
        it->Next();
        while (it->Valid() && it->key().ToString() < "package_list_01_" + uuid_ + "\3" + "\1" + "\3") {
          // std::cout << it->value().ToString() << endl;
          // 这里使用 data() 似乎有点问题
          vector<string> prsm = PrsmReader::string_to_prsm(it->value().ToString());
          std::string key = "package_list_01_" + uuid_ + "\3" + "\2" + attribute + "\2" + prsm[PrsmReader::prsm_head_map.at(attribute)] + "_" + prsm[PrsmReader::prsm_head_map.at("Prsm ID")];
          (*db_)->Put(write_options_, key, it->key());
          it->Next();
        }

      } else {
        std::cerr << "不存在此属性" << std::endl;
      }
      break;
  }
}
// 根据索引获取值
void Package::GetIdentifiedMSFileByIndex(const std::string& attribute, const std::string& value, bool approximate) {
  // 如果 attribute 为空，则获取原始的数据
  if (attribute.empty()) {
    GetIdentifiedMSFile();
  } else {
    std::vector<std::vector<std::string>> res;
    leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
    it->Seek("package_list_01_" + uuid_ + "\3" + "\2" + attribute + "\2" + value);
    while (it->Valid()) {
      std::string key = it->key().ToString();
      if (!Util::start_with(it->key().ToString(), "package_list_01_" + uuid_ + "\3" + "\2" + attribute + "\2" + value)) {
        break;
      }
      std::string prsm;
      (*db_)->Get(read_options_, it->value(), &prsm);
      res.push_back(PrsmReader::string_to_prsm(prsm));
      it->Next();
    }
    std::cout << res.size() << std::endl;
  }
}
// 判断是否存在相应的文件
bool Package::ExistMSFile() {
  return Seek("package_list_01_" + uuid_ + "\1" + "\1" + "\1");
}
bool Package::ExistDeconvolutedMSFile() {
  return Seek("package_list_01_" + uuid_ + "\2" + "\1" + "\1");
}
bool Package::ExistIdentifiedMSFile() {
  return Seek("package_list_01_" + uuid_ + "\3" + "\1" + "\1");
}
bool Package::ExistDeconMS1File() {
    return Seek("package_list_01_" + uuid_ + "\4" + "\1" + "\1");
}

std::vector<bool> Package::ExistAllFile() {
  std::vector<bool> res;
  res.push_back(ExistMSFile());
  res.push_back(ExistDeconvolutedMSFile());
  res.push_back(ExistIdentifiedMSFile());
  res.push_back(ExistDeconMS1File());
  return res;
}

bool Package::Seek(const string& str) {
  leveldb::Iterator* it = (*db_)->NewIterator(read_options_);
  it->Seek(str);
  if (it->Valid() && it->key().ToString() == str) {
    delete it;
    return true;
  } else {
    delete it;
    return false;
  }
}
const std::string& Package::get_uuid() const {
  return uuid_;
}

const std::string& Package::get_name() const {
  return name_;
}

std::string Package::parseHeaderToString(toppic::MsHeaderPtr header_ptr) {
  std::stringstream s;

  s << std::fixed;
  s << "ID=" << header_ptr->getId() << std::endl;
  s << "FRACTION_ID=" << header_ptr->getFractionId() << std::endl;
  //  s << "FILE_NAME=" << header_ptr->getFileName() << std::endl;
  s << "SCANS=" << header_ptr->getScansString() << std::endl;
  s << "RETENTION_TIME=" << std::setprecision(2)
    << header_ptr->getRetentionTime() << std::endl;
  s << "LEVEL=" << header_ptr->getMsLevel() << std::endl;

  if (header_ptr->getMsLevel() > 1) {
    if (header_ptr->getActivationPtr() != nullptr) {
      s << "ACTIVATION=" << header_ptr->getActivationPtr()->getName() << std::endl;
    }
    s << "MS_ONE_ID=" << header_ptr->getMsOneId() << std::endl;
    s << "MS_ONE_SCAN=" << header_ptr->getMsOneScan() << std::endl;
    s << "PRECURSOR_MZ=" << std::setprecision(5)
      << header_ptr->getPrecMonoMz() << std::endl;
    s << "PRECURSOR_CHARGE=" << header_ptr->getPrecCharge() << std::endl;
    s << "PRECURSOR_MASS=" << std::setprecision(5)
      << header_ptr->getPrecMonoMass() << std::endl;
    s << "PRECURSOR_INTENSITY=" << std::setprecision(2)
      << header_ptr->getPrecInte() << std::endl;
    /*
    if (header_ptr->getFeatureId() >= 0) {
      output_ << "FEATURE_ID=" << header_ptr->getFeatureId() << std::endl;
      output_ << "FEATURE_INTENSITY=" << std::setprecision(2)
          << header_ptr->getFeatureInte() << std::endl;
    }
    */
  }

  return s.str();
}

std::string Package::type_id(Package::PackageType type, const string& str) {
  std::string s(1, type + 1);
  int i = Util::str_int(str);
  return s + '\1' + Util::val_str(str);
}
std::string Package::type_id(Package::PackageType type, int id) {
  std::string s(1, type + 1);
  return s + '\1' + Util::val_str(id);
}

void Package::DeleteOneMSItem(int id) {
  (*db_)->Delete(write_options_, "package_list_01_" + uuid_ + "\1" + "\1" + "\2" + Util::val_str(id));
}
void Package::DeleteOneDeconvolutedMSItem(int id) {
  (*db_)->Delete(write_options_, "package_list_01_" + uuid_ + "\2" + "\1" + "\2" + Util::val_str(id));
}
void Package::DeleteOneDeconMS1Item(int id) {
    (*db_)->Delete(write_options_, "package_list_01_" + uuid_ + "\4" + "\1" + "\2" + Util::val_str(id));
}
void Package::DeleteOneIdentifiedMSItem(int id) {
  (*db_)->Delete(write_options_, "package_list_01_" + uuid_ + "\3" + "\1" + "\2" + Util::val_str(id));
}
void Package::PutInfo(string being, string tissue, string package_name) {
  (*db_)->Put(write_options_, "package_info_00_" + being + "\t" + tissue + "\t" + package_name, uuid_);
}

std::vector<std::string> Package::GetIdentifiedMSFileKeyByMod(vector<vector<string>> raw_ims_data_vec,vector<string> mod_vec){
    vector<string> prsm_id_vec;
    if(mod_vec.size()==1 && mod_vec[0]=="NoPtm"){//无修饰
        for(auto raw_ims_data:raw_ims_data_vec){
            string prsmId = raw_ims_data[1];
            string proSeq = raw_ims_data[21];
            vector<string> cur_prsm_mod;
            int i = 0;
            while(i < proSeq.size()){
                if(proSeq[i] != '['){
                    i++;
                    continue;
                }else{
                    break;
                }
            }
            if(proSeq[i]!='['){
                prsm_id_vec.push_back(prsmId);
            }
        }//for raw_ims_data_vec
    }
    else{//有修饰
        for(auto raw_ims_data:raw_ims_data_vec){
            string prsmId = raw_ims_data[1];
            string proSeq = raw_ims_data[21];
            vector<string> cur_prsm_mod;
            int i = 0;
            while(i < proSeq.size()){
                if(proSeq[i] != '['){
                    i++;
                    continue;
                }else{
                    i++;
                    int j = i;
                    string cur_single_mod = "";
                    while(proSeq[j] != ']'){
                        if(proSeq[j] !=';'){
                            cur_single_mod += proSeq[j];
                            j++;
                        }else{
                            cur_prsm_mod.push_back(cur_single_mod);
                            cur_single_mod="";
                            j++;
                        }
                    }
                    cur_prsm_mod.push_back(cur_single_mod);
                    cur_single_mod="";
                    i = j+1;
                }
            }
            for(i=0;i<cur_prsm_mod.size();i++){
                if(!Util::isVarPtm(cur_prsm_mod[i])){
                    cur_prsm_mod[i] = "Unknown";
                }
            }
            if(Util::checkMod(cur_prsm_mod,mod_vec)){
                prsm_id_vec.push_back(prsmId);
            }
        }//for raw_ims_data_vec
    }
    sort(prsm_id_vec.begin(), prsm_id_vec.end(),compareStringsAsIntegers);
    return prsm_id_vec;
}

PrsmPtrVec Package::GetPrsmVecByMod(vector<vector<string>> raw_ims_data_vec,vector<string> mod_vec,string packageName){
    PrsmPtrVec prsmPtrVec;
    if(mod_vec.size()==1 && mod_vec[0]=="NoPtm"){//无修饰
        for(auto raw_ims_data:raw_ims_data_vec){
            string proSeq = raw_ims_data[21];
            int i = 0;
            while(i < proSeq.size()){
                if(proSeq[i] != '['){
                    i++;
                    continue;
                }else{
                    break;
                }
            }
            if(proSeq[i]!='['){
                string prsmId = raw_ims_data[1];
                string proteinName = raw_ims_data[15];
                string evalue = raw_ims_data[32];
                PrsmPtr prsmPtr = std::make_shared<PrSM>(packageName,prsmId,proteinName,evalue);
                prsmPtrVec.push_back(prsmPtr);
            }
        }//for raw_ims_data_vec
    }
    else{//有修饰
        for(auto raw_ims_data:raw_ims_data_vec){
            string proSeq = raw_ims_data[21];
            vector<string> cur_prsm_mod;
            int i = 0;
            while(i < proSeq.size()){
                if(proSeq[i] != '['){
                    i++;
                    continue;
                }else{
                    i++;
                    int j = i;
                    string cur_single_mod = "";
                    while(proSeq[j] != ']'){
                        if(proSeq[j] !=';'){
                            cur_single_mod += proSeq[j];
                            j++;
                        }else{
                            cur_prsm_mod.push_back(cur_single_mod);
                            cur_single_mod="";
                            j++;
                        }
                    }
                    cur_prsm_mod.push_back(cur_single_mod);
                    cur_single_mod="";
                    i = j+1;
                }
            }
            for(i=0;i<cur_prsm_mod.size();i++){
                if(!Util::isVarPtm(cur_prsm_mod[i])){
                    cur_prsm_mod[i] = "Unknown";
                }
            }
            if(Util::checkMod(cur_prsm_mod,mod_vec)){
                string prsmId = raw_ims_data[1];
                string proteinName = raw_ims_data[15];
                string evalue = raw_ims_data[32];
                PrsmPtr prsmPtr = std::make_shared<PrSM>(packageName,prsmId,proteinName,evalue);
                prsmPtrVec.push_back(prsmPtr);
            }
        }//for raw_ims_data_vec
    }

    return prsmPtrVec;
}

PrsmPtrVec Package::GetPrsmVecBySeq(vector<vector<string>> raw_ims_data_vec,string proteinSeq,string packageName){
        PrsmPtrVec prsmPtrVec;
        for(auto raw_ims_data:raw_ims_data_vec){
            string cur_proSeq = raw_ims_data[21];
            string database_seq = Util::processProFormSeq(cur_proSeq);
            if(Util::KMPSearch(proteinSeq,database_seq)){
                string prsmId = raw_ims_data[1];
                string proteinName = raw_ims_data[15];
                string evalue = raw_ims_data[32];
                PrsmPtr prsmPtr = std::make_shared<PrSM>(packageName,prsmId,proteinName,evalue);
                prsmPtrVec.push_back(prsmPtr);
            }
        }
        return prsmPtrVec;
    }

PrsmPtrVec Package::GetPrsmVecByName(vector<vector<string>> raw_ims_data_vec,string proteinName,string packageName){
    PrsmPtrVec prsmPtrVec;
    for(auto raw_ims_data:raw_ims_data_vec){
        string cur_proName = raw_ims_data[15];
        string proSeq = raw_ims_data[21];
        if(Util::KMPSearch(proteinName,cur_proName)){
            string prsmId = raw_ims_data[1];
            string proteinName = raw_ims_data[15];
            string evalue = raw_ims_data[32];
            PrsmPtr prsmPtr = std::make_shared<PrSM>(packageName,prsmId,proteinName,evalue);
            prsmPtrVec.push_back(prsmPtr);
        }
    }
    return prsmPtrVec;
}

PrsmPtrVec Package::GetPrsmVecAll(vector<vector<string>> raw_ims_data_vec,string packageName){
    PrsmPtrVec prsmPtrVec;
    for(auto raw_ims_data:raw_ims_data_vec){
        string prsmId = raw_ims_data[1];
        string proteinName = raw_ims_data[15];
        string evalue = raw_ims_data[32];
        PrsmPtr prsmPtr = std::make_shared<PrSM>(packageName,prsmId,proteinName,evalue);
        prsmPtrVec.push_back(prsmPtr);
    }
    return prsmPtrVec;
}


std::vector<std::string> Package::GetIdentifiedMSFileKeyByProteinName(vector<vector<string>> raw_ims_data_vec,string proteinName){
    vector<string> prsm_id_vec;
    for(auto raw_ims_data:raw_ims_data_vec){
        string prsmId = raw_ims_data[1];
        string cur_proName = raw_ims_data[15];
        string proSeq = raw_ims_data[21];
        if(Util::KMPSearch(proteinName,cur_proName))
            prsm_id_vec.push_back(prsmId);
    }
    sort(prsm_id_vec.begin(), prsm_id_vec.end(),compareStringsAsIntegers);
    return prsm_id_vec;
}

std::vector<std::string> Package::Get_IdMSK_PN_ByPName(vector<vector<string>> raw_ims_data_vec,string proteinName,vector<string> &chosenProName){
    vector<string> prsm_id_vec;
    for(auto raw_ims_data:raw_ims_data_vec){
        string prsmId = raw_ims_data[1];
        string cur_proName = raw_ims_data[15];
        string proSeq = raw_ims_data[21];
        if(Util::KMPSearch(proteinName,cur_proName)){
            prsm_id_vec.push_back(prsmId);
            chosenProName.push_back(cur_proName);
        }
    }
    sort(prsm_id_vec.begin(), prsm_id_vec.end(),compareStringsAsIntegers);
    return prsm_id_vec;
}

std::vector<std::string> Package::GetIdentifiedMSFileKeyByProteinSeq(vector<vector<string>> raw_ims_data_vec,string proteinSeq){
    vector<string> prsm_id_vec;
    for(auto raw_ims_data:raw_ims_data_vec){
        string prsmId = raw_ims_data[1];
        string cur_proSeq = raw_ims_data[21];
        string database_seq = Util::processProFormSeq(cur_proSeq);
        if(Util::KMPSearch(proteinSeq,database_seq))
            prsm_id_vec.push_back(prsmId);
    }
    sort(prsm_id_vec.begin(), prsm_id_vec.end(),compareStringsAsIntegers);
    return prsm_id_vec;
}

bool Package::compareStringsAsIntegers(const std::string a, const std::string b) {
        try {
            // 尝试将字符串转换为整数
            int numA = std::stoi(a.c_str());
            int numB = std::stoi(b.c_str());
            // 如果转换成功，比较整数大小
            return numA < numB;
        } catch (const std::invalid_argument&) {
            // 如果字符串不是有效的整数，认为是无效输入，可以根据需要处理
            // 这里选择抛出异常，也可以选择其他逻辑，比如比较字符串本身
            throw;
        } catch (const std::out_of_range&) {
            // 如果整数超出了int的范围，同样处理异常
            throw;
        }
    }

std::vector<std::string> Package::GetMs2ScanByPrsm(vector<vector<string>> raw_ims_data_vec,string proteinName){
    vector<string> Ms2ScanVec;
    for(auto raw_ims_data:raw_ims_data_vec){
        string prsmId = raw_ims_data[1];
        string ms2scan = raw_ims_data[4];
        string cur_proName = raw_ims_data[15];
        if(Util::KMPSearch(proteinName,cur_proName))
            Ms2ScanVec.push_back(ms2scan);
    }
    sort(Ms2ScanVec.begin(), Ms2ScanVec.end(),compareStringsAsIntegers);
    return Ms2ScanVec;
}

std::set<std::string> Package::GetDm1ScanByMs2(vector<toppic::DeconvMsPtr> chosenMs2Vec){
    set<string> chosenDm1IdVec;
    for(auto ms2Data:chosenMs2Vec){
        chosenDm1IdVec.insert(to_string(ms2Data->getMsHeaderPtr()->getMsOneScan()));
    }
    return chosenDm1IdVec;
}

std::vector<std::string> Package::GetDm1ScanVecByMs2(vector<toppic::DeconvMsPtr> chosenMs2Vec){
    vector<string> chosenDm1IdVec;
    for(auto ms2Data:chosenMs2Vec){
        chosenDm1IdVec.push_back(to_string(ms2Data->getMsHeaderPtr()->getMsOneScan()));
    }
    return chosenDm1IdVec;
}


}  // namespace massdb
