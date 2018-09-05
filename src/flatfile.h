/*
 flatfile.h
 
 Created by Yoichiro Kamatani on 8/06/13.
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
