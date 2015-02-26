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

#ifndef uFormVirtualMgrH
#define uFormVirtualMgrH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include "uFtVirtualTorrents.h"
#include "uFormVirtualTorrent.h"


//---------------------------------------------------------------------------
class TFormVirtualMgr : public TForm
{
__published:	// IDE-managed Components
    TGroupBox *GroupBox1;
    TListView *ListViewVirtual;
    TButton *BtnAdd;
    TButton *BtnEdit;
    TButton *BtnDelete;
    TStatusBar *StatusBar1;
    TButton *BtnCancel;
    TButton *BtnOk;
    void __fastcall BtnCancelClick(TObject *Sender);
    void __fastcall BtnOkClick(TObject *Sender);
    void __fastcall BtnDeleteClick(TObject *Sender);
    void __fastcall BtnAddClick(TObject *Sender);
    void __fastcall BtnEditClick(TObject *Sender);
    void __fastcall ListViewVirtualDblClick(TObject *Sender);
    void __fastcall ListViewVirtualSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
    void __fastcall ListViewVirtualClick(TObject *Sender);
    void __fastcall ListViewVirtualKeyPress(TObject *Sender, char &Key);

private:
    CFtVirtualTorrentMgr *m_VtMgr;
    bool m_bDirty;

public:
    __fastcall TFormVirtualMgr(TComponent* Owner);
    bool Execute(CFtVirtualTorrentMgr *a_VtMgr);
    bool GetDirty(void);

private:
    void Initialize(void);
    void UpdateVirtualList(void);
    void UpdateBtns(void);        

};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif
