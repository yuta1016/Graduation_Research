// DlgProxy.cpp : implementation file
//

#include "stdafx.h"
#include "XMManager.h"                                      /* Naming Convention */
#include "DlgProxy.h"
#include "XMManagerDlg.h"                                   /* Naming Convention */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXMManagerDlgAutoProxy

IMPLEMENT_DYNCREATE(CXMManagerDlgAutoProxy, CCmdTarget)

CXMManagerDlgAutoProxy::CXMManagerDlgAutoProxy()
{
	EnableAutomation();
	
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();

	// Get access to the dialog through the application's
	//  main window pointer.  Set the proxy's internal pointer
	//  to point to the dialog, and set the dialog's back pointer to
	//  this proxy.
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF(CXMManagerDlg, AfxGetApp()->m_pMainWnd);
	m_pDialog = (CXMManagerDlg*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

CXMManagerDlgAutoProxy::~CXMManagerDlgAutoProxy()
{
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	//  Among other things, this will destroy the main dialog
	AfxOleUnlockApp();
}

void CXMManagerDlgAutoProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTarget::OnFinalRelease();
}

BEGIN_MESSAGE_MAP(CXMManagerDlgAutoProxy, CCmdTarget)
	//{{AFX_MSG_MAP(CXMManagerDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CXMManagerDlgAutoProxy, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CXMManagerDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IXMManager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {D5CF7FC7-D53E-11D2-8859-00A0C9CE7A91}
static const IID IID_IXMManager =
{ 0xd5cf7fc7, 0xd53e, 0x11d2, { 0x88, 0x59, 0x0, 0xa0, 0xc9, 0xce, 0x7a, 0x91 } };

BEGIN_INTERFACE_MAP(CXMManagerDlgAutoProxy, CCmdTarget)
	INTERFACE_PART(CXMManagerDlgAutoProxy, IID_IXMManager, Dispatch)
END_INTERFACE_MAP()

// The IMPLEMENT_OLECREATE2 macro is defined in StdAfx.h of this project
// {D5CF7FC5-D53E-11D2-8859-00A0C9CE7A91}
IMPLEMENT_OLECREATE2(CXMManagerDlgAutoProxy, "XMManager.Application", 0xd5cf7fc5, 0xd53e, 0x11d2, 0x88, 0x59, 0x0, 0xa0, 0xc9, 0xce, 0x7a, 0x91)

/////////////////////////////////////////////////////////////////////////////
// CXMManagerDlgAutoProxy message handlers
