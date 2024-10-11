//
// Created by LYC on 2024/8/22.
//

#include "mod_total.h"
mod_total::mod_total(const std::string & packageName, double modTotal,
                     double retentionTime, const std::string & scans,
                     const std::string & noModIntensity,
                     const std::string & modIntensity):packageName(packageName),
                     modTotal(modTotal),retentionTime(retentionTime),scans(scans),
                     noModIntensity(noModIntensity),ModIntensity(modIntensity){}
const std::string & mod_total::getPackageName() const{
 return packageName;
}void mod_total::setPackageName(const std::string & packageName){
mod_total::packageName = packageName;
}double mod_total::getModTotal() const{
 return modTotal;
}void mod_total::setModTotal(double modTotal){
mod_total::modTotal = modTotal;
}double mod_total::getRetentionTime() const{
 return retentionTime;
}void mod_total::setRetentionTime(double retentionTime){
mod_total::retentionTime = retentionTime;
}const std::string & mod_total::getScans() const{
 return scans;
}void mod_total::setScans(const std::string & scans){
mod_total::scans = scans;
}const std::string & mod_total::getNoModIntensity() const{
 return noModIntensity;
}void mod_total::setNoModIntensity(const std::string & noModIntensity){
mod_total::noModIntensity = noModIntensity;
}const std::string & mod_total::getModIntensity() const{
 return ModIntensity;
}void mod_total::setModIntensity(const std::string & modIntensity){
ModIntensity = modIntensity;
}
mod_total::mod_total(){}
