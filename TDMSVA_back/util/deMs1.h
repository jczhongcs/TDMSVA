//
// Created by LYC on 2024/9/4.
//

#include <string>
#ifndef MASSDB_DM1_H
#define MASSDB_DM1_H


using namespace std;
class deMs1 {





public:
    deMs1(const std::string & packageName, const string & prsmId, const string & proteinName, const string & ms2Scan, const string & ms1Scan, const string & retentionTime):packageName(packageName),prsmId(prsmId),proteinName(proteinName),Ms2Scan(ms2Scan),Ms1Scan(ms1Scan),retentionTime(retentionTime){}
    deMs1(){}
    const std::string & getPackageName() const{
     return packageName;
    }const string & getPrsmId() const{
     return prsmId;
    }const string & getProteinName() const{
     return proteinName;
    }const string & getMs2Scan() const{
     return Ms2Scan;
    }const string & getMs1Scan() const{
     return Ms1Scan;
    }const string & getRetentionTime() const{
     return retentionTime;
    }
    void setPackageName(const std::string & packageName){
        deMs1::packageName = packageName;
    }void setPrsmId(const string & prsmId){
        deMs1::prsmId = prsmId;
    }void setProteinName(const string & proteinName){
        deMs1::proteinName = proteinName;
    }void setMs2Scan(const string & ms2Scan){
    Ms2Scan = ms2Scan;
    }void setMs1Scan(const string & ms1Scan){
    Ms1Scan = ms1Scan;
    }void setRetentionTime(const string & retentionTime){
        deMs1::retentionTime = retentionTime;
    }
private
:
    std::string packageName;
    string prsmId;
    string proteinName;
    string Ms2Scan;    string Ms1Scan;
    string retentionTime;

};

typedef std::shared_ptr<deMs1> dm1Ptr;
typedef std::vector<dm1Ptr> dm1PtrVec;
#endif //MASSDB_DM1_H