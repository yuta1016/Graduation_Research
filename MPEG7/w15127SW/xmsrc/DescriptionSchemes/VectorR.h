///////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// John R. Smith		- IBM T. J. Watson Research Center
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
// Copyright (c) 1998-2000.
//
// This notice must be included in all copies or derivative works.
//
// Applicable File Name:  VectorR.h
//

#ifndef INCLUDED_VECTORR_H
#define INCLUDED_VECTORR_H

#include <stdio.h>
#ifdef _VISUALC_
#include <fstream>
#define STREAMNS(stream) std::stream
#else
#include <fstream>
#define STREAMNS(stream) std::stream
#endif
#include "DescriptionSchemes/DescriptionSchemes.h"

#ifndef BOOLEAN
#define BOOLEAN int
#endif

namespace XM
{


//=============================================================================
class VectorRInterfaceABC: public I_InterfaceABC
{
public:
	VectorRInterfaceABC();
	virtual ~VectorRInterfaceABC() {}
	static const UUID myID;

};

//=============================================================================
class VectorRInterface: public VectorRInterfaceABC
{
public:
	VectorRInterface();
	virtual ~VectorRInterface();
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	static const UUID myID;
	static const char *myName;
};

//=============================================================================

class VectorR : public DescriptionScheme {

	public:

	static const UUID myID;
	static const char * myName;

protected:
  int d_length;
  int d_id;
  float *d_data_p;
  int d_own;

 public:
  VectorR() : d_length(0), d_id(0),d_data_p(0), d_own(1) {}

  virtual ~VectorR();
 
  VectorR(int len, int id = 0) : 
   d_length(len), d_id(id), d_data_p(new float[len]), d_own(1) {}
  
  VectorR(int len, const float *d, int id = 0);
  VectorR(int len, const int *d, int id = 0);

  VectorR(int len, float *d, int id = 0): d_length(len),
    d_id(id), d_data_p(d), d_own(0) {}

  VectorR(const VectorR& c);

  VectorR(const char* file);
  
  void hold(int len, float *d, int id = 0) {
	clean();
    d_length = len;
    d_data_p = d;
    d_own = 0;
    d_id = id;
  }    

  int save(const char *file, const char *mode = "ab") const;
  int save(FILE* fp) const;
  int get(const char *file, const char *mode = "rb");
  int get(FILE* fp, int old = 0);

  void read(const char* filename) {
    STREAMNS(ifstream) inf(filename);
    read(inf);
    inf.close();
  }

  void write(const char* filename) const {
    STREAMNS(ofstream) outf(filename);
    write(outf);
    outf.close();
  }

  STREAMNS(istream)& read(std::istream& in);
  STREAMNS(ostream)& write(std::ostream& out) const;

  int length() const { return d_length; }
  int Id() const { return d_id; }
  void setId(int id) { d_id = id;}

  void clear();

  virtual void clean() {
  if (d_data_p && d_own) delete [] d_data_p;
  d_own = 1;
}

  const float* data(int i = 0) const {
    return (i < d_length) ? d_data_p + i : d_data_p + (d_length-1);
  }

  float* data_nc(int i = 0) {
    return (i < d_length) ? d_data_p + i : d_data_p + (d_length-1);
  }

  VectorR& normalize(float norm = 1, float p = 2);
  VectorR& operator+=(const VectorR& b);
  VectorR& operator-=(const VectorR& b);
  VectorR& operator*=(const VectorR& b);
  VectorR& operator/=(const VectorR& b);

  VectorR& operator=(const VectorR& b);

  VectorR& operator*=(float d);
  VectorR& operator+=(float d);
  VectorR& operator/=(float d);

  int nonzeroterms() const;
  float sum() const;
  float norm(float p) const;
  VectorR& square_terms();
  VectorR& power(float expon);
  VectorR& abs();

  float mean() const;
  float variance() const;
  float entropy() const;
  
  float minimum() const;
  float maxnorm() const;
  float maximum() const;

  float min_index() const;
  float max_index() const;

  std::ostream& pretty_print(std::ostream& out) const;

  
	static VectorR threshold(const VectorR &A, float value);

	static VectorR sort(const VectorR &A, int top);


	static float hamming_distance(const VectorR& c1, const VectorR &c2, 
				int threshold = 100);

	static float vector_distance(const VectorR& c1, const VectorR &c2, int len, int norm=1);
	static float inner_product(const VectorR& c1, const VectorR &c2, int len);
	static float intersection(const VectorR& c1, const VectorR &c2, int len, int norm);
	static float mse(const VectorR& c1, const VectorR &c2);
	static float dB(const VectorR& c1, const VectorR &c2);

	static VectorR shift(const VectorR &H);

	static VectorR uniform_quantize(const VectorR& h, int delta = 1);
 
	static VectorR cumulative(const VectorR& h, float p =2 );

	static VectorR prepend(const VectorR& H, float term);

	static float vector_multiply(const VectorR& G, const VectorR& H, int size);

};

	//VectorR operator+(const VectorR& a, const VectorR& b);
	//VectorR operator-(const VectorR& a, const VectorR& b);


};

#endif // INCLUDED_VECTORR_H
