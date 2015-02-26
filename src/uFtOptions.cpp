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

#include "uFtOptions.h"

#pragma package(smart_init)

TCriticalSection *g_CSOptions = new TCriticalSection;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtNetOpt - сетевые настройки
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtNetOpt::CFtNetOpt()
{
    m_nPort = FT_DEFAULT_PORT;
    m_bUseProxy = false;
    m_nProxyType = PRTYPE_HTTP;
    m_nProxyPort = FT_DEFAULT_PROXYPORT;
    m_sProxyHost = FT_DEFAULT_PROXYHOST;
}
CFtNetOpt::~CFtNetOpt()
{
}
//---------------------------------------------------------------------------
int CFtNetOpt::GetPort(void)
{
    return m_nPort;
}
bool CFtNetOpt::SetPort(int a_nPort)
{
    m_nPort = a_nPort;
    return true;
}
//---------------------------------------------------------------------------
bool CFtNetOpt::GetUseProxy(void)
{
    return m_bUseProxy;
}
bool CFtNetOpt::SetUseProxy(bool a_bUseProxy)
{
    m_bUseProxy = a_bUseProxy;
    return true;
}
//---------------------------------------------------------------------------
int CFtNetOpt::GetProxyType(void)
{
    return m_nProxyType;
}
bool CFtNetOpt::SetProxyType(int a_nProxyType)
{
    m_nProxyType = a_nProxyType;
    return true;
}
//---------------------------------------------------------------------------
int CFtNetOpt::GetProxyPort(void)
{
    return m_nProxyPort;
}
bool CFtNetOpt::SetProxyPort(int a_nProxyPort)
{
    m_nProxyPort = a_nProxyPort;
    return true;
}
//---------------------------------------------------------------------------
AnsiString CFtNetOpt::GetProxyHost(void)
{
    return m_sProxyHost;
}
bool CFtNetOpt::SetProxyHost(AnsiString a_sProxyHost)
{
    m_sProxyHost = a_sProxyHost;
    return true;
}

//---------------------------------------------------------------------------
bool CFtNetOpt::Load(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "NetOpt";
    m_nPort = a_ini->ReadInteger(SectName, "Port", FT_DEFAULT_PORT);
    m_bUseProxy = a_ini->ReadBool(SectName, "UseProxy", false);
    m_nProxyType = a_ini->ReadInteger(SectName, "ProxyType", PRTYPE_HTTP);
    m_nProxyPort = a_ini->ReadInteger(SectName, "ProxyPort", FT_DEFAULT_PROXYPORT);
    m_sProxyHost = a_ini->ReadString(SectName, "ProxyHost", FT_DEFAULT_PROXYHOST);

    return true;
}
//---------------------------------------------------------------------------
bool CFtNetOpt::Save(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "NetOpt";
    a_ini->WriteInteger(SectName, "Port", m_nPort);
    a_ini->WriteBool(SectName, "UseProxy", m_bUseProxy);
    a_ini->WriteInteger(SectName, "ProxyType", m_nProxyType);
    a_ini->WriteInteger(SectName, "ProxyPort", m_nProxyPort);
    a_ini->WriteString(SectName, "ProxyHost", m_sProxyHost);

    return true;    
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtTorrentCltOpt - Настройки используемого торрент-клиента
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtTorrentCltOpt::CFtTorrentCltOpt()
{
    m_bAutoStart = false;
    m_bAutoClose = false;
    m_TrntClientFileName = "";
}
CFtTorrentCltOpt::~CFtTorrentCltOpt()
{
}
//---------------------------------------------------------------------------
bool CFtTorrentCltOpt::GetAutoStart(void)
{
    return m_bAutoStart;
}
bool CFtTorrentCltOpt::SetAutoStart(bool a_bAutoStart)
{
    m_bAutoStart = a_bAutoStart;
    return true;
}
//---------------------------------------------------------------------------
bool CFtTorrentCltOpt::GetAutoClose(void)
{
    return m_bAutoClose;
}
bool CFtTorrentCltOpt::SetAutoClose(bool a_bAutoClose)
{
    m_bAutoClose = a_bAutoClose;
    return true;
}
//---------------------------------------------------------------------------
AnsiString CFtTorrentCltOpt::GetTrntClientFileName(void)
{
    return m_TrntClientFileName;
}
bool CFtTorrentCltOpt::SetTrntClientFileName(AnsiString a_TrntClientFileName)
{
    m_TrntClientFileName = a_TrntClientFileName;
    return true;
}

//---------------------------------------------------------------------------
TFtPathsList CFtTorrentCltOpt::GetPaths(void)
{
    return m_vPaths;
}
//---------------------------------------------------------------------------
UINT CFtTorrentCltOpt::GetPathsCount(void)
{
    return m_vPaths.size();
}
//---------------------------------------------------------------------------
AnsiString CFtTorrentCltOpt::GetPath(UINT a_Index)
{
    if(a_Index >= m_vPaths.size())
        return "";
    return m_vPaths[a_Index];
}
//---------------------------------------------------------------------------
bool CFtTorrentCltOpt::DeletePath(UINT a_Index)
{
    if(a_Index >= m_vPaths.size())
        return false;
    m_vPaths.erase( m_vPaths.begin() + a_Index);
    return true;
}
//---------------------------------------------------------------------------
bool CFtTorrentCltOpt::AddPath(AnsiString a_Path)
{
    m_vPaths.push_back(a_Path);
    return true;
}
//---------------------------------------------------------------------------
bool CFtTorrentCltOpt::EditPath(UINT a_Index, AnsiString a_newPath)
{
    if(a_Index >= m_vPaths.size())
        return false;
    m_vPaths[a_Index] = a_newPath;
    return true;
}

//---------------------------------------------------------------------------
bool CFtTorrentCltOpt::Load(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "TorrentCltOpt";
    m_bAutoStart = a_ini->ReadBool(SectName, "AutoStart", false);
    m_bAutoClose = a_ini->ReadBool(SectName, "AutoClose", false);
    m_TrntClientFileName = a_ini->ReadString(SectName, "TrntClientFileName", "");

    m_vPaths.clear();
    UINT cou = a_ini->ReadInteger(SectName, "PathsCount", 0);
    for(UINT a=0; a < cou; a++)
    {
        AnsiString parname = "Paths["+IntToStr(a)+"]";
        AnsiString str = a_ini->ReadString(SectName, parname, "");
        if(str!="")
        {
            m_vPaths.push_back(str);
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool CFtTorrentCltOpt::Save(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "TorrentCltOpt";
    a_ini->WriteBool(SectName, "AutoStart", m_bAutoStart);
    a_ini->WriteBool(SectName, "AutoClose", m_bAutoClose);
    a_ini->WriteString(SectName, "TrntClientFileName", m_TrntClientFileName);

    UINT cou = m_vPaths.size();
    a_ini->WriteInteger(SectName, "PathsCount", cou);
    for(UINT a=0; a < cou; a++)
    {
        AnsiString parname = "Paths["+IntToStr(a)+"]";
        a_ini->WriteString(SectName, parname, m_vPaths[a]);
    }
    return true;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtTorrentsOpt - Настройки для торрентов..
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtTorrentsOpt::CFtTorrentsOpt()
{
    CreateDefaultExcTrackers();
}
CFtTorrentsOpt::~CFtTorrentsOpt()
{
}
//---------------------------------------------------------------------------
void CFtTorrentsOpt::CreateDefaultExcTrackers(void)
{
    if(m_vExcTrackers.size() > 0) return;
    m_vExcTrackers.push_back("retracker.local");
}
//---------------------------------------------------------------------------
UINT CFtTorrentsOpt::GetExcTrackersCount(void)
{
    return m_vExcTrackers.size();
}
//---------------------------------------------------------------------------
AnsiString CFtTorrentsOpt::GetExcTracker(UINT a_nIndex)
{
    if(a_nIndex >= m_vExcTrackers.size())
        return "";
    return m_vExcTrackers[a_nIndex];
}
//---------------------------------------------------------------------------
bool CFtTorrentsOpt::DeleteExcTracker(UINT a_nIndex)
{
    if(a_nIndex >= m_vExcTrackers.size())
        return false;
    m_vExcTrackers.erase( m_vExcTrackers.begin() + a_nIndex);
    return true;
}
//---------------------------------------------------------------------------
bool CFtTorrentsOpt::AddExcTracker(AnsiString a_Host)
{
    if(a_Host=="") return false;
    m_vExcTrackers.push_back(a_Host);
    return true;
}
//---------------------------------------------------------------------------
bool CFtTorrentsOpt::EditExcTracker(UINT a_nIndex, AnsiString a_Host)
{
    if(a_nIndex >= m_vExcTrackers.size())
        return false;
    m_vExcTrackers[a_nIndex] = a_Host;
    return true;
}
//---------------------------------------------------------------------------
//Проверяем не является ли Хост исключением
bool CFtTorrentsOpt::IsExcHost(AnsiString a_str)
{
    for(UINT a=0; a < m_vExcTrackers.size(); a++)
    {
        if(m_vExcTrackers[a] == a_str)
        {
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool CFtTorrentsOpt::Load(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "TorrentsOpt";
    m_vExcTrackers.clear();
    UINT cou = a_ini->ReadInteger(SectName, "ExcTrackersCount", 0);
    for(UINT a=0; a < cou; a++)
    {
        AnsiString parname = "ExcTracker["+IntToStr(a)+"]";
        AnsiString str = a_ini->ReadString(SectName, parname, "");
        if(str!="")
        {
            m_vExcTrackers.push_back(str);
        }
    }
    CreateDefaultExcTrackers();    
    return true;
}
//---------------------------------------------------------------------------
bool CFtTorrentsOpt::Save(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "TorrentsOpt";
    UINT cou = m_vExcTrackers.size();
    a_ini->WriteInteger(SectName, "ExcTrackersCount", cou);
    for(UINT a=0; a < cou; a++)
    {
        AnsiString parname = "ExcTracker["+IntToStr(a)+"]";
        a_ini->WriteString(SectName, parname, m_vExcTrackers[a]);
    }
    return true;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtVirtTorrentsOpt - Настройки для Виртуальных торрентов..
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtVirtTorrentsOpt::CFtVirtTorrentsOpt()
{
    //Первоначально инициализируем порт случайным числом
    //из диапазона 15000 - 15999
    int n999 = random(999);
    m_nVirtPort = 15000+n999;
    m_nCurrentCltIndex = 0;
    CreateCientsList();    
}
CFtVirtTorrentsOpt::~CFtVirtTorrentsOpt()
{
}
//---------------------------------------------------------------------------
//Список поддерживаемых торрент-клиентов
void CFtVirtTorrentsOpt::CreateCientsList(void)
{
    m_vCltInfo.clear();

    TFtClientInfo clt;
    clt.m_Name = "uTorrent 1.8.3";
    clt.m_UserAgent = "uTorrent/1830(15772)";
    clt.m_PeerId = "-UT1830-%xx%xx%xx%xx%xx%xx%xx%xx%xx%xx%xx%xx";
    m_vCltInfo.push_back(clt);
}
//---------------------------------------------------------------------------
UINT CFtVirtTorrentsOpt::GetVirtPort(void)
{
    return m_nVirtPort;
}
bool CFtVirtTorrentsOpt::SetVirtPort(UINT a_nVirtPort)
{
    m_nVirtPort = a_nVirtPort;
    return true;
}
//---------------------------------------------------------------------------
UINT CFtVirtTorrentsOpt::GetCltInfoCount(void)
{
    return m_vCltInfo.size();
}
//---------------------------------------------------------------------------
TFtClientInfo *CFtVirtTorrentsOpt::GetCltInfo(UINT a_nIndex)
{
    if(a_nIndex >= m_vCltInfo.size())
        return 0;
    return &m_vCltInfo[a_nIndex];
}
//---------------------------------------------------------------------------
UINT CFtVirtTorrentsOpt::GetCurrClt(void)
{
    if(m_nCurrentCltIndex >= m_vCltInfo.size())
        return 0;
    return m_nCurrentCltIndex;
}
//---------------------------------------------------------------------------
bool CFtVirtTorrentsOpt::SetCurrClt(UINT a_nCurrentCltIndex)
{
    m_nCurrentCltIndex = a_nCurrentCltIndex;
    return true;
}

//---------------------------------------------------------------------------
bool CFtVirtTorrentsOpt::Load(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "VirtTorrentsOpt";
    m_nVirtPort = a_ini->ReadInteger(SectName, "VirtPort", m_nVirtPort);
    m_nCurrentCltIndex = a_ini->ReadInteger(SectName, "CurrentCltIndex", m_nCurrentCltIndex);

    return true;
}
//---------------------------------------------------------------------------
bool CFtVirtTorrentsOpt::Save(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "VirtTorrentsOpt";
    a_ini->WriteInteger(SectName, "VirtPort", m_nVirtPort);
    a_ini->WriteInteger(SectName, "CurrentCltIndex", m_nCurrentCltIndex);    

    return true;
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtUpdaterOpt - Обнвления
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtUpdaterOpt::CFtUpdaterOpt()
{
    m_nLastUpdateNumber = 0;
}
CFtUpdaterOpt::~CFtUpdaterOpt()
{
}
//---------------------------------------------------------------------------
UINT CFtUpdaterOpt::GetLastUpdateNumber(void)
{
    return m_nLastUpdateNumber;
}
bool CFtUpdaterOpt::SetLastUpdateNumber(UINT a_nLastUpdateNumber)
{
    m_nLastUpdateNumber = a_nLastUpdateNumber;
    return true;
}
//---------------------------------------------------------------------------
UINT CFtUpdaterOpt::GetCountItems(void)
{
    return m_vItems.size();
}
//---------------------------------------------------------------------------
CFtUpdateItem *CFtUpdaterOpt::GetItem(UINT a_nIndex)
{
    if(a_nIndex >= m_vItems.size())
        return 0;
    return &m_vItems[a_nIndex];
}
//---------------------------------------------------------------------------
CFtUpdateItem *CFtUpdaterOpt::AddItem(void)
{
    CFtUpdateItem tmp;
    tmp.m_nLastUPdNumber = 0;
    m_vItems.push_back(tmp);
    return &m_vItems[ m_vItems.size() - 1 ];
}
//---------------------------------------------------------------------------
CFtUpdateItem *CFtUpdaterOpt::GetCreateItemByName(AnsiString a_sName)
{
    UINT cou = m_vItems.size();
    for(UINT a=0; a < cou; a++)
    {
        if(m_vItems[a].m_sName == a_sName)
            return &m_vItems[a];
    }
    CFtUpdateItem tmp;
    tmp.m_sName = a_sName;
    tmp.m_nLastUPdNumber = 0;
    m_vItems.push_back(tmp);
    return &m_vItems[ m_vItems.size() - 1 ];
}
//---------------------------------------------------------------------------
bool CFtUpdaterOpt::Load(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "UpdaterOpt";
    m_nLastUpdateNumber = a_ini->ReadInteger(SectName, "LastUpdateNumber", m_nLastUpdateNumber);

    UINT cou = a_ini->ReadInteger(SectName, "ItemsCount", 0);
    for(UINT a=0; a < cou; a++)
    {
        SectName = "UpdateItem_"+IntToStr(a);
        AnsiString name = a_ini->ReadString(SectName, "Name", "");
        UINT lun = a_ini->ReadInteger(SectName, "LastUPdNumber", 0);
        CFtUpdateItem *itm = GetCreateItemByName(name);
        if(itm)
        {
            itm->m_nLastUPdNumber = lun;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool CFtUpdaterOpt::Save(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "UpdaterOpt";
    a_ini->WriteInteger(SectName, "LastUpdateNumber", m_nLastUpdateNumber);

    UINT cou = m_vItems.size();
    a_ini->WriteInteger(SectName, "ItemsCount", cou);
    for(UINT a=0; a < cou; a++)
    {
        SectName = "UpdateItem_"+IntToStr(a);
        a_ini->WriteString(SectName, "Name", m_vItems[a].m_sName);
        a_ini->WriteInteger(SectName, "LastUPdNumber", m_vItems[a].m_nLastUPdNumber);        
    }
    return true;
}




//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtOtherOpt - Прочие настройки...
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtOtherOpt::CFtOtherOpt()
{
    m_bStartWithWin = true;
    m_bStartMinimized = true;
    m_bCloseToTray = true;
    m_bSearchUpdate = true;
    m_bFTUpdateAsk = true;
    m_bLogToFile = false;
}
CFtOtherOpt::~CFtOtherOpt()
{
}
//---------------------------------------------------------------------------
bool CFtOtherOpt::GetStartWithWin(void)
{
    return m_bStartWithWin;
}
bool CFtOtherOpt::SetStartWithWin(bool a_bStartWithWin)
{
    m_bStartWithWin = a_bStartWithWin;
    return true;
}
//---------------------------------------------------------------------------
bool CFtOtherOpt::GetStartMinimized(void)
{
    return m_bStartMinimized;
}
bool CFtOtherOpt::SetStartMinimized(bool a_bStartMinimized)
{
    m_bStartMinimized = a_bStartMinimized;
    return true;
}
//---------------------------------------------------------------------------
bool CFtOtherOpt::GetCloseToTray(void)
{
    return m_bCloseToTray;
}
bool CFtOtherOpt::SetCloseToTray(bool a_bCloseToTray)
{
    m_bCloseToTray = a_bCloseToTray;
    return true;
}
//---------------------------------------------------------------------------
bool CFtOtherOpt::GetSearchUpdate(void)
{
    return m_bSearchUpdate;
}
bool CFtOtherOpt::SetSearchUpdate(bool a_bSearchUpdate)
{
    m_bSearchUpdate = a_bSearchUpdate;
    return true;
}
//---------------------------------------------------------------------------
bool CFtOtherOpt::GetFTUpdateAsk(void)
{
    return m_bFTUpdateAsk;
}
bool CFtOtherOpt::SetFTUpdateAsk(bool a_bFTUpdateAsk)
{
    m_bFTUpdateAsk = a_bFTUpdateAsk;
    return true;
}
//---------------------------------------------------------------------------
bool CFtOtherOpt::GetLogToFile(void)
{
    return m_bLogToFile;
}
bool CFtOtherOpt::SetLogToFile(bool a_bLogToFile)
{
    m_bLogToFile = a_bLogToFile;
    return true;
}

//---------------------------------------------------------------------------
bool CFtOtherOpt::Load(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "OtherOpt";
    m_bStartWithWin = a_ini->ReadBool(SectName, "StartWithWin", m_bStartWithWin);
    m_bStartMinimized = a_ini->ReadBool(SectName, "StartMinimized", m_bStartMinimized);
    m_bCloseToTray = a_ini->ReadBool(SectName, "CloseToTray", m_bCloseToTray);
    m_bSearchUpdate = a_ini->ReadBool(SectName, "SearchUpdate", m_bSearchUpdate);
    m_bFTUpdateAsk = a_ini->ReadBool(SectName, "FTUpdateAsk", m_bFTUpdateAsk);
    m_bLogToFile = a_ini->ReadBool(SectName, "LogToFile", m_bLogToFile);
    return true;
}
//---------------------------------------------------------------------------
bool CFtOtherOpt::Save(TIniFile *a_ini)
{
    if(!a_ini) return false;

    AnsiString SectName = "OtherOpt";
    a_ini->WriteBool(SectName, "StartWithWin", m_bStartWithWin);
    a_ini->WriteBool(SectName, "StartMinimized", m_bStartMinimized);
    a_ini->WriteBool(SectName, "CloseToTray", m_bCloseToTray);
    a_ini->WriteBool(SectName, "SearchUpdate", m_bSearchUpdate);
    a_ini->WriteBool(SectName, "FTUpdateAsk", m_bFTUpdateAsk);
    //a_ini->WriteBool(SectName, "LogToFile", m_bLogToFile);
    return true;
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtViewOpt - Настройки отображения
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtViewOpt::CFtViewOpt()
{
}
CFtViewOpt::~CFtViewOpt()
{
}
//---------------------------------------------------------------------------
CFtViewFormStruct *CFtViewOpt::GetViewFormStruct(void)
{
    return &m_ViewForm;
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtOptions - Все настройки программы
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtOptions::CFtOptions()
{
}
CFtOptions::~CFtOptions()
{
}
//---------------------------------------------------------------------------
CFtNetOpt *CFtOptions::GetNetOpt(void)
{
    return &m_NetOpt;
}
//---------------------------------------------------------------------------
CFtTorrentCltOpt *CFtOptions::GetTorrentCltOpt(void)
{
    return &m_TorrentCltOpt;
}
//---------------------------------------------------------------------------
CFtTorrentsOpt *CFtOptions::GetTorrentsOpt(void)
{
    return &m_TorrentsOpt;
}
CFtVirtTorrentsOpt *CFtOptions::GetVirtTorrentsOpt(void)
{
    return &m_VirtTorrentsOpt;
}
//---------------------------------------------------------------------------
CFtOtherOpt *CFtOptions::GetOtherOpt(void)
{
    return &m_OtherOpt;
}
//---------------------------------------------------------------------------
CFtViewOpt *CFtOptions::GetViewOpt(void)
{
    return &m_ViewOpt;
}
//---------------------------------------------------------------------------
CFtUpdaterOpt *CFtOptions::GetUpdaterOpt(void)
{
    return &m_UpdaterOpt;
}
//---------------------------------------------------------------------------
bool CFtOptions::Load(AnsiString a_fname)
{
    TIniFile *ini = new TIniFile(a_fname);
    if(!ini) return false;
    bool bRes = true;

    bRes &= m_NetOpt.Load(ini);
    bRes &= m_TorrentCltOpt.Load(ini);
    bRes &= m_TorrentsOpt.Load(ini);
    bRes &= m_VirtTorrentsOpt.Load(ini);
    bRes &= m_OtherOpt.Load(ini);
    bRes &= m_UpdaterOpt.Load(ini);

    delete ini;
    return bRes;
}
//---------------------------------------------------------------------------
bool CFtOptions::Save(AnsiString a_fname)
{
    TIniFile *ini = new TIniFile(a_fname);
    if(!ini) return false;
    bool bRes = true;

    bRes &= m_NetOpt.Save(ini);
    bRes &= m_TorrentCltOpt.Save(ini);
    bRes &= m_TorrentsOpt.Save(ini);
    bRes &= m_VirtTorrentsOpt.Save(ini);    
    bRes &= m_OtherOpt.Save(ini);
    bRes &= m_UpdaterOpt.Save(ini);    

    delete ini;
    return bRes;
}

