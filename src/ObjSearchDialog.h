///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Feb 26 2014)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __OBJSEARCHDIALOG_H__
#define __OBJSEARCHDIALOG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/choice.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ObjSearchDialog
///////////////////////////////////////////////////////////////////////////////
class ObjSearchDialog : public wxDialog 
{
	private:
	
	protected:
		wxChoice* m_choiceFeature;
		wxTextCtrl* m_textCtrlSearchTerm;
		wxButton* m_buttonSearch;
		wxListCtrl* m_listCtrlResults;
		wxButton* m_btnShowOnChart;
		wxButton* m_btnClose;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnFeatureSelected( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnSearch( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void OnShowOnChart( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnClose( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		ObjSearchDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Chart Object Search"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 649,560 ), long style = wxDEFAULT_DIALOG_STYLE ); 
		~ObjSearchDialog();
			
};

#endif //__OBJSEARCHDIALOG_H__
