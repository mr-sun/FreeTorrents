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

#define NO_WIN32_LEAN_AND_MEAN
#include <vcl.h>
#pragma hdrstop

#include "uFormEditPath.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
int CALLBACK BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam,
                                 LPARAM lpData )
{
    if (BFFM_INITIALIZED == uMsg)
        SendMessage( hwnd, BFFM_SETSELECTION, TRUE, lpData);
    return 0;
}
//---------------------------------------------------------------------------
bool GetFolder( LPCSTR szTitle, LPSTR szPath)
{
    AnsiString szIntPath = szPath;
    BROWSEINFO bi;

    bi.hwndOwner = 0;
    bi.pidlRoot = 0;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = szTitle;
    bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_BROWSEINCLUDEURLS |BIF_SHAREABLE | BIF_USENEWUI;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = reinterpret_cast<LPARAM>(szIntPath.c_str());
    bi.iImage = 0;

    ITEMIDLIST * p = SHBrowseForFolder( &bi);

    if ( 0 != p )
    {
        BOOL bres = SHGetPathFromIDList( p, szPath);
        CoTaskMemFree (p);
        return bres;
    }
    else
        return false;
}

//---------------------------------------------------------------------------
__fastcall TFormEditPath::TFormEditPath(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFormEditPath::BtnOKClick(TObject *Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TFormEditPath::BtnCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------
bool TFormEditPath::Execute(AnsiString &a_Path, int a_FormType)
{
    if(a_FormType == FT_FTYPE_PATH_ADD)
        this->Caption = "Добавление нового пути";
    else
        this->Caption = "Редактирование пути";
    EditPath->Text = a_Path;
    if(this->ShowModal() == mrOk)
    {
        a_Path = EditPath->Text;
        return true;
    }
    return false;
}
void __fastcall TFormEditPath::EditPathKeyPress(TObject *Sender, char &Key)
{
    if(Key == VK_RETURN)
      BtnOKClick(Sender);
}
//---------------------------------------------------------------------------
//Обзор каталогов
void __fastcall TFormEditPath::BtnBrowseClick(TObject *Sender)
{
    AnsiString folder, title;
    title = "Выбор каталога";
    folder = EditPath->Text;
    char cfolder[MAX_PATH];
    FillMemory(cfolder, MAX_PATH, 0);
    strcpy(cfolder, folder.c_str());
    if(GetFolder( title.c_str(), cfolder))
    {
        EditPath->Text = cfolder;
    }
}
//---------------------------------------------------------------------------

