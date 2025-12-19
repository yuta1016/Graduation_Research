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
// UUIDs.h

#ifndef __UUID_H__
#define __UUID_H__
//=============================================================================
// UUIDs are guaranteed unique numbers which are fromed from a combination
// of a time stamp and a network card ID

namespace XM
{

class UUID
{
public:
	// the following constructor uses the string format
	// "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"

	UUID();

	UUID(const char uniqueString[37]);

	UUID(
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
		unsigned char Data4_7);
	
	virtual ~UUID();

	const UUID& operator =(const UUID& other);

	int operator==(const UUID& other) const;

private:

	unsigned long m_Data1;
	unsigned short m_Data2;
	unsigned short m_Data3;
	unsigned char m_Data4[12];
};
}




#endif //__UUID_H__
