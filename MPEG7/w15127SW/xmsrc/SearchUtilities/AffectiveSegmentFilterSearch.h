//////////////////////////////////////////////////////////////////////////////
//
// This software module was originally developed by
//
// Yoshiaki Shibata (Sony Corporation)
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
// Applicable File Name:  AffectiveSegmentFilter.h
//

#ifndef __AFFECTIVE_SEGMENT_FILTER_H__
#define __AFFECTIVE_SEGMENT_FILTER_H__

namespace XM
{

// Forward Declarations:
class AffectiveSegmentFilterInterfaceABC;
class AffectiveSegmentFilterInterface;
class AffectiveSegmentFilterTool;

//=============================================================================
class AffectiveSegmentFilterInterfaceABC: public I_SearchInterfaceABC
{
public :
	AffectiveSegmentFilterInterfaceABC();
	virtual ~AffectiveSegmentFilterInterfaceABC() {};

	virtual int SetGenericDSInterface(GenericDSInterfaceABC *aGenericDSInterface) = 0;
	virtual bool ExecuteAffectiveFiltering() = 0;

	static const UUID myID;
};

//=============================================================================
class AffectiveSegmentFilterInterface:   public AffectiveSegmentFilterInterfaceABC
{
public:
	AffectiveSegmentFilterInterface(AffectiveSegmentFilterTool *aTool);
	virtual ~AffectiveSegmentFilterInterface();
	
	virtual const UUID& GetInterfaceID(void);
	virtual const char *GetName(void);

	virtual int SetGenericDSInterface(GenericDSInterfaceABC *aGenericDSInterface);
	virtual bool ExecuteAffectiveFiltering(void);

	static const UUID myID;
	static const char *myName;

private :
	AffectiveSegmentFilterTool *m_FilterTool;
};

//=============================================================================
class AffectiveSegmentFilterTool: public Search
{
friend class AffectiveSegmentFilterInterface;
public:
	AffectiveSegmentFilterTool();

	virtual const UUID& GetObjectID(void);
	virtual const char *GetName(void);
	virtual AffectiveSegmentFilterInterfaceABC *GetInterface(void);

	virtual bool SupportsPush(void);
	virtual bool SupportsPull(void);

	virtual int SetGenericDSInterface(GenericDSInterfaceABC *aGenericDSInterface);
	bool ExecuteAffectiveFiltering(void);

	static const UUID myID;
	static const char *myName;

	// MPEG-7 D/DS as public sub-classes for AffectiveSegmentFilterTool:
	class MediaDurationType//sth whats the effect of this twisted code?
	{
	public:
		MediaDurationType();
		MediaDurationType(double aDuration);
		MediaDurationType(string aDuration);
		MediaDurationType(GenericDS& aDuration);
		~MediaDurationType();

		void SetDuration(double aDuration);
		void SetDuration(string aDuration);
		void SetDuration(GenericDS& aDuration);
		bool SetNumOfFraction(int aNumOfFraction);

		double GetDurationInSec(void);
		string GetDurationAsString(void);
		GenericDS GetDurationAsDescription(GenericDS& root, string tagName="MediaDuration");
		int GetNumOfFraction(void);

	private:
		double m_duration;	// in second
		int m_numOfFraction;
	};

	//=========================================================================
	class MediaRelTimePointType
	{
	public:
		MediaRelTimePointType();
		MediaRelTimePointType(double aRelTimePoint);
		MediaRelTimePointType(string aRelTimePoint);
		MediaRelTimePointType(GenericDS& aRelTimePoint);
		~MediaRelTimePointType();

		void SetRelTimePoint(double aRelTimePoint);
		void SetRelTimePoint(string aRelTimePoint);
		void SetRelTimePoint(GenericDS& aRelTimePoint);
		bool SetNumOfFraction(int aNumOfFraction);
		void SetTimeBase(string aTimeBase);

		double GetRelTimePointInSec(void);
		string GetRelTimePointAsString(void);
		GenericDS GetRelTimePointAsDescription(GenericDS& root, string tagName="MediaRelTimePoint");
		int GetNumOfFraction(void);
		string GetTimeBase(void);

	private:
		string m_timeBase;		// in xPathType
		double m_relTimePoint;	// in second
		int m_numOfFraction;
	};

	//=========================================================================
	class MediaTimePointType
	{
	public:
		MediaTimePointType();
		MediaTimePointType(string aTimePoint);
		MediaTimePointType(GenericDS& aTimePoint);
		~MediaTimePointType();

		void SetTimePoint(string aTimePoint);
		void SetTimePoint(GenericDS& aTimePoint);
		bool SetYear(int aYear);
		bool SetMonth(int aMonth);
		bool SetDay(int aDay);
		bool SetHour(int anHour);
		bool SetMinute(int aMinute);
		bool SetSecond(int aSecond);
		bool SetFraction(int aFraction);
		bool SetNumOfFraction(int aNumOfFraction);

		string GetTimePointAsString(void);
		GenericDS GetTimePointAsDescription(GenericDS& root, string tagName="MediaTimePoint");
		int GetYear(void);
		int GetMonth(void);
		int GetDay(void);
		int GetHour(void);
		int GetMinute(void);
		int GetSecond(void);
		int GetFraction(void);
		int GetNumOfFraction(void);

	private:
		int m_year;
		int m_month;
		int m_day;
		int m_hour;
		int m_minute;
		int m_second;
		int m_fraction;
		int m_numOfFraction;
	};

	//=========================================================================
	class MediaTimeType
	{
	public:
		MediaTimeType();
		MediaTimeType(GenericDS aMediaTime);
		~MediaTimeType();

		void SetMediaTime(GenericDS& aMediaTime);
		bool SetMediaTimePoint(MediaTimePointType aMediaTimePoint);
		bool SetMediaRelTimePoint(MediaRelTimePointType aMediaRelTimePoint);
		bool SetMediaDuration(MediaDurationType aMediaDuration);

		GenericDS GetMediaTimeAsDescription(GenericDS& root, string tagName="MediaTime");
		MediaTimePointType test(MediaTimePointType a);
		MediaTimePointType * GetMediaTimePoint(void);
		MediaRelTimePointType * GetMediaRelTimePoint(void);
		MediaDurationType * GetMediaDuration(void);

	private:
		MediaTimePointType		*m_mediaTimePoint;
		MediaRelTimePointType	*m_mediaRelTimePoint;
		MediaDurationType		*m_mediaDuration;
	};

	//=========================================================================
	class AffectiveType
	{
	public:
		AffectiveType();
		AffectiveType(GenericDS& anAffective);
		~AffectiveType();

		void SetAffective(GenericDS& anAffective);
		void SetId(string id);
		void SetType(string typeName, string href="");
		void SetScore(string ref, double value);
		bool SetScoreAt(int index, string ref, double value);

		GenericDS GetAffectiveAsDescription(GenericDS& root, string tagName="Affective");
		string GetId(void);
		string GetTypeName(void);
		GenericDS GetTypeAsDescription(GenericDS& root);
		double GetScoreValueAt(int index);
		string GetScoreRefAt(int index);

	private:
		string	m_id;
		string	m_typeName;
		string	m_typeHref;
		std::vector<double>	m_scoreValueList;
		std::vector<string>	m_scoreRefList;
	};

	//=========================================================================
	class MediaLocatorType
	{
	public:
		MediaLocatorType();
		MediaLocatorType(GenericDS aMediaLocator);
		~MediaLocatorType();

		void SetMediaLocator(GenericDS& aMediaLocator);
		void SetMediaUri(string aMediaUri);
		void SetStreamId(int aStreamId);

		GenericDS GetMediaLocatorAsDescription(GenericDS& root, string tagName="MediaLocator");
		string GetMediaUri(void);
		int GetStreamId(void);

	private:
		string	m_mediaUri;
		int		m_streamId;
	};

	//=========================================================================
private:
	virtual ~AffectiveSegmentFilterTool();
	// for interface
	AffectiveSegmentFilterInterface m_Interface;
	GenericDSInterfaceABC *m_GenericDSInterface;

	// parameters for input description
	int	m_introductionEndSceneNum;
	int	m_developmentEndSceneNum;
	int	m_turnEndSceneNum;

	// thresholds for the affective filtering
	double m_theSemanticScoreThreshold;
	double m_thePeakThreshold;

	// private members for the affective segment filtering
	MediaLocatorType	m_mediaLocator;
	AffectiveType		m_affective;
	std::vector<GenericDS> m_sceneList;	// for AudioVisualSegment

	// subroutines ExecuteAffectiveFiltering()
	bool SetPrivateMembers(void);
	bool ExecuteFiltering(void);
	bool CreateSummaryDescription(void);
};

};

#endif //__AFFECTIVE_SEGMENT_FILTER_H__
