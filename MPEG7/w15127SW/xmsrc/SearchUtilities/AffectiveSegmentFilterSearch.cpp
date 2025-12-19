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
// Applicable File Name:  AffectiveSegmentFilter.cpp
//

#include <stdio.h>
#ifdef DEBUG
#ifndef NODISP
#define XYUVDISP
#endif
#endif
#ifdef XYUVDISP
#include <xyuv.h>
#endif

#include <math.h>
#include <cstring>

#include "SearchUtilities/SearchUtilities.h"        /* Naming Convention */
#include "Utilities/readparam.h"

using namespace XM;
using namespace std;

const UUID AffectiveSegmentFilterInterface::myID = UUID("");
const char *AffectiveSegmentFilterInterface::myName = "Affective Segment Filtering Interface";

const UUID AffectiveSegmentFilterTool::myID = UUID("");
const char *AffectiveSegmentFilterTool::myName = "Affective Segment Filtering Tool";

const UUID AffectiveSegmentFilterInterfaceABC::myID = UUID();

//=============================================================================
AffectiveSegmentFilterInterfaceABC::AffectiveSegmentFilterInterfaceABC()
{
	SetInterfaceID(myID);
}

//=============================================================================
AffectiveSegmentFilterInterface::AffectiveSegmentFilterInterface(AffectiveSegmentFilterTool* aTool):
m_FilterTool(aTool)
{
	SetInterfaceID(myID);
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterInterface::~AffectiveSegmentFilterInterface()
{
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterInterface::
SetGenericDSInterface(GenericDSInterfaceABC  *aGenericDSInterface)
{
  return( m_FilterTool->SetGenericDSInterface(aGenericDSInterface) );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterInterface::
ExecuteAffectiveFiltering(void)
{
  return( m_FilterTool->ExecuteAffectiveFiltering() );
}

//----------------------------------------------------------------------------
const UUID & AffectiveSegmentFilterInterface::GetInterfaceID(void)
{ 
	return(myID);
}

//----------------------------------------------------------------------------
const char * AffectiveSegmentFilterInterface::GetName(void)
{
	return(myName);
}

//=============================================================================
AffectiveSegmentFilterTool::AffectiveSegmentFilterTool():
m_Interface(this),
m_GenericDSInterface(0)
{
	SetExposedInterface(&m_Interface);
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::~AffectiveSegmentFilterTool()
{
}

//----------------------------------------------------------------------------
const UUID & AffectiveSegmentFilterTool::GetObjectID(void)
{
	return(myID);
}

//----------------------------------------------------------------------------
const char * AffectiveSegmentFilterTool::GetName(void)
{
	return(myName);
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::SupportsPush(void)
{
	return(true);
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::SupportsPull(void)
{
	return(true);
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterInterfaceABC *
AffectiveSegmentFilterTool::GetInterface(void)
{
	return(&m_Interface);
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::
SetGenericDSInterface(GenericDSInterfaceABC *aGenericDSInterface)
{
	m_GenericDSInterface = (GenericDSInterface *)aGenericDSInterface;
	return(0);
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::
ExecuteAffectiveFiltering(void)
{

	// extract scene and affective value list from theGenericDS
	if( !SetPrivateMembers() ){
		cerr << "Error: SetPrivateMembers() failed" << endl;
		return(false);
	};
	// filtering scene list
	if( !ExecuteFiltering() ){
		cerr << "Error: ExecuteFiltering() failed" << endl;
		return(false);
	};
	// create resulting summary and set it to theGenericDS
	if( !CreateSummaryDescription() ){
		cerr << "Error: CreateSummaryDescription() failed" << endl;
		return(false);
	};

	return(true);
}

//----------------------------------------------------------------------------
bool
AffectiveSegmentFilterTool::SetPrivateMembers(void)
{
	GenericDS theRootOfAudioVisualSegment, theRootOfAffective, theNode;

	// set root element
	GenericDS theRootNode = m_GenericDSInterface->GetFirstChild();

	// scan DOM tree, and find roots for <AudioVisual> and <Affective>
	GenericDS	currentRootNode = theRootNode.GetFirstChild();
	for(;;)
	{
		// Are the roots found in children?
		if( theRootOfAudioVisualSegment.isNull() )
		{
			theRootOfAudioVisualSegment
				= currentRootNode.GetDescription( "AudioVisual" );
		}
		if( theRootOfAffective.isNull() )
		{
			theRootOfAffective = currentRootNode.GetDescription( "Affective" );
		}

		// update currentRootNode
		GenericDS nextRootNode = currentRootNode.GetFirstChild();
		if( nextRootNode.isNull() )
		{
			nextRootNode = currentRootNode.GetNextSibling();
			if( nextRootNode.isNull() )
			{
				nextRootNode = (currentRootNode.GetParent()).GetNextSibling();
				while( nextRootNode.isNull() )
				{
					currentRootNode = currentRootNode.GetParent();
					if( currentRootNode == theRootNode )
					{
						break;
					}
					else
					{
						nextRootNode = (currentRootNode.GetParent()).GetNextSibling();
					}
				};

			}
		}
		if( currentRootNode == theRootNode )
		{
			break;
		}
		else
		{
			currentRootNode = nextRootNode;
		}
	};

	// set m_mediaTimeHeader, m_mediaLocator and m_sceneList
	if( !theRootOfAudioVisualSegment.isNull() )
	{
//	        {
//		  GenericDS ds_node=theRootOfAudioVisualSegment.GetDescription( "MediaLocator" );
//		  m_mediaLocator = (GenericDS &)ds_node;
//		}
		m_mediaLocator = theRootOfAudioVisualSegment.GetDescription( "MediaLocator" );
		m_sceneList = theRootOfAudioVisualSegment
						.GetDescription( "TemporalDecomposition" )
						.GetAllDescriptions( "AudioVisualSegment" );
	}
	else
	{
		cerr << "Error: AudioVisual not found\n";
		return(false);
	}

	// set m_affective
	if( !theRootOfAffective.isNull() )
	{
		m_affective.SetAffective( theRootOfAffective );
	}
	else
	{
		cerr << "Error: Affective not found\n";
		return(false);
	}

	// set other parameters
	m_introductionEndSceneNum = atoi( getpara( "introductionEndSceneNum", "0" ) );
	m_developmentEndSceneNum = atoi( getpara( "developmentEndSceneNum", "0" ) );
	m_turnEndSceneNum = atoi( getpara( "turnEndSceneNum", "0" ) );
	m_theSemanticScoreThreshold = atof( getpara( "theSemanticScoreThreshold", "0" ) );
	m_thePeakThreshold = atof( getpara( "thePeakThreshold", "0" ) );

	return(true);
}

//----------------------------------------------------------------------------
bool
AffectiveSegmentFilterTool::ExecuteFiltering()
{
	// set original length of theScoredSceneList
	int	theOriginalListSize = m_sceneList.size();
	int i;
	// detect peaks and valleys in Semantic Graph(SG)
	const int thePeak = 1;
	const int theValley = -1;
	const int none = 0;
	vector<int> theSGPeakAndValley( theOriginalListSize, none );

	for(i=1; i<theOriginalListSize-1; i++ )
	{
		// get score at [before|this|after]
		double beforeScore = m_affective.GetScoreValueAt( i-1 );
		double thisScore = m_affective.GetScoreValueAt( i );
		double afterScore = m_affective.GetScoreValueAt( i+1 );
		// detect the valley
		if( (thisScore<beforeScore) && (thisScore<afterScore) )
		{
			theSGPeakAndValley[i] = theValley;
		}
		// detect the peak
		if( (thisScore>beforeScore) && (thisScore>afterScore) )
		{
			theSGPeakAndValley[i] = thePeak;
		}
	}

	// find initial peak position and theScoreAt[Before|After]Valley
	double theValleyScoreBeforePeak = 0.0;
	double thePeakScore = 0.0;
	double theValleyScoreAfterPeak = 0.0;
	int thePeakPosition;
	for( i=0; i<theOriginalListSize; i++ )
	{
		if( theSGPeakAndValley[i]==thePeak )
		{
		        int j;
			thePeakPosition = i;
			thePeakScore = m_affective.GetScoreValueAt( i );
			// get initial theValleyScoreAfterPeak
			for(j=thePeakPosition; j<theOriginalListSize; j++ )
			{
				if( theSGPeakAndValley[j]==theValley )
				{
					theValleyScoreAfterPeak = m_affective.GetScoreValueAt( j );
					break;
				}
			}
			if( j==theOriginalListSize )
			{
				theValleyScoreAfterPeak = m_affective.GetScoreValueAt( j-1 );
			}
			// get initial theValleyScoreBeforePeak
			for( j=thePeakPosition; j>0; j-- )
			{
				if( theSGPeakAndValley[j]==theValley )
				{
					theValleyScoreBeforePeak = m_affective.GetScoreValueAt( j );
					break;
				}
			}
			if( j==0 )
			{
				theValleyScoreBeforePeak = m_affective.GetScoreValueAt( 0 );
			}
			break;
		}
	}

	// prune out non-prominant peaks
	for(;;)
	{
		// prone out peak if not prominant
		if( (fabs(theValleyScoreBeforePeak - thePeakScore) < m_thePeakThreshold)
			|| (fabs(theValleyScoreAfterPeak - thePeakScore) < m_thePeakThreshold) )
		{
			theSGPeakAndValley[thePeakPosition] = none;
		}
		else
		{
			thePeakPosition++;
		}
		// find the next peak
		while( (thePeakPosition<theOriginalListSize)
			&& (theSGPeakAndValley[thePeakPosition]!=thePeak) )
		{
			thePeakPosition++;
		}
		// break the loop if out of range
		if( thePeakPosition==theOriginalListSize )
		{
			break;
		}
		{
		int j;
		// set next thePeakScore, theValleyScore[Before|After]Peak
		thePeakScore = m_affective.GetScoreValueAt( thePeakPosition );
		theValleyScoreBeforePeak = theValleyScoreAfterPeak;
		for(j=thePeakPosition; j<theOriginalListSize; j++ )
		{
			if( theSGPeakAndValley[j]==theValley )
			{
				theValleyScoreAfterPeak = m_affective.GetScoreValueAt( j );
				break;
			}
		}
		if( j==theOriginalListSize )
		{
			theValleyScoreAfterPeak = m_affective.GetScoreValueAt( j-1 );
		}
		}
	}

	// re-calculate the semantic score
	vector<double> theSemanticScoreList( theOriginalListSize, 0.0 );
	double theMaxSemanticScore = m_affective.GetScoreValueAt( 0 );
	theSemanticScoreList[0] = theMaxSemanticScore;
	for( i=1; i<theOriginalListSize; i++ )
	{
		double prevScore = m_affective.GetScoreValueAt( i-1 );
		double thisScore = m_affective.GetScoreValueAt( i );
		double theSemanticScore = thisScore - prevScore;
		theSemanticScoreList[i] = theSemanticScore;
		if( fabs( theSemanticScore ) > theMaxSemanticScore )
		{
			theMaxSemanticScore = fabs( theSemanticScore );
		}
	}
	// normaized by 5
	for( i=0; i<theOriginalListSize-1; i++ )
	{
		theSemanticScoreList[i] *= (5/theMaxSemanticScore);
	}

	// set "false" for scenes to be kept
	vector<bool> isThisSceneFilteredOut( theOriginalListSize, true );
	for( i=0; i<theOriginalListSize; i++ )
	{
		// applied to "Ki", "Show" and "Ten"
		if( i<m_turnEndSceneNum )
		{
			if( theSGPeakAndValley[i] == thePeak )
			{
				isThisSceneFilteredOut[i] = false;
				// applied only to "Ki"
				if( i<m_introductionEndSceneNum )
				{
					isThisSceneFilteredOut[i+1] = false;
				}
			}
			if( theSemanticScoreList[i] > m_theSemanticScoreThreshold )
			{
				isThisSceneFilteredOut[i] = false;
			}
		}
	}

	// scene filtering for theScoreSceneList
	std::vector<GenericDS>::iterator theIterator = m_sceneList.end() - 1;
	for( i=theOriginalListSize-1; i>=0; i--, theIterator-- )
	{
		if(isThisSceneFilteredOut[i])
		{
			m_sceneList.erase( theIterator );
		}
	}

	// show result
	cout << "*** selected scenes ***" << endl;
	for( i=0; i<m_sceneList.size(); i++ )
	{
		string valStr;
		m_sceneList[i].GetTextAttribute( "id", valStr );
		cout << " - " << valStr.c_str() << endl;
	}
	cout << "Total Number of scene = " << i << endl;

	return(true);
}

//----------------------------------------------------------------------------
bool
AffectiveSegmentFilterTool::CreateSummaryDescription()
{
	// create root node for summary description
	GenericDS theSummary = GenericDS::CreateDescriptionRoot();

	// <Mpeg7>
	GenericDS theNode = theSummary.CreateChild( "Mpeg7" );
	theNode.SetAttribute( "xmlns", "urn:mpeg:mpeg7:schema:2001" );
	theNode.SetAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" );
	theNode.SetAttribute( "xsi:schemaLocation",
		"urn:mpeg:mpeg7:schema:2001 Mpeg7-2001.xsd" );

	// <DescriptionUnit>
	theNode = theNode.CreateChild( "DescriptionUnit" );
	theNode.SetAttribute( "xsi:type", "SummarizationType" );

	// <Summary>
	GenericDS theSummaryNode = theNode.CreateChild( "Summary" );
	theSummaryNode.SetAttribute( "xsi:type", "HierarchicalSummaryType" );
	theSummaryNode.SetAttribute( "hierarchy", "independent" );
	theSummaryNode.SetAttribute( "components", "keyAudioVisualClips" );
	theSummaryNode.SetAttribute( "timeBase", "SourceLocator[1]" );

	// Summayr/SourceLocator
	GenericDS theLocator = m_mediaLocator.GetMediaLocatorAsDescription( theSummaryNode, "SourceLocator" );
	theSummaryNode.AppendChild( theLocator );

	// Summayr/SummaryThemeList
	theNode = theSummaryNode.CreateChild( "SummaryThemeList" );
	theNode = theNode.CreateChild( "SummaryTheme" );
	theNode.SetAttribute( "id", "preview1" );
	theNode.SetValue( "Preview type movie summary" );

	// Summayr/SummarySegmentGroup
	GenericDS theSummarySegmentGroupNode = theSummaryNode.CreateChild( "SummarySegmentGroup" );
	double summaryLengthInSec = 0.0;
	for( int i=0; i<m_sceneList.size(); i++ )
	{
		// SummarySegmentGroup/SummarySegment
		GenericDS theSummarySegmentNode = theSummarySegmentGroupNode.CreateChild( "SummarySegment" );
		// SummarySegment/Name
		theNode = theSummarySegmentNode.CreateChild( "Name" );
		string	valStr;
		m_sceneList[i].GetTextAttribute( "id", valStr );
		theNode.SetValue( valStr );
		// SummarySegment/KeyAudioVisualClip
		theNode = theSummarySegmentNode.CreateChild( "KeyAudioVisualClip" );
		MediaTimeType mediaTime( m_sceneList[i].GetDescription( "MediaTime" ) );
		summaryLengthInSec += mediaTime.GetMediaDuration()->GetDurationInSec();
		theNode.AppendChild( mediaTime.GetMediaTimeAsDescription( theNode ) );
	}
	cout << "  Duration = " << summaryLengthInSec << " [sec]" << endl;

#ifdef USEXML3
	// 2014 fix for new Xerces-C++ library, only the interface to XML library should be used.
	m_GenericDSInterface->SetDocument(theSummary);
#elif  defined(USEXML)
	// set theSummaryDS to theGenericDS via interface 
	m_GenericDSInterface->SetDOMNode( theSummary.GetDocument() );
#endif //

	return(true);
}

//=============================================================================
AffectiveSegmentFilterTool::MediaDurationType::MediaDurationType()
{
	m_duration = 0.0;
	m_numOfFraction = 0;
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaDurationType::MediaDurationType(double aDuration)
{
	SetDuration( aDuration );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaDurationType::MediaDurationType(string aDuration)
{
	SetDuration( aDuration );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaDurationType::MediaDurationType(GenericDS& aDuration)
{
	SetDuration( aDuration );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaDurationType::~MediaDurationType()
{
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaDurationType::
SetDuration(double aDuration)
{
	m_duration = aDuration;
	m_numOfFraction = 0;
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaDurationType::
SetDuration(string aDuration)
{
	double	sign, day, hour, min, sec, subSec;
	char	valStr[16], c;
	int		i, vid;

	// set sign
	if( aDuration[0] == '-' )
	{
		sign = -1.0;
	}
	else
	{
		sign = 1.0;
	}

	// set day, hour, ...
	day = hour = min = sec = subSec = 0.0;
	i = vid = 0;
	for( i=0; i<aDuration.length(); i++ )
	{
		c = aDuration[i];
		if(isdigit(c))
		{
			valStr[vid++] = c ;
		}
		else if(c == 'D')
		{
			if(vid > 0)
			{
				valStr[vid] = '\0' ;
				day = atof(valStr) ;
				vid = 0 ;
			}
		}
		else if(c == 'H')
		{
			if(vid > 0)
			{
				valStr[vid] = '\0' ;
				hour = atof(valStr) ;
				vid = 0 ;
			}
		}
		else if(c == 'M')
		{
			if(vid > 0)
			{
				valStr[vid] = '\0' ;
				min = atof(valStr) ;
				vid = 0 ;
			}
		}
		else if(c == 'S')
		{
			if(vid > 0)
			{
				valStr[vid] = '\0' ;
				sec = atof(valStr) ;
				vid = 0 ;
			}
		}
		else if(c == 'N')
		{
			if(vid > 0)
			{
				valStr[vid] = '\0' ;
				subSec = atof(valStr) ;
				vid = 0 ;
			}
		}
		else if(c == 'F')
		{
			if(vid > 0)
			{
				valStr[vid] = '\0' ;
				m_numOfFraction = atoi(valStr) ;
				vid = 0 ;
			}
		}
	}

	// set m_duration
	m_duration = ((day*24 + hour)*60 + min)*60 + sec + subSec/(double)m_numOfFraction;
	m_duration *= sign;
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaDurationType::
SetDuration(GenericDS& aDuration)
{
	MediaDurationType dummy;
	string valStr;

	aDuration.GetTextValue( valStr );
	dummy.SetDuration( valStr );
	m_duration = dummy.GetDurationInSec();
	m_numOfFraction = dummy.GetNumOfFraction();
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaDurationType::
SetNumOfFraction(int aNumOfFraction)
{
	if( aNumOfFraction < 1 )
	{
		return( false );
	}
	m_numOfFraction = aNumOfFraction;
	return( true );
}

//----------------------------------------------------------------------------
double AffectiveSegmentFilterTool::MediaDurationType::
GetDurationInSec(void)
{
	return( m_duration );
}

//----------------------------------------------------------------------------
string AffectiveSegmentFilterTool::MediaDurationType::
GetDurationAsString(void)
{
	int	day, hour, min, sec, subSec;
	long	durIntValue;
	double	durFracValue;
	string	durString;

	// set sign
	if( m_duration > 0.0 )
	{
		durString = "P";
		durIntValue = (long)m_duration;
		durFracValue = m_duration - (double)durIntValue;
	}
	else
	{
		durString = "-P";
		durIntValue = (long)(-1.0*m_duration);
		durFracValue = -1.0*( m_duration + (double)durIntValue );

	}

	// set fraction
	if( m_numOfFraction == 0 )
	{
		char	valStr[8];
		int i;
		// find # of digit after decimal point
		sprintf( valStr, "%.5f", durFracValue );
		for(i=6; i>=0; i-- )	// ignore valStr[7]
		{
			if( valStr[i] == '0' )
			{
				valStr[i] = '\0';
			}
			else
			{
				break;
			}
		}
		// set initial m_numOfFraction as 10^x
		for( m_numOfFraction=1, i=0; i<strlen(valStr)-2; i++ )
		{
			m_numOfFraction *= 10;
		}
		subSec = (long)(durFracValue * (double)m_numOfFraction + 0.5);
		if(subSec == m_numOfFraction)
		{
			subSec = 0;
			durIntValue++;
		}
		else // reduced to lowest term
		{
			// Euclid algorithem
			int	x = m_numOfFraction;
			int	y = subSec;
			while( y != 0 )
			{
				int t = x % y;
				x = y;
				y = t;
			}
			m_numOfFraction /= x;
			subSec /= x;
		}
	}
	else
	{
		subSec = (long)(durFracValue * (double)m_numOfFraction + 0.5);
		if(subSec == m_numOfFraction)
		{
			subSec = 0;
			durIntValue++;
		}
	}

	// set sec, min, ...
	sec = durIntValue % 60;
	min = (durIntValue/60) % 60;
	hour = (durIntValue/3600) % 24;
	day = durIntValue/86400;
	char	valStr[16];
	// eventually 0 sec
	if( day==0 && hour==0 && min==0 && sec==0 && subSec==0 )
	{
		durString = "PT0S";
		return( durString );
	}
	// else
	if( day > 0 )
	{
		sprintf(valStr, "%dD", day);
		durString += string(valStr);
	}
	durString += string( "T" );
	if(hour > 0)
	{
		sprintf(valStr, "%dH", hour);
		durString += string(valStr);
	}
	if(min > 0)
	{
		sprintf(valStr, "%dM", min);
		durString += string(valStr);
	}
	if(sec > 0)
	{
		sprintf(valStr, "%dS", sec);
		durString += string(valStr);
	}
	if(subSec > 0)
	{
		sprintf(valStr, "%dN%dF", subSec, m_numOfFraction);
		durString += string(valStr);
	}
	
	return( durString );
}

//----------------------------------------------------------------------------
GenericDS AffectiveSegmentFilterTool::MediaDurationType::
GetDurationAsDescription(GenericDS& root, string tagName)
{
	MediaDurationType dummy( m_duration );
	dummy.SetNumOfFraction( m_numOfFraction );
	string	valStr = dummy.GetDurationAsString();
	GenericDS	description = GenericDS::CreateDescription( root, tagName );
	description.SetValue( valStr );

	return( description );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaDurationType::
GetNumOfFraction(void)
{
	return( m_numOfFraction );
}

//=============================================================================
AffectiveSegmentFilterTool::MediaRelTimePointType::MediaRelTimePointType()
{
	m_timeBase = string();
	m_relTimePoint = 0.0;
	m_numOfFraction = 0;
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaRelTimePointType::MediaRelTimePointType(double aRelTimePoint)
{
	SetRelTimePoint( aRelTimePoint );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaRelTimePointType::MediaRelTimePointType(string aRelTimePoint)
{
	SetRelTimePoint( aRelTimePoint );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaRelTimePointType::MediaRelTimePointType(GenericDS& aRelTimePoint)
{
	SetRelTimePoint( aRelTimePoint );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaRelTimePointType::~MediaRelTimePointType()
{
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaRelTimePointType::
SetRelTimePoint(double aRelTimePoint)
{
	m_timeBase = string();
	m_relTimePoint = aRelTimePoint;
	m_numOfFraction = 0;
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaRelTimePointType::
SetRelTimePoint(string aRelTimePoint)
{
	m_timeBase = string();
	MediaDurationType dummy( aRelTimePoint );
	m_relTimePoint = dummy.GetDurationInSec();
	m_numOfFraction = dummy.GetNumOfFraction();
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaRelTimePointType::
SetRelTimePoint(GenericDS& aRelTimePoint)
{
	MediaDurationType dummy;
	string valStr;

	aRelTimePoint.GetTextValue( valStr );
	dummy.SetDuration( valStr );
	m_relTimePoint = dummy.GetDurationInSec();
	m_numOfFraction = dummy.GetNumOfFraction();
	if( aRelTimePoint.GetTextAttribute( "timeBase", valStr ) != GenericDS::NO_SUCH_ATTR )
	{
		m_timeBase = valStr;
	}
	else
	{
		m_timeBase = string();
	}
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaRelTimePointType::
SetNumOfFraction(int aNumOfFraction)
{
	if( aNumOfFraction < 1 )
	{
		return( false );
	}
	m_numOfFraction = aNumOfFraction;

	return( true );
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaRelTimePointType::
SetTimeBase(string aTimeBase)
{
	m_timeBase = aTimeBase;
}

//----------------------------------------------------------------------------
double AffectiveSegmentFilterTool::MediaRelTimePointType::
GetRelTimePointInSec(void)
{
	return( m_relTimePoint );
}

//----------------------------------------------------------------------------
string AffectiveSegmentFilterTool::MediaRelTimePointType::
GetRelTimePointAsString(void)
{
	MediaDurationType dummy( m_relTimePoint );
	dummy.SetNumOfFraction( m_numOfFraction );

	return( dummy.GetDurationAsString() );
}

//----------------------------------------------------------------------------
GenericDS AffectiveSegmentFilterTool::MediaRelTimePointType::
GetRelTimePointAsDescription(GenericDS& root, string tagName )
{
	MediaDurationType dummy( m_relTimePoint );
	dummy.SetNumOfFraction( m_numOfFraction );
	string	valStr = dummy.GetDurationAsString();
	GenericDS	description = GenericDS::CreateDescription( root, tagName );
	if( !m_timeBase.empty() )
	{
		description.SetAttribute( "timeBase", m_timeBase );
	}
	description.SetValue( valStr );

	return( description );
}
//----------------------------------------------------------------------------
string AffectiveSegmentFilterTool::MediaRelTimePointType::
GetTimeBase(void)
{
	return( m_timeBase );
}

//=============================================================================
AffectiveSegmentFilterTool::MediaTimePointType::MediaTimePointType()
{
	m_year = m_month = m_day = m_hour = m_minute = m_second = m_fraction
	= m_numOfFraction = 0;
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaTimePointType::MediaTimePointType(string aTimePoint)
{
	SetTimePoint( aTimePoint );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaTimePointType::MediaTimePointType(GenericDS& aTimePoint)
{
	SetTimePoint( aTimePoint );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaTimePointType::~MediaTimePointType()
{
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaTimePointType::
SetTimePoint(string aTimePoint)
{
	char	valStr[16];

	// initialize
	m_year = m_month = m_day = m_hour = m_minute = m_second = m_fraction
	= m_numOfFraction = 0;

	int i = 0;
	// get YYYY
	if( isdigit(aTimePoint[i]) )
	{
		int vid = 0;
		while( isdigit(aTimePoint[i]) )
		{
			valStr[vid++] = aTimePoint[i];
			i++;
		}
		valStr[vid] ='\0';
		m_year = atoi(valStr);
	}
	// get MM
	if( aTimePoint[i] == '-' )
	{
		int vid = 0;
		i++;
		while( isdigit(aTimePoint[i]) )
		{
			valStr[vid++] = aTimePoint[i];
			i++;
		}
		valStr[vid] ='\0';
		m_month = atoi(valStr);
	}
	// get DD
	if( aTimePoint[i] == '-' )
	{
		int vid = 0;
		i++;
		while( isdigit(aTimePoint[i]) )
		{
			valStr[vid++] = aTimePoint[i];
			i++;
		}
		valStr[vid] ='\0';
		m_day = atoi(valStr);
	}
	// get hh
	if( aTimePoint[i] == 'T' )
	{
		int vid = 0;
		i++;
		while( isdigit(aTimePoint[i]) )
		{
			valStr[vid++] = aTimePoint[i];
			i++;
		}
		valStr[vid] ='\0';
		m_hour = atoi(valStr);
	}
	// get mm
	if( aTimePoint[i] == ':' )
	{
		int vid = 0;
		i++;
		while( isdigit(aTimePoint[i]) )
		{
			valStr[vid++] = aTimePoint[i];
			i++;
		}
		valStr[vid] ='\0';
		m_minute = atoi(valStr);
	}
	// get ss
	if( aTimePoint[i] == ':' )
	{
		int vid = 0;
		i++;
		while( isdigit(aTimePoint[i]) )
		{
			valStr[vid++] = aTimePoint[i];
			i++;
		}
		valStr[vid] ='\0';
		m_second = atoi(valStr);
	}
	// get nn/NN
	if( aTimePoint[i] == ':' )
	{
		int vid = 0;
		i++;
		while( isdigit(aTimePoint[i]) )
		{
			valStr[vid++] = aTimePoint[i];
			i++;
		}
		valStr[vid] ='\0';
		m_fraction = atoi(valStr);
		i++;	// skip 'F'
		vid = 0;
		while( isdigit(aTimePoint[i]) )
		{
			valStr[vid++] = aTimePoint[i];
			i++;
		}
		valStr[vid] ='\0';
		m_numOfFraction = atoi(valStr);
	}
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaTimePointType::
SetTimePoint(GenericDS& aTimePoint)
{
	MediaTimePointType dummy;
	string valStr;

	aTimePoint.GetTextValue( valStr );
	dummy.SetTimePoint( valStr );
	m_year = dummy.GetYear();
	m_month = dummy.GetMonth();
	m_day = dummy.GetDay();
	m_hour = dummy.GetHour();
	m_minute = dummy.GetMinute();
	m_second = dummy.GetSecond();
	m_fraction = dummy.GetFraction();
	m_numOfFraction = dummy.GetNumOfFraction();
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimePointType::
SetYear(int aYear)
{
	m_year = aYear;
	return( true );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimePointType::
SetMonth(int aMonth)
{
	if( aMonth < 1 || aMonth > 12 )
	{
		return( false );
	}
	m_month = aMonth;

	return( true );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimePointType::
SetDay(int aDay)
{
	if( aDay < 1 || aDay > 31 )
	{
		return( false );
	}
	m_day = aDay;

	return( true );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimePointType::
SetHour(int anHour)
{
	if( anHour < 1 || anHour > 24 )
	{
		return( false );
	}
	m_hour = anHour;

	return( true );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimePointType::
SetMinute(int aMinute)
{
	if( aMinute < 1 || aMinute > 60 )
	{
		return( false );
	}
	m_minute = aMinute;

	return( true );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimePointType::
SetSecond(int aSecond)
{
	if( aSecond < 1 || aSecond > 60 )
	{
		return( false );
	}
	m_second = aSecond;

	return( true );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimePointType::
SetFraction(int aFraction)
{
	if( aFraction < 1 || aFraction >= m_numOfFraction )
	{
		return( false );
	}
	m_fraction = aFraction;

	return( true );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimePointType::
SetNumOfFraction(int aNumOfFraction)
{
	if( aNumOfFraction < 1 )
	{
		return( false );
	}
	m_fraction = (int)((double)m_fraction/(double)m_numOfFraction*(double)aNumOfFraction + 0.5 );
	m_numOfFraction = aNumOfFraction;

	return( true );
}

//----------------------------------------------------------------------------
string AffectiveSegmentFilterTool::MediaTimePointType::
GetTimePointAsString(void)
{
	string timePointStr;
	char valStr[32];

	// YYYY
	if( m_year!=0 )
	{
		sprintf( valStr, "%04d", m_year );
		timePointStr = string(valStr);
	}
	else
	{
		return( string("") );
	}
	// YYYY-MM
	if( m_month!=0 )
	{
		sprintf( valStr, "-%d", m_month );
		timePointStr += string(valStr);
	}
	else
	{
		return( timePointStr );
	}
	// YYYY-MM-DD
	if( m_day != 0 )
	{
		sprintf( valStr, "-%d", m_day );
		timePointStr += string(valStr);
	}
	else
	{
		return( timePointStr );
	}
	// YYYY-MM-DDThh
	if( m_hour!=0 || m_minute!=0 || m_second!=0 || m_fraction!=0 )
	{
		sprintf( valStr, "T%02d", m_hour );
		timePointStr += string(valStr);
		// YYYY-MM-DDThh:mm
		if( m_minute!=0 || m_second!=0 || m_fraction!=0 )
		{
			sprintf( valStr, ":%02d", m_minute );
			timePointStr += string(valStr);
			// YYYY-MM-DDThh:mm:ss
			if( m_second!=0 || m_fraction!=0 )
			{
				sprintf( valStr, ":%02d", m_second );
				timePointStr += string(valStr);
				// YYYY-MM-DDThh:mm:ss:nnnFNNN
				if( m_fraction!=0 )
				{
					sprintf( valStr, ":%dF%d", m_fraction, m_numOfFraction );
					timePointStr += string(valStr);
				}
			}
		}
	}

	return( timePointStr );
}

//----------------------------------------------------------------------------
GenericDS AffectiveSegmentFilterTool::MediaTimePointType::
GetTimePointAsDescription(GenericDS& root, string tagName)
{
	MediaTimePointType dummy;
	dummy.SetYear( m_year );
	dummy.SetMonth( m_month );
	dummy.SetDay( m_day );
	dummy.SetHour( m_hour );
	dummy.SetMinute( m_minute );
	dummy.SetSecond( m_second );
	dummy.SetNumOfFraction( m_numOfFraction );
	dummy.SetFraction( m_fraction );
	string	valStr = dummy.GetTimePointAsString();
	GenericDS	description = GenericDS::CreateDescription( root, tagName );
	description.SetValue( valStr );

	return( description );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaTimePointType::
GetYear(void)
{
	return( m_year );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaTimePointType::
GetMonth(void)
{
	return( m_month );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaTimePointType::
GetDay(void)
{
	return( m_day );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaTimePointType::
GetHour(void)
{
	return( m_hour );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaTimePointType::
GetMinute(void)
{
	return( m_minute );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaTimePointType::
GetSecond(void)
{
	return( m_second );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaTimePointType::
GetFraction(void)
{
	return( m_fraction );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaTimePointType::
GetNumOfFraction(void)
{
	return( m_numOfFraction );
}

//=============================================================================
AffectiveSegmentFilterTool::MediaTimeType::MediaTimeType()
{
	m_mediaTimePoint = (MediaTimePointType *)NULL;
	m_mediaRelTimePoint = (MediaRelTimePointType *)NULL;
	m_mediaDuration = (MediaDurationType *)NULL;
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaTimeType::MediaTimeType(GenericDS aMediaTime)
{
	m_mediaTimePoint = (MediaTimePointType *)NULL;
	m_mediaRelTimePoint = (MediaRelTimePointType *)NULL;
	m_mediaDuration = (MediaDurationType *)NULL;
	SetMediaTime( aMediaTime );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaTimeType::~MediaTimeType()
{
	if( m_mediaTimePoint != (MediaTimePointType *)NULL )
	{
		delete m_mediaTimePoint;
	}
	if( m_mediaRelTimePoint != (MediaRelTimePointType *)NULL )
	{
		delete m_mediaRelTimePoint;
	}
	if( m_mediaDuration != (MediaDurationType *)NULL )
	{
		delete m_mediaDuration;
	}
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaTimeType::
SetMediaTime(GenericDS& aMediaTime)
{
	// set time point
	GenericDS theTimePointNode = aMediaTime.GetFirstChild();
	if( !strcmp(theTimePointNode.GetDSName().c_str(), "MediaTimePoint") )
	{
		MediaTimePointType theMediaTimePoint( theTimePointNode );
		m_mediaTimePoint = new MediaTimePointType();
		*m_mediaTimePoint = theMediaTimePoint;
	}
	else if( !strcmp(theTimePointNode.GetDSName().c_str(), "MediaRelTimePoint") )
	{
		MediaRelTimePointType theMediaRelTimePoint( theTimePointNode );
		m_mediaRelTimePoint = new MediaRelTimePointType();
		*m_mediaRelTimePoint = theMediaRelTimePoint;
	}

	// set duration
	GenericDS theDurationNode = theTimePointNode.GetNextSibling();
	if( !strcmp(theDurationNode.GetDSName().c_str(), "MediaDuration") )
	{
		MediaDurationType theMediaDuration( theDurationNode );
		m_mediaDuration = new MediaDurationType();
		*m_mediaDuration = theMediaDuration;
	}
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimeType::
SetMediaTimePoint(MediaTimePointType aMediaTimePoint)
{
	if( (m_mediaTimePoint!=(MediaTimePointType *)NULL) ||
		(m_mediaRelTimePoint!=(MediaRelTimePointType *)NULL) )
	{
		return( false );
	}

	m_mediaTimePoint = new MediaTimePointType();
	*m_mediaTimePoint = aMediaTimePoint;

	return( true );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimeType::
SetMediaRelTimePoint(MediaRelTimePointType aMediaRelTimePoint)
{
	if( (m_mediaTimePoint!=(MediaTimePointType *)NULL) ||
		(m_mediaRelTimePoint!=(MediaRelTimePointType *)NULL) )
	{
		return( false );
	}

	m_mediaRelTimePoint = new MediaRelTimePointType();
	*m_mediaRelTimePoint = aMediaRelTimePoint;

	return( true );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::MediaTimeType::
SetMediaDuration(MediaDurationType aMediaDuration)
{
	m_mediaDuration = new MediaDurationType();
	*m_mediaDuration = aMediaDuration;

	return( true );
}

//----------------------------------------------------------------------------
GenericDS AffectiveSegmentFilterTool::MediaTimeType::
GetMediaTimeAsDescription(GenericDS& root, string tagName)
{
	GenericDS	description = GenericDS::CreateDescription( root, tagName );

	if( m_mediaTimePoint != (MediaTimePointType *)NULL )
	{
		description.AppendChild( m_mediaTimePoint->GetTimePointAsDescription(root) );
	}
	if( m_mediaRelTimePoint != (MediaRelTimePointType *)NULL )
	{
		description.AppendChild( m_mediaRelTimePoint->GetRelTimePointAsDescription(root) );
	}
	if( m_mediaDuration != (MediaDurationType *)NULL )
	{
		description.AppendChild( m_mediaDuration->GetDurationAsDescription(root) );
	}

	return( description );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaTimePointType* AffectiveSegmentFilterTool::MediaTimeType::
GetMediaTimePoint(void)
{
	return ( m_mediaTimePoint );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaRelTimePointType* AffectiveSegmentFilterTool::MediaTimeType::
GetMediaRelTimePoint(void)
{
	return ( m_mediaRelTimePoint );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaDurationType* AffectiveSegmentFilterTool::MediaTimeType::
GetMediaDuration(void)
{
	return ( m_mediaDuration );
}

//=============================================================================
AffectiveSegmentFilterTool::AffectiveType::AffectiveType()
{
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::AffectiveType::AffectiveType(GenericDS& anAffective)
{
	SetAffective( anAffective );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::AffectiveType::~AffectiveType()
{
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::AffectiveType::
SetAffective(GenericDS& anAffective)
{
	string valStr;

	anAffective.GetTextAttribute( "id", valStr );
	if( valStr.size() > 0 )
	{
		m_id = valStr;
	}

	GenericDS theTypeDescription = anAffective.GetFirstChild();
	while( strcmp(theTypeDescription.GetDSName().c_str(), "Type") )
	{
		theTypeDescription = theTypeDescription.GetNextSibling();
	};
	theTypeDescription.GetFirstChild().GetTextValue( m_typeName );
	theTypeDescription.GetTextAttribute( "href", valStr );
	if( valStr.size() > 0 )
	{
		m_typeHref = valStr;
	}

	GenericDS theScoreDescription = theTypeDescription.GetNextSibling();
	while( !theScoreDescription.isNull() )
	{
		theScoreDescription.GetTextAttribute( "idref", valStr );
		double value;
		theScoreDescription.GetRealValue( value );
		SetScore( valStr.c_str(), value );
		theScoreDescription = theScoreDescription.GetNextSibling();
	}
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::AffectiveType::
SetId(string id)
{
	m_id = id;
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::AffectiveType::
SetType(string typeName, string href)
{
	m_typeName = typeName;
	m_typeHref = href;
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::AffectiveType::
SetScore(string ref, double value)
{
	m_scoreRefList.push_back( ref );
	m_scoreValueList.push_back( value );
}

//----------------------------------------------------------------------------
bool AffectiveSegmentFilterTool::AffectiveType::
SetScoreAt(int index, string ref, double value)
{
	if( index >= m_scoreRefList.size() )
	{
		return( false );
	}

	m_scoreRefList[index] = ref;
	m_scoreValueList[index] = value;

	return( true );
}

//----------------------------------------------------------------------------
GenericDS AffectiveSegmentFilterTool::AffectiveType::
GetAffectiveAsDescription(GenericDS& root, string tagName)
{
	GenericDS	description[3];
	char textValue[64];

	// Affective
	description[0] = GenericDS::CreateDescription( root, tagName );
	description[0].SetAttribute( "id", m_id );

	// Type
	AffectiveType affective;
	affective.SetType( m_typeName, m_typeHref );
	description[0].AppendChild( affective.GetTypeAsDescription( root ) );

	// Score
	for( int i=0; i<m_scoreValueList.size(); i++ )
	{
		description[1] = GenericDS::CreateDescription( root, "Score" );
		description[1].SetAttribute( "idref", m_scoreRefList[i] );
		sprintf( textValue, "%g", m_scoreValueList[i] );
		description[1].SetValue( string(textValue) );
		description[0].AppendChild( description[1] );
	}

	return( description[0] );
}

//----------------------------------------------------------------------------
string AffectiveSegmentFilterTool::AffectiveType::
GetId(void)
{
	return( m_id );
}

//----------------------------------------------------------------------------
string AffectiveSegmentFilterTool::AffectiveType::
GetTypeName(void)
{
	return( m_typeName );
}

//----------------------------------------------------------------------------
GenericDS AffectiveSegmentFilterTool::AffectiveType::
GetTypeAsDescription(GenericDS& root)
{
	GenericDS	description[2];

	// Type
	description[0] = GenericDS::CreateDescription( root, "Type" );
	description[0].SetAttribute( "href", m_typeHref );
	description[1] = GenericDS::CreateDescription( root, "Name" );
	description[1].SetValue( m_typeName );
	description[0].AppendChild( description[1] );

	return( description[0] );
}

//----------------------------------------------------------------------------
double AffectiveSegmentFilterTool::AffectiveType::
GetScoreValueAt(int index)
{
	return( m_scoreValueList[index] );
}

//----------------------------------------------------------------------------
string AffectiveSegmentFilterTool::AffectiveType::
GetScoreRefAt(int index)
{
	return( m_scoreRefList[index] );
}

//=============================================================================
AffectiveSegmentFilterTool::MediaLocatorType::MediaLocatorType()
{
	m_streamId = -1;
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaLocatorType::MediaLocatorType(GenericDS aMediaLocator)
{
	SetMediaLocator( aMediaLocator );
}

//----------------------------------------------------------------------------
AffectiveSegmentFilterTool::MediaLocatorType::~MediaLocatorType()
{
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaLocatorType::
SetMediaLocator(GenericDS& anMediaLocator)
{
	string valStr;

	GenericDS theMediaUriDescription = anMediaLocator.GetFirstChild();
	theMediaUriDescription.GetTextValue( valStr );
	if( valStr.size() > 0 )
	{
		m_mediaUri = valStr;
	}

	GenericDS theStreamIdDescription = theMediaUriDescription.GetNextSibling();
	if( !theStreamIdDescription.isNull() )
	{
		theStreamIdDescription.GetIntValue( m_streamId );
	}
	else
	{
		m_streamId = -1;
	}
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaLocatorType::
SetMediaUri(string aMediaUri)
{
	m_mediaUri = aMediaUri;
}

//----------------------------------------------------------------------------
void AffectiveSegmentFilterTool::MediaLocatorType::
SetStreamId(int aStreamId)
{
	m_streamId = aStreamId;
}

//----------------------------------------------------------------------------
GenericDS AffectiveSegmentFilterTool::MediaLocatorType::
GetMediaLocatorAsDescription(GenericDS& root, string tagName)
{
	GenericDS	description[2];
	char textValue[64];

	// set MediaLocator
	description[0] = GenericDS::CreateDescription( root, tagName );

	// set MediaUri
	description[1] = GenericDS::CreateDescription( root, "MediaUri" );
	description[1].SetValue( m_mediaUri.c_str() );
	description[0].AppendChild( description[1] );

	// set StreamId
	if( m_streamId >= 0 )
	{
		description[1] = GenericDS::CreateDescription( root, "StreamID" );
		sprintf( textValue, "%d", m_streamId );
		description[1].SetValue( string(textValue) );
		description[0].AppendChild( description[1] );
	}

	return( description[0] );
}

//----------------------------------------------------------------------------
string AffectiveSegmentFilterTool::MediaLocatorType::
GetMediaUri(void)
{
	return( m_mediaUri );
}

//----------------------------------------------------------------------------
int AffectiveSegmentFilterTool::MediaLocatorType::
GetStreamId(void)
{
	return( m_streamId );
}
