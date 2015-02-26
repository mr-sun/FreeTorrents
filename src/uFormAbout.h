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

#ifndef uFormAboutH
#define uFormAboutH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "uFtFunctions.h"
#include <ComCtrls.hpp>
#include <Graphics.hpp>

#define FT_ABOUTFORM_PAGEABOUT   0//Страница "О программе"
#define FT_ABOUTFORM_PAGEKEY     1//Страница "Ключи"

//---------------------------------------------------------------------------
class TFormAbout : public TForm
{
__published:	// IDE-managed Components
    TButton *ButtonOk;
        TPageControl *PageControl1;
        TTabSheet *TabSheet1;
        TLabel *Label16;
        TLabel *Label17;
        TLabel *Label18;
        TLabel *LabelVers;
        TLabel *LabelType;
        TLabel *Label21;
        TLabel *Label22;
        TLabel *Label23;
        TLabel *Label24;
        TBevel *Bevel4;
        TTabSheet *TabSheet2;
        TBevel *Bevel1;
        TLabel *Label1;
        TLabel *Label2;
        TEdit *EditKey;
        TLabel *Label3;
        TLabel *Label8;
        TLabel *LabelKey;
        TLabel *Label4;
        TLabel *LabelKeyDateEnd;
        TLabel *LabelWarn;
        TLabel *LabelWarn2;
        TImage *Image1;
        TImage *Image2;
    void __fastcall LabelSiteClick(TObject *Sender);
    void __fastcall ButtonOkClick(TObject *Sender);
        void __fastcall EditKeyKeyPress(TObject *Sender, char &Key);

private:
    bool Apply(void);
    
public:
    __fastcall TFormAbout(TComponent* Owner);
    bool Execute(AnsiString a_sVers, int a_nActvePage=FT_ABOUTFORM_PAGEABOUT);
};
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#endif
