///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Intel Corporation
// (contributing organizations names)
//
// in the course of development of the MPEG-7 Experimentation Model.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Model tools as specified by the MPEG-7 Requirements.
//
// ISO/IEC gives users of MPEG-7 free license to this software module or
// modifications thereof for use in hardware or software products claiming
// conformance to MPEG-7.
//
// Those intending to use this software module in hardware or software products
// are advised that its use may infringe existing patents. The original
// developer of this software module and his/her company, the subsequent
// editors and their companies, and ISO/IEC have no liability for use of this
// software module or modifications thereof in an implementation.
//
// Copyright is not released for non MPEG-7 conforming products. The
// organizations named above retain full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// UUIDs.cpp

#include "../Utilities/UUIDs.h"                    /* Naming Convention */
#include "string.h"
#include "stdlib.h"

using namespace XM;

UUID::UUID()
{
	m_Data1 = 0;
	m_Data2 = 0;
    m_Data3 = 0;
	for (int i=0; i < 8; i++)
		m_Data4[i] = 0;

};

UUID::UUID(const char uniqueString[37])
{
	char tempstring[9];
	char *endptr;
	
	m_Data1 = 0;

	{
	int i;
	for (i=0; i < 4; i++)
	{
		strncpy(tempstring, &uniqueString[i*2],2);
		tempstring[2] = 0x00;

		m_Data1 += (strtol(tempstring, &endptr, 16) << ((3-i)*8));	
	}
	}
	
	
	strncpy(tempstring, &uniqueString[9],4);
	tempstring[4] = 0x00;
	m_Data2 = (unsigned short) strtol(tempstring, &endptr, 16);

	strncpy(tempstring, &uniqueString[14],4);
	tempstring[4] = 0x00;
	m_Data3 = (unsigned short) strtol(tempstring, &endptr, 16);

	strncpy(tempstring, &uniqueString[19],1);
	tempstring[1] = 0x00;
	m_Data4[0] = (unsigned char) strtol(tempstring, &endptr, 16);

	strncpy(tempstring, &uniqueString[20],1);
	tempstring[1] = 0x00;
	m_Data4[1] = (unsigned char) strtol(tempstring, &endptr, 16);

	strncpy(tempstring, &uniqueString[21],1);
	tempstring[1] = 0x00;
	m_Data4[2] = (unsigned char) strtol(tempstring, &endptr, 16);

	strncpy(tempstring, &uniqueString[22],1);
	tempstring[1] = 0x00;
	m_Data4[3] = (unsigned char) strtol(tempstring, &endptr, 16);

	{
	int i;
	for (i=4; i < 12; i++)
	{
		tempstring[1] = 0x00;
		strncpy(tempstring, &uniqueString[24-4+i],1);
		m_Data4[i] = (unsigned char) strtol(tempstring, &endptr, 16);
	}
	}

}

UUID::UUID(
	unsigned int Data1,
	unsigned short Data2,
	unsigned short Data3,
	unsigned char Data4_0,
	unsigned char Data4_1,
	unsigned char Data4_2,
	unsigned char Data4_3,
	unsigned char Data4_4,
	unsigned char Data4_5,
	unsigned char Data4_6,
	unsigned char Data4_7)
{
	m_Data1 = Data1;
	m_Data2 = Data2;
	m_Data3 = Data3;

	m_Data4[0] = Data4_0;
	m_Data4[1] = Data4_1;
	m_Data4[2] = Data4_2;
	m_Data4[3] = Data4_3;
	m_Data4[4] = Data4_4;
	m_Data4[5] = Data4_5;
	m_Data4[6] = Data4_6;
	m_Data4[7] = Data4_7;

}

UUID::~UUID() 
{
};

const UUID& UUID::operator =(const UUID& other)
{
	if (this == &other)
		return *this;

	m_Data1 = other.m_Data1;
	m_Data2 = other.m_Data2;
	m_Data3 = other.m_Data3;

	m_Data4[0] = other.m_Data4[0];
	m_Data4[1] = other.m_Data4[1];
	m_Data4[2] = other.m_Data4[2];
	m_Data4[3] = other.m_Data4[3];
	m_Data4[4] = other.m_Data4[4];
	m_Data4[5] = other.m_Data4[5];
	m_Data4[6] = other.m_Data4[6];
	m_Data4[7] = other.m_Data4[7];

	return *this;

}


int UUID::operator==(const UUID& other) const
{
	return m_Data1 == other.m_Data1 &&
		m_Data2 == other.m_Data2 &&
		m_Data3 == other.m_Data3 &&
		m_Data4[0] == other.m_Data4[0] &&
		m_Data4[1] == other.m_Data4[1] &&
		m_Data4[2] == other.m_Data4[2] &&
		m_Data4[3] == other.m_Data4[3] &&
		m_Data4[4] == other.m_Data4[4] &&
		m_Data4[5] == other.m_Data4[5] &&
		m_Data4[6] == other.m_Data4[6] &&
		m_Data4[7] == other.m_Data4[7];

}

