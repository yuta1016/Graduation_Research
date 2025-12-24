////////////////////////////////////////////////////////////////////////
//
// HierarchicalKeyFrameExtraction.h
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


#ifndef HIERARCHICAL_KEY_FRAME_H
#define HIERARCHICAL_KEY_FRAME_H

#include<fstream>  
#include<assert.h>
#include "BasicArchitecture/XMInterfaces.h"               /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                  /* Naming Convention */
#include "DescriptionSchemes/DescriptionSchemes.h"
#include "momusys.h"
#include "intra.h"

#include "ExtractionUtilities/ExtractionUtilities.h"
#include "CodingSchemes/CodingSchemes.h"          /* Naming Convention */
#include "Encoders/Encoders.h"               /* Naming Convention */
#include "Media/Media.h"                  /* Naming Convention */
#include <climits>


typedef unsigned int UINT;
typedef unsigned long ULONG;


namespace XM
{

//----------------------------------------------------------------------------
char *StripFrameRange( char *buf, const char *fileName, ULONG &start, ULONG &end );
char *BuildFileName( char *buf, const char *dir, const char *srcFile, const char *ext );
void TimeStamp(const char* text = NULL);

 class Histogram;
 long GetDiffL2(Histogram &a, Histogram &b);
 long GetDiffL2(ULONG *a, Histogram &b);

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
//                                                      KEY FRAME EXTRACTION SUBSYSTEMS
//
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

std::ostream & operator << (std::ostream &out, Histogram &hist);
std::istream & operator >> (std::istream &in, Histogram &hist);
long GetDiff(Histogram &a, Histogram &b);


// Forward Declarations:
class HierarchicalKeyFrameExtractionTool;

//----------------------------------------------------------------------------
class HierarchicalKeyFrameExtractionInterfaceABC: public I_InterfaceABC
{
public:
        HierarchicalKeyFrameExtractionInterfaceABC();
        virtual ~HierarchicalKeyFrameExtractionInterfaceABC() {};

	virtual void destroy(void) = 0;

        virtual int SetSourceMedia(MomVop *media) = 0;
        virtual int SetDescriptorInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0;
        virtual GenericDSInterfaceABC * GetDescriptorInterface() = 0;

        virtual unsigned long InitExtracting(char *mediaName) = 0;
        virtual unsigned long StartExtracting(void) = 0;
        virtual unsigned long PostExtraction(void) = 0;


        static const UUID myID;
};

//----------------------------------------------------------------------------
class HierarchicalKeyFrameExtractionInterface: 
  public HierarchicalKeyFrameExtractionInterfaceABC
{
public:
        HierarchicalKeyFrameExtractionInterface(
          HierarchicalKeyFrameExtractionTool* aTool);
        virtual ~HierarchicalKeyFrameExtractionInterface();
	
        virtual const UUID& GetInterfaceID(void);
        virtual const char *GetName(void);

	virtual void destroy(void);

        virtual int SetSourceMedia(MomVop *media);
        virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *aGenericDSInterface);
        virtual GenericDSInterfaceABC * GetDescriptorInterface();

        virtual unsigned long InitExtracting(char *mediaName);
        virtual unsigned long StartExtracting(void);
        virtual unsigned long PostExtraction(void);

        static const UUID myID;
        static const char *myName;

private:
        HierarchicalKeyFrameExtractionTool *m_ExtractionTool;
};


// Forward Declarations:
class HistogramSeries;

//----------------------------------------------------------------------------
class HierarchicalKeyFrameExtractionTool: public DescriptorExtractor
{
friend class HierarchicalKeyFrameExtractionInterface;
public:
        HierarchicalKeyFrameExtractionTool();
        HierarchicalKeyFrameExtractionTool(GenericDSInterface *aDescription);

        virtual const UUID& GetObjectID(void);
        virtual const char *GetName(void);

        virtual bool SupportsPush(void);
        virtual bool SupportsPull(void);

        virtual int SetSourceMedia(MomVop *media);
        virtual int SetDescriptorInterface(GenericDSInterfaceABC
					   *aGenericDSInterfaceABC);
        virtual GenericDSInterfaceABC * GetDescriptorInterface();
	
        virtual unsigned long InitExtracting(char *mediaName);
        virtual unsigned long StartExtracting(void);
        virtual unsigned long PostExtraction(void);

        virtual HierarchicalKeyFrameExtractionInterfaceABC *GetInterface(void);

        // access is allowed only by class factories for this
        // object.  This avoids having to duplicate the
        // ID definition in multiple locations.  In the future, we may
        // have to do this.  PLEASE DO NOT USE THESE UNLESS YOU ARE 
        // IMPLEMENTING A CLASS FACTORY GENERATING THIS OBJECT
        static const UUID myID;
        static const char * myName;
private:
        virtual ~HierarchicalKeyFrameExtractionTool();

        HierarchicalKeyFrameExtractionInterface m_Interface;
        GenericDSInterfaceABC *m_DSInterface;

        MomVop  *m_media;
        HistogramSeries *m_histSeries;	

	// Assumptions and simplifications
	static const UINT MIN_SHOT;              // Minimum number of frames per shot
	static const UINT NUM_LEVELS;            // Number of HighlightLevels
	                                         // in keyFrame
	                                         // HierarchicalSummary
	static const UINT RATIO_LEVELS;          // Ratio of number of
	                                         // HighlightSegments between
	                                         // HighlightLevels
	static const double RATIO_KEYFRAMES;     // Ratio of number of
	                                         // keyframes to the total
	                                         // frames
	static const UINT SUB_SAMPLING;          // Sub-sampling during color
	                                         // histogram extraction
	static const ColorSpaceType COLOR_SPACE; // Images are converted to
	                                         // YUV before accumulating
	                                         // histograms
	static const UINT UNIVARIATE_HISTOGRAM;  // Each component of color
	                                         // space has separate
	                                         // histogram bins
	static const UINT NUM_BINS_1;            // Number of bins for first
	                                         // color space axis (Y)
	static const UINT NUM_BINS_2;            // Number of bins for second
	                                         // color space axis (U)
	static const UINT NUM_BINS_3;            // Number of bins for third 
	                                         // color space axis (V)
	static const UINT QUANT_BITS;            // Number of bits in the
	                                         // converted color space

};



//----------------------------------------------------------------------------
class Histogram // This is basically a container class.  HistogramSeries does most operations
{
public:
        Histogram(UINT histSize);
        ~Histogram() {delete [] m_data;}

        ULONG & operator [] (UINT index) {return m_data[index];}
        long GetDiff() {return m_diff;}
        long GetDiff(Histogram &other);
        void SetDiff(long diff) {m_diff = diff;}

        ULONG * GetData() {return m_data;}

private:

        const UINT m_histSize;
        ULONG *m_data;
        long m_diff;    // abs errors between this histogram and next
        friend long XM::GetDiff(Histogram &a, Histogram &b);
        friend long XM::GetDiffL2(Histogram &a, Histogram &b);
        friend long XM::GetDiffL2(ULONG *i, Histogram &b);
        friend std::ostream & XM::operator << (std::ostream &out, Histogram &hist);
        friend std::istream & XM::operator >> (std::istream &in, Histogram &hist);

};


//----------------------------------------------------------------------------
class HistogramSeries 
{
public:
        HistogramSeries(ColorSpaceType colorSpace, UINT quantBits,
                                        UINT nBins1, UINT nBins2, UINT nBins3,
                                        UINT bUnivariate, UINT subSample);
        ~HistogramSeries();

        int SetSourceMedia(MomVop *media);
        ULONG InitExtracting( char *mediaName );
        bool SetZDim( ULONG zdim );
        ULONG StartExtracting();
#ifdef READ_WRITE_HISTOGRAMS
        void Write(char *filename);
        ULONG Read(char *filename);
#endif
        long GetDiffHist(ULONG i);
        long GetDiffHist(ULONG i, ULONG j);
        ULONG GetNumFrames() {return m_zdim;}
        UINT GetHistSize() {return m_histSize;}
        ULONG *GetHistData(ULONG iFrame) {return GetHist(iFrame).GetData();}
        ULONG GetHistData(ULONG iFrame, UINT iCol) {return GetHist(iFrame).GetData()[iCol];}
        const char *GetMediaName() {return m_mediaName;}

        long GetDiffL2(ULONG i, ULONG j) {return XM::GetDiffL2( GetHist(i), GetHist(j) );}
        long GetDiffL2(ULONG *i, ULONG j) {return XM::GetDiffL2( i, GetHist(j) );}


private:
        Histogram & operator [] (ULONG index) {return *m_hist[index];}
        Histogram & GetHist(ULONG index) {return *m_hist[index];}       // For clarity
        void ExtractFromImage(MomVop *image);
        void RGBToYCbCr( unsigned char RVal, unsigned char GVal, unsigned char BVal,
                                         int *YPt, int *CbPt, int *CrPt );

        const static ULONG incrBlockSize;
        ULONG m_blockSize;      // size of actual array m_hist (a multiple of incrBlockSize)

        Histogram **m_hist;
        ULONG m_zdim;           // number of frames            
        UINT m_height;          // Vertical size of video frame 
        UINT m_width;           // Horizontal size of video frame
        const UINT m_nBins1, m_nBins2, m_nBins3;        // number of bins per channel
        const UINT m_histSize;  // total number of bins in histogram
        UINT m_quantizeShift1, m_quantizeShift2, m_quantizeShift3;      // binary quantization
        const ColorSpaceType m_colorSpace;
        const UINT m_subSample;
        const UINT m_bUnivariate;
        char m_mediaName[MAXMEDIANAMELEN];    // Media (sequence) for the whole histogram series
        ULONG m_iHist;          // Current histogram being extracted
        MomVop *m_image;        // Current image being extracted

#ifdef READ_WRITE_HISTOGRAMS
        ULONG m_initFromFile;
#endif
};


//----------------------------------------------------------------------------
class KeyFrame
{
public:
        KeyFrame() : m_iFrame(-1) {}
        ~KeyFrame() {}

        long GetFrame() {return m_iFrame;}
        void SetFrame(long i) {m_iFrame = i;}

private:
        long m_iFrame;
};



//=============================================================================
//=============================================================================
//
//                                                      TREE BUILDING SUBSYSTEMS
//
//=============================================================================
//=============================================================================


//=============================================================================
class KeyFrameRef
{
public:
        KeyFrameRef() : m_iNextLevelPartition(-1), m_keyFrame(&DefaultKeyFrame) {}
        ~KeyFrameRef();

        void SetKeyFrameRef(KeyFrame &k) {m_keyFrame = &k;}
        KeyFrame &GetKeyFrame() {return *m_keyFrame;}
        void SetRefPartition(int i) {m_iNextLevelPartition = i;}
        int  GetRefPartition() {return m_iNextLevelPartition;}

        static KeyFrame DefaultKeyFrame;

private:
        int m_iNextLevelPartition;  // partition of the next (coarser) level
        KeyFrame *m_keyFrame;   // pointer to the associated keyframe
	
};


//=============================================================================
class Level
{
public:
        Level() : 
#ifdef DEBUG_FILES
	m_iLevel(0),
#endif
	m_keyFrameRef(0), m_numKeyFrameRefs(0), m_numAllocKeyFrameRefs(0) {};
        ~Level();

        //KeyFrame &GetKeyFrame(UINT i) {assert(i<m_numAllocKeyFrameRefs&&i<m_numKeyFrameRefs); return m_keyFrameRef[i].GetKeyFrame();}
        KeyFrame &GetKeyFrame(UINT i);// {if(i<m_numAllocKeyFrameRefs||i<m_numKeyFrameRefs) return KeyFrameRef::DefaultKeyFrame; return m_keyFrameRef[i].GetKeyFrame();}
        ULONG GetKeyFrameNum(UINT i);// {return GetKeyFrame(i).GetFrame();}
        KeyFrame &GetLastKeyFrame();// {return GetKeyFrame(m_numKeyFrameRefs - 1);}
        ULONG GetLastKeyFrameNum();// {return GetKeyFrameNum(m_numKeyFrameRefs - 1);}
        void SetKeyFrame(UINT i, KeyFrame &k);
        void SetKeyFramePartition(UINT i, int p);
        int  GetKeyFramePartition(UINT i);// {if(i<m_numAllocKeyFrameRefs||i<m_numKeyFrameRefs) {cerr << iLevel << return -1; return m_keyFrameRef[i].GetRefPartition();}
        UINT GetNumKeyFrames();// {return m_numKeyFrameRefs;}
        void SetNumKeyFrames(UINT i);
        void AllocKeyFrameRefs();// {if(m_numKeyFrameRefs) m_keyFrameRef = new KeyFrameRef[m_numAllocKeyFrameRefs = m_numKeyFrameRefs + 10];}    // Called after numKeyFrames stabilizes
        void AllocNumKeyFrames(UINT i);
	
#ifdef DEBUG_FILES
	UINT m_iLevel;
#endif

private:
        KeyFrameRef *m_keyFrameRef;
        UINT m_numKeyFrameRefs;
        UINT m_numAllocKeyFrameRefs;

};


//=============================================================================
class Shot
{
public:
        Shot();
        ~Shot();

        Level &GetLevel(UINT i) {assert(i<m_numLevels); return m_level[i];}
        ULONG GetStart() {return m_iStart;}
        void  SetStart(ULONG i) {m_iStart = i;}
        UINT  GetNumKeyFrames() {return GetLevel(0).GetNumKeyFrames();} // level 0 is finest resolution
        void  SetNumKeyFrames(UINT i) {GetLevel(0).SetNumKeyFrames(i);} // level 0 is finest resolution


        static void SetNumLevels(UINT n) {m_numLevels = n;}

private:
        Level *m_level;
        static UINT  m_numLevels;
        ULONG m_iStart;
};


//=============================================================================
class KeyFrameTree
{
public:
        KeyFrameTree( UINT minShotLength, float ratioKeyFrames, 
		      UINT numLevels, UINT ratioLevels,
		      HistogramSeries *histSeries );
        ~KeyFrameTree();

        void ExtractKeyFrameTree();

        // Accessor functions to key tree
        ULONG GetNumShots() {return m_numShots;}
        ULONG GetShotBoundary(UINT iShot) {return GetShot(iShot).GetStart();}
        ULONG GetNumKeyFramesPerShotLevel(UINT iShot, UINT iLevel) {return GetShot(iShot).GetLevel(iLevel).GetNumKeyFrames();}
        KeyFrame &GetKeyFrame(UINT iShot, UINT iLevel, UINT iRef) {return GetShot(iShot).GetLevel(iLevel).GetKeyFrame(iRef);}
        ULONG GetKeyFrameNum(UINT iShot, UINT iLevel, UINT iRef) {return GetKeyFrame(iShot,iLevel,iRef).GetFrame();}
        KeyFrame &GetKeyFrame(UINT iKeyFrame) {return m_keyFrames[iKeyFrame];}
        UINT  GetNumKeyFrames() {return m_numKeyFrames;}
        ULONG GetNumFrames() {return m_zdim;}
        UINT  GetNumLevels() {return m_numLevels;}
        const char *GetMediaName() {return m_histSeries->GetMediaName();}

        void SetKeyFrameIntoTree(UINT iShot, UINT iLevel, UINT iRef, KeyFrame &keyFrame);

private:
        // sub-extraction algorithms
        void FindShotChanges(float alpha, UINT minShotLength);
        void AllocateKeyFramesPerShot();

        void keyfind();
        void DistributeInitialKeyFrames();
        void MaximizeInterKeyFrame();
        void PruneKeyFrames();
        void AllocateKeyFramesWithinShot(UINT iShot);
        void IdentifyKeyFramesWithinShot(UINT iShot);

        Shot &GetShot(UINT iShot) {return m_shot[iShot];}

        HistogramSeries *m_histSeries;
        Shot *m_shot;
        KeyFrame *m_keyFrames;

        // The quantity of frames in a media sequence may exceed max unsigned int.
        ULONG m_zdim;   // copied for convenience
        ULONG m_numShots;	
        ULONG m_numRequestedKeyFrames;
        UINT  m_numKeyFrames;
        ULONG m_minShotLength;
        UINT  m_numLevels;
        UINT  m_ratioLevels;

	// Assumptions and simplifications
	static const double ALPHA;
	static const double BETA;
	static const double BETA_DECREMENT;
	static const double MAXERASE;
	static const double THETA;
	static const int NUMITER;
#ifdef DEBUG_FILES
	ofstream m_fkey;
#endif
};


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//                                                      DOM BUILDING SUBSYSTEMS
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class SummarizationDOM
{
public:
        SummarizationDOM(KeyFrameTree *keyFrameTree);
        ~SummarizationDOM();

        GenericDS CreateRoot(GenericDSInterfaceABC *doc);	// Need to check that this is the root document
        GenericDS CreateSummarization(GenericDS parent);

private:
        GenericDS CreateMediaInfo(GenericDS parent);
        GenericDS CreateMetaInfo(GenericDS parent);
        GenericDS CreateSummarySegmentGroup(GenericDS parent, int level);
        GenericDS CreateSummarySegment(GenericDS parent, int level, ULONG segment);

	// Do we care if the return is an element or a node?
	//(DOM_Element vs. DOM_Node)
	GenericDS CreateElem(GenericDS parent, const char *tag,
			     const char *content = NULL); 
        GenericDS CreateElem(GenericDS parent, const char *tag,
			     const long val, const long val2 = LONG_MAX); 
        GenericDS CreateElemWithAttrib(GenericDS parent, const char *tag,
				       const char *content, 
					   const char *attr,			const char *value,
					   const char *attr2 = NULL,	const char *value2 = NULL,
					   const char *attr3 = NULL,	const char *value3 = NULL);

        KeyFrameTree *m_keyFrameTree;
        GenericDS m_domDoc;

	// Assumptions and simplifications
	static const UINT INCLUDE_VIDEO_SEGMENT; // VideSegmentLocators:
	                                         // 0=None,
	                                         // 1=PerShot,
	                                         // 2=PerKeyFrame
	static const UINT VIDEO_IMAGE_OFFSET;    // VideoSegment begins
	                                         // at Image (0=BEGIN)
	static const UINT LEVEL_HIERARCHY;       // HighlightLevels are
	                                         // not nested (0=FLAT)

	ULONG offset;
};

};      // namespace XM

#endif  // HIERARCHICAL_KEY_FRAME_H
