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

#ifndef uFormEditRulesH
#define uFormEditRulesH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include "uFormEditRule.h"
#include "uFtRules.h"
#include <ExtCtrls.hpp>
#include <Graphics.hpp>

//---------------------------------------------------------------------------
class TFormEditRules : public TForm
{
__published:	// IDE-managed Components
        TButton *BtnOk;
        TButton *BtnCancel;
        TGroupBox *GroupBox1;
        TListView *ListViewRules;
        TButton *BtnAdd;
        TButton *BtnEdit;
        TButton *BtnDelete;
        TStatusBar *StatusBar1;
        TStaticText *StaticText1;
        TImage *Image1;
        void __fastcall BtnOkClick(TObject *Sender);
        void __fastcall BtnCancelClick(TObject *Sender);
        void __fastcall BtnDeleteClick(TObject *Sender);
        void __fastcall BtnAddClick(TObject *Sender);
        void __fastcall BtnEditClick(TObject *Sender);
        void __fastcall ListViewRulesDblClick(TObject *Sender);
        void __fastcall ListViewRulesSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall ListViewRulesClick(TObject *Sender);
        void __fastcall ListViewRulesKeyPress(TObject *Sender, char &Key);

private:
    CFtRulesMgr *m_RulesMgr;
    bool m_bDirty;

public:
    __fastcall TFormEditRules(TComponent* Owner);
    bool Execute(CFtRulesMgr *a_RulesMgr);
    bool GetDirty(void);

private:
    void Initialize(void);
    bool Apply(void);
    void UpdateRulesList(void);
    void UpdateRulesBtns(void);    
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
