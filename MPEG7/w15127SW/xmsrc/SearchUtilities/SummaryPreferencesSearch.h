//////////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Toshihiko Munetsugu 
// Katsunao Takahashi
//        (Matsushita Electric Industrial Co., Ltd./
//         Matsushita Research Institute Tokyo, Inc.)
//
// Ver4.0 for FDIS, 20010928
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
 * SummayPreferencesSearch.h     *
 *********************************/

#ifndef __SPR_SEGMENT_FILTER_SEARCH_H__
#define __SPR_SEGMENT_FILTER_SEARCH_H__
#include "BasicArchitecture/XMInterfaces.h"                            /* Naming Convention */
#include "BasicArchitecture/XMObjects.h"                               /* Naming Convention */
#include "SearchUtilities/MatchList.h"
#include "DescriptionSchemes/DescriptionSchemes.h"

//=============================================================================

namespace XM
{


// Forward Declarations:
class SummaryPreferencesSearchInterfaceABC ;
class SummaryPreferencesSearchInterface ;
class SummaryPreferencesSearchTool ;
class SPRBrowsingQuery ;
class SPRPreferredSummaryTheme ;

//=============================================================================
class SummaryPreferencesSearchInterfaceABC: public I_SearchInterfaceABC
{
public :
	SummaryPreferencesSearchInterfaceABC() ;
	virtual ~SummaryPreferencesSearchInterfaceABC() {} ;

	virtual void destroy(void) = 0 ;
	virtual int SetRefDescriptionInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0 ;
	virtual int SetQueryDescriptionInterface(GenericDSInterfaceABC *aQueryInterface) = 0 ;
	virtual int SetQuery(void) = 0 ;
	virtual double GetDistance(void) = 0 ;
	virtual GenericDS * GetFilteredSegment(void) = 0 ;
	
	static const UUID myID ;
};

//=============================================================================
class SummaryPreferencesSearchInterface:   public SummaryPreferencesSearchInterfaceABC
{
public :
	SummaryPreferencesSearchInterface(SummaryPreferencesSearchTool* aTool) ;
	virtual ~SummaryPreferencesSearchInterface() ;
	
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
	SummaryPreferencesSearchTool *m_SearchTool ;
} ;

//=============================================================================
class SummaryPreferencesSearchTool: public Search
{
friend class SummaryPreferencesSearchInterface ;
public :
	SummaryPreferencesSearchTool() ;
	SummaryPreferencesSearchTool(GenericDSInterfaceABC *aInterface) ;

	virtual const UUID& GetObjectID(void) ;
	virtual const char *GetName(void) ;
	virtual SummaryPreferencesSearchInterfaceABC *GetInterface(void) ;

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
	virtual ~SummaryPreferencesSearchTool() ;
	bool SegmentFiltering(void) ;
	void CopyNode(GenericDSInterface *distInterface, GenericDSInterface *srcInterface) ;
	bool IsThisSegmentFilteredOut(GenericDS & theNowSegment) ;
	int ExecuteFiltering(GenericDS & theNowNode) ;
	SummaryPreferencesSearchInterface m_Interface ;
	GenericDSInterface   *m_RefDSInterface ;
	GenericDSInterface   *m_QueryDSInterface ;
	SPRBrowsingQuery  * m_Query ;
	GenericDS  m_FilteredSegment ;
	double m_distanceValue ;
	enum FilterStatus {
		FILTEROUT_THIS_SEGMENT =  1,
		FILTERING_SUCCEEDED    =  0,
		FILTERING_FAILED       = -1
	} ;	
};

//=============================================================================
class SPRBrowsingQuery
{
public :
	SPRBrowsingQuery() ;
	~SPRBrowsingQuery() ;

	int ExtractQueryFromDS(GenericDSInterface *QueryDSInterface) ;
	int GetPreferredSummaryThemeLength(void) ;
	std::vector<SPRPreferredSummaryTheme> GetPreferredSummaryTheme(void) ;
	void SetSummaryDuration(long summaryDuration) ;
	long GetSummaryDuration(void) ;

	long GetDurationValue(const char * durString) ;

#define DEFAULT_SUMMARYTHEME_PREFERENCEVALUE  10

private:
	std::vector<SPRPreferredSummaryTheme> m_povList ;
	long	m_summaryDuration;	
	// Error Code
	enum	QueryStatus {
		NO_ERROR = 0,
		NOT_SUMMARYPREFERENCES          = -1,
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
class SPRPreferredSummaryTheme
{
public :
	SPRPreferredSummaryTheme() ;
	~SPRPreferredSummaryTheme() ;

	void SetThemeValue(string newThemeValue) ;
	string GetThemeValue(void) ;
	void SetPreferrenceValue(int newPrefValue) ;
	int GetPreferrenceValue(void) ;
	void SetXMLLanguage(string newLangValue) ;
	string GetXMLLanguage(void) ;

private :
	string	m_themeValue ;
	int	m_prefValue ;
	string	m_langValue ;
#define	UPPERBOUND_OF_PREFERENCEVALUE         100
#define	LOWERBOUND_OF_PREFERENCEVALUE        -100
} ;


#ifndef	MPEG7MAIN_ELEMENT
#define	MPEG7MAIN_ELEMENT	"Mpeg7"
#define	MAIN_TYPE_ATTR		"type"
#define	MAIN_XMLNS_ATTR		"xmlns"
#define	MAIN_XMLNSXSI_ATTR	"xmlns:xsi"
#define	MAIN_XSISL_ATTR		"xsi:schemaLocation"
#endif

#ifndef SUMMARYPREFERENCE_ELEMENT
#define	SPR_USERDESCRIPTION		"UserDescriptionType"
#define	SUMMARYPREFERENCE_ELEMENT	"UserPreferences"
#define	SPR_USERID			"UserIdentifier"
#define	SPR_USAGEPREF			"UsagePreferences"
#define	SPR_BROWSINGPREF		"BrowsingPreferences"
#define	SPR_SUMMARYPREF			"SummaryPreferences"
#define	SPR_PREFERREDTHEME		"SummaryTheme"
#define	SPR_PREFERENCEVALUE		"preferenceValue"
#define	SPR_SUMMARYDURATION		"SummaryDuration"
#endif

#ifndef	CONTENTMANAGEMENT
#define	CONTENTMANAGEMENT	"Description"
#define	CMNG_TYPE_ATTR		"xsi:type"
#endif

#ifndef AUDIOVISUAL_ELEMENT
#define	CDESC_CENTITY_TYPE	"ContentEntityType"
#define	AUDIOVISUAL_CONTENT	"MultimediaContent"
#define	AV_AVTYPE		"AudioVisualType"
#define	AUDIOVISUAL_ELEMENT	"AudioVisual"
#define	AV_MEDIATIME		"MediaTime"
#define	AV_MEDIATIMEPOINT	"MediaRelTimePoint"
#define	AV_MEDIADURATION	"MediaDuration"

#define	AUDIOVISUAL_SEGMENT	"AudioVisualSegment"
#define	AUDIOVISUAL_ID_ATTR	"id"
#define	AUDIOVISUAL_GAP_ATTR	"gap"
#define	AV_SEGMENTDECOMP	"TemporalDecomposition"
#endif

#ifndef SPR_POINTOFVIEW_ELEMENT
#define	POINTOFVIEW_ELEMENT	"PointOfView"
#define	POV_VIEWPOINT_ATTR	"viewpoint"
#define	POV_SUPPLEMENTALINFO	"SupplementalInfo"
#define	POV_IMPORTANCE_ELEMENT	"Importance"
#define	POV_VALUE_ELEMENT	"Value"

#define	POV_HREF_ELEMENT	"Href"
#define	POV_IDREF_ELEMENT	"IDRef"
#define	POV_XPATH_ELEMENT	"XPath"
#endif

#ifndef CONTENTDESCRIPTION
#define    CONTENTDESCRIPTION     "Description"
#define    CDESC_TYPE_ATTR        "xsi:type"
#endif

#ifndef SUMMARIZATION_ELEMENT
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
#define    HLS_KEYVIDEOCLIP_ELEM  "keyAudiolVisualClip"
#define    HLS_KEYVIDEOCLIP_VAL   "keyAudiolVisualClips"
#endif

} ;

#endif //__SPR_SEGMENT_FILTER_SEARCH_H__
