///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Mitsubishi Electric R&D Centre Europe
// P. Brasnett, W. Price
//
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
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
// purpose, assign or donate the code to a third party and inhibit third parties// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2015.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  ImageSignatureSearch.h
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __IMAGESIGNATURESEARCH_H__
#define __IMAGESIGNATURESEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "Descriptors/Descriptors.h"

//=============================================================================
static int costTable[256] =
{
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

namespace XM
{


// Forward Declarations:
class ImageSignatureSearchTool;

//=============================================================================
class ImageSignatureSearchInterfaceABC: public I_SearchInterfaceABC
{
public:
	ImageSignatureSearchInterfaceABC();
	virtual ~ImageSignatureSearchInterfaceABC() {};

	virtual void destroy() = 0;

	virtual int SetRefDescriptorInterface
	  (ImageSignatureDescriptorInterfaceABC
	   *aImageSignatureDescriptorInterface) = 0;
	virtual int SetQueryDescriptorInterface
	  (ImageSignatureDescriptorInterfaceABC
	   *aImageSignatureDescriptorInterface) = 0;
	virtual double GetDistance(void) = 0;

	static const UUID myID;
};

//=============================================================================
class ImageSignatureSearchInterface: 
  public ImageSignatureSearchInterfaceABC
{
public:
	ImageSignatureSearchInterface(ImageSignatureSearchTool* aTool);
	virtual ~ImageSignatureSearchInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual void destroy();

	virtual int SetRefDescriptorInterface
	  (ImageSignatureDescriptorInterfaceABC
	   *aImageSignatureDescriptorInterface);
	virtual int SetQueryDescriptorInterface
	  (ImageSignatureDescriptorInterfaceABC
	   *aImageSignatureDescriptorInterface);
	virtual double GetDistance(void);


	static const UUID myID;
	static const char *myName;

private:
	ImageSignatureSearchTool *m_SearchTool;
};

//=============================================================================
class ImageSignatureSearchTool: public Search
{
friend class ImageSignatureSearchInterface;
public:
	ImageSignatureSearchTool();
	ImageSignatureSearchTool(ImageSignatureDescriptorInterfaceABC
                               *aQueryDescriptorInterface);

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual ImageSignatureSearchInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetRefDescriptorInterface
	  (ImageSignatureDescriptorInterfaceABC
	   *aImageSignatureDescriptorInterface);
	virtual int SetQueryDescriptorInterface
	  (ImageSignatureDescriptorInterfaceABC
	   *aImageSignatureDescriptorInterface);
	virtual double GetDistance(void);

	static const UUID myID;
	static const char * myName;
private:
	virtual ~ImageSignatureSearchTool();

	ImageSignatureSearchInterface m_Interface;
	ImageSignatureDescriptorInterfaceABC *m_RefDescriptorInterface;
	ImageSignatureDescriptorInterfaceABC *m_QueryDescriptorInterface;

};

//================================================================================
class FeatureSignature
{
private:

  unsigned char x_, y_, angle_;
  ImageSignatureFeatureBits bits_;
  ImageSignatureFeatureHash hash_;

public:

  FeatureSignature(unsigned char loc_x, unsigned char loc_y, unsigned char angle, ImageSignatureFeatureBits bits, unsigned char *hash_mask);
  FeatureSignature();

  void print();

  unsigned char getX(void) const					{ return x_; }
  unsigned char getY(void) const					{ return y_; }
  unsigned char getAngle(void) const				{ return angle_; }
  const ImageSignatureFeatureBits &getBits(void) const		{ return bits_; }
  ImageSignatureFeatureBits *getBitsPt(void)				{return &bits_;}

  bool near_border(unsigned char width, unsigned char height, int distance) const
  {
    return ((x_ < distance) || (y_ < distance) || (x_ > (width-distance)) || (y_ > (height-distance)));
  }

  inline int distance(const FeatureSignature &fp2) const
  {
    int cost=0;

    for(int i = 0; i < (IMAGESIGNATURE_BYTES_FEATURE); i++)
    {
      cost += costTable[(bits_[i]^fp2.bits_[i])];
    }

    return cost;
  }

  inline int distance(const FeatureSignature &fp2, unsigned char *mask) const
  {
    int cost=0;
    for(int i = 0; i < IMAGESIGNATURE_BYTES_FEATURE; i++)
      cost += costTable[(bits_[i]^fp2.bits_[i])&mask[i]];
    return cost;
  }

  inline int hash_distance( const FeatureSignature &fp2 ) const
  {
    int cost=0;
    for(int i = 0; i < IMAGESIGNATURE_BYTES_HASH_2; i++)
      cost += costTable[(hash_[i]^fp2.hash_[i])];
    return cost;
  }

  const ImageSignatureFeatureHash &getHash(void) const			{ return hash_; }
  ImageSignatureFeatureHash *getHashPt(void)			{return &hash_;}

  friend std::ostream &operator<<(std::ostream &os, const FeatureSignature &fs)
  {
    os << (int)fs.x_ << "," << (int)fs.y_ << "," << (int)fs.angle_;
    return os;
  }
};

class Pair
{
private:

  FeatureSignature a_;
  FeatureSignature b_;
  int distance_;
  int idxa_;
  int idxb_;

public:
  Pair(FeatureSignature a, FeatureSignature b, int distance, int idxa, int idxb)
  {
    a_=a;
    b_=b;
    idxa_=idxa;
    idxb_=idxb;
    distance_=distance;
  }	
  Pair()
  {
    FeatureSignature a;
    a_=a;
    b_=a;
    idxa_=0;
    idxb_=0;
    distance_=0;
  }	
  int getDistance( void )  
  { 
    return distance_; 
  }

  int get_idxa()
  {
    return idxa_;
  }

  int get_idxb()
  {
    return idxb_;
  }

  bool operator<(const Pair p2) const
  {
    return (distance_ < p2.distance_ ) ? true : false;
  }

  bool operator>(const Pair p2) const
  {
    return (distance_ > p2.distance_ ) ? true : false;
  }

  unsigned char get_a_x()
  {
    return a_.getX();
  }

  unsigned char get_a_y()
  {
    return a_.getY();
  }

  unsigned char get_b_x()
  {
    return b_.getX();
  }

  unsigned char get_b_y()
  {
    return b_.getY();
  }

  unsigned char get_a_angle()
  {
    return a_.getAngle();
  }

  unsigned char get_b_angle()
  {
    return b_.getAngle();
  }

  int get_angle_distance()
  {
    return abs(a_.getAngle()-b_.getAngle());
  }

};

typedef std::vector<FeatureSignature> FeatureSignatures;

};


#endif //__IMAGESIGNATURESEARCH_H__
