//
// Created by Xsakura on 2023/4/20.
//

#include "massdb.h"

#include "util.h"

namespace massdb {

const std::string MassDB::package_list_start = "package_list_00";
const std::string MassDB::package_list_content = "package_list_01";
const std::string MassDB::package_list_end = "package_list_11";

const std::string MassDB::package_index_start = "package_index_00";
const std::string MassDB::package_index_content = "package_index_01";
const std::string MassDB::package_index_end = "package_index_11";

// 负责打开文件

// 关闭数据库接口
MassDB::~MassDB() {
  //  delete db_;
}

// 考虑一下类和指针返回给上级的问题
massdb::Package* MassDB::OpenPackage(const std::string& package_name, bool create_if_missing) {
  if (alias_to_uuid_.count(package_name) == 0) {
    if (create_if_missing == false) {
      return nullptr;
    }
    // 如果不存在当前包，则创建一个新的包
    std::string uuid = Util::get_uuid();
    Package* package = new Package(&db_, leveldb::WriteOptions(), leveldb::ReadOptions(), uuid);
    // 通过别名访问相应的包
    db_->Put(leveldb::WriteOptions(), "package_index_01_" + package_name, uuid);
    alias_to_uuid_[package_name] = uuid;
    return package;
  } else {
    std::string uuid = alias_to_uuid_[package_name];
    Package* package = new Package(&db_, leveldb::WriteOptions(), leveldb::ReadOptions(), uuid);
    return package;
  }
}

// 根据 uuid 返回
// massdb::Package* MassDB::OpenPackage(const std::string& uuid) {
//  leveldb::Status status;
//  leveldb::Iterator* it = db_->NewIterator(leveldb::ReadOptions());
//  it->Seek("package_index_01_" + uuid);
//  if (it->Valid()) {
//    delete it;
//    return new Package(&db_, leveldb::WriteOptions(), leveldb::ReadOptions(), uuid);
//  } else {
//    delete it;
//    return nullptr;
//  }
//}

leveldb::Status MassDB::Init(const std::string& db_path, bool create_if_missing) {
  toppic::base_data::init();

  leveldb::Options options;
  options.create_if_missing = create_if_missing;
  std::string tmp;
  leveldb::Status status = leveldb::DB::Open(options, db_path, &db_);
  if (!status.ok()) {
    return status;
  }
  status = db_->Get(leveldb::ReadOptions(), "package_list_00", &tmp);
  // 如果不存在包列表，则创建
  if (status.IsNotFound()) {
    db_->Put(leveldb::WriteOptions(), "package_list_00", "");
    db_->Put(leveldb::WriteOptions(), "package_list_11", "");
  }
  // 如果额外的信息列
  status = db_->Get(leveldb::ReadOptions(), "package_info_00", &tmp);
  if (status.IsNotFound()) {
    // 索引别名
    db_->Put(leveldb::WriteOptions(), "package_info_00", "");
    db_->Put(leveldb::WriteOptions(), "package_info_11", "");
  }
  if (status.IsNotFound()) {
    // 索引别名
    db_->Put(leveldb::WriteOptions(), "package_index_00", "");
    db_->Put(leveldb::WriteOptions(), "package_index_11", "");
  } else {
    // 加载所有的别名
    leveldb::Iterator* it = db_->NewIterator(leveldb::ReadOptions());
    it->Seek("package_index_00");
    it->Next();
    while (it->key().ToString() < "package_index_11" && it->Valid()) {
      std::string alias = std::string(it->key().data() + 17, it->key().size() - 17);
      std::string uuid = it->value().ToString();
      alias_to_uuid_[alias] = uuid;
      // 这里注意遍历
      it->Next();
    }

    it->Seek("package_info_00");
    it->Next();
    while (it->key().ToString() < "package_info_11" && it->Valid()) {
      vector<string> tmp = massdb::Util::split(std::string(it->key().data() + 16, it->key().size() - 16), "\t");
      package_info_[it->value().ToString()] = tmp;
      // 这里注意遍历
      it->Next();
    }
  };
  return leveldb::Status::OK();
}

// 删除包
void MassDB::DeletePackage(const string& package_name) {
  if (alias_to_uuid_.count(package_name) == 0) {
    return;
  } else {
    string key = massdb::MassDB::package_list_content + "_" + alias_to_uuid_[package_name];
    leveldb::Iterator* it = db_->NewIterator(leveldb::ReadOptions());
    it->Seek(key);
    // 遍历删除所有包含 uuid 的 key
    while (it->Valid()) {
      if ((*it).key().starts_with(key)) {
        db_->Delete(leveldb::WriteOptions(), (*it).key());
        it->Next();
      } else {
        break;
      }
    }
    db_->Delete(leveldb::WriteOptions(), "package_index_01_" + package_name);
  }
}

leveldb::Status MassDB::Get(const leveldb::ReadOptions& options, const string& key, std::string* value) {
  leveldb::Status status = db_->Get(options, key, value);
  return status;
}

std::map<std::string, std::string>* MassDB::get_alis_to_uuid() {
  return &alias_to_uuid_;
}

// 展示所有的包
// void MassDB::list_package() {
//  for (auto& i : alias_to_uuid_) {
//    std::cout << "name: " << i.first << ", id: " << i.second << std::endl;
//  }
//}

// void MassDB::for_list() {
//   leveldb::Iterator* it = db_->NewIterator(leveldb::ReadOptions());
//   it->SeekToFirst();
//   while (it->Valid()) {
//     std::cout << it->key().ToString() << std::endl;
//     it->Next();
//   }
// }

}  // namespace massdb
