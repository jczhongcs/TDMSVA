//
// Created by LYC on 2024/8/22.
//

#ifndef MASSDB_MOD_TOTAL_H
#define MASSDB_MOD_TOTAL_H


#include <string>
#include <memory>
#include <vector>

class mod_total {



public: mod_total(const std::string & packageName, double modTotal, double retentionTime,
                  const std::string & scans, const std::string & noModIntensity, const std::string & modIntensity);
mod_total();

const std::string & getPackageName() const;
void setPackageName(const std::string & packageName);
double getModTotal() const;void setModTotal(double modTotal);
double getRetentionTime() const;
void setRetentionTime(double retentionTime);
const std::string & getScans() const;
void setScans(const std::string & scans);
const std::string & getNoModIntensity() const;
void setNoModIntensity(const std::string & noModIntensity);
const std::string & getModIntensity() const;
void setModIntensity(const std::string & modIntensity);

private:
    std::string packageName;
    double modTotal;
    double retentionTime;
    std::string scans;
    std::string noModIntensity;
    std::string ModIntensity;

};

typedef std::shared_ptr<mod_total> modTotalPtr;
typedef std::vector<modTotalPtr> modTotalPtrVec;

#endif //MASSDB_MOD_TOTAL_H
