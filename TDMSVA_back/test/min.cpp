////
//// Created by Xsakura on 2023/4/17.
////
//
// #include <iostream>
// #include <string>
// #include <vector>
// #include
//
// using namespace std;
//
//// 浮点类型数组转换为字符串
// void double_vec_to_str(vector<double>& from, string& to) {
//   size_t n = from.size();
//   to.resize(n * 8);
//   int index = 0;
//   for (double val : from) {
//     for (int i = 0; i < 8; ++i) {
//       to[index++] = (reinterpret_cast<char*>(&val))[i];
//     }
//   }
// }
//
// template <class T>
// string val_str(T val) {
//   int byte_size = sizeof(val);
//   string res(byte_size, 0);
//   for (int i = 0; i < byte_size; i++) {
//     res[i] = (reinterpret_cast<char*>(&val))[i];
//   }
//   return res;
// }
//
// int main() {
//   //  vector<double> test = {
//   //      112.101,
//   //      131.21};
//   //  string result;
//   //  double_vec_to_str(test, result);
//   //  const char* c = result.c_str();
//   //  for (size_t i = 0; i < test.size(); i++) {
//   //    double mass = *(reinterpret_cast<const double*>(c + i * 8));
//   //    cout << mass << endl;
//   //  }
//
// }

#include <iostream>

#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"

int main() {
  // 36
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  std::string uuid_str = boost::uuids::to_string(uuid);
  std::cout << "UUID as string: " << uuid_str << std::endl;
  std::cout << uuid_str.size() << std::endl;
}
