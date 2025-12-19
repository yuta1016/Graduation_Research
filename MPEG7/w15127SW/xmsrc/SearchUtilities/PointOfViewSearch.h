//////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Katsunao Takahashi
// Toshihiko Munetsugu 
//        (Matsushita Research Institute Tokyo, Inc./
//         Matsushita Electric Industrial Co., Ltd.)
//
// Ver4.0 for FDIS, 20010928
// Ver3.0 for FCD,  20010824
//
// in the course of development of the MPEG-7 standard (ISO/IEC 15938).
// This software module is an implementation of a part of one or more
// MPEG-7 tools as specified by the MPEG-7 standard (ISO/IEC 15938).
// ISO/IEC gives users of the MPEG-7 standard (ISO/IEC 15938) free licence
// to this software module or modifications thereof for use in hardware or
// software products claiming confermance to the MPEG-7 standard (ISO/IEC 15938).
// Those intending to use this software module in hardware or software 
// products are advised that its use may infringe existing patents.
// THE ORIGINAL DEVELOPER OF THIS SOFTWARE MODULE AND HIS/HER COMPANY,
// THE SUBSEQUENT EDITORS AND THEIR COMPANIES, AND ISO/IEC HAVE NO 
// LIABILITY FOR USE OF THIS SOFTWARE MODULE OR MODIFICATIONS THEROF.
// No license to this software module is granted for non MPEG-7 standard
// (ISO/IEC 15938) comforming products.
// Matsushita Electric Industrial Co., Ltd. and Matsushita Research Institute
// Tokyo, Inc. retain full right to use the software module for their own
// purpose, assign or donate the software module to a third party and 
// to inhibit third parties from using the code for non MPEG-7 standard
// (ISO/IEC 15938) conforming products.
//
// Copyright (c) 2000-2001.
//
// This copyright notice must be included in all copies or derivative works
// of the software module.
//
//
//////////////////////////////////////////////////////////////////////////////////
/*********************************
 * PointOfViewSearch.h           *
 *********************************/

#ifndef __POINTOFVIEW_SEARCH_H__
#define __POINTOFVIEW_SEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

#include <vector>
#include <map>

//=============================================================================

namespace XM
{

// Forward Declarations:
class PointOfViewSearchTool;
class POVSegmentFilterToolABC;
class ObjectSegmentInfo;
class POV_BrowsingQuery;
class POV_PreferredSummaryTheme;

//=============================================================================
class PointOfViewSearchInterfaceABC: public I_SearchInterfaceABC
{
public :
	PointOfViewSearchInterfaceABC() ;
	virtual ~PointOfViewSearchInterfaceABC() {} ;

	virtual void destroy(void) = 0 ;
	virtual int SetRefDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0 ;
	virtual int SetQueryDescriptionInterface(GenericDSInterfaceABC *aQueryInterface) = 0 ;
	virtual int SetQuery(void) = 0 ;
	virtual double GetDistance(void) = 0 ;
	virtual GenericDS * GetFilteredSegment(void) = 0 ;
	
	static const UUID myID ;
};

//=============================================================================
class PointOfViewSearchInterface:   public PointOfViewSearchInterfaceABC
{
public :
	PointOfViewSearchInterface(PointOfViewSearchTool* aTool) ;
	virtual ~PointOfViewSearchInterface() ;
	
	virtual const UUID& GetInterfaceID(void) ;
	virtual const char *GetName(void) ;

	virtual void destroy(void) ;
	virtual int SetRefDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface) ;
	virtual int SetQueryDescriptionInterface(GenericDSInterfaceABC *aQueryInterface) ;
	virtual int SetQuery(void) ;
	virtual double GetDistance(void) ;
	virtual GenericDS * GetFilteredSegment(void) ;
	
	static const UUID myID ;
	static const char *myName ;

private :
	PointOfViewSearchTool *m_SearchTool ;
} ;

//=============================================================================
class PointOfViewSearchTool: public Search
{
friend class PointOfViewSearchInterface ;
public :
	PointOfViewSearchTool() ;
	PointOfViewSearchTool(GenericDSInterfaceABC *aInterface) ;

	virtual const UUID& GetObjectID(void) ;
	virtual const char *GetName(void) ;
	virtual PointOfViewSearchInterfaceABC *GetInterface(void) ;

	virtual bool SupportsPush(void) ;
	virtual bool SupportsPull(void) ;

	virtual int SetRefDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface) ;
	virtual int SetQueryDescriptionInterface(GenericDSInterfaceABC *aQueryInterface) ;
	virtual int SetQuery(void) ;
	virtual double GetDistance(void) ;

	virtual GenericDS * GetFilteredSegment(void) ;

	static const UUID myID ;
	static const char * myName ;

private :
	virtual ~PointOfViewSearchTool() ;
	bool SegmentFiltering(void) ;
	PointOfViewSearchInterface m_Interface ;
	GenericDSInterface   *m_RefDSInterface ;
	GenericDSInterface   *m_QueryDSInterface ;
	POV_BrowsingQuery  * m_Query;
	POVSegmentFilterToolABC *m_FilteringTool ;
	GenericDS  m_FilteredSegment ;
	double m_distanceValue ;
};

//=============================================================================
class POVSegmentFilterToolABC
{
public:
	POVSegmentFilterToolABC(POV_BrowsingQuery *aQuery, GenericDS & aFilteredSegment) ;
	virtual ~POVSegmentFilterToolABC() ;

	virtual int SetObjectDS(GenericDS & aFilteredSegment) ;

	virtual void PrepareForFiltering(void) = 0 ;
	virtual bool ExecuteFiltering(void) ;

protected:
	virtual bool IsThisSegmentFilteredOut(GenericDS & theNowNode) = 0;
	virtual GenericDS FindTopSegmentNode(GenericDS & theNowNode) ;
	virtual int NodeFiltering(GenericDS & theNowNode) ;
	virtual double calcSegmentScore(GenericDS & theNowNode) ;
	virtual void stripPointOfView(GenericDS & theNowNode) ;
	POV_BrowsingQuery *m_Query ;
	GenericDS  m_FilteredSegment ;
	enum FilterStatus {
		FILTEROUT_THIS_SEGMENT =  1,
		FILTERING_SUCCEEDED    =  0,
		FILTERING_FAILED       = -1
	} ;	

} ;

//=============================================================================
class POVSegmentFilterWithDurationTool: public POVSegmentFilterToolABC
{
public:
	POVSegmentFilterWithDurationTool(POV_BrowsingQuery *aQuery, GenericDS & aFilteredSegment) ;
	virtual ~POVSegmentFilterWithDurationTool() ;

	virtual void PrepareForFiltering(void) ;
	
private:
	virtual bool IsThisSegmentFilteredOut(GenericDS & theNowNode) ;
	void sortObjectSegment(void) ;
	void checkNode(GenericDS & theNowNode) ;
	void checkObjectSegment(GenericDS & theNowNode) ;
	void appendSegmentVector(ObjectSegmentInfo newSegment) ;
	int selectSegments(void) ;
	std::vector<ObjectSegmentInfo>  m_vSegs ;
	int  m_selectedIdx ;
} ;

//=============================================================================
class POVSegmentFilterWithoutDurationTool: public POVSegmentFilterToolABC
{
public:
	POVSegmentFilterWithoutDurationTool(POV_BrowsingQuery *aQuery, GenericDS & aFilteredSegment) ;
	virtual ~POVSegmentFilterWithoutDurationTool() ;

	virtual void PrepareForFiltering(void) ;

private:
	virtual bool IsThisSegmentFilteredOut(GenericDS & theNowNode) ;

#define THRESHOLD_SEGMENT_VALUE  50

} ;

//=============================================================================
class ObjectSegmentInfo
{
public:
	ObjectSegmentInfo() ;
	~ObjectSegmentInfo() ;

	void SetID(string aID) ;
	string GetID(void) ;
	void SetValue(double aValue) ;
	double GetValue(void) ;
	void SetDuration(double aDuration) ;
	double GetDuration(void) ;

private:
	string   m_segmentID ;
	double   m_value ;
	double   m_segmentDuration ;
} ;

//=============================================================================
class FilteredSegmentToSummary
{
public:
	FilteredSegmentToSummary() ;
	~FilteredSegmentToSummary() ;

	void SetFilteredSegment(GenericDS * filteredSegment) ;
	bool ConvertToSummary(void) ;
	GenericDS * GetSummarization(void) ;

private:
	void convertNode(GenericDS & theNowNode, GenericDS & theHighlightSummary, GenericDS & theSummaryThemeList) ;
	void convertObjectSegment(GenericDS & theNowNode, GenericDS & theHighlightSummary, GenericDS & theSummaryThemeList) ;
	GenericDS FindTopSegmentNode(GenericDS & theNowNode) ;
	string createDurationString(double summaryDuration, long base = 10) ;
	GenericDS *m_filteredSegment ;
	GenericDS m_Summarization ;
	double    m_summaryDuration ;
	std::map<string, string>  m_summaryTheme;
} ;

//=============================================================================
class CalcDurationValue
{
public:
	CalcDurationValue(GenericDS & aMediaDuration) ;
	~CalcDurationValue() ;

	void SetMediaDuration(GenericDS & aMediaDuration) ;
	double GetDurationValue(void) ;
	
private:
	GenericDS  theMediaDuration ;
} ;

//=============================================================================
class CopyDescriptionTool
{
public:
	CopyDescriptionTool() ;
	CopyDescriptionTool(GenericDSInterface *aDestInterface, GenericDSInterface *aSrcInterface) ;
	~CopyDescriptionTool() ;

	void SetDestination(GenericDSInterface *aDestInterface) ;
	void SetSource(GenericDSInterface *aSrcInterface) ;

	void ExecCopy(void) ;
	void ExecFullCopy(void) ;

private :
	void copyAttributes(GenericDSInterface *destInterface, GenericDSInterface *srcInterface) ;
	void copyContents(GenericDSInterface *destInterface, GenericDSInterface *srcInterface) ;
	void copyNodeWithChildren(GenericDSInterface *destInterface, GenericDSInterface *srcInterface) ;
	GenericDSInterface *m_DestInterface ;
	GenericDSInterface *m_SrcInterface ;
};

//=============================================================================

#ifndef POV_MPEG7MAIN_ELEMENT
#define	MPEG7MAIN_ELEMENT	"Mpeg7"
#define	MAIN_TYPE_ATTR		"type"
#define	MAIN_XMLNS_ATTR		"xmlns"
#define	MAIN_XMLNSMPEG7_ATTR	"xmlns:mpeg7"
#define	MAIN_TARGETNS_ATTR	"targetNamescace"
#define	MAIN_ELEMENTFD_ATTR	"elementFormDefault"
#define	MAIN_ATTRIBUTEFD_ATTR	"attributeFormDefault"
#endif

#ifndef POV_USERPREFERENCE_ELEMENT
#define	POV_USERDESCRIPTION	"UserDescriptionType"
#define	USERPREFERENCE_ELEMENT	"UserPreferences"
#define	POV_USAGEPREF		"UsagePreferences"
#define	POV_BROWSINGPREF	"BrowsingPreferences"
#define	POV_SUMMARYPREF		"SummaryPreferences"
#define	POV_PREFERREDTHEME	"SummaryTheme"
#define	POV_PREFERENCEVALUE	"preferenceValue"
#define	POV_SUMMARYDURATION	"SummaryDuration"
#endif

#ifndef POV_CONTENTMANAGEMENT
#define    CONTENTMANAGEMENT      "Description"
#define    CMNG_TYPE_ATTR         "xsi:type"
#endif

#ifndef POV_AUDIOVISUAL_ELEMENT
#define    CDESC_CENTITY_TYPE     "ContentEntityType"
#define    AUDIOVISUAL_CONTENT    "MultimediaContent"
#define    AUDIOVISUAL_ELEMENT    "AudioVisual"
#define    AV_MEDIATIME           "MediaTime"
#define    AV_MEDIADURATION       "MediaDuration"

#define    AUDIOVISUAL_SEGMENT    "AudioVisualSegment"
#define    AUDIOVISUAL_ID_ATTR    "id"
#define    AUDIOVISUAL_GAP_ATTR   "gap"
#define    AV_SEGMENTDECOMP       "TemporalDecomposition"
#endif

#ifndef POINTOFVIEW_ELEMENT
#define    POINTOFVIEW_ELEMENT    "PointOfView"
#define    POV_VIEWPOINT_ATTR     "viewpoint"
#define    POV_IMPORTANCE_ELEMENT "Importance"
#define    POV_VALUE_ELEMENT      "Value"

#endif

#ifndef POV_CONTENTDESCRIPTION
#define    CONTENTDESCRIPTION     "Description"
#define    CDESC_TYPE_ATTR        "xsi:type"
#endif

#ifndef	POV_SUMMARIZATION_ELEMENT
#define    CDESC_SUMMARYTYPE_VAL  "SummaryDescriptionType"
#define    SUMMARIZATION_ELEMENT  "Summarization"
#define    SUMMARY_ELEMENT        "Summary"
#define    SUMMARY_TYPE_ATTR      "xsi:type"
#define    HIERAR_SUMM_TYPE       "HierarchicalSummaryType"
#define    HS_HIERARCHY_ATTR      "hierarchy"
#define    HS_COMPLIST_ATTR       "components"
#define    HS_SUMMARYTHEMELIST    "SummaryThemeList"
#define    HS_SUMMARYTHEME        "SummaryTheme"
#define    HS_ST_ID_ATTR          "id"
#define    HIGHLIGHTSUMMARY_ELEM  "SummarySegmentGroup"
#define    HLL_LEVEL_ATTR         "level"
#define    HLL_DURATION_ATTR      "duration"
#define    HIGHLIGHTSEGMENT_ELEM  "SummarySegment"
#define    HLS_NAME_ELEM          "Name"
#define    HLS_THEMEIDS_ATTR      "themeIds"
#define    HLS_KEYVIDEOCLIP_ELEM  "keyAudioVisualClip"
#define    HLS_KEYVIDEOCLIP_VAL   "keyAudioVisualClips"
#endif

class POV_BrowsingQuery
{
public :
	POV_BrowsingQuery();
	~POV_BrowsingQuery();

	int ExtractQueryFromDS(GenericDSInterface *QueryDSInterface);
	int GetPreferredSummaryThemeLength(void);
	std::vector<POV_PreferredSummaryTheme> GetPreferredSummaryTheme(void);
	void SetSummaryDuration(long summaryDuration);
	long GetSummaryDuration(void);

	long GetDurationValue(const char * durString);

#define DEFAULT_SUMMARYTHEME_PREFERENCEVALUE  10

private:
	std::vector<POV_PreferredSummaryTheme> m_povList;
	long	m_summaryDuration;	
	// Error Code
	enum	QueryStatus {
		NO_ERROR = 0,
		NOT_USERPREFERENCES             = -1,
		NO_USAGEPREFERENCES             = -2,
		NO_BROWSINGPREFERENCES          = -3,
		NO_SUMMARYPREFERENCES           = -4,
		UNABLE_EXTRACT_SUMMARY_THEME    = -5,
		UNABLE_EXTRACT_SUMMARY_DURATION = -6,
		DURATION_FORMAT_ERROR           = -7,
		FORMAT_ERROR                    = -8,
		QUERY_IS_NULL                   = -9
	};
};

//=============================================================================
class POV_PreferredSummaryTheme
{
public :
	POV_PreferredSummaryTheme();
	~POV_PreferredSummaryTheme();

	void SetThemeValue(string newThemeValue);
	string GetThemeValue(void);
	void SetPreferrenceValue(int newPrefValue);
	int GetPreferrenceValue(void);
	void SetXMLLanguage(string newLangValue);
	string GetXMLLanguage(void);

private :
	string   m_themeValue;
	int      m_prefValue;
	string   m_langValue;
#define UPPERBOUND_OF_PREFERENCEVALUE         100
#define LOWERBOUND_OF_PREFERENCEVALUE        -100
};

};
#endif //__POINTOFVIEW_SEARCH_H__
