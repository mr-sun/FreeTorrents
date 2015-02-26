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

#include "uFormVirtualTorrent.h"
#include "uFtCore.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TFormVirtualTorrent::TFormVirtualTorrent(TComponent* Owner)
    : TForm(Owner)
{
}

//---------------------------------------------------------------------------
bool TFormVirtualTorrent::Execute(CFtVirtualTorrent *a_VTorrent, int a_Type)
{
    m_bDirty = false;
    m_VTorrent = a_VTorrent;
    Initialize();
    if(this->ShowModal() == mrOk)
    {
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
void TFormVirtualTorrent::Initialize(void)
{
    UpdateData();
}

//---------------------------------------------------------------------------
void TFormVirtualTorrent::UpdateData(void)
{
    if(!m_VTorrent) return;

    EditUpSpeed->Text = FloatToStr(m_VTorrent->GetUploadSpeed());
    EditFName->Text = m_VTorrent->GetFileName();
    EditName->Text = ExtractFileName(m_VTorrent->GetFileName());
    EditHash->Text = m_VTorrent->GetHash20();

    if( EditFName->Text == "")
    {
        BtnOk->Enabled = false;
    }
    else
    {
        BtnOk->Enabled = true;    
    }

    ListViewTrackers->Items->Clear();
    ListViewTrackers->Clear();    
    UINT cou = m_VTorrent->GetTrackersCount();
    for(UINT a=0; a < cou; a++)
    {
        TFtTracker *track = m_VTorrent->GetTracker(a);
        if(track)
        {
            TListItem *it = ListViewTrackers->Items->Add();
            if(it)
            {
                it->Caption = track->m_sTracker;
                it->Checked = track->m_bUse;
            }
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualTorrent::BtnOkClick(TObject *Sender)
{
    if(!Apply()) return;
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormVirtualTorrent::BtnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

//---------------------------------------------------------------------------
void __fastcall TFormVirtualTorrent::BtnBrowseClick(TObject *Sender)
{
    AnsiString oldFname = EditFName->Text;
    if(OpenDialog1->Execute())
    {
        AnsiString fname = OpenDialog1->FileName;
        if(fname != oldFname)
        {
            //Имя изменилось
            m_VTorrent->SetFileName(fname);
            m_VTorrent->Initialize();
            m_bDirty = true;            
            UpdateData();
        }
    }
}

//---------------------------------------------------------------------------
bool TFormVirtualTorrent::Apply(void)
{
    if(!m_VTorrent) return true;

    char ds = DecimalSeparator;
    DecimalSeparator = '.';

    //Скорость закачки
    AnsiString sUpSpeed = EditUpSpeed->Text;
    sUpSpeed = AnsiReplaceStr(sUpSpeed, ",", ".");
    double fUpSpeed;
    try
    {
        fUpSpeed = StrToFloat(sUpSpeed);
    }
    catch(...)
    {
        MessageBox(Handle, "Значение скорости указано не корректно.", "Внимание!",
                   MB_OK|MB_ICONWARNING);
        DecimalSeparator = ds;
        return false;
    }
    double oldSpeed = m_VTorrent->GetUploadSpeed();
    if(oldSpeed!=fUpSpeed) m_bDirty = true;
    if(g_Ver) //Ограничиваем Начальную версию скоростью отдачи в 1 кбит/сек.
    {
        if(g_Ver->GetProgrType()==FTTYPE_VER_LIGHT)
        {
            if(fUpSpeed > FT_VIRT_UPLOAD_SPEED)
            {
                fUpSpeed = FT_VIRT_UPLOAD_SPEED;
                AnsiString str = "Вы используете Начальную версию FreeTorrents.\n"
                                   "В этой версии максимально допустимая скорость раздачи виртуального торрента = "
                                   +AnsiString(FT_VIRT_UPLOAD_SPEED)+" Кбит/сек.\n"
                                   "Если Вам необходима большая скорость раздачи виртуальных торрентов, то Вы можете\n"
                                   "приобрести расширенную VIP версию на сайте программы.";
                MessageBox(Handle, str.c_str(), "Внимание!", MB_OK|MB_ICONINFORMATION);
                DecimalSeparator = ds;
                EditUpSpeed->Text = FloatToStr(fUpSpeed);
                return false;
            }
        }
    }
    m_VTorrent->SetUploadSpeed(fUpSpeed);

    //Список трекеров
    UINT cou = ListViewTrackers->Items->Count;
    for(UINT a=0; a < cou; a++)
    {
        TFtTracker *tr = m_VTorrent->GetTracker(a);
        if(tr)
        {
            TListItem *it = ListViewTrackers->Items->Item[a];
            if(it)
            {
                if(tr->m_bUse != it->Checked) m_bDirty = true;
                tr->m_bUse = it->Checked;
            }
        }
    }

    DecimalSeparator = ds;
    return true;
}

//---------------------------------------------------------------------------
bool TFormVirtualTorrent::GetDirty(void)
{
    return m_bDirty;
}
