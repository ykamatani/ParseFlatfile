# ParseFlatfile: Utility program to convert Illumina FinalReport file to plink binary file.

This is prepared to accelerate the use of RIKEN's BioBank Japan genotype data release
	`https://humandbs.biosciencedbc.jp/en/hum0014-v10`

To compile program, simply do this

```
make
```

After obtaining FinalReport files, it is easy to convert it into plink format

```
ParseFlatfile --flat BBJ_OmniExpressV1_XChr_Cutoff_AB_20130611_FinalReport.csv.gz --make-bed --out BBJ_OmniExpressV1_XChr_Cutoff_AB_20130611
```

Yoichiro Kamatani, 2018
Contact: yoichiro.kamatani@riken.jp
