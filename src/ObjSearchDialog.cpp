///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 26 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ObjSearchDialog.h"

///////////////////////////////////////////////////////////////////////////

ObjSearchDialog::ObjSearchDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSearchSizer;
	bSearchSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_choiceFeature = new wxGenericComboCtrl( this, wxID_ANY, wxEmptyString, wxPoint(0,0), wxDefaultSize, wxCB_READONLY );
	
	// Make sure we use popup that allows focusing the treectrl.
	m_choiceFeature->UseAltPopupWindow();
	
	// Set popup interface right away, otherwise some of the calls
	// below may fail
	CheckListComboPopup* clcPopup = new CheckListComboPopup();
	m_choiceFeature->SetPopupControl(clcPopup);
	bSearchSizer->Add( m_choiceFeature, 0, wxALL, 5 );
	
	m_textCtrlSearchTerm = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	bSearchSizer->Add( m_textCtrlSearchTerm, 1, wxALL|wxEXPAND, 5 );
	
	m_buttonSearch = new wxButton( this, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	bSearchSizer->Add( m_buttonSearch, 0, wxALL, 5 );
	
	
	bMainSizer->Add( bSearchSizer, 0, wxEXPAND, 5 );
	
	m_listCtrlResults = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bMainSizer->Add( m_listCtrlResults, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizerButtons;
	bSizerButtons = new wxBoxSizer( wxHORIZONTAL );
	
	m_btnShowOnChart = new wxButton( this, wxID_ANY, _("Show on Chart"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( m_btnShowOnChart, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	m_btnClose = new wxButton( this, wxID_ANY, _("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizerButtons->Add( m_btnClose, 0, wxALIGN_RIGHT|wxALL, 5 );
	
	
	bMainSizer->Add( bSizerButtons, 0, wxALIGN_RIGHT|wxEXPAND, 5 );
	
	
	this->SetSizer( bMainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	m_textCtrlSearchTerm->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ObjSearchDialog::OnSearch ), NULL, this );
	m_buttonSearch->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjSearchDialog::OnSearch ), NULL, this );
	m_listCtrlResults->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ObjSearchDialog::OnItemSelected ), NULL, this );
	m_btnShowOnChart->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjSearchDialog::OnShowOnChart ), NULL, this );
	m_btnClose->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjSearchDialog::OnClose ), NULL, this );
}

ObjSearchDialog::~ObjSearchDialog()
{
	// Disconnect Events
	m_textCtrlSearchTerm->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( ObjSearchDialog::OnSearch ), NULL, this );
	m_buttonSearch->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjSearchDialog::OnSearch ), NULL, this );
	m_listCtrlResults->Disconnect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( ObjSearchDialog::OnItemSelected ), NULL, this );
	m_btnShowOnChart->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjSearchDialog::OnShowOnChart ), NULL, this );
	m_btnClose->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ObjSearchDialog::OnClose ), NULL, this );
	
}
