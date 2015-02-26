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

#include "uFormVirtualMgr.h"
#include "uFtCore.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TFormVirtualMgr::TFormVirtualMgr(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
bool TFormVirtualMgr::Execute(CFtVirtualTorrentMgr *a_VtMgr)
{
    m_bDirty = false;
    m_VtMgr = a_VtMgr;
    Initialize();
    if(this->ShowModal() == mrOk)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
void TFormVirtualMgr::Initialize(void)
{
    UpdateVirtualList();
}

//---------------------------------------------------------------------------
void TFormVirtualMgr::UpdateVirtualList(void)
{
    ListViewVirtual->Items->Clear();
    ListViewVirtual->Clear();
    if(!m_VtMgr) return;

    UINT cou = m_VtMgr->GetCount();
    for(UINT a=0; a < cou; a++)
    {
        CFtVirtualTorrent *tor = m_VtMgr->GetVTorrent(a);
        if(!tor) continue;
        TListItem *it = ListViewVirtual->Items->Add();
        if(it)
        {
            it->Caption = ExtractFileName( tor->GetFileName() );
            it->Checked = tor->GetUsed();
            it->SubItems->Add( FloatToStr(tor->GetUploadSpeed())+" Кбит/сек" );

            //Отображаем в списке только используемые трекеры
            AnsiString sTracks = "";
            UINT cou = tor->GetTrackersCount();
            for(UINT a=0; a < cou; a++)
            {
                TFtTracker *track = tor->GetTracker(a);
                if(track)
                {
                    if(track->m_bUse)
                    {
                        sTracks += track->m_sTracker + "; ";
                    }
                }
            }
            it->SubItems->Add( sTracks );
        }
    }
    UpdateBtns();    
}

//---------------------------------------------------------------------------
void TFormVirtualMgr::UpdateBtns(void)
{
    if(ListViewVirtual->ItemIndex >= 0)
    {
        BtnEdit->Enabled = true;
        BtnDelete->Enabled = true;
    }
    else
    {
        BtnEdit->Enabled = false;
        BtnDelete->Enabled = false;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualMgr::BtnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;    
}
//---------------------------------------------------------------------------
void __fastcall TFormVirtualMgr::BtnOkClick(TObject *Sender)
{
    ModalResult = mrOk;    
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualMgr::BtnDeleteClick(TObject *Sender)
{
    if(!m_VtMgr) return;
    if(ListViewVirtual->ItemIndex >= 0)
    {
        AnsiString str = "Вы действительно хотите удалить этот виртуальный торрент?";
        if( MessageBox(Handle, str.c_str(), "Внимание!", MB_YESNO|MB_ICONQUESTION) == IDYES)
        {
            m_VtMgr->DeleteVTorrent(ListViewVirtual->ItemIndex);
            m_bDirty = true;
            UpdateVirtualList();
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualMgr::BtnAddClick(TObject *Sender)
{
    if(!m_VtMgr) return;

    //Если это урезанная версия лайт, то можно только 1 вирт.торрент
    if(g_Ver)
    {
        if(g_Ver->GetProgrType()==FTTYPE_VER_LIGHT)
        {
            if(m_VtMgr->GetCount() > 0)
            {
                MessageBox(Handle, "Вы используете Начальную версию FreeTorrents.\n"
                                   "В этой версии допустимо создание только 1 виртуального торрента.\n"
                                   "Если Вам необходимо большее количество виртуальных торрентов, то Вы можете\n"
                                   "приобрести расширенную VIP версию на сайте программы.",
                                   "Внимание!", MB_OK|MB_ICONINFORMATION);
                return;
            }
        }
    }

    TFormVirtualTorrent *frm = new TFormVirtualTorrent(this);
    if(frm)
    {
        CFtVirtualTorrent tmpTor;
        if(frm->Execute(&tmpTor))
        {
            m_VtMgr->CreateVTorrent(&tmpTor);
            m_bDirty = true;
            UpdateVirtualList();
        }
        delete frm;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualMgr::BtnEditClick(TObject *Sender)
{
    if(!m_VtMgr) return;
    if(ListViewVirtual->ItemIndex >= 0)
    {
        TFormVirtualTorrent *frm = new TFormVirtualTorrent(this);
        if(frm)
        {
            CFtVirtualTorrent *tor = m_VtMgr->GetVTorrent( ListViewVirtual->ItemIndex );
            CFtVirtualTorrent tmpTor = *tor;
            if(frm->Execute(&tmpTor, FT_FTYPE_VT_EDIT))
            {
                if(frm->GetDirty())
                {
                    *tor = tmpTor;
                    m_bDirty = true;
                    UpdateVirtualList();
                }
            }
            delete frm;
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualMgr::ListViewVirtualDblClick(TObject *Sender)
{
    BtnEditClick(Sender);
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualMgr::ListViewVirtualSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    UpdateBtns();    
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualMgr::ListViewVirtualClick(TObject *Sender)
{
    if(!m_VtMgr) return;
    UINT cou = m_VtMgr->GetCount();
    for(UINT a=0; a < cou; a++)
    {
        CFtVirtualTorrent *tor = m_VtMgr->GetVTorrent(a);
        if(!tor) continue;
        if(tor->GetUsed() != ListViewVirtual->Items->Item[a]->Checked)
        {
            tor->SetUsed( ListViewVirtual->Items->Item[a]->Checked );
            m_bDirty = true;
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualMgr::ListViewVirtualKeyPress(TObject *Sender,
      char &Key)
{
    if(Key == VK_SPACE)
        ListViewVirtualClick(Sender);
}
//---------------------------------------------------------------------------
bool TFormVirtualMgr::GetDirty(void)
{
    return m_bDirty;
}
