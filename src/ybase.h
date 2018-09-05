/*
 ybase.h
 
 Created by Yoichiro Kamatani on 8/20/12.
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

#ifndef __ParsePlink__ybase__
#define __ParsePlink__ybase__

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "time.h"
#include "zlib.h"

using namespace std;

string baseFname(string str);
string baseFname(string str,string suffix);
bool isNumeric(string str);
void checkFileExists(string f);

class timestamp{
private:
    int year, month, day, hour, minute, second;
    void SetTime();
public:
    timestamp();
    string format1();
    void StartMessage();
    void EndMessage();
};

class decimal
{
public:
    int inte,dec;
    
    void input(string val);
    void zero();
    string str();
    double value();
};

class GzReader{
    /*  Usage
            GzReader gz() ; parameter can be given to open file simulataneously with generating instance
            gz.GetGzLine() to get line as string
            gz.GetGzItems() to get items separated by delimitor delim
            gz.SetDelimitor(char) to set delimitor. if not, text is assumed to be separated by consecutive space / tab 
     */
private:
    
    gzFile  gzf;
    string  FileName,BufferStr;
    int     ReadLength; // determined in its constructor
    vector<string> BufferStrs;
    char    *reading,delim;
    bool    gzfIsOpened,fail;
    
    // Utility methods
    bool    isDelim(char input);
    void    ColCountDecision(int line_cnt,int col_cnt,int &Ncol,int &TitleNcol);
public:
    GzReader(string file_name);
    GzReader();
    void OpenGzFile();
    void OpenGzFile(string FileName);
    
    void SetDelimitor(char adelim){delim = adelim;}
    
    string GetGzLine();
    vector<string> GetGzItems();
    void counter(int &Nrow,int &Ncol,int &TitleNcol,bool verbose);
    void counter(int &Nrow,int &Ncol,int &TitleNcol);
    bool exist();
    bool exist(string file_name);
};

#endif /* defined(__ParsePlink__ybase__) */
