////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by 
// Karol Wnukowicz (Visual Atoms) 
// Stavros Paschalakis (Visual Atoms) 
// parts based on SpatioTemporalLocator.cpp (K.Masukura (R&D Center Toshiba, Japan)
// in the course of development of the MPEG-7 Standard (ISO/IEC 15938). 
// This software module is an implementation of a part 
// of one or more MPEG-7 Standard tools as specified by the MPEG-7 Standard. 
// ISO/IEC gives users of the MPEG-7 Standard free license to this software
// module or modifications thereof for use in hardware or software products
// claiming conformance to the MPEG-7 Standard. 
// Those intending to use this software module in hardware or software products 
// are advised that its use may infringe existing patents. The original 
// developer of this software module and his/her company, the subsequent 
// editors and their companies, and ISO/IEC have no liability for use of this 
// software module or modifications thereof in an implementation.
// Copyright is not released for non MPEG-7 Standard conforming products. 
// <CN1> retains full right to use the code for his/her own purpose, 
// assign or donate the code to a third party and to inhibit third parties 
// from using the code for non MPEG-7 Standard conforming products. 
// This copyright notice must be included in all copies or derivative works. 
// Copyright (c)2003-2015
//
////////////////////////////////////////////////////////////////////////////////
//
// Applicable File Name:  media_time_tool.cpp
//

#include "media_time_tool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned long  MTT_GetMediaTimePointString(unsigned long ul,char* s) {
	sprintf(s,"T%02ld:%02ld:%02ld:%03ldF1000", MTT_ExtractHours(ul), MTT_ExtractMinutes(ul),
		MTT_ExtractSeconds(ul), MTT_ExtractMilliseconds(ul));
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  MTT_GetMediaDurationString(unsigned long ul,char* s) {
	if(MTT_ExtractHours(ul)){
		sprintf(s,"PT%ldH%ldM%ldS%ldN1000F", MTT_ExtractHours(ul), MTT_ExtractMinutes(ul),
			MTT_ExtractSeconds(ul), MTT_ExtractMilliseconds(ul) );
	} else if(MTT_ExtractMinutes(ul)){
		sprintf(s,"PT%ldM%ldS%ldN1000F", MTT_ExtractMinutes(ul), MTT_ExtractSeconds(ul), MTT_ExtractMilliseconds(ul) );
	} else if(MTT_ExtractSeconds(ul)){
		sprintf(s,"PT%ldS%ldN1000F", MTT_ExtractSeconds(ul), MTT_ExtractMilliseconds(ul) );
	} else if(MTT_ExtractMilliseconds(ul)){
		sprintf(s,"PT%ldN1000F", MTT_ExtractMilliseconds(ul) );
	}
	return 0;
}

//----------------------------------------------------------------------------
unsigned long  MTT_ExtractHours(unsigned long milliseconds) {
	unsigned long hrs=milliseconds/(3600*1000);
	return hrs%24;
}

//----------------------------------------------------------------------------
unsigned long  MTT_ExtractMinutes(unsigned long milliseconds) {
	unsigned long mts=milliseconds/(60*1000);
	return mts%60;
}

//----------------------------------------------------------------------------
unsigned long  MTT_ExtractSeconds(unsigned long milliseconds) {
	unsigned long seconds = milliseconds/1000;
	return seconds%60;
}

//----------------------------------------------------------------------------
unsigned long  MTT_ExtractMilliseconds(unsigned long milliseconds) {
	return milliseconds%1000;
}


// Media time point format: DDThh:mm:ss:nnnFNNN
unsigned long  MTT_GetMediaTimeMilliseconds(std::string s) {
	int n;
	unsigned long millisecond=0;
	char c[64], *token;
	n = s.find('T');
	if(n==0){	// TimePoint
		n=1;
		strncpy(c,s.data()+n,2);
		c[2] = 0;
		millisecond+=1000*atoi(c)*3600;
		n+=3;
		strncpy(c,s.data()+n,2);
		millisecond+=1000*atoi(c)*60;
		n+=3;
		strncpy(c,s.data()+n,2);
		millisecond+=1000*atoi(c);
		n+=3;
		strcpy(c,s.data()+n);
		token = strtok(c,"F");
		if(token)
		{
			double N=atof(token);
			token = strtok(NULL," ");
			double F=atof(token);
			millisecond+=(unsigned int)(1000*N/F);
		}
	}
	return millisecond;
}

//----------------------------------------------------------------------------
// Extracts millisecond from string of the following format PnDTnHnMnSnNnF
// The days are not supported - used for short time durations
unsigned long  MTT_GetMediaDurationMilliseconds(std::string s) {
	int n;
	unsigned long millisecond=0;
	char c[64], *token;
	n = s.find('P');
	if(n==0){	// Duration
		strcpy(c,s.data());
		n = s.find('T');
		if(n>0){
			token = strtok(c,"T");
			n = s.find('H');
			if(n>0){
				token = strtok(NULL,"H");
				millisecond+=1000*atoi(token)*3600;
			}
			n = s.find('M');
			if(n>0){
				token = strtok(NULL,"M");
				millisecond+=1000*atoi(token)*60;
			}
			n = s.find('S');
			if(n>0){
				token = strtok(NULL,"S");
				millisecond+=1000*atoi(token);
			}
			n = s.find('N');
			if(n>0){
				token = strtok(NULL,"N");
				double N=atof(token);
				token = strtok(NULL,"F");
				double F=atof(token);
				millisecond+=(unsigned int)(1000*N/F);
			}
		}
	}
	return millisecond;
}
