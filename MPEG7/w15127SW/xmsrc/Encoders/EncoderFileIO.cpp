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
// Applicable File Name:  EncoderFileIO.cpp

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

#include "EncoderFileIO.h"

#ifndef _VISUALC_
#define CREATE_FILE_FLAGS O_RDWR|O_CREAT|O_TRUNC
#define OPEN_FILE_FLAGS  O_RDWR
#else
#define CREATE_FILE_FLAGS O_RDWR|O_CREAT|O_TRUNC|O_BINARY
#define OPEN_FILE_FLAGS  O_RDWR|O_BINARY
#endif


#ifdef UNIX
#define PERMISSIONS S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH
#else
#define PERMISSIONS S_IREAD|S_IWRITE
#endif

namespace XM
{

//------------------------------------------------------------------------
EncoderFileIO::EncoderFileIO(unsigned long buffersize)

{
  m_Buffer=new char [buffersize];
  if (!m_Buffer) {
    fprintf(stderr,"Error: Out of memory\n");
    exit (1);
  }
  m_BufferSize=buffersize;
  m_BufferPosition=0L;
  m_BitPosition=0;
  *m_Buffer=0;
}

//------------------------------------------------------------------------

EncoderFileIO::~EncoderFileIO()

{
  if (m_Buffer) {
    if (m_BufferPosition || m_BitPosition) FlushBuffer(1);
    delete [] m_Buffer;
  }
}

//------------------------------------------------------------------------
int EncoderFileIO::StartBitStream(char *filename)

{
  m_FileHandle=open(filename,CREATE_FILE_FLAGS,PERMISSIONS);

  if (m_FileHandle == -1) {
    fprintf(stderr,"Error: Could not open File %s\n",filename);
    exit (1);
  }
  return 0;
}

//------------------------------------------------------------------------
int EncoderFileIO::AppendBitStream(char *filename)

{
  m_FileHandle=open(filename,OPEN_FILE_FLAGS);

  if (m_FileHandle == -1) {

    /* if open fail try to create file*/
    m_FileHandle=open(filename,CREATE_FILE_FLAGS,PERMISSIONS);
    if (m_FileHandle == -1) {
      fprintf(stderr,"Error: Could not append to File %s\n",filename);
      exit (1);
    }
  }

  lseek(m_FileHandle,0,SEEK_END);
  return 0;
}

//------------------------------------------------------------------------
void EncoderFileIO::FlushBuffer(int fillzero)

{
  if (m_FileHandle == -1) {
    fprintf(stderr,"Error: No file connected to Buffer\n");
    exit (1);
  }

  if (!m_BufferPosition && !m_BitPosition) return;

  if (fillzero && m_BitPosition) {
    m_BufferPosition++;
    m_BitPosition=0;
  }

  if (m_BufferPosition) {
    if (m_BufferPosition !=  write(m_FileHandle,m_Buffer,m_BufferPosition)) {
      fprintf(stderr,"Error: could not write buffer to file \n");
      exit (1);
    }
  }
  m_BufferPosition=0;
  *m_Buffer=0;
}

//------------------------------------------------------------------------
int EncoderFileIO::PutBit(char bit)

{
  char *a;
  
  if (m_BufferPosition>=m_BufferSize) {
    FlushBuffer(0);
    a=m_Buffer;
  }

  a=m_Buffer+m_BufferPosition;
/*  if (!m_BitPosition) *a=0;*/

  /* filling from msb -> lsb*/
  if (bit) *a+=1<<(7-m_BitPosition);

  m_BitPosition++;
  if (m_BitPosition == 8) {
    m_BitPosition=0;
    m_BufferPosition++;
    a++;
    if (m_BufferPosition<m_BufferSize) {
      *a=0;
    }
  }

  return 0;
}

//------------------------------------------------------------------------
int EncoderFileIO::PutBits(char *buffer,unsigned long bits)

{
  unsigned long bitpos=0;
  char a;
  /* filling from msb -> lsb*/

  a=*buffer;
  while (bitpos<bits) {
    if (a & 0x80) {
      PutBit(1);
    }
    else {
      PutBit(0);
    }
    bitpos++;
    if (!(bitpos&7)) {
      buffer++;
      a=*buffer;
    }
    else {
      a<<=1;
    }
  }
  return 0;
}

//------------------------------------------------------------------------
int EncoderFileIO::PutBitsLong(long value,unsigned long bits)

{
  /* filling from msb -> lsb*/
  /* first msb's of old byte*/
  value<<=(32-bits);
  while (bits>0) {
    if (value & 0x80000000) {
      PutBit(1);
    }
    else {
      PutBit(0);
    }
    value<<=1;
    bits--;
  }
  return 0;
}

//------------------------------------------------------------------------
int EncoderFileIO::CloseBitStream()

{
  FlushBuffer(1);
  close(m_FileHandle);
  m_FileHandle=-1;
  return 0;
}


} /* namespace*/
