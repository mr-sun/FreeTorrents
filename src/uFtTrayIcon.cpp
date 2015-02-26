/****************************************************************************
* FreeTorrents
* Copyright (C) 2009 Vitaly Kuznetsov <freetorrents@mrsun.ru>
*
* FreeTorrent is a standalone application which fakes upload and download stats
* of a torrent to almost all bittorrent trackers. http://ft.mrsun.ru
*
* This programm is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this software. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#include <vcl.h>
#pragma hdrstop

#include "uFtTrayIcon.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall CFtTrayIcon::CFtTrayIcon(TComponent* Owner)
                        : TTrayIcon(Owner)
{
}

//---------------------------------------------------------------------------
bool CFtTrayIcon::SetBigHint(AnsiString a_BigHint)
{
    m_BigHint = a_BigHint;
    Update();
    return true;
}

//---------------------------------------------------------------------------
void __fastcall CFtTrayIcon::Update()
{
    UINT nTipSize = sizeof(Data.szTip);
    memset(Data.szTip, 0, nTipSize);
    StrPLCopy(Data.szTip, m_BigHint.SubString(1, nTipSize-1), nTipSize-1);
    Data.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
    TTrayIcon::Update();
}

//---------------------------------------------------------------------------
bool CFtTrayIcon::BalloonHint(AnsiString a_sTitle, AnsiString a_sHint, UINT a_nTimeout, DWORD a_nIcon)
{
    Data.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON | NIF_INFO; // We add a flag, NIF_INFO
    StrPLCopy(Data.szInfoTitle, a_sTitle, 63);                        // We set the balloon hint title
    StrPLCopy(Data.szInfo, "", 255);                               // We clear text for deleting
    Shell_NotifyIcon(NIM_MODIFY, &Data);                           // We modify the icon
    StrPLCopy(Data.szInfo, a_sHint, 255);                             // We set the balloon hint text
    Data.uTimeout = a_nTimeout * 1000;                               // We set the timeout (ms)
    Data.dwInfoFlags = a_nIcon;                           // We set the icon
    Shell_NotifyIcon(NIM_MODIFY, &Data);                           // We modify the icon again !
    Update();
    return true;
}
