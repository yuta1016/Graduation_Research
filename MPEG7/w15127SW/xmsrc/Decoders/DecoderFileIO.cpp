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
// Copyright (c) 1998-1999.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  DecoderFileIO.cpp

#include <stdio.h>
#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#else
#include <io.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "DecoderFileIO.h"

#ifndef _VISUALC_
#define OPEN_FILE_FLAGS  O_RDONLY
#else
#define OPEN_FILE_FLAGS  O_RDONLY|O_BINARY
#endif

namespace XM
{

//------------------------------------------------------------------------
DecoderFileIO::DecoderFileIO(unsigned long buffersize)

{
  m_Buffer=new char [buffersize];
  if (!m_Buffer) {
    fprintf(stderr,"Error: Out of memory\n");
    exit (1);
  }
  m_BufferSize=buffersize;
  m_tmpBufferPosition=m_BufferPosition=m_BufferSize;
  m_EndOfBuffer=0;
  m_tmpBitPosition=m_BitPosition=0;
  *m_Buffer=0;
  m_EOF=1;
  m_ReportEOF=1;
}

//------------------------------------------------------------------------

DecoderFileIO::~DecoderFileIO()

{
  if (m_Buffer) delete [] m_Buffer;
}

//------------------------------------------------------------------------
int DecoderFileIO::OpenBitStream(char *filename)

{
  m_FileHandle=open(filename,OPEN_FILE_FLAGS);

  if (m_FileHandle == -1) {
    fprintf(stderr,"Error: Could not open File %s\n",filename);
    exit (1);
  }
  m_tmpBufferPosition=m_BufferPosition=m_BufferSize;
  m_EndOfBuffer=0;
  m_tmpBitPosition=m_BitPosition=0;
  m_EOF=0;
  return 0;
}

//------------------------------------------------------------------------
int DecoderFileIO::CloseBitStream()

{
  close(m_FileHandle);
  return 0;
}


//------------------------------------------------------------------------
void DecoderFileIO::Preload()

{
  unsigned long readbits;

  if (m_FileHandle == -1) {
    fprintf(stderr,"Error: No file connected to Buffer\n");
    exit (1);
  }

  /* input file empty*/
  if (m_EOF) return;

  /* Buffer still full*/
  if (!m_BufferPosition && m_EndOfBuffer) return;

  /* move rest of buffer to beginning of buffer*/
  m_tmpBufferPosition-=m_BufferPosition;
  {
    int i=0;
    while (m_BufferPosition < m_EndOfBuffer) {
      m_Buffer[i]=m_Buffer[m_BufferPosition];
      i++;
      m_BufferPosition++;
    }
    m_EndOfBuffer=i;
  }
  m_BufferPosition=0;

  /* buffer to full for further preload*/
  if (m_EndOfBuffer > (m_BufferSize>>1)) return;

  /* read from file*/
  readbits=read(m_FileHandle,&m_Buffer[m_EndOfBuffer],(m_BufferSize>>1));
  if (readbits != (m_BufferSize>>1)) m_EOF=1;
  m_EndOfBuffer+=readbits;
}


//------------------------------------------------------------------------
int DecoderFileIO::GetBit(int init)

{
  char *a;
  int result;

  if (init) {
    m_tmpBufferPosition=m_BufferPosition;
    m_tmpBitPosition=m_BitPosition;
  }

  if (m_EndOfBuffer<=m_tmpBufferPosition) {
    if (m_EOF && m_ReportEOF) {
      fprintf(stderr,"Warning: Unexpected end of stream\n");
    }
    Preload();
  }

  a=m_Buffer+m_tmpBufferPosition;

  result= ((*a) << (m_tmpBitPosition)) & 128;

  if ((m_tmpBitPosition & 7) == 7) {
    m_tmpBufferPosition++;
    m_tmpBitPosition=0;
  }
  else {
    m_tmpBitPosition++;
  }

  return result;
}

//------------------------------------------------------------------------
int DecoderFileIO::GetBits(char *buffer,unsigned long bits)

{
  int bitpos=0;

  /* go again to first untaken bits in stream*/
  m_tmpBufferPosition=m_BufferPosition;
  m_tmpBitPosition=m_BitPosition;


  /* reading from lsb -> msb*/
  /* first msb's of old byte*/
  *buffer=0;
  if(bits==0) return(0);
  while (1) {
    if (GetBit(0)) {
      *buffer+=(128>>bitpos);
    }

    bits--;
    /* check if last bit was read*/
    if (bits==0) break;
    if ((bitpos & 7) == 7) {
      buffer++;
      *buffer=0;
      bitpos=0;
    }
    else {
      bitpos++;
    }
  }

  return 0;
}

//------------------------------------------------------------------------
int DecoderFileIO::GetBitsLong(long *value, unsigned long bits)

{
  /* go again to first untaken bits in stream*/
  m_tmpBufferPosition=m_BufferPosition;
  m_tmpBitPosition=m_BitPosition;


  /* reading from lsb -> msb*/
  /* first msb's of old byte*/
  *value=0;

  // If no bits requested then do nothing
  if(bits==0) return(0);

  while (1) {
    if (GetBit(0)) {
      *value+=1;
    }

    bits--;
    /* check if last bit was read*/
    if (bits==0) break;
    *value<<=1;
  }

  return 0;
}

//------------------------------------------------------------------------
int DecoderFileIO::TakeBits(unsigned long bits)

{
  unsigned long newbitpos,newbufferpos,
    bytes;

  /* increment bitpos and bytpos*/
  newbitpos=m_BitPosition+bits;
  bytes=newbitpos/8;
  newbitpos-=bytes*8;
  newbufferpos=m_BufferPosition+bytes;

  if (newbufferpos >= m_EndOfBuffer) {

    /* prelod next part of stream*/
    Preload();
    newbufferpos=m_BufferPosition+bytes;
    if ((newbufferpos > m_EndOfBuffer) ||
	((newbufferpos == m_EndOfBuffer) && newbitpos)) {
      /* read exceeds file*/
      if (m_ReportEOF) {
	fprintf(stderr,"To many bits taken from data stream\n");
      }
      m_EOF=2;
      return -1;
    }
  }
  m_tmpBufferPosition=m_BufferPosition=newbufferpos;
  m_tmpBitPosition=m_BitPosition=newbitpos;
  return 0;
}

//------------------------------------------------------------------------
bool DecoderFileIO::GetStatusEOF(void)
{
  if(m_EOF == 2) return true;
  else return false;
}

//------------------------------------------------------------------------
bool DecoderFileIO::SetSilentEOF(int yes)
{
  if (yes) m_ReportEOF=0;
  else m_ReportEOF=1;

  return m_ReportEOF;
}

} /* namespace*/

