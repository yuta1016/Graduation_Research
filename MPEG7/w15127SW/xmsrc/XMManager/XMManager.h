// XM Manager.h : main header file for the XM MANAGER application
//

#if !defined(AFX_XMMANAGER_H__D5CF7FCA_D53E_11D2_8859_00A0C9CE7A91__INCLUDED_)
#define AFX_XMMANAGER_H__D5CF7FCA_D53E_11D2_8859_00A0C9CE7A91__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CXMManagerApp:
// See XM Manager.cpp for the implementation of this class
//

class CXMManagerApp : public CWinApp
{
public:
	CXMManagerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXMManagerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CXMManagerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XMMANAGER_H__D5CF7FCA_D53E_11D2_8859_00A0C9CE7A91__INCLUDED_)
