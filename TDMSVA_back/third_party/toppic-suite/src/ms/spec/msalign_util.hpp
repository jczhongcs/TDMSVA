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


#ifndef TOPPIC_MS_SPEC_MSALIGN_UTIL_HPP_
#define TOPPIC_MS_SPEC_MSALIGN_UTIL_HPP_

#include "ms/spec/sp_para.hpp"
#include "ms/spec/msalign_reader.hpp"

namespace toppic {

namespace msalign_util {

int countSpNum(const std::string &spectrum_file, SpParaPtr sp_para_ptr);

void geneSpIndex(const std::string &spectrum_file_name, SpParaPtr sp_para_ptr);

int getSpNum(const std::string &spectrum_file_name);

void mergeMsalignFiles(const std::vector<std::string> & spec_file_lst, int N,
                       const std::string & output_file);

}  // namespace msalign_util

}  // namespace toppic
#endif
