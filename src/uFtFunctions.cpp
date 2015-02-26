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

#include "uFtFunctions.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
//Преобразует число в байтах, в читаемую строку в (КБ, МБ, ГБ).
AnsiString ConvertBytesS(double a_bytes )
{
    double kb = 1024;
    double mb = kb*1024;
    double gb = mb*1024;
    if(a_bytes < kb)
    {
        return FloatToStrF(a_bytes, ffFixed, 10, 2)+" Б";
    }
    if(a_bytes < mb)
    {
        return FloatToStrF(a_bytes/kb, ffFixed, 10, 2)+" КБ";
    }
    if(a_bytes < gb)
    {
        return FloatToStrF(a_bytes/mb, ffFixed, 10, 2)+" МБ";
    }
    return FloatToStrF(a_bytes/gb, ffFixed, 10, 2)+" ГБ";    
}

//---------------------------------------------------------------------------
//Выделить информацию о торренте из GET запроса
bool GetTorentInfoFromGET(AnsiString a_sData, AnsiString &a_sHash, AnsiString &a_sHost,
                          __int64 &a_nUpload, __int64 &a_nDownload, __int64 &a_nLeft, AnsiString &a_sAction)
{
    a_nUpload = FTGET_NOPARAMETR;
    a_nDownload = FTGET_NOPARAMETR;
    a_nLeft = FTGET_NOPARAMETR;
    a_sAction = "";
    int eos = a_sData.AnsiPos("\n");
    if(!eos) return false;
    AnsiString str1 = (a_sData.SubString(1, eos)).Trim();

    //Ищем Хеш (info_hash)
    a_sHash = "";
    int ih = str1.AnsiPos("info_hash=");
    if(!ih) return false;
    AnsiString sInfoHash = str1.SubString(ih+10, str1.Length());
    int ihe = sInfoHash.AnsiPos("&");
    if(!ihe) ihe = sInfoHash.AnsiPos(" ");
    if(!ihe) return false;
    a_sHash = sInfoHash.SubString(1, ihe-1);
    if(a_sHash=="") return false;

    //Ищем хост (трекер)
    a_sHost = "";
    int hst = a_sData.AnsiPos("\nHost:");
    if(!hst) return false;
    AnsiString sHst = a_sData.SubString(hst+6, a_sData.Length());
    eos = sHst.AnsiPos("\n");
    if(!eos) return false;
    AnsiString str2 = (sHst.SubString(1, eos)).Trim();
    ih = str2.AnsiPos(":");
    if(ih)
    {
        a_sHost = (str2.SubString(1, ih-1)).Trim();
    }
    else
    {
        a_sHost = str2.Trim();
    }

    //Ищем (uploaded)
    AnsiString sUpload = "";
    ih = str1.AnsiPos("uploaded=");
    if(ih)
    {
        sUpload = str1.SubString(ih+9, str1.Length());
        ihe = sUpload.AnsiPos("&");
        if(!ihe) ihe = sUpload.AnsiPos(" ");
        if(ihe)
        {
            sUpload = sUpload.SubString(1, ihe-1);
            if(sUpload!="")
            {
                try
                {
                    a_nUpload = StrToInt64(sUpload);
                }
                catch(...)
                {
                    a_nUpload = FTGET_NOPARAMETR;
                }
            }
        }
    }

    //Ищем (downloaded)
    AnsiString sDownload = "";
    ih = str1.AnsiPos("downloaded=");
    if(ih)
    {
        sDownload = str1.SubString(ih+11, str1.Length());
        ihe = sDownload.AnsiPos("&");
        if(!ihe) ihe = sDownload.AnsiPos(" ");
        if(ihe)
        {
            sDownload = sDownload.SubString(1, ihe-1);
            if(sDownload!="")
            {
                try
                {
                    a_nDownload = StrToInt64(sDownload);
                }
                catch(...)
                {
                    a_nDownload = FTGET_NOPARAMETR;
                }
            }
        }
    }

    //Ищем (left)
    AnsiString sLeft = "";
    ih = str1.AnsiPos("left=");
    if(ih)
    {
        sLeft = str1.SubString(ih+5, str1.Length());
        ihe = sLeft.AnsiPos("&");
        if(!ihe) ihe = sLeft.AnsiPos(" ");
        if(ihe)
        {
            sLeft = sLeft.SubString(1, ihe-1);
            if(sLeft!="")
            {
                try
                {
                    a_nLeft = StrToInt64(sLeft);
                }
                catch(...)
                {
                    a_nLeft = FTGET_NOPARAMETR;
                }
            }
        }
    }

    //Ищем (event)
    AnsiString sAction = "";
    ih = str1.AnsiPos("event=");
    if(ih)
    {
        sAction = str1.SubString(ih+6, str1.Length());
        ihe = sAction.AnsiPos("&");
        if(!ihe) ihe = sAction.AnsiPos(" ");
        if(ihe)
        {
            sAction = sAction.SubString(1, ihe-1);
            if(sAction!="")
            {
                a_sAction = sAction;
            }
        }
    }

    return true;
}

//---------------------------------------------------------------------------
//Поместсить инфу о торренте в GET запрос
AnsiString SetTorrentInfoForGET(AnsiString a_sData, __int64 a_nUpload, __int64 a_nUploadNew,
                                __int64 a_nDownload, __int64 a_nDownloadNew, __int64 a_nLeft,
                                __int64 a_nLeftNew, AnsiString a_sAction, AnsiString a_sActionNew)
{
    //меняем параметры uploaded, downloaded и left
    if(a_nUpload != FTGET_NOPARAMETR)
    {
        AnsiString sFrom = "uploaded="+IntToStr(a_nUpload);
        AnsiString sTo = "uploaded="+IntToStr(a_nUploadNew);
        a_sData = AnsiReplaceStr(a_sData, sFrom, sTo);
    }
    if(a_nDownload != FTGET_NOPARAMETR)
    {
        AnsiString sFrom = "downloaded="+IntToStr(a_nDownload);
        AnsiString sTo = "downloaded="+IntToStr(a_nDownloadNew);
        a_sData = AnsiReplaceStr(a_sData, sFrom, sTo);
    }
    if(a_nLeft != FTGET_NOPARAMETR)
    {
        AnsiString sFrom = "left="+IntToStr(a_nLeft);
        AnsiString sTo = "left="+IntToStr(a_nLeftNew);
        a_sData = AnsiReplaceStr(a_sData, sFrom, sTo);
    }
    if(a_sAction != "")
    {
        AnsiString sFrom = "event="+a_sAction;
        AnsiString sTo = "event="+a_sActionNew;
        a_sData = AnsiReplaceStr(a_sData, sFrom, sTo);
    }
    
    return a_sData;
}

//------------------------------------------------------------------------------
//Проверить вхождение имени Хоста в подмножество хостов по шаблону
bool CompareHosts(AnsiString a_sHost, AnsiString a_sHostPattern)
{
    return NFAST_wildcmp(a_sHostPattern.c_str(), a_sHost.c_str());
}

//------------------------------------------------------------------------------
bool NFAST_wildcmp(PCSTR mask, PCSTR name)
{
    PCSTR last; /* yказывает на пpедыдyщий шаблонный символ */

    /* Сpавнить начало (до пеpвого символа '*') шаблона с именем */
    for(;; mask++, name++)
    {
      char ch = *name;
      if(*mask != '?' && *mask != ch) break;
      if(ch == EOS) return isMaskDone(); /* *mask == EOS */
    }

    if(*mask != '*') return false;

    for(;; mask++, name++)
    {
        /* Если символ гpyппы, значит стаpая гpyппа совпала и */
        /* нyжно запомнить новые стаpтовые позиции сpавнения */
        while(*mask == '*')
        { /* while - защита от "**" */
            last = mask++;
            /* Если '*' стоит в конце шаблона, то сканиpовать */
            /* хвост стpоки не тpебyется */
            if(*mask == EOS) return isMaskDone(); /* true */
        }

        /* Если кончилось имя, веpнyть pезyльтат сpавнения */
        char ch = *name;
        if(ch == EOS) return isMaskDone(); /* *mask == EOS */
        if(*mask != '?' && *mask != ch)
        {
            /* Если знак шаблона не совпадает со знаком имени, */
            /* нyжно отстyпить к началy подстpоки и попытаться */
            /* найти её со следyющей позиции имени */
            name -= (size_t)(mask - last) - 1, mask = last;
        }
    }
}

/*----------------------------------------------------------------------------*/
//Выделяем из урла запрос, хост и порт.
bool GetHostPortFromURL(AnsiString a_URL, AnsiString &a_Req, AnsiString &a_Host, UINT &a_Port)
{
    a_Port = 80;
    a_Host = "";
    a_Req = "/";
    int httpPos = a_URL.AnsiPos("http://");
    if(httpPos)
    {
        //Вытаскиваем из строки имя хоста и порт, если есть...
        UINT scou = a_URL.Length();
        int sPosE = 0;
        for(UINT b=httpPos+7; b <= scou; b++)
        {
            if(a_URL[b]!='/'  && a_URL[b]!=' ')
                sPosE++;
            else
            {
                break;
            }
        }
        AnsiString shost = a_URL.SubString(httpPos+7, sPosE);
        AnsiString sreq = a_URL.SubString(httpPos+7+sPosE, a_URL.Length());
        int sep = shost.AnsiPos(":");
        if(sep)
        {
            a_Host = shost.SubString(1, sep-1).Trim();
            AnsiString sPort = shost.SubString(sep+1, shost.Length()).Trim();
            int nPort;
            __try
            {
                nPort = StrToInt(sPort);
            }
            catch(...)
            {
                nPort = a_Port;
            }
            a_Port = nPort;
        }
        else
        {
            a_Host = shost.Trim();
        }
        a_Req = sreq.Trim();
    }
    return false;
}

/*----------------------------------------------------------------------------*/
bool FtURLDecode(AnsiString a_S, BYTE *a_Hash)
{
    int i=0;
    int cou = a_S.Length();
    for(int a=1; a <= cou; a++)
    {
        if( UCHAR(a_S[a]) == UCHAR('%'))
        {
            if( (a+2) <= cou)
            {
                AnsiString sChar = "0x" + AnsiString(a_S[a+1]) + AnsiString(a_S[a+2]);
                char ch = sChar.ToInt();
                a_Hash[i] = ch;
                i++;
                a += 2;
            }
            else
            {
                //Ошибка кодирования строки
                return false;
            }
        }
        else
        {
            a_Hash[i] = a_S[a];
            i++;
        }
    }
    return true;
}

/*----------------------------------------------------------------------------*/
AnsiString FtURLEncode(BYTE *a_S)
{
  AnsiString res;
  for(int i=0; i < 20; i++)
  {
      if (((UCHAR(a_S[i]) >= UCHAR('0')) && (UCHAR(a_S[i]) <= UCHAR('9'))) ||
          ((UCHAR(a_S[i]) >= UCHAR('A')) && (UCHAR(a_S[i]) <= UCHAR('Z'))) ||
          ((UCHAR(a_S[i]) >= UCHAR('a')) && (UCHAR(a_S[i]) <= UCHAR('z'))) ||
          (UCHAR(a_S[i]) == UCHAR('-')) || (UCHAR(a_S[i]) == UCHAR('_')) ||
          (UCHAR(a_S[i]) == UCHAR('.')) || (UCHAR(a_S[i]) == UCHAR('~'))
         )
      {
          res += char(a_S[i]);
      }
      else
      {
        res += '%';
        res += IntToHex((unsigned char)a_S[i], 2).LowerCase();
      }
  }
  return res;
}

/*----------------------------------------------------------------------------*/
AnsiString FtHashEncode20(BYTE *a_S, int a_size)
{
    if(!a_S) return "";
    AnsiString sRes = "";
    for(int a=0; a < a_size; a++)
    {
        sRes += IntToHex(a_S[a], 2);
    }
    return sRes.Trim();
}

//------------------------------------------------------------------------------
bool FtOpenHttpPage(AnsiString a_address)
{
    //Проверяем ключ браузера в реестре...
    TRegistry *reg = new TRegistry(KEY_ALL_ACCESS);
    if(reg)
    {
        //HKEY_CURRENT_USER\Software\Classes\http\shell\open\command
        reg->RootKey = HKEY_CURRENT_USER;
        AnsiString tkey = "Software\\";
        tkey += "Classes\\";
        tkey += "http\\";
        tkey += "shell\\";
        tkey += "open\\command";
        if(reg->OpenKey(tkey, false))
        {
            AnsiString str = reg->ReadString("");
            if(str!="")
            {
                //Считаем, что нашли браузер по умолчанию
                str = AnsiReplaceStr(str, "%1", a_address);
                UINT nRes = WinExec(str.c_str(), SW_SHOWDEFAULT);
                if(nRes < 31)//то была ошибка...
                {
                    /*
                    AnsiString strErr = "";
                    switch(nRes)
                    {
                        case 0: strErr="The system is out of memory or resources."; break;
                        case ERROR_BAD_FORMAT: strErr="The .EXE file is invalid (non-Win32 .EXE or error in .EXE image)."; break;
                        case ERROR_FILE_NOT_FOUND: strErr="The specified file was not found."; break;
                        case ERROR_PATH_NOT_FOUND: strErr="The specified path was not found."; break;
                        default: strErr="Unknown error of start Torrent-client program ("+IntToStr(nRes)+").";
                    }
                    MessageBox(NULL, strErr.c_str(), "Ошибка запуска браузера «по умолчанию».", MB_OK|MB_ICONERROR);
                    */
                    return false;
                }
            }
        }
    }
    return false;
}
