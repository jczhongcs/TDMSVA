//
// Created by Xsakura on 2023/3/27.
//

#include <chrono>
#include <iostream>
#include <random>

#include "common/base/base_data.hpp"
#include "massdb.h"

massdb::MassDB db;
void testsmall();
void testlarge();
void testsingle();
int main() {
  // leveldb 在写入大量数据的时候并不能够保证全局数据的有序性

  // 初始化各种蛋白质相关数据
  toppic::base_data::init();
  db = massdb::MassDB();
  db.Init(R"(E:\test)", true);
  //  testsmall();
  testsingle();
  std::cout << 1 << std::endl;
  return 0;
  //  package->PutMSFile(R"(D:\Users\Xsakura\Desktop\test_data\prsm&proteoform\st_1.mzML)");
  //  //  package->PutDeconvolutedMSFile(R"(D:\Users\Xsakura\Desktop\test_data\prsm&proteoform\st_1_ms2.msalign)");
  //  //  package->PutIdentifiedMSFile(R"(D:\Users\Xsakura\Desktop\test_data\prsm&proteoform\st_1_ms2_toppic_proteoform_single.tsv)");
  //
  //  // 从 1 到 2799 中生成一个随机数
  //  std::random_device rd;
  //  std::mt19937 gen(rd());
  //  std::uniform_int_distribution<> dis(1, 2799);
  //  long long sum = 0;
  //  for (int n = 0; n < 50; ++n) {
  //    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
  //    package->GetMSFileSingleData(dis(gen));
  //    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  //    sum += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
  //    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
  //              << "[ms]" << std::endl;
  // 换算成秒
  //    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count()
  //              << "[s]" << std::endl;
  //    std::cout << dis(gen) << ' ';
  //  }
  //  cout << sum / 50 << endl;
  //  //  // 对 Protein accession 属性创建检索
  //  //  package->CreateIndex(massdb::Package::IDENTIFIED, "Protein accession");
  //  //  // 获取 Protein accession 属性中的 sp|P66191|RL31_SALTY 值
  //  //  package->GetIdentifiedMSFileByIndex("Protein accession", "P68", true);
  //
  //  return 0;
}

void testsingle() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> id(1, 8893);
  massdb::Package* package;
  package = db.OpenPackage("1", true);
//  package->PutMSFile(R"(D:\Users\Xsakura\Desktop\test\1100.mzML)", true);
  long long sum_ms = 0;
  int loop_times = 5;
  // 5 以内随机读取测试
  for (int i = 0; i < loop_times; i++) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    package->GetMSFile(true);
    //    package->GetMSFileSingleData(id(gen));
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    long long diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << "Time difference = " << diff << "[ms]" << std::endl;
    sum_ms += diff;
  }
  std::cout << sum_ms / loop_times << "[ms]" << std::endl;
  cout << "over" << endl;
  delete package;
}

// small id 1-48 随机小文件测试
void testsmall() {
  std::random_device rd;
  std::mt19937 gen(rd());
  massdb::Package* package;
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 20; j++) {
      // 创建包
      package = db.OpenPackage(std::to_string(i * 20 + j), true);
      package->PutMSFile(R"(D:\Users\Xsakura\Desktop\testcase\small.mzML)");
    }
    // 左闭右闭
    std::uniform_int_distribution<> pack(0, (i + 1) * 20 - 1);
    std::uniform_int_distribution<> id(1, 48);
    long long sum = 0;
    for (int i = 0; i < 2; ++i) {
      std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
      // 随机获取一个包络
      package = db.OpenPackage(std::to_string(pack(gen)), false);
      if (package == nullptr) {
        std::cout << "错误" << std::endl;
      }
      // 随机获取当前包络的某一个键值
      auto a = package->GetMSFileSingleData(id(gen));
      // 打开一个包
      //      package->GetMSFile(true);
      std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
      sum += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    }
    std::cout << sum / 2 << "[ms]" << std::endl;
  }
}

void testlarge() {
  std::random_device rd;
  std::mt19937 gen(rd());
  massdb::Package* package;
  for (int i = 0; i < 5; i++) {
    //    for (int j = 0; j < 6; j++) {
    //      // 创建包
    //      package = db.OpenPackage(std::to_string(i * 6 + j), true);
    //      package->PutMSFile(R"(D:\Users\Xsakura\Desktop\testcase\large.mzML)");
    //    }
    // 左闭右闭
    std::uniform_int_distribution<> pack(0, (i + 1) * 6 - 1);
    std::uniform_int_distribution<> id(1, 2779);
    long long sum = 0;
    for (int i = 0; i < 2; ++i) {
      std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
      // 随机获取一个包络
      package = db.OpenPackage(std::to_string(pack(gen)), false);
      if (package == nullptr) {
        std::cout << "错误" << std::endl;
      }
      // 随机获取当前包络的某一个键值
      auto x = package->GetMSFileSingleData(id(gen));
      // 打开一个包
      //      package->GetMSFile(true);
      std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
      sum += std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    }
    std::cout << sum / 2 << "[ms]" << std::endl;
  }
}
