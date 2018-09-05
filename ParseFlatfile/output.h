/*
 * output.h
 *
 *  Created on: 2013/08/15
 *      Author: Kamatani Yoichiro
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include <string>
#include "bed2.h"

using namespace std;

class Gout{
private:
	binary_ped *b;
public:
	Gout(binary_ped *bp):b(bp){}

	/* write out plink flat format */
	void write_LINKAGE(string FileName);
	void write_PEDFILE(string FileName);
	void write_MAPFILE(string FileName);

	/* write out binary ped files */
	void write_BED(string FileName);
	void write_FAMFILE(string FileName);
	void write_BIMFILE(string FileName);
	void write_BEDFILE(string FileName);

};



#endif /* OUTPUT_H_ */
