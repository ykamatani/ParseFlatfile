/*
 * locus.cpp
 *
 *  Created on: 2013/08/15
 *      Author: Kamatani Yoichiro
 */

#include "stdio.h"
#include "locus.h"

//-----------------------------------------------------------
// locus class
//-----------------------------------------------------------

locus::locus():chrom(0),pos(-9),name(""),a1("-"),a2("-"){}
locus::locus(string tName):chrom(0),pos(-9),name(tName),a1("-"),a2("-"){}
locus::locus(int tchrom,string tName,int tPos,string tA1,string tA2):chrom(tchrom),pos(tPos),name(tName),a1(tA1),a2(tA2){}

void locus::copy(locus &l){
    chrom = l.chrom;
    pos = l.pos;
    name = l.name;
    a1 = l.a1;
    a2 = l.a2;
}

string locus::str4(){
    char RetStr[255];
    snprintf(RetStr, 255, "%d\t%s\t0\t%d",chrom,name.c_str(),pos);
    return string(RetStr);
}

string locus::posName(){
    char RetStr[32];
    snprintf(RetStr, 32, "%d:%d",chrom,pos);
    return string(RetStr);
}



