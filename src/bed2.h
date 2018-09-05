//
//  bed.h
//  ParseBed
//
//  Created by Yoichiro Kamatani on 3/9/13.
//  Copyright (c) 2013 Yoichiro Kamatani. All rights reserved.
//
//	Modified on 8/13/13
//	Genotypes are stored as vector<bool> in individual class
//


#ifndef __ParseBed__bed__
#define __ParseBed__bed__

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <bitset>
#include <map>
#include <string>
#include "locus.h"
#include "bGeno.h"

using namespace std;

class individual;
class geno_cnt;
class genotype_list;
class locus;

//-----------------------------------------------------------
// elemental classes
//-----------------------------------------------------------

class individual{
public:
    string FID,IID,PID,MID,SEX,PHENO;
    signed char csex,b_pheno;
    double q_pheno;

    individual(string tFID,string tIID);
    individual(string tFID,string tIID,string tPID,string tMID, string tSex,string  tPheno);
    individual(string tFID,string tIID,string tPID,string tMID, string tSex,string  tPheno,bool qt);

    string ID();
};


//-----------------------------------------------------------
// binary_ped data component files
//-----------------------------------------------------------
class fam{
private:
    map<string,int> mSamp;

    void MakeMap();
public:
    vector<individual*> sample;

    fam();
    void read(string fam_file);
    int size(){return (int)sample.size();};
    individual* get(int index);
    int get_index(string FID,string IID);
    vector<string> names();
};

class bim{
private:
    map<string,int> mLoci;
    map<string,int> pLoci;

    void MakeMap();
    void MakePMap();
public:
    vector<locus*> loci;

    bim();

    void read(string bim_file);
    int size(){return (int)loci.size();};

    locus* get(int index);
    bool exists(string MarkerName);
    bool exists(int chrom,int pos);
    int get_index(string MarkerName);
    int get_index(int chrom,int pos);

    vector<string> names();
    vector<locus*> markers();
};

class bed{
private:

    // each element of begenos indicates genotypes for all individuals at a SNP

    bool openBinaryFile(string s, ifstream & BIT);
public:
    int SampleSize;
    vector<bGeno_SNPmajor*> bgenos;

    bed();
    void read(string bed_file,bim &markers,int sample_size);
    vector<string> markerNames();
    void count();
    void indLevel_count();
    vector<GENO*> genotypes_per_marker(int SNPidx);
    vector<GENO*> genotypes_per_ind(int INDidx);
};

//-----------------------------------------------------------
// binary_ped class
//-----------------------------------------------------------

class binary_ped{
protected:
    string plink_head;

public:
    bed be;
    bim bi;
    fam fa;
    vector<bitset<1> > SNPincl;

    binary_ped();

    binary_ped(string file_head);
    binary_ped(string file_head, string snp_file);

    void read();
    void read(string file_head);
    void setSNPsInclude();
    void extractSNPs(string snp_file);
//    void test_markerNames();
//    void test_count();

    GENO* genotype(int INDidx,int SNPidx);
    GENO* genotype(string FID,string IID,string MarkerName);
    vector<GENO*> genotypes(string FID,string IID);
    vector<GENO*> genotypes(string MarkerName);
    vector<GENO*> genotypes(int MarkerIdx);

    vector<locus*> markers();

    void OverAllCallRate();

    int SampleSize();
    int MarkerSize();

    void add_dummy(string FID,string IID,string sex);

//    void write_PEDFILE(string FileName);
//    void write_MAPFILE(string FileName);
//    void write_FILEs(string FileName);
//
//    void write_FAMFILE(string FileName);
//    void write_BIMFILE(string FileName);
//    void write_BEDFILE(string FileName);
//    void write_BFILEs(string FileName);
};





#endif /* defined(__ParseBed__bed__) */
