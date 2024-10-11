//Copyright (c) 2014 - 2019, The Trustees of Indiana University.
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

#ifndef TOPPIC_COMMON_UTIL_LOGGER_HPP_
#define TOPPIC_COMMON_UTIL_LOGGER_HPP_

#include <iostream>

namespace toppic {

namespace logger {

extern int log_level;
void setLogLevel(int level); 

}

#define LOG_LEVEL_TRACE 1
#define LOG_LEVEL_DEBUG 2
#define LOG_LEVEL_INFO  3
#define LOG_LEVEL_WARN  4
#define LOG_LEVEL_ERROR 5

#define LOG_TRACE(X)                    \
  {                                     \
    if (logger::log_level <= LOG_LEVEL_TRACE) { \
      std::cout << "LOG TRACE: " << __FILE__ << "[" << __LINE__ << "]: " << X << std::endl;     \
    }                                   \
  }

#define LOG_DEBUG(X)                    \
  {                                     \
    if (logger::log_level <= LOG_LEVEL_DEBUG) { \
      std::cout << "LOG DEBUG: " << __FILE__ << "[" << __LINE__ << "]: " << X << std::endl;     \
    }                                   \
  }

#define LOG_INFO(X)                     \
  {                                     \
    if (logger::log_level <= LOG_LEVEL_INFO) {  \
      std::cout << "LOG INFO: " << __FILE__ << "[" << __LINE__ << "]: " << X << std::endl;      \
    }                                   \
  }

#define LOG_WARN(X)                     \
  {                                     \
    if (logger::log_level <= LOG_LEVEL_WARN) {  \
      std::cout << "LOG WARN: " << __FILE__ << "[" << __LINE__ << "]: " << X << std::endl;      \
    }                                   \
  }

#define LOG_ERROR(X)                    \
  {                                     \
    if (logger::log_level <= LOG_LEVEL_ERROR) { \
      std::cout << "LOG ERROR: " << __FILE__ << "[" << __LINE__ << "]: " << X << std::endl;     \
    }                                   \
  }
}

#endif
