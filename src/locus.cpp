/*
 locus.cpp
 
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



