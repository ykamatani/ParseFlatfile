/*
 locus.h
 
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

#ifndef LOCUS_H_
#define LOCUS_H_

#include <string>

using namespace std;

class locus{
private:
public:
    int chrom,pos;
    string name,a1,a2;

    locus();
    locus(string tName);
    locus(int tchrom,string tName,int tPos,string tA1,string tA2);

    void copy(locus &l);
    string str4();
    string str6();
    string posName();
};

#endif /* LOCUS_H_ */
