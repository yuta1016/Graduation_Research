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
// Applicable File Name:  media_time_tool.h

#ifndef __media_time_tool__
#define __media_time_tool__

#include <string>

unsigned long  MTT_GetMediaTimePointString(unsigned long ul,char* s);
unsigned long  MTT_GetMediaDurationString(unsigned long ul,char* s);
unsigned long  MTT_ExtractHours(unsigned long second);
unsigned long  MTT_ExtractMinutes(unsigned long second);
unsigned long  MTT_ExtractSeconds(unsigned long second);
unsigned long  MTT_ExtractMilliseconds(unsigned long second);
unsigned long  MTT_GetMediaTimeMilliseconds(std::string s);
unsigned long  MTT_GetMediaDurationMilliseconds(std::string s);

#endif // __media_time_tool__
