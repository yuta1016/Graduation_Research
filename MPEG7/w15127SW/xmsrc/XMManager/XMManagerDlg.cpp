// XM ManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "XMManager.h"                                      /* Naming Convention */
#include "XMManagerDlg.h"                                   /* Naming Convention */
#include "DlgProxy.h" 


#include "XMManager/ComponentDB.h"                                    /* Naming Convention */

#include "../EvaluationTools/EvaluationTools.h"             /* Naming Convention */
#include "../Utilities/UUIDs.h"                             /* Naming Convention */

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_Copyright;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_Copyright = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_COPYRIGHT, m_Copyright);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXMManagerDlg dialog

IMPLEMENT_DYNAMIC(CXMManagerDlg, CDialog);

CXMManagerDlg::CXMManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXMManagerDlg::IDD, pParent),
	m_xmdb(0),
	m_descriptor(0),
	m_extractor(0),
	m_server(0),
	m_client(0),
	m_encoder(0),
	m_decoder(0),
	m_parser(0),
	m_system(0),
	m_search(0),
	m_CS(0),
	m_DS(0)
{
	//{{AFX_DATA_INIT(CXMManagerDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAutoProxy = NULL;
}

CXMManagerDlg::~CXMManagerDlg()
{
	// If there is an automation proxy for this dialog, set
	//  its back pointer to this dialog to NULL, so it knows
	//  the dialog has been deleted.
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;

	delete m_xmdb;
	m_xmdb = 0;
}

void CXMManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CXMManagerDlg)
	DDX_Control(pDX, IDC_EVAL, m_EvalList);
	DDX_Control(pDX, IDC_DS, m_DSList);
	DDX_Control(pDX, IDC_D, m_DList);
	DDX_Control(pDX, IDC_DDL, m_DDLList);
	DDX_Control(pDX, IDC_PARSER, m_ParserList);
	DDX_Control(pDX, IDC_SYSTEM, m_SystemList);
	DDX_Control(pDX, IDC_CLIENT, m_ClientList);
	DDX_Control(pDX, IDC_CS, m_CSList);
	DDX_Control(pDX, IDC_DECODER, m_DecoderList);
	DDX_Control(pDX, IDC_ENCODER, m_EncoderList);
	DDX_Control(pDX, IDC_SEARCH, m_SearchList);
	DDX_Control(pDX, IDC_SERVER, m_ServerList);
	DDX_Control(pDX, IDC_EXTRACTION, m_ExtractList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CXMManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CXMManagerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_EXTRACTION, OnSelchangeExtraction)
	ON_LBN_SELCHANGE(IDC_CLIENT, OnSelchangeClient)
	ON_LBN_SELCHANGE(IDC_SERVER, OnSelchangeServer)
	ON_BN_CLICKED(IDC_RUN, OnRun)
	ON_BN_CLICKED(IDC_STOP, OnStop)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	ON_BN_CLICKED(IDC_CONNECT, OnConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXMManagerDlg message handlers

BOOL CXMManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (m_xmdb != 0)
		delete m_xmdb;

	m_xmdb = new XM::ComponentDatabase;

	m_xmdb->InitDB();
	
	
	for (int i=0; i < m_xmdb->GetDs().size(); i++)
	{
		m_DList.AddString(m_xmdb->GetDs()[i]->GetName());		
	}
	m_DList.AddString("None");
	m_DList.SetCurSel(0);
	
	for (i=0; i < m_xmdb->GetDSs().size(); i++)
	{
		m_DSList.AddString(m_xmdb->GetDSs()[i]->GetName());		
	}
	m_DSList.AddString("None");
	m_DSList.SetCurSel(0);
	
	for (i=0; i < m_xmdb->GetDDLs().size(); i++)
	{
		m_DDLList.AddString(m_xmdb->GetDDLs()[i]->GetName());		
	}
	m_DDLList.AddString("None");
	m_DDLList.SetCurSel(0);

	for (i=0; i < m_xmdb->GetExtractors().size(); i++)
	{
		m_ExtractList.AddString(m_xmdb->GetExtractors()[i]->GetName());		
	}
	m_ExtractList.AddString("None");
	m_ExtractList.SetCurSel(0);

	for (i=0; i < m_xmdb->GetServerApps().size(); i++)
	{
		m_ServerList.AddString(m_xmdb->GetServerApps()[i]->GetName());	
	}
	m_ServerList.AddString("None");
	m_ServerList.SetCurSel(0);

	for (i=0; i < m_xmdb->GetClientApps().size(); i++)
	{
		m_ClientList.AddString(m_xmdb->GetClientApps()[i]->GetName());	
	}
	m_ClientList.AddString("None");
	m_ClientList.SetCurSel(0);

	for (i=0; i < m_xmdb->GetDecoders().size(); i++)
	{
		m_DecoderList.AddString(m_xmdb->GetDecoders()[i]->GetName());	
	}
	m_DecoderList.AddString("None");
	m_DecoderList.SetCurSel(0);
	
	for (i=0; i < m_xmdb->GetEncoders().size(); i++)
	{	
		m_EncoderList.AddString(m_xmdb->GetEncoders()[i]->GetName());	
	}
	m_EncoderList.AddString("None");
	m_EncoderList.SetCurSel(0);
	
	for (i=0; i < m_xmdb->GetParsers().size(); i++)
	{
		m_ParserList.AddString(m_xmdb->GetParsers()[i]->GetName());	
	}
	m_ParserList.AddString("None");
	m_ParserList.SetCurSel(0);
	
	for (i=0; i < m_xmdb->GetSearchUtilities().size(); i++)
	{
		m_SearchList.AddString(m_xmdb->GetSearchUtilities()[i]->GetName());	
	}
	m_SearchList.AddString("None");
	m_SearchList.SetCurSel(0);
	
	for (i=0; i < m_xmdb->GetSystems().size(); i++)
	{
		m_SystemList.AddString(m_xmdb->GetSystems()[i]->GetName());	
	}
	m_SystemList.AddString("None");
	m_SystemList.SetCurSel(0);
	
	for (i=0; i < m_xmdb->GetCSs().size(); i++)
	{
		m_CSList.AddString(m_xmdb->GetCSs()[i]->GetName());	
	}
	m_CSList.AddString("None");
	m_CSList.SetCurSel(0);

	for (i=0; i < m_xmdb->GetEvalTools().size(); i++)
	{
		m_EvalList.AddString(m_xmdb->GetEvalTools()[i]->GetName());	
	}
	m_EvalList.AddString("None");
	m_EvalList.SetCurSel(0);
	
	UpdateData(false);



	
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CXMManagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CXMManagerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CXMManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

// Automation servers should not exit when a user closes the UI
//  if a controller still holds on to one of its objects.  These
//  message handlers make sure that if the proxy is still in use,
//  then the UI is hidden but the dialog remains around if it
//  is dismissed.

void CXMManagerDlg::OnClose() 
{
	if (CanExit())
		CDialog::OnClose();
}

void CXMManagerDlg::OnOK() 
{
	if (CanExit())
		CDialog::OnOK();
}

void CXMManagerDlg::OnCancel() 
{
	if (CanExit())
		CDialog::OnCancel();
}

BOOL CXMManagerDlg::CanExit()
{
	// If the proxy object is still around, then the automation
	//  controller is still holding on to this application.  Leave
	//  the dialog around, but hide its UI.
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}

void CXMManagerDlg::OnSelchangeExtraction() 
{
	// TODO: Add your control notification handler code here
	
}

void CXMManagerDlg::OnSelchangeClient() 
{
	// TODO: Add your control notification handler code here
	
}

void CXMManagerDlg::OnSelchangeServer() 
{
	// TODO: Add your control notification handler code here
	
}

void CXMManagerDlg::OnRun() 
{
	OnConnect();

	XM::I_StreamingComponentControlABC *serverControl = m_server->GetStreamingControl();
	XM::I_StreamingComponentControlABC *clientControl = m_client->GetStreamingControl();

	clientControl->Open();
	serverControl->Start();

	if (m_ET != 0)
	{
		
		if (strcmp(m_ET->GetName(),"D/DS Size EvaluationTool") == 0)
		{
			XM::P564CodedSizeInterfaceABC * evalIntfc = ((XM::P564CodedSize*)m_ET)->GetSizeInterface();

			CString displayStr;
			displayStr.Format("Total number of items seen: %u\n\n",evalIntfc->IdentifySizes().size());
			for (int i=0; i < evalIntfc->IdentifySizes().size(); i++)
			{
				CString tempStr;
				tempStr.Format("D/DS Number: %u size(bytes): %u\n",i,evalIntfc->IdentifySizes()[i]);
				displayStr += tempStr;
			}

			AfxMessageBox(displayStr);
		}
	}


}

void CXMManagerDlg::OnStop() 
{
	// TODO: Add your control notification handler code here
	
}

BOOL CAboutDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
	
	return CDialog::PreCreateWindow(cs);
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Copyright = "\
///////////////////////////////////////////////////////////////////////////////\r\n\
//\r\n\
// This software module was originally developed by\r\n\
//\r\n\
// Intel Corporation\r\n\
// Munich Technical University \r\n\
// (contributing organizations names)\r\n\
//\r\n\
// in the course of development of the MPEG-7 Experimentation Model.\r\n\
//\r\n\
// This software module is an implementation of a part of one or more MPEG-7\r\n\
// Experimentation Model tools as specified by the MPEG-7 Requirements.\r\n\
//\r\n\
// ISO/IEC gives users of MPEG-7 free license to this software module or\r\n\
// modifications thereof for use in hardware or software products claiming\r\n\
// conformance to MPEG-7.\r\n\
//\r\n\
// Those intending to use this software module in hardware or software products\r\n\
// are advised that its use may infringe existing patents. The original\r\n\
// developer of this software module and his/her company, the subsequent\r\n\
// editors and their companies, and ISO/IEC have no liability for use of this\r\n\
// software module or modifications thereof in an implementation.\r\n\
//\r\n\
// Copyright is not released for non MPEG-7 conforming products. The\r\n\
// organizations named above retain full right to use the code for their own\r\n\
// purpose, assign or donate the code to a third party and inhibit third parties\r\n\
// from using the code for non MPEG-7 conforming products.\r\n\
//\r\n\
// Copyright (c) 1998-1999.\r\n\
//\r\n\
// This notice must be included in all copies or derivative works.";

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CXMManagerDlg::OnAbout() 
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();	
}

void CXMManagerDlg::OnConnect() 
{
	//Connect the components
	UpdateData(true);

	m_xmdb->InitDB();

	CString tempstr;

	m_DList.GetLBText(m_DList.GetCurSel(),tempstr);
	m_descriptor = m_xmdb->CreateD(tempstr,0,0);

	m_ExtractList.GetLBText(m_ExtractList.GetCurSel(),tempstr);
	m_extractor = m_xmdb->CreateExtractor(tempstr,0,0);
	
	m_ServerList.GetLBText(m_ServerList.GetCurSel(),tempstr);
	m_server = m_xmdb->CreateServer(tempstr,0,0);

	m_ClientList.GetLBText(m_ClientList.GetCurSel(),tempstr);
	m_client = m_xmdb->CreateClient(tempstr,0,0);

	m_EncoderList.GetLBText(m_EncoderList.GetCurSel(),tempstr);
	m_encoder = m_xmdb->CreateEncoder(tempstr,0,0);

	m_DecoderList.GetLBText(m_DecoderList.GetCurSel(),tempstr);
	m_decoder = m_xmdb->CreateDecoder(tempstr,0,0);

	m_ParserList.GetLBText(m_ParserList.GetCurSel(),tempstr);
	m_parser = m_xmdb->CreateParser(tempstr,0,0);
	
	m_SystemList.GetLBText(m_SystemList.GetCurSel(),tempstr);
	m_system = m_xmdb->CreateSystem(tempstr,0,0);

	m_SearchList.GetLBText(m_SearchList.GetCurSel(),tempstr);
	m_search = m_xmdb->CreateSearch(tempstr,0,0);

	m_CSList.GetLBText(m_CSList.GetCurSel(),tempstr);
	m_CS = m_xmdb->CreateCS(tempstr,0,0);

	m_DSList.GetLBText(m_DSList.GetCurSel(),tempstr);
	m_DS = m_xmdb->CreateDS(tempstr,0,0);

	m_DDLList.GetLBText(m_DDLList.GetCurSel(),tempstr);
	m_DDL = m_xmdb->CreateDDL(tempstr,0,0);

	m_EvalList.GetLBText(m_EvalList.GetCurSel(),tempstr);
	m_ET = m_xmdb->CreateEvalTool(tempstr,0,0);


	unsigned long attachResult;

	if (m_extractor != 0)
	{
		attachResult = m_server->AttachInterface(m_extractor->GetExtractionInterface());
		if (attachResult != 0)
		{	AfxMessageBox("Server Attachment Failed!");	}
	}

	if (m_encoder != 0)
	{
		attachResult = m_server->AttachInterface(m_encoder->GetInterface());
		if (attachResult != 0)
		{	AfxMessageBox("Server Attachment Failed!");	}
	}
	/*

	if (m_encoder != 0)
	{
		attachResult = m_CS->AttachInterface(m_encoder->GetInterface());
		if (attachResult != 0)
		{	AfxMessageBox("Coding Scheme Attachment Failed!");	}
	}
	*/
	
/*	if (m_encoder != 0)
	{
		attachResult = m_system->AttachInterface(m_encoder->GetInterface());
		if (attachResult != 0)
		{	AfxMessageBox("System Attachment Failed!");	}
	}
*/
	if (m_parser != 0)
	{
		attachResult = m_system->AttachInterface(m_parser->GetInterface());
		if (attachResult != 0)
		{	AfxMessageBox("System Attachment Failed!");	}
	}

	if (m_DDL != 0)
	{
		attachResult = m_encoder->AttachInterface(m_DDL->GetDDLInterface());
		if (attachResult != 0)
		{	AfxMessageBox("Encoder Attachment Failed!");	}
	}

	if (m_CS != 0)
	{
		attachResult = m_encoder->AttachInterface(m_CS->GetCodingInterface());
		if (attachResult != 0)
		{	AfxMessageBox("Encoder Attachment Failed!");	}
	}

	if (m_ET != 0)
	{
		
		if (strcmp(m_ET->GetName(),"D/DS Size EvaluationTool") == 0)
		{
			attachResult = m_encoder->AttachInterface(((XM::P564CodedSize*) m_ET)->GetSystemFrontEndInterface());
			if (attachResult != 0)
			{	AfxMessageBox("Evaluator Attachment Failed!");	}
		}
	}

	/*
	if (m_system != 0)
	{
		attachResult = m_encoder->AttachInterface(m_system->GetServerInterface());
		if (attachResult != 0)
		{	AfxMessageBox("Attachment Failed!");	}
	}
	*/

	if (m_system != 0)
	{
		attachResult = m_parser->AttachInterface(m_system->GetClientInterface());
		if (attachResult != 0)
		{	AfxMessageBox("Parser Attachment Failed!");	}
	}

	if (m_parser != 0)
	{
		attachResult = m_decoder->AttachInterface(m_parser->GetInterface());
		if (attachResult != 0)
		{	AfxMessageBox("Decoder Attachment Failed!");	}
	}

	if (m_system != 0)
	{
		attachResult = m_search->AttachInterface(m_system->GetSystemQuery());
		if (attachResult != 0)
		{	AfxMessageBox("Search Attachment Failed!");	}
	}
	
	if (m_client != 0)
	{
		attachResult = m_client->AttachInterface(m_search->GetInterface());	
		if (attachResult != 0)
		{	AfxMessageBox("Attachment Failed!");	}
	}



		
}
