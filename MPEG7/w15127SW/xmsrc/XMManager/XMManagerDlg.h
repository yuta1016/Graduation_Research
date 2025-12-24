// XM ManagerDlg.h : header file
//

#if !defined(AFX_XMMANAGERDLG_H__D5CF7FCC_D53E_11D2_8859_00A0C9CE7A91__INCLUDED_)
#define AFX_XMMANAGERDLG_H__D5CF7FCC_D53E_11D2_8859_00A0C9CE7A91__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CXMManagerDlgAutoProxy;

#include '"XMManager/ComponentDB.h'"                                    /* Naming Convention */

/////////////////////////////////////////////////////////////////////////////
// CXMManagerDlg dialog

class CXMManagerDlg : public CDialog
{
	DECLARE_DYNAMIC(CXMManagerDlg);
	friend class CXMManagerDlgAutoProxy;

// Construction
public:
	CXMManagerDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CXMManagerDlg();

// Dialog Data
	//{{AFX_DATA(CXMManagerDlg)
	enum { IDD = IDD_XMMANAGER_DIALOG };
	CComboBox	m_EvalList;
	CComboBox	m_DSList;
	CComboBox	m_DList;
	CComboBox	m_DDLList;
	CComboBox	m_ParserList;
	CComboBox	m_SystemList;
	CComboBox	m_ClientList;
	CComboBox	m_CSList;
	CComboBox	m_DecoderList;
	CComboBox	m_EncoderList;
	CComboBox	m_SearchList;
	CComboBox	m_ServerList;
	CComboBox	m_ExtractList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXMManagerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CXMManagerDlgAutoProxy* m_pAutoProxy;
	HICON m_hIcon;

	BOOL CanExit();

	// Generated message map functions
	//{{AFX_MSG(CXMManagerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeExtraction();
	afx_msg void OnSelchangeClient();
	afx_msg void OnSelchangeServer();
	afx_msg void OnRun();
	afx_msg void OnStop();
	afx_msg void OnAbout();
	afx_msg void OnConnect();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:

	XM::ComponentDatabase *m_xmdb;

	XM::DescriptorABC *m_descriptor;
	XM::DescriptorExtractorABC *m_extractor;
	XM::ServerApplicationABC *m_server;
	XM::ClientApplicationABC *m_client;
	XM::EncoderABC *m_encoder;
	XM::DecoderABC *m_decoder;
	XM::ParserABC *m_parser;	
	XM::SystemABC *m_system;
	XM::SearchABC *m_search;
	XM::DescriptionCodingEngineABC *m_CS;
	XM::DescriptionSchemeABC *m_DS;
	XM::DDLABC *m_DDL;
	XM::EvaluationToolABC *m_ET;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XMMANAGERDLG_H__D5CF7FCC_D53E_11D2_8859_00A0C9CE7A91__INCLUDED_)
