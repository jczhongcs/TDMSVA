//
// Created by Xsakura on 2023/5/1.
//

#include "network_server.h"

#include <fstream>
#include <iostream>
#include <mod_total.h>
#include <deMs1.h>
#include "prsm_reader.h"

#include "common/base/base_data.hpp"
#include "crow.h"
#include "crow/middlewares/cors.h"
#include "leveldb/db.h"
#include "massdb.h"
#include "ms_writer.h"
#include "util.h"
#include "PrSM.h"

int main() {
  // 初始化各种蛋白质相关数据
  toppic::base_data::init();
  massdb::MassDB db = massdb::MassDB();
  db.Init(R"(E:\PNAdb)", false);
//  db.Init(R"(E:\test)", false);
  massdb::Package* package = nullptr;

  crow::App<crow::CORSHandler> app;
  // 配置中间层处理 cors
  auto& cors = app.get_middleware<crow::CORSHandler>();
  cors.global()
      .origin("*")
      .headers("origin, x-requested-with, accept, access-control-allow-origin, authorization, content-type")
      .methods("POST"_method, "GET"_method, "PUT"_method, "DELETE"_method, "PATCH"_method, "OPTIONS"_method)
      .allow_credentials();

  // 从数据库中获取所有的包
  CROW_ROUTE(app, "/database")
      .methods("POST"_method)([&](crow::request req) {
        string package_list = massdb::data2json::json_package_list(db.get_alis_to_uuid());
        // 构建 Crow 响应对象
        crow::response response(package_list);
        return response;
      });

  // 从数据库中获取当前包的文件列表
  CROW_ROUTE(app, "/database/<string>")
      .methods("POST"_method)([&](const crow::request& req, string package_name) {
        if (package == nullptr || (*db.get_alis_to_uuid())[package_name] != package->get_uuid()) {
          package = db.OpenPackage(package_name, false);
        }
        crow::response response;
        // TODO 返回不存在该包
        if (package == nullptr) {
        } else {
          std::string package_list = massdb::data2json::json_ms_file_list(package->ExistAllFile());
          response.write(package_list);
          return response;
        }
        // 构建 Crow 响应对象
      });

  // 从数据库中获取当前包的文件列表
  CROW_ROUTE(app, "/database/<string>/<string>")
      .methods("POST"_method)([&](const crow::request& req, string package_name, string file_type) {
        if (package == nullptr || (*db.get_alis_to_uuid())[package_name] != package->get_uuid()) {
          package = db.OpenPackage(package_name, false);
        }
        package = db.OpenPackage(package_name, false);
        crow::response response;
        // 未找到这个文件
        if (massdb::Package::file_type_map.count(file_type) == 0) {
          crow::response response;
          response.code = 404;
          response.write("Not Found");
          return response;
        } else {
          string res;
          switch (massdb::Package::file_type_map.at(file_type)) {
            case 0: {
              res = massdb::data2json::json_rms_key_list(package->GetMSFileKey());
              break;
            }
            case 1: {
              res = massdb::data2json::json_dms_key_list(package->GetDeconvolutedMSFileKey());
              break;
            }
            case 2: {
              res = massdb::data2json::json_ims_key_list(package->GetIdentifiedMSFileKey());
              break;
            }
            case 3: {
                res = massdb::data2json::json_dm1_key_list(package->GetDeconMS1Key());
                break;
            }
          }
          response.write(res);
        }
        return response;
        // 构建 Crow 响应对象
      });

  CROW_ROUTE(app, "/database/<string>/<string>/<string>")
      .methods("POST"_method)([&](const crow::request& req, string package_name, string file_type, string id) {
        if (package == nullptr || (*db.get_alis_to_uuid())[package_name] != package->get_uuid()) {
          package = db.OpenPackage(package_name, false);
        }
        crow::response response;
        // 未找到这个文件
        if (massdb::Package::file_type_map.count(file_type) == 0) {
          response.code = 404;
          response.write("Not Found");
          return response;
        } else {
          switch (massdb::Package::file_type_map.at(file_type)) {
            case 0: {
              auto&& raw_rms_data = package->GetMSFileSingleData(stoi(id));
              string res = massdb::data2json::json_rms_single_data(raw_rms_data);
              response.write(res);
              break;
            }
            case 1: {
              auto&& raw_dms_data = package->GetDeconvolutedMSFileSingleData(stoi(id));
              string res = massdb::data2json::json_dms_single_data(raw_dms_data);
              response.write(res);
              break;
            }
            case 2: {
              string&& raw_ims_data = package->GetIdentifiedMSFileSingleData(stoi(id));
              string res = massdb::data2json::json_ims_single_data(raw_ims_data);
              response.write(res);

              break;
            }
            case 3: {
                auto&& raw_dms_data = package->GetDeconMS1FileSingleData(stoi(id));
                string res = massdb::data2json::json_dm1_single_data(raw_dms_data);
                response.write(res);
                break;
            }

          }
        }
        return response;
        // 构建 Crow 响应对象
      });



    ///检索修饰，返回带修饰的ID
    ///包名/文件类型/修饰名
    CROW_ROUTE(app, "/database/selectMod/<string>/<string>/<string>")
        .methods("POST"_method)([&](const crow::request& req, string package_name, string file_type, string mods_string) {
          if (package == nullptr || (*db.get_alis_to_uuid())[package_name] != package->get_uuid()) {
            package = db.OpenPackage(package_name, false);
          }
          crow::response response;
          // 未找到这个文件
          if (massdb::Package::file_type_map.count(file_type) == 0) {
            response.code = 404;
            response.write("Not Found");
            return response;
          } else {
            string res;
            switch (massdb::Package::file_type_map.at(file_type)) {
              case 2: {
                  vector<string> mod_vec = massdb::Util::split(mods_string,",");
                  string tempstr = "NoSelect";
                  if(mod_vec.size()==1 && strcmp(mod_vec[0].c_str(),tempstr.c_str())==0){
                      res = massdb::data2json::json_ims_key_list(package->GetIdentifiedMSFileKey());
                  }else{
                      vector<vector<string>> raw_ims_data_vec = package->GetIdentifiedMSFile();
                      res = massdb::data2json::json_ims_key_list(package->GetIdentifiedMSFileKeyByMod(raw_ims_data_vec,mod_vec));
                  }
                  response.write(res);
                  break;
              }
            }
          }
          return response;
          // 构建 Crow 响应对象
        });

    ///检索蛋白名，返回蛋白名满足条件的Prsm
    ///包名/文件类型/蛋白名
    CROW_ROUTE(app, "/database/proteinName/<string>/<string>/<string>")
            .methods("POST"_method)([&](const crow::request& req, string package_name, string file_type, string proteinName) {
                if (package == nullptr || (*db.get_alis_to_uuid())[package_name] != package->get_uuid()) {
                    package = db.OpenPackage(package_name, false);
                }
                crow::response response;
                // 未找到这个文件
                if (massdb::Package::file_type_map.count(file_type) == 0) {
                    response.code = 404;
                    response.write("Not Found");
                    return response;
                } else {
                    string res;
                    switch (massdb::Package::file_type_map.at(file_type)) {
                        case 2: {
                            vector<vector<string>> raw_ims_data_vec = package->GetIdentifiedMSFile();
                            res = massdb::data2json::json_ims_key_list(package->GetIdentifiedMSFileKeyByProteinName(raw_ims_data_vec,proteinName));
                            response.write(res);
                            break;
                        }
                    }
                }
                return response;
                // 构建 Crow 响应对象
            });

    ///检索序列肽段，返回变体序列满足条件的Prsm
    ///包名/文件类型/肽段序列
    CROW_ROUTE(app, "/database/proteinSeq/<string>/<string>/<string>")
            .methods("POST"_method)([&](const crow::request& req, string package_name, string file_type, string proteinSeq) {
                if (package == nullptr || (*db.get_alis_to_uuid())[package_name] != package->get_uuid()) {
                    package = db.OpenPackage(package_name, false);
                }
                crow::response response;
                // 未找到这个文件
                if (massdb::Package::file_type_map.count(file_type) == 0) {
                    response.code = 404;
                    response.write("Not Found");
                    return response;
                } else {
                    string res;
                    switch (massdb::Package::file_type_map.at(file_type)) {
                        case 2: {
                            vector<vector<string>> raw_ims_data_vec = package->GetIdentifiedMSFile();
                            res = massdb::data2json::json_ims_key_list(package->GetIdentifiedMSFileKeyByProteinSeq(raw_ims_data_vec,proteinSeq));
                            response.write(res);
                            break;
                        }
                    }
                }
                return response;
                // 构建 Crow 响应对象
            });



    CROW_ROUTE(app, "/database/ttt/<string>/<string>/<string>")
            .methods("GET"_method)([&](const crow::request& req, string package_name, string pro_mass, string mod_mass) {



                //1.搜索所有的包
                auto packageNameIdMap = db.get_alis_to_uuid();
                vector<string> packageVec;
                for(auto pNameId:*packageNameIdMap){

                    int idx=pNameId.first.find(package_name);
                    if(idx != string::npos )//不存在
                        packageVec.push_back(pNameId.first);
                }

                for(int i=0;i<packageVec.size();i++){
                    //2.构建基峰图
                    map<double,double> basePeakSp;
                    package = db.OpenPackage(packageVec[i], false);
                    vector<string> deconvFileIdVec = package->GetDeconMS1Key();


                    for(string deconvFileId:deconvFileIdVec){
                        auto&& raw_dms_data = package->GetDeconMS1FileSingleData(stoi(deconvFileId));
                        auto&& raw_peaks = raw_dms_data->getPeakPtrVec();
                        double rtime= raw_dms_data->getMsHeaderPtr()->getRetentionTime();

                        double cur_Position = raw_peaks[0]->getPosition();
                        double max_Position = raw_peaks[0]->getPosition();
                        double cur_Intensity = raw_peaks[0]->getIntensity();
                        double max_Intesity = raw_peaks[0]->getIntensity();

                        for (auto& raw_peak : raw_peaks) {
                            cur_Position = raw_peak->getPosition();
                            cur_Intensity = raw_peak->getIntensity();
                            if(cur_Intensity > max_Intesity){
                                max_Intesity = cur_Intensity;
                                max_Position = cur_Position;
                            }
                        }
                        basePeakSp.insert(make_pair(max_Position,max_Intesity));
                        cout<<max_Position<<"  "<<max_Intesity<<endl;
                    }


                    for(auto x:basePeakSp){
                        if(massdb::Util::PPMCheck(x.first,stod(pro_mass),15)){
                            cout<<x.first<< "  "<< x.second;
                        }else if (massdb::Util::PPMCheck(x.first,stod(pro_mass)+stod(mod_mass),15)){
                            cout<<x.first<< "  "<< x.second;
                        }
                    }

                    auto xxx = basePeakSp[0];
                }





                //3.将mtpVec转换为json数据返回
                crow::response response;
                string res;
//                res = massdb::data2json::json_mtpVec_list(mtpVec);
                response.write(res);
                return response;
                // 构建 Crow 响应对象
            });


    CROW_ROUTE(app, "/database/getPrsmScanQA/<string>/<string>/<string>")
            .methods("GET"_method)([&](const crow::request& req, string package_name, string pro_name, string pro_mass) {


                //1.搜索所有的包
                auto packageNameIdMap = db.get_alis_to_uuid();
                vector<string> packageVec;
                for(auto pNameId:*packageNameIdMap){

                    int idx=pNameId.first.find(package_name);
                    if(idx != string::npos )//不存在
                        packageVec.push_back(pNameId.first);
                }

                for(int i=0;i<packageVec.size();i++){
                    //2.根据PrSM对搜索符合搜索蛋白的PrSM查询Ms1 Scan
                    package = db.OpenPackage(packageVec[i], false);
                    //全部prsm文件id
                    vector<vector<string>> prsmFileVec = package->GetIdentifiedMSFile();
                    //搜索蛋白名得到的prsm文件id
                    vector<string> chosenPrsmIdVec = package->GetIdentifiedMSFileKeyByProteinName(prsmFileVec,pro_name);
                    //搜索蛋白名得到的ms2 scan
                    vector<string> chosenMs2Scan = package->GetMs2ScanByPrsm(prsmFileVec,pro_name);
                    //ms2 scan得到对应的deconv ms1 scan
                    vector<toppic::DeconvMsPtr> chosenMs2Vec;
                    for(auto Ms2Scan:chosenMs2Scan){
                        auto&& ms2Data = package->GetDeconvolutedMSFileSingleData(stoi(Ms2Scan));
                        chosenMs2Vec.push_back(ms2Data);
                    }
                    set<string> chosenDm1IdSet = package->GetDm1ScanByMs2(chosenMs2Vec);
                    //计算每个scan的Ptotal
                    for(string dm1Id:chosenDm1IdSet){
                        auto&& raw_dms_data = package->GetDeconMS1FileSingleData(stoi(dm1Id));
                        auto&& raw_peaks = raw_dms_data->getPeakPtrVec();
                        double ori_Intensity = 0;
                        double mod_Intensity = 0;
                        for (auto& raw_peak : raw_peaks) {
                            if(massdb::Util::PPMCheck(raw_peak->getPosition(),stod(pro_mass),15)
                            && raw_peak->getIntensity() > ori_Intensity){
                                ori_Intensity = raw_peak->getIntensity();
                            }
                            if(massdb::Util::PPMCheck(raw_peak->getPosition(),stod(pro_mass)+79.966331,15)
                               && raw_peak->getIntensity() > mod_Intensity){
                                mod_Intensity = raw_peak->getIntensity();
                            }
                        }
                        if(ori_Intensity==0 || mod_Intensity==0){
                            cout<<"dm1_scan:"<<dm1Id<<" ori_Int:"<<ori_Intensity<<"  mod_Int:"<<mod_Intensity<<endl;
                        }else{
                            cout<<"dm1_scan:"<<dm1Id<<" ori_Int:"<<ori_Intensity<<"  mod_Int:"<<mod_Intensity<<" Ptotal:"<<
                                mod_Intensity/(mod_Intensity + ori_Intensity)<<endl;
                        }
                    }//for - chosenDm1IdSet



                }//for - packageVec





                //3.将mtpVec转换为json数据返回
                crow::response response;
                string res;
//                res = massdb::data2json::json_mtpVec_list(mtpVec);
                response.write(res);
                return response;
                // 构建 Crow 响应对象
            });


    CROW_ROUTE(app, "/database/boxplot/<string>/<string>/<string>")
            .methods("GET"_method)([&](const crow::request& req, string package_name, string pro_name, string pro_mass) {


                //1.搜索所有的包
                auto packageNameIdMap = db.get_alis_to_uuid();
                vector<string> packageVec;
                for(auto pNameId:*packageNameIdMap){

                    int idx=pNameId.first.find(package_name);
                    if(idx != string::npos )//不存在
                        packageVec.push_back(pNameId.first);
                }

                modTotalPtrVec mtpVec;

                for(int i=0;i<packageVec.size();i++){
                    //2.根据PrSM对搜索符合搜索蛋白的PrSM查询Ms1 Scan
                    package = db.OpenPackage(packageVec[i], false);
                    //全部prsm文件id
                    vector<vector<string>> prsmFileVec = package->GetIdentifiedMSFile();
                    //搜索蛋白名得到的prsm文件id
                    vector<string> chosenPrsmIdVec = package->GetIdentifiedMSFileKeyByProteinName(prsmFileVec,pro_name);
                    //搜索蛋白名得到的ms2 scan
                    vector<string> chosenMs2Scan = package->GetMs2ScanByPrsm(prsmFileVec,pro_name);
                    //ms2 scan得到对应的deconv ms1 scan
                    vector<toppic::DeconvMsPtr> chosenMs2Vec;
                    for(auto Ms2Scan:chosenMs2Scan){
                        auto&& ms2Data = package->GetDeconvolutedMSFileSingleData(stoi(Ms2Scan));
                        chosenMs2Vec.push_back(ms2Data);
                    }
                    set<string> chosenDm1IdSet = package->GetDm1ScanByMs2(chosenMs2Vec);
                    //计算每个scan的Ptotal
                    for(string dm1Id:chosenDm1IdSet){
                        auto&& raw_dms_data = package->GetDeconMS1FileSingleData(stoi(dm1Id));
                        auto&& raw_peaks = raw_dms_data->getPeakPtrVec();
                        double ori_Intensity = 0;
                        double mod_Intensity = 0;
                        for (auto& raw_peak : raw_peaks) {
                            if(massdb::Util::PPMCheck(raw_peak->getPosition(),stod(pro_mass),15)
                               && raw_peak->getIntensity() > ori_Intensity){
                                ori_Intensity = raw_peak->getIntensity();
                            }
                            if(massdb::Util::PPMCheck(raw_peak->getPosition(),stod(pro_mass)+79.966331,15)
                               && raw_peak->getIntensity() > mod_Intensity){
                                mod_Intensity = raw_peak->getIntensity();
                            }
                        }
                        if(ori_Intensity!=0 && mod_Intensity!=0){
                            if((mod_Intensity)/(ori_Intensity+mod_Intensity) > 0.05){
                                double rtime = raw_dms_data->getMsHeaderPtr()->getRetentionTime();
                                modTotalPtr mtPTR = std::make_shared<mod_total>(packageVec[i],(mod_Intensity)/(ori_Intensity+mod_Intensity),
                                                                                rtime,dm1Id,std::to_string(ori_Intensity),std::to_string(mod_Intensity));
                                mtpVec.push_back(mtPTR);
                            }
                        }
                    }//for - chosenDm1IdSet

                }//for - packageVec





                //3.将mtpVec转换为json数据返回
                crow::response response;
                string res;
                res = massdb::data2json::json_mtpVec_list(mtpVec);
                response.write(res);
                return response;
                // 构建 Crow 响应对象
            });


    CROW_ROUTE(app, "/database/idMs1Vec/<string>/<string>")
            .methods("GET"_method)([&](const crow::request& req, string package_name, string pro_name) {


                //1.搜索所有的包
                auto packageNameIdMap = db.get_alis_to_uuid();
                vector<string> packageVec;
                dm1PtrVec dm1PVec;

                for(auto pNameId:*packageNameIdMap){

                    int idx=pNameId.first.find(package_name);
                    if(idx != string::npos )//不存在
                        packageVec.push_back(pNameId.first);
                }


                for(int i=0;i<packageVec.size();i++){
                    //2.根据PrSM对搜索符合搜索蛋白的PrSM查询Ms1 Scan
                    package = db.OpenPackage(packageVec[i], false);
                    //全部prsm文件id
                    vector<vector<string>> prsmFileVec = package->GetIdentifiedMSFile();
                    //搜索蛋白名得到的prsm文件id
                    vector<string> chosenProName;
                    vector<string> chosenPrsmIdVec = package->Get_IdMSK_PN_ByPName(prsmFileVec,pro_name,chosenProName);
                    //搜索蛋白名得到的ms2 scan
                    vector<string> chosenMs2Scan = package->GetMs2ScanByPrsm(prsmFileVec,pro_name);
                    //ms2 scan得到对应的deconv ms1 scan
                    vector<toppic::DeconvMsPtr> chosenMs2Vec;
                    for(auto Ms2Scan:chosenMs2Scan){
                        auto&& ms2Data = package->GetDeconvolutedMSFileSingleData(stoi(Ms2Scan));
                        chosenMs2Vec.push_back(ms2Data);
                    }

                    set<string> chosenDm1IdSet = package->GetDm1ScanByMs2(chosenMs2Vec);
                    vector<string> chosenDm1IdVec = package->GetDm1ScanVecByMs2(chosenMs2Vec);
                    vector<string> chosenRTime;

                    for(string dm1Id:chosenDm1IdVec){
                        auto&& raw_dms_data = package->GetDeconMS1FileSingleData(stoi(dm1Id));
                        auto&& raw_peaks = raw_dms_data->getPeakPtrVec();

                        std::stringstream ss;
                        ss << std::fixed << std::setprecision(2) << raw_dms_data->getMsHeaderPtr()->getRetentionTime();
                        std::string rTime = ss.str();

                        chosenRTime.push_back(rTime);
                    }//for - chosenDm1IdSet

                    for(int dIndex = 0;dIndex < chosenPrsmIdVec.size();dIndex++){
                        dm1Ptr dm1P = std::make_shared<deMs1>(packageVec[i],chosenPrsmIdVec[dIndex],chosenProName[i],
                                                            chosenMs2Scan[dIndex],chosenDm1IdVec[dIndex],chosenRTime[dIndex]);
                        dm1PVec.push_back(dm1P);
                    }
                }//for - packageVec





                //3.将mtpVec转换为json数据返回
                crow::response response;
                string res;
                res = massdb::data2json::json_dm1Vec_list(dm1PVec);
                response.write(res);
                return response;
                // 构建 Crow 响应对象
            });

    CROW_ROUTE(app, "/dm1range/<string>/<string>/<string>")
            .methods("POST"_method)([&](const crow::request& req, string package_name, string ms1Scan, string prsm_id) {
                if (package == nullptr || (*db.get_alis_to_uuid())[package_name] != package->get_uuid()) {
                    package = db.OpenPackage(package_name, false);
                }
                crow::response response;

                auto&& raw_dms_data = package->GetDeconMS1FileSingleData(stoi(ms1Scan));
                vector<string> prsm_data = massdb::PrsmReader::string_to_prsm( package->GetIdentifiedMSFileSingleData(stoi(prsm_id)));

                double pro_mass = stod(prsm_data[8]);
                double mass_min = pro_mass - 500;
                if(mass_min < 0)
                    mass_min = 0;
                double mass_max = pro_mass + 500;


                auto oriPeakVec = raw_dms_data->getPeakPtrVec();
                vector<toppic::DeconvPeakPtr> chosenPeakVec;

                for(auto oriPeak:oriPeakVec){
                    double monoMass = oriPeak->getMonoMass();
                    if(monoMass >= mass_min && monoMass <= mass_max){
                        oriPeak->setIntensity(oriPeak->getIntensity()/2000000.0);
                        chosenPeakVec.push_back(oriPeak);
                    }
                }

                raw_dms_data->setPeakPtrVec(chosenPeakVec);
                string res = massdb::data2json::json_dm1_single_data(raw_dms_data);
                response.write(res);


                return response;
                // 构建 Crow 响应对象
            });

    ///在所有包下根据修饰搜索prsm
    CROW_ROUTE(app, "/allPackage/selectMod/<string>/<string>")
            .methods("GET"_method)([&](const crow::request& req, string package_name, string mods_string) {

                vector<string> mod_vec = massdb::Util::split(mods_string,",");
                string tempstr = "NoSelect";
                //1.搜索所有的包
                auto packageNameIdMap = db.get_alis_to_uuid();
                vector<string> packageVec;

                for(auto pNameId:*packageNameIdMap){
                    int idx=pNameId.first.find(package_name);
                    if(idx != string::npos )//不存在
                        packageVec.push_back(pNameId.first);
                }

                PrsmPtrVec prsmPtrVec;






                for(int i=0;i<packageVec.size();i++){
                    //2.根据PrSM对搜索符合搜索蛋白的PrSM查询Ms1 Scan
                    package = db.OpenPackage(packageVec[i], false);
                    //全部prsm文件id
                    vector<vector<string>> prsmFileVec = package->GetIdentifiedMSFile();


                    PrsmPtrVec curPrsmVec;
                    if(mod_vec.size()==1 && strcmp(mod_vec[0].c_str(),tempstr.c_str())==0){
                        curPrsmVec = package->GetPrsmVecAll(prsmFileVec,packageVec[i]);
                    }else{
                        curPrsmVec = package->GetPrsmVecByMod(prsmFileVec,mod_vec,packageVec[i]);
                    }


                    prsmPtrVec.insert(prsmPtrVec.end(),curPrsmVec.begin(),curPrsmVec.end());
                }//for - packageVec


                //3.将mtpVec转换为json数据返回
                crow::response response;
                string res;
                res = massdb::data2json::json_prsmVec_list(prsmPtrVec);
                response.write(res);
                return response;
                // 构建 Crow 响应对象
            });

    ///在所有包下根据序列搜索prsm
    CROW_ROUTE(app, "/allPackage/selectSeq/<string>/<string>")
            .methods("GET"_method)([&](const crow::request& req, string package_name, string proteinSeq) {

                //1.搜索所有的包
                auto packageNameIdMap = db.get_alis_to_uuid();
                vector<string> packageVec;

                for(auto pNameId:*packageNameIdMap){
                    int idx=pNameId.first.find(package_name);
                    if(idx != string::npos )//不存在
                        packageVec.push_back(pNameId.first);
                }

                PrsmPtrVec prsmPtrVec;
                for(int i=0;i<packageVec.size();i++){
                    //2.根据PrSM对搜索符合搜索蛋白的PrSM查询Ms1 Scan
                    package = db.OpenPackage(packageVec[i], false);
                    //全部prsm文件id
                    vector<vector<string>> prsmFileVec = package->GetIdentifiedMSFile();
                    PrsmPtrVec curPrsmVec = package->GetPrsmVecBySeq(prsmFileVec,proteinSeq,packageVec[i]);
                    prsmPtrVec.insert(prsmPtrVec.end(),curPrsmVec.begin(),curPrsmVec.end());
                }//for - packageVec

                //3.将mtpVec转换为json数据返回
                crow::response response;
                string res;
                res = massdb::data2json::json_prsmVec_list(prsmPtrVec);
                response.write(res);
                return response;
                // 构建 Crow 响应对象
            });

    ///在所有包下根据蛋白名搜索prsm
    CROW_ROUTE(app, "/allPackage/selectName/<string>/<string>")
            .methods("GET"_method)([&](const crow::request& req, string package_name, string proteinName) {

                //1.搜索所有的包
                auto packageNameIdMap = db.get_alis_to_uuid();
                vector<string> packageVec;

                for(auto pNameId:*packageNameIdMap){
                    int idx=pNameId.first.find(package_name);
                    if(idx != string::npos )//不存在
                        packageVec.push_back(pNameId.first);
                }

                PrsmPtrVec prsmPtrVec;
                for(int i=0;i<packageVec.size();i++){
                    //2.根据PrSM对搜索符合搜索蛋白的PrSM查询Ms1 Scan
                    package = db.OpenPackage(packageVec[i], false);
                    //全部prsm文件id
                    vector<vector<string>> prsmFileVec = package->GetIdentifiedMSFile();
                    PrsmPtrVec curPrsmVec = package->GetPrsmVecByName(prsmFileVec,proteinName,packageVec[i]);
                    prsmPtrVec.insert(prsmPtrVec.end(),curPrsmVec.begin(),curPrsmVec.end());
                }//for - packageVec

                //3.将mtpVec转换为json数据返回
                crow::response response;
                string res;
                res = massdb::data2json::json_prsmVec_list(prsmPtrVec);
                response.write(res);
                return response;
                // 构建 Crow 响应对象
            });


    ///包关键字/蛋白质质量/修饰质量
//    CROW_ROUTE(app, "/database/boxplot/<string>/<string>/<string>")
//            .methods("GET"_method)([&](const crow::request& req, string package_name, string pro_mass, string mod_mass) {
//
//
//
//                //1.搜索所有的包
//                auto packageNameIdMap = db.get_alis_to_uuid();
//                vector<string> packageVec;
//                for(auto pNameId:*packageNameIdMap){
//
//                    int idx=pNameId.first.find(package_name);
//                    if(idx != string::npos )//不存在
//                        packageVec.push_back(pNameId.first);
//                }
//                int packageNum = packageVec.size();
//
//
//                //2.对每个包搜索到最大丰度值，计算Mtotal
//                vector<double> MtotalVec;
//                double pro_mass_d = stod(pro_mass);
//                double mod_mass_d = stod(mod_mass);
//                double modProMass = pro_mass_d + mod_mass_d;
//                modTotalPtrVec mtpVec;
//
//                for(int i=0;i<packageVec.size();i++){
//                    double curMaxIntensity = 0.0;
//                    double curMaxNMI = 0.0;
//                    double curMaxMI = 0.0;
//                    package = db.OpenPackage(packageVec[i], false);
//                    vector<string> deconvFileIdVec = package->GetDeconMS1Key();
////                    vector<string> deconvFileIdVec = package->GetMSFileKey();
//                    for(string deconvFileId:deconvFileIdVec){
//                        double noModIntensity = 0.0;
//                        double oneModIntensity = 0.0;
//                        double douModIntensity = 0.0;
//                        double triModIntensity = 0.0;
//
//                        auto&& raw_dms_data = package->GetDeconMS1FileSingleData(stoi(deconvFileId));
////                        auto&& raw_dms_data = package->GetMSFileSingleData(stoi(deconvFileId));
//                        auto&& raw_peaks = raw_dms_data->getPeakPtrVec();
//                        double rtime= raw_dms_data->getMsHeaderPtr()->getRetentionTime();
//
//                        for (auto& raw_peak : raw_peaks) {
//                            double peakMass = raw_peak->getPosition();
//                            double peakIntensity = raw_peak->getIntensity();
//
//                            bool checkPPM = massdb::Util::PPMCheck(peakMass,pro_mass_d,15);
//                            if(checkPPM && peakIntensity>noModIntensity){
//                                noModIntensity = peakIntensity;
//                            }
//                            checkPPM = massdb::Util::PPMCheck(peakMass,modProMass,15);
//                            if(checkPPM && peakIntensity>oneModIntensity){
//                                oneModIntensity = peakIntensity;
//                            }
////                            checkPPM = massdb::Util::PPMCheck(peakMass,modProMass + mod_mass_d,15);
////                            if(checkPPM && peakIntensity>douModIntensity){
////                                douModIntensity = peakIntensity;
////                            }
////                            checkPPM = massdb::Util::PPMCheck(peakMass,modProMass + mod_mass_d + mod_mass_d,15);
////                            if(checkPPM && peakIntensity>triModIntensity){
////                                triModIntensity = peakIntensity;
////                            }
//                        }
//
//                        if(fabs(noModIntensity*oneModIntensity - 0.0)>0.1){
////                            cout<<"pack:"<<packageVec[i]<<" file:"<<deconvFileId<<" total:"<<(oneModIntensity)/(noModIntensity+oneModIntensity)
////                            <<" NoModI:"<<noModIntensity<<" oneModI:"<<oneModIntensity<<endl;
//                            modTotalPtr mtPTR = std::make_shared<mod_total>(packageVec[i],(oneModIntensity)/(noModIntensity+oneModIntensity),
//                                              rtime,deconvFileId,std::to_string(noModIntensity),std::to_string(oneModIntensity));
//                            mtpVec.push_back(mtPTR);
//                        }
//
//
//                    }
//                }
//
//
//                //3.将mtpVec转换为json数据返回
//                crow::response response;
//                string res;
//                res = massdb::data2json::json_mtpVec_list(mtpVec);
//                response.write(res);
//                return response;
//                // 构建 Crow 响应对象
//            });


    //  CROW_ROUTE(app, "/post")
  //      .methods("POST"_method)([&](const crow::request& req) {
  //        // 构建 Crow 响应对象
  //        // crow::response response(buffer.GetString());
  //        crow::response response(json_str);
  //        // 设置响应头部为 JSON 格式
  //        // response.add_header("Content-Type", "application/json");
  //        return response;
  //      });
  //
  //  // axios 默认不设置发送头的话，默认发送 json 格式的参数数据
  //  // crow post 默认解析 form 格式的参数数据，对于 json 格式的参数会提示 204 错误
  //  CROW_ROUTE(app, "/database")
  //      .methods("POST"_method)([](crow::request req) {
  //        auto body{req.get_body_params()};
  //        CROW_LOG_INFO << body;
  //        return crow::response(200);
  //      });

  app.bindaddr("0.0.0.0").port(8888).multithreaded().run();

}  // namespace massdb::network_server
