/*
 flatfile.cpp
 
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

#include <stdio.h>
#include <fstream>
#include <map>
#include "flatfile.h"
#include "output.h"
#include <boost/algorithm/string.hpp>

void ErrorWithStopMessage(string msg)
{
	fprintf(stderr,"%s\n",msg.c_str());
	exit(1);
}

flatfile::flatfile():nSNP(0),nSamp(0),b(NULL),gzr(NULL){
	b = new binary_ped();
}

int flatfile::modechange(string &s)
{
	boost::trim_if(s,boost::is_any_of("\n\r"));
	if (s == "[Header]") return 0;
	if (s == "[Data]") return 1;
	return -9;
}

void flatfile::readHeaderLine(string &s)
{
	vector<string> strs;
	boost::trim_if(s,boost::is_any_of("\n\r"));
	boost::split(strs,s,boost::is_any_of(","));
	if (strs[0] == "Num SNPs")
	{
		if (! isNumeric(strs[1])) ErrorWithStopMessage("Num SNPs is not numeric [ "+strs[1]+" ]");
		nSNP = atoi(strs[1].c_str());
		printf("Number of markers [ %d ]\n",nSNP);
		b->bi.loci.resize(nSNP);
	}
	else if (strs[0] == "Num Samples")
	{
		if (! isNumeric(strs[1])) ErrorWithStopMessage("Num Samples is not numeric [ "+strs[1]+" ]");
		nSamp = atoi(strs[1].c_str());
		printf("Number of individuals [ %d ]\n",nSamp);
		b->fa.sample.resize(nSamp);
	}
}

void flatfile::readSampNames(string &s)
{
	vector<string> strs;
	boost::trim_if(s,boost::is_any_of("\n\r"));
	boost::split(strs,s,boost::is_any_of(","));

	if ((int)strs.size() != nSamp + 1) ErrorWithStopMessage("Num Samples value and actual sample size differ.");

	for (int i=1;i < (int)strs.size();i++)
	{
		individual *ind = new individual(strs[i],strs[i]);
		b->fa.sample[i-1] = ind;
	}
}

void flatfile::readGenotypes(string &s,int SNPidx)
{
	vector<string> strs;
	boost::trim_if(s,boost::is_any_of("\n\r"));
	boost::split(strs,s,boost::is_any_of(","));

	/* get locus info */
	string a1,a2;
	determineAlleles(strs,a1,a2);
	locus *l = new locus(0,strs[0],0,a1,a2);

	b->bi.loci[SNPidx] = l;

	/* store genotypes */
	bGeno_SNPmajor *bg = new bGeno_SNPmajor(nSamp,l);
	for(int i=1; i < nSamp+1;i++)
	{
		bool ba1=true,ba2=false;
		if (strs[i] == a1+a1){
			ba1 = false;	ba2 = false;
		} else if (strs[i] == a1 + a2 || strs[i] == a2 + a1){
			ba1 = false;	ba2 = true;
		} else if (strs[i] == a2 + a2){
			ba1 = true;	ba2 = true;
		}
		bg->append(ba1,ba2);
	}

	b->be.bgenos[SNPidx] = bg;
}

void flatfile::createBits(string &s,int SNPidx,vector<bitset<8> > &bits,string &SNPname,string &a1,string &a2)
{
	vector<string> strs;
	boost::trim_if(s,boost::is_any_of("\n\r"));
	boost::split(strs,s,boost::is_any_of(","));


	/* get locus info */
//	string a1,a2;
	determineAlleles(strs,a1,a2);
	SNPname = strs[0];

	bits.clear();
	int c = 0;
	bitset<8> bit;
	bit.reset();

	/* store genotypes */
	for(int i=1; i < nSamp+1;i++)
	{
		if (strs[i] == a1+a1){
			bit.set(c++,0);	bit.set(c++,0);
		} else if (strs[i] == a1 + a2 || strs[i] == a2 + a1){
			bit.set(c++,0);	bit.set(c++,1);
		} else if (strs[i] == a2 + a2){
			bit.set(c++,1);	bit.set(c++,1);
		} else {
			bit.set(c++,1);	bit.set(c++,0);
		}
		if (c > 7)
		{
			bits.push_back(bit);
			bit.reset();
			c = 0;
		}
	}
	if (c > 0) bits.push_back(bit);
}

void flatfile::CheckStrangeAllele(multimap<int,int> &Sals)
{
	multimap<int,int>::iterator im;

	vector<int> validAlleles;// ,obsAlleles;
	int alleles[5]={'A','B','C','G','T'};
	for(int i=0;i<5;i++) validAlleles.push_back(alleles[i]);
	for(im = Sals.begin();im != Sals.end();im++)
	{
		if (find(validAlleles.begin(),validAlleles.end(),im->second) == validAlleles.end()){
			char tmp[255];
			snprintf(tmp,255,"Invalid allele [ %c ] found.",im->second);
			ErrorWithStopMessage(string(tmp));
		}
	}


}

void flatfile::determineAlleles(vector<string> &genos,string &a1,string &a2)
{
	int	alcnt[128];

	for(int i=0;i<128;i++) alcnt[i] = 0;
	for(int i=1;i < (int)genos.size();i++)
	{
		if ((int)genos[i].size() != 2) {

			ErrorWithStopMessage("Invalid genotype [ "+genos[i]+" ] at [ "+genos[0]+" ]");
		}
		alcnt[(int)genos[i][0]] ++;	alcnt[(int)genos[i][1]] ++;
	}

	/* count and sort alleles (without missing allele)*/
	multimap<int,int> Sals;
	for(int i=0;i<128;i++) if (alcnt[i] > 0 && i != '-') Sals.insert(make_pair(alcnt[i],i));

	/* Check existence of strange alleles */
	CheckStrangeAllele(Sals);

	/* Check multialleleic loci */
	if ((int)Sals.size() > 2) ErrorWithStopMessage("Multialleleic loci found.");

	a1 = a2 = "0";

	/* Finished if completely missing */
	if ((int)Sals.size() == 0) {
		a1 = "0";	a2 = "0";
		return;
	}


	/* Determine alleles */
	multimap<int,int>::iterator im;
	vector<string> Fals;
	for (im = Sals.begin();im != Sals.end();im++) {
		char tmp[2];
		tmp[0] = im->second; tmp[1] = 0;
		Fals.push_back(string(tmp));
	}

	if ((int)Sals.size() == 1)
	{
		a1 = "0"; a2 = Fals[0];
	} else {
		a1 = Fals[0];	a2 = Fals[1];
	}
}

void flatfile::readLine(string &s,int mode,int LineCnt)
{

	if (mode == 0)
	{
		readHeaderLine(s);
		b->be.bgenos.resize(nSNP);
	}
	else if (mode == 1)
	{
		if (LineCnt == 1)
			readSampNames(s);
		else if (LineCnt > 1)
			readGenotypes(s,LineCnt - 2);
	}
}

void flatfile::read(string FileName)
{
	checkFileExists(FileName);

	gzr = new GzReader(FileName);
	string s;
	int mode = 0, c=0;

    while(!(s = gzr->GetGzLine()).empty()){

    	if (s[0] == '[')
    	{
    		mode = modechange(s);
    	}
    	else
    		readLine(s,mode,c);

    	if (mode == 1) c++;

    }

    printf("%d data lines were loaded.\n",c);
    b->setSNPsInclude();
}

void flatfile::writeBEDheader(ofstream &BIT)
{
	bitset<8> bits;
	char ch[1];

	// Magic numbers for .bed file: 00110110 11011000 = v1.00 bed file

	bits.reset();
	bits.set(2);  bits.set(3);  bits.set(5);  bits.set(6);
	ch[0] = (char)bits.to_ulong();
	BIT.write(ch,1);

	bits.reset();
	bits.set(0);  bits.set(1);  bits.set(3);  bits.set(4);
	ch[0] = (char)bits.to_ulong();
	BIT.write(ch,1);

	// BIT represents status of SNP-major (true) or Ind-major (false)
	// (in this code, individual-major mode is disabled

	bits.reset();
//	if (par::out_SNP_major)
	bits.set(0);
	ch[0] = (char)bits.to_ulong();
	BIT.write(ch,1);
}

void flatfile::writeBit(bitset<8> &bit,ofstream &BIT)
{
	char ch[1];
	ch[0] = (char)bit.to_ulong();
	BIT.write(ch,1);
}

void flatfile::writeBITdirect(string FileName,string OutFile)
{
	checkFileExists(FileName);
	gzr = new GzReader(FileName);
	string s;
	int mode = 0;

//    printf("Writing genotype bitfile to [ %s ] \n",(OutFile+".bed").c_str());
//    ofstream BIT((OutFile+".bed").c_str(), ios::out | ios::binary);
//    printf("Using (default) SNP-major mode\n");
//    writeBEDheader(BIT);
//
//    ofstream BIM((OutFile+".bim").c_str(), ios::out);

	// Read header
    while(!(s = gzr->GetGzLine()).empty()){
    	if (s[0] == '[')
    		mode = modechange(s);
    	else if (mode == 0)
    		readHeaderLine(s);
    	else {
    		readSampNames(s);
    		break;
    	}
    }

    // FAM file is written
    Gout go(b);
	go.write_FAMFILE(OutFile+".fam");

    // Read data lines and write into BED file directly
    printf("Writing genotype bitfile to [ %s ] \n",(OutFile+".bed").c_str());
    ofstream BIT((OutFile+".bed").c_str(), ios::out | ios::binary);
    printf("Using (default) SNP-major mode\n");
    writeBEDheader(BIT);
    
    ofstream BIM((OutFile+".bim").c_str(), ios::out);
    // Also, write BIM file directly
    int c = 0;
    while(!(s = gzr->GetGzLine()).empty()){
    	vector<bitset<8> > bits;
    	string SNPname,a1,a2;

		createBits(s,c,bits,SNPname,a1,a2);

		BIM << "0\t" << SNPname << "\t0\t0\t" << a1 << "\t" << a2 << "\n";

		for (vector<bitset<8> >::iterator ib = bits.begin();ib != bits.end();ib++)
			writeBit(*ib,BIT);

		c++;
    }

    printf("%d data lines were loaded and written.\n",c);
    BIT.close();
    BIM.close();
}

void flatfile::makebed(string OutFile)
{
	Gout go(b);
	go.write_BED(OutFile);
}

void flatfile::recode(string OutFile)
{
	Gout go(b);
	go.write_LINKAGE(OutFile);
}

void flatfile::debug()
{
	for(int i=0;i<10;i++) cout << b->fa.sample[i]->ID() << endl;
	for(int i=0;i<10;i++) cout << b->bi.loci[i]->name << endl;
}
