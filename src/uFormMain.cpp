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

#include "uFtFunctions.h"
#include "uFormMain.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
        : TForm(Owner)
{
    CreateTrayIcon();

    m_bServerRunning = false;
    m_isRepaint = true;
    m_CanClose = false;
    Application->OnMessage = AppMessage;
    m_Core.Load();
    CreateGroups();
    CreateStatusBar();

    CreateOnNotify();

    m_Core.InitializeOncePreInit();
    m_Core.Init();
    m_Core.InitializeOnceAfterInit();

    CFtOptions *opt = m_Core.GetOptions();
    if(!opt) return;
    CFtOtherOpt *other = opt->GetOtherOpt();
    if(!other) return;
    if(other->GetStartMinimized())
    {
        this->WindowState = wsMinimized;
    }
    UpdateServerState(true);

    m_TrayIcon->Visible = true;
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormDestroy(TObject *Sender)
{
    m_Core.UnInit();
    m_Core.UnInitializeOnce();
}

//---------------------------------------------------------------------------
//Обновление списков на форме (перерисовка)
void __fastcall TFormMain::TimerUpdateTimer(TObject *Sender)
{
    MinimizedOnStart();//Срабатывает однажды при старте

    g_CSTorrents->Enter();
    UpdateTorrentsList();
    UpdateTrayHint();
    UpdateStatusBar();
    g_CSTorrents->Leave();

    UpdateServerState(); //Обновляем отображение состояния сервера
}

//---------------------------------------------------------------------------
void TFormMain::MinimizedOnStart(void)
{
    if(TimerUpdate->Tag < 1)
    {
        TimerUpdate->Tag++;
        CFtOptions *opt = m_Core.GetOptions();
        if(!opt) return;
        CFtOtherOpt *other = opt->GetOtherOpt();
        if(!other) return;
        if(other->GetStartMinimized())
        {
            m_TrayIcon->Minimize();
        }
    }
}

//---------------------------------------------------------------------------
//Создание иконки в трее
void TFormMain::CreateTrayIcon(void)
{
    m_TrayIcon = new CFtTrayIcon(this);
    m_TrayIcon->Animate = false;
    m_TrayIcon->AnimateInterval = 1000;
    m_TrayIcon->Hide = true;
    m_TrayIcon->Icons = ImageList1;    
    m_TrayIcon->IconIndex = 0;
    m_TrayIcon->PopupMenu = PopupMenuTray;
    m_TrayIcon->PopupMenuOn = imRightClickUp;
    m_TrayIcon->RestoreOn = imDoubleClick;
    //m_TrayIcon->Visible = false;
}

//---------------------------------------------------------------------------
void TFormMain::CreateStatusBar(void)
{
    StatusBar->Panels->Clear();

    TStatusPanel *pan = StatusBar->Panels->Add();
    pan->Width = 200;
    pan->Text = "";

    pan = StatusBar->Panels->Add();
    pan->Width = 200;
    pan->Text = "";

    pan = StatusBar->Panels->Add();
    pan->Width = 200;
    pan->Text = "";

    pan = StatusBar->Panels->Add();
    pan->Width = 2250;
    pan->Text = "";
}

//---------------------------------------------------------------------------
void TFormMain::UpdateStatusBar(void)
{
    AnsiString sUpl = "";
    AnsiString sDown = "";
    CFtTorrentsMgr *mgr = m_Core.GetTorrnetsMgr();
    if(mgr)
    {
        double fUp=0, fUpNew=0, fDown=0, fDownNew=0;
        mgr->GetAllStatistic(fUp, fUpNew, fDown, fDownNew);
        CFtVirtualTorrentMgr *vmgr = m_Core.GetVirtTorrnetsMgr();
        if(vmgr)
        {
            fUpNew += vmgr->GetAllUploadedSize();
        }
        sDown = "Скачано: " + ConvertBytesS(fDown) + " / " + ConvertBytesS(fDownNew);
        sUpl = "Отдано: " + ConvertBytesS(fUp) + " / " + ConvertBytesS(fUpNew);
    }

    StatusBar->Panels->Items[1]->Text = sUpl;
    StatusBar->Panels->Items[2]->Text = sDown;    

    StatusBar->Invalidate();
    StatusBar->Repaint();
    StatusBar->Refresh();
}

//---------------------------------------------------------------------------
//Обновляем отображение состояния сервера
void TFormMain::UpdateServerState(bool a_bForce)
{
    CFtServer *serv = m_Core.GetServer();
    if(!serv) return;

    bool bRuning = serv->IsRunning();
    if(bRuning == m_bServerRunning && !a_bForce) return;

    AnsiString str = AnsiString("FreeTorrents ") + m_Core.GetVer(FTVT_MAJ_MIN_REL);
    if(bRuning)
    {
        str += " :: Работает";
        m_TrayIcon->IconIndex = 0;

        PopupMenuTray->Items->Items[7]->Caption = "Остановить";
        PopupMenuTray->Items->Items[7]->OnClick = ToolBtnStopClick;

        NStartStop->Caption = "Остановить";
        NStartStop->OnClick = ToolBtnStopClick;

        ToolBtnStart->Enabled = false;
        ToolBtnStop->Enabled = true;
    }
    else
    {
        str += " :: Остановлен";
        m_TrayIcon->IconIndex = 6;

        PopupMenuTray->Items->Items[7]->Caption = "Запустить";
        PopupMenuTray->Items->Items[7]->OnClick = ToolBtnStartClick;

        NStartStop->Caption = "Запустить";
        NStartStop->OnClick = ToolBtnStartClick;

        ToolBtnStart->Enabled = true;
        ToolBtnStop->Enabled = false;
    }
    this->Caption = str;

    m_bServerRunning = serv->IsRunning();
}

//---------------------------------------------------------------------------
void TFormMain::CreateGroups(void)
{
    LVGroups->Items->Clear();
    LVGroups->Clear();

    TListItem *it = LVGroups->Items->Add();
    it->Caption = "Все";
    it->SubItems->Add("(0)");
    it->ImageIndex = 2;

    it = LVGroups->Items->Add();
    it->Caption = "Изменяемые";
    it->SubItems->Add("(0)");
    it->ImageIndex = 4;

    it = LVGroups->Items->Add();
    it->Caption = "Не изменяемые";
    it->SubItems->Add("(0)");
    it->ImageIndex = 3;

    it = LVGroups->Items->Add();
    it->Caption = "Виртуальные";
    it->SubItems->Add("(0)");
    it->ImageIndex = 9;    

    m_CurrentGroup = FTVIEW_CUR_GROUP_ALL;
    LVGroups->ItemIndex = m_CurrentGroup;
}

//---------------------------------------------------------------------------
UINT GetCountInLVGroupItem(AnsiString a_str)
{
    int cou;
    a_str = a_str.SubString(2, a_str.Length()-2);
    try
    {
        cou = StrToInt(a_str);
    }
    catch(...)
    {
        cou = 0;
    }
    return cou;
}

//---------------------------------------------------------------------------
//Обновление листа торрентов
void TFormMain::UpdateTorrentsList(void)
{
    CFtTorrentsMgr *tmgr = m_Core.GetTorrnetsMgr();
    CFtVirtualTorrentMgr *vmgr = m_Core.GetVirtTorrnetsMgr();
    if(!tmgr) return;
    if(!vmgr) return;
    bool isRepaint = m_isRepaint;
    bool isRepaintNew = false;

    //Подсчитываем кол-во для каждого типа
    UINT tcou = tmgr->GetCount();
    UINT act_rules_cou = 0;
    UINT noact_rules_cou = 0;
    for(UINT a=0; a < tcou; a++)
    {
        CFtTorrent *tr = tmgr->GetTorrent(a);
        if(tr)
        {
            if(tr->IsActiveRules())
                act_rules_cou++;
            else
                noact_rules_cou++;
        }
    }
    UINT virtual_cou = 0;
    UINT vcou = vmgr->GetCount();
    for(UINT a=0; a < vcou; a++)
    {
        CFtVirtualTorrent *vt = vmgr->GetVTorrent(a);
        if(vt)
        {
            if(vt->GetUsed())
            {
                virtual_cou++;
            }
        }
    }

    UINT oldcou = GetCountInLVGroupItem(LVGroups->Items->Item[0]->SubItems->Strings[0]);
    if(oldcou != (tcou + virtual_cou))
        LVGroups->Items->Item[0]->SubItems->Strings[0] = "("+AnsiString(tcou + virtual_cou)+")";

    oldcou = GetCountInLVGroupItem(LVGroups->Items->Item[1]->SubItems->Strings[0]);
    if(oldcou != act_rules_cou)
        LVGroups->Items->Item[1]->SubItems->Strings[0] = "("+AnsiString(act_rules_cou)+")";

    oldcou = GetCountInLVGroupItem(LVGroups->Items->Item[2]->SubItems->Strings[0]);
    if(oldcou != noact_rules_cou)
        LVGroups->Items->Item[2]->SubItems->Strings[0] = "("+AnsiString(noact_rules_cou)+")";

    oldcou = GetCountInLVGroupItem(LVGroups->Items->Item[3]->SubItems->Strings[0]);
    if(oldcou != virtual_cou)
        LVGroups->Items->Item[3]->SubItems->Strings[0] = "("+AnsiString(virtual_cou)+")";

    //необходимо обновить список?
    if((m_CurrentGroup==FTVIEW_CUR_GROUP_ALL) && ( (tcou+virtual_cou) != (UINT)LVTorrents->Items->Count))
    {
        isRepaintNew = true;
    }
    if((m_CurrentGroup==FTVIEW_CUR_GROUP_ACTRUL) && (act_rules_cou != (UINT)LVTorrents->Items->Count))
    {
        isRepaintNew = true;
    }
    if((m_CurrentGroup==FTVIEW_CUR_GROUP_NOACTRUL) && (noact_rules_cou != (UINT)LVTorrents->Items->Count))
    {
        isRepaintNew = true;
    }
    if((m_CurrentGroup==FTVIEW_CUR_GROUP_VIRTUAL) && (virtual_cou != (UINT)LVTorrents->Items->Count))
    {
        isRepaintNew = true;
    }

    if(isRepaintNew || isRepaint)
    {
        //Прежде чем очистить список, освобождаем память от объектов  CFtItemData
        for(int b=0; b < LVTorrents->Items->Count; b++)
        {
            CFtItemData *data = (CFtItemData*)LVTorrents->Items->Item[b]->Data;
            if(data) delete data;
        }
        LVTorrents->Items->Clear();
        LVTorrents->Clear();
    }

    int useCou = 0;
    //Перебираем все торренты
    for(UINT a=0; a < tcou; a++)
    {
        CFtTorrent *tr = tmgr->GetTorrent(a);
        if(!tr) continue;

        bool bUsed = false;
        switch(m_CurrentGroup)
        {
            case FTVIEW_CUR_GROUP_ALL:
                {
                    bUsed = true;
                    break;
                }
            case FTVIEW_CUR_GROUP_ACTRUL:
                {
                    if(tr->IsActiveRules())
                        bUsed = true;
                    break;
                }
            case FTVIEW_CUR_GROUP_NOACTRUL:
                {
                    if(!tr->IsActiveRules())
                        bUsed = true;
                    break;
                }
        }

        //Торрент нужно отображать в текущем списке
        if(bUsed)
        {
            //если нужно пересоздать список, то пересоздаем итем
            if(isRepaintNew || isRepaint)
            {
                TListItem *it = LVTorrents->Items->Add();
                it->Data = 0;
                AnsiString name = tr->GetName();
                if(name=="")
                {
                    name = tr->GetHash20();
                }
                it->Caption = name;
                it->SubItems->Add( tr->GetTracker() );

                AnsiString strDown = ConvertBytesS(tr->GetLastDownload()) + " / "
                                     + ConvertBytesS(tr->GetLastNewDownload());
                AnsiString strUp = ConvertBytesS(tr->GetLastUpload()) + " / "
                                   + ConvertBytesS(tr->GetLastNewUpload());
                AnsiString strLeft = ConvertBytesS(tr->GetLastLeft()) + " / "
                                   + ConvertBytesS(tr->GetLastNewLeft());

                it->SubItems->Add( strDown );
                it->SubItems->Add( strUp );

                AnsiString rname = "нет правила";
                CFtRule *rule = tr->GetRule();
                if(rule)
                {
                    if(!tr->IsActiveRules())
                    {
                        rname = rule->GetName();
                    }
                    else
                    {
                        rname = "";
                        if(rule->GetUseKUpload())
                        {
                            rname += "Up: x"+FloatToStr(rule->GetKUpload())+" ";
                        }
                        if(rule->GetUseKDownload())
                        {
                            rname += "Down: "+FloatToStr(rule->GetPercentDownload())+"% ";
                        }
                        rname += "[" + rule->GetName() + "]";
                    }
                }
                it->SubItems->Add( rname );

                it->SubItems->Add( strLeft );

                CFtItemData *ItemData = new CFtItemData;
                it->Data = ItemData;
                ItemData->m_nIndex = a;
                if(tr->IsActiveRules())
                {
                    ItemData->m_Group = FTVIEW_GROUP_ACTRUL;
                }
                else
                {
                    ItemData->m_Group = FTVIEW_GROUP_NOACTRUL;
                }

                if(tr->GetState() == FTTS_STOPPED)
                {
                    it->ImageIndex = 0;
                }
                else
                {
                    it->ImageIndex = 1;
                    if(tr->GetLastLeft() > 0)
                    {
                        it->ImageIndex = 2;
                    }
                }

            }
            else
            {
                //Если обновились данные, то можно перерисовать их
                if(tr->IsNewData())
                {
                    //Не нужно перерисовывать все
                    //Только обновляем поля алоад, даунлоад и т.п.
                    TListItem *it = LVTorrents->Items->Item[useCou];
                    if(it)
                    {
                        AnsiString strDown = ConvertBytesS(tr->GetLastDownload()) + " / "
                                             + ConvertBytesS(tr->GetLastNewDownload());
                        AnsiString strUp = ConvertBytesS(tr->GetLastUpload()) + " / "
                                           + ConvertBytesS(tr->GetLastNewUpload());
                        AnsiString strLeft = ConvertBytesS(tr->GetLastLeft()) + " / "
                                             + ConvertBytesS(tr->GetLastNewLeft());

                        it->SubItems->Strings[1] = strDown;
                        it->SubItems->Strings[2] = strUp;
                        it->SubItems->Strings[4] = strLeft;

                        if(tr->GetState() == FTTS_STOPPED)
                        {
                            it->ImageIndex = 0;
                        }
                        else
                        {
                            it->ImageIndex = 1;
                            if(tr->GetLastLeft() > 0)
                            {
                                it->ImageIndex = 2;
                            }
                        }
                    }
                }
            }
            useCou++;
        }
    }

    //Отображаем виртуальные торренты если необходимо
    for(UINT a=0; a < vcou; a++)
    {
        CFtVirtualTorrent *vt = vmgr->GetVTorrent(a);
        if(!vt) continue;
        if(!vt->GetUsed()) continue;

        bool bUsed = false;
        switch(m_CurrentGroup)
        {
            case FTVIEW_CUR_GROUP_ALL:
                {
                    bUsed = true;
                    break;
                }
            case FTVIEW_CUR_GROUP_VIRTUAL:
                {
                    bUsed = true;
                    break;
                }
        }

        //Торрент нужно отображать в текущем списке
        if(bUsed)
        {
            //если нужно пересоздать список, то пересоздаем итем
            if(isRepaintNew || isRepaint)
            {
                TListItem *it = LVTorrents->Items->Add();
                AnsiString name = ExtractFileName( vt->GetFileName() );
                it->Caption = name;
                it->ImageIndex = 3;
                
                CFtItemData *ItemData = new CFtItemData;
                ItemData->m_Group = FTVIEW_GROUP_VIRTUAL;
                ItemData->m_nIndex = a;
                it->Data = ItemData;

                //Трекер
                AnsiString strack = "";
                TFtTracker *tra = vt->GetFirstActTracker();
                if(tra)
                {
                    strack = tra->m_sTracker;
                }
                it->SubItems->Add( strack );

                //Скачано
                AnsiString strDown = "-- / --";
                it->SubItems->Add( strDown );

                //Отдано
                AnsiString strUp = ConvertBytesS( 0 ) + " / " + ConvertBytesS( vt->GetUpload() );
                it->SubItems->Add( strUp );

                //Правило
                it->SubItems->Add( "Вирт: " + FloatToStr(vt->GetUploadSpeed()) + " Кбит/сек" ); 

                //Требуется
                AnsiString strLeft = "-- / --";
                it->SubItems->Add( strLeft );
            }
            else
            {
                //Если обновились данные, то можно перерисовать их
                if(vt->IsNewData())
                {
                    //Не нужно перерисовывать все
                    //Только обновляем поля алоад, даунлоад и т.п.
                    TListItem *it = LVTorrents->Items->Item[useCou];
                    if(it)
                    {
                        //Отдано
                        AnsiString strUp = ConvertBytesS( 0 ) + " / "
                                             + ConvertBytesS( vt->GetUpload() );
                        it->SubItems->Strings[2] = strUp;
                    }
                }
            }
            useCou++;
        }
    }

    if(isRepaint) m_isRepaint = false;
}

//---------------------------------------------------------------------------
void TFormMain::UpdateTrayHint(void)
{
    AnsiString TrayHint = "FreeTorrents " + m_Core.GetVer(FTVT_MAJ_MIN_REL) + "\n";
    CFtTorrentsMgr *mgr = m_Core.GetTorrnetsMgr();
    if(mgr)
    {
        double fUp=0, fUpNew=0, fDown=0, fDownNew=0;
        mgr->GetAllStatistic(fUp, fUpNew, fDown, fDownNew);
        CFtVirtualTorrentMgr *vmgr = m_Core.GetVirtTorrnetsMgr();
        if(vmgr)
        {
            fUpNew += vmgr->GetAllUploadedSize();
        }
        TrayHint += "Скачано: " + ConvertBytesS(fDown) + " / " + ConvertBytesS(fDownNew) + "\n";
        TrayHint += "Отдано: " + ConvertBytesS(fUp) + " / " + ConvertBytesS(fUpNew);
    }
    m_TrayIcon->SetBigHint(TrayHint);
}

//---------------------------------------------------------------------------
//Вызов настроек
void __fastcall TFormMain::NPropertyClick(TObject *Sender)
{
    TFormProperty *frm = new TFormProperty(this);
    if(frm)
    {
        CFtOptions *Opt = m_Core.GetOptions();
        g_CSOptions->Enter();
        CFtOptions tmp_Opt = *Opt;
        g_CSOptions->Leave();
        if( frm->Execute( &tmp_Opt ))
        {
            if(frm->GetDirty())
            {
                m_Core.UnInit();
                g_CSOptions->Enter();
                *Opt = tmp_Opt;
                g_CSOptions->Leave();
                m_Core.Init();
                m_isRepaint = true;
            }
        }
        delete frm;
    }
}

//---------------------------------------------------------------------------
//Вызов редактора правил
void __fastcall TFormMain::NRulesEditClick(TObject *Sender)
{
    TFormEditRules *frm = new TFormEditRules(this);
    if(frm)
    {
        CFtRulesMgr *RulesMgr = m_Core.GetRulesMgr();
        CFtRulesMgr tmpRulesMgr = *RulesMgr;
        if( frm->Execute( &tmpRulesMgr ) )
        {
            if(frm->GetDirty())
            {
                //Проверяем имеется ли хоть один активный торрент?
                bool bAct = false;
                if(m_Core.GetTorrnetsMgr()->GetCount() > 0)
                    bAct = true;

                bool needReInit = false;
                if(bAct)
                {
                    //Предупреждаем о том, что нужно остановить торрент-клиент (или выгрузить его).
                    AnsiString str = "Внимание!\n"
                                     "Прежде чем применить правила, Вам необходимо остановить все раздачи\n"
                                     "в торрент-клиенте (uTorrent, BitCommet и т.п.) или выйти из программы.\n\n"

                                     "1. Если Вы вышли из программы торрент-клиента и готовы применить\n"
                                     "измененные правила FreeTorrents – нажмите «OK».\n\n"
                                     "2. Если Вы не желаете применять измененные правила – нажмите «Отмена».";
                    if(MessageBox(Handle, str.c_str(), "Внимание!", MB_OKCANCEL|MB_ICONWARNING) == IDOK)
                    {
                        needReInit = true;
                    }
                }
                else
                {
                    needReInit = true;
                }
                
                //Нужна переинициализация торрентов.
                if(needReInit)
                {
                    m_Core.UnInit(FT_IL_TORRENTS);
                    *RulesMgr = tmpRulesMgr;
                    m_Core.Init(FT_IL_TORRENTS);
                    m_isRepaint = true;
                }
            }
        }
        delete frm;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::NExitClick(TObject *Sender)
{
    m_CanClose = true;
    Close();
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::LVGroupsChange(TObject *Sender, TListItem *Item,
      TItemChange Change)
{
    if(LVGroups->ItemIndex >= 0)
    {
        switch(LVGroups->ItemIndex)
        {
            case 0:
            {
                m_CurrentGroup = FTVIEW_CUR_GROUP_ALL;
                m_isRepaint = true;
                break;
            }
            case 1:
            {
                m_CurrentGroup = FTVIEW_CUR_GROUP_ACTRUL;
                m_isRepaint = true;
                break;
            }
            case 2:
            {
                m_CurrentGroup = FTVIEW_CUR_GROUP_NOACTRUL;
                m_isRepaint = true;
                break;
            }
            case 3:
            {
                m_CurrentGroup = FTVIEW_CUR_GROUP_VIRTUAL;
                m_isRepaint = true;
                break;
            }
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    m_Core.Save();
    CanClose = true;
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::NShowClick(TObject *Sender)
{
    m_TrayIcon->Restore();    
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::AppMessage(tagMSG &Msg, bool &Handled)
{
    if(Msg.hwnd == this->Handle) //Только для главного окна приложения
    if (Msg.message == 161 && Msg.wParam == 20) //WM_SYSCOMMAND (CS_CLOSE) ???
    {
        g_CSOptions->Enter();
        CFtOptions *opt = m_Core.GetOptions();
        if(opt)
        {
            CFtOtherOpt *other = opt->GetOtherOpt();
            if(other)
            {
                bool bCtt = other->GetCloseToTray();
                if(bCtt)
                {
                    m_TrayIcon->Minimize();
                    Handled = true;
                }
            }
        }
        g_CSOptions->Leave();
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::LVTorrentsCustomDrawItem(
      TCustomListView *Sender, TListItem *Item, TCustomDrawState State,
      bool &DefaultDraw)
{
    int tmp = Item->Index/2;
    if( Item->Index == tmp*2 )
    {
        //Четные
        LVTorrents->Canvas->Brush->Color = (TColor)0xF7F7F7;
    }
    else
    {
        //Не четные
        LVTorrents->Canvas->Brush->Color = (TColor)0xFFFFFF;
    }

    if(Item->Data)
    {
        CFtItemData *data = (CFtItemData *)Item->Data;
        if(data->m_Group == FTVIEW_GROUP_ACTRUL)
        {
            LVTorrents->Canvas->Font->Color = clGreen;
        }
        if(data->m_Group == FTVIEW_GROUP_VIRTUAL)
        {
            LVTorrents->Canvas->Font->Color = clBlue;
        }
    }
    else
    {
        LVTorrents->Canvas->Font->Color = clRed;
    }

}

//---------------------------------------------------------------------------
//Перекрашиваем левый список...
void __fastcall TFormMain::LVGroupsCustomDrawItem(TCustomListView *Sender,
      TListItem *Item, TCustomDrawState State, bool &DefaultDraw)
{
    if(Item->Index == m_CurrentGroup)
    {
        //Текущий раздел
        LVGroups->Canvas->Brush->Color = (TColor)0xE5E5E5;
    }
    else
    {
        LVGroups->Canvas->Brush->Color = (TColor)0xFFFFFF;
    }

    if(Item->Index == FTVIEW_CUR_GROUP_ACTRUL)
    {
        LVGroups->Canvas->Font->Color = clGreen;
    }
    if(Item->Index == FTVIEW_CUR_GROUP_VIRTUAL)
    {
        LVGroups->Canvas->Font->Color = clBlue;
    }

}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ToolBtnStopClick(TObject *Sender)
{
    CFtServer *serv = m_Core.GetServer();
    if(!serv) return;
    serv->Stop();
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::ToolBtnStartClick(TObject *Sender)
{
    CFtServer *serv = m_Core.GetServer();
    if(!serv) return;
    serv->Start();
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::ToolBtnUpdateClick(TObject *Sender)
{
    m_Core.StartSearchUpdate();
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::NVirtualMgrClick(TObject *Sender)
{
    TFormVirtualMgr *frm = new TFormVirtualMgr(this);
    if(frm)
    {
        CFtVirtualTorrentMgr *VtMgr = m_Core.GetVirtTorrnetsMgr();
        CFtVirtualTorrentMgr tmpVtMgr;
        tmpVtMgr = *VtMgr;
        if( frm->Execute( &tmpVtMgr ) )
        {
            if(frm->GetDirty())
            {
                m_Core.UnInit(FT_IL_VTORRENTS);
                *VtMgr = tmpVtMgr;
                m_Core.Init(FT_IL_VTORRENTS);
                m_isRepaint = true;
            }
        }
        delete frm;
    }
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::LVTorrentsChange(TObject *Sender,
      TListItem *Item, TItemChange Change)
{
    ShowCurrentTorrentInfo();
}
//---------------------------------------------------------------------------
void TFormMain::ShowCurrentTorrentInfo(void)
{
    if(LVTorrents->ItemIndex >= 0)
    {
        TListItem *it = LVTorrents->Items->Item[ LVTorrents->ItemIndex ];
        if(it)
        {
            CFtItemData *data = (CFtItemData *)it->Data;
            if(data->m_Group == FTVIEW_GROUP_VIRTUAL )
            {
                CFtVirtualTorrentMgr *vmgr = m_Core.GetVirtTorrnetsMgr();
                if(vmgr)
                {
                    CFtVirtualTorrent *vt = vmgr->GetVTorrent(data->m_nIndex);
                    if(vt)
                    {
                        AnsiString name = ExtractFileName(vt->GetFileName());
                        AnsiString hash = vt->GetHash20();
                        EditInfoHash->Text = hash;
                        EditInfoFname->Text = name;
                    }
                }
            }
            else
            {
                CFtTorrentsMgr *mgr = m_Core.GetTorrnetsMgr();
                if(mgr)
                {
                    CFtTorrent *tr = mgr->GetTorrent(data->m_nIndex);
                    if(tr)
                    {
                        AnsiString hash = tr->GetHash20();
                        EditInfoHash->Text = hash;
                        AnsiString name = "Название торрента неизвестно...";
                        CFtTorrentInfo *info = tr->GetTorrentInfo();
                        if(info)
                        {
                            name = ExtractFileName(info->GetFileName());
                        }
                        EditInfoFname->Text = name;
                    }
                }
            }
        }
    }
    else
    {
        EditInfoFname->Text = "";
        EditInfoHash->Text = "";
    }
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void TFormMain::CreateOnNotify(void)
{
    m_Core.SetOnSearchUpdates( OnSearchUpdates );
    m_Core.SetOnProcessUpdates( OnProcessUpdates );
}

//---------------------------------------------------------------------------
//Нотифай о проверке обновлений...
void TFormMain::OnSearchUpdates(bool a_bNewUpdates)
{
    if(a_bNewUpdates)
    {
        StatusBar->Panels->Items[0]->Text = "Найдены новые обновления";

        bool bUpdate = true;
        if(m_Core.GetOptions()->GetOtherOpt()->GetFTUpdateAsk())
        {
            TFormUpdateAsk *frm = new TFormUpdateAsk(this);
            if(frm)
            {
                if(!frm->Execute())
                {
                    bUpdate = false;
                }
                delete frm;
            }
        }
        if(bUpdate)
        {
            //Обновляем
            m_Core.StartUpdate();
        }
    }
    else
    {
        StatusBar->Panels->Items[0]->Text = "";
    }
}

//---------------------------------------------------------------------------
//Нотифай о процессе загрузки обновлений...
void TFormMain::OnProcessUpdates(UINT a_nPercent)
{
    AnsiString str = "";
    if(a_nPercent < 100)
    {
        str += "Загрузка обновлений...";
    }
    else
    {
        str += "Обновления загружены";
        m_TrayIcon->BalloonHint("Обовление завершено.",
                                "Обновления вступят в силу при следующем запуске программы.",
                                2, NIIF_INFO);
    }
    StatusBar->Panels->Items[0]->Text = str;
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::NAboutClick(TObject *Sender)
{
    TFormAbout *frm = new TFormAbout(this);
    if(frm)
    {
        frm->Execute(m_Core.GetVer(FTVT_MAJ_MIN_REL_BUI));
        delete frm;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NFtSiteClick(TObject *Sender)
{
    FtOpenHttpPage("http://freetorrents.mrsun.ru");
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::NKeyClick(TObject *Sender)
{
    TFormAbout *frm = new TFormAbout(this);
    if(frm)
    {
        frm->Execute(m_Core.GetVer(FTVT_MAJ_MIN_REL_BUI), FT_ABOUTFORM_PAGEKEY);
        delete frm;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::NGetVIPClick(TObject *Sender)
{
    FtOpenHttpPage("http://freetorrents.mrsun.ru/?p=vip");
}
//---------------------------------------------------------------------------

