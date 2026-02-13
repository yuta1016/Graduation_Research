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
// Applicable File Name:  VectorR.cpp
//

#include "DescriptionSchemes/VectorR.h"

#include <math.h>
#include <string.h>

typedef unsigned char uc;


static const float XM_MAXFLOAT = 999999;
static const float MINFLOAT = -999999;

template<class T>
inline T MIN(T a, T b) { return (a<b) ? a : b; }

template<class T>
inline T MAX(T a, T b) { return (a>b) ? a : b; }

template<class T>
inline T ABS(T a) { return (a>0) ? a : -a; }

template<class T>
inline T SQR(T a) { return a*a; }


//=============================================================================



using namespace XM;
using namespace std;

const UUID VectorRInterface::myID = UUID("");
const char *VectorRInterface::myName = "VectorRInterface";

const UUID VectorR::myID = UUID("");
const char *VectorR::myName = "VectorR";

//const UUID VectorR::valID = UUID("");
//const char *VectorR::valName = "VectorR";

const UUID VectorRInterfaceABC::myID = UUID();

//sth function delimiters are missing
VectorR::~VectorR() {
  if (d_data_p && d_own) delete [] d_data_p;
}

VectorR::VectorR( const char* file )  :
d_length(0),
d_data_p(0),
d_own(1)
{
  ifstream inf(file);
  read(inf);
  inf.close();
}
  
istream& VectorR::read(istream& in) {
  in >> d_id;
  int len;
  in >> len;
  if (len>d_length) {
	  delete [] d_data_p;
	  d_data_p = new float[len];
	  d_length = len;
	  }
  if (len<=d_length) {
    int c = 0;
    float d;
    while (c<d_length && in >> d ) d_data_p[c++] = d;
  }
  return in;
}
  
ostream& VectorR::write(ostream& out) const {
  const float *g = data();
  out << Id() << " " << length() << " ";
  for (int i = 0; i < length(); i++, g++) out << (*g) << " ";
  return out;
} 

int VectorR::save(const char *file, const char *mode) const {
  FILE *fp = fopen(file, mode);
  save(fp);
  return fclose(fp);
}

int VectorR::save(FILE* fp) const {
  fwrite (&d_id, sizeof d_id, 1, fp);
  fwrite (&d_length, sizeof d_length, 1, fp);
  return fwrite(d_data_p, sizeof *d_data_p, d_length, fp);
}

int VectorR::get(const char *file, const char *mode) {
  FILE *fp = fopen(file, mode);
  get(fp);
  return fclose(fp);
}

int VectorR::get(FILE* fp, int old) {
  fread (&d_id, sizeof d_id, 1, fp);
  if (old) {
    int l = 0;
    while (l!=d_length && fread (&l, sizeof l, 1, fp)) {}
  }
  else {
    fread (&d_length, sizeof d_length, 1, fp);
    clean();
    d_data_p = new float[d_length];
  }
  return fread(d_data_p, sizeof *d_data_p, d_length, fp);
}

VectorR::VectorR(int len, const float* data, int id) : 
  d_length(len), d_id(id), d_data_p(new float[len]), d_own(1) {
    if (data) memcpy(d_data_p, data, d_length*sizeof *d_data_p);
    else clear();
}

VectorR::VectorR(int len, const int *data, int id) : 
  d_length(len), d_id(id), d_data_p(new float[len]), d_own(1)  {
    if (data) {
      float *g = d_data_p;
      const int *h = data;
      for (int i = 0; i < d_length; i++, g++, h++) *g = (float)*h;
    }
    else clear();
}

void VectorR::clear() {
  memset(d_data_p, 0x0, d_length*sizeof *d_data_p);
}

VectorR::VectorR(const VectorR& c) : d_length(c.length()), d_id(c.Id()),
  d_data_p(new float[c.length()]), d_own(1) {
    memcpy(d_data_p, c.data(), d_length*sizeof *d_data_p);
}

ostream& VectorR::pretty_print(ostream& out) const {  
  return write(out) << endl;
}
  
float VectorR::dB(const VectorR& c1, const VectorR &c2) {
  float tmse = sqrt(mse(c1, c2));
  return (tmse>0) ? -10 * log10 (tmse / (255*255)) : 0;
}

float VectorR::mse(const VectorR& c1, const VectorR &c2) {
  const float *g = c1.data(), *h = c2.data();
  VectorR z(c1);
  z-=c2;
  double d = z.square_terms().sum();
  //cerr << "mse " << c1.Id() << " = " << d << endl; 
  return d;
}

float VectorR::inner_product(const VectorR& c1, const VectorR &c2, int len) {
  const float *g = c1.data(), *h = c2.data();
  float d = 0;
  for (int i = 0; i < len; i++, g++, h++) d+=(*g * (*h));
  return d;
}

float VectorR::intersection(const VectorR& c1, const VectorR &c2, int len, int norm) {
  VectorR A(c1), B(c2);
  if (norm) {
    A.normalize(1, 1); 
    B.normalize(1, 1); 
  }
  const float *g = A.data(), *h = B.data();
  float d = 0;
  //for (i = 0; i < len; i++, g++, h++) d+=MIN(*g, *h);
  for (int i = 0; i < len; i++, g++, h++) d+=ABS(*g-*h);
  return d;
}
  
float VectorR::vector_distance(const VectorR& c1, const VectorR &c2, int len, int norm) {
  VectorR A(c1), B(c2);
  if (norm) {
    A.normalize(1, 2); 
    B.normalize(1, 2); 
  }
  const float *g = A.data(), *h = B.data();
  float d = 0;
  for (int i = 0; i < len; i++, g++, h++) d+=SQR(*g-*h);
  return d;
}


float VectorR::vector_multiply(const VectorR& G, const VectorR& H, int size) {
  const float *g = G.data(), *h = H.data();
  float R = 0;
  for (int j = 0; j < size; j++, g++, h++)  R+=(*g) * (*h);
  return R;
}


VectorR VectorR::prepend(const VectorR& H, float term) {
  VectorR temp(H.length()+1);
  temp.setId(H.Id());
  const float *h = H.data();
  float *g = temp.data_nc();
  *g++=term;
  for (int j = 0; j < H.length(); j++, g++, h++)  *g = *h;
  return temp;
}


float VectorR::hamming_distance(const VectorR& c1, const VectorR &c2, 
			int thresh) {
  VectorR h1(threshold(c1, 1.0/thresh)),
    h2(threshold(c2, 1.0/thresh));
  const float *g = h1.data(), *h = h2.data();
  int length = MIN(h1.length(), h2.length());
  float D = 0;
  for (int i = 0; i < length; i++, g++, h++) {
    if (*g != *h) D++;
  }
  return D;
}


VectorR VectorR::threshold(const VectorR &A, float value) {
  VectorR temp(A);
  float *g = temp.data_nc();
  for (int i = 0; i < temp.length(); i++, g++) 
    *g = (*g > value) ? 1 : 0;
  return temp;
}

VectorR VectorR::sort(const VectorR &A, int top) {
  VectorR temp(A), B(A);
  B.clear();
  temp.normalize(1);
  float *g = B.data_nc();
  for (int i = 0; i < top; i++, g++) {
    int m = (int)temp.max_index();
    *g = *(temp.data(m));
    *(temp.data_nc(m)) = 0;
  }
  return B;
}

int VectorR::nonzeroterms() const {
  const float *h = data();
  //  float cols = 0;// modified by sth
  int cols = 0;
  for (int i = 0; i < length(); i++, h++) if (*h>0) cols++;
  return cols;
}

float VectorR::sum() const {
  const float *h = data();
  float sum = 0;
  for (int i = 0; i < length(); i++, h++) sum+=ABS((*h));
  return sum;
}

float VectorR::norm(float p) const {
  const float *h = data();
  float sum = 0;
  for (int i = 0; i < length(); i++, h++) sum+=pow(ABS(*h), p);
  return pow(sum, 1.0f/p);
}

VectorR& VectorR::normalize(float fctr, float p) {
  float *h = data_nc();
  float n, s = norm(p);
  if (s == 0) n=0;
  else n = fctr/s;
  for (int i = 0; i < length(); i++, h++) *h*=n;
  //cerr << "NORMALIZING" << endl;
  return *this;
}

VectorR& VectorR::operator*=(float d) {
  float *h = data_nc();
  for (int i = 0; i < length(); i++, h++) *h*=d;
  return *this;
}

VectorR& VectorR::operator+=(float d) {
  float *h = data_nc();
  for (int i = 0; i < length(); i++, h++) *h+=d;
  return *this;
}

VectorR& VectorR::operator/=(float d) {
  return *this*=(1/d);
}
  
VectorR& VectorR::square_terms() {
  float *h = data_nc();
  for (int i = 0; i < length(); i++, h++) *h*=*h;
  return *this;
}

VectorR& VectorR::power(float expon) {
  float *h = data_nc();
  for (int i = 0; i < length(); i++, h++) pow(*h, expon);
  return *this;
}

float VectorR::max_index() const {
  float dmax = MINFLOAT;
  int index = 0;
  const float *h = data();
  for (int i = 0; i < length(); i++, h++)
    if (*h > dmax) dmax = *h, index = i;
  return index;
}

float VectorR::min_index() const {
  float dmin = XM_MAXFLOAT;
  int index = 0;
  const float *h = data();
  for (int i = 0; i < length(); i++, h++) 
    if (*h < dmin) dmin = *h, index = i;
  return index;
}

float VectorR::maximum() const {
  float dmax = MINFLOAT;
  const float *h = data();
  for (int i = 0; i < length(); i++, h++) 
    dmax = (dmax > *h) ? dmax : *h;
  return dmax;
}

float VectorR::maxnorm() const {
  float dmax = MINFLOAT;
  const float *h = data();
  for (int i = 0; i < length(); i++, h++) 
    dmax = (dmax > ABS(*h)) ? dmax : ABS(*h);
  return dmax;
}

float VectorR::minimum() const {
  float dmin = XM_MAXFLOAT;
  const float *h = data();
  for (int i = 0; i < length(); i++, h++) 
    dmin = (dmin < *h) ? dmin : *h;
  return dmin;
}

VectorR& VectorR::abs() {
  float *h = data_nc();
  for (int i = 0; i < length(); i++, h++) *h = ABS(*h);
  return *this;
}

float VectorR::mean() const {
  float dmean = 0;
  const float *g = data();
  for (int i = 0; i < length(); i++, g++) dmean+=((*g)*i);
  return dmean/sum();
}
  
float VectorR::variance() const {
  float dmean = mean();
  float dvar = 0;
  const float *g = data();
  for (int i = 0; i < length(); i++, g++) dvar+=(*g*SQR(i-dmean));
  return dvar/sum();;
}

float VectorR::entropy() const {
  const float *d = data();
  float adj = 1/log10(2.0f), entropy = 0;
  float p;
  float norm = 1.0/sum();
  for (int i=0; i<length(); i++, d++) {
   if (*d>0){
     p = *d*norm;
     entropy+=(p*log10(p));
   }
  }
  return -1*adj*entropy;
}

VectorR& VectorR::operator*=(const VectorR& b) {
  const float *g = b.data();
  float *h = data_nc();
  int stop = MIN(length(), b.length());
  for (int i = 0; i < stop; i++, g++, h++) {
    *h*=*g;
    }
  return *this;
}

VectorR& VectorR::operator/=(const VectorR& b) {
  const float *g = b.data();
  float *h = data_nc();
  int stop = MIN(length(), b.length());
  for (int i = 0; i < stop; i++, g++, h++) {
    if (*g!=0) *h/=*g;
    }
  return *this;
}

VectorR& VectorR::operator+=(const VectorR& b) {
  const float *g = b.data();
  float *h = data_nc();
  int stop = MIN(length(), b.length());
  for (int i = 0; i < stop; i++, g++, h++) {
    *h+=*g;
    }
  return *this;
}

VectorR& VectorR::operator-=(const VectorR& b) {
  const float *g = b.data();
  float *h = data_nc();
  int stop = MIN(length(), b.length());
  for (int i = 0; i < stop; i++, g++, h++) {
    *h-=*g;
    }
  return *this;
}

VectorR& VectorR::operator=(const VectorR& b) {
	clear();
	d_length = b.length();
	d_id = b.Id();
	d_data_p = new float[b.length()];
	d_own = 1;
    memcpy(d_data_p, b.data(), d_length*sizeof *d_data_p);

	return *this;
}

/*VectorR operator+(const VectorR& a, const VectorR& b) {
  VectorR temp(a);
  return temp+=b;
}

VectorR operator-(const VectorR& a, const VectorR& b) {
  VectorR temp(a);
  return temp-=b;
}*/

VectorR VectorR::shift(const VectorR &H) {
  VectorR temp(H);
  const float *h = H.data();
  float *g = temp.data_nc();
  temp.clear();
  h++;
  for (int i = 1; i < temp.length(); i++, h++, g++) *g = *h;
  ++g = 0;
  return temp;
}

VectorR VectorR::uniform_quantize(const VectorR& a, int delta) {
  VectorR temp(a.length());
  float *g = temp.data_nc();
  const float *h = a.data();
  int index, halfstep = delta/2;
  for (int i = 0; i < a.length(); i++, h++) {
    index = ((int)(i/delta))*delta + halfstep;
    index = (index > temp.length()-1) ? temp.length()-1 : index;
    g[index]+=*h;
  }
  return temp;
}

VectorR VectorR::cumulative(const VectorR& a, float p) {
  VectorR temp = a;
  //temp.normalize();
  float *g = temp.data_nc();
  float cum = 0;
  for (int i = 0; i < temp.length(); i++, g++) {
    cum+=pow(ABS(*g), p);
    *g = pow(cum, 1.0f/p);
  }
  return temp;
}
    
