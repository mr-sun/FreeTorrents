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


#ifndef uFormMainH
#define uFormMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>

#include <Menus.hpp>
#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>
#include <AppEvnts.hpp>
#include <Buttons.hpp>
#include <ActnCtrls.hpp>
#include <ActnMan.hpp>

#include "uFormAbout.h"
#include "uFormUpdateAsk.h"
#include "uFormVirtualMgr.h"
#include "uFormEditRules.h"
#include "uFormProperty.h"
#include "uFtCore.h"
#include "uFtTrayIcon.h"

//Группы отображения (Все, изменяемые и не изменяемые)
#define FTVIEW_CUR_GROUP_ALL       0
#define FTVIEW_CUR_GROUP_ACTRUL    1
#define FTVIEW_CUR_GROUP_NOACTRUL  2
#define FTVIEW_CUR_GROUP_VIRTUAL   3

#define FTVIEW_GROUP_ACTRUL    1
#define FTVIEW_GROUP_NOACTRUL  2
#define FTVIEW_GROUP_VIRTUAL   3

//Дополнтельные данные для каждого итема в списке торрентов
struct CFtItemData
{
    int  m_Group;  // FTVIEW_GROUP (1-ACTRUL, 2-NOACTRUL, 3-VIRTUAL)
    UINT m_nIndex; //Индекс в списке менеджера
};

//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components
        TMainMenu *MainMenu1;
        TMenuItem *N1;
        TMenuItem *N3;
        TMenuItem *N4;
        TPanel *PanelLeft;
        TStatusBar *StatusBar;
        TSplitter *Splitter1;
        TPanel *PanelRight;
        TListView *LVGroups;
        TPanel *PanelTop;
        TPanel *PanelBottom;
        TSplitter *Splitter2;
        TListView *LVTorrents;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TTimer *TimerUpdate;
        TMenuItem *NProperty;
        TMenuItem *NExit;
        TMenuItem *NRulesEdit;
        TPopupMenu *PopupMenuTray;
        TMenuItem *NShow;
        TMenuItem *N5;
        TMenuItem *N6;
        TMenuItem *N7;
        TMenuItem *N8;
        TImageList *ImageList1;
    TMenuItem *NAbout;
        TMenuItem *N9;
        TImageList *ImageList2;
        TImageList *ImageList4;
        TMenuItem *N2;
        TMenuItem *N10;
        TToolBar *ToolBar1;
        TToolButton *ToolBtnProp;
        TToolButton *ToolBtnRules;
        TToolButton *ToolButton3;
        TToolButton *ToolBtnStart;
        TToolButton *ToolBtnStop;
        TToolButton *ToolButton6;
        TToolButton *ToolBtnUpdate;
        TToolButton *ToolBtnVirtual;
        TMenuItem *N11;
    TMenuItem *NVirtualMgr;
        TMenuItem *N13;
    TMenuItem *N14;
    TMenuItem *NStartStop;
        TLabel *Label1;
        TEdit *EditInfoHash;
        TLabel *Label2;
        TEdit *EditInfoFname;
        TPopupMenu *PopupMenuList;
    TMenuItem *N12;
    TMenuItem *NUpdate;
    TMenuItem *NFtSite;
    TMenuItem *NAbout2;
    TMenuItem *N15;
    TMenuItem *N16;
        TMenuItem *NKey;
        TMenuItem *NGetVIP;
        void __fastcall TimerUpdateTimer(TObject *Sender);
        void __fastcall NPropertyClick(TObject *Sender);
        void __fastcall NExitClick(TObject *Sender);
        void __fastcall NRulesEditClick(TObject *Sender);
        void __fastcall LVGroupsChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
        void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall NShowClick(TObject *Sender);
    void __fastcall LVTorrentsCustomDrawItem(TCustomListView *Sender,
          TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
    void __fastcall LVGroupsCustomDrawItem(TCustomListView *Sender,
          TListItem *Item, TCustomDrawState State, bool &DefaultDraw);
        void __fastcall ToolBtnStopClick(TObject *Sender);
        void __fastcall ToolBtnStartClick(TObject *Sender);
    void __fastcall ToolBtnUpdateClick(TObject *Sender);
    void __fastcall NVirtualMgrClick(TObject *Sender);
        void __fastcall LVTorrentsChange(TObject *Sender, TListItem *Item,
          TItemChange Change);
    void __fastcall NAboutClick(TObject *Sender);
    void __fastcall NFtSiteClick(TObject *Sender);
        void __fastcall NKeyClick(TObject *Sender);
        void __fastcall NGetVIPClick(TObject *Sender);

private:
    CFtCore m_Core;
    int m_CurrentGroup; //FTVIEW_...
    int m_SelectedTorrent; //Торрент выбранный в списке..
    bool m_isRepaint; //Флаг необходимости полностью перерисовать список торрентов.
    bool m_bServerRunning; //Запущенность сервера
    bool m_CanClose;
    CFtTrayIcon *m_TrayIcon;

public:
    __fastcall TFormMain(TComponent* Owner);

private:
    //Создание иконки в трее
    void CreateTrayIcon(void);
    //Создание левой колонки Групп
    void CreateGroups(void);
    //Создаем панельки в статус-баре
    void CreateStatusBar(void);

    //Обновление листа торрентов
    void UpdateTorrentsList(void);
    void UpdateServerState(bool a_bForce=false);
    void UpdateStatusBar(void);
    void UpdateTrayHint(void);        

    void ShowCurrentTorrentInfo(void);

private:
    void __fastcall AppMessage(tagMSG &Msg, bool &Handled);
    void MinimizedOnStart(void);

private:
    void CreateOnNotify(void);
    void OnSearchUpdates(bool a_bNewUpdates);
    void OnProcessUpdates(UINT a_nPercent);    
        
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
