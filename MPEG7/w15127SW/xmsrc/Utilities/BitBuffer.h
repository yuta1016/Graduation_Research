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
// Applicable File Name:  BitBuffer.h

#ifndef __BIT_BUFFER__
#define __BIT_BUFFER__




namespace XM
{
  class  BitBuffer 

  {
  public:
    BitBuffer();
    virtual ~BitBuffer();

    // MPEG-7 encoder
    virtual void FlushBuffer(int fillzero);
    virtual int PutBit(char bit);
    virtual int PutBits(char *buffer,unsigned long bits);
    virtual int PutBitsLong(long value, unsigned long bits);

    // MPEG-7 decoder
    virtual int GetBit(int init);
    virtual int GetBits(char *buffer,unsigned long bits);
    virtual int GetBitsLong(long *vaule,unsigned long bits);
    virtual int TakeBits(unsigned long bits);
    virtual bool GetStatusEOF(void);
    virtual bool SetSilentEOF(int yes);
    virtual int SelectField(int field);

  };

};




#endif // __BIT_BUFFER__


