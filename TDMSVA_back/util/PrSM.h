//
// Created by LYC on 2024/9/9.
//

#ifndef MASSDB_PRSM_H
#define MASSDB_PRSM_H

#include <string>
using namespace std;
class PrSM{

    public:
        PrSM(const string & packageName, const string & prsmId, const string & proteinName, const string & evalue):packageName(packageName),prsmId(prsmId),proteinName(proteinName),evalue(evalue){}

        PrSM(){}

        const string & getPackageName() const{
         return packageName;
        }void setPackageName(const string & packageName){
        PrSM::packageName = packageName;
        }const string & getPrsmId() const{
         return prsmId;
        }void setPrsmId(const string & prsmId){
        PrSM::prsmId = prsmId;
        }const string & getProteinName() const{
         return proteinName;
        }void setProteinName(const string & proteinName){
        PrSM::proteinName = proteinName;
        }const string & getEvalue() const{
         return evalue;
        }void setEvalue(const string & evalue){
        PrSM::evalue = evalue;
        }

    private:
        string packageName;
        string prsmId;
        string proteinName;
        string evalue;

};

typedef std::shared_ptr<PrSM> PrsmPtr;
typedef vector<PrsmPtr> PrsmPtrVec;

#endif //MASSDB_PRSM_H
