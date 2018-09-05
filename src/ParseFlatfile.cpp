/*
  ParseFlatfile.cpp

  Created by Yoichiro Kamatani on 3/9/13.
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

#include <iostream>
#include <getopt.h>
#include "ybase.h"
#include "bed2.h"
#include "flatfile.h"

using namespace std;

void check_arg(string &check,string errmsg){
    if (check == ""){
        cerr << errmsg << endl;
        exit(1);
    }
}

int main(int argc, char ** argv)
{
    struct option opts[]={
        {"flatfile", required_argument, 0, 'f'},
        {"make-bed", no_argument, 0, 'b'},
        {"recode", no_argument, 0, 'r'},
        {"out", required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };
    int     opt,option_index = 0;
//    int num_threads = 1;
    string  fFile = "",oFile = "";
    bool makebed = false, recode = false;

    while((opt = getopt_long(argc, argv, "f:o:", opts, &option_index)) != -1){
        switch (opt) {
            case 'f':   fFile = string(optarg);		break;
            case 'b':   makebed = true;				break;
            case 'r':   recode = true;				break;
//            case 't':   num_threads = atoi(optarg);	break;
            case 'o':   oFile = string(optarg);		break;
            default:    break;
        }
    }
    
    timestamp *t = new timestamp();
    t->StartMessage();

    if (makebed)
    {
		check_arg(fFile, "Illumina's flat file name is not specified.");
		check_arg(oFile, "Output file name is not specified.");

		flatfile f;
//		f.read(fFile);
//		f.makebed(oFile);
		f.writeBITdirect(fFile,oFile);
//		f.debug();
    }
    
    if (recode)
    {
		check_arg(fFile, "Illumina's flat file name is not specified.");
		check_arg(oFile, "Output file name is not specified.");

		flatfile f;
		f.read(fFile);
		f.recode(oFile);
    }

    t->EndMessage();

    return 0;
}

