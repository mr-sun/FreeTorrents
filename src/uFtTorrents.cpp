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
#include <stdio.h>
#pragma hdrstop

#include "uFtTorrents.h"

#pragma package(smart_init)

TCriticalSection *g_CSTorrents = new TCriticalSection;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtTorrent
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtTorrent::CFtTorrent()
{
    m_nState = FTTS_STOPPED;
    m_Rule = 0;
    m_Info = 0;
    m_LastLeft = 0;
    m_LastUpload = 0;
    m_LastDownload = 0;    
}
CFtTorrent::~CFtTorrent()
{
    UnInit();
}
//---------------------------------------------------------------------------
bool CFtTorrent::Init(CFtRulesMgr *a_RulesMgr, CFtTorrentInfoMgr *a_TorrentMgr)
{
    UnInit();
    m_Rule = a_RulesMgr->GetRule(m_Tracker, m_sHash20);
    m_Info = a_TorrentMgr->GetTorrentInfo(m_Hash);

    return true;
}
//---------------------------------------------------------------------------
bool CFtTorrent::UnInit(void)
{
    m_Rule = 0;
    m_Info = 0;
    m_bNewData = true;
    return true;
}

//---------------------------------------------------------------------------
//Активны или нет правила?...
bool CFtTorrent::IsActiveRules(void)
{
    if(!m_Rule) return false;
    if(!m_Rule->GetUse()) return false;
    if(!m_Rule->GetUseKDownload() && !m_Rule->GetUseKUpload()) return false;
    if(m_Rule->GetPercentDownload()==100 && !m_Rule->GetUseKUpload()) return false;
    if(m_Rule->GetKUpload()==1 && !m_Rule->GetUseKDownload()) return false;
    if(m_Rule->GetKUpload()==1 && m_Rule->GetPercentDownload()==100) return false;

    return true;
}

//---------------------------------------------------------------------------
//Есть новые данные?...
bool CFtTorrent::IsNewData(void)
{
    if(m_bNewData)
    {
        m_bNewData = false;
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
//Состояние торрента
FtTorrentState CFtTorrent::GetState(void)
{
    return m_nState;
}
//---------------------------------------------------------------------------
bool CFtTorrent::SetState(FtTorrentState a_nState)
{
    m_nState = a_nState;
    return true;
}

//---------------------------------------------------------------------------
bool CFtTorrent::Process(__int64 &a_nUpload, __int64 &a_nDownload, __int64 &a_nLeft, AnsiString &a_sAction)
{
    if(a_nUpload >= 0)
    {
        m_LastUpload = a_nUpload;
        m_LastNewUpload = m_LastUpload;
    }
    if(a_nDownload >= 0)
    {
        m_LastDownload = a_nDownload;
        m_LastNewDownload = m_LastDownload;
    }
    if(a_nLeft >= 0)
    {
        m_LastLeft = a_nLeft;
        m_LastNewLeft = m_LastLeft;
    }
    m_bNewData = true;

    //Если указано состояние, то запоминаем его
    if(a_sAction!="")
    {
        if(a_sAction == "started")
            SetState(FTTS_STARTED);

        if(a_sAction == "stopped")
            SetState(FTTS_STOPPED);

        if(a_sAction == "complited")
        {
            //SetState(FTTS_STOPPED);
            //Закачка завершена...
        }
    }

    if(!m_Rule) return false;
    if(!IsActiveRules()) return false;

    //Определяем длинну передаваемых "кусочков"
    UINT piece = FT_DATAPIECE;

    if(a_nDownload >= 0)
    {
        if(m_Rule->GetUseKDownload())
        {
            __int64 nDownNew = (a_nDownload / 100) * m_Rule->GetPercentDownload();
            /* Модифицируем согласно длине "кусочка" */
            __int64 tmp = (nDownNew / piece);
            nDownNew = tmp * piece;
            a_nDownload = nDownNew;

            //Корректируем Left
            __int64 nLeftNew = (a_nLeft / 100) * m_Rule->GetPercentDownload();
            a_nLeft = nLeftNew;

            //Все скачали - раздаем...
            if(nLeftNew == 0)
            {
               // if(a_sAction!="" && a_sAction !="complited")
               //     a_sAction == "complited";
            }
        }
        m_LastNewDownload = a_nDownload;
    }

    if(a_nUpload >= 0)
    {
        if(m_Rule->GetUseKUpload())
        {
            __int64 nUpNew = (a_nUpload * m_Rule->GetKUpload());
            /* Модифицируем согласно длине "кусочка" */
            __int64 tmp = (nUpNew / piece);
            nUpNew = tmp * piece;
            a_nUpload = nUpNew;
        }
        m_LastNewUpload = a_nUpload;
    }

    //Меняем значение Left в соответсвии с коэффициентами Download...
    //todo
    if(a_nLeft >= 0)
    {
        m_LastNewLeft = a_nLeft;
    }

    return true;
}

//---------------------------------------------------------------------------
//Получаем имя торрента из TorrentInfo
AnsiString CFtTorrent::GetName(void)
{
    if(!m_Info) return "";
    /* В качестве имени торрента возвращаем имя его файла (без расширения). Временно. */
    AnsiString fname = ExtractFileName(m_Info->GetFileName());
    fname = fname.SubString(1, fname.Length()-strlen(".torrent"));
    return fname.Trim();
}

//---------------------------------------------------------------------------
AnsiString CFtTorrent::GetHash(void)
{
    return m_Hash;
}
AnsiString CFtTorrent::GetHash20(void)
{
    return m_sHash20;
}
//---------------------------------------------------------------------------
bool CFtTorrent::SetHash(AnsiString a_Hash)
{
    m_Hash = a_Hash;
    FtURLDecode(m_Hash, m_mHash20);
    m_sHash20 = FtHashEncode20(m_mHash20, 20);
    return true;
}
//---------------------------------------------------------------------------
AnsiString CFtTorrent::GetTracker(void)
{
    return m_Tracker;
}
//---------------------------------------------------------------------------
bool CFtTorrent::SetTracker(AnsiString a_Tracker)
{
    m_Tracker = a_Tracker;
    return true;
}

//---------------------------------------------------------------------------
double CFtTorrent::GetLastUpload(void)
{
    return m_LastUpload;
}
//---------------------------------------------------------------------------
double CFtTorrent::GetLastDownload(void)
{
    return m_LastDownload;
}
//---------------------------------------------------------------------------
double CFtTorrent::GetLastLeft(void)
{
    return m_LastLeft;
}
//---------------------------------------------------------------------------
double CFtTorrent::GetLastNewUpload(void)
{
    return m_LastNewUpload;
}
//---------------------------------------------------------------------------
double CFtTorrent::GetLastNewDownload(void)
{
    return m_LastNewDownload;
}
//---------------------------------------------------------------------------
double CFtTorrent::GetLastNewLeft(void)
{
    return m_LastNewLeft;
}

//---------------------------------------------------------------------------
CFtRule *CFtTorrent::GetRule(void)
{
    return m_Rule;
}
//---------------------------------------------------------------------------
CFtTorrentInfo *CFtTorrent::GetTorrentInfo(void)   
{
    return m_Info;
}




//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtTorrentsMgr
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtTorrentsMgr::CFtTorrentsMgr()
{
    m_TorrentsOpt = 0;
    m_AllUpload = 0;
    m_AllNewUpload = 0;
    m_AllDownload = 0;
    m_AllNewDownload = 0;
}
CFtTorrentsMgr::~CFtTorrentsMgr()
{
}

//---------------------------------------------------------------------------
UINT CFtTorrentsMgr::GetCount(void)
{
    return m_vTorrents.size();
}
//---------------------------------------------------------------------------
CFtTorrent *CFtTorrentsMgr::GetTorrent(UINT a_Index)
{
    if(a_Index >= m_vTorrents.size())
        return 0;
    return &m_vTorrents[a_Index];
}

//---------------------------------------------------------------------------
bool CFtTorrentsMgr::GetAllStatistic(double &a_AllUpload, double &a_AllNewUpload,
                         double &a_AllDownload, double &a_AllNewDownload)
{
    a_AllUpload = m_AllUpload;
    a_AllNewUpload = m_AllNewUpload;
    a_AllDownload = m_AllDownload;
    a_AllNewDownload = m_AllNewDownload;
    return true;
}
//---------------------------------------------------------------------------
CFtTorrent *CFtTorrentsMgr::CreateTorrent(AnsiString a_Hash, AnsiString a_Tracker)
{
    UINT cou = GetCount();
    for(UINT a=0; a < cou; a++)
    {
        CFtTorrent *tr = GetTorrent(a);
        if(!tr) continue;
        if(tr->GetHash() == a_Hash && tr->GetTracker() == a_Tracker)
        {
            //Такой торрент уже есть в списке.. просто возвращаем ссылку на него
            return tr;
        }
    }
    CFtTorrent tmp;
    tmp.SetHash(a_Hash);
    tmp.SetTracker(a_Tracker);
    m_vTorrents.push_back(tmp);
    CFtTorrent *tr = &m_vTorrents[m_vTorrents.size()-1];
    //Обязательно нужно проинициализировать!
    tr->Init(m_RulesMgr, m_TorrentMgr);
    return tr;
}

//---------------------------------------------------------------------------
bool CFtTorrentsMgr::Init(CFtRulesMgr *a_RulesMgr, CFtTorrentInfoMgr *a_TorrentMgr,
                          CFtLogger *a_Logger, CFtTorrentsOpt *a_TorrentsOpt)
{
    bool bRes = true;
    m_RulesMgr = a_RulesMgr;
    m_TorrentMgr = a_TorrentMgr;
    m_Logger = a_Logger;
    m_TorrentsOpt = a_TorrentsOpt;
    UINT cou = m_vTorrents.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vTorrents[a].Init(a_RulesMgr, a_TorrentMgr);
    }
    return bRes;
}
//---------------------------------------------------------------------------
bool CFtTorrentsMgr::UnInit(void)
{
    bool bRes = true;
    UINT cou = m_vTorrents.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vTorrents[a].UnInit();
    }
    return bRes;
}

//---------------------------------------------------------------------------
//tmp
void SaveHshLog(AnsiString a_str)
{
    AnsiString fdir = ExtractFilePath(Application->ExeName) + "\\logs";
    if(!DirectoryExists(fdir))
        ForceDirectories(fdir);
    AnsiString fname = fdir + "\\" + "hash_log.log";
    FILE *fl = fopen(fname.c_str(), "a");
    if(fl)
    {
        a_str += "\r\n\r\n";
        fwrite(a_str.c_str(), a_str.Length(), 1, fl);
        fclose(fl);
    }
}

//---------------------------------------------------------------------------
AnsiString CFtTorrentsMgr::ModifingData(AnsiString a_sData)
{
    AnsiString sDataLogPrev = a_sData;
    AnsiString sLogData = "";
    AnsiString sHash, sHost, sAction;
    __int64 nUpload, nDownload, nLeft;
    //Выделяем инфу о торренте из GET запроса
    bool bRes = GetTorentInfoFromGET(a_sData, sHash, sHost, nUpload, nDownload, nLeft, sAction);

    //Проверяем, не нужно ли проигнорировать запрос на данный (трекер)
    bool bIgnor = false;
    if(m_TorrentsOpt)
    {
        bIgnor = m_TorrentsOpt->IsExcHost(sHost);
    }
    if(!bIgnor) //Отсеиваем retracker.local, и т.п.
    {
        if(bRes)
        {
            //Преобразуем хеш в байт-код
            BYTE mHash[20];
            if( FtURLDecode(sHash, mHash) )
            {
                AnsiString nativHash = sHash;//tmp
                sHash = FtURLEncode(mHash);
                if(sHash != nativHash)       //tmp
                {                            //tmp
                    SaveHshLog(nativHash+"\r\n"+sHash);//tmp
                }                            //tmp

                //Создается новый торрент, или, если с такими параметрами уже существует,
                //то возвращается ссылка именно на него.
                CFtTorrent *tr = CreateTorrent( sHash, sHost);
                if(tr)
                {
                    __int64 nUploadNew = nUpload;
                    __int64 nDownloadNew = nDownload;
                    __int64 nLeftNew = nLeft;
                    AnsiString sActionNew = sAction;
                    tr->Process(nUploadNew, nDownloadNew, nLeftNew, sActionNew);
                    //Устанавливаем новую (преобразованную) инфу в GET запрос
                    a_sData = SetTorrentInfoForGET(a_sData, nUpload, nUploadNew, nDownload,
                                                   nDownloadNew, nLeft, nLeftNew, sAction, sActionNew);

                    CalcAllTraffic();

                    //Логим
                    AnsiString sName = tr->GetName();
                    if(sName=="")sName = tr->GetHash();
                    sName = "«"+sName+"» ";
                    AnsiString sDown = "Down: "+IntToStr(nDownload)+"/"+IntToStr(nDownloadNew)+"; ";
                    AnsiString sUp = "Up: "+IntToStr(nUpload)+"/"+IntToStr(nUploadNew)+"; ";
                    AnsiString sLeft = "Left: "+IntToStr(nLeft)+"/"+IntToStr(nLeftNew)+"; ";
                    AnsiString sAct = "Action: "+sAction+"/"+sActionNew+"; ";
                    sLogData = sName + sDown + sUp + sLeft + sAct;
                }
                else
                {
                    sLogData = "Err>> Не удалось создать торрент.";
                }
            }
        }
        else
        {
            sLogData = "Err>> Не удалось найти в запросе информацию о торренте.";
        }

        AnsiString sDataLogPost = a_sData;

        if(m_Logger)
        {
            m_Logger->AddString("-----------------------------------------------------\r\n"+
                                sLogData+"\r\n--------\r\n**До**\r\n"+sDataLogPrev+
                                "\r\n--------\r\n**После**\r\n"+sDataLogPost+
                                "\r\n-----------------------------------------------------"
                                , "FtTorrent");
        }
    }
    return a_sData;
}

//---------------------------------------------------------------------------
void CFtTorrentsMgr::CalcAllTraffic(void)
{
    m_AllDownload = 0;
    m_AllNewDownload = 0;
    m_AllUpload = 0;
    m_AllNewUpload = 0;
    UINT cou = m_vTorrents.size();
    for(UINT a=0; a < cou; a++)
    {
        m_AllDownload += m_vTorrents[a].GetLastDownload();
        m_AllNewDownload += m_vTorrents[a].GetLastNewDownload();
        m_AllUpload += m_vTorrents[a].GetLastUpload();
        m_AllNewUpload += m_vTorrents[a].GetLastNewUpload();
    }
}

