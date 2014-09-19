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
    
    if ( m_bDBUsable )
	{
		QueryDB( db, _T("PRAGMA synchronous=OFF") );
        QueryDB( db, _T("PRAGMA count_changes=OFF") );
        QueryDB( db, _T("PRAGMA journal_mode=MEMORY") );
        QueryDB( db, _T("PRAGMA temp_store=MEMORY") );
        
        //Fix the broken objects created by v 0.1 and 0.2
        QueryDB( db, _T("UPDATE object SET lon = lon - 360 WHERE lon > 180") );
        QueryDB( db, _T("UPDATE object SET lon = lon + 360 WHERE lon < - 180") );
        QueryDB( db, _T("DELETE FROM object WHERE lon < - 180 OR lon > 180 OR lat < -90 OR lat > 90") );
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
    if (!m_bDBUsable)
        return wxSQLite3ResultSet();
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
    : opencpn_plugin_112 ( ppimgr )
{
    // Create the PlugIn icons
    initialize_images();
    
    m_bDBUsable = true;
    
    m_bWaitForDB = true;
    
    m_db = initDB();
    
    wxSQLite3ResultSet set;
    
    if (m_bDBUsable)
    {
        set = SelectFromDB( m_db, wxT("SELECT id, chartname, scale, nativescale FROM chart") );
        if (m_bDBUsable)
        {
            while (set.NextRow())
            {
                Chart ch;
                ch.id = set.GetInt(0);
                ch.name = set.GetAsString(1);
                ch.scale = set.GetDouble(2);
                ch.nativescale = set.GetInt(3);
                m_chartsInDb[ch.name] = ch;
            }
        }
        set.Finalize();
    }
    if (m_bDBUsable)
    {
        set = SelectFromDB( m_db, wxT("SELECT id, featurename FROM feature"));
        if (m_bDBUsable)
        {
            while (set.NextRow())
            {
                m_featuresInDb[set.GetAsString(1)] = set.GetInt(0);
            }
        }
        set.Finalize();
    }
    
    m_bWaitForDB = false;
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
        // Do not wait and assume the thread will have enough time to die during the OPenCPN shutdown sequence // wxThread::This()->Sleep(1);
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
    
    m_bWaitForDB = false;
    
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
        m_bWaitForDB = true;
        Chart ch = StoreNewChart( chart, scale, nativescale );
        m_chartsInDb[ch.name] = ch;
        m_bWaitForDB = false;
    }
    if ( feature_id == 0 )
    {
        m_bWaitForDB = true;
        feature_id = StoreNewFeature( feature ).ToLong();
        m_featuresInDb[feature] = feature_id;
        m_bWaitForDB = false;
    }
        
    if ( chart == m_chartLoading )
        StoreNewObject( chart_id, feature_id, objname, lat, lon );
}

Chart objsearch_pi::StoreNewChart(wxString chart, double scale, int nativescale)
{
    Chart ch;
    if ( !m_bDBUsable )
        return ch;
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
    
    // get object on the world chart...    
    if( lon < -180. )
        lon += 360.;
    else if( lon > 180. )
        lon -= 360.;
    
    // if it is still off the map, forget about it...
    if ( lon > 180. || lon < -180. || lat > 90. || lat < -90.)
        return;
                
    while ( m_bWaitForDB )
        wxMilliSleep(1);
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
    if (!m_bDBUsable)
    {
        wxMessageBox(_("There is a problem with your database, check the OpenCPN logfile for more information."));
        return;
    }
    m_pObjSearchDialog->ClearObjects();
    wxString safe_value = search_string;
    safe_value.Replace(_T("'"), _T("''"));
    wxSQLite3ResultSet set = SelectFromDB( m_db, wxString::Format( wxT("SELECT COUNT(*) FROM object o LEFT JOIN feature f ON (o.feature_id = f.id) WHERE instr('%s', featurename) > 0 AND objname LIKE '%%%s%%'"), feature_filter.c_str(), safe_value.c_str() ) );
    int objects_found = 0;
    if (m_bDBUsable)
    {
        objects_found = set.GetInt(0);
    }
    set.Finalize();
    int show = wxYES;
    if ( objects_found > 1000 )
    {
        show = wxMessageBox( wxString::Format( _("Your search resulted in %i objects found. This is a lot, do you really want to show all of them?"), objects_found ), _("Too many objects found"), wxYES_NO | wxCENTER );
    }
    if ( show == wxYES )
    {
        if (m_bDBUsable)
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
            if (m_bDBUsable)
            {
                while (set.NextRow())
                {
                    DistanceBearingMercator_Plugin( lat, lon, set.GetDouble(2), set.GetDouble(3), &brg, &dist );
                    m_pObjSearchDialog->AddObject( set.GetAsString(0),  set.GetAsString(1), set.GetDouble(2), set.GetDouble(3), dist, set.GetDouble(4), set.GetInt(5), set.GetAsString(6) );
                }
                m_pObjSearchDialog->SortResults();
            }
            set.Finalize();
        }
    }
}

ObjSearchDialogImpl::ObjSearchDialogImpl( objsearch_pi* plugin, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : ObjSearchDialog(parent, id, title, pos, size, style )
{
    p_plugin = plugin;
    
    m_btnShowOnChart->Enable(false);
    
	// Set popup interface right away, otherwise some of the calls
	// below may fail
	m_clcPopup = new CheckListComboPopup();
	m_choiceFeature->SetPopupControl(m_clcPopup);
}

void ObjSearchDialogImpl::ClearFeatures()
{
    if(m_clcPopup)
    {
        m_clcPopup->Clear();
        m_choiceFeature->SetValue(_("All"));
        ((wxCheckListBox*)m_clcPopup)->Append(_("All"));
        m_clcPopup->Check(0);
    }
}

void ObjSearchDialogImpl::AddFeature(const wxString& feature)
{
    if(m_clcPopup)
    {
        m_clcPopup->Append(HumanizeFeatureName(feature), feature);
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
    
    m_listCtrlResults->SetItem(n, 0, HumanizeFeatureName(feature));
    m_listCtrlResults->SetItem(n, 1, objectname);
    m_listCtrlResults->SetItem(n, 2, toSDMM_PlugIn(1, lat));
    m_listCtrlResults->SetItem(n, 3, toSDMM_PlugIn(2, lon));
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

double fromDMM( wxString sdms )
{
    wchar_t buf[64];
    char narrowbuf[64];
    int i, len, top = 0;
    double stk[32], sign = 1;

    //First round of string modifications to accomodate some known strange formats
    wxString replhelper;
    replhelper = wxString::FromUTF8( "´·" ); //UKHO PDFs
    sdms.Replace( replhelper, _T(".") );
    replhelper = wxString::FromUTF8( "\"·" ); //Don't know if used, but to make sure
    sdms.Replace( replhelper, _T(".") );
    replhelper = wxString::FromUTF8( "·" );
    sdms.Replace( replhelper, _T(".") );

    replhelper = wxString::FromUTF8( "s. š." ); //Another example: cs.wikipedia.org (someone was too active translating...)
    sdms.Replace( replhelper, _T("N") );
    replhelper = wxString::FromUTF8( "j. š." );
    sdms.Replace( replhelper, _T("S") );
    sdms.Replace( _T("v. d."), _T("E") );
    sdms.Replace( _T("z. d."), _T("W") );

    //If the string contains hemisphere specified by a letter, then '-' is for sure a separator...
    sdms.UpperCase();
    if( sdms.Contains( _T("N") ) || sdms.Contains( _T("S") ) || sdms.Contains( _T("E") )
            || sdms.Contains( _T("W") ) ) sdms.Replace( _T("-"), _T(" ") );

    wcsncpy( buf, sdms.wc_str( wxConvUTF8 ), 64 );
    len = wcslen( buf );

    for( i = 0; i < len; i++ ) {
        wchar_t c = buf[i];
        if( ( c >= '0' && c <= '9' ) || c == '-' || c == '.' || c == '+' ) {
            narrowbuf[i] = c;
            continue; /* Digit characters are cool as is */
        }
        if( c == ',' ) {
            narrowbuf[i] = '.'; /* convert to decimal dot */
            continue;
        }
        if( ( c | 32 ) == 'w' || ( c | 32 ) == 's' ) sign = -1; /* These mean "negate" (note case insensitivity) */
        narrowbuf[i] = 0; /* Replace everything else with nuls */
    }

    /* Build a stack of doubles */
    stk[0] = stk[1] = stk[2] = 0;
    for( i = 0; i < len; i++ ) {
        while( i < len && narrowbuf[i] == 0 )
            i++;
        if( i != len ) {
            stk[top++] = atof( narrowbuf + i );
            i += strlen( narrowbuf + i );
        }
    }

    return sign * ( stk[0] + ( stk[1] + stk[2] / 60 ) / 60 );
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
    double lat = fromDMM(row_info.m_text);
    
    row_info.m_col = 3;
    m_listCtrlResults->GetItem( row_info );
    double lon = fromDMM(row_info.m_text);
    
    row_info.m_col = 5;
    m_listCtrlResults->GetItem( row_info );
    double scale;
    row_info.m_text.ToDouble(&scale);
    if (scale < 0.001)
        scale = 0.001;
        
    event.Skip();
    JumpToPosition(lat, lon, scale);
}

wxString ObjSearchDialogImpl::HumanizeFeatureName(const wxString& feature_name)
{
    if ( feature_name == _T("ADMARE") )
        return _("Administration area (Named)");
    if ( feature_name == _T("AIRARE") )
        return _("Airport / airfield");
    if ( feature_name == _T("ACHBRT") )
        return _("Anchor berth");
    if ( feature_name == _T("ACHARE") )
        return _("Anchorage area");
    if ( feature_name == _T("BCNCAR") )
        return _("Beacon, cardinal");
    if ( feature_name == _T("BCNISD") )
        return _("Beacon, isolated danger");
    if ( feature_name == _T("BCNLAT") )
        return _("Beacon, lateral");
    if ( feature_name == _T("BCNSAW") )
        return _("Beacon, safe water");
    if ( feature_name == _T("BCNSPP") )
        return _("Beacon, special purpose/general");
    if ( feature_name == _T("BERTHS") )
        return _("Berth");
    if ( feature_name == _T("BRIDGE") )
        return _("Bridge");
    if ( feature_name == _T("BUISGL") )
        return _("Building, single");
    if ( feature_name == _T("BUAARE") )
        return _("Built-up area");
    if ( feature_name == _T("BOYCAR") )
        return _("Buoy, cardinal");
    if ( feature_name == _T("BOYINB") )
        return _("Buoy, installation");
    if ( feature_name == _T("BOYISD") )
        return _("Buoy, isolated danger");
    if ( feature_name == _T("BOYLAT") )
        return _("Buoy, lateral");
    if ( feature_name == _T("BOYSAW") )
        return _("Buoy, safe water");
    if ( feature_name == _T("BOYSPP") )
        return _("Buoy, special purpose/general");
    if ( feature_name == _T("CBLARE") )
        return _("Cable area");
    if ( feature_name == _T("CBLOHD") )
        return _("Cable, overhead");
    if ( feature_name == _T("CBLSUB") )
        return _("Cable, submarine");
    if ( feature_name == _T("CANALS") )
        return _("Canal");
    if ( feature_name == _T("CANBNK") )
        return _("Canal bank");
    if ( feature_name == _T("CTSARE") )
        return _("Cargo transshipment area");
    if ( feature_name == _T("CAUSWY") )
        return _("Causeway");
    if ( feature_name == _T("CHKPNT") )
        return _("Checkpoint");
    if ( feature_name == _T("CGUSTA") )
        return _("Coastguard station");
    if ( feature_name == _T("COALNE") )
        return _("Coastline");
    if ( feature_name == _T("COSARE") )
        return _("Continental shelf area");
    if ( feature_name == _T("CTRPNT") )
        return _("Control point");
    if ( feature_name == _T("CONVYR") )
        return _("Conveyor");
    if ( feature_name == _T("CRANES") )
        return _("Crane");
    if ( feature_name == _T("CURENT") )
        return _("Current - non - gravitational");
    if ( feature_name == _T("DAMCON") )
        return _("Dam");
    if ( feature_name == _T("DAYMAR") )
        return _("Daymark");
    if ( feature_name == _T("DWRTCL") )
        return _("Deep water route centerline");
    if ( feature_name == _T("DWRTPT") )
        return _("Deep water route part");
    if ( feature_name == _T("DISMAR") )
        return _("Distance mark");
    if ( feature_name == _T("DOCARE") )
        return _("Dock area");
    if ( feature_name == _T("DRGARE") )
        return _("Dredged area");
    if ( feature_name == _T("DRYDOC") )
        return _("Dry dock");
    if ( feature_name == _T("DMPGRD") )
        return _("Dumping ground");
    if ( feature_name == _T("FAIRWY") )
        return _("Fairway");
    if ( feature_name == _T("FNCLNE") )
        return _("Fence/wall");
    if ( feature_name == _T("FERYRT") )
        return _("Ferry route");
    if ( feature_name == _T("FSHZNE") )
        return _("Fishery zone");
    if ( feature_name == _T("FSHFAC") )
        return _("Fishing facility");
    if ( feature_name == _T("FSHGRD") )
        return _("Fishing ground");
    if ( feature_name == _T("FLODOC") )
        return _("Floating dock");
    if ( feature_name == _T("FOGSIG") )
        return _("Fog signal");
    if ( feature_name == _T("FORSTC") )
        return _("Fortified structure");
    if ( feature_name == _T("FRPARE") )
        return _("Free port area");
    if ( feature_name == _T("GATCON") )
        return _("Gate");
    if ( feature_name == _T("GRIDRN") )
        return _("Gridiron");
    if ( feature_name == _T("HRBARE") )
        return _("Harbour area (administrative)");
    if ( feature_name == _T("HRBFAC") )
        return _("Harbour facility");
    if ( feature_name == _T("HULKES") )
        return _("Hulk");
    if ( feature_name == _T("ICEARE") )
        return _("Ice area");
    if ( feature_name == _T("ICNARE") )
        return _("Incineration area");
    if ( feature_name == _T("LAKARE") )
        return _("Lake");
    if ( feature_name == _T("LAKSHR") )
        return _("Lake shore");
    if ( feature_name == _T("LNDARE") )
        return _("Land area");
    if ( feature_name == _T("LNDELV") )
        return _("Land elevation");
    if ( feature_name == _T("LNDRGN") )
        return _("Land region");
    if ( feature_name == _T("LNDMRK") )
        return _("Landmark");
    if ( feature_name == _T("LIGHTS") )
        return _("Light");
    if ( feature_name == _T("LITFLT") )
        return _("Light float");
    if ( feature_name == _T("LITVES") )
        return _("Light vessel");
    if ( feature_name == _T("LOCMAG") )
        return _("Local magnetic anomaly");
    if ( feature_name == _T("LOKBSN") )
        return _("Lock basin");
    if ( feature_name == _T("LOGPON") )
        return _("Log pond");
    if ( feature_name == _T("MARCUL") )
        return _("Marine farm/culture");
    if ( feature_name == _T("MIPARE") )
        return _("Military practice area");
    if ( feature_name == _T("MORFAC") )
        return _("Mooring/warping facility");
    if ( feature_name == _T("OBSTRN") )
        return _("Obstruction");
    if ( feature_name == _T("OFSPLF") )
        return _("Offshore platform");
    if ( feature_name == _T("OSPARE") )
        return _("Offshore production area");
    if ( feature_name == _T("OILBAR") )
        return _("Oil barrier");
    if ( feature_name == _T("PILPNT") )
        return _("Pile");
    if ( feature_name == _T("PILBOP") )
        return _("Pilot boarding place");
    if ( feature_name == _T("PIPARE") )
        return _("Pipeline area");
    if ( feature_name == _T("PIPOHD") )
        return _("Pipeline, overhead");
    if ( feature_name == _T("PIPSOL") )
        return _("Pipeline, submarine/on land");
    if ( feature_name == _T("PONTON") )
        return _("Pontoon");
    if ( feature_name == _T("PRDARE") )
        return _("Production / storage area");
    if ( feature_name == _T("PYLONS") )
        return _("Pylon/bridge support");
    if ( feature_name == _T("RADLNE") )
        return _("Radar line");
    if ( feature_name == _T("RADRNG") )
        return _("Radar range");
    if ( feature_name == _T("RADSTA") )
        return _("Radar station");
    if ( feature_name == _T("RTPBCN") )
        return _("Radar transponder beacon");
    if ( feature_name == _T("RDOCAL") )
        return _("Radio calling-in point");
    if ( feature_name == _T("RDOSTA") )
        return _("Radio station");
    if ( feature_name == _T("RAILWY") )
        return _("Railway");
    if ( feature_name == _T("RAPIDS") )
        return _("Rapids");
    if ( feature_name == _T("RCRTCL") )
        return _("Recommended route centerline");
    if ( feature_name == _T("RECTRC") )
        return _("Recommended track");
    if ( feature_name == _T("RSCSTA") )
        return _("Rescue station");
    if ( feature_name == _T("RESARE") )
        return _("Restricted area");
    if ( feature_name == _T("RIVERS") )
        return _("River");
    if ( feature_name == _T("RIVBNK") )
        return _("River bank");
    if ( feature_name == _T("ROADWY") )
        return _("Road");
    if ( feature_name == _T("RUNWAY") )
        return _("Runway");
    if ( feature_name == _T("SEAARE") )
        return _("Sea area / named water area");
    if ( feature_name == _T("SPLARE") )
        return _("Sea-plane landing area");
    if ( feature_name == _T("SBDARE") )
        return _("Seabed area");
    if ( feature_name == _T("SLCONS") )
        return _("Shoreline Construction");
    if ( feature_name == _T("SISTAT") )
        return _("Signal station, traffic");
    if ( feature_name == _T("SISTAW") )
        return _("Signal station, warning");
    if ( feature_name == _T("SILTNK") )
        return _("Silo / tank");
    if ( feature_name == _T("SLOTOP") )
        return _("Slope topline");
    if ( feature_name == _T("SLOGRD") )
        return _("Sloping ground");
    if ( feature_name == _T("SMCFAC") )
        return _("Small craft facility");
    if ( feature_name == _T("SOUNDG") )
        return _("Sounding");
    if ( feature_name == _T("SPRING") )
        return _("Spring");
    if ( feature_name == _T("SQUARE") )
        return _("Square");
    if ( feature_name == _T("SUBTLN") )
        return _("Submarine transit lane");
    if ( feature_name == _T("TS_PRH") )
        return _("Tidal stream - harmonic prediction");
    if ( feature_name == _T("TS_PNH") )
        return _("Tidal stream - non-harmonic prediction");
    if ( feature_name == _T("TS_PAD") )
        return _("Tidal stream panel data");
    if ( feature_name == _T("TS_TIS") )
        return _("Tidal stream - time series");
    if ( feature_name == _T("T_HMON") )
        return _("Tide - harmonic prediction");
    if ( feature_name == _T("T_NHMN") )
        return _("Tide - non-harmonic prediction");
    if ( feature_name == _T("T_TIMS") )
        return _("Tidal stream - time series");
    if ( feature_name == _T("TIDEWY") )
        return _("Tideway");
    if ( feature_name == _T("TUNNEL") )
        return _("Tunnel");
    if ( feature_name == _T("UWTROC") )
        return _("Underwater rock / awash rock");
    if ( feature_name == _T("VEGATN") )
        return _("Vegetation");
    if ( feature_name == _T("WATTUR") )
        return _("Water turbulence");
    if ( feature_name == _T("WATFAL") )
        return _("Waterfall");
    if ( feature_name == _T("WEDKLP") )
        return _("Weed/Kelp");
    if ( feature_name == _T("WRECKS") )
        return _("Wreck");
    if ( feature_name == _T("TS_FEB") )
        return _("Tidal stream - flood/ebb");
    if ( feature_name == _T("C_AGGR") )
        return _("Aggregation");
    if ( feature_name == _T("C_ASSO") )
        return _("Association");
    if ( feature_name == _T("canbnk") )
        return _("Canal bank");
    if ( feature_name == _T("rivbnk") )
        return _("River bank");
    if ( feature_name == _T("wtwaxs") )
        return _("waterway axis");
    if ( feature_name == _T("brgare") )
        return _("Bridge area");
    if ( feature_name == _T("lokare") )
        return _("Lock area");
    if ( feature_name == _T("bcnwtw") )
        return _("Beacon water-way");
    if ( feature_name == _T("boywtw") )
        return _("Buoy water-way");
    if ( feature_name == _T("rtplpt") )
        return _("Route planning point");
    if ( feature_name == _T("_texto") )
        return _("Text label");
    if ( feature_name == _T("BRTFAC") )
        return _("Berthing facility");
    if ( feature_name == _T("BUIREL") )
        return _("Building, religious");
    if ( feature_name == _T("CTNARE") )
        return _("Caution area");
    if ( feature_name == _T("MONUMT") )
        return _("Monument");
    if ( feature_name == _T("PRDINS") )
        return _("Production installation");
    if ( feature_name == _T("SLTPAN") )
        return _("Salt pan");
    if ( feature_name == _T("TNKCON") )
        return _("Tank");
    if ( feature_name == _T("TOWERS") )
        return _("Tower structure");
    return feature_name;
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
        m_pHandler->QueryDB(_T("BEGIN TRANSACTION"));
        while (m_pHandler->HasQueries())
        {
            m_pHandler->QueryDB(m_pHandler->GetQuery());
        }
        m_pHandler->QueryDB(_T("COMMIT TRANSACTION"));
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
