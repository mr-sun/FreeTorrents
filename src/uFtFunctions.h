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

#ifndef uFtFunctionsH
#define uFtFunctionsH

#include <StrUtils.hpp>
#include <Registry.hpp>
#include <vector.h>

#define isMaskDone() ((bool)!*mask)
#define EOS  0

#define FTGET_NOPARAMETR  -1 //идентификатор отсутствующего парамтера


//---------------------------------------------------------------------------
//Преобразует число в байтах, в читаемую строку в (КБ, МБ, ГБ).
AnsiString ConvertBytesS(double a_bytes );

//---------------------------------------------------------------------------
//Выделить информацию о торренте из GET запроса
bool GetTorentInfoFromGET(AnsiString a_sData, AnsiString &a_sHash, AnsiString &a_sHost,
                          __int64 &a_nUpload, __int64 &a_nDownload, __int64 &a_nLeft, AnsiString &a_sAction);

//---------------------------------------------------------------------------
//Поместсить инфу о торренте в GET запрос
AnsiString SetTorrentInfoForGET(AnsiString a_sData, __int64 a_nUpload, __int64 a_nUploadNew,
                                __int64 a_nDownload, __int64 a_nDownloadNew, __int64 a_nLeft,
                                __int64 a_nLeftNew, AnsiString a_sAction, AnsiString a_sActionNew);

//------------------------------------------------------------------------------
//Проверить вхождение имени Хоста в подмножество хостов по шаблону
bool CompareHosts(AnsiString a_sHost, AnsiString a_sHostPattern);

bool GetHostPortFromURL(AnsiString a_URL, AnsiString &a_Req, AnsiString &a_Host, UINT &a_Port);

//------------------------------------------------------------------------------
bool NFAST_wildcmp(PCSTR mask, PCSTR name);

//------------------------------------------------------------------------------
//Кодирование и декодирование Хешей торрентов
AnsiString FtURLEncode(BYTE *a_S);
AnsiString FtHashEncode20(BYTE *a_S, int a_size);
bool FtURLDecode(AnsiString a_S, BYTE *a_Hash);

//------------------------------------------------------------------------------
bool FtOpenHttpPage(AnsiString a_address);

#endif
