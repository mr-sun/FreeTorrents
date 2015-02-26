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
#include <inifiles.hpp>
#pragma hdrstop

#include "uFtUpdater.h"
#include "uFtCore.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
CFtUpdater::CFtUpdater()
{
    m_Opt = 0;
    m_OnUpdates = 0;
    m_OnProcessUpd = 0;
    m_bUpdating = false;
    m_Clt = new TIdHTTP(NULL);
    m_Clt->Request->UserAgent = FTUPD_UAGENT;
    m_Timer = new TTimer(NULL);
    m_Timer->Enabled = false;
    m_Timer->OnTimer = OnTimer;
    m_Timer->Interval = 1000;
}
CFtUpdater::~CFtUpdater()
{
    if(m_Clt) delete m_Clt;
    if(m_Timer) delete m_Timer;
}
//---------------------------------------------------------------------------
bool CFtUpdater::Init(CFtOptions *a_Opt)
{
    if(!a_Opt) return false;
    if(!m_Clt) return false;
    m_Opt = a_Opt;
    CFtNetOpt *NetOpt = m_Opt->GetNetOpt();
    if(!NetOpt) return false;

    m_Clt->Request->UserAgent = GetUserAgent();
    if(NetOpt->GetUseProxy() && NetOpt->GetProxyType() == PRTYPE_HTTP)
    {
        m_Clt->Port = NetOpt->GetProxyPort();
        m_Clt->Host = NetOpt->GetProxyHost();
    }
    else
    {
        m_Clt->Port = FTUPD_SERVPORT;
        m_Clt->Host = FTUPD_SERVHOST;
    }
    m_Clt->SocksInfo->Version = svNoSocks;
    if(NetOpt->GetUseProxy() && NetOpt->GetProxyType() != PRTYPE_HTTP)
    {
        m_Clt->SocksInfo->Port = NetOpt->GetProxyPort();
        m_Clt->SocksInfo->Host = NetOpt->GetProxyHost();
        switch( NetOpt->GetProxyType() )
        {
            case PRTYPE_SOCKS4:
            {
                m_Clt->SocksInfo->Version = svSocks4;
                break;
            }
            case PRTYPE_SOCKS4a:
            {
                m_Clt->SocksInfo->Version = svSocks4A;
                break;
            }
            case PRTYPE_SOCKS5:
            {
                m_Clt->SocksInfo->Version = svSocks5;
                break;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool CFtUpdater::UnInit(void)
{
    m_Opt = 0;
    m_Clt->Disconnect();
    return true;
}
//---------------------------------------------------------------------------
AnsiString CFtUpdater::GetUserAgent(void)
{
    AnsiString ua = FTUPD_UAGENT;
    if(!m_Opt) return ua;
    CFtVirtTorrentsOpt *vopt = m_Opt->GetVirtTorrentsOpt();
    if(!vopt) return ua;
    ua += " (";
    ua += "ft"; //Версия FreeTorrents
    if(g_Ver)
    {
        ua += g_Ver->GetVersionStr(FTVT_MAJ_MIN_REL_BUI);
    }
    else
    {
        ua += "unknown";
    }
    ua += "/";
    ua += "vt"+IntToStr(vopt->GetVirtPort());//Порт vt
    ua += ")";

    return ua;
}
//---------------------------------------------------------------------------
//Проверить наличие обновлений
bool CFtUpdater::SerchUpdate(void)
{
    if(!m_Clt) return false;
    if(!m_Opt) return false;
    if(m_bUpdating) return false;

    CFtNetOpt *NetOpt = m_Opt->GetNetOpt();
    if(!NetOpt) return false;

    m_bUpdating = true;
    bool bUseHttpProxy = (NetOpt->GetUseProxy() && NetOpt->GetProxyType() == PRTYPE_HTTP);
    TThreadUpdater *th = new TThreadUpdater(true, FTTH_ACTION_SEARCHUPDATE, bUseHttpProxy, m_Clt, m_Opt->GetUpdaterOpt());
    th->OnTerminate = OnTerminate;
    th->SetOnSearchUpdates( m_OnUpdates );
    th->SetOnProcessUpdates( m_OnProcessUpd );
    th->Resume();
    return true;
}
//---------------------------------------------------------------------------
bool CFtUpdater::Update(void)
{
    if(!m_Clt) return false;
    if(!m_Opt) return false;
    if(m_bUpdating) return false;

    CFtNetOpt *NetOpt = m_Opt->GetNetOpt();
    if(!NetOpt) return false;

    m_bUpdating = true;
    bool bUseHttpProxy = (NetOpt->GetUseProxy() && NetOpt->GetProxyType() == PRTYPE_HTTP);
    TThreadUpdater *th = new TThreadUpdater(true, FTTH_ACTION_UPDATE, bUseHttpProxy, m_Clt, m_Opt->GetUpdaterOpt());
    th->OnTerminate = OnTerminate;
    th->SetOnSearchUpdates( m_OnUpdates );
    th->SetOnProcessUpdates( m_OnProcessUpd );
    th->Resume();
    return true;
}
//---------------------------------------------------------------------------
bool CFtUpdater::UpdateAfterTimeout(UINT a_msec)
{
    if(!m_Timer) return false;
    m_Timer->Interval = a_msec;
    m_Timer->Enabled = true;
    m_OnTimerType = FTUPD_OTT_UPDATE;
    return true;
}
//---------------------------------------------------------------------------
bool CFtUpdater::SearchAfterTimeout(UINT a_msec)
{
    if(!m_Timer) return false;
    m_Timer->Interval = a_msec;
    m_Timer->Enabled = true;
    m_OnTimerType = FTUPD_OTT_SEARCH;
    return true;
}
//---------------------------------------------------------------------------
void __fastcall CFtUpdater::OnTimer(TObject *Sender)
{
    m_Timer->Enabled = false;
    switch(m_OnTimerType)
    {
        case FTUPD_OTT_UPDATE:
        {
            Update();
            break;
        }
        case FTUPD_OTT_SEARCH:
        {
            SerchUpdate();
            break;
        }
    }
}

//---------------------------------------------------------------------------
void __fastcall CFtUpdater::OnTerminate(TObject *Sender)
{
    m_bUpdating = false;
}

//---------------------------------------------------------------------------
//Копируем по нужным местам обновленные файлы
bool CFtUpdater::CopyUpdatingFiles(void)
{
    AnsiString tmpdir = ExtractFilePath(Application->ExeName) + FTUPD_LOCALTMPDIR;
    if(!DirectoryExists(tmpdir)) return false;

    AnsiString info = tmpdir + "\\" + AnsiString(FTUPD_LOCALINFOFILE);
    TIniFile *ini = new TIniFile(info);
    if(ini)
    {
        UINT cou = ini->ReadInteger("Main", "ItemCount", 0);
        for(UINT a=0; a < cou; a++)
        {
            AnsiString Section = "Item_"+IntToStr(a);
            AnsiString name = ini->ReadString(Section, "FileName", "");
            AnsiString ftype = ini->ReadString(Section, "FType", "none");
            AnsiString fname = tmpdir + "\\" + name;
            if(!FileExists(fname)) continue;

            //Копируем его в нужное место..
            if(ftype == "plugin") //Плагин
            {
                //Копируем в папку с плагинами
                AnsiString plgdir = ExtractFilePath(Application->ExeName) + FTUPD_LOCALPLGDIR;
                if(!DirectoryExists(plgdir))
                    ForceDirectories(plgdir);
                AnsiString plgdirname = plgdir + "\\" + name;

                DeleteFile(plgdirname);
                CopyFileTo(fname, plgdirname);
                DeleteFile(fname);
            }

            if(ftype == "ownbin") //Собственный EXE-шник (т.е. сам freetorrents.exe)
            {
                //Текущий exe-шник
                AnsiString exename = Application->ExeName;
                AnsiString backname = Application->ExeName +".back";
                DeleteFile(backname);
            }

            if(ftype == "addbin") //Дополнительный EXE-шник
            {
                //Копируем в папку с бинарниками
                /*
                AnsiString plgdir = ExtractFilePath(Application->ExeName) + FTUPD_LOCALPLGDIR;
                if(!DirectoryExists(plgdir))
                    ForceDirectories(plgdir);
                AnsiString plgdirname = plgdir + "\\" + name;

                DeleteFile(plgdirname);
                CopyFileTo(fname, plgdirname);
                DeleteFile(fname);
                */
            }

        }
        delete ini;
    }
    DeleteFile(info);
    return true;
}

//---------------------------------------------------------------------------
bool CFtUpdater::SetOnSearchUpdates(TOnSearchUpdates a_OnUpdates)
{
    m_OnUpdates = a_OnUpdates;
    return true;
}
bool CFtUpdater::SetOnProcessUpdates(TOnProcessUpdates a_OnProcessUpd)
{
    m_OnProcessUpd = a_OnProcessUpd;
    return true;
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  TThreadUpdater
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
__fastcall TThreadUpdater::TThreadUpdater(bool CreateSuspended, int a_nAction, bool a_bHttpProxy, TIdHTTP *a_Clt, CFtUpdaterOpt *a_UpdOpt)
                        : TThread(CreateSuspended), m_nAction(a_nAction), m_bHttpProxy(a_bHttpProxy)
{
    m_UpdOpt = a_UpdOpt;
    m_Clt = a_Clt;
    m_OnUpdates = 0;
}
//---------------------------------------------------------------------------
void __fastcall TThreadUpdater::Execute()
{
    switch(m_nAction)
    {
        case FTTH_ACTION_SEARCHUPDATE:
        {
            SearchUpdate();        
            break;
        }

        case FTTH_ACTION_UPDATE:
        {
            Update();        
            break;
        }
    }
}

//---------------------------------------------------------------------------
//Проверить наличие обновлений
void TThreadUpdater::SearchUpdate(void)
{
    if(_SearchUpdate())
    {
        OnSearchUpdates(true);
    }
    else
    {
        OnSearchUpdates(false);
    }
}

//---------------------------------------------------------------------------
//Обновить
void TThreadUpdater::Update(void)
{
    if(_SearchUpdate())
    {
        _Update();
    }
    else
    {
    }
}

//---------------------------------------------------------------------------
bool TThreadUpdater::_SearchUpdate(void)
{
    //Скачивайм файл-описатель обновления (ini-файл)
    AnsiString info = ExtractFilePath(Application->ExeName) + FTUPD_LOCALTMPDIR;
    if(!DirectoryExists(info))
        ForceDirectories(info);
    info += "\\" + AnsiString(FTUPD_LOCALINFOFILE);
    if( DownloadFile(info, FTUPD_SERVINFOFILE) )
    {
        //Файл со списком обновлений скачан..
        UINT UpdateNumber = GetUpdateNumber(info);
        if(UpdateNumber > m_UpdOpt->GetLastUpdateNumber())
        {
            //Есть новые обновления...
            return true;
        }
    }
    return false;
}

//---------------------------------------------------------------------------
bool TThreadUpdater::_Update(void)
{
    AnsiString tmpdir = ExtractFilePath(Application->ExeName) + FTUPD_LOCALTMPDIR;
    if(!DirectoryExists(tmpdir))
        ForceDirectories(tmpdir);
    AnsiString info = tmpdir + "\\" + AnsiString(FTUPD_LOCALINFOFILE);

    TIniFile *ini = new TIniFile(info);
    if(ini)
    {
        OnProcessUpdates(0);    
        //bool isUpdated = false;
        g_CSOptions->Enter();
        UINT UpdateNumber = ini->ReadInteger("Main", "UpdateNumber", 0);
        UINT cou = ini->ReadInteger("Main", "ItemCount", 0);
        for(UINT a=0; a < cou; a++)
        {
            AnsiString Section = "Item_"+IntToStr(a);
            AnsiString name = ini->ReadString(Section, "FileName", "");
            AnsiString ftype = ini->ReadString(Section, "FType", "none");
            UINT un = ini->ReadInteger(Section, "UpdateNumber", 0);
            CFtUpdateItem *itm = m_UpdOpt->GetCreateItemByName(name);
            if(itm)
            {
                if(un > itm->m_nLastUPdNumber)
                {
                    AnsiString dwnfname = tmpdir + "\\" + name;
                    //Данный файл нужно обновить...
                    if(DownloadFile(dwnfname, name))
                    {
                        //Если это собственный EXE-шник (т.е. сам freetorrents.exe)
                        if(ftype == "ownbin")
                        {
                            //Текущий exe-шник
                            AnsiString exename = Application->ExeName;
                            AnsiString backname = Application->ExeName +".back";
                            DeleteFile(backname);
                            if(MoveFile(exename.c_str(), backname.c_str())) //Переименовываем текущий
                            {
                                //Перемещаем скаченный файл
                                if(!MoveFile(dwnfname.c_str(), exename.c_str()))
                                {
                                    //Если не удалось переместить, то возвращаем обратно старую версию
                                    MoveFile(backname.c_str(), exename.c_str());
                                }
                            }
                        }

                        //Запоминаем значение последжнего обновления для этого файла
                        itm->m_nLastUPdNumber = un;
                        //isUpdated = true;
                    }
                }
            }
        }
        m_UpdOpt->SetLastUpdateNumber(UpdateNumber);
        g_CSOptions->Leave();
        OnProcessUpdates(100);
        delete ini;
    }
    return true;
}

//---------------------------------------------------------------------------
bool TThreadUpdater::DownloadFile(AnsiString a_sLocalFile, AnsiString a_sFile)
{
    if(!m_Clt) return false;
    AnsiString sGet = "";
    if(m_bHttpProxy)
    {
        sGet += "http://"+AnsiString(FTUPD_SERVHOST)+":"+AnsiString(FTUPD_SERVPORT);
    }
    sGet += AnsiString(FTUPD_SERVPATH) + "/" + AnsiString(a_sFile);

    bool bRes = false;
    TFileStream *fstream = new TFileStream(a_sLocalFile, fmCreate);
    if(fstream)
    {
        try
        {
            m_Clt->Get(sGet, fstream);
            if(m_Clt->ResponseCode == 200)
            {
                //Продолжаем обновление...
                bRes = true;
            }
            m_Clt->Disconnect();
        }
        catch(...)
        {
            m_Clt->Disconnect();
            bRes = false;
        }
        delete fstream;
    }
    return bRes;
}

//---------------------------------------------------------------------------
UINT TThreadUpdater::GetUpdateNumber(AnsiString a_fname)
{
    UINT UpdateNumber = 0;
    TIniFile *ini = new TIniFile(a_fname);
    if(ini)
    {
        UpdateNumber = ini->ReadInteger("Main", "UpdateNumber", 0);
        delete ini;
    }
    return UpdateNumber;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TThreadUpdater::SetOnSearchUpdates(TOnSearchUpdates a_OnUpdates)
{
    m_OnUpdates = a_OnUpdates;
    return true;
}
//---------------------------------------------------------------------------
void TThreadUpdater::OnSearchUpdates(bool a_bNewUpdates)
{
    m_bNewUpdates = a_bNewUpdates;
    Synchronize(SynchOnUpdates);
}
//---------------------------------------------------------------------------
void __fastcall TThreadUpdater::SynchOnUpdates(void)
{
    if(m_OnUpdates)
        m_OnUpdates(m_bNewUpdates);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool TThreadUpdater::SetOnProcessUpdates(TOnProcessUpdates a_OnProcessUpd)
{
    m_OnProcessUpd = a_OnProcessUpd;
    return true;
}
//---------------------------------------------------------------------------
void TThreadUpdater::OnProcessUpdates(UINT a_nPercent)
{
    m_nPercent = a_nPercent;
    Synchronize(SynchOnProcessUpd);
}
//---------------------------------------------------------------------------
void __fastcall TThreadUpdater::SynchOnProcessUpd(void)
{
    if(m_OnProcessUpd)
        m_OnProcessUpd(m_nPercent);
}
