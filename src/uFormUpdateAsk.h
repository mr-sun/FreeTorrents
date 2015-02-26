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

#ifndef uFormUpdateAskH
#define uFormUpdateAskH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>

#include "uFtFunctions.h"

//---------------------------------------------------------------------------
class TFormUpdateAsk : public TForm
{
__published:	// IDE-managed Components
    TStaticText *StaticText1;
    TButton *BtnCancel;
    TButton *BtnOk;
    TLabel *Label1;
    TLabel *LabelSite;
    TLabel *Label3;
    TBevel *Bevel1;
    TImage *Image1;
    void __fastcall BtnOkClick(TObject *Sender);
    void __fastcall BtnCancelClick(TObject *Sender);
    void __fastcall LabelSiteClick(TObject *Sender);
private:	// User declarations
public:
    __fastcall TFormUpdateAsk(TComponent* Owner);
    bool Execute(void);
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif
