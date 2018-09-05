/*
 * locus.h
 *
 *  Created on: 2013/08/15
 *      Author: Kamatani Yoichiro
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
