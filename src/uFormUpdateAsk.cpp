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

#include "uFormUpdateAsk.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TFormUpdateAsk::TFormUpdateAsk(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
bool TFormUpdateAsk::Execute(void)
{
    if(this->ShowModal() == mrOk)
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TFormUpdateAsk::BtnOkClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormUpdateAsk::BtnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
void __fastcall TFormUpdateAsk::LabelSiteClick(TObject *Sender)
{
    FtOpenHttpPage("http://freetorrents.mrsun.ru");    
}
//---------------------------------------------------------------------------
