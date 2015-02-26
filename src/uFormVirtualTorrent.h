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

#ifndef uFormVirtualTorrentH
#define uFormVirtualTorrentH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <StrUtils.hpp>
#include <Dialogs.hpp>

#include "uFtVirtualTorrents.h"


//Тип формы
#define FT_FTYPE_VT_ADD   0
#define FT_FTYPE_VT_EDIT  1

//---------------------------------------------------------------------------
class TFormVirtualTorrent : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
        TEdit *EditFName;
    TButton *BtnBrowse;
    TGroupBox *GroupBox2;
    TGroupBox *GroupBox3;
        TListView *ListViewTrackers;
    TLabel *Label2;
        TEdit *EditUpSpeed;
    TLabel *Label3;
    TButton *BtnCancel;
    TButton *BtnOk;
    TOpenDialog *OpenDialog1;
        TLabel *Label1;
        TLabel *Label4;
        TEdit *EditName;
        TEdit *EditHash;
        TStaticText *StaticText1;
    void __fastcall BtnOkClick(TObject *Sender);
    void __fastcall BtnCancelClick(TObject *Sender);
    void __fastcall BtnBrowseClick(TObject *Sender);

private:
    CFtVirtualTorrent *m_VTorrent;
    bool m_bDirty;

public:
    __fastcall TFormVirtualTorrent(TComponent* Owner);
    bool Execute(CFtVirtualTorrent *a_VTorrent, int a_Type = FT_FTYPE_VT_ADD);
    bool GetDirty(void);

private:
    void Initialize(void);
    void UpdateData(void);
    bool Apply(void);
        
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif
