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

#include "uFormEditRule.h"
#include "uFtCore.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TFormEditRule::TFormEditRule(TComponent* Owner)
        : TForm(Owner)
{
}

//---------------------------------------------------------------------------
bool TFormEditRule::Execute(CFtRule *a_Rule, int a_Type)
{
    m_bDirty = false;
    if(a_Type == FT_FTYPE_RULE_ADD)
        this->Caption = "Создание правила";
    else
        this->Caption = "Редактирование правила";
    m_Rule = a_Rule;
    UpdateData();
    if(this->ShowModal() == mrOk)
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRule::BtnOkClick(TObject *Sender)
{
    if( !Apply() ) return;
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRule::BtnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
bool TFormEditRule::Apply(void)
{
    if(!m_Rule) return true;

    AnsiString sName = EditName->Text.Trim();
    if(sName=="")
    {
        MessageBox(Handle, "Необходимо задать название правила.",
                           "Внимание!", MB_OK|MB_ICONWARNING);
        return false;
    }
    AnsiString oldName = m_Rule->GetName();
    m_Rule->SetName( sName );
    if(oldName != sName) m_bDirty = true;

    AnsiString oldHost = m_Rule->GetHost();
    AnsiString sHost = EditHost->Text;
    m_Rule->SetHost( sHost );
    if(oldHost != sHost) m_bDirty = true;

    //Хэш либо пустой, либо 20 символов
    AnsiString sHash20 = EditHash->Text.Trim();
    if(sHash20.Length() > 0 && sHash20.Length() < 40 )
    {
        MessageBox(Handle, "Неверно указан Хэш торрент-файла.\n"
                           "Значение должно состоять либо из 40 символов, либо быть пустым.",
                           "Внимание!", MB_OK|MB_ICONWARNING);
        return false;
    }
    AnsiString oldHash = m_Rule->GetTorrentHash();
    m_Rule->SetTorrentHash( sHash20 );
    if(oldHash != sHash20) m_bDirty = true;

    bool oldUseUp = m_Rule->GetUseKUpload();
    m_Rule->SetUseKUpload( CheckBoxUseUp->Checked );
    if(oldUseUp != CheckBoxUseUp->Checked) m_bDirty = true;

    bool oldUseKDownload = m_Rule->GetUseKDownload();
    m_Rule->SetUseKDownload( CheckBoxUseDown->Checked );
    if(oldUseKDownload != CheckBoxUseDown->Checked) m_bDirty = true;

    double kUp;
    double PercentDown;
    char dc = DecimalSeparator;
    DecimalSeparator = '.';
    try
    {
        AnsiString skUp = EditKUp->Text;
        AnsiString skDown = EditKDown->Text;
        skUp = AnsiReplaceStr(skUp, ",", ".");
        skDown = AnsiReplaceStr(skDown, ",", ".");
        kUp = StrToFloat(skUp);
        PercentDown = StrToFloat(skDown);
    }
    catch(...)
    {
        DecimalSeparator = dc;
        MessageBox(Handle, "Неверно указано значение одного из коэффициентов.",
                           "Внимание!", MB_OK|MB_ICONWARNING);
        return false;
    }
    if(g_Ver) //Ограничиваем Коэффициенты UP
    {
        if(g_Ver->GetProgrType()==FTTYPE_VER_LIGHT)
        {
            if(kUp > FT_RULE_UPDEFCOEFF)
            {
                kUp = FT_RULE_UPDEFCOEFF;
                DecimalSeparator = dc;                
                AnsiString str = "Вы используете Начальную версию FreeTorrents.\n"
                                   "В этой версии максимально допустимый коэффициент увеличения раздачи = (Upload) x "
                                   +AnsiString(FT_RULE_UPDEFCOEFF)+"\n"
                                   "Если Вам необходимо устанавливать более высокие коэффициенты, то Вы можете\n"
                                   "приобрести расширенную VIP версию на сайте программы.";
                MessageBox(Handle, str.c_str(), "Внимание!", MB_OK|MB_ICONINFORMATION);
                EditKUp->Text = FloatToStr(kUp);
                return false;
            }
        }
    }
    double oldUp = m_Rule->GetKUpload();
    m_Rule->SetKUpload(kUp);
    if(oldUp != kUp) m_bDirty = true;

    if(g_Ver) //Ограничиваем Коэффициенты Down
    {
        if(g_Ver->GetProgrType()==FTTYPE_VER_LIGHT)
        {
            if(PercentDown < FT_RULE_DOWNDEFPERCENT)
            {
                PercentDown = FT_RULE_DOWNDEFPERCENT;
                DecimalSeparator = dc;                
                AnsiString str = "Вы используете Начальную версию FreeTorrents.\n"
                                   "В этой версии допускается уменьшение закачки с отображением не менее чем "
                                   +AnsiString(FT_RULE_DOWNDEFPERCENT)+"%.\n"
                                   "Если Вам необходимо устанавливать более низкий % отображения, то Вы можете\n"
                                   "приобрести расширенную VIP версию на сайте программы.";
                MessageBox(Handle, str.c_str(), "Внимание!", MB_OK|MB_ICONINFORMATION);
                EditKDown->Text = FloatToStrF(PercentDown, ffFixed, 10, 1);
                TBPercentDownload->Position = PercentDown * 10;
                return false;
            }
        }
    }
    double oldDown = m_Rule->GetPercentDownload();
    m_Rule->SetPercentDownload(PercentDown);
    if(oldDown != PercentDown) m_bDirty = true;

    DecimalSeparator = dc;
    return true;
}
//---------------------------------------------------------------------------
void TFormEditRule::UpdateData(void)
{
    if(!m_Rule) return;
    EditName->Text = m_Rule->GetName();
    EditHost->Text = m_Rule->GetHost();
    EditHash->Text = m_Rule->GetTorrentHash();

    CheckBoxUseUp->Checked = m_Rule->GetUseKUpload();
    CheckBoxUseDown->Checked = m_Rule->GetUseKDownload();
    EditKUp->Text = FloatToStrF(m_Rule->GetKUpload(), ffFixed, 10, 2);

    double percentdown = m_Rule->GetPercentDownload();
    EditKDown->Text = FloatToStrF(percentdown, ffFixed, 10, 1);
    TBPercentDownload->Position = percentdown * 10;

    UpdateCheck();
}

//---------------------------------------------------------------------------
void TFormEditRule::UpdateCheck(void)
{
    if(CheckBoxUseUp->Checked)
    {
        EditKUp->Enabled = true;
        EditKUp->Color = clWindow;
    }
    else
    {
        EditKUp->Enabled = false;
        EditKUp->Color = clBtnFace;
    }
    if(CheckBoxUseDown->Checked)
    {
        TBPercentDownload->Enabled = true;
        EditKDown->Enabled = true;
        EditKDown->Color = clWindow;
    }
    else
    {
        TBPercentDownload->Enabled = false;
        EditKDown->Enabled = false;        
        EditKDown->Color = clBtnFace;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRule::CheckBoxUseUpClick(TObject *Sender)
{
    UpdateCheck();
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRule::EditNameKeyPress(TObject *Sender, char &Key)
{
    if(Key == VK_RETURN)
      BtnOkClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TFormEditRule::TBPercentDownloadChange(TObject *Sender)
{
    double percent = TBPercentDownload->Position / 10.0;
    EditKDown->Text = FloatToStrF(percent, ffFixed, 10, 1);
}
//---------------------------------------------------------------------------
bool TFormEditRule::GetDirty(void)
{
    return m_bDirty;
}
