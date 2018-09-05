/*
 output.h
 
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
