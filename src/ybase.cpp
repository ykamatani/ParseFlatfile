/*
 ybase.cpp
 
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

#include <fstream>
#include "ybase.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#define GZ_READ_LENGTH 10000

using namespace std;

timestamp::timestamp(){
    SetTime();
}

void timestamp::SetTime(){
    struct tm *date;
    time_t now;
    
    time(&now);
    date = localtime(&now);
    
    year = date->tm_year + 1900;
    month = date->tm_mon + 1;
    day = date->tm_mday;
    hour = date->tm_hour;
    minute = date->tm_min;
    second = date->tm_sec;
}

string timestamp::format1(){
    char buff[128];
    
    sprintf(buff,"%04d/%02d/%02d %02d:%02d:%02d",year,month,day,hour,minute,second);
    return string(buff);
}

void timestamp::StartMessage(){
    SetTime();
    cout << "Analysis started. [" << format1() << "]" << endl;
}

void timestamp::EndMessage(){
    SetTime();
    cout << "Finished. [" << format1() << "]" << endl;
}

void decimal::input(string val)
{
    char tmp[16];
    int counter=0;
    
    for(int i=0;i<(int)val.size();i++) {
        if (val[i] == '.'){
            inte = atoi(tmp);
            counter=0;
        } else {
            tmp[counter++] = val[i];
            tmp[counter]=0;
        }
    }
    dec=atoi(tmp);
}
void decimal::zero(){ inte=0;dec=0;}
string decimal::str(){
    char tmp[32];
    snprintf(tmp, 32, "%d.%d",inte,dec);
    return string(tmp);
}
double decimal::value(){
    char tmp[32];
    snprintf(tmp, 32, "%d.%d",inte,dec);
    return atof(tmp);
}

string baseFname(string str){
    vector<string> items;
    boost::algorithm::split(items,str,boost::is_any_of("/"));
    return items.back();
}

string baseFname(string str,string CutSuffix){
    vector<string>      items;
    string::size_type   CutPos;
    
    boost::algorithm::split(items,str,boost::is_any_of("/"));
    if ( (CutPos = items.back().rfind(CutSuffix)) == string::npos){
        return items.back();
    }
    return items.back().substr(0,items.back().rfind(CutSuffix));
}

/* Do not allow scientific format (e.g. 4e-10) */
bool isNumeric(string str)
{
    int pos=0,NumPoint=0;
    
    while (str[pos++] == ' ')
        ;
    
    if (str[pos] == '-' || str[pos] == '+') pos++;
    
    for(int i = pos;i < (int)str.size();i++)
    {
        if (str[i] < 48 || str[i] > 57)
        {
            if (str[i] == '.')
            {
                NumPoint++;
                if (NumPoint > 1)   return false;
            }
            else
                return false;
        }
    }
    
    return true;
}

void checkFileExists(string f)
{

    ifstream inp;

    inp.open(f.c_str(), ifstream::in);
    if(inp.fail())
    {
        inp.clear(ios::failbit);
        inp.close();
        string msg = "No file [ " + f + " ] exists.";
        fprintf(stderr, "%s\n",msg.c_str());
        exit(1);
    }
    inp.close();
    return;

}

/*----------------------------*/
/* methods for GzReader class */
/*----------------------------*/

// constructors

GzReader::GzReader():FileName(""),BufferStr(""),ReadLength(GZ_READ_LENGTH),delim(0),gzfIsOpened(false),fail(false)
{
    reading = (char*) malloc(sizeof(char) * ReadLength);
    reading[0] = 0;
    BufferStrs.clear();
}

GzReader::GzReader(string file_name):FileName(file_name),BufferStr(""),ReadLength(GZ_READ_LENGTH),delim(0),gzfIsOpened(false),fail(false)
{
    reading = (char*) malloc(sizeof(char) * ReadLength);
    reading[0] = 0;
    BufferStrs.clear();
}

void GzReader::OpenGzFile()
{
    if (gzfIsOpened)
        return;
    
    if((gzf = gzopen(FileName.c_str(),"r")) == NULL)
    {
        fprintf(stderr,"File [ %s ] cannot be opened.\n",FileName.c_str());
        fail = true;
        return;
    }
    
    printf("Reading file [ %s ]...\n",FileName.c_str());
    gzfIsOpened = true;
}


void GzReader::OpenGzFile(string file_name)
{
    FileName = file_name;
    OpenGzFile();
}

int len(char* input){
    int i = 0;
    
    while(input[i++] != 0);
    return i;
}


char* lTrimChar(char* input,int point){
    int i;
    char* RetChar;
    
    RetChar = (char*) malloc(sizeof(char) * (len(input) - point));
    
    for(i = point;i < len(input) && input[i+1] != 0; i++)
        RetChar[i - point] = input[i+1];
    RetChar[i - point] = 0;
    return RetChar;
}


string GetStrAfterStr(string str,string cut){
    return str.substr(str.find(cut)+1);
}

char* CopyCharBeforeChar(char* str,char delim,bool &flag){
    int i = 0,j;
    
    while (str[i] != 0 && str[i] != delim)
        i++;
    
    flag = (str[i] == delim ? true : false);
    
    char *ret;
    ret = (char*) malloc(sizeof(char) * i);
    
    for(j = 0;j < i;j++)
        ret[j] = str[j];
    ret[j] = 0;
    
    return ret;
}

string CopyStrBeforeStr(string str,string delim,bool &DelimExists){
    if (str.find(delim) != string::npos){
        DelimExists = true;
        return str.substr(0,str.find(delim));
    }
    DelimExists = false;
    return str;
}

string CopyStrOfCharAfterChar(char *str,char delim){
    int i = 0,j,k;
    
    while (str[i] != 0 && str[i] != delim)
        i++;
    
    if (str[i] == 0)   // delim char is not found in the string
        return "";
    // delim char found
    j = i + 1;
    while (str[j] != 0)
        j++;
    char tmp[j];
    for (k = i+1;k < j;k ++)
        tmp[k - i - 1] = str[k];
    tmp[k - i - 1] = 0;
    return string(tmp);
}

string GzReader::GetGzLine()
{
    OpenGzFile(FileName);
    
    int     char_lim;
    
    if (BufferStr.find("\n") != string::npos){ // BufferStr has return code
        vector<string> items;
        boost::split(items, BufferStr, boost::is_any_of("\n"));
        BufferStr = GetStrAfterStr(BufferStr,"\n");
        return items[0];
    }
    
    while((char_lim = gzread(gzf, reading, ReadLength)) > 0){
        reading[char_lim] = 0;
        bool ReturnCodeExists;
        string ThisStr = CopyStrBeforeStr(string(reading),"\n",ReturnCodeExists);
        if (ReturnCodeExists){ // return code exists in the string
            string RetStr = BufferStr + ThisStr;
            BufferStr = GetStrAfterStr(string(reading), "\n");
            return RetStr;
        }
        
        BufferStr += string(reading);
    }
    string RetStr = BufferStr;
    BufferStr.clear();
    return RetStr;
}

void GzReader::counter(int &Nrow,int &Ncol,int &TitleNcol,bool verbose){
    printf("Checking file size of [ %s ]...",FileName.c_str());
    counter(Nrow, Ncol, TitleNcol);
    printf(" %d rows and %d cols.\n",Nrow,Ncol);
}

void GzReader::counter(int &Nrow,int &Ncol,int &TitleNcol){
    gzFile gz2;
    Nrow=Ncol=TitleNcol=0;
    
    if (FileName == ""){
        fprintf(stderr,"FileName should be set before analysis.\n");
    } else if ((gz2 = gzopen(FileName.c_str(),"r")) == NULL) {
        fprintf(stderr, "Could not open [ %s ].\n",FileName.c_str());
    } else {
        int     char_lim,i,line_cnt = 0,col_cnt = 0,contents = 0;
        char    tmp[ReadLength];
        
        while((char_lim = gzread(gz2, tmp, ReadLength)) > 0){// printf("%s",tmp);
            for( i = 0;i < char_lim; i++){
                if (tmp[i] != '\0' && tmp[i] != '\n' && !isDelim(tmp[i])){
                    contents++;
                } else if (tmp[i] == '\n'){
                    if (contents) col_cnt++;
                    ColCountDecision(line_cnt,col_cnt,Ncol,TitleNcol);
                    line_cnt++;
                    col_cnt = 0;
                    contents = 0;
                } else if (isDelim(tmp[i])){
                    if (delim != 0){
                        col_cnt++;
                    } else {
                        if (contents) col_cnt++;
                    }
                    contents = 0;
                } else if (tmp[i] == 0){
                    break;
                }
            }
        }
        gzclose(gz2);
        Nrow = line_cnt;
    }
}

bool GzReader::exist(){
    if (FileName == ""){
        fprintf(stderr, "File name has not been specified.\n");
        return false;
    }
    OpenGzFile();
    return gzfIsOpened;
}

bool GzReader::exist(string file_name){
    if (FileName != ""){
        fprintf(stderr, "File name [ %s ]has already been specified, but new file name [ %s ] is given; it is ignored.\n",FileName.c_str(),file_name.c_str());
    } else FileName = file_name;
    OpenGzFile();
    return gzfIsOpened;
}

///////////////////////////
// utilities
///////////////////////////
void GzReader::ColCountDecision(int line_cnt,int col_cnt,int &Ncol,int &TitleNcol){
    if (line_cnt == 0){
        TitleNcol = col_cnt;
    } else if(Ncol != -9) {
        if (Ncol == 0){
            Ncol = col_cnt;
        } else if(col_cnt != Ncol){
            fprintf(stderr, "Column count inconsistency at line %d.\n",line_cnt);
            Ncol = -9;
        }
    }
}

bool GzReader::isDelim(char input){
    if (delim != 0){
        if (input == delim) return true;
        return false;
    }
    if (input == ' ' || input == '\t') return true;
    return false;
}
