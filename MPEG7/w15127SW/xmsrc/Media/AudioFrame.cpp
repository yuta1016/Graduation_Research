///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Bernhard Feiten, Deutsche Telekom Berkom, 15.05.99, 12.03.01
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
// AudioFrame.cpp: Implementierung der Klasse CAudioFrame.
//#include "stdafx.h"

#include <memory.h>
#include "AudioFrame.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

//========================================================================
CAudioFrame::CAudioFrame(void)
{
	m_len = 1024;
	m_nchan = 1;
	m_srate = 22050;
	m_framenumber = 0;
	m_audiobuf = new float[m_len * m_nchan];
	for (unsigned int i=0;i<m_len*m_nchan;i++) this->SetAudioBufIndex(i,0);
}
 
//--------------------------------------------------------------------------
CAudioFrame::CAudioFrame(unsigned int len, unsigned int nchan, double srate)
{
	if (len < 1) {throw AudioFrameError("Framelength must be >= 1");} 
	if ((nchan < 1) || (nchan > 16)) {throw AudioFrameError("Number of Audiochannels must be in the range from 1 to 16");} // Fehlerbhandlung
	m_len = len;
	m_nchan = nchan;
	m_srate = srate;
	m_framenumber= 0;
	m_audiobuf = new float [m_len * m_nchan];
	for (unsigned int i=0;i<m_len*m_nchan;i++) this->SetAudioBufIndex(i,0);
}

//--------------------------------------------------------------------------
CAudioFrame::CAudioFrame(const CAudioFrame& f)
{
	unsigned int i;
	m_len = f.m_len;
	m_nchan = f.m_nchan;
	m_srate = f.m_srate;
	m_framenumber = f.m_framenumber;
	m_hopsize = f.m_hopsize;
	m_audiobuf = new float[m_len * m_nchan];
	for (i=0;i<m_len * m_nchan;i++)
		m_audiobuf[i] = f.m_audiobuf[i];
}

//--------------------------------------------------------------------------
CAudioFrame::~CAudioFrame()
{
	delete[] m_audiobuf;
}

//--------------------------------------------------------------------------
const CAudioFrame& CAudioFrame::operator= (const CAudioFrame& f)
{
	unsigned int i;

	if (this == &f) {
		return *this;
	}

	if ((m_len != f.m_len) || (m_nchan != f.m_nchan))
	{ 
		delete [] m_audiobuf;
		m_len = f.m_len;
		m_nchan = f.m_nchan;
		m_srate = f.m_srate;
		m_framenumber = f.m_framenumber;
		m_hopsize = f.m_hopsize;
		m_audiobuf = new float[m_len * m_nchan];
		for (i=0;i<m_len * m_nchan;i++)
			m_audiobuf[i] = f.m_audiobuf[i];
		
		return *this; 
	}
	else 
	{
		m_srate = f.m_srate;
		m_framenumber = f.m_framenumber;
		m_hopsize = f.m_hopsize;
		for (i=0;i<m_len * m_nchan;i++)
			m_audiobuf[i] = f.m_audiobuf[i];
		return *this;
	}
}

//--------------------------------------------------------------------------
float * CAudioFrame::GetAudioFrame()
{
	return m_audiobuf;
}

//--------------------------------------------------------------------------
unsigned int CAudioFrame::GetAudioFrameLen()
{
	return m_len;
}

//--------------------------------------------------------------------------
void CAudioFrame::SetAudioFrameLen(unsigned int len)
{
	if (m_len == len) return;
	if (len > m_len) {
		delete[] m_audiobuf;
		m_audiobuf = new float[m_len * m_nchan];
	}
	m_len = len;
}

//--------------------------------------------------------------------------
unsigned int CAudioFrame::GetAudioHopsize()
{
	return m_hopsize;
}

//--------------------------------------------------------------------------
void CAudioFrame::SetAudioHopsize(unsigned int hopsize)
{
	m_hopsize = hopsize;
}

//--------------------------------------------------------------------------
unsigned int CAudioFrame::GetNumOfAudioChannels()
{
	return m_nchan;	
}

//--------------------------------------------------------------------------
void CAudioFrame::SetNumOfAudioChannels(unsigned int nchan)
{
	if (m_nchan == nchan) return;
	delete[] m_audiobuf;
	m_nchan = nchan;
	m_audiobuf = new float[m_len * m_nchan];
}

//--------------------------------------------------------------------------
void CAudioFrame::SetTimeStamp(long framenumber)
{
	m_framenumber = framenumber;
}

//--------------------------------------------------------------------------
long CAudioFrame::GetTimeStamp(void)
{
	return m_framenumber;
}

//--------------------------------------------------------------------------
void CAudioFrame::SetSampleRate(double srate)
{
	m_srate = srate;
}

//--------------------------------------------------------------------------
double CAudioFrame::GetSampleRate(void)
{
	return m_srate;
}

//--------------------------------------------------------------------------
void CAudioFrame::SetAudioCharBuf(char *audio_buf)
{
	for( unsigned int ui = 0; ui < m_len * m_nchan; ++ui )
		m_audiobuf[ui] = (float) audio_buf[ui];
}

//--------------------------------------------------------------------------
void CAudioFrame::SetAudioBuf(float *audiobuf)
{
	memcpy(m_audiobuf, audiobuf, m_len * m_nchan * (sizeof(float) / sizeof(char)));
}

//--------------------------------------------------------------------------
void	CAudioFrame::GetAudioCharBuf(char *char_audio_buf) const
{
	for( unsigned int ui = 0; ui < m_len; ++ui )
		char_audio_buf[ui] = (char) m_audiobuf[ui];
}
	

