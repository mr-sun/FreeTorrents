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

#include "uFormEditRules.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TFormEditRules::TFormEditRules(TComponent* Owner)
        : TForm(Owner)
{
}

//---------------------------------------------------------------------------
bool TFormEditRules::Execute(CFtRulesMgr *a_RulesMgr)
{
    m_bDirty = false;
    m_RulesMgr = a_RulesMgr;
    Initialize();    
    if(this->ShowModal() == mrOk)
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void TFormEditRules::Initialize(void)
{
    UpdateRulesList();
}

//---------------------------------------------------------------------------
void TFormEditRules::UpdateRulesList(void)
{
    ListViewRules->Items->Clear();
    ListViewRules->Clear();
    if(!m_RulesMgr) return;

    UINT cou = m_RulesMgr->GetCount();
    for(UINT a=0; a < cou; a++)
    {
        CFtRule *rule = m_RulesMgr->GetRule(a);
        if(!rule) continue;
        TListItem *it = ListViewRules->Items->Add();
        if(it)
        {
            it->Caption = rule->GetName();
            it->Checked = rule->GetUse();
            it->SubItems->Add( rule->GetHost() );
            it->SubItems->Add( rule->GetTorrentHash() );
        }
    }
    UpdateRulesBtns();
}
//---------------------------------------------------------------------------
void TFormEditRules::UpdateRulesBtns(void)
{
    if(ListViewRules->ItemIndex >= 0)
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
bool TFormEditRules::Apply(void)
{
    //todo
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TFormEditRules::BtnOkClick(TObject *Sender)
{
    if( !Apply() ) return;
    ModalResult = mrOk;        
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRules::BtnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;        
}

//---------------------------------------------------------------------------
void __fastcall TFormEditRules::BtnDeleteClick(TObject *Sender)
{
    if(!m_RulesMgr) return;
    if(ListViewRules->ItemIndex >= 0)
    {
        AnsiString str = "Вы действительно хотите удалить это правило?";
        if( MessageBox(Handle, str.c_str(), "Внимание!", MB_YESNO|MB_ICONQUESTION) == IDYES)
        {
            m_RulesMgr->DeleteRule( ListViewRules->ItemIndex );
            m_bDirty = true;            
            UpdateRulesList();
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRules::BtnAddClick(TObject *Sender)
{
    if(!m_RulesMgr) return;

    TFormEditRule *frm = new TFormEditRule(this);
    if(frm)
    {
        CFtRule tmpRule;
        if(frm->Execute(&tmpRule))
        {
            m_RulesMgr->AddRule(&tmpRule);
            m_bDirty = true;
            UpdateRulesList();
        }
        delete frm;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRules::BtnEditClick(TObject *Sender)
{
    if(!m_RulesMgr) return;
    if(ListViewRules->ItemIndex >= 0)
    {
        TFormEditRule *frm = new TFormEditRule(this);
        if(frm)
        {
            CFtRule *Rule = m_RulesMgr->GetRule( ListViewRules->ItemIndex );
            CFtRule tmpRule = *Rule;
            if(frm->Execute(&tmpRule, FT_FTYPE_RULE_EDIT))
            {
                if(frm->GetDirty())
                {
                    *Rule = tmpRule;
                    m_bDirty = true;
                    UpdateRulesList();
                }
            }
            delete frm;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRules::ListViewRulesDblClick(TObject *Sender)
{
    BtnEditClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRules::ListViewRulesSelectItem(TObject *Sender,
      TListItem *Item, bool Selected)
{
    UpdateRulesBtns();        
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRules::ListViewRulesClick(TObject *Sender)
{
    if(!m_RulesMgr) return;
    UINT cou = m_RulesMgr->GetCount();
    for(UINT a=0; a < cou; a++)
    {
        CFtRule *rule = m_RulesMgr->GetRule(a);
        if(!rule) continue;
        if(rule->GetUse() != ListViewRules->Items->Item[a]->Checked)
        {
            rule->SetUse( ListViewRules->Items->Item[a]->Checked );
            m_bDirty = true;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRules::ListViewRulesKeyPress(TObject *Sender,
      char &Key)
{
    if(Key == VK_SPACE)
        ListViewRulesClick(Sender);
}

//---------------------------------------------------------------------------
bool TFormEditRules::GetDirty(void)
{
    return m_bDirty;
}
