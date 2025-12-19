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
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2003.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DecoderFileIO.h
#ifndef __DecoderFileIO__
#define __DecoderFileIO__

#include "Utilities/BitBuffer.h"

namespace XM
{

  class  DecoderFileIO:public BitBuffer

  {
  public:
    DecoderFileIO(unsigned long buffersize);
    ~DecoderFileIO();

    int OpenBitStream(char *filename);
    int CloseBitStream();
    void Preload();
    int GetBit(int init);
    int GetBits(char *buffer,unsigned long bits);
    int GetBitsLong(long *vaule,unsigned long bits);
    int TakeBits(unsigned long bits);
    bool GetStatusEOF(void);
    bool SetSilentEOF(int yes);

  private:
    char *m_Buffer;
    unsigned long m_BufferSize;
    int  m_FileHandle;
    unsigned long m_BufferPosition;
    unsigned long m_tmpBufferPosition;
    unsigned long m_EndOfBuffer;
    int m_BitPosition;
    int m_tmpBitPosition;
    int m_EOF;
    int m_ReportEOF;
  };

};
#endif /* __DecoderFileIO__ */
