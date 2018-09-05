//
//  flatfile.h
//  ParseFlatfile2
//
//  Created by Yoichiro Kamatani on 8/6/13.
//

#ifndef __ParseFlatfile__flatfile__
#define __ParseFlatfile__flatfile__

#include <string>
#include "ybase.h"
#include "bed2.h"

using namespace std;

class flatfile{
private:
	int nSNP,nSamp;
	binary_ped *b;
	GzReader *gzr;

	int modechange(string &s);
	void readHeaderLine(string &s);
	void readLine(string &s,int mode,int LineCnt);
	void readSampNames(string &s);
	void readGenotypes(string &s,int SNPidx);

	void CheckStrangeAllele(multimap<int,int> &Sals);
	void determineAlleles(vector<string> &genos,string &a1,string &a2);

	void createBits(string &s,int SNPidx,vector<bitset<8> > &bits,string &SNPname,string &a1,string &a2);
	void writeBit(bitset<8> &bit,ofstream &BIT);
public:
	flatfile();
	void read(string FileName);
	void makebed(string OutFile);
	void recode(string OutFile);
	void writeBEDheader(ofstream &BIT);
	void writeBITdirect(string FileName,string OutFile);

	void debug();
};

#endif /* defined(__ParseFlatfile__flatfile__) */
