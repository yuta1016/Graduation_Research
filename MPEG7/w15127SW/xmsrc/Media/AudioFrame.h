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
// AudioFrame.h: Schnittstelle für die Klasse CAudioFrame
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIOFRAME_H__0477D143_01A1_11D3_998A_0080C79515BF__INCLUDED_)
#define AFX_AUDIOFRAME_H__0477D143_01A1_11D3_998A_0080C79515BF__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//===========================================================================
class CAudioFrame  
{
public:
	CAudioFrame(void);
	CAudioFrame(unsigned int len, unsigned int nchan, double srate);
	CAudioFrame(const CAudioFrame&);	// Copy-Constructor		
	const CAudioFrame& operator= (const CAudioFrame&);
	virtual ~CAudioFrame();
	unsigned int	GetAudioFrameLen();
	void			SetAudioFrameLen(unsigned int len);
	unsigned int	GetAudioHopsize();
	void			SetAudioHopsize(unsigned int len);
	unsigned int	GetNumOfAudioChannels();
	void			SetNumOfAudioChannels(unsigned int nchan);
	long			GetTimeStamp(void);
	void			SetTimeStamp(long);
	double			GetSampleRate(void);
	void			SetSampleRate(double);
	void			GetAudioCharBuf(char *) const;
	float *			GetAudioBuf(void) const			{return m_audiobuf;}
	float			GetAudioBufIndex(int i) const	{return m_audiobuf[i];}
	void			SetAudioBufIndex(int i, float w)	{m_audiobuf[i] = w;}
	void			SetAudioCharBuf(char *);
	float *			GetAudioFrame();
	void			SetAudioBuf(float *audiobuf);

private:
	unsigned int m_len;
	unsigned int m_nchan;	
	unsigned int m_hopsize;
	long		 m_framenumber;
	double		 m_srate;
	float *		 m_audiobuf;	 
};


//===========================================================================
class AudioFrameError 
{
private:
	AudioFrameError( ) { }
public:
	AudioFrameError( const char * msg ) { m_msg = msg;}
	const char * m_msg;
};




#endif // !defined(AFX_AUDIOFRAME_H__0477D143_01A1_11D3_998A_0080C79515BF__INCLUDED_)
