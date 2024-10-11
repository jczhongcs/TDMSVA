//
// Created by Xsakura on 2023/4/20.
//

#ifndef MASSDB_PACKAGE_H
#define MASSDB_PACKAGE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "leveldb/db.h"
#include "massdb.h"
#include "ms/spec/deconv_ms.hpp"
#include "topfd/msreader/raw_ms_group_reader.hpp"
#include "PrSM.h"

namespace massdb {

// 包数据结构
// 负责获取和解析包内数据
class Package {
public:
  enum PackageType {
    RAW,
    DECONVOLUTED,
    IDENTIFIED
  };

  static const std::unordered_map<std::string, int> file_type_map;

  Package(leveldb::DB** db, const leveldb::WriteOptions& writeOptions, const leveldb::ReadOptions& readOptions, const std::string& uuid);

  const std::string& get_uuid() const;
  const std::string& get_name() const;

  void CreatePackge(const std::string& package_name);  // 创建包
  void OpenPackage();                                  // 打开包

  // 写入质谱数据

  static std::string parseHeaderToString(toppic::MsHeaderPtr header_ptr);

  void PutMSFile(const std::string& file_name, bool missing_level_one = false);
  /// @brief 根据索引获取质谱数据
  /// @param index_name 索引名
  /// @param missing_level_one 如果为 true，则不会建立二级质谱和一级质谱之间的关系
  std::vector<toppic::RawMsGroupPtr>  GetMSFile(bool missing_level_one = false);
  std::vector<std::string> GetMSFileKey();
  toppic::RawMsPtr GetMSFileSingleData(int id);

  void PutDeconvolutedMSFile(const std::string& file_name);  // 写入解卷积之后的2级质谱数据
  void GetDeconvolutedMSFile(bool missing_level_one);        // 读取解卷积之后的2级质谱数据
  vector<string> GetDeconvolutedMSFileKey();
  toppic::DeconvMsPtr GetDeconvolutedMSFileSingleData(int id);

  void PutDeconMS1File(const std::string& file_name);  // 写入解卷积之后的1级质谱
  vector<string> GetDeconMS1Key();
  toppic::DeconvMsPtr GetDeconMS1FileSingleData(int id);
  std::vector<std::string> GetMs2ScanByPrsm(vector<vector<string>> raw_ims_data_vec,string proteinName);
  std::set<std::string> GetDm1ScanByMs2(vector<toppic::DeconvMsPtr> chosenMs2Scan);
  std::vector<std::string> GetDm1ScanVecByMs2(vector<toppic::DeconvMsPtr> chosenMs2Vec);

  void PutIdentifiedMSFile(const std::string& file_name);       // 写入鉴定后的质谱数据
  std::vector<std::vector<std::string>> GetIdentifiedMSFile();  // 读取解卷积后的质谱数据
  std::vector<std::string> GetIdentifiedMSFileKey();
  vector<vector<std::string>> GetIdentifiedMSFileKeyByIndex();
  std::string GetIdentifiedMSFileSingleData(int id);
  std::vector<std::string> GetIdentifiedMSFileKeyByMod(vector<vector<string>> raw_ims_data_vec,vector<string> mod_vec);
  PrsmPtrVec GetPrsmVecByMod(vector<vector<string>> raw_ims_data_vec,vector<string> mod_vec,string packageName);
  PrsmPtrVec GetPrsmVecBySeq(vector<vector<string>> raw_ims_data_vec,string proteinSeq,string packageName);
  PrsmPtrVec GetPrsmVecByName(vector<vector<string>> raw_ims_data_vec,string proteinName,string packageName);
  PrsmPtrVec GetPrsmVecAll(vector<vector<string>> raw_ims_data_vec,string packageName);


  std::vector<std::string> GetIdentifiedMSFileKeyByProteinName(vector<vector<string>> raw_ims_data_vec,string proteinName);
  std::vector<std::string> Get_IdMSK_PN_ByPName(vector<vector<string>> raw_ims_data_vec,string proteinName,vector<string> &chosenProName);
  std::vector<std::string> GetIdentifiedMSFileKeyByProteinSeq(vector<vector<string>> raw_ims_data_vec,string proteinSeq);

  void CreateIndex(PackageType type, const std::string& attribute);
  void GetIdentifiedMSFileByIndex(const std::string& attribute, const std::string& value, bool approximate = false);

  bool ExistMSFile();                // 判断是否存在质谱数据
  bool ExistDeconvolutedMSFile();    // 判断是否存在解卷积后的2级质谱数据
  bool ExistIdentifiedMSFile();      // 判断是否存在鉴定后的质谱数据
  bool ExistDeconMS1File();    // 判断是否存在解卷积后的1级质谱数据
  std::vector<bool> ExistAllFile();  // 获取所有文件存在状态

  bool Seek(const std::string& str);  // 判断是否存在索引

  static std::string type_id(PackageType type, const string& id);  // 获取包类型的字符串
  static std::string type_id(PackageType type, int id);            // 获取包类型的字符串

  void DeleteMSFile();
  void DeleteOneMSItem(int id);

  void DeleteDeconvolutedMSFile();
  void DeleteOneDeconvolutedMSItem(int id);
  void DeleteOneDeconMS1Item(int id);

  void DeleteIdentifiedMSFile();
  void DeleteOneIdentifiedMSItem(int id);

  void DeleteDeconMS1File();


  void PutInfo(string being, string tissue, string package_name);

  static bool compareStringsAsIntegers(const std::string a, const std::string b);
private:
  // 包的唯一标识
  std::string uuid_;
  std::string name_;
  // 包，不同文件的集合，不同的数值标识不同的文件
  //  unsigned char package_;
  //  // 包中文件的索引，用于遍历包内容
  //  // 00000000 表示原始数据。类似于 MySQL 中的聚簇索引
  //  // 0XXXXXXX 用于指向原始数据的索引。类似于 MySQL 中的二级索引
  //  // 1XXXXXXX 索引别名，值中存储的人类可读的字符串，用于访问二级索引
  //  unsigned char package_index_;
  //  // 000000 00 表示索引的开始
  //  // 000000 01 后面 data_id_，表示这个数据部分+
  //  // 000000 10 表示索引的结束
  //  unsigned char index_position_;
  //  // 数据排序的 id
  //  long long data_id_;

  //  const massdb::MassDB* massdb_;
  leveldb::DB** db_;
  leveldb::WriteOptions write_options_;
  leveldb::ReadOptions read_options_;
};

}  // namespace massdb

#endif  // MASSDB_PACKAGE_H
