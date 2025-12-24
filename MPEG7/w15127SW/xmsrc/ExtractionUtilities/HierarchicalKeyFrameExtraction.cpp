////////////////////////////////////////////////////////////////////////
//
// HierarchicalKeyFrameExtraction.cpp
//
// This software module was originally developed by
//
// Jim Errico, Sharp Laboratories of America, Camas, WA
// Peter van Beek, Sharp Laboratories of America, Camas, WA
// 
// and edited by
// Karol Wnukowicz (Visual Atoms)
// Stavros Paschalakis (Visual Atoms)
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938). 
// This software module is an implementation of a part of one or more 
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard free license to this 
// software module or modifications thereof for use in hardware or 
// software products claiming conformance to the MPEG-7 standard. 
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents. 
// The original developer of this software module and his/her company, 
// the subsequent editors and their companies, and ISO/IEC have no 
// liability for use of this software module or modifications thereof 
// in an application. No license to this software is granted for non 
// MPEG-7 conforming products. 
// Sharp Laboratories of America retains full right to use the software
// module for their own purpose, assign or donate the software module 
// to a third party and to inhibit third parties from using the software 
// module for non MPEG-7 conforming products. 
//
// Copyright (c) 2000=2015.
//
// This copyright notice must be included in all copies or derivative 
// works of this software module.
//
////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <math.h>
#include <fstream>  
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "Descriptors/Descriptors.h"            // COLOR_SPACE
#include "DescriptionSchemes/DescriptionSchemes.h"     /* Naming Convention */

//----------------------------------------------------------------------------

using namespace XM;


const UUID HierarchicalKeyFrameExtractionInterface::myID = UUID("");
const char *HierarchicalKeyFrameExtractionInterface::myName = "HierarchicalKeyFrameDSExtractionInterface";

const UUID HierarchicalKeyFrameExtractionTool::myID = UUID("");
const char *HierarchicalKeyFrameExtractionTool::myName = "HierarchicalKeyFrameExtraction";

const UUID HierarchicalKeyFrameExtractionInterfaceABC::myID = UUID();



//----------------------------------------------------------------------------
// Static data
UINT  Shot::m_numLevels = 0;
std::ofstream ftree;
KeyFrame KeyFrameRef::DefaultKeyFrame = KeyFrame();

// Assumptions and simplifications
const UINT HierarchicalKeyFrameExtractionTool::MIN_SHOT = 3;
//const UINT HierarchicalKeyFrameExtractionTool::NUM_LEVELS = 3;
const UINT HierarchicalKeyFrameExtractionTool::NUM_LEVELS = 4;
//const UINT HierarchicalKeyFrameExtractionTool::RATIO_LEVELS = 2;
const UINT HierarchicalKeyFrameExtractionTool::RATIO_LEVELS = 3;
//const double HierarchicalKeyFrameExtractionTool::RATIO_KEYFRAMES = 0.02F;
const double HierarchicalKeyFrameExtractionTool::RATIO_KEYFRAMES = 0.008F;
const UINT HierarchicalKeyFrameExtractionTool::SUB_SAMPLING = 1;
const ColorSpaceType HierarchicalKeyFrameExtractionTool::COLOR_SPACE = YCRCB;
const UINT HierarchicalKeyFrameExtractionTool::UNIVARIATE_HISTOGRAM = 1;
const UINT HierarchicalKeyFrameExtractionTool::NUM_BINS_1 = 256;
const UINT HierarchicalKeyFrameExtractionTool::NUM_BINS_2 = 128;
const UINT HierarchicalKeyFrameExtractionTool::NUM_BINS_3 = 128;
const UINT HierarchicalKeyFrameExtractionTool::QUANT_BITS = 8;

const UINT SummarizationDOM::INCLUDE_VIDEO_SEGMENT = 2;
const UINT SummarizationDOM::VIDEO_IMAGE_OFFSET = 0;
const UINT SummarizationDOM::LEVEL_HIERARCHY = 0;

const double KeyFrameTree::ALPHA = 3.0;
const double KeyFrameTree::BETA = 2.0;
const double KeyFrameTree::BETA_DECREMENT = 0.2;
const double KeyFrameTree::MAXERASE = 0.45;
const double KeyFrameTree::THETA = 0.75;
const int KeyFrameTree::NUMITER = 9;


//----------------------------------------------------------------------------
char *XM::StripFrameRange( char *buf, const char *mediaFileName, ULONG &start, ULONG &end )
{
        char *pTmp;

        strcpy(buf, mediaFileName);
        if ( strrchr( buf, ']') )
        {
                pTmp = strrchr(buf, '-');
                *pTmp = '\0';
                end = atol(++pTmp);

                pTmp = strrchr(buf, '[');
                *pTmp = '\0';
                start = atol(++pTmp);
        }
        else
        {
                start = 0;
                end = LONG_MAX-1;
        }

        return buf;
}


//----------------------------------------------------------------------------
char *XM::BuildFileName( char *buf, const char *dir, const char *srcFile, const char *ext )
{
        const char *pBaseName;

        // Prepend directory
        if ( dir )
                strcpy( buf, dir );
        else
                buf[0] = '\0';

        // Find basename of source file
        if ( (pBaseName = strrchr(srcFile, '/')) ||
                 (pBaseName = strrchr(srcFile, '\\')) )
                pBaseName ++;
        else
                pBaseName = srcFile;
        strcat( buf, pBaseName );

        // Append extension
        strcat( buf, ext );

        return buf;
}


//----------------------------------------------------------------------------
void XM::TimeStamp(const char* text)
{
#ifdef SHOW_STATUS
	time_t t;
	time(&t);
	struct tm *ptm = localtime(&t);
	fprintf(stderr, "%s %02d:%02d:%02d\n", text?text:"", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
#endif
}


//----------------------------------------------------------------------------
HierarchicalKeyFrameExtractionInterfaceABC::HierarchicalKeyFrameExtractionInterfaceABC()
{
        SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HierarchicalKeyFrameExtractionInterface::HierarchicalKeyFrameExtractionInterface(HierarchicalKeyFrameExtractionTool* aTool):
m_ExtractionTool(aTool)
{
        SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
HierarchicalKeyFrameExtractionInterface::~HierarchicalKeyFrameExtractionInterface()
{
}

//----------------------------------------------------------------------------
int HierarchicalKeyFrameExtractionInterface::SetSourceMedia(MomVop *media)
{
  return m_ExtractionTool->SetSourceMedia(media);
}

//----------------------------------------------------------------------------
void HierarchicalKeyFrameExtractionInterface::destroy()
{
  delete m_ExtractionTool;
}

//----------------------------------------------------------------------------
int HierarchicalKeyFrameExtractionInterface::
SetDescriptorInterface(GenericDSInterfaceABC *aGenericDSInterface)

{
  return m_ExtractionTool->SetDescriptorInterface(aGenericDSInterface);
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC *HierarchicalKeyFrameExtractionInterface::
GetDescriptorInterface()

{
  return m_ExtractionTool->GetDescriptorInterface();
}

//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameExtractionInterface::InitExtracting(char *mediaName)

{
  return m_ExtractionTool->InitExtracting( mediaName );
}

//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameExtractionInterface::StartExtracting(void)

{
  return m_ExtractionTool->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameExtractionInterface::PostExtraction(void)

{
  return m_ExtractionTool->PostExtraction();
}

//----------------------------------------------------------------------------
const UUID& HierarchicalKeyFrameExtractionInterface::GetInterfaceID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HierarchicalKeyFrameExtractionInterface::GetName(void)
{ return myName; }


//----------------------------------------------------------------------------
HierarchicalKeyFrameExtractionTool::HierarchicalKeyFrameExtractionTool():
m_Interface(this),
m_DSInterface(0),
m_histSeries(0)
{
        SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
HierarchicalKeyFrameExtractionTool::~HierarchicalKeyFrameExtractionTool()
{
}

//----------------------------------------------------------------------------
bool HierarchicalKeyFrameExtractionTool::SupportsPush(void)
{ return true; }
//----------------------------------------------------------------------------
bool HierarchicalKeyFrameExtractionTool::SupportsPull(void)
{ return false; }

//----------------------------------------------------------------------------
int HierarchicalKeyFrameExtractionTool::SetSourceMedia(MomVop *media)
{
        m_histSeries->SetSourceMedia(media);
//      m_media = media;
        return(0);
}


//----------------------------------------------------------------------------
int HierarchicalKeyFrameExtractionTool::SetDescriptorInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
  if (!aGenericDSInterface) return -1;

  m_DSInterface = aGenericDSInterface;
  return 0;
}

//----------------------------------------------------------------------------
GenericDSInterfaceABC *HierarchicalKeyFrameExtractionTool::GetDescriptorInterface()
{
  return m_DSInterface;
}

//----------------------------------------------------------------------------
const UUID& HierarchicalKeyFrameExtractionTool::GetObjectID(void)
{ return myID; }

//----------------------------------------------------------------------------
const char *HierarchicalKeyFrameExtractionTool::GetName(void)
{ return myName; }

//----------------------------------------------------------------------------
HierarchicalKeyFrameExtractionInterfaceABC *HierarchicalKeyFrameExtractionTool::GetInterface(void)
{ return &m_Interface; }

//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameExtractionTool::InitExtracting(char *mediaName)
{
        if ( m_histSeries )
                delete m_histSeries;
        // This could be implemented as the combination of existing descriptors (Histogram, 
        // ColorQuantizer, and ColorSpace).  It would require extensions to support a 
        // series of histograms, with difference and cumulative data between each.
        m_histSeries = new HistogramSeries(COLOR_SPACE, QUANT_BITS, NUM_BINS_1, NUM_BINS_2, NUM_BINS_3,
                                                                           UNIVARIATE_HISTOGRAM, SUB_SAMPLING); 

        return m_histSeries->InitExtracting( mediaName );
}


//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameExtractionTool::StartExtracting(void)
{
        return m_histSeries->StartExtracting();
}

//----------------------------------------------------------------------------
unsigned long HierarchicalKeyFrameExtractionTool::PostExtraction(void)
{

        if ( !m_histSeries )
                return (unsigned long)-1;

#ifdef READ_WRITE_HISTOGRAMS
        char histFile[MAXMEDIANAMELEN];

        // Build histogram filename
        BuildFileName( histFile, "/temp/", m_histSeries->GetMediaName(), ".hist" );

        // Save the extracted histogram series
        m_histSeries->Write( histFile );
#endif

        // Extract the key frames
        KeyFrameTree keyFrameTree( MIN_SHOT, RATIO_KEYFRAMES, NUM_LEVELS, RATIO_LEVELS, m_histSeries );
        keyFrameTree.ExtractKeyFrameTree();

        // Build the DOM document
	TimeStamp("Build the DOM document");
        SummarizationDOM summarizationDOM( &keyFrameTree );
        summarizationDOM.CreateRoot(m_DSInterface);
        	
        return 0;
}


//----------------------------------------------------------------------------
Histogram::Histogram(UINT histSize) :
        m_histSize(histSize),
        m_diff(0)
{
        m_data = new ULONG[histSize];
        assert (m_data);
        memset(m_data, 0, sizeof(ULONG)*histSize);
}


//----------------------------------------------------------------------------
std::ostream & XM::operator << (std::ostream &out, Histogram &hist)
{
        for(int i = 0; i < hist.m_histSize; i++)
                out << hist.m_data[i] << std::endl;
        return out;
}


//----------------------------------------------------------------------------
std::istream & XM::operator >> (std::istream &in, Histogram &hist)
{
        for(int i = 0; i < hist.m_histSize; i++)
                in >> hist.m_data[i];
        return in;
}


//----------------------------------------------------------------------------
long XM::GetDiff(Histogram &a, Histogram &b)
{
        long diff = 0;
        ULONG *i = a.GetData();
        ULONG *j = b.GetData();
	
        // Accumulate the absolute value of each individual difference (L1)
        for(int k=0; k < b.m_histSize; k++, i++, j++ )
                diff += *i > *j ? *i - *j : *j - *i;

        return diff;
}


//----------------------------------------------------------------------------
long XM::GetDiffL2(Histogram &a, Histogram &b)
{
        long diff = 0;
        long delta = 0;
        ULONG *i = a.GetData();
        ULONG *j = b.GetData();
	
        // Accumulate the square of each individual difference
        for(int k=0; k < b.m_histSize; k++, i++, j++ )
        {
                delta = *i - *j;
                diff += delta * delta;
        }

        return diff;
}


//----------------------------------------------------------------------------
long XM::GetDiffL2(ULONG *i, Histogram &b)
{
        long diff = 0;
        long delta = 0;
        ULONG *j = b.GetData();
	
        // Accumulate the square of each individual difference
        for(int k=0; k < b.m_histSize; k++, i++, j++ )
        {
                delta = *i - *j;
                diff += delta * delta;
        }

        return diff;
}


//----------------------------------------------------------------------------
const ULONG HistogramSeries::incrBlockSize = 1024;


//----------------------------------------------------------------------------
HistogramSeries::HistogramSeries(ColorSpaceType colorSpace,  UINT quantBits,
                                                                 UINT nBins1, UINT nBins2, UINT nBins3,
                                                                 UINT bUnivariate, UINT subSample) :
        m_blockSize(0),
        m_hist(0),
        m_zdim(0),
        m_height(0),
        m_width(0),
        m_nBins1(nBins1),
        m_nBins2(nBins2),
        m_nBins3(nBins3),
        m_histSize (bUnivariate ? nBins1+nBins2+nBins3 : nBins1*nBins2*nBins3),
        m_colorSpace(colorSpace),
        m_subSample(subSample),
        m_bUnivariate(bUnivariate),
        m_iHist(0)


{
        // Currently only supports specific configurations
        assert( m_bUnivariate );
        assert( subSample == 1 );
        assert( colorSpace == YCRCB );
        assert( quantBits == 8 );

        ULONG binMax = 1 << quantBits;

        // derive histogram offset divisors
        if ( m_bUnivariate )
        {
                // assume all bin sizes are binary multiples
                m_quantizeShift1 = m_quantizeShift2 = m_quantizeShift3 = 0;
                while( m_nBins1 < binMax>>m_quantizeShift1 )    m_quantizeShift1 ++;
                while( m_nBins2 < binMax>>m_quantizeShift2 )    m_quantizeShift2 ++;
                while( m_nBins3 < binMax>>m_quantizeShift3 )    m_quantizeShift3 ++;
                assert( binMax>>m_quantizeShift1 == m_nBins1 &&
                                binMax>>m_quantizeShift2 == m_nBins2 &&
                                binMax>>m_quantizeShift3 == m_nBins3 );
        }

        // Other initialization
        m_mediaName[0] = '\0';
}


//----------------------------------------------------------------------------
HistogramSeries::~HistogramSeries()
{
        // deallocate dynamic resources
        SetZDim(0);
}


//----------------------------------------------------------------------------
int HistogramSeries::SetSourceMedia(MomVop *media)
{
        // Save this media pointer of a single image to use when StartExtracting() is called
        m_image = media; 

        return 0;
}


//----------------------------------------------------------------------------
ULONG HistogramSeries::InitExtracting( char *mediaName )
{
        // Deallocate existing histograms
        SetZDim(0);

        // Store mediaName
        strncpy( m_mediaName, mediaName, sizeof m_mediaName - 1 );

#ifdef READ_WRITE_HISTOGRAMS
        std::ifstream ftest;
        char histFile[MAXMEDIANAMELEN];

        // Build histogram filename
        BuildFileName( histFile, "/temp/", GetMediaName(), ".hist" );
        ftest.open(histFile,std::ios::_Nocreate);
        //ftest.open(histFile,std::ios::nocreate);

        // Attempt to load a previously extracted histogram series
        m_initFromFile = false;
        if ( ftest.is_open() )
        {
                ftest.close();
                std::cerr << "Using existing histogram series " << histFile << std::endl;
                return (m_initFromFile = Read( histFile ) );
        }
#endif

        // reset histogram to be extracted
        m_iHist = 0;

        return false;
}


//----------------------------------------------------------------------------
// allocate, reallocate, and free dynamic resources
bool HistogramSeries::SetZDim( ULONG zdim )
{
        ULONG setBlockSize = (zdim ? (zdim - 1) / incrBlockSize + 1 : 0) * incrBlockSize;

        // Allocate block of pointers
        if ( setBlockSize > m_blockSize )
        {
                Histogram **hist = new Histogram* [setBlockSize];
                assert ( hist );
                if ( m_zdim )
                        memcpy(hist, m_hist, m_zdim * sizeof(Histogram*));
                delete [] m_hist;
                m_hist = hist;
        }


        // Allocate histograms
        while( m_zdim < zdim )
        {
                m_hist[m_zdim] = new Histogram( m_histSize );
                assert( m_hist[m_zdim] );
                m_zdim ++;
        }


        // Deallocate histograms
        while( m_zdim > zdim )
                delete m_hist[--m_zdim];


        // Deallocate block of pointers
        if ( setBlockSize < m_blockSize )
        {
                Histogram **hist = 0;
                if ( setBlockSize )
                {
                        hist = new Histogram* [setBlockSize];
                        memcpy(hist, m_hist, m_zdim * sizeof(Histogram*));
                }
                delete [] m_hist;
                m_hist = hist;
        }

        m_blockSize = setBlockSize;

        return true;
}


//----------------------------------------------------------------------------
ULONG HistogramSeries::StartExtracting()
{
        // confirm assumptions
        assert( m_image->v_chan->type == UCHAR_TYPE && m_image->y_chan->type == UCHAR_TYPE && m_image->u_chan->type == UCHAR_TYPE );

        // retrieve and confirm image dimensions
        if( m_height && m_width && (m_image->height != m_height || m_image->width != m_width) )
                fprintf(stderr, "Warning: Image %ld changed dimensions %dx%d to %dx%d\n",m_iHist,m_width,m_height,m_image->width,m_image->height);
        m_height = m_image->height;
        m_width  = m_image->width;

        // Allocate a new histogram
        SetZDim(m_zdim + 1);

        // Extract the current histogram for this single image
        ExtractFromImage( m_image );

        // build L1 difference 
        // for previous timepoint
        if (m_iHist)
                GetHist(m_iHist - 1).SetDiff( GetDiff( GetHist(m_iHist - 1), GetHist(m_iHist) ) );

        // indicate histogram has been completed
        return m_iHist ++;

}


//----------------------------------------------------------------------------
void HistogramSeries::ExtractFromImage(MomVop *media)
{
        long i, j;
        int chan1, chan2, chan3;        // converted data
        UChar *mediaChan1, *mediaChan2, *mediaChan3;    // pointers into media data
        ULONG *histChan1, *histChan2, *histChan3;       // basepointers into histogram


        /* not yet supported
        if ( m_bUnivariate )
        {
        */
                histChan1 = GetHist(m_iHist).GetData();
                histChan2 = histChan1 + m_nBins1;
                histChan3 = histChan2 + m_nBins2;
        /* not yet supported
        }
        */

        // loop thru pixels building current histogram
        for( i=0; i < m_height; i += m_subSample )
        {
                mediaChan1 = &media->v_chan->data->u[i * m_width];
                mediaChan2 = &media->y_chan->data->u[i * m_width];
                mediaChan3 = &media->u_chan->data->u[i * m_width];

                for( j=0; j < m_width; j += m_subSample )
                {
                        /* not yet supported
                        if ( colorSpace == YCRCB )
                        */
                                // RGB corresponds to channels vyu
                                RGBToYCbCr( *mediaChan1, *mediaChan2, *mediaChan3, 
                                                        &chan1, &chan2, &chan3 );
                        /* not yet supported
                        else if ( colorSpace == RGB )
                        {
                                chan1 = media->v_chan->data->u;
                                chan2 = media->y_chan->data->u;
                                chan3 = media->u_chan->data->u
                        }
                        */
                        // Advance each media channel along this row
                        mediaChan1 += m_subSample;
                        mediaChan2 += m_subSample;
                        mediaChan3 += m_subSample;

                        /* not yet supported
                        if ( m_bUnivariate )
                        {
                        */
                                histChan1[ chan1 >> m_quantizeShift1 ] ++;
                                histChan2[ chan2 >> m_quantizeShift2 ] ++;
                                histChan3[ chan3 >> m_quantizeShift3 ] ++;
                        /* not yet supported
                        }
                        */
                }
        }
}


#ifdef READ_WRITE_HISTOGRAMS
//----------------------------------------------------------------------------
void HistogramSeries::Write(char *filename)
{
        std::ofstream fout;
        int i;

        if ( m_initFromFile )
                return;

        fout.open(filename, std::ios::out );
        assert( fout.is_open() );
        std::cerr << "Writing HistogramFile: " << filename << std::endl;

        // Dump image sizes so we know those 
        // when we read histogram that have been previously saved

        fout << m_width << " ";
        fout << m_height << " ";
        fout << m_zdim << " ";
        fout << m_nBins1 << " ";
        fout << m_nBins2 << " ";
        fout << m_nBins3 << " ";
        fout << (int)m_colorSpace << " ";
        fout << m_subSample << " ";
        fout << m_bUnivariate << " ";

        for(i=0;i<m_zdim;i++)
                fout << GetHist(i);

        fout.close();

        return;

}


//----------------------------------------------------------------------------
ULONG HistogramSeries::Read(char *filename)
{
        std::ifstream fin;
        ULONG width, height, zdim;
        UINT nBins1, nBins2, nBins3;
        ColorSpaceType colorSpace;
        UINT subSample;
        UINT bUnivariate;

        int i;

        fin.open(filename, std::ios::in );
        assert( fin.is_open() );


        // Read image dimensions 
        fin >> width;
        fin >> height;
        fin >> zdim;
        fin >> nBins1;
        fin >> nBins2;
        fin >> nBins3;
        fin >> (int&)colorSpace;
        fin >> subSample;
        fin >> bUnivariate;

  if ( nBins1 != m_nBins1 || nBins2 != m_nBins2 || nBins3 != m_nBins3 ||
           colorSpace != m_colorSpace || subSample != m_subSample || bUnivariate != m_bUnivariate )
        {
                std::cerr << "HistogramFile " << filename << " differs from internal config" << std::endl;
                std::cerr << "nBins1\t" << nBins1 << "\t" << m_nBins1 << std::endl;
                std::cerr << "nBins2\t" << nBins2 << "\t" << m_nBins2 << std::endl;
                std::cerr << "nBins3\t" << nBins3 << "\t" << m_nBins3 << std::endl;
                std::cerr << "colorSpace\t" << colorSpace << "\t" << m_colorSpace << std::endl;
                std::cerr << "subSample\t" << subSample << "\t" << m_subSample << std::endl;
                std::cerr << "bUnivariate\t" << bUnivariate << "\t" << m_bUnivariate << std::endl;
                return false;
        }

        // Allocate space
        SetZDim( zdim );



        for(i=0;i<m_zdim;i++)
                fin >> GetHist(i);

        fin.close();

        // Recreate derived data
        for( m_iHist=1; m_iHist<m_zdim; m_iHist++)
                GetHist(m_iHist - 1).SetDiff( GetDiff( GetHist(m_iHist - 1), GetHist(m_iHist) ) );

        return true;
}
#endif


//----------------------------------------------------------------------------
void HistogramSeries::RGBToYCbCr( unsigned char RVal, unsigned char GVal, unsigned char BVal,
                                                                  int *YPt, int *CbPt, int *CrPt )
{

        double dRVal = (double)RVal;
        double dGVal = (double)GVal;
        double dBVal = (double)BVal;

        *YPt = (int)(4096.0+(46.5375*dRVal)+(156.6726*dGVal)+(15.7899*dBVal)) >> 8; 
        if( *YPt > 235 )
        {
                *YPt = 235;
        }
        else if( *YPt < 16 )
        {
                *YPt = 16;
        }


        *CbPt = (int)(32768.0+(112.0*dBVal)-(25.76*dRVal)-(86.24*dGVal)) >> 8; 

        if( *CbPt > 240 )
        {
                *CbPt = 240;
        }
        else if( *CbPt < 16 )
        {
                *CbPt = 16;
        }


        *CrPt = (int)(32768.0+(112.0*dRVal)-(101.696*dGVal)-(10.304*dBVal)) >> 8; 

        if( *CrPt > 240 )
        {
                *CrPt = 240;
        }
        else if( *CrPt < 16 )
        {
                *CrPt = 16;
        }
}


//----------------------------------------------------------------------------
long HistogramSeries::GetDiffHist(ULONG i)
{
        return GetHist(i).GetDiff();
}


//----------------------------------------------------------------------------
long HistogramSeries::GetDiffHist(ULONG i, ULONG j)
{
        return GetDiff( GetHist(i), GetHist(j) );
}


//----------------------------------------------------------------------------
KeyFrameTree::KeyFrameTree( UINT minShotLength, float ratioKeyFrames, UINT numLevels, 
                                          UINT ratioLevels, HistogramSeries *histSeries ) :
m_histSeries(histSeries),
m_shot(0),
m_keyFrames(0),

m_numShots(0),
m_minShotLength(minShotLength),
m_numLevels(numLevels),
m_ratioLevels(ratioLevels)

{
        assert( m_histSeries );

        m_zdim = m_histSeries->GetNumFrames();
        m_numRequestedKeyFrames = (unsigned long)ratioKeyFrames * m_zdim + 0.5;
        if ( m_numRequestedKeyFrames < 1 )
                m_numRequestedKeyFrames = 1;

        Shot::SetNumLevels(m_numLevels);
}


//----------------------------------------------------------------------------
KeyFrameTree::~KeyFrameTree()
{
        delete [] m_shot;
}


//----------------------------------------------------------------------------
Shot::~Shot()
{
                delete [] m_level;
}


//----------------------------------------------------------------------------
Level::~Level()
{
        if(m_numAllocKeyFrameRefs)
                delete [] m_keyFrameRef;
}


//----------------------------------------------------------------------------
KeyFrameRef::~KeyFrameRef()
{
}


//----------------------------------------------------------------------------
Shot::Shot()
{
        assert(m_numLevels);
        m_level = new Level[m_numLevels];
#ifdef DEBUG_FILES
		for(int i=0; i<m_numLevels; i++)
			m_level[i].m_iLevel = i;
#endif
        assert(m_level);
}


//----------------------------------------------------------------------------
void KeyFrameTree::ExtractKeyFrameTree()
{
        UINT iShot;
        // Determine shot boundaries
	TimeStamp("FindShotChanges");
        FindShotChanges( ALPHA, m_minShotLength );

#ifdef DEBUG_FILES
	char buf[MAXMEDIANAMELEN];
	m_fkey.open( BuildFileName(buf,"/temp/",GetMediaName(),"_Key.txt") );
	assert( m_fkey.is_open() );

	// output shot changes detected 
	m_fkey << "Number of detected shots = " << m_numShots << endl;
	m_fkey << "Detected Shot Boundaries ..." << endl;
	for( iShot = 0; iShot < m_numShots; iShot++)
		m_fkey << "Inclusive Low boundary of shot #" << iShot << " is at frame number: " << GetShot(iShot).GetStart() << endl;
#endif

        // Determine the number of key frames to be allocated to each shot
	TimeStamp("AllocateKeyFramesPerShot");
        AllocateKeyFramesPerShot();

#ifdef DEBUG_FILES
	// output allocated key frames
	for( iShot = 0; iShot < m_numShots; iShot++)
		m_fkey << "Shot #" << iShot << " : Assigned number of keyframes is "  << GetShot(iShot).GetLevel(0).GetNumKeyFrames() << endl;
#endif


        // Find key frames

	TimeStamp("DistributeInitialKeyFrames");
        DistributeInitialKeyFrames();

	TimeStamp("MaximizeInterKeyFrame");
        MaximizeInterKeyFrame();
	
	TimeStamp("PruneKeyFrames");
        PruneKeyFrames();



#ifdef DEBUG_FILES
	ofstream _ftree;
	ftree = _ftree;
	ftree.open( BuildFileName(buf,"/temp/",m_histSeries->GetMediaName(),"_Tree.txt") );
	assert( ftree.is_open() );
#endif

	TimeStamp("Allocate&IdentifyKeyFramesWithinShot");
        // Generate hierarchical summary 
        for( iShot = 0; iShot < m_numShots; iShot ++ )
        {
                AllocateKeyFramesWithinShot(iShot);

                IdentifyKeyFramesWithinShot(iShot);
        }

#ifdef DEBUG_FILES
	ftree.close();
#endif
}



//----------------------------------------------------------------------------
void KeyFrameTree::FindShotChanges(float alpha, UINT minShotLength)
{
        ULONG iFrame, iShot;
        bool *shotBdry; // non-zero on frames that start a shot
        double threshold=0.0;
        ULONG cntr = 0;
        double mu=0.0;
        double sigma=0.0;
        int prev = 0;         // Set previous shot boundary at 0
        double currentMean;
        double currentVar;

#ifdef DEBUG_FILES
	ofstream fshot;
	char buf[MAXMEDIANAMELEN];
	fshot.open(BuildFileName( buf, "/temp/", GetMediaName(), "_ShotFile.txt"));
	assert(fshot.is_open());
#endif

        // Allocate memory for recording shot boundaries 
        // shotBdry is an array flagging the start of a new shot 
        shotBdry = new bool[m_zdim];


        // Set minimum shot length to 3 frames 
        if(minShotLength < 3)
                 minShotLength = 3;
 

        // First shot boundary corresponds to first frame in sequence. 
        // At least one shot from frame 0 to end of sequence
        shotBdry[0] = true;
        m_numShots = 1;

        // m_diffHist has dimension m_zdim-1 
        for( iFrame = 0; iFrame < (m_zdim-1); iFrame++)
        {
                cntr++;
                // calculate first and second running moments
                mu += m_histSeries->GetDiffHist(iFrame);
                sigma += ((double)(m_histSeries->GetDiffHist(iFrame)) *
                                  (double)(m_histSeries->GetDiffHist(iFrame)));

                // Calculate threshold value to detect 
                currentMean = mu / (double) cntr;
                currentVar = (sigma/(double)cntr) - (currentMean*currentMean);
                threshold = currentMean + (alpha*sqrt(currentVar));

                // m_diffHist[iFrame] represents the histogram difference
                // between frame iFrame and iFrame+1
                if( (m_histSeries->GetDiffHist(iFrame)>threshold) && ((iFrame-prev)>minShotLength) )
                {
                        // Record shot boundary and reset estimates
                        shotBdry[iFrame+1] = true;
                        // Frame iFrame+1 is the first to belong to new shot
                        prev = iFrame+1;
                        mu = 0.0;
                        sigma = 0.0;
                        cntr = 0;
                        m_numShots++;

#ifdef DEBUG_FILES
			fshot << iFrame << " " << m_histSeries->GetDiffHist(iFrame) << " " << (long)threshold << endl;
#endif

                }
                else
                {
                        shotBdry[iFrame+1] = false;
                }
        }

        // Instantiate Shots
        m_shot = new Shot[m_numShots];

        iShot=0;

        // Start recording location of beginning 
        // of each shot (shot boundary)
        // Account for the first boundary at zero 
        // since shotBdry[0] was set to 1
        for( iFrame = 0; iFrame < m_zdim; iFrame++)
        {
                if( shotBdry[iFrame] )
                {
                        assert( iShot < m_numShots );
                        GetShot(iShot++).SetStart(iFrame);
                }
        }
        // Release resources
        delete [] shotBdry;

#ifdef DEBUG_FILES
	char *str = new char[10];
	for( iShot = 0; iShot < m_numShots; iShot++)
		fshot << GetShot( iShot ).GetStart() << endl;
	fshot.close();
#endif

        return;
}



//----------------------------------------------------------------------------
void KeyFrameTree::AllocateKeyFramesPerShot()
{
        ULONG iShot, iFrame, iStart, iEnd;
        int totKeysAllocd;
        double totalCumulativeDif;
        double val;
        double minResidue;
        double maxResidue;
        int minIndex;
        int maxIndex;
        double *shotCumDiff;
	
#ifdef DEBUG_FILES
	ofstream falloc;
	char buf[MAXMEDIANAMELEN];
	falloc.open(BuildFileName( buf, "/temp/", GetMediaName(), "_AllocFiles.txt"));
	assert(falloc.is_open());
#endif

        // Test assumptions
        assert ( m_numRequestedKeyFrames < m_zdim );
	
        // Allocate memory for recording the cumulative differences for each shot 
        shotCumDiff = new double[ m_numShots ] ;

        totalCumulativeDif = 0.0;

        for( iShot = 0; iShot < m_numShots; iShot++)
        {


                // Inclusive Lower boundary of shot 
                iStart = GetShot(iShot).GetStart();
                // Exclusive Higher boundary of shot 
                iEnd = ((iShot+1) == m_numShots) ? m_zdim : GetShot(iShot + 1).GetStart();

                GetShot(iShot).SetNumKeyFrames( 0 );
                shotCumDiff[iShot] = 0.0;

                // Choose (iEnd-2) as last array entry 
                // since it represents histogram difference 
                // between frames (iEnd-1) and iEnd , which are 
                // the last two frames belonging to this segment
                for( iFrame = iStart; iFrame < (iEnd-1); iFrame++)
                {
                        // add one to cumulative distortion 
                        // to avoid totalCumulative distortion 
                        // to be equal to 0 
                        shotCumDiff[iShot] += m_histSeries->GetDiffHist(iFrame);
                }
                // The histogram differences at the shot 
                // boundaries shall always be ignored 
                totalCumulativeDif += shotCumDiff[iShot];

#ifdef DEBUG_FILES
		falloc << iShot << " " << shotCumDiff[iShot] << endl;
#endif

        }

        // To keep track of how many key frames have already 
        // been assigned
        totKeysAllocd = 0;



        // At this point each m_numKeysPerShot entry represents a shot 
        // cumulative Histogram Difference 
        // m_numKeysPerShot is going to be overwritten such that each 
        // of its entries represents the number of key frames
        // allocated to the shot 

        if( totalCumulativeDif == 0.0 )
        {
                assert( m_numShots >= m_numRequestedKeyFrames );

                // Assign one keyframe per shot 
                // Since they are all the same frames
                for( iShot = 0; iShot < m_numShots; iShot++)
                {
                        GetShot(iShot).SetNumKeyFrames( 1 );
                }
        }
        else 
        {

                // Allocate memory for storing fractional amounts of keyframes 
                double *residualAmount = new double[ m_numShots ];
                for( iShot = 0; iShot < m_numShots; iShot++)
                {
	
                        val = shotCumDiff[iShot] * (double)m_numRequestedKeyFrames / totalCumulativeDif;

                        // Quantize to lower integer;
                        GetShot(iShot).SetNumKeyFrames( (UINT)floor( (double) val) );

                        // When all histogram are similar or almost similar 
                        // val could be quantized down to 0 
                        // So here allocate at least one keyframe 
                        // to the shot
                        if( GetShot(iShot).GetNumKeyFrames() == 0 )
                        {
                                GetShot(iShot).SetNumKeyFrames( 1 );
                        }

                        // Residual will be 0 or positive as long as 
                        // val is greater or equal to 1
                        // Residual will be negative when val is less 
                        // than 1

                        residualAmount[iShot] = val - (double)GetShot(iShot).GetNumKeyFrames();
                        totKeysAllocd += GetShot(iShot).GetNumKeyFrames();

#ifdef DEBUG_FILES
			falloc << iShot << " " << GetShot(iShot).GetNumKeyFrames() << " " << residualAmount[iShot] << endl;
#endif

                }

                // The following case arises when shots for which m_numKeysPerShot[iShot]=0 
                // end up taking more keyframes than made available 
                // through floor() quantization 
                // In this case remove one keyframe from shots where 
                // normalized cumulative histogram differences are closest 
                // to the floor integer (smallest POSITIVE residual)

                while( totKeysAllocd > m_numRequestedKeyFrames )
                {
                        minResidue = totalCumulativeDif;
                        minIndex = 0-1;
                        for( iShot = 0; iShot < m_numShots; iShot++)
                        {
                                // Can't substract from shot which has one keyframe only
                                // This includes the case where m_numKeysPerShot[iShot] was forced to 1
                                // (negative residual)
                                if( GetShot(iShot).GetNumKeyFrames() <= 1 ) 
                                {
                                        continue;
                                }
                                else if( residualAmount[iShot] <= minResidue )
                                {
                                        minResidue = residualAmount[iShot];
                                        minIndex = iShot;
                                }
                        }
                        if( minIndex >= 0 )
                        {
                                // Subtract one frame 
                                GetShot(minIndex).SetNumKeyFrames( GetShot(minIndex).GetNumKeyFrames() - 1 );
                                // Account for taking away a keyframe
                                totKeysAllocd -= 1;
                                // Disallow subtracting any additional keyframes from this shot
                                residualAmount[minIndex] = 2 * totalCumulativeDif;
                        }
                        else 
                        {
                                std::cerr << "Warning: Can't no longer adjust number of keyframe" <<std::endl;
                                m_numRequestedKeyFrames = totKeysAllocd;
                                break;
                        }
        	
                }

                // The following arises when quantization 
                // has led to assign less key frames than
                // budgeted. 
                // Rank residual in decreasing order 
                // and at the same time , give one more
                // keyframe to shots having the largest 
                // residuals 
                // Do this operation until keyframe budget 
                // has been consumed 
                while( m_numRequestedKeyFrames > totKeysAllocd )
                {
                        maxResidue = 0.0;
                        maxIndex = 0-1;
                        for( iShot = 0; iShot < m_numShots; iShot++)
                        {
                                if( residualAmount[iShot] >= maxResidue )
                                {
                                        maxResidue = residualAmount[iShot];
                                        maxIndex = iShot;
                                }
                        }
                        if( maxIndex >= 0 ) 
                        {
                                // Add one more frame 
                                GetShot(maxIndex).SetNumKeyFrames( GetShot(maxIndex).GetNumKeyFrames() + 1 );
                                // Account for taking away a keyframe
                                totKeysAllocd += 1;
                                // Disallow allocating any additional keyframes to this shot
                                residualAmount[maxIndex] = 0-1;
                        }
                        else 
                        {
                                std::cerr << "Warning: Can't no longer adjust number of keyframe" <<std::endl;
                                m_numRequestedKeyFrames = totKeysAllocd;
                                break;
                        }
                }
                delete [] residualAmount;
        }

        delete [] shotCumDiff;

#ifdef DEBUG_FILES
	for( iShot = 0; iShot < m_numShots; iShot ++ )
		falloc << iShot << " " << GetShot(iShot).GetNumKeyFrames() << std::endl;
	falloc.close();
#endif

}




//----------------------------------------------------------------------------
void KeyFrameTree::DistributeInitialKeyFrames()
{
        int i,j,k;
        int brkpt;
        int save_brkpt;
        int iStart;
        int iEnd;
        UINT keydumcntr=0;
        int *cum;
//        int initial_value=0;
//        int totcntr=0;
//        int pbest=0;
//        int curdif=0;
//        long int cntr=0;
        float beta;
        int offset_val;
        int inc_val;
	
        int tmp_index;


        m_numKeyFrames = 0;

        // Allocate memory to store location of keyframes 
        // Number of entries required is maximum 
        // number of keyframes allowed
        m_keyFrames = new KeyFrame[ m_numRequestedKeyFrames ];

        // Allocate storage for cumulative difference histograms 
        cum = new int[m_zdim];

        // new formula for beta
        beta  = (float)(log10( (double)m_zdim/ (double)m_numRequestedKeyFrames) * BETA);
        
#ifdef DEBUG_FILES
	m_fkey << "Value of Parameter Beta is : " << beta << endl;
#endif
		
		if(beta > ALPHA)
        {
                beta = ALPHA;
        }



        int increment;

        for( i = 0; i < m_numShots; i++)
        {

                // Inclusive Lower boundary of shot 
                iStart = GetShot(i).GetStart();
                // Exclusive Higher boundary of shot 
                iEnd = ((i+1) == m_numShots) ? m_zdim : GetShot(i + 1).GetStart();
        	
                // No need to go through optimal keyframe 
                // location algorithm if only one key frame 
                // need to be located 
                if( GetShot(i).GetNumKeyFrames() == 1)
                {
                        // Set location of keyframe at shot 
                        // middle point
                        //m_keyFrames[m_numKeyFrames++] = (iStart+iEnd) >> 1;
                        GetKeyFrame( m_numKeyFrames++ ).SetFrame( (iStart+iEnd) >> 1 );
                }
                else
                {
                        // Calculate evolution of cumulative histogram 
                        // differences over shot 

                        // Initialize array to histogram difference 
                        // between frame iStart and frame (iStart+1)
                        cum[iStart] = m_histSeries->GetDiffHist(iStart);
                        for( j = (iStart+1); j < (iEnd-1); j++)
                        {
                                cum[j] = cum[j-1] + m_histSeries->GetDiffHist(j);
                        }

                        // initialize beginning of FIRST segment to t0
                        brkpt = iStart;



                        // Initialize increment defining location of
                        // beginning of NEXT segment  
                        // (which is equal to iStart + increment) 
                        increment = 1;

                        // calculate break points t1 through tn
                        // cum entries go from iStart to iEnd-2 included
                        // Entry at iEnd-2 represents 
                        // cumulative histogram difference over
                        // entire shot. This because diffHist[iEnd-2]
                        // is histogram difference between frame 
                        // (iEnd-2) and (iEnd-1)

                        while( 1 )
                        {
                                keydumcntr = m_numKeyFrames;

                                // Set boundary of SECOND segment 
                                // at brkpt  (since first segment 
                                // goes from iStart to brkpt-1 included ) 

                                // Position first keyframe in segment 
                                // Note increment is at least equal to 1
                                // First keyframe cannot coincide with iStart 
                                // because from t_i = 2*k_i - t_(i-1) 
                                // segment would be null since t_i == t_(i-1)

                                GetKeyFrame(keydumcntr).SetFrame( iStart + increment );

                                // brkpt is beginning of next segment 
                                brkpt = ( 2 * GetKeyFrame(keydumcntr).GetFrame() ) - iStart;

                                // Assign the remaining key frames 
                                // in the shot 
                                // Account for the fact that one keyframe 
                                // has already been assigned by starting 
                                // at j = 1;

                                for( j = 1; j < GetShot(i).GetNumKeyFrames(); j++)
                                {

                                        // Given the CURRENT keyframe 
                                        // at keydumcntr, and given
                                        // brkpt which is the NEXT
                                        // segment lower boundary 
                                        // (meaning that current 
                                        // key frame stops at brkpt-1), 
                                        // look to establish location 
                                        // of next keyframe in the 
                                        // NEXT segment

                                        // Start testing from second 
                                        // frame within this segment :
                                        // k will represent the candidate 
                                        // new keyframe of the NEXT segment
                                        // k cannot coincide with segment 
                                        // boundary because resulting segment
                                        // would be null 

                                        k = brkpt + 1;

                                        // Do test until 
                                        // candidate keyframe verifies 
                                        // condition for being 
                                        // promoted to a real key frame

                                        tmp_index = GetKeyFrame(keydumcntr).GetFrame();
                                        while( (k <= (iEnd-2)) && ( ((2*cum[brkpt])-cum[tmp_index]) > cum[k] ) )
                                        {
                                                // Increment k if test 
                                                // has not been satisfied 
                                                k++;
                                        }

                                        // k can be equal to iEnd coming from 
                                        // the initialization k=brkpt+1
                                        // In this case arbitrarily 
                                        // position the keyframe at 
                                        // iEnd-1 (the last possible position
                                        // within the current shot )


                                        if( k == iEnd )
                                        {
                                                std::cerr << "Forcing position of keyframe " << std::endl;
                                                //m_keyFrames[++keydumcntr] = k-1;
                                                GetKeyFrame( ++keydumcntr ).SetFrame( k-1 );
                                        }
                                        else 
                                        {
                                                // Save location of keyframe 
                                                // belonging to next segment 
                                                // and increment to account for 
                                                // new keyframe
                                                //m_keyFrames[++keydumcntr] = k;
                                                GetKeyFrame( ++keydumcntr ).SetFrame( k );
                                        }

                                        // added to support case where increment == 1 
                                        save_brkpt = brkpt;

                                        // Calculate boundary of 
                                        // the NEW NEXT segment 
                                        // ti = (2 * ki) - ti 

                                        brkpt = (2*k) - brkpt;
                        	
                                        // Test whether NEW NEXT boundary 
                                        // falls into shot 
                                        // Exit if not 

                                        if( brkpt >= iEnd )
                                        {
                                                break;
                                        }
                                }

#ifdef DEBUG_FILES
				m_fkey << " ========= brkpt, en = " << brkpt << "  " << iEnd << endl;
				m_fkey << " ============ inc =  " << increment << endl;
#endif

                                if( brkpt >= iEnd ) 
                                {
                                        break;
                                }
                                else
                                {
                                        // Move boundary of SECOND segment by 
                                        // one to the right
                                        increment++;
                                }
                        }


                        // This case arises when previous loops 
                        // have either exceeded the 
                        // shot length in the first iteration 
                        // indicating that there are too many key frames

                        if( increment == 1 )
                        {

                                // restore previous break point 
                                brkpt = save_brkpt;

#ifdef DEBUG_FILES
				//				m_fkey << "too many key frames!!! spacing them evenly for shot " <<  i << '\n';
				m_fkey << "too many key frames!!! " << endl;
				m_fkey << "Old keyframe count for shot # " <<  i << "  was : " << GetShot(i).GetNumKeyFrames() << endl;
#endif

                                // calculate new keyframe count 
                                GetShot(i).SetNumKeyFrames( keydumcntr - m_numKeyFrames + 1 );

#ifdef DEBUG_FILES
				m_fkey << "New keyframe count for shot # " <<  i << "  is : " << GetShot(i).GetNumKeyFrames() << endl;

				// Adjust key frame location
				// from the number of remaining frames
				// at end of the shot 

				m_fkey << "REMAINDER is equal to " << iEnd-brkpt << endl;
#endif

                                inc_val = (int)((double)(iEnd - brkpt) / (double)(keydumcntr + 1));
                                offset_val = (int)((double)(iEnd - brkpt) / (double)((keydumcntr + 1) * 2));
                                while( m_numKeyFrames <= keydumcntr )
                                {

#ifdef DEBUG_FILES
					m_fkey << "============= keyframe at (before) " << GetKeyFrame(m_numKeyFrames).GetFrame() <<  endl;
#endif
									
										//m_keyFrames[m_numKeyFrames++] += offset_val;
                                        GetKeyFrame( m_numKeyFrames ).SetFrame( 
                                                GetKeyFrame( m_numKeyFrames ).GetFrame() + offset_val );
                                        m_numKeyFrames++;

#ifdef DEBUG_FILES
					m_fkey << "============= keyframe at (after) " << GetKeyFrame(m_numKeyFrames - 1).GetFrame() <<  endl;
#endif

                                        offset_val += inc_val; 
                                }

                        }

                        // Go back one increment level , the last one 
                        // which did not fail 
                        else
                        {

                                // Set boundary of SECOND segment 
                                // at brkpt  (since first segment 
                                // goes from iStart to brkpt-1 ) 
                                increment = increment - 1;

#ifdef DEBUG_FILES
				m_fkey << "INCREMENT = " << increment << endl;
#endif

                                keydumcntr = m_numKeyFrames;

                                // Set position of first key frame 
                                //m_keyFrames[keydumcntr] = iStart + increment ;
                                GetKeyFrame( keydumcntr ).SetFrame( iStart + increment );

                                // Calculate upper boundary of second segment
                                brkpt = ( 2 * GetKeyFrame( keydumcntr ).GetFrame() ) - iStart;

                                // Assign the remaining key frames 
                                // in the shot 
                                // Account for the fact that one keyframe 
                                // has already been assigned 
                                // at j = 1;
                                for( j = 1; j < GetShot(i).GetNumKeyFrames(); j++)
                                {

                                        // Given the CURRENT keyframe 
                                        // at keydumcntr, and given
                                        // brkpt which is the NEXT
                                        // segment lower boundary 
                                        // (meanining that current 
                                        // key frame stops at brkpt-1), 
                                        // establish location 
                                        // of next keyframe in the 
                                        // NEXT segment

                                        // Start from first 
                                        // frame within this segment :
                                        // k represents the candidate 
                                        // new keyframe of the NEXT segment
                                        k = brkpt + 1;

                                        // Do test until 
                                        // candidate keyframe verifies 
                                        // condition for being 
                                        // promoted to a real key frame
                                        tmp_index = GetKeyFrame( keydumcntr ).GetFrame();
                                        while( (k <= (iEnd-2)) && ( ((2*cum[brkpt])-cum[tmp_index]) > cum[k] ) )

                                        {
                                                // Increment k if test 
                                                // has not been satisfied 
                                                k++;
                                        }

                                        // k can be equal to iEnd coming from 
                                        // the initialization k=brkpt+1
                                        // In this case arbitrarily 
                                        // position the keyframe at 
                                        // iEnd-1 (the last possible position
                                        // within the current shot )


                                        if( k == iEnd )
                                        {
                                                std::cerr << "Forcing position of keyframe " << std::endl;
                                                k--;
                                        }

                                        // Save location of keyframe 
                                        // belonging to next segment 
                                        //m_keyFrames[++keydumcntr] = k;
                                        GetKeyFrame( ++keydumcntr ).SetFrame( k );

                                        // Calculate boundary of 
                                        // the NEW NEXT segment 
                                        brkpt = (2*k) - brkpt;                                	

#ifdef DEBUG_FILES
					m_fkey << "============= j = " << j << "  keyframe at " << GetKeyFrame( keydumcntr ).GetFrame() << " and segment boundary at " << brkpt <<  std::endl;
#endif
					
                                }

                                // Adjust key frame location
                                // from the number of remaining frames
                                // at end of the shot 

#ifdef DEBUG_FILES
				m_fkey << " REMAINDER brkpt, iEnd = " << brkpt << "  " << iEnd << endl;
				m_fkey << "REMAINDER is equal to " << iEnd-brkpt << endl;
#endif

                                // Adjust location of first keyframe 
                                // such that it is midway between origin 
                                // of the shot and first segment boundary 
                                // elongated by iEnd-1-brkpt frames 
//                              m_keyFrames[m_numKeyFrames++] += (iEnd-brkpt) >> 1;
        	
                                // The other segments keep the same 
                                // length but as the result of elongating 
                                // the first segment, the position of each 
                                // keyframe must be shifted by iEnd-1-brkpt
                                // frames 


                                inc_val = (int)((double)(iEnd - brkpt) / (double)(keydumcntr + 1));
                                offset_val = (int)((double)(iEnd - brkpt) / (double)((keydumcntr + 1) * 2));
                                while( m_numKeyFrames <= keydumcntr )
                                {
                                        //m_keyFrames[m_numKeyFrames++] += offset_val;
                                        GetKeyFrame( m_numKeyFrames ).SetFrame( 
                                                GetKeyFrame( m_numKeyFrames ).GetFrame() + offset_val );
                                        m_numKeyFrames++;
                                        offset_val += inc_val; 
                                }
                        	
                        } 
                }
        } 


        // release memory used for cumulative difference histograms 
        delete [] cum;

#ifdef DEBUG_FILES
	m_fkey << ">>>>>>>>>>>>> Final count of keyframes is " << m_numKeyFrames << endl;
	// Save positions of keyframes 
	for(i=0;i<m_numKeyFrames;i++)
		m_fkey << " >>>>>>>>>>>> keyframe at " << GetKeyFrame( i ).GetFrame() << endl;
#endif

}



//----------------------------------------------------------------------------
void KeyFrameTree::MaximizeInterKeyFrame()
{
        int i,j,k;
        int iStart;
        int iEnd;
        ULONG *keyfrm_;
        int keydumcntr=0;
//        int initial_value=0;
//        int totcntr=0;
        int pbest=0;
        int curdif=0;
//        long int cntr=0;
	
        int tmp_index;

#ifdef DEBUG_FILES
	m_fkey << ">>>>> Version 2 " << endl;
#endif

        // Duplicate keyframe_ for working
        keyfrm_ = new ULONG[m_numKeyFrames];
        for( k = 0; k < m_numKeyFrames; k++)
                keyfrm_[k] = GetKeyFrame(k).GetFrame();

        // Moving the keyframes about so as to maximize inter-key frame
        // histogram difference
        // Algorithm - for each key frame (which is not the start key frame for the shot)
        // move the key frame within its rectangle till you find the best key frame
        // replacement for the usual ``middle key frame choice''.

        keydumcntr=0;


        // Inspect each shot 
        for( i = 0; i < m_numShots; i++)
        {

                keydumcntr++; // skip thru first key frm

                //for each keyframe except the first key frame
                for( j = 1; j < GetShot(i).GetNumKeyFrames(); j++, keydumcntr++)
                {

                        // use array keyfrm_ instead 
                        // since m_keyFrames vector is work array
                        iStart = (keyfrm_[keydumcntr-1]+keyfrm_[keydumcntr]) >> 1;  



                        // Calculate exclusive boundary of the current
                        // segment 
                        if( i < m_numShots-1 )
                        {
                                iEnd = ( j < GetShot(i).GetNumKeyFrames() - 1 )? (keyfrm_[keydumcntr]+keyfrm_[keydumcntr+1]) >> 1 : GetShot( i+1 ).GetStart();
                        }
                        else 
                        {
                                iEnd = ( j < GetShot(i).GetNumKeyFrames() - 1 )? (keyfrm_[keydumcntr]+keyfrm_[keydumcntr+1]) >> 1 : m_zdim;
                        }




                        pbest=0;

                        if( iStart < iEnd) // if there is room to move
                        {
                                for( k = iStart; k < iEnd; k++)
                                {
                                        tmp_index = GetKeyFrame( keydumcntr-1 ).GetFrame();
                                        curdif = m_histSeries->GetDiffHist( k, tmp_index );

                                        if( curdif > pbest )
                                        {
                                                GetKeyFrame(keydumcntr).SetFrame( k );
                                                pbest = curdif;
                                        }
                                }
                        }
                }
        }

        delete [] keyfrm_;
}

//----------------------------------------------------------------------------
void KeyFrameTree::PruneKeyFrames()
{
        int i,j,k;
        int iStart;
        int iEnd;
        int keydumcntr=0;
//        int initial_value=0;
        int totcntr=0;
//        int pbest=0;
//        int curdif=0;
        int l;
        double mean;
        double sigma;
        double threshold;
//        long int cntr=0;
        float beta=0;
	

        int numerase=0;
        int keydumnum;
        float keydiff;
        int *keystore=new int[m_numKeyFrames];
        int *keynumstore=new int[m_numShots];


        // Save position of key frames obtained at this point
        // in the process. 
        for( i = 0; i < m_numKeyFrames; i++)
        {
                keystore[i] = GetKeyFrame(i).GetFrame();
        }


        // Save number of keyframes per shot 
        for( i = 0; i < m_numShots; i++)
        {
                keynumstore[i] = GetShot(i).GetNumKeyFrames();
        }

        // Remove keyframes but do not exceed pre-specified count
        // Therefore perform key frame removal iteratively 
        // by adjusting the value of beta until minimum of 
        // keyframes is provided 

        while( 1 )
        {

                // Reset key frame counter 
                keydumcntr = 0;

                // Consider every shot 
                for( i = 0; i < m_numShots; i++)
                {
                        // skip thru first key frame within the shot
                        keydumcntr++;

                        keydumnum = GetShot(i).GetNumKeyFrames();


                        //for each keyframe in the shot 
                        // except the first one
                        for( j = 1; j < GetShot(i).GetNumKeyFrames(); j++, keydumcntr++)
                        {
                                l = keydumcntr - 1;

                                // rewind back over key frames that have 
                                // been removed (positions reset to -1)
                                while( GetKeyFrame(l).GetFrame() == ULONG_MAX )
                                {
                                        l--;
                                }

                                // Position of the last retained key frame
                                iStart = GetKeyFrame(l).GetFrame();

                                // Position of the key frame being considered
                                iEnd = GetKeyFrame(keydumcntr).GetFrame();

                        	
                                // Calculate histogram difference 
                                // between key frame at iStart and key frame at iEnd
                                ///keydiff = calcdifhist_l1(iEnd, iStart);
                                keydiff = m_histSeries->GetDiffHist(iEnd, iStart);

                                mean = 0.0;
                                sigma = 0.0;


                                // Calculate statistics of histogram 
                                // difference between the two key frames
                                // diffHist[iEnd-1] represents histogram 
                                // difference between key frame at iEnd 
                                // and previous frame at iEnd-1

                                for( k = iStart; k < iEnd; k++)
                                {
                                        mean += m_histSeries->GetDiffHist(k);
                                        sigma += ((double)m_histSeries->GetDiffHist(k) * (double)m_histSeries->GetDiffHist(k) );
                                }
                                if( iEnd > iStart )
                                {
                                        mean /= (double) (iEnd -iStart);
                                        sigma /= (double)(iEnd - iStart - 1);
                                	
                                        if( iEnd > iStart+1 )
                                        {
                                                threshold = mean + (beta * sqrt( sigma-(mean*mean*(double)(iEnd-iStart)/(double)(iEnd-iStart-1)) ));
                                        }
                                        else 
                                        {
                                                threshold = mean;
	
                                        }
                                }
                                else 
                                {
                                        // Trigger removal of this keyframe
                                        // which has appeared twice 
                                        // should not happen 
                                        threshold = keydiff + 1;
                                        std::cerr << " histseq.c: Should not happen " << std::endl;
                                        exit( -1 );
                                }

#ifdef DEBUG_FILES
				m_fkey << "===== i,j = " << i << " " << j << "  keydiff,thresh = "  << keydiff << "  " << threshold << std::endl; 
#endif

                                if( keydiff < threshold )
                                {
                                        //erase the keyframe
                                        //m_keyFrames[keydumcntr] = -1;
                                        GetKeyFrame( keydumcntr ).SetFrame( ULONG_MAX );

                                        // Decrement number of key frames by 1
                                        keydumnum--;

                                        // Update number of key frames 
                                        // that has been removed so far
                                        numerase++;
                                }

                        }

                        // Write final number of retained key frames out
                        GetShot(i).SetNumKeyFrames( keydumnum );
                }

                if( numerase > (1+(int)(MAXERASE * (double)m_numKeyFrames)) )
                {
                        // Start all over again if pre-specified maximum 
                        // percentage of key frames removed was exceeded
                        // Do so after adjusting beta 

                        beta -= (float)BETA_DECREMENT;

#ifdef DEBUG_FILES
			m_fkey << "New Value of  beta is " << beta << std::endl;
#endif

                        if( beta < 0 )
                        {
                                beta = 0;
                        }


                        // Restore keyframe positions 
                        for( i = 0; i < m_numKeyFrames; i++)
                        {
                                GetKeyFrame(i).SetFrame( keystore[i] );
                        }

                        // Restore number of keyframes per shot
                        for(i=0; i<m_numShots; i++)
                        {
                                GetShot(i).SetNumKeyFrames( keynumstore[i] );
                        }

                        numerase = 0;
                }
                else
                {
                        // Number of key frames removed satisfies 
                        // pre-specified maximum percentage of 
                        // keyframes to remove
                        // Exit is allowed
                        break;
                }
        }

        delete [] keynumstore;
        delete [] keystore;



        //change the m_keyFrames array to reflect the changed m_keyFrames order


        keydumcntr=0;
        int originalKeyframesCount = m_numKeyFrames;

        for( i = 0; i < originalKeyframesCount; i++)
        {
                if( GetKeyFrame( i ).GetFrame() != ULONG_MAX )
                {
                        //m_keyFrames[ keydumcntr++ ]  = m_keyFrames[ i ];
                        GetKeyFrame( keydumcntr++ ).SetFrame( GetKeyFrame( i).GetFrame() );
                }
                else
                {
                        ///m_keyFrames.setDim( m_numKeyFrames - 1 );
                        m_numKeyFrames --;
                }
        }

#ifdef DEBUG_FILES
	m_fkey << "FINAL KEYFRAMES ( " << m_numKeyFrames << " of them )" << endl;

	for( i = 0; i < m_numKeyFrames; i++)
		m_fkey << "==============>>>>>>> " << GetKeyFrame( i ).GetFrame() << endl;
#endif

}



//=============================================================================
//=============================================================================
//
//                                                      TREE BUILDING SUBSYSTEMS
//
//=============================================================================
//=============================================================================



//=============================================================================
void KeyFrameTree::SetKeyFrameIntoTree(UINT iShot, UINT iLevel, UINT iRef, KeyFrame &keyFrame)
{
        assert( iShot < m_numShots );
        assert( iLevel < m_numLevels );
        assert( iRef < GetShot(iShot).GetLevel(iLevel).GetNumKeyFrames() );

        GetShot(iShot).GetLevel(iLevel).SetKeyFrame( iRef, keyFrame );
}


//=============================================================================
KeyFrame &Level::GetKeyFrame(UINT i)
{
        if(i>=m_numAllocKeyFrameRefs||i>=m_numKeyFrameRefs)
        {
                return KeyFrameRef::DefaultKeyFrame;
        }
        return m_keyFrameRef[i].GetKeyFrame();
}


//=============================================================================
ULONG Level::GetKeyFrameNum(UINT i) 
{
        if(i>=m_numAllocKeyFrameRefs||i>=m_numKeyFrameRefs)
        {
                return (unsigned long)-1;
        }
        return GetKeyFrame(i).GetFrame();
}


//=============================================================================
KeyFrame &Level::GetLastKeyFrame()
{
        return GetKeyFrame(m_numKeyFrameRefs - 1);
}


//=============================================================================
ULONG Level::GetLastKeyFrameNum()
{
        return GetKeyFrameNum(m_numKeyFrameRefs - 1);
}


//=============================================================================
int  Level::GetKeyFramePartition(UINT i)
{
        if(i>=m_numAllocKeyFrameRefs||i>=m_numKeyFrameRefs)
        {
                return -1;
        }
        return m_keyFrameRef[i].GetRefPartition();
}


//=============================================================================
UINT Level::GetNumKeyFrames()
{
        return m_numKeyFrameRefs;
}


//=============================================================================
void Level::AllocKeyFrameRefs()
{
        if(m_numKeyFrameRefs)
                m_keyFrameRef = new KeyFrameRef[m_numAllocKeyFrameRefs = m_numKeyFrameRefs];
}       // Called after numKeyFrames stabilizes


//=============================================================================
void Level::SetKeyFrame(UINT iRef, KeyFrame &k)
{
        assert(iRef < m_numAllocKeyFrameRefs);
        m_keyFrameRef[iRef].SetKeyFrameRef(k);

#ifdef DEBUG_FILES
	ftree << m_iLevel << "\t" << iRef << "\t" << GetKeyFrameNum(iRef) << endl;
#endif

}


//=============================================================================
void  Level::SetKeyFramePartition(UINT iRef, int p)
{
        assert(iRef<m_numAllocKeyFrameRefs);
        m_keyFrameRef[iRef].SetRefPartition(p);

#ifdef DEBUG_FILES
	ftree << "P: " << m_iLevel + 1 << "\t" << iRef << "\t" << GetKeyFramePartition(iRef) << endl;
#endif

}


//=============================================================================
void Level::AllocNumKeyFrames(UINT num)
{
        if( m_numAllocKeyFrameRefs < num )
        {
                KeyFrameRef *keyFrameRef = new KeyFrameRef[ num ];
                assert( keyFrameRef );

                if( m_numKeyFrameRefs )
                {
                        memcpy( keyFrameRef, m_keyFrameRef, m_numKeyFrameRefs * sizeof(KeyFrameRef) );
                        delete [] m_keyFrameRef;
                }
                m_keyFrameRef = keyFrameRef;
                m_numAllocKeyFrameRefs = num;
        }
        m_numKeyFrameRefs = num;
}


//=============================================================================
void Level::SetNumKeyFrames(UINT num)
{
        // This function should not be called after the KeyFrameRefs have been allocated
        assert( m_numAllocKeyFrameRefs == 0 );

        m_numKeyFrameRefs = num;

#ifdef DEBUG_FILES
	ftree << "N: " << m_iLevel << "\t" << num <<  endl;
#endif

}


//=============================================================================
void KeyFrameTree::AllocateKeyFramesWithinShot(UINT iShot)
{
        int i,j;
        float val;
        int st;
        int en;


        // Determine first INCLUSIVE index st 
        // of keyframe belonging to this shot 
        for( i = 0; i < GetNumKeyFrames(); i++)
        {
                // Need >= test since shotStart represents an INCLUSIVE
                // upper shot boundary
                if( GetKeyFrame(i).GetFrame() >= GetShot(iShot).GetStart() )
                {
                        st=i;
                        break;
                }
        }

        // Determine first EXCLUSIVE index en
        // of keyframe NOT belonging to this shot 
        if ( iShot >= GetNumShots() - 1 )
                en = GetNumKeyFrames();
        else
                for( i = 0; i < GetNumKeyFrames(); i++)
                {
                        // Need >= test since shotEnd represents an EXCLUSIVE
                        // upper shot boundary
                        if( GetKeyFrame(i).GetFrame() >= GetShot(iShot+1).GetStart() )
                        {
                                en=i;
                                break;
                        }
                }

#ifdef DEBUG_FILES
		ftree << "Shot\t" << iShot << "\t" << st << "\t" << en << endl << "Levels";
#endif

        j = 1;
        for( i=0; i<GetNumLevels(); i++, j *= m_ratioLevels)
        {
                // Calculate number of keyframe at level i 

                val = (float)(en-st)/ (float)j;
                // store number of keyframes at level i
                GetShot(iShot).GetLevel(i).SetNumKeyFrames(     (val)>((int)(val)+0.5)?(int)(val)+1:(int)(val) );
                if( (i > 0) && (GetShot(iShot).GetLevel(i - 1).GetNumKeyFrames() <= 1) )
                {
                        GetShot(iShot).GetLevel(i).SetNumKeyFrames( 0 );
                }
                else 
                {
                        GetShot(iShot).GetLevel(i).SetNumKeyFrames( (val + 0.30)>((int)(val + 0.30)+0.5)?(int)(val + 0.30)+1:(int)(val + 0.30) );

#ifdef DEBUG_FILES
			ftree << "(" << (val + 0.30) << ")";
		}
		ftree << "\t" << GetShot(iShot).GetLevel(i).GetNumKeyFrames();
	}
	ftree << endl;
#else
		}
	}
#endif

        for( i = 0; i < GetNumLevels(); i++)
                GetShot(iShot).GetLevel(i).AllocKeyFrameRefs();

        //initialize the finest level to original keyframes
        for( i=st; i<en; i++)
                SetKeyFrameIntoTree( iShot, 0, i-st, GetKeyFrame(i) );
}


//=============================================================================
//Lloyds quantization algorithm for the histogram vectors
//within one shot. All levels of the tree are generated.
void KeyFrameTree::IdentifyKeyFramesWithinShot(UINT iShot)
{
        int i,j;
        int level;
        float val,val1,minVal;
        int cntr,parti,nexti,partcntr;
        ULONG **meanhist;
        int iter,idum;
        int previous_parti;
        int saveCount;
        float mean;
        float saveLevMinusTwo;
        float saveLevMinusOne;

        Shot *thisShot = &GetShot(iShot);
        Level *thisLevel = 0;
        Level *prevLevel = 0;

        int *addFlag;
        addFlag = new int[m_numLevels];
        for( i = 0; i < m_numLevels; i++)
        {
                addFlag[i] = -1;
        }


#ifdef DEBUG_FILES
	ofstream fcluster;
	fcluster.open("ClusterFile.txt");
	assert(fcluster.is_open());
#endif

        //allocate and initialize meanhist
        if( thisShot->GetLevel(1).GetNumKeyFrames() > 0 )
        {
                meanhist = new ULONG* [thisShot->GetLevel(1).GetNumKeyFrames()];
                for(i=0; i< thisShot->GetLevel(1).GetNumKeyFrames(); i++)
                        meanhist[i] = new ULONG[m_histSeries->GetHistSize()];

                // To be used by delete statement at the end
                saveCount = thisShot->GetLevel(1).GetNumKeyFrames(); 

                //for each level
                for(level=1; level<m_numLevels; level++)
                {

                        thisLevel = &thisShot->GetLevel(level);
                        prevLevel = &thisShot->GetLevel(level - 1);

                        //if there is at least two keyframes to be allocated 
                        // to  this level 
                        if( thisLevel->GetNumKeyFrames() > 1 )
                        {
                                // Calculate initial partitions
                                // across the levels 
                                // The keyframes of previous levels indicate
                                // which partition of this level they belong to

                                cntr=0;
                                for( i=0; i < prevLevel->GetNumKeyFrames(); i++)
                                {
                                        // The following criterion may cause last 
                                        // partition to have less keyframes as 
                                        // the others  
                                        if( (i*thisLevel->GetNumKeyFrames()) >= 
                                                ((cntr+1)*prevLevel->GetNumKeyFrames()) )
                                        {
                                                cntr++;
                                        }
                                        prevLevel->SetKeyFramePartition(i, cntr);
                                }



                                //start loop
                                for( iter=0; iter<NUMITER; iter++)
                                {
                                        // find mean histogram (centroids)
                                        // of keyframes in partition number i. Original
                                        // keyframes are in previous level.  Each of these
                                        // previous keyframes has index of the partition in this level
                                        // Number of partitions is the number of keyframes at the 
                                        // current level. 

                                        for(i=0; i<thisLevel->GetNumKeyFrames(); i++)
                                        {
                                                {
                                                        int j,k;
                                                        int cntr=0;

                                                        for( k = 0; k < m_histSeries->GetHistSize(); k++)
                                                        {
                                                                meanhist[i][k]=0;
                                                        }

                                                        // len_ind is number of keyframes at previous level
                                                        for( j = 0; j < prevLevel->GetNumKeyFrames(); j++)
                                                        {
                                                                if( prevLevel->GetKeyFramePartition(j) == i )
                                                                {
                                                                        cntr++;
                                                                        for( k = 0; k < m_histSeries->GetHistSize(); k++)
                                                                        {
                                                                                meanhist[i][k] += m_histSeries->GetHistData( prevLevel->GetKeyFrameNum(j), k );
                                                                        }
                                                                }
                                                        }


                                                        // Calculate mean if cntr greater than 1
                                                        // No need to include case cntr = 1 here 
                                                        // since average readily available. 
                                                        if(cntr>1)
                                                        {
                                                                for(k=0;k<m_histSeries->GetHistSize();k++)
                                                                {
                                                                        meanhist[i][k] /= cntr;
                                                                }
                                                        }
                                                        else if(cntr==0)
                                                        {
                                                                std::cout<< "cntr=0 in findmeanhist! Exiting.\n"; 
                                                                exit(1);
                                                        }
                                                }
                                        }


                                        if(iter < NUMITER-1)
                                        {
                                                //change partitions (as recorded in previous level)

                                                // current partition index is parti
                                                parti=0; 
                                        	
                                                // partition being compared with 
                                                nexti=1; 

                                                // counter tracking number of keyframes 
                                                // in current partition (index equal to parti )
                                                partcntr=0;

                                                // Consider all keyframes at previous, finer 
                                                // level and make sure that we don't beyond 
                                                // the maximum number of keyframes 
                                                // allocated to current level 

                                                i = 0;
                                                while((i<prevLevel->GetNumKeyFrames()) &&
                                                          (nexti<thisLevel->GetNumKeyFrames()))
                                                {
                                                        // calculate difference between average
                                                        // histogram over current partition 
                                                        // and keyframe i in previous level
                                                        val = m_histSeries->GetDiffL2(meanhist[parti], prevLevel->GetKeyFrameNum(i));

                                                        // calculate difference between average
                                                        // histogram over next partition 
                                                        // and keyframe i in previous level
                                                        val1 = m_histSeries->GetDiffL2(meanhist[nexti], prevLevel->GetKeyFrameNum(i));


                                                        // Adjust partition boundary 
                                                        // according to distortion values 
                                                        // obtained above 

                                                        // If val >= val1: starting from i inclusive
                                                        // make keyframe belong  to the next 
                                                        // partition 
                                                        // If val < val1: take keyframe at index i
                                                        // and make it belong to the first 
                                                        // partition 
                                        	
                                                        if( (val >= val1) && (partcntr > 0) )
                                                        {

                                                                // partcntr indicates that 
                                                                // there is at least one keyframe 
                                                                // has been assigned to first 
                                                                // partition 
                                                                // Since test indicates that 
                                                                // keyframe i should belong 
                                                                // to nexti, we can safely 
                                                                // do partition re-assignment
                                                                // from keyframe i included


                                                                // Do not increment i 
                                                                // so next time around 
                                                                // in the while loop 
                                                                // we can start
                                                                // at beginning of next
                                                                // partition 
                                                                // This allows testing of 
                                                                // keyframe number i to see
                                                                // whether it belongs yet 
                                                                // to another partition 
                                                                // (When this happens it 
                                                                // triggers a new partition 
                                                                // starting at i+1. It is i+1
                                                                // and not i because partition
                                                                // nexti has to be at least one 
                                                                // keyframe long 
	

                                                                idum = i;

                                                                // Change partition affiliation 
                                                                // for all keyframes in this 
                                                                // partition (from i)
                                                                while( prevLevel->GetKeyFramePartition(idum) == parti )
                                                                {
                                                                        prevLevel->SetKeyFramePartition(idum++, nexti);
                                                                        if( idum == prevLevel->GetNumKeyFrames() )
                                                                        {
                                                                                break;
                                                                        }
                                                                }


                                                                // reset parti continuity counter
                                                                // for next partition to come
                                                                partcntr=0;

                                                                // Go deal with next partitions
                                                                // starting  at position i
                                                                parti++;
                                                                nexti++;
                                                        }
                                                        else if( (val >= val1) && (partcntr == 0) )
                                                        {
                                                                // There no keyframe in this
                                                                // partition yet since
                                                                // partcntr is equal to 0

                                                                // To guarantee that I have 
                                                                // at least one keyframe left 
                                                                // in previous partition
                                                                // pre-increment i 
                                                                idum=++i;

                                                                // Change partition affiliation 
                                                                while( prevLevel->GetKeyFramePartition(idum) == parti )
                                                                {
                                                                        prevLevel->SetKeyFramePartition(idum++, nexti);
                                                                        if( idum == prevLevel->GetNumKeyFrames() )
                                                                        {
                                                                                break;
                                                                        }
                                                                }

                                                                // counter is already reset 
                                                                // for next iteration so 
                                                                // need to reset it.

                                                                // Go deal with next partitions
                                                                // starting at position i+1
                                                                parti++;
                                                                nexti++;
                                                        } 
                                                        else if( (val < val1) && (i+1 == prevLevel->GetNumKeyFrames()) )
                                                        {
                                                                // don't re-assign because 
                                                                // we have reached last 
                                                                // keyframe in previous level
                                                                prevLevel->SetKeyFramePartition( i++, nexti++ );
                                                        }
                                                        else if( (val < val1) && (prevLevel->GetKeyFramePartition(i+1) > nexti) )
                                                        {
                                                                // leave at least one keyframe 
                                                                // in partition nexti
                                                                // so don't make it belong to 
                                                                // partition parti
                                                                partcntr=0;

                                                                // Go deal with next iteration 
                                                                // starting at position i
                                                                // since it belongs to partition
                                                                // nexti
                                                                // Therefore i is not incremented

                                                                parti++;
                                                                nexti++;
                                                        }
                                                        else if( (val < val1) && (prevLevel->GetKeyFramePartition(i+1) <= nexti) )
                                                        {
                                                                // merge with first partition 
                                                                // and increment i 
                                                                prevLevel->SetKeyFramePartition( i++, parti );

                                                                // record this extra frame 
                                                                // in first partition 
                                                                partcntr++;
                                                                // don't increment nexti and parti
                                                                // partition parti might still 
                                                                // be grown further
        	
                                                        }
                                                        else
                                                        {
                                                                std::cout << "Unexpected condition (1) in quantize(). Exiting\n";
                                                                exit(1);
                                                        }
                                                } // end of while loop 

                                        }//end of if(iter)
                                }//end of for(iter)



                                // Determine representative keyframes at this level

                                float cumulativeDisto;
                                val = 0.0;

                                // initialize save_i in case there is 
                                // only one partition 
                                int save_i = prevLevel->GetNumKeyFrames();

                                int flagFirstTime = 1;

                                // Identify length of first partition
                                for(i=0; i<prevLevel->GetNumKeyFrames(); i++)
                                {
                                        if( prevLevel->GetKeyFramePartition(i) > 0 )
                                        {
                                                save_i = i;
                                                break;
                                        }
                                }

                        	

                                // Calculate most representative keyframe 
                                // in first partition 
                                for( i=0; i < save_i; i++)
                                {

                                        // Calculate distance from keyframe to average
                                        cumulativeDisto = m_histSeries->GetDiffL2(meanhist[0], prevLevel->GetKeyFrameNum(i));

                                        // Record minimum distortion achieved 
                                        // if lower value is found 
                                        // or if it is first time cumulative distortion 
                                        // number is available. 
                                        if( (flagFirstTime == 1) || (cumulativeDisto < val) )
                                        {
                                                val = cumulativeDisto;
                                                thisLevel->SetKeyFrame( 0, prevLevel->GetKeyFrame(i) );
                                                flagFirstTime = 0;
                                        }	
                                }

                                                	
                                // Set previous partition to be 0 
                                // since we have just finished processing it.
                                previous_parti = 0;

                                // initialize mean to minimum distortion value 
                                mean = val;


                                // Note: Here val contains minimum distortion found within 
                                // first partition at level-1. 
	
                                // Parse every remaining keyframe in previous level 
                                // Note: We know that there are at least 2 keyframes 
                                // at level (level-1) 
                                // since keydim[level] > 1 (see if above) 

                                for(i=save_i; i<prevLevel->GetNumKeyFrames(); i++)
                                {
                                        // Get partition to which keyframe belongs to 
                                        parti = prevLevel->GetKeyFramePartition(i);
                                        if( parti <= 0 )
                                        {
                                                std::cerr << "keytree: Should not happen " << std::endl;
                                                exit( -1 );
                                        }

                                        // record last distortion value 
                                        // so average of all distortions can be 
                                        // determined later on. 

                                        if( previous_parti != parti )
                                        {
                                                mean += val;

                                                // By doing the following, we will 
                                                // end up with the val previous to last
                                                saveLevMinusTwo = val;

                                                // Reset val. This has the benefit of 
                                                // triggering the test below 
                                                val = 0;
                                                previous_parti = parti;
                                        }

                                        // calculate difference between 
                                        // previous keyframe at this level 
                                        // and keyframe in previous level.

                                        val1 = m_histSeries->GetDiffL2(thisLevel->GetKeyFrameNum(parti-1), prevLevel->GetKeyFrameNum(i));
                                        // Bring this keyframe in from previous level
                                        // if it is more distinct than the ones 
                                        // previously found. 
                                        // Update val as new distance to beat 

                                        if( val1 >= val )
                                        {
                                                thisLevel->SetKeyFrame( parti, prevLevel->GetKeyFrame(i) );
                                                val = val1;
                                        }
                                }

                                // Account for last distortion  
                                mean += val;
                                saveLevMinusOne = val;


                                float pbest=0; 

                                // Add a flag to signal that the last keyframe 
                                // from previous level can be appended at the end 
                                // of this summmary ( see comments below )

                                addFlag[level] = -1;

                                if( thisLevel->GetNumKeyFrames() < prevLevel->GetNumKeyFrames()-1 )
                                {


                                        // Signal that condition keydim[level]<keydim[level-1]-1 
                                        // has been verified
                                        addFlag[level] = 0;

                        	
                                        // if necessary, add a keyframe at the end to take care 
                                        // of redundancies
                                        // compare last keyframe number at previous level and 
                                        // current level

                                        if( thisLevel->GetLastKeyFrameNum() != prevLevel->GetLastKeyFrameNum() )
                                        {

#ifdef DEBUG_FILES
						m_fkey << " >>>>>><<<<<<>>>>><<<< Appending at level " << level  << std::endl;
						m_fkey << " >>>>>><<<<<<>>>>><<<< tree[level][keydim[level]-1] " <<  thisLevel->GetLastKeyFrameNum()  << std::endl;
						m_fkey << " >>>>>><<<<<<>>>>><<<< prevLevel->GetLastKeyFrameNum() " <<  prevLevel->GetLastKeyFrameNum()  << std::endl;
#endif

                                                // overwrite partition information 
                                                // in last entry of previous level 


                                                // append last keyframe at previous level to 
                                                // end of the summary at this level. 
                                                // adjust dimension NumKeyFrames accordingly. 

                                                thisLevel->AllocNumKeyFrames( thisLevel->GetNumKeyFrames() + 1 );

                                                prevLevel->AllocNumKeyFrames( prevLevel->GetNumKeyFrames() + 1 );
                                        	
                                                // Signal that keyframe has been appended 
                                                addFlag[level] = 1;

                                                prevLevel->SetKeyFramePartition( prevLevel->GetNumKeyFrames()-1, thisLevel->GetNumKeyFrames()-1 );

                                                thisLevel->SetKeyFrame( thisLevel->GetNumKeyFrames()-1, prevLevel->GetKeyFrame( prevLevel->GetNumKeyFrames()-2 ) );

#ifdef DEBUG_FILES
						m_fkey << "keydim[lv] = " << thisLevel->GetNumKeyFrames()-1 << std::endl;
						m_fkey << " ======>  tree[lv][keydim[lv]] = " << thisLevel->GetKeyFrameNum( thisLevel->GetNumKeyFrames()-1 ) << std::endl;
#endif


                                                prevLevel->SetKeyFrame( prevLevel->GetNumKeyFrames() - 1, 
                                                        prevLevel->GetKeyFrame( prevLevel->GetNumKeyFrames() - 2 ) );
                                        }


                                        // adjust keyframe selection at partition keydim[level]-2
                                        // that is the partition before last
                                        // Here, and because of the previous block of processing, 
                                        // we know that the last keyframe corresponding to 
                                        // last partition is ALWAYS the last keyframe found 
                                        // at the previous level 
                                        // This step allows optimization of the keyframe 
                                        // with respect to last keyframe and preceding keyframe


                                        if( thisLevel->GetNumKeyFrames() > 2 )
                                        {
                                                for(i=0; i<prevLevel->GetNumKeyFrames(); i++)
                                                {
                                                        if( prevLevel->GetKeyFramePartition(i) == thisLevel->GetNumKeyFrames() - 2 )
                                                        {
                                                                // calculate distortion between 
                                                                // keyframe in previous level
                                                                // and preceding and following 
                                                                // representative keyframe
                                                                // at this level 

                                                                // Difference with last keyframe from 
                                                                // previous level (always true because
                                                                // of the possible appending performed above )
                                                                val = m_histSeries->GetDiffL2(thisLevel->GetLastKeyFrameNum(),prevLevel->GetKeyFrameNum(i));
                                                                // Difference with previous keyframe 
                                                                // at this level 
                                                                val1 = m_histSeries->GetDiffL2(thisLevel->GetKeyFrameNum( thisLevel->GetNumKeyFrames()-3 ), 
                                                                                                                          prevLevel->GetKeyFrameNum(i));
                                                                // take minimum 
                                                                // in effect calculating 
                                                                // a max min solution here
                                                                minVal = val < val1 ? val : val1;
                                                                if( minVal > pbest )
                                                                {
                                                                        pbest = minVal;
                                                                        // replace representative keyframe with the keyframe from previous level 
                                                                        // reason: It is the most distant to either preceding or next keyframe 
                                                                        thisLevel->SetKeyFrame( thisLevel->GetNumKeyFrames()-2, prevLevel->GetKeyFrame(i) );
                                                                }
                                                        }
                                                }
                                        }
                                }


                                // pruning routine to remove ISOLATED keyframes 
                                // (partition length equal to one) 
                                // which are too close from neighboring keyframes

                                int *pc_dum = new int[prevLevel->GetNumKeyFrames()];
                                pbest=0;


                                // ATTENTION
                                // Truly speaking the value of mean should 
                                // have been modified to take into account new 
                                // distortion value obtained from maxmin calculation
                                // which requires keeping a record of orginal min 
                                // distortion at last and previous to last keyframe 
                                // at this level (depending on whether appending has 
                                // occurred or not, one of these two values is used )



                                if( ((thisLevel->GetNumKeyFrames() > 2)  && (addFlag[level] == 1)) || 
                                    ((thisLevel->GetNumKeyFrames() > 1 ) && (addFlag[level] != 1)) )
                                {

                                        if( addFlag[level] == 0 )
                                        {
                                                // Last keyframe at this level coincide 
                                                // with last keyframe at previous level.
                                                // Consequently, no new keyframe was appended 
                                                // update distortion value to account for 
                                                // selection of new keyframe before last 
                                                mean -= saveLevMinusTwo;
                                                mean += pbest;
                                                mean /= thisLevel->GetNumKeyFrames();
                                        }
                                        else if( addFlag[level] == 1 )
                                        {
                                                // last keyframe has been artifically added 
                                                // and should not help reduce mean value 
                                                // because it has no distortion associated with it 
                                                // update distortion value to account for 
                                                // selection of new last keyframe 
                                                mean -= saveLevMinusOne;
                                                mean += pbest;
                                                mean /= (thisLevel->GetNumKeyFrames() - 1);
                                        }
                                        else
                                        {
                                                // addFlag is equal to -1
                                                mean /= thisLevel->GetNumKeyFrames();
                                        }

                                        // pc_dum is 0 for redundant; 1 for non-redundant

                                        // scan all keyframes available at previous level
                                        for( i = 0; i < prevLevel->GetNumKeyFrames(); i++)
                                        {
                                                if( i == 0 )
                                                {
                                                        // default is not redundant
                                                        pc_dum[i] = 1;
                                                        if( prevLevel->GetKeyFramePartition(i) != prevLevel->GetKeyFramePartition(i+1) )
                                                        {
                                                                // calculate how far from each other current and next keyframes are 
                                                                val = m_histSeries->GetDiffL2( thisLevel->GetKeyFrameNum( prevLevel->GetKeyFramePartition(i) ),
                                                                                                                          thisLevel->GetKeyFrameNum( prevLevel->GetKeyFramePartition(i+1) ) );
                                                                // redundant if too close 
                                                                if( val <= (mean*THETA) )
                                                                {
                                                                        pc_dum[0] = 0;
                                                                }
                                                        }
                                                }
                                                else if( i == prevLevel->GetNumKeyFrames()-1 )
                                                {
                                                        // default is not redundant 
                                                        pc_dum[i] = 1;
                                                        if( prevLevel->GetKeyFramePartition(i) != prevLevel->GetKeyFramePartition(i-1) )
                                                        {
                                                                // calculate how far from each other current and previous keyframes are 
                                                                val = m_histSeries->GetDiffL2(thisLevel->GetKeyFrameNum( prevLevel->GetKeyFramePartition(i) ),
                                                                                                                          thisLevel->GetKeyFrameNum( prevLevel->GetKeyFramePartition(i-1) ) );
                                                                // redundant if too close 
                                                                if( val <= (mean*THETA) )
                                                                {
                                                                        pc_dum[i] = 0;
                                                                }
                                                        }
                                                }
                                                else 
                                                {
                                                        // default is not redundant 
                                                        pc_dum[i] = 1;
                                                        if( (prevLevel->GetKeyFramePartition(i) != prevLevel->GetKeyFramePartition(i-1)) &&
                                                                        (prevLevel->GetKeyFramePartition(i) != prevLevel->GetKeyFramePartition(i+1)) )
                                                        {
                                                                        val = m_histSeries->GetDiffL2(thisLevel->GetKeyFrameNum( prevLevel->GetKeyFramePartition(i) ),
                                                                                                                                  thisLevel->GetKeyFrameNum( prevLevel->GetKeyFramePartition(i-1) ) );

                                                                        val1 = m_histSeries->GetDiffL2(thisLevel->GetKeyFrameNum( prevLevel->GetKeyFramePartition(i) ),
                                                                                                                                   thisLevel->GetKeyFrameNum( prevLevel->GetKeyFramePartition(i+1) ) );
                                                                if( ((val+val1)*0.5) <= (mean*THETA) )
                                                                {
                                                                        pc_dum[i]=0;
                                                                }
                                                        }
                                                }
                                        }

#ifdef DEBUG_FILES
			for( int ff = 0; ff < prevLevel->GetNumKeyFrames(); ff++)
			{
				m_fkey << " lv, ff = " << level << " " << ff << " pc[lv][ff] = " << prevLevel->GetKeyFramePartition(ff) << endl;
			}
#endif
                                        //change partitions and level's keyframes

                                        // cntr is counter for new number of partitions 
                                        // and therefore new number of keyframes 
                                        // at this level 
                                        cntr = 0;
                                        int pcntr = 0;
                                        for( i=1; i<prevLevel->GetNumKeyFrames(); i++)
                                        {
                                                // absorbe (remove) keyframe 0 and 
                                                // remap all keyframes in partition 1 
                                                // to partition 0
                                                if( (prevLevel->GetKeyFramePartition(i)==1) && (pc_dum[0]==0) )
                                                {
                                                        // initialize original index being 
                                                        // considered to one
                                                        pcntr = 1;
                                                        prevLevel->SetKeyFramePartition( i, 0 );
                                                }
                                                else if( (prevLevel->GetKeyFramePartition(i)==1) && (pc_dum[0]==1) )
                                                {
                                                        // partition being considered has 
                                                        // original index equal to 1
                                                        pcntr = 1;
                                                        // account for partition 0 
                                                        cntr = 1;
                                                }
                                                else if( (prevLevel->GetKeyFramePartition(i)!=pcntr) && (pc_dum[i]==0) )
                                                {
                                                        // record new original index being considered
                                                        pcntr = prevLevel->GetKeyFramePartition(i);
                                                        // absorb in current new partition
                                                        prevLevel->SetKeyFramePartition( i, cntr );

#ifdef DEBUG_FILES
							m_fkey << "PRUNING 0 " << pcntr << " " << cntr << endl;
#endif

                                                }
                                                else if( (prevLevel->GetKeyFramePartition(i)!=pcntr) && (pc_dum[i] == 1) )
                                                {

                                                        // record new original index being considered
                                                        pcntr = prevLevel->GetKeyFramePartition(i);
                                                        // give next index available to this 
                                                        // partition
                                                        prevLevel->SetKeyFramePartition( i, ++cntr );

#ifdef DEBUG_FILES
							m_fkey << "PRUNING 1 " << pcntr << " " << cntr << endl;
#endif

                                                        // Time to re-calculate best 
                                                        // representative keyframe 
                                                        // over previous new partition
                                                        // now that we have reached the 
                                                        // end of it 
                                                        pbest = 0;
                                                        if( cntr > 1 )
                                                        {
                                                                // go over what has been done 
                                                                // so far 
                                                                for( j=0; j<i; j++)
                                                                {
                                        	
                                                                        // look for previous partition 
                                                                        if( prevLevel->GetKeyFramePartition(j) == (cntr-1) )
                                                                        {
                                                                                // look for keyframe which is farthest from previous keyframe at this level 
                                                                                val = m_histSeries->GetDiffL2( thisLevel->GetKeyFrameNum(cntr-2), prevLevel->GetKeyFrameNum(j) ); 
                                                                                if( val > pbest )
                                                                                {
                                                                                        pbest = val;
                                                                                        //tree[level][cntr-1] = tree[level-1][j];
                                                                                        thisLevel->SetKeyFrame( cntr-1, prevLevel->GetKeyFrame(j) );
                                                                                }
                                                                        }
                                                                }
                                                        }
                                                }
                                                else 
                                                {
                                                        prevLevel->SetKeyFramePartition( 1, cntr );
                                                }
                                        }
                                        // Do it in a fashion which is consistent with what 
                                        // has been done for other previous partitions 

                                        pbest = 0;
                                        if( cntr > 0 ) 
                                        {
                                                // go over what has been done 
                                                // so far 
                                                for( j=0; j< prevLevel->GetNumKeyFrames(); j++)
                                                {
                        	
                                                        // look for previous partition 
                                                        if( prevLevel->GetKeyFramePartition(j) == cntr )
                                                        {
                                                                // look for keyframe which is farthest from previous keyframe at this level 
                                                                val = m_histSeries->GetDiffL2( thisLevel->GetKeyFrameNum(cntr-1), prevLevel->GetKeyFrameNum(j) ); 
                                                                if(val>pbest)
                                                                {
                                                                        pbest = val;
                                                                        thisLevel->SetKeyFrame( cntr, prevLevel->GetKeyFrame(j) );
                                                                }
                                                        }
                                                }
                                        }
                                        // Find closest to average.
                                        else 
                                        {
                                                for( j=0; j< prevLevel->GetNumKeyFrames(); j++)
                                                {
                                                        val = m_histSeries->GetDiffL2(meanhist[0], prevLevel->GetKeyFrameNum(j)); 
                                                        if(val>pbest)
                                                        {
                                                                pbest = val;
                                                                thisLevel->SetKeyFrame( 0, prevLevel->GetKeyFrame(j) );
                                                        }
                                                }
                                        }

                                        thisLevel->AllocNumKeyFrames( cntr + 1 );

#ifdef DEBUG_FILES
					m_fkey << "========= NEW DIM = " << thisLevel->GetNumKeyFrames() << endl; 
#endif

                                }

                                delete [] pc_dum;

                                if( addFlag[level] == 1 )
                                {
                                        prevLevel->AllocNumKeyFrames( prevLevel->GetNumKeyFrames() - 1 );
                                }

                        }//end of if(keyt)
                        else if(thisLevel->GetNumKeyFrames() == 1)
                        {
                                for( i=0; i<prevLevel->GetNumKeyFrames(); i++)
                                {
                                        prevLevel->SetKeyFramePartition( i, 0 );
                                }

                                // Find Mean Hist
                                {
                                        int j,k;
                                        int cntr=0;

                                        for( k = 0; k < m_histSeries->GetHistSize(); k++)
                                        {
                                                meanhist[0][k]=0;
                                        }

                                        // len_ind is number of keyframes at previous level
                                        for( j = 0; j < prevLevel->GetNumKeyFrames(); j++)
                                        {
                                                if( prevLevel->GetKeyFramePartition(j) == 0 )
                                                {
                                                        cntr++;
                                                        for( k = 0; k < m_histSeries->GetHistSize(); k++)
                                                        {
                                                                meanhist[0][k] += m_histSeries->GetHistData( prevLevel->GetKeyFrameNum(j), k );
                                                        }
                                                }
                                        }


                                        // Calculate mean if cntr greater than 1
                                        // No need to include case cntr = 1 here 
                                        // since average readily available. 
                                        if(cntr>1)
                                        {
                                                for(k=0;k<m_histSeries->GetHistSize();k++)
                                                {
                                                        meanhist[0][k] /= cntr;
                                                }
                                        }
                                        else if(cntr==0)
                                        {
                                                std::cout<< "cntr=0 in findmeanhist! Exiting.\n"; 
                                                exit(1);
                                        }
                                }




                                // Initialize search for best representative keyframe 
                                val = m_histSeries->GetDiffL2(meanhist[0], prevLevel->GetKeyFrameNum(0));

                                thisLevel->SetKeyFrame( 0, prevLevel->GetKeyFrame(0) );
                                for( i=1; i<prevLevel->GetNumKeyFrames(); i++)
                                {
                                        val1 = m_histSeries->GetDiffL2(meanhist[0], prevLevel->GetKeyFrameNum(i));
                                        if( val1 < val )
                                        {
                                                thisLevel->SetKeyFrame( 0, prevLevel->GetKeyFrame(i) );
                                                val = val1;
                                        }
                                }
                        }
                        else //if keyt==0 
                        {
#ifdef DEBUG_FILES
				m_fkey << "No keyframes at and beyond level: " << level << endl;
#endif
                                //level = levels;
                                break;
                        }

#ifdef DEBUG_FILES
			m_fkey << "keydim[level], keydim[level-1] = " << thisLevel->GetNumKeyFrames() << "  " << prevLevel->GetNumKeyFrames() << endl;
			for( int f = 0; f < prevLevel->GetNumKeyFrames(); f++)
			{
				m_fkey << " lv, f = " << level << " " << f << " pc[lv][f] = " << prevLevel->GetKeyFramePartition(f) << endl;
			}
			for( f = 0; f < thisLevel->GetNumKeyFrames(); f++)
			{
				m_fkey << " f = " << f << " tree[lv][f] = " << thisLevel->GetKeyFrameNum(f) << endl;
			}
			m_fkey << endl << endl;
#endif

                }

                for( i=0; i<saveCount; i++)
                {
                        delete [] meanhist[i];
                }
                delete [] meanhist;


        }

#ifdef DEBUG_FILES
	fcluster.close();
#endif

        delete [] addFlag;

}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//                                                      DOM BUILDING SUBSYSTEMS
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SummarizationDOM::SummarizationDOM(KeyFrameTree *keyFrameTree) :
m_keyFrameTree(keyFrameTree)
// m_domDoc(0)  // Default constructor produces null object
{
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SummarizationDOM::~SummarizationDOM()
{
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GenericDS SummarizationDOM::CreateRoot(GenericDSInterfaceABC *doc)	// Need to check that doc is the root document
{	
	// <Program>
	GenericDS prog = doc->CreateChild( "Mpeg7" );
	prog.SetAttribute("xmlns",
		"urn:mpeg:mpeg7:schema:2001");
	prog.SetAttribute("xmlns:xsi",
		"http://www.w3.org/2001/XMLSchema-instance");
	prog.SetAttribute("xsi:schemaLocation",
		"urn:mpeg:mpeg7:schema:2001 .\\Mpeg7-2001.xsd");

	CreateMediaInfo( prog );
	CreateSummarization( prog );

	return prog;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GenericDS SummarizationDOM::CreateMediaInfo(GenericDS parent)
{
	char *pTmp, uri[MAXMEDIANAMELEN] = {0};
	int  start, end = 0;


	// Extract the filename and the starting/ending frames if present
	if (m_keyFrameTree->GetMediaName())
	{
		strcpy(uri,m_keyFrameTree->GetMediaName());
		if ((pTmp = strchr(uri,'[')))
		{
			*pTmp++ = '\0';
			start = offset = atoi(pTmp);
			if ((pTmp = strchr(pTmp,'-')))
			{
				end = atoi(pTmp + 1);
			}
		}
	}


	//	<Description xsi:type="MediaDescriptionType">
	GenericDS desc	= CreateElemWithAttrib(parent, "Description", NULL,
							"xsi:type", "MediaDescriptionType");

	//		<MediaInformation>
	GenericDS mediaInf	= CreateElem(desc, "MediaInformation");

	//			<MediaProfile>
	GenericDS mediaProf	= CreateElem(mediaInf, "MediaProfile");

	//				<MediaInstance>
	GenericDS mediaInst	= CreateElem(mediaProf, "MediaInstance");

	//					<InstanceIdentifier>0</InstanceIdentifier>
	GenericDS instId	= CreateElem(mediaInst, "InstanceIdentifier", "0");

	//					<MediaLocator>
	GenericDS mediaLoc	= CreateElem(mediaInst, "MediaLocator");

	//						<MediaUri>filename</MediaUri>
	GenericDS mediaUri	= CreateElem(mediaLoc, "MediaUri", uri);

	return desc;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GenericDS SummarizationDOM::CreateSummarization(GenericDS parent)
{
	int		level, numLevels, shot, numSegments = 0;
	char levelName[100], levelId[10];

	// Check assumptions
	assert(LEVEL_HIERARCHY == 0);               // HierarchicalSummary is flat (and hence independent)
	assert(INCLUDE_VIDEO_SEGMENT == 2); // Create a VideoSegment per ImageLocator (keyframe)
	assert(VIDEO_IMAGE_OFFSET == 0);    // VideoSegment begins at ImageLocator

	// Determine number of levels
	numLevels = m_keyFrameTree->GetNumLevels();
	while (numLevels)
	{
		// Determine number of segments across all shots
		for( shot = 0; shot < m_keyFrameTree->GetNumShots(); shot ++)
			numSegments += m_keyFrameTree->GetNumKeyFramesPerShotLevel(shot, numLevels - 1);
		if (numSegments)
			break;
		else
			numLevels--;
	}


	//	<Description xsi:type="SummaryDescriptionType">
	GenericDS desc	= CreateElemWithAttrib(parent, "Description", NULL,
							"xsi:type", "SummaryDescriptionType");

	//		<Summarization mediaTimeUnit="PT1N25F">
	GenericDS summarization	= CreateElemWithAttrib(desc, "Summarization", NULL,
							"mediaTimeUnit", "PT1N25F");

	//			<Summary  xsi:type="HierarchicalSummaryType" hierarchy="independent" components="keyVideoClips keyFrames">
	GenericDS summary	= CreateElemWithAttrib(summarization, "Summary", NULL,
							"xsi:type", "HierarchicalSummaryType",
							"hierarchy", "independent",
							"components", "keyVideoClips keyFrames");

	//				<Name>HierarchicalKeyFrame Summary</Name>
	GenericDS name	= CreateElem(summary, "Name", "HierarchicalKeyFrame Summary");

	//				<SummaryThemeList>
	GenericDS themeList	= CreateElem(summary, "SummaryThemeList");


	// Create children SummaryThemes
	for( level = 0; level < numLevels; level ++)
	{
        // Name the level Fine, Medium, Coarse, Extra Coarse, ...
        if (level == 0)
                strcpy(levelName, "Fine");
        else if ((level == 1) && (numLevels > 2))
                strcpy(levelName, "Medium");
        else
        {
                levelName[0] = '\0';
                for( int l = 2; l < level; l++ )
                        strncat(levelName, "Extra ", sizeof levelName - strlen(levelName) - 1);
                strncat(levelName, "Coarse", sizeof levelName - strlen(levelName) - 1);
        }
		sprintf(levelId, "E%d", level + 1);
		
	//					<SummaryTheme id="E0">Fine</SummaryTheme>
		CreateElemWithAttrib(themeList, "SummaryTheme", levelName, "id", levelId);
	}

	// Create children SummarySegmentGroup
	for( level = 0; level < numLevels; level ++)
	{
		CreateSummarySegmentGroup(summary, level);
	}

	return desc;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GenericDS SummarizationDOM::CreateSummarySegmentGroup(GenericDS parent, int level)
{
	char levelName[100], levelNum[10], levelId[10], segNum[10];
	UINT numSegments = 0, shot;
	ULONG segment;

	// Name the level Fine, Medium, Coarse, Extra Coarse, ...
	if (level == 0)
		strcpy(levelName, "Fine");
	else if ((level == 1) && (m_keyFrameTree->GetNumLevels() > 2))
		strcpy(levelName, "Medium");
	else
	{
		levelName[0] = '\0';
		for( int l = 2; l < level; l++ )
			strncat(levelName, "Extra ", sizeof levelName - strlen(levelName) - 1);
		strncat(levelName, "Coarse", sizeof levelName - strlen(levelName) - 1);
	}
	sprintf(levelNum, "%d", level + 1);
	sprintf(levelId, "E%d", level + 1);

	// Determine number of segments across all shots
	for( shot = 0; shot < m_keyFrameTree->GetNumShots(); shot ++)
		numSegments += m_keyFrameTree->GetNumKeyFramesPerShotLevel(shot, level);
	sprintf(segNum, "%d", numSegments);


	//<SummarySegmentGroup level="0" numOfKeyFrames="100" themeIDs="E0">
	GenericDS segGroup = CreateElemWithAttrib(parent,
						  "SummarySegmentGroup", NULL,
						  "numOfKeyFrames", segNum,
						  "themeIds", levelId);

	//					<Name>Fine</Name>
	CreateElem(segGroup, "Name", levelName);


	// Create children SummarySegment
	for( segment = 0; segment < numSegments; segment ++ )
	{
		CreateSummarySegment(segGroup, level, segment);
	}

	return segGroup;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GenericDS SummarizationDOM::CreateSummarySegment(GenericDS parent, int level, ULONG segment)
{
	int shot = 0, numPrevShotSegments = 0;
	ULONG nextShotStart = 0;
	ULONG keyFrame, nextKeyFrame;

	// Find the shot containing this keyframe
	while( numPrevShotSegments + m_keyFrameTree->GetNumKeyFramesPerShotLevel(shot, level) <= segment )
	{
		numPrevShotSegments += m_keyFrameTree->GetNumKeyFramesPerShotLevel(shot, level);
		shot ++;
	}

	// Adjust segment, relative to start of this shot
	segment -= numPrevShotSegments;
	keyFrame = m_keyFrameTree->GetKeyFrameNum(shot, level, segment);

	// Find the end of this shot
	if ( shot + 1 < m_keyFrameTree->GetNumShots() )
		nextShotStart = m_keyFrameTree->GetShotBoundary( shot + 1 );
	else
		nextShotStart = m_keyFrameTree->GetNumFrames();

	// Use the nextKeyFrame as the end of this VideoSegment
	if ( segment + 1 < m_keyFrameTree->GetNumKeyFramesPerShotLevel(shot, level) )
		nextKeyFrame = m_keyFrameTree->GetKeyFrameNum(shot, level, segment + 1);
	else
		nextKeyFrame = nextShotStart - 1;

	// Don't allow the VideoSegment to exceed the shot (take into account overlap)
	if ( nextKeyFrame > nextShotStart - 1 )
		nextKeyFrame = nextShotStart - 1;


	//					<SummarySegment>
	GenericDS sumrySeg	= CreateElem(parent, "SummarySegment");

	//						<KeyVideoClip>
	GenericDS keyVidClip	= CreateElem(sumrySeg, "KeyVideoClip");

	//							<MediaTime>
	GenericDS keyVidTime = CreateElem(keyVidClip, "MediaTime");

	//								<MediaRelIncrTimePoint>7</MediaRelIncrTimePoint>
	GenericDS vidTimePoint = CreateElem(keyVidTime, "MediaRelIncrTimePoint", keyFrame + offset);

	//								<MediaIncrDuration>2</MediaIncrDuration>
	GenericDS vidDuration = CreateElem(keyVidTime, "MediaIncrDuration", nextKeyFrame - keyFrame);

	//						<KeyFrame>
	GenericDS keyFrm	= CreateElem(sumrySeg, "KeyFrame");

	//							<MediaRelIncrTimePoint>8</MediaRelIncrTimePoint>
	GenericDS frmTimePoint = CreateElem(keyFrm, "MediaRelIncrTimePoint", keyFrame + offset);


	return sumrySeg;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GenericDS SummarizationDOM::CreateElem(GenericDS parent, const char *tag, const char *content)
{
        GenericDS elem;

	elem = parent.CreateChild(tag);
        if (content)
        {
	  elem.SetValue(content);
        }

        return elem;
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GenericDS SummarizationDOM::CreateElem(GenericDS parent, const char *tag, const long val, const long val2)
{
        char content[100];

        if ( val2 == LONG_MAX )
                sprintf( content, "%ld", val );
        else
                sprintf( content, "%ld %ld", val, val2 );

        return CreateElem(parent, tag, content);
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
GenericDS SummarizationDOM::CreateElemWithAttrib(GenericDS parent, 
												 const char *tag, const char *content, 
												 const char *attr,const char *value,
												 const char *attr2,const char *value2,
												 const char *attr3,const char *value3)
{        GenericDS elem;


        elem = CreateElem(parent, tag, content);

		elem.SetAttribute(attr, value);

        if (attr2)
		{
			elem.SetAttribute(attr2, value2);

			if (attr3)
				elem.SetAttribute(attr3, value3);
		}

        return elem;
}
