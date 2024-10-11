//
// Created by Xsakura on 2023/4/20.
//

#ifndef MASSDB_MASSDB_H
#define MASSDB_MASSDB_H

#include <map>
#include <memory>
#include <unordered_map>

#include "common/base/base_data.hpp"
#include "leveldb/db.h"
#include "package.h"

namespace massdb {

class Package;

class MassDB {
public:
  static const std::string package_list_start;
  static const std::string package_list_content;
  static const std::string package_list_end;

  static const std::string package_index_start;
  static const std::string package_index_content;
  static const std::string package_index_end;

  explicit MassDB() = default;
  ~MassDB();

  leveldb::Status Init(const std::string& db_path, bool create_if_missing = false);
  Package* OpenPackage(const std::string& package_name, bool create_if_missing = false);
  void DeletePackage(const std::string& package_name);

  //  Package* OpenPackage(const std::string& uuid);

  //  void for_list();
  std::map<std::string, std::string>* get_alis_to_uuid();
  leveldb::Status Get(const leveldb::ReadOptions& options, const std::string& key,
                      std::string* value);

  std::map<std::string, std::vector<std::string>> package_info_;
  std::map<std::string, std::string> alias_to_uuid_;

private:
  leveldb::DB* db_;
};

}  // namespace massdb

#endif  // MASSDB_MASSDB_H
