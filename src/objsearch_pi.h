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

#ifndef _OBJSEARCHPI_H_
#define _OBJSEARCHPI_H_

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <map>
#include <wx/fileconf.h>

#define     PLUGIN_VERSION_MAJOR    0
#define     PLUGIN_VERSION_MINOR    1

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    11

#include "../../../include/ocpn_plugin.h"

#include "ObjSearchDialog.h"


//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------

#define OBJSEARCH_TOOL_POSITION    -1          // Request default positioning of toolbar tool

class objsearch_pi;

class ObjSearchDialogImpl : public ObjSearchDialog
{
public:
    ObjSearchDialogImpl( objsearch_pi* plugin, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Chart Object Search"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 649,560 ), long style = wxDEFAULT_DIALOG_STYLE ); 

    void ClearFeatures();
    void AddFeature(wxString feature);
    
    void ClearObjects();
    void AddObject(const wxString& feature, const wxString& objectname, double lat, double lon);
    
    objsearch_pi *p_plugin;
    
protected:
    void OnSearch( wxCommandEvent& event );
    void OnItemSelected( wxListEvent& event );
    void OnClose( wxCommandEvent& event );
    void OnShowOnChart( wxCommandEvent& event );
};

class objsearch_pi : public opencpn_plugin_111
{
public:
    objsearch_pi ( void *ppimgr );
    ~objsearch_pi ( void );

//    The required PlugIn Methods
    int Init ( void );
    bool DeInit ( void );

    int GetAPIVersionMajor();
    int GetAPIVersionMinor();
    int GetPlugInVersionMajor();
    int GetPlugInVersionMinor();
    wxBitmap *GetPlugInBitmap();
    wxString GetCommonName();
    wxString GetShortDescription();
    wxString GetLongDescription();

//    The override PlugIn Methods
    void SetCursorLatLon ( double lat, double lon );
    int GetToolbarToolCount ( void );
    void OnToolbarToolCallback ( int id );
    void SetPositionFix( PlugIn_Position_Fix &pfix );
    void SendVectorChartObjectInfo( wxString &chart, wxString &feature, wxString &objname, double lat, double lon );

// Other public methods
    void SetColorScheme ( PI_ColorScheme cs );
    
    void FindObjects( const wxString& feature_filter, const wxString& search_string );

private:
    bool LoadConfig ( void );
    bool SaveConfig ( void );

    wxFileConfig *m_pconfig;
    wxWindow *m_parent_window;

    int m_display_width, m_display_height;
    int m_leftclick_tool_id;
    
    ObjSearchDialogImpl *m_pObjSearchDialog;
    
    std::map<wxString, int> m_chartsInDb;
    std::map<wxString, int> m_featuresInDb;
    wxSQLite3Database *m_db;
    
    int GetChartId(wxString chart);
    int GetFeatureId(wxString feature);
    
    wxLongLong StoreNewChart(wxString chart);
    wxLongLong StoreNewFeature(wxString feature);
    void StoreNewObject(long chart_id, long feature_id, wxString objname, double lat, double lon);
    
    wxString m_chartLoading;
    bool m_bDBUsable;
    
    wxSQLite3Database* initDB(void);
    void clearDB(wxSQLite3Database* db);
    
    int QueryDB(wxSQLite3Database* db, const wxString& sql);
    wxSQLite3ResultSet SelectFromDB(wxSQLite3Database* db, const wxString& sql);

};

#endif
