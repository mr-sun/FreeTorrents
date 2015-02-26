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

#ifndef uFormPropertyH
#define uFormPropertyH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include <Dialogs.hpp>

#include "uFormEditPath.h"
#include "uFormEditStr.h"
#include "uFtOptions.h"


//---------------------------------------------------------------------------
class TFormProperty : public TForm
{
__published:	// IDE-managed Components
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TTabSheet *TabSheet2;
        TButton *BtnCancel;
        TButton *BtnOk;
        TTabSheet *TabSheet3;
        TGroupBox *FreeTorrent;
        TLabel *Label1;
        TComboBox *ComboBox1;
        TLabel *Label2;
        TEdit *EditPort;
        TGroupBox *GroupBox1;
        TRadioButton *RBUseProxyNo;
        TRadioButton *RBUseProxyYes;
        TComboBox *ComboBoxProxyType;
        TLabel *Label3;
        TLabel *Label4;
        TEdit *EditProxyHost;
        TLabel *Label5;
        TEdit *EditProxyPort;
        TGroupBox *GroupBox2;
        TEdit *EditTrntClnt;
        TButton *BtnBrowse;
        TLabel *Label6;
        TCheckBox *CheckBoxAutoStart;
        TCheckBox *CheckBoxAutoClose;
        TGroupBox *GroupBox3;
        TListView *ListViewPaths;
        TButton *BtnAddPath;
        TButton *BtnDelPath;
        TButton *BtnEditPath;
        TOpenDialog *OpenDialog1;
        TGroupBox *GroupBox4;
        TCheckBox *CheckBoxStartWithWin;
        TCheckBox *CheckBoxCloseToTray;
        TGroupBox *GroupBox5;
        TCheckBox *CheckBoxSearchUpdate;
        TCheckBox *CheckBoxStartMinimized;
    TCheckBox *CheckBoxFTUpdateAsk;
    TTabSheet *TabSheet4;
    TGroupBox *GroupBox6;
    TListView *ListViewExcTrackers;
    TButton *BtnAddExc;
    TButton *BtnEditExc;
    TButton *BtnDelExc;
    TTabSheet *TabSheet5;
    TGroupBox *GroupBox7;
    TLabel *Label7;
    TComboBox *ComboBoxClts;
    TLabel *Label8;
    TEdit *EditVirtPort;
        void __fastcall BtnOkClick(TObject *Sender);
        void __fastcall BtnCancelClick(TObject *Sender);
        void __fastcall RBUseProxyNoClick(TObject *Sender);
        void __fastcall BtnBrowseClick(TObject *Sender);
        void __fastcall BtnDelPathClick(TObject *Sender);
        void __fastcall BtnAddPathClick(TObject *Sender);
        void __fastcall BtnEditPathClick(TObject *Sender);
        void __fastcall ListViewPathsSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall EditPortKeyPress(TObject *Sender, char &Key);
    void __fastcall CheckBoxSearchUpdateClick(TObject *Sender);
    void __fastcall BtnAddExcClick(TObject *Sender);
    void __fastcall BtnEditExcClick(TObject *Sender);
    void __fastcall BtnDelExcClick(TObject *Sender);
    void __fastcall ListViewExcTrackersSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);

private:
    CFtOptions *m_Opt;
    bool m_bDirty; //Изменения настроек

public:
    __fastcall TFormProperty(TComponent* Owner);
    bool Execute(CFtOptions *a_Opt);
    bool GetDirty(void);

private:
    bool Initialize(void);
    bool Apply(void);

    //Обновить данные на странице сетевых настроек
    bool UpdateNetPage(void);
    bool ApplyNetPage(void);
    bool NP_UpdateUseProxy(void);

    //Обновить данные на странице торрент-клиента
    bool UpdateTrntClntPage(void);
    bool ApplyTrntClnt(void);
    bool TC_UpdatePathsList(void);
    void TC_UpdatePathsBtns(void);

    //Обновить данные на странице виртуального клиента
    bool UpdateVirtClntPage(void);
    bool ApplyVirtClnt(void);

    //Обновить данные на странице Трекеров
    bool UpdateExcTrackersPage(void);
    bool ApplyExcTrackers(void);
    void TC_UpdateExcTracksBtns(void);    

    //Обновить данные на странице "Общее"
    bool UpdateOtherPage(void);
    bool ApplyOtherPage(void);

};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
