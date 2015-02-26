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

#include "uFormAbout.h"
#include "uFtCore.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TFormAbout::TFormAbout(TComponent* Owner)
    : TForm(Owner)
{
    LabelWarn->Caption = "";
    LabelWarn2->Caption = "";
    if(g_Ver)
    {
        BYTE progrType = g_Ver->GetProgrType();
        BYTE progrCurType = g_Ver->GetCurrProgrType();
        if(progrType!=progrCurType)
        {
            LabelWarn->Caption = "(��������� ����)";
            LabelWarn2->Caption = "(���������)";
        }
        switch(progrCurType)
        {
            case FTTYPE_VER_LIGHT:
            {
                LabelType->Caption = "��������� (����������)";
                break;
            }
            case FTTYPE_VER_NORMAL:
            {
                LabelType->Caption = "�����������";
                break;
            }
            case FTTYPE_VER_PROF:
            {
                LabelType->Caption = "����������������";
                break;
            }
            case FTTYPE_VER_VIP:
            {
                LabelType->Caption = "VIP";            
                break;
            }
        }
    }

    AnsiString key = g_Ver->GetKey();
    if(key=="")
    {
        LabelKey->Caption = "�����������";
    }
    else
    {
        LabelKey->Caption = key;
    }
    AnsiString dateEnd = g_Ver->GetDateEnd();
    if(dateEnd=="")
    {
        LabelKeyDateEnd->Caption = "����������";
    }
    else
    {
        LabelKeyDateEnd->Caption = dateEnd;
    }
}

//---------------------------------------------------------------------------
bool TFormAbout::Execute(AnsiString a_sVers, int a_nActvePage)
{
    int nInd = 0;
    if(a_nActvePage < PageControl1->PageCount && a_nActvePage>=0)
        nInd = a_nActvePage;
    PageControl1->ActivePageIndex = nInd;

    LabelVers->Caption = a_sVers;
    if(this->ShowModal()==mrOk)
    {
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TFormAbout::LabelSiteClick(TObject *Sender)
{
    FtOpenHttpPage("http://freetorrents.mrsun.ru");
}
//---------------------------------------------------------------------------
void __fastcall TFormAbout::ButtonOkClick(TObject *Sender)
{
    if(!Apply())
    {
        PageControl1->ActivePageIndex = 1;
        return;
    }
    ModalResult = mrOk;
}

//---------------------------------------------------------------------------
bool TFormAbout::Apply(void)
{
    AnsiString key = EditKey->Text;
    if(key=="") return true;
    if(!g_Ver) return true;
    bool bRes;

    //������ �����-�� ���� - ���������
    int nErr;
    if(g_Ver->SetSertificate(key, nErr))
    {
        //���������� ������� ����������
        AnsiString str = "���� ������� ���������������!\n"
                         "����� �������� ����� �������� � ����, ���������� ������������� FreeTorrents.\n"
                         "�������, ��� ����������� ����� ����������.";
        MessageBox(Handle, str.c_str(), "���� ������� ���������������!", MB_OK|MB_ICONINFORMATION);
        bRes = true;
    }
    else
    {
        //������ ����� �����������
        switch(nErr)
        {
            case FTSERT_ERR_INVALID:
            {
                //�� ������ ��� �����������
                AnsiString str = "������ �� ���������� ����.";
                MessageBox(Handle, str.c_str(), "������ ����������� �����", MB_OK|MB_ICONWARNING);
                break;
            }
            case FTSERT_ERR_ENDREGDATE:
            {
                //������ ������ ��� ����� ����� ����������� (2-3 ���)
                AnsiString str = "������ ��� ����� ������� ����� (3 ��� ����� ���������) ����������.";
                MessageBox(Handle, str.c_str(), "������ ����������� �����", MB_OK|MB_ICONWARNING);
                break;
            }
            case FTSERT_ERR_ENDDATE:
            {
                //������ ���� �������� �����������
                AnsiString str = "���� �������� ����� ����������.";
                MessageBox(Handle, str.c_str(), "������ ����������� �����", MB_OK|MB_ICONWARNING);
                break;
            }
            case FTSERT_ERR_NOBEGINDATE:
            {
                //���� �������� ��� �� ��������
                AnsiString str = "���� �������� ����� ��� �� ��������.";
                MessageBox(Handle, str.c_str(), "������ ����������� �����", MB_OK|MB_ICONWARNING);
                break;
            }
            case FTSERT_ERR_MORELONGDATE:
            {
                //������� ������� ���� ����������� - ����������� ����.
                AnsiString str = "������ �� ���������� ����.";
                MessageBox(Handle, str.c_str(), "������ ����������� �����", MB_OK|MB_ICONWARNING);
                break;
            }
            case FTSERT_ERR_NOBEGINEND:
            {
                //����� �������� ������, ��� ������ ������ �����������
                AnsiString str = "������ �� ���������� ����.";
                MessageBox(Handle, str.c_str(), "������ ����������� �����", MB_OK|MB_ICONWARNING);
                break;
            }
            case FTSERT_ERR_NOWRITEREG:
            {
                //�� ������� ��������� ���������� � �������.
                AnsiString str = "��������!\n"
                                 "��������� ���� ���� �� ������� ���������� � �������.\n"
                                 "������ ����� �� �� ��������� ������� �������������� �� ���� ����������.\n"
                                 "�������, ��� �� ������ ���������� ���� ���� � ������� 3 ���� � ������� ��� ���������.";
                MessageBox(Handle, str.c_str(), "��������!", MB_OK|MB_ICONWARNING);
                break;
            }
        }
        bRes = false;
    }
    return bRes;
}
//---------------------------------------------------------------------------
void __fastcall TFormAbout::EditKeyKeyPress(TObject *Sender, char &Key)
{
    if(Key==VK_RETURN)
    {
        ButtonOkClick(Sender);
    }
}
//---------------------------------------------------------------------------

