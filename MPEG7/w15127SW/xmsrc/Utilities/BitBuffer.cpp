///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// TU-Munich, S.Herrmann
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
// purpose, assign or donate the code to a third party and inhibit
// third parties from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  BitBuffer.cpp

#include <stdio.h>
#include "Utilities/BitBuffer.h"

namespace XM
{

//------------------------------------------------------------------------
BitBuffer::BitBuffer()

{
}

//------------------------------------------------------------------------
BitBuffer::~BitBuffer()

{
}

// MPEG-7 encoder
//------------------------------------------------------------------------
void BitBuffer::FlushBuffer(int fillzero)

{
  fprintf(stderr,"FlushBuffer not implemented\n");
}

//------------------------------------------------------------------------
int BitBuffer::PutBit(char bit)

{
  fprintf(stderr,"PutBit not implemented\n");
  return -1;
}


// MPEG-7 decoder
//------------------------------------------------------------------------
int BitBuffer::PutBits(char *buffer,unsigned long bits)

{
  fprintf(stderr,"PutBits not implemented\n");
  return -1;
}

//------------------------------------------------------------------------
int BitBuffer::PutBitsLong(long value, unsigned long bits)

{
  fprintf(stderr,"PutBitsLong not implemented\n");
  return -1;
}

//------------------------------------------------------------------------
int BitBuffer::GetBit(int init)

{
  fprintf(stderr,"GetBit not implemented\n");
  return 0;
}

//------------------------------------------------------------------------
int BitBuffer::GetBits(char *buffer,unsigned long bits)

{
  fprintf(stderr,"GetBits not implemented\n");
  return 0;
}

//------------------------------------------------------------------------
int BitBuffer::GetBitsLong(long *vaule,unsigned long bits)

{
  fprintf(stderr,"GetBitsLong not implemented\n");
  return 0;
}

//------------------------------------------------------------------------
int BitBuffer::TakeBits(unsigned long bits)

{
  fprintf(stderr,"TakeBits not implemented\n");
  return -1;
}

//------------------------------------------------------------------------
bool BitBuffer::GetStatusEOF(void)

{
  fprintf(stderr,"GetStatusEOF not implemented\n");
  return 0;
}

//------------------------------------------------------------------------
bool BitBuffer::SetSilentEOF(int yes)

{
  fprintf(stderr,"SetSilentEOF not implemented\n");
  return -1;
}

//------------------------------------------------------------------------
int BitBuffer::SelectField(int fieldno)

{
  //  fprintf(stderr,"SelectField not implemented\n");
  return -1;
}

} // end namespace
