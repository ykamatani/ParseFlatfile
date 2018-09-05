/*
 bed2.cpp
 
 Created by Yoichiro Kamatani on 3/9/13.
 Copyright (c) 2013 Yoichiro Kamatani.
 
 This file is part of ParseFlatfile.
 
 ParseFlatfile is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 ParseFlatfile is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with ParseFlatfile.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include "ybase.h"
#include "bed2.h"

//-----------------------------------------------------------
// individual class
//-----------------------------------------------------------
individual::individual(string tFID,string tIID,string tPID,string tMID, string tSex,string  tPheno):FID(tFID),IID(tIID),PID(tPID),MID(tMID),SEX(tSex),PHENO(tPheno),csex(0),b_pheno(-9),q_pheno(0){

    /* parse sex */
    if (tSex == "0" || tSex == "1" || tSex == "2"){
        csex = atoi(tSex.c_str());
    } else {
        fprintf(stderr, "Invalid sex code [ %s ] found at individual %s:%s.\n",tSex.c_str(),FID.c_str(),IID.c_str());
        exit(1);
    }

    /* parse phenotype */
    if (tPheno == "-9" || tPheno == "1" || tPheno == "2"){
        b_pheno = atoi(tPheno.c_str());
    } else {
        fprintf(stderr, "Invalid binary phenotype code [ %s ] found at individual %s:%s.\n",tPheno.c_str(),FID.c_str(),IID.c_str());
        exit(1);
    }

}

individual::individual(string tFID,string tIID):FID(tFID),IID(tIID),PID("0"),MID("0"),SEX("0"),PHENO("-9"),csex(0),b_pheno(-9),q_pheno(0){

}

string individual::ID(){
    return FID+"_"+IID;
}

//-----------------------------------------------------------
// fam class
//-----------------------------------------------------------
fam::fam(){
    sample.clear();
    mSamp.clear();
}

void fam::MakeMap(){
    if (mSamp.size() == sample.size()) return;

    mSamp.clear();
    for (int i=0; i < (int)sample.size(); i++)
        mSamp.insert(make_pair(sample[i]->ID(), i));
}

void fam::read(string fam_file){
    cout << "Reading pedigree information from [ " << fam_file << " ]" << endl;
    int c = 0;
    ifstream FAM(fam_file.c_str(),ios::in);

    FAM.clear();

    while (!FAM.eof()){
        string tFID,tIID,tPID,tMID,tSex,tPheno;
        FAM >> tFID >> tIID >> tPID >> tMID >> tSex >> tPheno;

        if ( FAM.eof() ){
            continue;
        } else if ( FAM.fail() ){
            fprintf(stderr,"Problem reading FAM file, line %d\n",c+1);
            exit(1);
        }

        if (mSamp.find(tFID+"_"+tIID) != mSamp.end()){
            fprintf(stderr, "Duplicated name found: [ %s %s]\n",tFID.c_str(),tIID.c_str());
            exit(1);
        }
        individual *tind = new individual(tFID, tIID, tPID, tMID, tSex, tPheno);
        mSamp.insert(make_pair(tind->ID(), c));
        sample.push_back(tind);
        c++;
    }
    FAM.clear();
    FAM.close();
    cout << c << " individuals read from [ " << fam_file << " ]" << endl;
}

individual* fam::get(int index){
    if (index < 0 || index >= (int)sample.size()) return NULL;
    return sample[index];
}

int fam::get_index(string FID,string IID){
    MakeMap();

    string ID = FID+"_"+IID;
    if (mSamp.find(ID) == mSamp.end())
        return -9;
    return mSamp[ID];
}

vector<string> fam::names(){
    vector<string> RetVec;
    vector<individual*>::iterator iSamp;
    for(iSamp = sample.begin();iSamp != sample.end();iSamp++)
        RetVec.push_back((*iSamp)->ID());
    return RetVec;
}

//vector<individual*> fam::inds(){
//    vector<individual*> RetVec;
//    vector<individual>::iterator iSamp;
//    for(iSamp = sample.begin();iSamp != sample.end();iSamp++)
//        RetVec.push_back(&*iSamp);
//    return RetVec;
//}
//-----------------------------------------------------------
// bim class
//-----------------------------------------------------------
bim::bim(){
    loci.clear();
    mLoci.clear();
    pLoci.clear();
}

void bim::MakeMap(){
    if (!mLoci.empty()) return;

    for (int i=0; i < (int)loci.size(); i++)
        mLoci.insert(make_pair(loci[i]->name, i));
}

void bim::MakePMap()
{
    if (pLoci.empty())
        for (int i = 0; i < (int)loci.size(); i++)
            pLoci.insert(make_pair(loci[i]->posName(), i));
}

void bim::read(string bim_file){
    cout << "Reading map (extended format) from [ " << bim_file << " ]" << endl;
    int c = 0;
    ifstream MAP(bim_file.c_str(),ios::in);

    MAP.clear();

    while (!MAP.eof()) {
        int chrom,pos,dummy;
        string Mname,a1,a2;

        MAP >> chrom >> Mname >> dummy >> pos >> a1 >> a2;

        if ( MAP.eof() ){
            continue;
        } else if ( MAP.fail() ){
            fprintf(stderr,"Problem reading BIM file, line %d\n",c+1);
        }

        locus *tmp = new locus(chrom,Mname,pos,a1,a2);

        loci.push_back(tmp);
        c++;
    }
    cout << loci.size() << " markers to be included from [ " << bim_file << " ]" << endl;

    MAP.clear();
    MAP.close();
}

locus* bim::get(int index){
    if (index < 0 || index >= (int)loci.size()) return NULL;
    return loci[index];
}

bool bim::exists(string MarkerName)
{
    MakeMap();
    if (mLoci.find(MarkerName) != mLoci.end()) return true;
    return false;
}

bool bim::exists(int chrom,int pos)
{
    MakePMap();
    char tmp[32];
    snprintf(tmp, 32, "%d:%d",chrom,pos);
    if (pLoci.find(string(tmp)) != pLoci.end()) return true;
    return false;
}

int bim::get_index(string MarkerName)
{
    if(!exists(MarkerName)) return -9;
    return mLoci[MarkerName];
}

int bim::get_index(int chrom,int pos)
{
    if(!exists(chrom,pos)) return -9;
    char tmp[32];
    snprintf(tmp,32,"%d:%d",chrom,pos);
    return pLoci[string(tmp)];
}

vector<string> bim::names(){
    vector<string> RetVec;
    vector<locus*>::iterator iloci;
    for(iloci = loci.begin();iloci != loci.end();iloci++)
        RetVec.push_back((*iloci)->name);
    return RetVec;
}

vector<locus*> bim::markers(){
    vector<locus*> RetVec;
    vector<locus*>::iterator iLoci;
    for(iLoci = loci.begin();iLoci != loci.end();iLoci ++)
        RetVec.push_back(*iLoci);
    return RetVec;
}
//-----------------------------------------------------------
// bed class
//-----------------------------------------------------------
bed::bed():SampleSize(-9){};

bool bed::openBinaryFile(string s, ifstream & BIT)
{

    BIT.open(s.c_str(), ios::in | ios::binary);

    // 1) Check for magic number
    // 2) else check for 0.99 SNP/Ind coding
    // 3) else print warning that file is too old

    char ch[1];
    BIT.read(ch,1);
    bitset<8> b;
    b = ch[0];

    bool bfile_SNP_major = false;
    bool v1_bfile = true;

    // If v1.00 file format
    // Magic numbers for .bed file: 00110110 11011000 = v1.00 bed file
    if (   ( b[2] && b[3] && b[5] && b[6] ) &&
        ! ( b[0] || b[1] || b[4] || b[7] )    )
    {

        // Next number
        BIT.read(ch,1);
        b = ch[0];
        if (   ( b[0] && b[1] && b[3] && b[4] ) &&
            ! ( b[2] || b[5] || b[6] || b[7] )    )
        {
            // Read SNP/Ind major coding
            BIT.read(ch,1);
            b = ch[0];
            if ( b[0] ) bfile_SNP_major = true;
            else bfile_SNP_major = false;

            if (bfile_SNP_major)
                printf("Detected that binary PED file is v1.00 SNP-major mode\n");
            else
                printf("Detected that binary PED file is v1.00 individual-major mode\n");

        } else v1_bfile = false;

    } else v1_bfile = false;


    // Reset file if < v1
    if ( ! v1_bfile )
    {
        fprintf(stderr,"Warning, old BED file <v1.00 : will try to recover...\n");
        fprintf(stderr,"  but you should --make-bed from PED )\n");
        BIT.close();
        BIT.clear();
        BIT.open(s.c_str(), ios::in | ios::binary);
        BIT.read(ch,1);
        b = ch[0];
    }

    // If 0.99 file format
    if ( (!v1_bfile) && ( b[1] || b[2] || b[3] || b[4] || b[5] || b[6] || b[7] ) )
    {
        fprintf(stderr,"\n *** Possible problem: guessing that BED is < v0.99      *** \n");
        fprintf(stderr," *** High chance of data corruption, spurious results    *** \n");
        fprintf(stderr," *** Unles you are _sure_ this really is an old BED file *** \n");
        fprintf(stderr," *** you should recreate PED -> BED                      *** \n\n");

        bfile_SNP_major = false;
        BIT.close();
        BIT.clear();
        BIT.open(s.c_str(), ios::in | ios::binary);
    }
    else if ( ! v1_bfile )
    {
        if ( b[0] ) bfile_SNP_major = true;
        else bfile_SNP_major = false;

        fprintf(stderr,"Binary PED file is v0.99\n");

        if (bfile_SNP_major)
            printf("Detected that binary PED file is in SNP-major mode\n");
        else
            printf("Detected that binary PED file is in individual-major mode\n");
    }

    return bfile_SNP_major;

}

void bed::read(string bed_file,bim &markers,int sample_size){
    SampleSize = sample_size;
    printf("Reading genotype bitfile from [ %s ] \n",bed_file.c_str());

    ifstream BIT;
    bool bfile_SNP_major = openBinaryFile(bed_file, BIT);

    if (bfile_SNP_major)
    {

    }

    ///////////////////////////
    // SNP-major mode

    if (bfile_SNP_major){
        // Outer loop for SNPs
        int s=0;
        while (s<markers.size()) // for all SNPs
	    {
            bGeno_SNPmajor *bGeno = new bGeno_SNPmajor(sample_size,markers.get(s));

            // Inner loop for individuals
            int indx = 0;
            int ss = sample_size;

            while ( indx < ss ){
                bitset<8> b;
                char ch[1];

                BIT.read(ch,1);

                if (!BIT)
                    fprintf(stderr,"Problem with the BED file...has the FAM/BIM file been changed?\n");

                b = ch[0];

                for (int c = 0; c < 8; c+=2){
                    if (indx < ss)
                        bGeno->append(b[c], b[c+1]);
                    indx++;
                }
            }
            bgenos.push_back(bGeno);
            // next SNP
            s++;
	    }
	} else {
        fprintf(stderr, "Individual-major mode is not supported by this program.\n");
        exit(1);
	}

    // Check that we got what we expected

    char ch[1];
    BIT.read(ch,1);
    if (BIT)
        fprintf(stderr,"Problem with the BED file... has the FAM/BIM file been changed?\n");

    BIT.clear();
    BIT.close();
}

void bed::count(){
    vector<bGeno_SNPmajor*>::iterator b;
    for(b = bgenos.begin();b != bgenos.end();b++)
        cout << (*b)->loc->name << "\t" << (*b)->count().str() << endl;
}

void bed::indLevel_count(){
    if (SampleSize == -9) SampleSize = bgenos[0]->size();

}

vector<GENO*> bed::genotypes_per_marker(int SNPidx){
    return bgenos[SNPidx]->get_genotypes();
}

vector<GENO*> bed::genotypes_per_ind(int INDidx){
    vector<GENO*> RetVec;
    RetVec.clear();
    RetVec.reserve(SampleSize);
    vector<bGeno_SNPmajor*>::iterator ib;
    for(ib = bgenos.begin();ib != bgenos.end();ib++)
        RetVec.push_back((*ib)->get_genotype(INDidx));
    return RetVec;
}

//-----------------------------------------------------------
// binary_ped class
//-----------------------------------------------------------
binary_ped::binary_ped():plink_head(""){
}

binary_ped::binary_ped(string file_head):plink_head(file_head){
    read();
}

binary_ped::binary_ped(string file_head, string snp_file):plink_head(file_head){
    read();
    if (snp_file != "") extractSNPs(snp_file);
}

void binary_ped::read(string file_head){
    plink_head = file_head;
    read();
}

void binary_ped::read(){
    if (plink_head == "") {
        fprintf(stderr, "Error at reading binary ped files: header name is not specified.\n");
        return;
    }

    checkFileExists(plink_head+".bed");
    checkFileExists(plink_head+".bim");
    checkFileExists(plink_head+".fam");

    fa.read(plink_head+".fam");
    bi.read(plink_head+".bim");
    setSNPsInclude();
    be.read(plink_head+".bed",bi,fa.size());
}

void binary_ped::setSNPsInclude(){
    SNPincl.reserve(bi.size());
    for (int i = 0; i < bi.size(); i++)
        SNPincl[i].set(0,1);
}

void binary_ped::extractSNPs(string snp_file){
    for (int i = 0; i < bi.size(); i++)
        SNPincl[i].set(0,0);

    ifstream SNPS(snp_file.c_str(),ios::in);
    SNPS.clear();

    int c = 0,idx;
    while (!SNPS.eof()){
        string snp;
        SNPS >> snp;
        if ((idx = bi.get_index(snp)) != -9){
            SNPincl[idx].set(0,1);
            c++;
        }
        getline(SNPS,snp);
    }
    cout << c << " SNPs selected from [ " << snp_file << " ]" << endl;
    SNPS.clear();
    SNPS.close();
}

//void binary_ped::test_markerNames(){
//    cout << "Writing marker names..." << endl;
//    vector<string> markers = be.markerNames();
//
//    vector<string>::iterator m;
//    for(m = markers.begin();m != markers.end();m++)
//        cout << *m << endl;
//
//}

//void binary_ped::test_count(){
//    be.count();
//}

GENO* binary_ped::genotype(int INDidx,int SNPidx){
    return be.bgenos[SNPidx]->get_genotype(INDidx);

//    vector<GENO> genos = be.genotypes_per_marker(SNPidx);
//    return genos[INDidx];
}

GENO* binary_ped::genotype(string FID,string IID,string MarkerName){
    GENO *RetGeno;
    int SNPidx = bi.get_index(MarkerName);
    int INDidx = fa.get_index(FID,IID);
    if (SNPidx == -9 || INDidx == -9) return NULL;

    vector<GENO*> genos = be.genotypes_per_marker(SNPidx);
    RetGeno = genos[INDidx];
//    individual *tmp = new individual(FID,IID,"0","0","0","0");
//    RetGeno->ind = tmp;
    return RetGeno;
}

vector<GENO*> binary_ped::genotypes(string MarkerName){
    int SNPidx = bi.get_index(MarkerName);
    if (SNPidx == -9){
        fprintf(stderr, "Marker name [ %s ] not found.\n",MarkerName.c_str());
        vector<GENO*> RetGenoV;
        RetGenoV.clear();
        return RetGenoV;
    }

    vector<GENO*> genos = be.genotypes_per_marker(SNPidx);
    // vector<individual*> inds = fa.inds();
//    for(int i=0;i < (int)genos.size();i++)
//        genos[i]->ind = fa.sample[i];
    return genos;
}

vector<GENO*> binary_ped::genotypes(int MarkerIdx)
{
    vector<GENO*> genos = be.genotypes_per_marker(MarkerIdx);
    // vector<individual*> inds = fa.inds();
//    for(int i=0;i < (int)genos.size();i++)
//        genos[i]->ind = fa.sample[i];

    return genos;
}

vector<GENO*> binary_ped::genotypes(string FID,string IID){
    int INDidx = fa.get_index(FID, IID);
    if (INDidx == -9){
        fprintf(stderr, "Individual [ %s %s ] not found.\n",FID.c_str(),IID.c_str());
        vector<GENO*> RetGenoV;
        RetGenoV.clear();
        return RetGenoV;
    }

    vector<GENO*> genos = be.genotypes_per_ind(INDidx);
    vector<locus*> markers = bi.markers();

    // individual *tmp = new individual(FID, IID, "0", "0", "0", "-9");
//    for(int i = 0; i < (int)genos.size();i++)
//        genos[i]->ind = fa.sample[INDidx];

    return genos;
}

vector<locus*> binary_ped::markers(){
    vector<locus*> ret_v(bi.size()),orig_v =bi.markers();

    int c = 0;
    for (int i = 0; i < bi.size(); i++)
        if (SNPincl[i].any())
            ret_v[c++] = orig_v[i];
    ret_v.resize(c);
    return ret_v;
}

void binary_ped::OverAllCallRate(){
    int NumM = 0, c = 0;
    for (vector<bGeno_SNPmajor*>::iterator ib = be.bgenos.begin(); ib != be.bgenos.end(); ib++) {
        NumM += (*ib)->NumMissing();
        c += be.SampleSize;
    }
    cout << fixed << setprecision(4) << (double)NumM / (double) c;
}

int binary_ped::SampleSize()
{
    return (int)fa.size();
}

int binary_ped::MarkerSize()
{
    int c = 0;
    for(int i = 0;i < (int)bi.size();i++)
        if (SNPincl[i].any()) c++;
    return c;
}

void binary_ped::add_dummy(string FID,string IID,string sex)
{
    individual* dummy = new individual(FID,IID,"0","0",sex,"-9");
    fa.sample.push_back(dummy);

    for(int i=0;i<(int)bi.loci.size();i++)
    {
        bitset<2> missing(1);
        be.bgenos[i]->append(missing[0], missing[1]);
    }
}
