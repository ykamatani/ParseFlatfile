/*
 bGeno.cpp
 
 Created by Yoichiro Kamatani on 2013/08/15.
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

#include "stdio.h"
#include "stdlib.h"
#include "bGeno.h"

static bitset<2> hom1(0);
static bitset<2> het(1);
static bitset<2> miss(2);
static bitset<2> hom2(3);

//-----------------------------------------------------------
// bGeno_SNPmajor class
//-----------------------------------------------------------
bGeno_SNPmajor::bGeno_SNPmajor(int nSamp,locus* l):Ngeno(0),loc(l){
	one.resize(nSamp/8+1,0);
	two.resize(nSamp/8+1,0);
}

bGeno_SNPmajor::bGeno_SNPmajor(int nSamp):Ngeno(0),loc(NULL){
	one.resize(nSamp/8+1,0);
	two.resize(nSamp/8+1,0);
}

void bGeno_SNPmajor::append(bool b1, bool b2){
	one[Ngeno / 8].set(Ngeno % 8,b1);
	two[Ngeno / 8].set(Ngeno % 8,b2);
	Ngeno++;
}

geno_cnt bGeno_SNPmajor::count(){
    geno_cnt cnt;
    for(int i = 0;i < Ngeno; i++)
    {
    	bitset<2> tmp;
    	tmp[1] = one[i / 8][i % 8];
    	tmp[0] = two[i / 8][i % 8];
    	cnt.add(tmp);
    }
    return cnt;
}

geno_cnt bGeno_SNPmajor::count(vector<int> &INDindices){
    geno_cnt cnt;
    vector<int>::iterator ii;
	for(ii = INDindices.begin();ii != INDindices.end();ii++)
    {
    	bitset<2> tmp;
    	tmp[1] = one[*ii / 8][*ii % 8];
    	tmp[0] = two[*ii / 8][*ii % 8];
    	cnt.add(tmp);
    }
    return cnt;
}

vector<GENO*> bGeno_SNPmajor::get_genotypes(){
    vector<GENO*> RetVec(0);
    RetVec.reserve(Ngeno);

    for(int i = 0;i < Ngeno;i++)
    {
    	GENO *tmp = get_genotype(i);
        RetVec.push_back(tmp);
    }
    return RetVec;
}

GENO* bGeno_SNPmajor::get_genotype(int idx){
    GENO *tmp = new GENO();

    switch (dosage(idx)) {
        case 0: 	tmp->G1 = tmp->G2 = loc->a1;          break;
        case 1: 	tmp->G1 = loc->a1; tmp->G2 = loc->a2; break;
        case -9: 	tmp->G1 = tmp->G2 = "0";              break;
        case 2: 	tmp->G1 = tmp->G2 = loc->a2;          break;
        default: break;
    }
    return tmp;
}

int bGeno_SNPmajor::dosage(int idx) {
	bitset<2> tmp;
	tmp[1] = one[idx / 8][idx % 8];
	tmp[0] = two[idx / 8][idx % 8];

	if (tmp == hom1)
		return 0;

	if (tmp == het)
		return 1;

	if (tmp == hom2)
		return 2;

	return -9;
}

double bGeno_SNPmajor::Freq1(){
	geno_cnt cnt = count();
	return cnt.Freq1();
}

double bGeno_SNPmajor::Freq1(vector<int> &INDindices){
	geno_cnt cnt = count(INDindices);
	return cnt.Freq1();
}

int bGeno_SNPmajor::NumMissing(){
	geno_cnt cnt = count();
    return cnt.counts[2];
}

int bGeno_SNPmajor::NumMissing(vector<int> &INDindices){
	geno_cnt cnt = count(INDindices);
    return cnt.counts[2];
}

//-----------------------------------------------------------
// GENO class
//-----------------------------------------------------------
void GENO::inputBinary(bitset<2> &b,locus &l){
    switch (b.to_ulong()) {
        case 0: G1 = G2 = l.a1;         break;
        case 1: G1 = l.a1; G2 = l.a2;   break;
        case 2: G1 = G2 = "0";          break;
        case 3: G1 = G2 = l.a2;         break;
        default: break;
    }
}

bool GENO::isMissing(){
    if (G1 == "0" || G2 == "0") return true;
    return false;
}

bool GENO::isConsistent(GENO *geno2){
    if ((G1 == geno2->G1 && G2 == geno2->G2) || (G1 == geno2->G2 && G2 == geno2->G1))
        return true;
    return false;
}

//-----------------------------------------------------------
// geno_cnt class
//-----------------------------------------------------------
geno_cnt::geno_cnt(){
    counts.resize(4);
    counts[0] = 0;  // AA
    counts[1] = 0;  // AB
    counts[2] = 0;  // missing
    counts[3] = 0;  // BB
}

void geno_cnt::add(bitset<2> b){
    counts[b.to_ulong()] ++;
}

string geno_cnt::str(){
    char tmp[255];
    snprintf(tmp,255,"%d\t%d\t%d\t%d",counts[0],counts[1],counts[3],counts[2]);
    return string(tmp);
}

double geno_cnt::Freq1(){
	return (double) (counts[0]*2+counts[1]) / (double)(2*(counts[0] + counts[1] + counts[3]));
}

double geno_cnt::MAF(){
	int minor_cnt = (counts[0] < counts[3] ? counts[0] : counts[3]);
	return (double) (minor_cnt*2+counts[1]) / (double)(2*(counts[0] + counts[1] + counts[3]));
}
