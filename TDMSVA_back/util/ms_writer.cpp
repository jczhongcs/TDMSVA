//
// Created by Xsakura on 2023/5/1.
//

#include "ms_writer.h"


#include "ms/spec/ms_header.hpp"
#include "prsm_reader.h"
#include "util.h"
#include "xml2json/rapidjson/document.h"
#include "xml2json/rapidjson/prettywriter.h"
#include "xml2json/rapidjson/stringbuffer.h"

namespace massdb::data2json {

/*void write(const std::string& file_name, toppic::RawMsPtr ms_ptr) {
  rapidjson::Document doc;

  // define the document as an object rather than an array
  doc.SetObject();

  // must pass an allocator when the object may need to allocate memory
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  toppic::MsHeaderPtr header_ptr = ms_ptr->getMsHeaderPtr();
  int id = header_ptr->getId();
  int fraction_id = header_ptr->getFractionId();
  int scan_num = header_ptr->getFirstScanNum();
  double retention_time = header_ptr->getRetentionTime();
  int ms_level = header_ptr->getMsLevel();
  int ms_one_id = header_ptr->getMsOneId();
  int ms_one_scan = header_ptr->getMsOneScan();
  double precursor_mz = header_ptr->getPrecMonoMz();
  int precursor_charge = header_ptr->getPrecCharge();
  double precursor_mass = header_ptr->getPrecMonoMass();
  double precursor_intensity = header_ptr->getPrecInte();
  doc.AddMember("id", id, allocator);
  doc.AddMember("fraction_id", fraction_id, allocator);
  doc.AddMember("scan_num", scan_num, allocator);
  doc.AddMember("retention_time", retention_time, allocator);
  doc.AddMember("ms_level", ms_level, allocator);
  rapidjson::Value activation;
  if (header_ptr->getActivationPtr()) {
    // 对于字符串转换为 Value 类型
    activation = rapidjson::Value(header_ptr->getActivationPtr()->getName().c_str(), allocator);
  } else {
    activation = rapidjson::Value("NONE", allocator);
  }
  doc.AddMember("activation", activation, allocator);
  doc.AddMember("ms_one_id", ms_one_id, allocator);
  doc.AddMember("ms_one_scan", ms_one_scan, allocator);
  doc.AddMember("precursor_mz", precursor_mz, allocator);
  doc.AddMember("precursor_charge", precursor_charge, allocator);
  doc.AddMember("precursor_mass", precursor_mass, allocator);
  doc.AddMember("precursor_intensity", precursor_intensity, allocator);

  // create a rapidjson array type with similar syntax to std::vector
  rapidjson::Value peaks(rapidjson::kArrayType);

  toppic::PeakPtrVec raw_peaks = ms_ptr->getPeakPtrVec();

  for (auto & raw_peak : raw_peaks) {
    rapidjson::Value peak(rapidjson::kArrayType);
    peak.PushBack(raw_peak->getPosition(), allocator);
    peak.PushBack(raw_peak->getIntensity(), allocator);
    peaks.PushBack(peak, allocator);
  }

  doc.AddMember("peaks", peaks, allocator);

  // Convert JSON document to string
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  std::ofstream output;
  output.open(file_name.c_str());
  if (ms_level == 1) {
    output << "ms1_data =" << std::endl;
  } else {
    output << "ms2_data =" << std::endl;
  }
  output << buffer.GetString();
  output.close();
}*/

// 将 ms 转换成 rapid json 对象
std::string json_ms(toppic::RawMsPtr ms_ptr) {
  rapidjson::Document doc;

  // define the document as an object rather than an array
  doc.SetObject();

  // must pass an allocator when the object may need to allocate memory
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  toppic::MsHeaderPtr header_ptr = ms_ptr->getMsHeaderPtr();
  int id = header_ptr->getId();
  int fraction_id = header_ptr->getFractionId();
  int scan_num = header_ptr->getFirstScanNum();
  double retention_time = header_ptr->getRetentionTime();
  int ms_level = header_ptr->getMsLevel();
  int ms_one_id = header_ptr->getMsOneId();
  int ms_one_scan = header_ptr->getMsOneScan();
  double precursor_mz = header_ptr->getPrecMonoMz();
  int precursor_charge = header_ptr->getPrecCharge();
  double precursor_mass = header_ptr->getPrecMonoMass();
  double precursor_intensity = header_ptr->getPrecInte();
  doc.AddMember("id", id, allocator);
  doc.AddMember("fraction_id", fraction_id, allocator);
  doc.AddMember("scan_num", scan_num, allocator);
  doc.AddMember("retention_time", retention_time, allocator);
  doc.AddMember("ms_level", ms_level, allocator);
  rapidjson::Value activation;
  if (header_ptr->getActivationPtr()) {
    // 对于字符串转换为 Value 类型
    activation = rapidjson::Value(header_ptr->getActivationPtr()->getName().c_str(), allocator);
  } else {
    activation = rapidjson::Value("NONE", allocator);
  }
  doc.AddMember("activation", activation, allocator);
  doc.AddMember("ms_one_id", ms_one_id, allocator);
  doc.AddMember("ms_one_scan", ms_one_scan, allocator);
  doc.AddMember("precursor_mz", precursor_mz, allocator);
  doc.AddMember("precursor_charge", precursor_charge, allocator);
  doc.AddMember("precursor_mass", precursor_mass, allocator);
  doc.AddMember("precursor_intensity", precursor_intensity, allocator);

  // create a rapidjson array type with similar syntax to std::vector
  rapidjson::Value peaks(rapidjson::kArrayType);

  toppic::PeakPtrVec raw_peaks = ms_ptr->getPeakPtrVec();

  for (auto& raw_peak : raw_peaks) {
    rapidjson::Value peak(rapidjson::kArrayType);
    peak.PushBack(raw_peak->getPosition(), allocator);
    peak.PushBack(raw_peak->getIntensity(), allocator);
    peaks.PushBack(peak, allocator);
  }

  doc.AddMember("peaks", peaks, allocator);

  // Convert JSON document to string
  rapidjson::StringBuffer buffer;

  // PrettyWriter 用于人类可读形式的写出
  // rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}

std::string json_package_list(std::map<std::string, std::string>* package_list) {
  // 遍历 package_list，写入 rapidjson
  rapidjson::Document doc;
  doc.SetObject();
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  rapidjson::Value arr(rapidjson::kArrayType);
  for (auto& package : *package_list) {
    rapidjson::Value item(rapidjson::kArrayType);
    rapidjson::Value package_name(package.first.c_str(), allocator);
    rapidjson::Value package_uuid(package.second.c_str(), allocator);
    item.PushBack(package_name, allocator);
    item.PushBack(package_uuid, allocator);
    arr.PushBack(item, allocator);
  }
  doc.AddMember("package_list", arr, allocator);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}

std::string json_ms_file_list(const std::vector<bool>& exist_file) {
  rapidjson::Document doc;
  doc.SetObject();
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  rapidjson::Value arr(rapidjson::kArrayType);
  for (bool exist : exist_file) {
    arr.PushBack(exist, allocator);
  }
  doc.AddMember("ms_file", arr, allocator);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}

std::string json_rms_key_list(const std::vector<std::string>& rms_key_list) {
  rapidjson::Document doc;
  doc.SetObject();
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  rapidjson::Value arr(rapidjson::kArrayType);
  for (auto& rms_key : rms_key_list) {
    rapidjson::Value object(rapidjson::kObjectType);
    rapidjson::Value item(rms_key.c_str(), allocator);
    object.AddMember("file_type", "rms", allocator);
    object.AddMember("id", item, allocator);
    arr.PushBack(object, allocator);
  }
  doc.AddMember("id_list", arr, allocator);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}
std::string json_dms_key_list(const std::vector<std::string>& dms_key_list) {
  rapidjson::Document doc;
  doc.SetObject();
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  rapidjson::Value arr(rapidjson::kArrayType);
  for (auto& dms_key : dms_key_list) {
    rapidjson::Value object(rapidjson::kObjectType);
    rapidjson::Value item(dms_key.c_str(), allocator);
    object.AddMember("file_type", "dms", allocator);
    object.AddMember("id", item, allocator);
    arr.PushBack(object, allocator);
  }
  doc.AddMember("id_list", arr, allocator);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}
std::string json_dm1_key_list(const std::vector<std::string>& dms_key_list) {
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    rapidjson::Value arr(rapidjson::kArrayType);
    for (auto& dms_key : dms_key_list) {
        rapidjson::Value object(rapidjson::kObjectType);
        rapidjson::Value item(dms_key.c_str(), allocator);
        object.AddMember("file_type", "dm1", allocator);
        object.AddMember("id", item, allocator);
        arr.PushBack(object, allocator);
    }
    doc.AddMember("id_list", arr, allocator);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}


std::string json_ims_key_list(const std::vector<std::string>& ims_key_list) {
  rapidjson::Document doc;
  doc.SetObject();
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  rapidjson::Value arr(rapidjson::kArrayType);
  for (auto& ims_key : ims_key_list) {
    rapidjson::Value object(rapidjson::kObjectType);
    rapidjson::Value item(ims_key.c_str(), allocator);
    object.AddMember("file_type", "ims", allocator);
    object.AddMember("id", item, allocator);
    arr.PushBack(object, allocator);
  }
  doc.AddMember("id_list", arr, allocator);
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}

std::string json_rms_single_data(const toppic::RawMsPtr& rms_single_data) {
  rapidjson::Document doc;

  // define the document as an object rather than an array
  doc.SetObject();

  // must pass an allocator when the object may need to allocate memory
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  toppic::MsHeaderPtr header_ptr = rms_single_data->getMsHeaderPtr();
  int id = header_ptr->getId();
  int fraction_id = header_ptr->getFractionId();
  int scan_num = header_ptr->getFirstScanNum();
  double retention_time = header_ptr->getRetentionTime();
  int ms_level = header_ptr->getMsLevel();
  int ms_one_id = header_ptr->getMsOneId();
  int ms_one_scan = header_ptr->getMsOneScan();
  double precursor_mz = header_ptr->getPrecMonoMz();
  int precursor_charge = header_ptr->getPrecCharge();
  double precursor_mass = header_ptr->getPrecMonoMass();
  double precursor_intensity = header_ptr->getPrecInte();

  rapidjson::Value object(rapidjson::kObjectType);
  object.AddMember("id", id, allocator);
  object.AddMember("fraction_id", fraction_id, allocator);
  object.AddMember("scan_num", scan_num, allocator);
  object.AddMember("retention_time", retention_time, allocator);
  object.AddMember("ms_level", ms_level, allocator);
  rapidjson::Value activation;
  if (header_ptr->getActivationPtr()) {
    // 对于字符串转换为 Value 类型
    activation = rapidjson::Value(header_ptr->getActivationPtr()->getName().c_str(), allocator);
  } else {
    activation = rapidjson::Value("NONE", allocator);
  }
  object.AddMember("activation", activation, allocator);
  object.AddMember("ms_one_id", ms_one_id, allocator);
  object.AddMember("ms_one_scan", ms_one_scan, allocator);
  object.AddMember("precursor_mz", precursor_mz, allocator);
  object.AddMember("precursor_charge", precursor_charge, allocator);
  object.AddMember("precursor_mass", precursor_mass, allocator);
  object.AddMember("precursor_intensity", precursor_intensity, allocator);

  doc.AddMember("header", object, allocator);

  // create a rapidjson array type with similar syntax to std::vector
  rapidjson::Value peaks(rapidjson::kArrayType);
  toppic::PeakPtrVec raw_peaks = rms_single_data->getPeakPtrVec();

  for (auto& raw_peak : raw_peaks) {
    rapidjson::Value peak(rapidjson::kArrayType);
    peak.PushBack(raw_peak->getPosition(), allocator);
    peak.PushBack(raw_peak->getIntensity(), allocator);
    peaks.PushBack(peak, allocator);
  }

  doc.AddMember("peaks", peaks, allocator);

  // Convert JSON document to string
  rapidjson::StringBuffer buffer;

  // PrettyWriter 用于人类可读形式的写出
  // rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}
std::string json_dms_single_data(const toppic::DeconvMsPtr& dms_single_data) {
  rapidjson::Document doc;

  // define the document as an object rather than an array
  doc.SetObject();

  // must pass an allocator when the object may need to allocate memory
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

  toppic::MsHeaderPtr header_ptr = dms_single_data->getMsHeaderPtr();
  int id = header_ptr->getId();
  int fraction_id = header_ptr->getFractionId();
  int scan_num = header_ptr->getFirstScanNum();
  double retention_time = header_ptr->getRetentionTime();
  int ms_level = header_ptr->getMsLevel();
  int ms_one_id = header_ptr->getMsOneId();
  int ms_one_scan = header_ptr->getMsOneScan();
  double precursor_mz = header_ptr->getPrecMonoMz();
  int precursor_charge = header_ptr->getPrecCharge();
  double precursor_mass = header_ptr->getPrecMonoMass();
  double precursor_intensity = header_ptr->getPrecInte();

  rapidjson::Value object(rapidjson::kObjectType);
  object.AddMember("id", id, allocator);
  object.AddMember("fraction_id", fraction_id, allocator);
  object.AddMember("scan_num", scan_num, allocator);
  object.AddMember("retention_time", retention_time, allocator);
  object.AddMember("ms_level", ms_level, allocator);
  rapidjson::Value activation;
  if (header_ptr->getActivationPtr()) {
    // 对于字符串转换为 Value 类型
    activation = rapidjson::Value(header_ptr->getActivationPtr()->getName().c_str(), allocator);
  } else {
    activation = rapidjson::Value("NONE", allocator);
  }
  object.AddMember("activation", activation, allocator);
  object.AddMember("ms_one_id", ms_one_id, allocator);
  object.AddMember("ms_one_scan", ms_one_scan, allocator);
  object.AddMember("precursor_mz", precursor_mz, allocator);
  object.AddMember("precursor_charge", precursor_charge, allocator);
  object.AddMember("precursor_mass", precursor_mass, allocator);
  object.AddMember("precursor_intensity", precursor_intensity, allocator);

  doc.AddMember("header", object, allocator);

  // create a rapidjson array type with similar syntax to std::vector
  rapidjson::Value peaks(rapidjson::kArrayType);
  auto&& raw_peaks = dms_single_data->getPeakPtrVec();

  for (auto& raw_peak : raw_peaks) {
    rapidjson::Value peak(rapidjson::kArrayType);
    peak.PushBack(raw_peak->getPosition(), allocator);
    peak.PushBack(raw_peak->getIntensity(), allocator);
    peak.PushBack(raw_peak->getCharge(), allocator);
    peaks.PushBack(peak, allocator);
  }

  doc.AddMember("peaks", peaks, allocator);

  // Convert JSON document to string
  rapidjson::StringBuffer buffer;

  // PrettyWriter 用于人类可读形式的写出
  // rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}
std::string json_ims_single_data(const std::string& ims_single_data) {
  rapidjson::Document doc;
  doc.SetObject();
  rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
  std::vector<std::string> ims_data = PrsmReader::string_to_prsm(ims_single_data);
  //判断N端形式
  ims_data[21] = Util::processNterFormSeq(ims_data[21],ims_data[23]);


  for (int i = 0; i < ims_data.size(); i++) {
    rapidjson::Value attr(PrsmReader::prsm_head[i].c_str(), allocator);
    //    Util::trim(ims_data[i]);
    rapidjson::Value val(ims_data[i].c_str(), allocator);
    doc.AddMember(attr, val, allocator);
  }
  rapidjson::StringBuffer buffer;
  rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
  doc.Accept(writer);
  return buffer.GetString();
}
std::string json_dm1_single_data(const toppic::DeconvMsPtr& dms_single_data) {
    rapidjson::Document doc;

    // define the document as an object rather than an array
    doc.SetObject();

    // must pass an allocator when the object may need to allocate memory
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    toppic::MsHeaderPtr header_ptr = dms_single_data->getMsHeaderPtr();
    int id = header_ptr->getId();
    int fraction_id = header_ptr->getFractionId();
    int scan_num = header_ptr->getFirstScanNum();
    double retention_time = header_ptr->getRetentionTime();
    int ms_level = header_ptr->getMsLevel();
    int ms_one_id = header_ptr->getMsOneId();
    int ms_one_scan = header_ptr->getMsOneScan();
    double precursor_mz = header_ptr->getPrecMonoMz();
    int precursor_charge = header_ptr->getPrecCharge();
    double precursor_mass = header_ptr->getPrecMonoMass();
    double precursor_intensity = header_ptr->getPrecInte();

    rapidjson::Value object(rapidjson::kObjectType);
    object.AddMember("id", id, allocator);
    object.AddMember("fraction_id", fraction_id, allocator);
    object.AddMember("scan_num", scan_num, allocator);
    object.AddMember("retention_time", retention_time, allocator);
    object.AddMember("ms_level", ms_level, allocator);
    rapidjson::Value activation;
    if (header_ptr->getActivationPtr()) {
        // 对于字符串转换为 Value 类型
        activation = rapidjson::Value(header_ptr->getActivationPtr()->getName().c_str(), allocator);
    } else {
        activation = rapidjson::Value("NONE", allocator);
    }
    object.AddMember("activation", activation, allocator);
    object.AddMember("ms_one_id", ms_one_id, allocator);
    object.AddMember("ms_one_scan", ms_one_scan, allocator);
    object.AddMember("precursor_mz", precursor_mz, allocator);
    object.AddMember("precursor_charge", precursor_charge, allocator);
    object.AddMember("precursor_mass", precursor_mass, allocator);
    object.AddMember("precursor_intensity", precursor_intensity, allocator);

    doc.AddMember("header", object, allocator);

    // create a rapidjson array type with similar syntax to std::vector
    rapidjson::Value peaks(rapidjson::kArrayType);
    auto&& raw_peaks = dms_single_data->getPeakPtrVec();

    for (auto& raw_peak : raw_peaks) {
        rapidjson::Value peak(rapidjson::kArrayType);
        peak.PushBack(raw_peak->getPosition(), allocator);
        peak.PushBack(raw_peak->getIntensity(), allocator);
        peak.PushBack(raw_peak->getCharge(), allocator);
        peaks.PushBack(peak, allocator);
    }

    doc.AddMember("peaks", peaks, allocator);

    // Convert JSON document to string
    rapidjson::StringBuffer buffer;

    // PrettyWriter 用于人类可读形式的写出
    // rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}

std::string json_mtpVec_list(const modTotalPtrVec &mtpVec){
    rapidjson::Document doc;
    // define the document as an object rather than an array
    doc.SetObject();
    // must pass an allocator when the object may need to allocate memory
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();


    rapidjson::Value mtpList(rapidjson::kArrayType);
    for (auto mtp : mtpVec) {
        rapidjson::Value object(rapidjson::kObjectType);

        rapidjson::Value packageName(mtp->getPackageName().c_str(), allocator);
        rapidjson::Value modTotal(std::to_string(mtp->getModTotal()).c_str(), allocator);
        rapidjson::Value retentionTime(std::to_string(mtp->getRetentionTime()).c_str(), allocator);
        rapidjson::Value scans(mtp->getScans().c_str(), allocator);
        rapidjson::Value noModIntensity(mtp->getNoModIntensity().c_str(), allocator);
        rapidjson::Value ModIntensity(mtp->getModIntensity().c_str(), allocator);
        object.AddMember("packageName", packageName, allocator);
        object.AddMember("modTotal", modTotal, allocator);
        object.AddMember("retentionTime", retentionTime, allocator);
        object.AddMember("scans", scans, allocator);
        object.AddMember("noModIntensity", noModIntensity, allocator);
        object.AddMember("ModIntensity", ModIntensity, allocator);
        mtpList.PushBack(object, allocator);
    }
    doc.AddMember("mtp_list", mtpList, allocator);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();

}

std::string json_dm1Vec_list(const dm1PtrVec &dm1PVec){
        rapidjson::Document doc;
        // define the document as an object rather than an array
        doc.SetObject();
        // must pass an allocator when the object may need to allocate memory
        rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();


        rapidjson::Value dm1List(rapidjson::kArrayType);
        for (auto dm1P : dm1PVec) {
            rapidjson::Value object(rapidjson::kObjectType);

            rapidjson::Value packageName(dm1P->getPackageName().c_str(), allocator);
            rapidjson::Value prsmId(dm1P->getPrsmId().c_str(), allocator);
            rapidjson::Value proteinName(dm1P->getProteinName().c_str(), allocator);
            rapidjson::Value Ms2Scan(dm1P->getMs2Scan().c_str(), allocator);
            rapidjson::Value Ms1Scan(dm1P->getMs1Scan().c_str(), allocator);
            rapidjson::Value retentionTime(dm1P->getRetentionTime().c_str(), allocator);


            object.AddMember("packageName", packageName, allocator);
            object.AddMember("prsmId", prsmId, allocator);
            object.AddMember("proteinName", proteinName, allocator);
            object.AddMember("Ms2Scan", Ms2Scan, allocator);
            object.AddMember("Ms1Scan", Ms1Scan, allocator);
            object.AddMember("retentionTime", retentionTime, allocator);
            dm1List.PushBack(object, allocator);
        }
        doc.AddMember("dm1_list", dm1List, allocator);
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        doc.Accept(writer);
        return buffer.GetString();

    }

std::string json_prsmVec_list(const PrsmPtrVec &prsmPtrVec){
    rapidjson::Document doc;
    // define the document as an object rather than an array
    doc.SetObject();
    // must pass an allocator when the object may need to allocate memory
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

    rapidjson::Value prsmList(rapidjson::kArrayType);
    for (auto prsmPtr : prsmPtrVec) {
        rapidjson::Value object(rapidjson::kObjectType);

        rapidjson::Value packageName(prsmPtr->getPackageName().c_str(), allocator);
        rapidjson::Value prsmId(prsmPtr->getPrsmId().c_str(), allocator);
        rapidjson::Value proteinName(prsmPtr->getProteinName().c_str(), allocator);
        rapidjson::Value evalue(prsmPtr->getEvalue().c_str(), allocator);

        object.AddMember("packageName", packageName, allocator);
        object.AddMember("prsmId", prsmId, allocator);
        object.AddMember("proteinName", proteinName, allocator);
        object.AddMember("evalue", evalue, allocator);
        prsmList.PushBack(object, allocator);
    }
    doc.AddMember("prsm_list", prsmList, allocator);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();

}



}  // namespace massdb::data2json
