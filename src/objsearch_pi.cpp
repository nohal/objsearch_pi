/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Vector Chart Object Search Plugin
 * Author:   Pavel Kalian
 *
 ***************************************************************************
 *   Copyright (C) 2014 by Pavel Kalian                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/wxsqlite3.h"

#include "objsearch_pi.h"

// Define NAN, which is unavailable on Windows
#ifdef _MSC_VER
#define INFINITY (DBL_MAX+DBL_MAX)
#define NAN (INFINITY-INFINITY)
#endif


// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi ( void *ppimgr )
{
    return new objsearch_pi ( ppimgr );
}

extern "C" DECL_EXP void destroy_pi ( opencpn_plugin* p )
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    ObjSearch PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

wxSQLite3Database* objsearch_pi::initDB(void)
{
    bool have_to_create = false;
    wxString sDBName = *GetpPrivateApplicationDataLocation() + wxFileName::GetPathSeparator()+ wxT("objsearch_pi.db");

    wxLogMessage( _T("OBJSEARCH_PI: Database file to be used: %s"), sDBName.c_str() );
    if ( !wxFileExists(sDBName) )
    {
        have_to_create = true;
    }
    wxSQLite3Database* db = new wxSQLite3Database();
    try
    {
        db->Open( sDBName );
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage( _T("OBJSEARCH_PI: DB Exception: %i : %s"), e.GetErrorCode(), e.GetMessage().c_str() );
        m_bDBUsable = false;
    }
    catch (...)
    {
        wxLogMessage( _T("OBJSEARCH_PI: Unknown exception") );
        m_bDBUsable = false;
    }

    if ( have_to_create && m_bDBUsable )
    {
        QueryDB( db, wxT("CREATE TABLE chart (id INTEGER PRIMARY KEY AUTOINCREMENT, chartname TEXT, scale REAL, nativescale INTEGER)") );
        QueryDB( db, wxT("CREATE TABLE feature (id INTEGER PRIMARY KEY AUTOINCREMENT, featurename TEXT)") );
        QueryDB( db, wxT("CREATE TABLE object (chart_id INTEGER, feature_id INTEGER, objname TEXT, lat REAL, lon REAL)") );
    }
    return db;
}

int objsearch_pi::QueryDB(wxSQLite3Database* db, const wxString& sql)
{
    int ret = -1;
    try
    {
        ret = db->ExecuteUpdate(sql);
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage( _T("OBJSEARCH_PI: DB Exception: %i : %s"), e.GetErrorCode(), e.GetMessage().c_str() );
        m_bDBUsable = false;
    }
    catch (...)
    {
        wxLogMessage( _T("OBJSEARCH_PI: Unknown exception during '%s'"), sql.c_str() );
        m_bDBUsable = false;
    }
    
    return ret;
}

wxSQLite3ResultSet objsearch_pi::SelectFromDB(wxSQLite3Database* db, const wxString& sql)
{
    try
    {
        return db->ExecuteQuery(sql);
    }
    catch (wxSQLite3Exception& e)
    {
        wxLogMessage( _T("OBJSEARCH_PI: DB Exception: %i : %s"), e.GetErrorCode(), e.GetMessage().c_str() );
        m_bDBUsable = false;
    }
    catch (...)
    {
        wxLogMessage( _T("OBJSEARCH_PI: Unknown exception during '%s'"), sql.c_str() );
        m_bDBUsable = false;
    }
    return wxSQLite3ResultSet();
}

void objsearch_pi::clearDB(wxSQLite3Database* db)
{
    assert(db != NULL);
    db->Close();
    delete db;
}

objsearch_pi::objsearch_pi ( void *ppimgr )
    : opencpn_plugin_111 ( ppimgr )
{
    // Create the PlugIn icons
    initialize_images();
    
    m_bDBUsable = true;
    
    m_db = initDB();
    
    wxSQLite3ResultSet set = SelectFromDB( m_db, wxT("SELECT id, chartname, scale, nativescale FROM chart") );
    while (set.NextRow())
    {
        Chart ch;
        ch.id = set.GetInt(0);
        ch.name = set.GetAsString(1);
        ch.scale = set.GetDouble(2);
        ch.nativescale = set.GetInt(3);
        m_chartsInDb[ch.name] = ch;
    }
    set.Finalize();
    
    set = SelectFromDB( m_db, wxT("SELECT id, featurename FROM feature"));
    while (set.NextRow())
    {
        m_featuresInDb[set.GetAsString(1)] = set.GetInt(0);
    }
    set.Finalize();
}

objsearch_pi::~objsearch_pi ( void )
{
    {
        wxCriticalSectionLocker enter(m_pThreadCS);
        if (m_pThread) // does the thread still exist?
        {
            if (m_pThread->Delete() != wxTHREAD_NO_ERROR )
                wxLogError(_T("Can't delete the DB thread!"));
        }
    } // exit from the critical section to give the thread
        // the possibility to enter its destructor
        // (which is guarded with m_pThreadCS critical section!)
    while (1)
    {
        { // was the ~MyThread() function executed?
            wxCriticalSectionLocker enter(m_pThreadCS);
            if (!m_pThread)
                break;
        }
        // wait for thread completion
        wxThread::This()->Sleep(1);
    }
    clearDB(m_db);
    delete _img_objsearch_pi;
    delete _img_objsearch;
}

int objsearch_pi::Init ( void )
{
    AddLocaleCatalog ( _T ( "opencpn-objsearch_pi" ) );

    ::wxDisplaySize ( &m_display_width, &m_display_height );

    //    Get a pointer to the opencpn configuration object
    m_pconfig = GetOCPNConfigObject();
    
    //    And load the configuration items
    LoadConfig();

    // Get a pointer to the opencpn display canvas, to use as a parent for the OBJSEARCH dialog
    m_parent_window = GetOCPNCanvasWindow();

    m_leftclick_tool_id = InsertPlugInTool ( _T ( "" ), _img_objsearch, _img_objsearch, wxITEM_CHECK,
                          _ ( "Object Search" ), _T ( "" ), NULL,
                          OBJSEARCH_TOOL_POSITION, 0, this );

    m_pObjSearchDialog = new ObjSearchDialogImpl( this, m_parent_window );
    
    m_chartLoading = wxEmptyString;
    
    m_boatlat = NAN;
    m_boatlon = NAN;

    m_pThread = new DbThread(this);
    wxThreadError err = m_pThread->Run();

    if ( err != wxTHREAD_NO_ERROR )
    {
        delete m_pThread;
        m_pThread = NULL;
    }

    return ( WANTS_ONPAINT_VIEWPORT    |
             WANTS_TOOLBAR_CALLBACK    |
             INSTALLS_TOOLBAR_TOOL     |
             WANTS_CONFIG              |
             WANTS_NMEA_EVENTS         |
             WANTS_VECTOR_CHART_OBJECT_INFO
           );
}

bool objsearch_pi::DeInit ( void )
{
    if ( m_pObjSearchDialog )
    {
        m_pObjSearchDialog->Close();
        delete m_pObjSearchDialog;
        m_pObjSearchDialog = NULL;
    }

    return true;
}

int objsearch_pi::GetAPIVersionMajor()
{
    return MY_API_VERSION_MAJOR;
}

int objsearch_pi::GetAPIVersionMinor()
{
    return MY_API_VERSION_MINOR;
}

int objsearch_pi::GetPlugInVersionMajor()
{
    return PLUGIN_VERSION_MAJOR;
}

int objsearch_pi::GetPlugInVersionMinor()
{
    return PLUGIN_VERSION_MINOR;
}

wxBitmap *objsearch_pi::GetPlugInBitmap()
{
    return _img_objsearch_pi;
}

wxString objsearch_pi::GetCommonName()
{
    return _T ( "ObjSearch" );
}


wxString objsearch_pi::GetShortDescription()
{
    return _ ( "Vector Chart Object Search PlugIn for OpenCPN" );
}


wxString objsearch_pi::GetLongDescription()
{
    return _ ( "Vector Chart Object Search PlugIn for OpenCPN\n\
Provides a search function for the named vector chart objects." );
}

int objsearch_pi::GetToolbarToolCount ( void )
{
    return 1;
}

void objsearch_pi::OnToolbarToolCallback ( int id )
{
    SetToolbarItemState( id, false );
    m_pObjSearchDialog->ClearFeatures();
    for(std::map<wxString, int>::iterator it = m_featuresInDb.begin(); it != m_featuresInDb.end(); ++it)
        m_pObjSearchDialog->AddFeature(it->first);
    
    m_pObjSearchDialog->Show();
}

void objsearch_pi::SetCurrentViewPort(PlugIn_ViewPort &vp)
{
    m_vplat = vp.clat;
    m_vplon = vp.clon;
}

bool objsearch_pi::LoadConfig ( void )
{
    wxFileConfig *pConf = ( wxFileConfig * ) m_pconfig;

    if ( !pConf )
        return false;

    pConf->SetPath ( _T ( "/PlugIns/OBJSEARCH" ) );

    return true;
}

bool objsearch_pi::SaveConfig ( void )
{
    wxFileConfig *pConf = ( wxFileConfig * ) m_pconfig;

    if ( !pConf )
        return false;

    pConf->SetPath ( _T ( "/PlugIns/OBJSEARCH" ) );

    return true;
}

void objsearch_pi::SetColorScheme ( PI_ColorScheme cs )
{
    DimeWindow ( m_pObjSearchDialog );
}

void objsearch_pi::SetPositionFix( PlugIn_Position_Fix &pfix )
{
    m_boatlat = pfix.Lat;
    m_boatlon = pfix.Lon;
}

void objsearch_pi::SendVectorChartObjectInfo(wxString &chart, wxString &feature, wxString &objname, double lat, double lon, double scale, int nativescale)
{
    if ( !m_bDBUsable )
        return;
    long chart_id = GetChartId(chart);
    long feature_id = GetFeatureId(feature);
    if ( chart_id == 0 )
    {      
        Chart ch = StoreNewChart( chart, scale, nativescale );
        m_chartsInDb[ch.name] = ch;
    }
    if ( feature_id == 0 )
    {
        feature_id = StoreNewFeature( feature ).ToLong();
        m_featuresInDb[feature] = feature_id;
    }
        
    if ( chart == m_chartLoading )
        StoreNewObject( chart_id, feature_id, objname, lat, lon );
}

Chart objsearch_pi::StoreNewChart(wxString chart, double scale, int nativescale)
{
    Chart ch;
    if ( !m_bDBUsable )
    {
        ch.id = -1;
        return ch;
    }
    wxFileName chartname(chart);
    ch.name = chartname.GetName();
    ch.scale = scale;
    ch.nativescale = nativescale;
    
    m_chartLoading = chart;
    QueryDB( m_db, wxString::Format(_T("INSERT INTO chart(chartname, scale, nativescale) VALUES ('%s', %f, %i)"), ch.name.c_str(), ch.scale, ch.nativescale) );
    ch.id = m_db->GetLastRowId();
    return ch;
}

wxLongLong objsearch_pi::StoreNewFeature(wxString feature)
{
    if ( !m_bDBUsable )
        return -1;
    QueryDB( m_db, wxString::Format(_T("INSERT INTO feature(featurename) VALUES ('%s')"), feature.c_str()) );
    return m_db->GetLastRowId();
}

void objsearch_pi::StoreNewObject(long chart_id, long feature_id, wxString objname, double lat, double lon)
{
    if ( !m_bDBUsable )
        return;
    if ( objname.Len() > 1 )
    {
        wxString safe_value = objname;
        safe_value.Replace(_T("'"), _T("''"));
        wxString sql = wxString::Format(_T("INSERT INTO object(chart_id, feature_id, objname, lat, lon) VALUES (%i, %i, '%s', %f, %f)"), chart_id, feature_id, safe_value.c_str(), lat, lon);
        query_queue.push(sql);
    }
}

long objsearch_pi::GetChartId(wxString chart)
{
    if ( !m_bDBUsable )
        return -1;
    wxFileName chartname(chart);
    wxString chrt = chartname.GetName();
    
    if(m_chartsInDb.find(chrt) == m_chartsInDb.end())
        return 0;
    else
        return m_chartsInDb[chrt].id.ToLong();
}

int objsearch_pi::GetFeatureId(wxString feature)
{
    if ( !m_bDBUsable )
        return -1;
    if(m_featuresInDb.find(feature) == m_featuresInDb.end())
        return 0;
    else
        return m_featuresInDb[feature];
}

void objsearch_pi::FindObjects( const wxString& feature_filter, const wxString& search_string )
{
    m_pObjSearchDialog->ClearObjects();
    wxString safe_value = search_string;
    safe_value.Replace(_T("'"), _T("''"));
    wxSQLite3ResultSet set = SelectFromDB( m_db, wxString::Format( wxT("SELECT COUNT(*) FROM object o LEFT JOIN feature f ON (o.feature_id = f.id) WHERE instr('%s', featurename) > 0 AND objname LIKE '%%%s%%'"), feature_filter.c_str(), safe_value.c_str() ) );
    int objects_found = set.GetInt(0);
    set.Finalize();
    int show = wxYES;
    if ( objects_found > 1000 )
    {
        show = wxMessageBox( wxString::Format( _("Your search resulted in %i objects found. This is a lot, do you really want to show all of them?"), objects_found ), _("Too many objects found"), wxYES_NO | wxCENTER );
    }
    if ( show == wxYES )
    {
        set = SelectFromDB( m_db, wxString::Format( wxT("SELECT f.featurename, o.objname, o.lat, o.lon, ch.scale, ch.nativescale, ch.chartname FROM object o LEFT JOIN feature f ON (o.feature_id = f.id) LEFT JOIN chart ch ON (o.chart_id = ch.id) WHERE instr('%s', featurename) > 0 AND objname LIKE '%%%s%%'"), feature_filter.c_str(), safe_value.c_str() ) );
        double lat, lon;
        double dist, brg;
        if ( m_boatlat == NAN || m_boatlon == NAN)
        {
            lat = m_vplat;
            lon = m_vplon;
        }
        else
        {
            lat = m_boatlat;
            lon = m_boatlon;
        }
        while (set.NextRow())
        {
            DistanceBearingMercator_Plugin( lat, lat, set.GetDouble(2), set.GetDouble(3), &brg, &dist );
            m_pObjSearchDialog->AddObject( set.GetAsString(0),  set.GetAsString(1), set.GetDouble(2), set.GetDouble(3), dist, set.GetDouble(4), set.GetInt(5), set.GetAsString(6) );
        }
        m_pObjSearchDialog->SortResults();
        set.Finalize();
    }
}

ObjSearchDialogImpl::ObjSearchDialogImpl( objsearch_pi* plugin, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : ObjSearchDialog(parent, id, title, pos, size, style )
{
    p_plugin = plugin;
    
    m_btnShowOnChart->Enable(false);
    
    // Make sure we use popup that allows focusing the treectrl.
	m_choiceFeature->UseAltPopupWindow();
	
	// Set popup interface right away, otherwise some of the calls
	// below may fail
	m_clcPopup = new CheckListComboPopup();
	m_choiceFeature->SetPopupControl(m_clcPopup);
}

void ObjSearchDialogImpl::ClearFeatures()
{
    if(m_clcPopup)
        m_clcPopup->Clear();
    m_choiceFeature->SetValue(_("All"));
}

void ObjSearchDialogImpl::AddFeature(wxString feature)
{
    if(m_clcPopup)
    {
        m_clcPopup->Append(feature);
        m_clcPopup->Check(m_clcPopup->GetCount() - 1);
    }
}

void ObjSearchDialogImpl::OnSearch( wxCommandEvent& event )
{
    if ( m_textCtrlSearchTerm->GetValue().Len() == 0 )
    {
        wxMessageBox( _("You did not enter any search term, do so.") );
        return;
    }
    wxString feature_filter = wxEmptyString;
    feature_filter = m_clcPopup->GetStringValue();
    p_plugin->FindObjects( feature_filter, m_textCtrlSearchTerm->GetValue() );
}

void ObjSearchDialogImpl::ClearObjects()
{
    m_listCtrlResults->ClearAll();
    wxListItem col0;
    col0.SetId(0);
    col0.SetText( _("Type") );
    col0.SetWidth(80);
    m_listCtrlResults->InsertColumn(0, col0);

    wxListItem col1;
    col1.SetId(1);
    col1.SetText( _("Name") );
    col1.SetWidth(300);
    m_listCtrlResults->InsertColumn(1, col1);

    wxListItem col2;
    col2.SetId(2);
    col2.SetText( _("Lat") );
    col2.SetWidth(80);
    m_listCtrlResults->InsertColumn(2, col2);
    
    wxListItem col3;
    col3.SetId(3);
    col3.SetText( _("Lon") );
    col3.SetWidth(80);
    m_listCtrlResults->InsertColumn(3, col3);
    
    wxListItem col4;
    col4.SetId(4);
    col4.SetText( wxString::Format( _("Dist (%s)"), getUsrDistanceUnit_Plugin(-1).c_str() ) );
    col4.SetWidth(80);
    m_listCtrlResults->InsertColumn(4, col4);
    
    wxListItem col5;
    col5.SetId(5);
    col5.SetText( _("Scale") );
    col5.SetWidth(0);
    m_listCtrlResults->InsertColumn(5, col5);
    
    wxListItem col6;
    col6.SetId(6);
    col6.SetText( _("Scale") );
    col6.SetWidth(80);
    m_listCtrlResults->InsertColumn(6, col6);
    
    wxListItem col7;
    col7.SetId(7);
    col7.SetText( _("Chart") );
    col7.SetWidth(80);
    m_listCtrlResults->InsertColumn(7, col7);
    
    m_btnShowOnChart->Enable(false);
}

void ObjSearchDialogImpl::AddObject(const wxString& feature, const wxString& objectname, double lat, double lon, double dist, double scale, int nativescale, const wxString& chart)
{  
    wxListItem item;
    int n = m_listCtrlResults->GetItemCount();
    item.SetId(n);
    item.SetText( objectname );

    m_listCtrlResults->InsertItem( item );
    
    m_listCtrlResults->SetItem(n, 0, feature);
    m_listCtrlResults->SetItem(n, 1, objectname);
    m_listCtrlResults->SetItem(n, 2, wxString::Format(_T("%.4f"), lat));
    m_listCtrlResults->SetItem(n, 3, wxString::Format(_T("%.4f"), lon));
    m_listCtrlResults->SetItem(n, 4, wxString::Format(_T("%.1f"), toUsrDistance_Plugin(dist, -1)));
    m_listCtrlResults->SetItem(n, 5, wxString::Format(_T("%.4f"), scale));
    m_listCtrlResults->SetItem(n, 6, wxString::Format(_T("%i"), nativescale));
    m_listCtrlResults->SetItem(n, 7, chart);
    m_listCtrlResults->SetItemData(n, (int) (dist * 10) );
}

void ObjSearchDialogImpl::OnItemSelected( wxListEvent& event )
{
    m_btnShowOnChart->Enable();
}

void ObjSearchDialogImpl::OnClose( wxCommandEvent& event )
{
    Hide();
}

void ObjSearchDialogImpl::OnShowOnChart( wxCommandEvent& event )
{
    long itemIndex = -1;
    itemIndex = m_listCtrlResults->GetNextItem(itemIndex, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (itemIndex == -1) return;
    
    wxListItem     row_info;  
    wxString       cell_contents_string;
 
    row_info.m_itemId = itemIndex;
    row_info.m_col = 2;
    row_info.m_mask = wxLIST_MASK_TEXT;
    m_listCtrlResults->GetItem( row_info );
    double lat;
    row_info.m_text.ToDouble(&lat);
    
    row_info.m_col = 3;
    m_listCtrlResults->GetItem( row_info );
    double lon;
    row_info.m_text.ToDouble(&lon);
    
    row_info.m_col = 5;
    m_listCtrlResults->GetItem( row_info );
    double scale;
    row_info.m_text.ToDouble(&scale);
        
    event.Skip();
    JumpToPosition(lat, lon, scale);
}

int wxCALLBACK ObjectDistanceCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr WXUNUSED(sortData))
{
    if (item1 < item2)
        return -1;
  	if (item1 > item2)
        return 1;

 	return 0;
}

void ObjSearchDialogImpl::SortResults()
{
    m_listCtrlResults->SortItems(ObjectDistanceCompareFunction, 0);
}

void *DbThread::Entry()
{
    while (!TestDestroy())
    {
        while (m_pHandler->HasQueries())
        {
            //wxString sql(query_queue.front().c_str(), wxConvUTF8);
            m_pHandler->QueryDB(m_pHandler->GetQuery());
            //query_queue.pop();
        }
        Sleep(500);
        //wxQueueEvent(m_pHandler, new wxThreadEvent(wxEVT_COMMAND_DBTHREAD_UPDATE));
    }
    // signal the event handler that this thread is going to be destroyed
    // NOTE: here we assume that using the m_pHandler pointer is safe,
    // (in this case this is assured by the MyFrame destructor)
    //    wxQueueEvent(m_pHandler, new wxThreadEvent(wxEVT_COMMAND_DBTHREAD_COMPLETED));
    //return (wxThread::ExitCode)0; // success

    return 0;
}


DbThread::~DbThread()
{
    wxCriticalSectionLocker enter(m_pHandler->m_pThreadCS);
    m_pHandler->m_pThread = NULL;
}

wxString objsearch_pi::GetQuery()
{
    wxString query = query_queue.front();
    query_queue.pop();
    return query;
}

bool objsearch_pi::HasQueries()
{
    return !query_queue.empty();
}
