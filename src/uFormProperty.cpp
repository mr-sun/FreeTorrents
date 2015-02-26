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

#include "uFormProperty.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TFormProperty::TFormProperty(TComponent* Owner)
        : TForm(Owner)
{
}

//---------------------------------------------------------------------------
bool TFormProperty::Execute(CFtOptions *a_Opt)
{
    m_bDirty = false;
    m_Opt = a_Opt;
    Initialize();
    if(this->ShowModal() == mrOk)
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool TFormProperty::Initialize(void)
{
    UpdateNetPage();
    UpdateTrntClntPage();
    UpdateExcTrackersPage();
    UpdateVirtClntPage();        
    UpdateOtherPage();        
    return true;
}
//---------------------------------------------------------------------------
bool TFormProperty::Apply(void)
{
    bool bRes = true;
    bRes &= ApplyNetPage();
    bRes &= ApplyTrntClnt();
    bRes &= ApplyExcTrackers();
    bRes &= ApplyVirtClnt();
    bRes &= ApplyOtherPage();
    return bRes;
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::BtnOkClick(TObject *Sender)
{
    if(!Apply()) return;
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::BtnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}

//---------------------------------------------------------------------------
//Обновить данные на странице сетевых настроек
bool TFormProperty::UpdateNetPage(void)
{
    if(!m_Opt) return false;
    CFtNetOpt *net = m_Opt->GetNetOpt();
    if(!net) return false;

    EditPort->Text = IntToStr( net->GetPort() );
    if(net->GetUseProxy())
        RBUseProxyYes->Checked = true;
    else
        RBUseProxyNo->Checked = true;

    ComboBoxProxyType->Items->Clear();
    ComboBoxProxyType->Clear();
    ComboBoxProxyType->Items->Add("HTTP");
    ComboBoxProxyType->Items->Add("SOCKS4");
    ComboBoxProxyType->Items->Add("SOCKS4a");
    ComboBoxProxyType->Items->Add("SOCKS5");
    switch(net->GetProxyType())
    {
        case PRTYPE_HTTP:
        {
            ComboBoxProxyType->ItemIndex = 0;
            break;
        }
        case PRTYPE_SOCKS4:
        {
            ComboBoxProxyType->ItemIndex = 1;
            break;
        }
        case PRTYPE_SOCKS4a:
        {
            ComboBoxProxyType->ItemIndex = 2;
            break;
        }
        case PRTYPE_SOCKS5:
        {
            ComboBoxProxyType->ItemIndex = 3;
            break;
        }
    }
    EditProxyHost->Text = net->GetProxyHost();
    EditProxyPort->Text = IntToStr( net->GetProxyPort() );

    NP_UpdateUseProxy();
        
    return true;
}
//---------------------------------------------------------------------------
bool TFormProperty::ApplyNetPage(void)
{
    if(!m_Opt) return true;
    CFtNetOpt *net = m_Opt->GetNetOpt();
    if(!net) return true;

    int nPort;
    try
    {
        nPort = StrToInt(EditPort->Text);
    }
    catch(...)
    {
        MessageBox(Handle, "Задано неверное значение порта.", "Внимание!", MB_OK|MB_ICONWARNING);
        return false;
    }
    int oldPort = net->GetPort();
    if(oldPort!=nPort) m_bDirty = true;
    net->SetPort(nPort);

    net->SetUseProxy(RBUseProxyYes->Checked);
    bool oldUseProxy = net->GetUseProxy();
    if(oldUseProxy!=RBUseProxyYes->Checked) m_bDirty = true;

    int oldType = net->GetProxyType();
    if(oldType!=ComboBoxProxyType->ItemIndex) m_bDirty = true;
    switch( ComboBoxProxyType->ItemIndex )
    {
        case 0:
            {
                net->SetProxyType(PRTYPE_HTTP);
                break;
            }
        case 1:
            {
                net->SetProxyType(PRTYPE_SOCKS4);
                break;
            }
        case 2:
            {
                net->SetProxyType(PRTYPE_SOCKS4a);
                break;
            }
        case 3:
            {
                net->SetProxyType(PRTYPE_SOCKS5);
                break;
            }
        default:
            {
                net->SetProxyType(PRTYPE_HTTP);
                break;
            }
    }

    AnsiString sProxyHost = EditProxyHost->Text;
    AnsiString oldPrHost = net->GetProxyHost();
    if(oldPrHost!=sProxyHost) m_bDirty = true;
    net->SetProxyHost( sProxyHost );

    int nProxyPort;
    try
    {
        nProxyPort = StrToInt(EditProxyPort->Text);
    }
    catch(...)
    {
        MessageBox(Handle, "Задано неверное значение порта прокси-сервера.", "Внимание!", MB_OK|MB_ICONWARNING);
        return false;
    }
    int oldProxyPort = net->GetProxyPort();
    if(oldProxyPort!=nProxyPort) m_bDirty = true;
    net->SetProxyPort(nProxyPort);

    return true;
}
//---------------------------------------------------------------------------
bool TFormProperty::NP_UpdateUseProxy(void)
{
    if( RBUseProxyNo->Checked )
    {
        ComboBoxProxyType->Enabled = false;
        EditProxyHost->Enabled = false;
        EditProxyPort->Enabled = false;
        ComboBoxProxyType->Color = clBtnFace;
        EditProxyHost->Color = clBtnFace;
        EditProxyPort->Color = clBtnFace;
    }
    else
    {
        ComboBoxProxyType->Enabled = true;
        EditProxyHost->Enabled = true;
        EditProxyPort->Enabled = true;
        ComboBoxProxyType->Color = clWindow;
        EditProxyHost->Color = clWindow;
        EditProxyPort->Color = clWindow;
    }
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::RBUseProxyNoClick(TObject *Sender)
{
    NP_UpdateUseProxy();
}

//---------------------------------------------------------------------------
//Обновить данные на странице торрент-клиента
bool TFormProperty::UpdateTrntClntPage(void)
{
    if(!m_Opt) return false;
    CFtTorrentCltOpt *clnt = m_Opt->GetTorrentCltOpt();
    if(!clnt) return false;

    EditTrntClnt->Text = clnt->GetTrntClientFileName();
    CheckBoxAutoStart->Checked = clnt->GetAutoStart();
    CheckBoxAutoClose->Checked = clnt->GetAutoClose();

    TC_UpdatePathsList();

    return true;
}
//---------------------------------------------------------------------------
//Обновляем список путей из опций
bool TFormProperty::TC_UpdatePathsList(void)
{
    if(!m_Opt) return false;
    CFtTorrentCltOpt *clnt = m_Opt->GetTorrentCltOpt();
    if(!clnt) return false;

    ListViewPaths->Items->Clear();
    ListViewPaths->Clear();    

    UINT cou = clnt->GetPathsCount();
    for(UINT a=0; a < cou; a++)
    {
        AnsiString path = clnt->GetPath(a);
        TListItem *it = ListViewPaths->Items->Add();
        if(!it) continue;
        it->Caption = path;
    }
    TC_UpdatePathsBtns();
    return true;
}
//---------------------------------------------------------------------------
void TFormProperty::TC_UpdatePathsBtns(void)
{
    if(ListViewPaths->ItemIndex >= 0)
    {
        BtnEditPath->Enabled = true;
        BtnDelPath->Enabled = true;
    }
    else
    {
        BtnEditPath->Enabled = false;
        BtnDelPath->Enabled = false;
    }
}
//---------------------------------------------------------------------------
bool TFormProperty::ApplyTrntClnt(void)
{
    if(!m_Opt) return false;
    CFtTorrentCltOpt *clnt = m_Opt->GetTorrentCltOpt();
    if(!clnt) return false;

    AnsiString sClnt = EditTrntClnt->Text;
    AnsiString oldClnt = clnt->GetTrntClientFileName();
    if(oldClnt!=sClnt) m_bDirty = true;
    clnt->SetTrntClientFileName( sClnt );

    bool oldAutoStart = clnt->GetAutoStart();
    if(oldAutoStart!=CheckBoxAutoStart->Checked) m_bDirty = true;
    clnt->SetAutoStart(CheckBoxAutoStart->Checked);

    bool oldAutoClose = clnt->GetAutoClose();
    if(oldAutoClose!=CheckBoxAutoClose->Checked) m_bDirty = true;
    clnt->SetAutoClose(CheckBoxAutoClose->Checked);

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::BtnBrowseClick(TObject *Sender)
{
    AnsiString InitialDir = ExtractFilePath(EditTrntClnt->Text);
    OpenDialog1->InitialDir = InitialDir;
    if(OpenDialog1->Execute())
    {
        EditTrntClnt->Text = OpenDialog1->FileName;
    }
}
//---------------------------------------------------------------------------
//Удалить путь из списка
void __fastcall TFormProperty::BtnDelPathClick(TObject *Sender)
{
    if(!m_Opt) return;
    CFtTorrentCltOpt *clnt = m_Opt->GetTorrentCltOpt();
    if(!clnt) return;
    if(ListViewPaths->ItemIndex >= 0)
    {
        clnt->DeletePath( ListViewPaths->ItemIndex );
        m_bDirty = true;        
        TC_UpdatePathsList();
    }
}
//---------------------------------------------------------------------------
//Добавить путь в список
void __fastcall TFormProperty::BtnAddPathClick(TObject *Sender)
{
    if(!m_Opt) return;
    CFtTorrentCltOpt *clnt = m_Opt->GetTorrentCltOpt();
    if(!clnt) return;

    TFormEditPath *frm = new TFormEditPath(this);
    if(frm)
    {
        AnsiString str = "./";
        if(frm->Execute(str))
        {
            clnt->AddPath(str);
            m_bDirty = true;            
            TC_UpdatePathsList();
        }
        delete frm;
    }
}
//---------------------------------------------------------------------------
//Редактировать путь в списке
void __fastcall TFormProperty::BtnEditPathClick(TObject *Sender)
{
    if(!m_Opt) return;
    CFtTorrentCltOpt *clnt = m_Opt->GetTorrentCltOpt();
    if(!clnt) return;
    if(ListViewPaths->ItemIndex >= 0)
    {
        AnsiString str = clnt->GetPath( ListViewPaths->ItemIndex );
        TFormEditPath *frm = new TFormEditPath(this);
        if(frm)
        {
            if(frm->Execute(str, FT_FTYPE_PATH_EDIT))
            {
                clnt->EditPath(ListViewPaths->ItemIndex, str);
                m_bDirty = true;                
                TC_UpdatePathsList();
            }
            delete frm;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::ListViewPathsSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    TC_UpdatePathsBtns();
}

//---------------------------------------------------------------------------
//Обновить данные на странице виртуального клиента
bool TFormProperty::UpdateVirtClntPage(void)
{
    if(!m_Opt)return false;
    CFtVirtTorrentsOpt *opt = m_Opt->GetVirtTorrentsOpt();
    if(!opt) return false;

    //Обновляем список клиентов
    ComboBoxClts->Items->Clear();
    ComboBoxClts->Clear();
    UINT cou = opt->GetCltInfoCount();
    for(UINT a=0; a < cou; a++)
    {
        TFtClientInfo *info = opt->GetCltInfo(a);
        if(info)
        {
            ComboBoxClts->Items->Add(info->m_Name);
        }
    }
    ComboBoxClts->ItemIndex = opt->GetCurrClt();

    //Порт
    EditVirtPort->Text = IntToStr(opt->GetVirtPort());

    return true;
}
//---------------------------------------------------------------------------
bool TFormProperty::ApplyVirtClnt(void)
{
    if(!m_Opt)return false;
    CFtVirtTorrentsOpt *opt = m_Opt->GetVirtTorrentsOpt();
    if(!opt) return false;

    int oldIndex = opt->GetCurrClt();
    if(oldIndex != ComboBoxClts->ItemIndex) m_bDirty = true;
    opt->SetCurrClt( ComboBoxClts->ItemIndex );

    AnsiString sPort = EditVirtPort->Text;
    UINT nPort;
    try
    {
        nPort = StrToInt(sPort);
    }
    catch(...)
    {
        //Неверное значение порта
        AnsiString str = "Неверно указано значение порта у виртуального клиента.";
        MessageBox(Handle, str.c_str(), "Внимание!", MB_OK|MB_ICONWARNING);
        return false;
    }
    UINT oldPort = opt->GetVirtPort();
    if(oldPort!=nPort) m_bDirty = true;
    opt->SetVirtPort(nPort);
    
    return true;
}

//---------------------------------------------------------------------------
//Обновить данные на странице Трекеров
bool TFormProperty::UpdateExcTrackersPage(void)
{
    if(!m_Opt) return false;
    CFtTorrentsOpt *torr = m_Opt->GetTorrentsOpt();
    if(!torr) return false;

    ListViewExcTrackers->Items->Clear();
    ListViewExcTrackers->Clear();
    UINT cou = torr->GetExcTrackersCount();
    for(UINT a=0; a < cou; a++)
    {
        AnsiString host = torr->GetExcTracker(a);
        if(host == "") continue;
        TListItem *it = ListViewExcTrackers->Items->Add();
        if(it)
        {
            it->Caption = host;
        }
    }
    TC_UpdateExcTracksBtns();
    return true;
}
//---------------------------------------------------------------------------
bool TFormProperty::ApplyExcTrackers(void)
{
    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::BtnAddExcClick(TObject *Sender)
{
    if(!m_Opt) return;
    CFtTorrentsOpt *torr = m_Opt->GetTorrentsOpt();
    if(!torr) return;

    TFormEditStr *frm = new TFormEditStr(this);
    if(frm)
    {
        AnsiString str = "";
        if(frm->Execute(str))
        {
            torr->AddExcTracker(str);
            m_bDirty = true;
            UpdateExcTrackersPage();
        }
        delete frm;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::BtnEditExcClick(TObject *Sender)
{
    if(!m_Opt) return;
    CFtTorrentsOpt *torr = m_Opt->GetTorrentsOpt();
    if(!torr) return;

    if(ListViewExcTrackers->ItemIndex >= 0)
    {
        AnsiString str = torr->GetExcTracker( ListViewExcTrackers->ItemIndex );
        TFormEditStr *frm = new TFormEditStr(this);
        if(frm)
        {
            if(frm->Execute(str))
            {
                torr->EditExcTracker(ListViewExcTrackers->ItemIndex, str);
                m_bDirty = true;
                UpdateExcTrackersPage();
            }
            delete frm;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::BtnDelExcClick(TObject *Sender)
{
    if(!m_Opt) return;
    CFtTorrentsOpt *torr = m_Opt->GetTorrentsOpt();
    if(!torr) return;
    if(ListViewExcTrackers->ItemIndex >= 0)
    {
        torr->DeleteExcTracker( ListViewExcTrackers->ItemIndex );
        m_bDirty = true;
        UpdateExcTrackersPage();
    }
}
//---------------------------------------------------------------------------
void TFormProperty::TC_UpdateExcTracksBtns(void)
{
    if(ListViewExcTrackers->ItemIndex >= 0)
    {
        BtnEditExc->Enabled = true;
        BtnDelExc->Enabled = true;
    }
    else
    {
        BtnEditExc->Enabled = false;
        BtnDelExc->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::ListViewExcTrackersSelectItem(
      TObject *Sender, TListItem *Item, bool Selected)
{
    TC_UpdateExcTracksBtns();
}

//---------------------------------------------------------------------------
//Обновить данные на странице "Общее"
bool TFormProperty::UpdateOtherPage(void)
{
    if(!m_Opt) return false;
    CFtOtherOpt *other = m_Opt->GetOtherOpt();
    if(!other) return false;

    CheckBoxStartWithWin->Checked = other->GetStartWithWin();
    CheckBoxStartMinimized->Checked = other->GetStartMinimized();
    CheckBoxCloseToTray->Checked = other->GetCloseToTray();
    CheckBoxSearchUpdate->Checked = other->GetSearchUpdate();
    CheckBoxFTUpdateAsk->Checked = other->GetFTUpdateAsk();

    CheckBoxSearchUpdateClick(NULL);

    return true;
}
//---------------------------------------------------------------------------
bool TFormProperty::ApplyOtherPage(void)
{
    if(!m_Opt) return false;
    CFtOtherOpt *other = m_Opt->GetOtherOpt();
    if(!other) return false;

    bool oldStartWW = other->GetStartWithWin();
    if(oldStartWW!=CheckBoxStartWithWin->Checked) m_bDirty = true;
    other->SetStartWithWin( CheckBoxStartWithWin->Checked );

    bool oldStartMinimized = other->GetStartMinimized();
    if(oldStartMinimized!=CheckBoxStartMinimized->Checked) m_bDirty = true;
    other->SetStartMinimized( CheckBoxStartMinimized->Checked );

    bool oldCloseToTray = other->GetCloseToTray();
    if(oldCloseToTray!=CheckBoxCloseToTray->Checked) m_bDirty = true;
    other->SetCloseToTray( CheckBoxCloseToTray->Checked );

    bool oldSearchUpdate = other->GetSearchUpdate();
    if(oldSearchUpdate!=CheckBoxSearchUpdate->Checked) m_bDirty = true;
    other->SetSearchUpdate( CheckBoxSearchUpdate->Checked );

    bool oldUpdateAsk = other->GetFTUpdateAsk();
    if(oldUpdateAsk!=CheckBoxFTUpdateAsk->Checked) m_bDirty = true;
    other->SetFTUpdateAsk( CheckBoxFTUpdateAsk->Checked );

    return true;
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::CheckBoxSearchUpdateClick(TObject *Sender)
{
    if(CheckBoxSearchUpdate->Checked)
    {
        CheckBoxFTUpdateAsk->Enabled = true;
    }
    else
    {
        CheckBoxFTUpdateAsk->Enabled = false;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormProperty::EditPortKeyPress(TObject *Sender, char &Key)
{
    if(Key == VK_RETURN)
      BtnOkClick(Sender);
}

//---------------------------------------------------------------------------
bool TFormProperty::GetDirty(void)
{
    return m_bDirty;
}

//---------------------------------------------------------------------------



