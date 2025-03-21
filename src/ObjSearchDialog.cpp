///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Jan 25 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "ObjSearchDialog.h"

///////////////////////////////////////////////////////////////////////////

ObjSearchDialog::ObjSearchDialog(wxWindow* parent, wxWindowID id,
    const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxSize(300, 300), wxDefaultSize);

    wxBoxSizer* bMainSizer;
    bMainSizer = new wxBoxSizer(wxVERTICAL);

    wxBoxSizer* bSearchSizer;
    bSearchSizer = new wxBoxSizer(wxHORIZONTAL);

    m_choiceFeature = new wxGenericComboCtrl(this, wxID_ANY, wxEmptyString,
        wxPoint(0, 0), wxDefaultSize, wxCB_READONLY);

    // Make sure we use popup that allows focusing the treectrl.
    m_choiceFeature->UseAltPopupWindow();

    // Set popup interface right away, otherwise some of the calls
    // below may fail
    CheckListComboPopup* clcPopup = new CheckListComboPopup();
    m_choiceFeature->SetPopupControl(clcPopup);
    m_choiceFeature->SetMinSize(wxSize(100, -1));

    bSearchSizer->Add(m_choiceFeature, 0, wxALL, 5);

    m_textCtrlSearchTerm = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    bSearchSizer->Add(m_textCtrlSearchTerm, 1, wxALL | wxEXPAND, 5);

    m_buttonSearch = new wxButton(
        this, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, 0);
    bSearchSizer->Add(m_buttonSearch, 0, wxALL, 5);

    bMainSizer->Add(bSearchSizer, 0, wxEXPAND, 5);

    m_listCtrlResults = new wxListCtrl(this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
    bMainSizer->Add(m_listCtrlResults, 1, wxALL | wxEXPAND, 5);

    wxBoxSizer* bSizerButtons;
    bSizerButtons = new wxBoxSizer(wxHORIZONTAL);

    m_btnShowOnChart = new wxButton(this, wxID_ANY, _("Show on Chart"),
        wxDefaultPosition, wxDefaultSize, 0);
    bSizerButtons->Add(m_btnShowOnChart, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_cAutoClose = new wxCheckBox(this, wxID_ANY, _("Close on show"),
        wxDefaultPosition, wxDefaultSize, 0);
    m_cAutoClose->SetValue(true);
    bSizerButtons->Add(m_cAutoClose, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    bSizerButtons->Add(0, 0, 1, wxEXPAND, 5);

    m_stRange = new wxStaticText(this, wxID_ANY, _("Limit range to"),
        wxDefaultPosition, wxDefaultSize, 0);
    m_stRange->Wrap(-1);
    m_stRange->SetToolTip(_("0 = Unlimited"));

    bSizerButtons->Add(m_stRange, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    m_scRange = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
        wxSize(80, -1), wxSP_ARROW_KEYS, 0, 10000, 0);
    m_scRange->SetToolTip(_("0 = Unlimited"));

    bSizerButtons->Add(m_scRange, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    m_stUnit = new wxStaticText(
        this, wxID_ANY, _("NMi"), wxDefaultPosition, wxDefaultSize, 0);
    m_stUnit->Wrap(-1);
    bSizerButtons->Add(m_stUnit, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    m_btnSettings = new wxButton(this, wxID_ANY, _("..."), wxDefaultPosition,
        wxDefaultSize, wxBU_EXACTFIT);
    m_btnSettings->SetToolTip(_("Settings..."));

    bSizerButtons->Add(m_btnSettings, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    bMainSizer->Add(bSizerButtons, 0, wxEXPAND, 5);

    this->SetSizer(bMainSizer);
    this->Layout();

    this->Centre(wxBOTH);

    // Connect Events
    this->Connect(wxEVT_CHAR_HOOK,
        wxKeyEventHandler(ObjSearchDialog::ObjSearchDialogOnCharHook));
    this->Connect(
        wxEVT_SHOW, wxShowEventHandler(ObjSearchDialog::ObjSearchDialogOnShow));
    m_textCtrlSearchTerm->Connect(wxEVT_COMMAND_TEXT_ENTER,
        wxCommandEventHandler(ObjSearchDialog::OnSearch), NULL, this);
    m_buttonSearch->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(ObjSearchDialog::OnSearch), NULL, this);
    m_listCtrlResults->Connect(wxEVT_LEFT_DCLICK,
        wxMouseEventHandler(ObjSearchDialog::OnLeftDClick), NULL, this);
    m_listCtrlResults->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED,
        wxListEventHandler(ObjSearchDialog::OnItemSelected), NULL, this);
    m_btnShowOnChart->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(ObjSearchDialog::OnShowOnChart), NULL, this);
    m_btnSettings->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(ObjSearchDialog::OnSettings), NULL, this);
}

ObjSearchDialog::~ObjSearchDialog()
{
    // Disconnect Events
    this->Disconnect(wxEVT_CHAR_HOOK,
        wxKeyEventHandler(ObjSearchDialog::ObjSearchDialogOnCharHook));
    this->Disconnect(
        wxEVT_SHOW, wxShowEventHandler(ObjSearchDialog::ObjSearchDialogOnShow));
    m_textCtrlSearchTerm->Disconnect(wxEVT_COMMAND_TEXT_ENTER,
        wxCommandEventHandler(ObjSearchDialog::OnSearch), NULL, this);
    m_buttonSearch->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(ObjSearchDialog::OnSearch), NULL, this);
    m_listCtrlResults->Disconnect(wxEVT_LEFT_DCLICK,
        wxMouseEventHandler(ObjSearchDialog::OnLeftDClick), NULL, this);
    m_listCtrlResults->Disconnect(wxEVT_COMMAND_LIST_ITEM_SELECTED,
        wxListEventHandler(ObjSearchDialog::OnItemSelected), NULL, this);
    m_btnShowOnChart->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(ObjSearchDialog::OnShowOnChart), NULL, this);
    m_btnSettings->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(ObjSearchDialog::OnSettings), NULL, this);
}

SettingsDialog::SettingsDialog(wxWindow* parent, wxWindowID id,
    const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : wxDialog(parent, id, title, pos, size, style)
{
    this->SetSizeHints(wxSize(300, 300), wxDefaultSize);

    wxBoxSizer* bSizerMain;
    bSizerMain = new wxBoxSizer(wxVERTICAL);

    m_swMain = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxHSCROLL | wxVSCROLL);
    m_swMain->SetScrollRate(5, 5);
    wxBoxSizer* bSizerCintentMain;
    bSizerCintentMain = new wxBoxSizer(wxVERTICAL);

    m_notebookSettings = new wxNotebook(
        m_swMain, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0);
    m_panelPopulate = new wxPanel(m_notebookSettings, wxID_ANY,
        wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizerPopulate;
    bSizerPopulate = new wxBoxSizer(wxVERTICAL);

    m_stScanCharts = new wxStaticText(m_panelPopulate, wxID_ANY,
        _("Scan charts"), wxDefaultPosition, wxDefaultSize, 0);
    m_stScanCharts->Wrap(-1);
    bSizerPopulate->Add(m_stScanCharts, 0, wxALL, 5);

    m_stScanChartsExplain = new wxStaticText(m_panelPopulate, wxID_ANY,
        _("0-80S/N, 0-180E/W, W longitudes and S latitudes are expressed as "
          "negative numbers.\nThe area selected can't cross the 180th "
          "meridian"),
        wxDefaultPosition, wxDefaultSize, 0);
    m_stScanChartsExplain->Wrap(-1);
    m_stScanChartsExplain->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT,
        wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxT("Lucida Grande")));

    bSizerPopulate->Add(m_stScanChartsExplain, 0, wxALL | wxEXPAND, 5);

    wxBoxSizer* bSizerParams;
    bSizerParams = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer* bSizerArea;
    bSizerArea = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* sbSizerFrom;
    sbSizerFrom = new wxStaticBoxSizer(
        new wxStaticBox(m_panelPopulate, wxID_ANY, _("From")), wxVERTICAL);

    wxGridSizer* gSizerFrom;
    gSizerFrom = new wxGridSizer(0, 2, 0, 0);

    m_staticTextFromLat = new wxStaticText(sbSizerFrom->GetStaticBox(),
        wxID_ANY, _("Latitude"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticTextFromLat->Wrap(-1);
    gSizerFrom->Add(m_staticTextFromLat, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    m_spFromLat
        = new wxSpinCtrl(sbSizerFrom->GetStaticBox(), wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -80, 80, 0);
    gSizerFrom->Add(m_spFromLat, 0, wxALL, 5);

    m_staticTextFromLon = new wxStaticText(sbSizerFrom->GetStaticBox(),
        wxID_ANY, _("Longitude"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticTextFromLon->Wrap(-1);
    gSizerFrom->Add(m_staticTextFromLon, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    m_spFromLon
        = new wxSpinCtrl(sbSizerFrom->GetStaticBox(), wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -180, 180, 0);
    gSizerFrom->Add(m_spFromLon, 0, wxALL, 5);

    sbSizerFrom->Add(gSizerFrom, 0, wxEXPAND, 5);

    bSizerArea->Add(sbSizerFrom, 1, wxALL | wxEXPAND, 5);

    wxStaticBoxSizer* sbSizerTo;
    sbSizerTo = new wxStaticBoxSizer(
        new wxStaticBox(m_panelPopulate, wxID_ANY, _("To")), wxVERTICAL);

    wxGridSizer* gSizerTo;
    gSizerTo = new wxGridSizer(0, 2, 0, 0);

    m_staticTextToLat = new wxStaticText(sbSizerTo->GetStaticBox(), wxID_ANY,
        _("Latitude"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticTextToLat->Wrap(-1);
    gSizerTo->Add(m_staticTextToLat, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    m_spToLat
        = new wxSpinCtrl(sbSizerTo->GetStaticBox(), wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -80, 80, 0);
    gSizerTo->Add(m_spToLat, 0, wxALL, 5);

    m_staticTextToLon = new wxStaticText(sbSizerTo->GetStaticBox(), wxID_ANY,
        _("Longitude"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticTextToLon->Wrap(-1);
    gSizerTo->Add(m_staticTextToLon, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    m_spToLon
        = new wxSpinCtrl(sbSizerTo->GetStaticBox(), wxID_ANY, wxEmptyString,
            wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, -180, 180, 0);
    gSizerTo->Add(m_spToLon, 0, wxALL, 5);

    sbSizerTo->Add(gSizerTo, 0, wxEXPAND, 5);

    bSizerArea->Add(sbSizerTo, 1, wxALL | wxEXPAND, 5);

    bSizerParams->Add(bSizerArea, 0, wxEXPAND, 5);

    wxStaticBoxSizer* sbSizerScales;
    sbSizerScales = new wxStaticBoxSizer(
        new wxStaticBox(m_panelPopulate, wxID_ANY, _("Scales")), wxVERTICAL);

    m_cb5000000 = new wxCheckBox(sbSizerScales->GetStaticBox(), wxID_ANY,
        _("1:5000000"), wxDefaultPosition, wxDefaultSize, 0);
    m_cb5000000->SetValue(true);
    sbSizerScales->Add(m_cb5000000, 0, wxALL, 5);

    m_cb1000000 = new wxCheckBox(sbSizerScales->GetStaticBox(), wxID_ANY,
        _("1:1000000"), wxDefaultPosition, wxDefaultSize, 0);
    m_cb1000000->SetValue(true);
    sbSizerScales->Add(m_cb1000000, 0, wxALL, 5);

    m_cb200000 = new wxCheckBox(sbSizerScales->GetStaticBox(), wxID_ANY,
        _("1:200000"), wxDefaultPosition, wxDefaultSize, 0);
    sbSizerScales->Add(m_cb200000, 0, wxALL, 5);

    m_cb20000 = new wxCheckBox(sbSizerScales->GetStaticBox(), wxID_ANY,
        _("1:20000 (Very time consuming)"), wxDefaultPosition, wxDefaultSize,
        0);
    m_cb20000->Enable(false);

    sbSizerScales->Add(m_cb20000, 0, wxALL, 5);

    bSizerParams->Add(sbSizerScales, 1, wxALL | wxEXPAND, 5);

    bSizerPopulate->Add(bSizerParams, 1, wxEXPAND, 5);

    m_panelPopulate->SetSizer(bSizerPopulate);
    m_panelPopulate->Layout();
    bSizerPopulate->Fit(m_panelPopulate);
    m_notebookSettings->AddPage(m_panelPopulate, _("Populate database"), true);
    m_panelManage = new wxPanel(m_notebookSettings, wxID_ANY, wxDefaultPosition,
        wxDefaultSize, wxTAB_TRAVERSAL);
    wxBoxSizer* bSizerManageDB;
    bSizerManageDB = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* sbSizerCSV;
    sbSizerCSV = new wxStaticBoxSizer(
        new wxStaticBox(m_panelManage, wxID_ANY, _("Import data")),
        wxHORIZONTAL);

    m_stFile = new wxStaticText(sbSizerCSV->GetStaticBox(), wxID_ANY, _("File"),
        wxDefaultPosition, wxDefaultSize, 0);
    m_stFile->Wrap(-1);
    sbSizerCSV->Add(m_stFile, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    m_tPath = new wxTextCtrl(sbSizerCSV->GetStaticBox(), wxID_ANY,
        wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
    sbSizerCSV->Add(m_tPath, 1, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    m_button4 = new wxButton(sbSizerCSV->GetStaticBox(), wxID_ANY,
        _("Browse..."), wxDefaultPosition, wxDefaultSize, 0);
    sbSizerCSV->Add(m_button4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    bSizerManageDB->Add(sbSizerCSV, 0, wxALL | wxEXPAND, 5);

    m_panelManage->SetSizer(bSizerManageDB);
    m_panelManage->Layout();
    bSizerManageDB->Fit(m_panelManage);
    m_notebookSettings->AddPage(m_panelManage, _("Manage Database"), false);

    bSizerCintentMain->Add(m_notebookSettings, 1, wxEXPAND | wxALL, 5);

    m_swMain->SetSizer(bSizerCintentMain);
    m_swMain->Layout();
    bSizerCintentMain->Fit(m_swMain);
    bSizerMain->Add(m_swMain, 1, wxEXPAND | wxALL, 5);

    m_sdbSizerBtns = new wxStdDialogButtonSizer();
    m_sdbSizerBtnsOK = new wxButton(this, wxID_OK);
    m_sdbSizerBtns->AddButton(m_sdbSizerBtnsOK);
    m_sdbSizerBtnsCancel = new wxButton(this, wxID_CANCEL, _("Cancel"));
    m_sdbSizerBtns->AddButton(m_sdbSizerBtnsCancel);
    m_sdbSizerBtns->Realize();

    bSizerMain->Add(m_sdbSizerBtns, 0, wxALL | wxEXPAND, 5);

    this->SetSizer(bSizerMain);
    this->Layout();

    this->Centre(wxBOTH);

    // Connect Events
    m_cb5000000->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnScale), NULL, this);
    m_cb1000000->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnScale), NULL, this);
    m_cb200000->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnScale), NULL, this);
    m_cb20000->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnScale), NULL, this);
    m_tPath->Connect(wxEVT_COMMAND_TEXT_UPDATED,
        wxCommandEventHandler(SettingsDialog::OnPathChange), NULL, this);
    m_button4->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnBrowse), NULL, this);
    m_sdbSizerBtnsCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnCancel), NULL, this);
    m_sdbSizerBtnsOK->Connect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnOk), NULL, this);
}

SettingsDialog::~SettingsDialog()
{
    // Disconnect Events
    m_cb5000000->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnScale), NULL, this);
    m_cb1000000->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnScale), NULL, this);
    m_cb200000->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnScale), NULL, this);
    m_cb20000->Disconnect(wxEVT_COMMAND_CHECKBOX_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnScale), NULL, this);
    m_tPath->Disconnect(wxEVT_COMMAND_TEXT_UPDATED,
        wxCommandEventHandler(SettingsDialog::OnPathChange), NULL, this);
    m_button4->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnBrowse), NULL, this);
    m_sdbSizerBtnsCancel->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnCancel), NULL, this);
    m_sdbSizerBtnsOK->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(SettingsDialog::OnOk), NULL, this);
}
