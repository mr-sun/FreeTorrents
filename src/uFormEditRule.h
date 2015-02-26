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

#ifndef uFormEditRuleH
#define uFormEditRuleH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <StrUtils.hpp>


#include "uFtRules.h"
#include <ComCtrls.hpp>

#define FT_FTYPE_RULE_ADD   0
#define FT_FTYPE_RULE_EDIT  1

//---------------------------------------------------------------------------
class TFormEditRule : public TForm
{
__published:	// IDE-managed Components
        TButton *BtnOk;
        TButton *BtnCancel;
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TEdit *EditName;
        TLabel *Label2;
        TEdit *EditHost;
        TLabel *Label3;
        TEdit *EditHash;
        TCheckBox *CheckBoxUseUp;
        TBevel *Bevel1;
        TCheckBox *CheckBoxUseDown;
        TLabel *Label4;
        TEdit *EditKUp;
        TEdit *EditKDown;
        TTrackBar *TBPercentDownload;
        TLabel *Label5;
        TLabel *Label6;
        void __fastcall BtnOkClick(TObject *Sender);
        void __fastcall BtnCancelClick(TObject *Sender);
        void __fastcall CheckBoxUseUpClick(TObject *Sender);
        void __fastcall EditNameKeyPress(TObject *Sender, char &Key);
        void __fastcall TBPercentDownloadChange(TObject *Sender);

private:
    CFtRule *m_Rule;
    bool m_bDirty;

public:
    __fastcall TFormEditRule(TComponent* Owner);
    bool Execute(CFtRule *a_Rule, int a_Type = FT_FTYPE_RULE_ADD);
    bool GetDirty(void);
    
private:
    bool Apply(void);
    void UpdateData(void);
    void UpdateCheck(void);    
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
