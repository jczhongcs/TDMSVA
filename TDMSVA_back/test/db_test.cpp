//
// Created by Xsakura on 2023/4/20.
//
#include "leveldb/db.h"

#include <iostream>
#include <random>

#include "leveldb/write_batch.h"

int main() {
  // 打开一个 LevelDB 数据库
  leveldb::DB* db;
  leveldb::Options options;
  options.create_if_missing = true;
  leveldb::Status status = leveldb::DB::Open(options, "./mydb", &db);
  if (!status.ok()) {
    std::cerr << "Open leveldb failed: " << status.ToString() << std::endl;
    return 1;
  }

  // 写入一些测试数据
  //    leveldb::WriteOptions write_options;
  //    write_options.sync = true;
  //    status = db->Put(write_options, "2019300001", "小明");
  //    status = db->Put(write_options, "2019300002", "小云");
  //    status = db->Put(write_options, "2019300003", "小花");
  //    status = db->Put(write_options, "2019300004", "小光");
  //    status = db->Put(write_options, "2019男1", "2019300001");
  //    status = db->Put(write_options, "2019女2", "2019300002");
  //    status = db->Put(write_options, "2019女3", "2019300003");
  //    status = db->Put(write_options, "2019男4", "2019300004");

  leveldb::ReadOptions read_options;
  leveldb::Iterator* iter = db->NewIterator(read_options);
  iter->Seek("2019男");

  // 遍历符合条件的键值对
  while (iter->Valid()) {
    if (!iter->key().starts_with("2019男")) break;
    std::string value = iter->value().ToString();
    std::string name;
    db->Get(read_options, iter->value(), &name);
    std::cout << iter->key().ToString() << " " << iter->value().ToString() << " : " << name << std::endl;
    iter->Next();
  }

  // 释放迭代器和数据库资源
  delete iter;
  delete db;

  return 0;
}