/*
 * bGeno.h
 *
 *  Created on: 2013/08/15
 *      Author: Kamatani Yoichiro
 */

#ifndef BGENO_H_
#define BGENO_H_

#include <vector>
#include <string>
#include <bitset>
#include "locus.h"

using namespace std;

class GENO;
class geno_cnt;

class bGeno_SNPmajor{
private:
    int Ngeno;

public:
    locus *loc;
    vector<bitset<8> > one;
    vector<bitset<8> > two;

    bGeno_SNPmajor(int nSamp,locus *l);
    bGeno_SNPmajor(int nSamp);


    void append(bool b1,bool b2);

    int size(){return Ngeno;};

    GENO* get_genotype(int idx);
    vector<GENO*> get_genotypes();
    int dosage(int idx);

    geno_cnt count();
    geno_cnt count(vector<int> &INDindices);
    double Freq1();
    double Freq1(vector<int> &INDindices);
    double succ();
    double succ(vector<int> &INDindices);

    int NumMissing();
    int NumMissing(vector<int> &INDindices);
};

class GENO{
public:
    // string FID,IID;
    string MarkerName;
    string G1,G2;
//    individual* ind;

    GENO():MarkerName(""),G1(""),G2(""){};
    GENO(string name):MarkerName(name),G1(""),G2(""){};
    GENO(string name,string g1, string g2):MarkerName(name),G1(g1),G2(g2){};

    void inputBinary(bitset<2> &b,locus &l);

    bool isEmpty(){return (MarkerName == "" && G1 == "")?true:false;};
    string str(string delim){return G1+delim+G2;}
    bool isMissing();
    bool isConsistent(GENO *geno2);
};

//-----------------------------------------------------------
// supplementary classes
//-----------------------------------------------------------
class geno_cnt{
public:
	vector<int> counts;

    geno_cnt();
    void add(bitset<2> b);
    string str();
    double Freq1();
    double MAF();
};

#endif /* BGENO_H_ */
