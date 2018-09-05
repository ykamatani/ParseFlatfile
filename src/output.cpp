/*
 output.cpp
 
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

#include <iostream>
#include <fstream>
#include "output.h"

void Gout::write_MAPFILE(string FileName)
{
	printf( "Writing new map file to [ %s ] \n",FileName.c_str());
	ofstream MAP((FileName).c_str(), ios::out);

	int nMark = (int)b->bi.size();
	for (int l=0;l<nMark;l++)
	{
		if (b->SNPincl[l].any())
		{
			MAP << b->bi.loci[l]->chrom << "\t"
				  << b->bi.loci[l]->name << "\t"
				  << "0" << "\t"
				  << b->bi.loci[l]->pos << "\n";
		}
	}
	MAP.clear();
	MAP.close();
}

void Gout::write_PEDFILE(string FileName)
{
	printf( "Writing recoded ped file to [ %s ] \n",FileName.c_str());
	ofstream PED(FileName.c_str(), ios::out);
	PED.clear();

	string missingCode = "0";

	int nSamp = (int)b->fa.size();
	int nSNP = (int)b->bi.size();
	for (int i=0;i<nSamp;i++)
	{
		individual *person = b->fa.sample[i];
		PED << person->FID << " "
			  << person->IID << " "
			  << person->PID << " "
			  << person->MID << " "
			  << person->SEX << " ";
		/* Specific treatment for phenotype info will be made later, if necessary */
		PED << person->PHENO;

		for (int l=0;l<nSNP;l++) if (b->SNPincl[l].any())
		{
			GENO *g = b->be.bgenos[l]->get_genotype(i);
			PED << " " << g->str(" ");
			delete g;
		}

		PED << "\n";
	}
	PED.clear();
	PED.close();
}

void Gout::write_LINKAGE(string FileName)
{
	write_PEDFILE(FileName + ".ped");
	write_MAPFILE(FileName + ".map");
}

void Gout::write_FAMFILE(string FileName)
{
	printf( "Writing pedigree information to [ %s ] \n",FileName.c_str());
	ofstream FAM(FileName.c_str(), ios::out);

	int nSamp = (int)b->fa.size();
	for (int i=0;i<nSamp;i++)
	{
		individual *person = b->fa.sample[i];
		FAM << person->FID << " "
			  << person->IID << " "
			  << person->PID << " "
			  << person->MID << " "
			  << person->SEX << " ";
		/* Specific treatment for phenotype info will be made later, if necessary */
		FAM << person->PHENO << "\n";
	}
	FAM.clear();
	FAM.close();
}

void Gout::write_BIMFILE(string FileName)
{
	printf( "Writing map (extended format) information to [ %s ] \n",FileName.c_str());
	ofstream BIM((FileName).c_str(), ios::out);

	int nMark = (int)b->bi.size();
	for (int l=0;l<nMark;l++)
	{
		if (b->SNPincl[l].any())
		{
			BIM << b->bi.loci[l]->chrom << "\t"
				  << b->bi.loci[l]->name << "\t"
				  << "0" << "\t"
				  << b->bi.loci[l]->pos << "\t"
				  << b->bi.loci[l]->a1 << "\t"
				  << b->bi.loci[l]->a2 << "\n";
		}
	}
	BIM.clear();
	BIM.close();
}

void Gout::write_BEDFILE(string FileName)
{
	printf("Writing genotype bitfile to [ %s ] \n",FileName.c_str());
	ofstream BED(FileName.c_str(), ios::out | ios::binary);
	printf("Using (default) SNP-major mode\n");

	bitset<8> bits;
	char ch[1];

	// Magic numbers for .bed file: 00110110 11011000 = v1.00 bed file

	bits.reset();
	bits.set(2);  bits.set(3);  bits.set(5);  bits.set(6);
	ch[0] = (char)bits.to_ulong();
	BED.write(ch,1);

	bits.reset();
	bits.set(0);  bits.set(1);  bits.set(3);  bits.set(4);
	ch[0] = (char)bits.to_ulong();
	BED.write(ch,1);

	// BIT represents status of SNP-major (true) or Ind-major (false)
	// (in this code, individual-major mode is disabled

	bits.reset();
//	if (par::out_SNP_major)
	bits.set(0);
	ch[0] = (char)bits.to_ulong();
	BED.write(ch,1);

	//////////////////////////////////////////
	// Now consider genotypes: SNP-major mode

	// Outer loop over SNPs
	int nMark = (int)b->bi.size();
	for(int i = 0; i < nMark; i++){
		if (b->SNPincl[i].any())
		{
			bGeno_SNPmajor *bg = b->be.bgenos[i];
			vector<bitset<8> >::iterator i1 = bg->one.begin();
			vector<bitset<8> >::iterator i2 = bg->two.begin();
			int bitidx = 0;
			int nSamp = bg->size();
//			cout << nSamp << endl;
//			exit(1);
			int cnt = 0;// , cnt1 = 0, cnt2 = 0;

			// Inner loop over individuals
//			while (i1 != bg->one.end())
//			cout << "flag0" << endl;
			while (cnt < nSamp)
			{
				bitset<8> bits;
				bits.reset();
				int c = 0;
//				cout << "flag1" << endl;

//				while (c < 8 && cnt < nSamp && i1 != bg->one.end())
				while (c < 8 && cnt < nSamp)
				{
//					cout << "flag2" << endl;
					if ( (*i1)[bitidx] ) bits.set(c);
					c++;
					if ( (*i2)[bitidx] ) bits.set(c);
					c++;
					bitidx++;
					cnt++;

					if ( bitidx > 7)
					{
//						cout << "flag3" << endl;
						bitidx = 0;
//						cout << "[" << *i1 << "," << *i2 << "]" << endl;
						i1++;
						i2++;
					}
				}
//				cout << "flag4" << endl;
//				cout << cnt << ":" << bits << endl;

				char ch[1];
				ch[0] = (char)bits.to_ulong();
				BED.write(ch,1);
			}
//			exit(1);
		}
	}
	BED.close();
}

void Gout::write_BED(string FileName)
{
	write_FAMFILE(FileName + ".fam");
	write_BIMFILE(FileName + ".bim");
	write_BEDFILE(FileName + ".bed");
}
