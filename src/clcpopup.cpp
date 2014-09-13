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
 
 #include "clcpopup.h"

BEGIN_EVENT_TABLE( CheckListComboPopup, wxCheckListBox )
    EVT_CHECKLISTBOX( wxID_ANY, CheckListComboPopup::OnListBox )
END_EVENT_TABLE()

wxString CheckListComboPopup::GetStringValue() const
{
    bool add_comma = false;
    wxString ret = wxEmptyString;
    for ( unsigned int i = 0; i < GetCount(); i++ )
        if ( IsChecked(i) )
        {
            if ( add_comma )
                ret += _T(",");
            else
                add_comma = true;
            ret += ((wxStringClientData*)GetClientObject(i))->GetData();
        }

    return ret;
}


void CheckListComboPopup::OnListBox(wxCommandEvent& event)
{
    int item_id = event.GetInt();
/*        if IsChecked(item_id)
    {
        //blah
    }
    else
    {
        //blah
    }*/
}
