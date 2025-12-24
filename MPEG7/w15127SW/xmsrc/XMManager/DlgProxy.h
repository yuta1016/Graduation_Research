// DlgProxy.h : header file
//

#if !defined(AFX_DLGPROXY_H__D5CF7FCE_D53E_11D2_8859_00A0C9CE7A91__INCLUDED_)
#define AFX_DLGPROXY_H__D5CF7FCE_D53E_11D2_8859_00A0C9CE7A91__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CXMManagerDlg;

/////////////////////////////////////////////////////////////////////////////
// CXMManagerDlgAutoProxy command target

class CXMManagerDlgAutoProxy : public CCmdTarget
{
	DECLARE_DYNCREATE(CXMManagerDlgAutoProxy)

	CXMManagerDlgAutoProxy();           // protected constructor used by dynamic creation

// Attributes
public:
	CXMManagerDlg* m_pDialog;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXMManagerDlgAutoProxy)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CXMManagerDlgAutoProxy();

	// Generated message map functions
	//{{AFX_MSG(CXMManagerDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE(CXMManagerDlgAutoProxy)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CXMManagerDlgAutoProxy)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROXY_H__D5CF7FCE_D53E_11D2_8859_00A0C9CE7A91__INCLUDED_)
