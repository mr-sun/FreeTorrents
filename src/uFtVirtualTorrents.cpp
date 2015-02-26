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

#include "uFtVirtualTorrents.h"
#include "uFtCore.h"

#pragma package(smart_init)

//4 байта кея
BYTE g_nKey1 = rand()+5;
BYTE g_nKey2 = rand()+8;
BYTE g_nKey3 = rand()+14;
BYTE g_nKey4 = rand()+23;

//12 байт пир-ид
BYTE g_nPid1 = rand()+3;
BYTE g_nPid2 = rand()+6;
BYTE g_nPid3 = rand()+8;
BYTE g_nPid4 = rand()+10;
BYTE g_nPid5 = rand()+12;
BYTE g_nPid6 = rand()+14;
BYTE g_nPid7 = rand()+16;
BYTE g_nPid8 = rand()+18;
BYTE g_nPid9 = rand()+20;
BYTE g_nPid10 = rand()+22;
BYTE g_nPid11 = rand()+24;
BYTE g_nPid12 = rand()+26;

//100 байт случайных чисел
BYTE g_nMas[100] = { rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17,
                     rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17,
                     rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17,
                     rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17,
                     rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17,
                     rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17,
                     rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17,
                     rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17,
                     rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17,
                     rand(), rand()+1, rand()+3, rand()+5, rand()+7, rand()+9, rand()+11, rand()+13, rand()+15, rand()+17
                   };

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtVirtualTorrent
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtVirtualTorrent::CFtVirtualTorrent()
{
    m_fUploadSpeed = FT_VIRT_UPLOAD_SPEED;
    m_bNewData = true;
    m_fUpload = 0;
    m_bUsed = true;
    m_State = FT_VIRTST_STOPPED;
    m_NetOpt = 0;
    m_VirtTorrentsOpt = 0;
}
CFtVirtualTorrent::~CFtVirtualTorrent()
{
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrent::Initialize(void)
{
    m_TorrInfo.SetFileName(m_FileName);
    bool bRes = m_TorrInfo.Init();

    //Заполняем свой локальный список треккеров этого торрента
    m_vTrackers.clear();
    UINT cou = m_TorrInfo.GetTrackersCount();
    for(UINT a=0; a < cou; a++)
    {
        TFtTracker *tr = m_TorrInfo.GetTracker(a);
        if(tr)
        {
            m_vTrackers.push_back(*tr);
        }
    }

    //Запоминаем Хэш торрента локально
    m_sHash = m_TorrInfo.GetInfoHash();
    BYTE tmp[20];
    FtURLDecode(m_sHash, tmp);
    m_sHash20 = FtHashEncode20(tmp, 20);

    return bRes;
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrent::Init(CFtNetOpt *a_NetOpt, CFtVirtTorrentsOpt *a_VirtTorrentsOpt)
{
    m_NetOpt = a_NetOpt;
    m_VirtTorrentsOpt = a_VirtTorrentsOpt;
    if(!m_NetOpt) return false;

    if( GetTrackersCount() == 0 || m_sHash=="")
        Initialize();

    m_fUpload = 0;

    return true;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::UnInit(void)
{
    m_TorrInfo.UnInit();
    m_bNewData = true;
    m_NetOpt = 0;
    return true;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::Process(void)
{
    if(m_State == FT_VIRTST_UPLOADED)
    {
        if(m_bNeedStarted)
        {
            //Посылаем старт...
            m_fUpload = 0;            
            m_sEvent = FT_VIRT_EVENT_STARTED;
            SendAnnounce();
            m_bNeedStarted = false;
            return true;
        }

        //Подсчитываем число байт для закачки
        CalculateUpDataSize();
        //если время нового анонса подошло, то
        if(GetTickCount() >= m_LastAnnounceTime + m_nAnnounceTimeout)
        {
            m_LastAnnounceTime = GetTickCount();
            //Посылаем регулярное обновление            
            SendAnnounce();
        }
    }
    return true;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::Start(void)
{
    return SetState(FT_VIRTST_UPLOADED);
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrent::Stop(void)
{
    return SetState(FT_VIRTST_STOPPED);
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::SetState(UINT a_nState)
{
    bool bRes = false;
    switch(a_nState)
    {
        //Запуск
        case FT_VIRTST_UPLOADED:
        {
            if(m_State == FT_VIRTST_STOPPED)
            {
                //Посылаем скреп
                SendScrape();
                m_nAnnounceTimeout = FT_VIRT_ANNOUNCE_TIMEOUT1;
                m_bNeedStarted = true;
                m_fUpload = 0;
                m_bNewData = true;
                m_State = a_nState;
                m_LastAnnounceTime = GetTickCount();
                m_LastCalculateTime = GetTickCount();
                bRes = true;
            }
            break;
        }
        //Останов        
        case FT_VIRTST_STOPPED:
        {
            if(m_State == FT_VIRTST_UPLOADED)
            {
                m_State = a_nState;
                //Посылаем останов
                m_sEvent = FT_VIRT_EVENT_STOPPED;
                SendAnnounce();
                bRes = true;
            }
            break;
        }
    }
    return bRes;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::SendScrape(void)
{
    if(!m_NetOpt) return false;
    if(!m_VirtTorrentsOpt) return false;    
    if(m_sHash=="") return false;
    UINT cou = m_vTrackers.size();
    for(UINT a=0; a < cou; a++)
    {
        if(m_vTrackers[a].m_bUse)
        {
            if(m_vTrackers[a].m_sTracker != "")
            {
                //Проверяем возможность скрепа по данному трекеру
                char *pos = StrPos(m_vTrackers[a].m_sAnnounce.c_str(), "announce");
                //Если announce не встречается, значит скреп невозможен
                if(pos)
                {
                    TIdHTTP *Clt = new TIdHTTP(NULL);
                    if(Clt)
                    {
                        ConfigureHttpClt(Clt, m_vTrackers[a].m_sTracker, m_vTrackers[a].m_nPort);
                        AnsiString sScrape = AnsiReplaceStr(m_vTrackers[a].m_sAnnounce, "announce", "scrape");
                        sScrape += "&info_hash=" + m_sHash;
                        if(m_NetOpt->GetUseProxy() && m_NetOpt->GetProxyType() == PRTYPE_HTTP)
                        {
                            sScrape = "http://" + m_vTrackers[a].m_sTracker + ":" + m_vTrackers[a].m_nPort + sScrape;
                        }
                        AnsiString sRes;
                        try
                        {
                            sRes = Clt->Get(sScrape);
                            if(Clt->ResponseCode == 200)
                            {
                                //Продолжаем обновление...
                                //bRes = true;
                            }
                            Clt->Disconnect();
                        }
                        catch(...)
                        {
                            if(Clt->ResponseCode != 200)
                            {
                                Clt->ResponseText;
                            }
                            Clt->Disconnect();
                        }
                        delete Clt;
                    }
                }
            }
        }
    }
    return true;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::SendAnnounce(void)
{
    if(!m_NetOpt) return false;
    if(!m_VirtTorrentsOpt) return false;    
    if(m_sHash=="") return false;
    TFtClientInfo *clinfo = m_VirtTorrentsOpt->GetCltInfo( m_VirtTorrentsOpt->GetCurrClt() );
    if(!clinfo) return false;
        
    UINT cou = m_vTrackers.size();
    for(UINT a=0; a < cou; a++)
    {
        if(m_vTrackers[a].m_bUse)
        {
            if(m_vTrackers[a].m_sTracker != "")
            {
                TIdHTTP *Clt = new TIdHTTP(NULL);
                if(Clt)
                {
                    ConfigureHttpClt(Clt, m_vTrackers[a].m_sTracker, m_vTrackers[a].m_nPort);
                    AnsiString sMess = m_vTrackers[a].m_sAnnounce;
                    sMess += "&info_hash=" + m_sHash;
                    sMess += "&peer_id=" + GetPeerId();
                    sMess += "&port="+IntToStr(m_VirtTorrentsOpt->GetVirtPort());
                    sMess += "&uploaded="+IntToStr( GetUpload() );
                    sMess += "&downloaded=0&left=0";
                    sMess += "&corrupt=0";
                    sMess += "&key="+GetKey();
                    if(m_sEvent != FT_VIRT_EVENT_NONE)
                        sMess += "&event="+m_sEvent;
                    sMess += "&numwant=200&compact=1&no_peer_id=1";
                    if(m_NetOpt->GetUseProxy() && m_NetOpt->GetProxyType() == PRTYPE_HTTP)
                    {
                        sMess = "http://" + m_vTrackers[a].m_sTracker + ":" + m_vTrackers[a].m_nPort + sMess;
                    }
                    AnsiString sRes;
                    try
                    {
                        sRes = Clt->Get(sMess);
                        if(Clt->ResponseCode == 200)
                        {
                            //Поидее тут мы должны пропарсить ответ...
                            //...
                            m_sEvent = FT_VIRT_EVENT_NONE;
                            m_nAnnounceTimeout = FT_VIRT_ANNOUNCE_TIMEOUT2 + random(10)*60*1000;                        
                        }
                        Clt->Disconnect();
                    }
                    catch(...)
                    {
                        if(Clt->ResponseCode != 200)
                        {
                            Clt->ResponseText;
                        }
                        Clt->Disconnect();
                    }
                    delete Clt;
                }
            }
        }
    }
    return true;
}

//---------------------------------------------------------------------------
//Подсчитываем число байт для закачки
bool CFtVirtualTorrent::CalculateUpDataSize(void)
{
    double tmpUpSpeed = m_fUploadSpeed;
    if(g_Ver)
    {
        if(g_Ver->GetProgrType()==FTTYPE_VER_LIGHT)
        {
            tmpUpSpeed = FT_VIRT_UPLOAD_SPEED;
        }
    }
    //прошедшее время в секундах
    double time = (GetTickCount() - m_LastCalculateTime)/1000.0;
    double bytes = ((tmpUpSpeed*1024)/8) * time;
    m_fUpload += bytes;
    m_bNewData = true;
    m_LastCalculateTime = GetTickCount();
    return true;
}
//---------------------------------------------------------------------------
AnsiString CFtVirtualTorrent::GetPeerId(void)
{
    bool bDef = false;
    if(!m_VirtTorrentsOpt) bDef = true;
    TFtClientInfo *clinfo = m_VirtTorrentsOpt->GetCltInfo( m_VirtTorrentsOpt->GetCurrClt() );
    if(!clinfo) bDef = true;

    //Если строка клиента не найдена, то просто берем 20 байт.
    if(bDef)
    {
        AnsiString str = "-UT1830-";
        for(int a=0; a < 12; a++)
        {
            str += "%"+ IntToHex(g_nMas[a], 2);
        }
        return str;
    }

    AnsiString tmp = clinfo->m_PeerId;
    AnsiString sRes = "";
    int ind = 0;
    //Заменяем в этой последовательности %xx на байт из массива случайных чисел
    for(int a=1; a <= tmp.Length(); a++)
    {
        if(tmp[a]=='%')
        {
            sRes += tmp[a];
            //Впереди еще есть 2 символа?
            if(a+2 <= tmp.Length() )
            {
                if(tmp[a+1]=='x' && tmp[a+2]=='x')
                {
                    sRes += IntToHex(g_nMas[ind], 2);
                    a+=2;
                    ind++;
                }
            }
        }
        else
        {
            sRes += tmp[a];
        }
    }

    return sRes;
}

//---------------------------------------------------------------------------
AnsiString CFtVirtualTorrent::GetKey(void)
{
    AnsiString str = "";
    for(int a=20; a < 24; a++)
    {
        str += IntToHex(g_nMas[a], 2);
    }
    return str;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::ConfigureHttpClt(TIdHTTP *a_Clt, AnsiString a_Host, UINT a_nPort)
{
    if(!m_NetOpt) return false;
    if(!m_VirtTorrentsOpt) return false;

    TFtClientInfo *clinfo = m_VirtTorrentsOpt->GetCltInfo( m_VirtTorrentsOpt->GetCurrClt() );
    if(!clinfo) return false;

    a_Clt->Request->UserAgent = clinfo->m_UserAgent;
    a_Clt->Request->Accept = "";
    a_Clt->Request->AcceptEncoding = "gzip";
    if(m_NetOpt->GetUseProxy() && m_NetOpt->GetProxyType() == PRTYPE_HTTP)
    {
        a_Clt->Port = m_NetOpt->GetProxyPort();
        a_Clt->Host = m_NetOpt->GetProxyHost();
    }
    else
    {
        a_Clt->Port = a_nPort;
        a_Clt->Host = a_Host;
    }
    a_Clt->SocksInfo->Version = svNoSocks;
    if(m_NetOpt->GetUseProxy() && m_NetOpt->GetProxyType() != PRTYPE_HTTP)
    {
        a_Clt->SocksInfo->Port = m_NetOpt->GetProxyPort();
        a_Clt->SocksInfo->Host = m_NetOpt->GetProxyHost();
        switch( m_NetOpt->GetProxyType() )
        {
            case PRTYPE_SOCKS4:
            {
                a_Clt->SocksInfo->Version = svSocks4;
                break;
            }
            case PRTYPE_SOCKS4a:
            {
                a_Clt->SocksInfo->Version = svSocks4A;
                break;
            }
            case PRTYPE_SOCKS5:
            {
                a_Clt->SocksInfo->Version = svSocks5;
                break;
            }
        }
    }
    return true;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::IsNewData(void)
{
    if(m_bNewData)
    {
        m_bNewData = false;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
__int64 CFtVirtualTorrent::GetUpload(void)
{
    //Приводим общее значение накрученного трафика к длинне кусочка piece
    __int64 tmp = (m_fUpload / FT_DATAPIECE);
    __int64 nUpload = tmp * FT_DATAPIECE;
    return nUpload;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::SetFileName(AnsiString a_FileName)
{
    m_FileName = a_FileName;
    return true;
}
AnsiString CFtVirtualTorrent::GetFileName(void)
{
    return m_FileName;
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrent::SetUploadSpeed(double a_fUploadSpeed)
{
    m_fUploadSpeed = a_fUploadSpeed;
    return true;
}
double CFtVirtualTorrent::GetUploadSpeed(void)
{
    return m_fUploadSpeed;
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrent::GetUsed(void)
{
    return m_bUsed;
}
bool CFtVirtualTorrent::SetUsed(bool a_bUsed)
{
    m_bUsed = a_bUsed;
    return true;
}
//---------------------------------------------------------------------------
TFtTracker *CFtVirtualTorrent::GetTracker(UINT a_nIndex)
{
    if(a_nIndex >= m_vTrackers.size())
        return 0;
    return &m_vTrackers[a_nIndex];
}
//---------------------------------------------------------------------------
TFtTracker *CFtVirtualTorrent::GetFirstActTracker(void)
{
    UINT cou = GetTrackersCount();
    for(UINT a=0; a < cou; a++)
    {
        TFtTracker *vt = GetTracker(a);
        if(vt)
        {
            if(vt->m_bUse) return vt;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------
UINT CFtVirtualTorrent::GetTrackersCount(void)
{
    return m_vTrackers.size();
}
//---------------------------------------------------------------------------
AnsiString CFtVirtualTorrent::GetHash(void)
{
    return m_sHash;
}
//---------------------------------------------------------------------------
AnsiString CFtVirtualTorrent::GetHash20(void)
{
    return m_sHash20;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrent::Save(TIniFile *a_ini, AnsiString a_Section)
{
    if(!a_ini) return false;

    a_ini->WriteString(a_Section, "FileName", m_FileName);
    a_ini->WriteFloat(a_Section, "UploadSpeed", m_fUploadSpeed);
    a_ini->WriteBool(a_Section, "Used", m_bUsed);
    a_ini->WriteString(a_Section, "Hash", m_sHash);
    a_ini->WriteString(a_Section, "Hash20", m_sHash20);

    UINT cou = GetTrackersCount();
    a_ini->WriteInteger(a_Section, "TrackersCount", cou);
    for(UINT a=0; a < cou; a++)
    {
        TFtTracker *vt = GetTracker(a);
        if(vt)
        {
            a_ini->WriteBool(a_Section, "Use_"+IntToStr(a), vt->m_bUse);
            a_ini->WriteString(a_Section, "Tracker_"+IntToStr(a), vt->m_sTracker);
            a_ini->WriteString(a_Section, "Announce_"+IntToStr(a), vt->m_sAnnounce);
            a_ini->WriteInteger(a_Section, "Port_"+IntToStr(a), vt->m_nPort);
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrent::Load(TIniFile *a_ini, AnsiString a_Section)
{
    if(!a_ini) return false;

    m_FileName = a_ini->ReadString(a_Section, "FileName", m_FileName);
    m_fUploadSpeed = a_ini->ReadFloat(a_Section, "UploadSpeed", m_fUploadSpeed);
    m_bUsed = a_ini->ReadBool(a_Section, "Used", m_bUsed);
    m_sHash = a_ini->ReadString(a_Section, "Hash", m_sHash);
    m_sHash20 = a_ini->ReadString(a_Section, "Hash20", m_sHash20);                

    m_vTrackers.clear();
    UINT cou = a_ini->ReadInteger(a_Section, "TrackersCount", 0);
    for(UINT a=0; a < cou; a++)
    {
        TFtTracker vt;
        vt.m_bUse = a_ini->ReadBool(a_Section, "Use_"+IntToStr(a), true);
        vt.m_sTracker = a_ini->ReadString(a_Section, "Tracker_"+IntToStr(a), "");
        vt.m_sAnnounce = a_ini->ReadString(a_Section, "Announce_"+IntToStr(a), "");
        vt.m_nPort = a_ini->ReadInteger(a_Section, "Port_"+IntToStr(a), 80);
        m_vTrackers.push_back(vt);
    }
    return true;
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtVirtualTorrentMgr
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtVirtualTorrentMgr::CFtVirtualTorrentMgr()
{
    m_Timer = new TTimer(NULL);
    m_Timer->Enabled = false;
    m_Timer->Interval = 10000;
    m_Timer->OnTimer = OnTimer;
}
CFtVirtualTorrentMgr::~CFtVirtualTorrentMgr()
{
    UnInit();
    if(m_Timer) delete m_Timer;
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrentMgr::Init(CFtNetOpt *a_NetOpt, CFtVirtTorrentsOpt *a_VirtTorrentsOpt)
{
    bool bRes = true;
    UINT cou = GetCount();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vVirtuals[a].Init(a_NetOpt, a_VirtTorrentsOpt);
    }
    Start();
    return bRes;
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrentMgr::UnInit(void)
{
    Stop();
    bool bRes = true;
    UINT cou = GetCount();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vVirtuals[a].UnInit();
    }
    return bRes;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrentMgr::Start(void)
{
    UINT cou = GetCount();
    for(UINT a=0; a < cou; a++)
    {
        if(m_vVirtuals[a].GetUsed())
        {
            m_vVirtuals[a].Start();
        }
    }

    if(!m_Timer) return false;
    m_Timer->Enabled = true;

    return true;
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrentMgr::Stop(void)
{
    if(!m_Timer) return false;
    m_Timer->Enabled = false;

    UINT cou = GetCount();
    for(UINT a=0; a < cou; a++)
    {
        m_vVirtuals[a].Stop();
    }
    return true;
}

//---------------------------------------------------------------------------
void __fastcall CFtVirtualTorrentMgr::OnTimer(TObject *Sender)
{
    UINT cou = GetCount();
    for(UINT a=0; a < cou; a++)
    {
        m_vVirtuals[a].Process();
    }
}

//---------------------------------------------------------------------------
UINT CFtVirtualTorrentMgr::GetCount(void)
{
    if(g_Ver)
    {
        //Пробная версия (разрешаем только 1 Вирт.Торрент)
        if(g_Ver->GetProgrType()==FTTYPE_VER_LIGHT)
        {
            if(m_vVirtuals.size() > 0)
            {
                return 1;
            }
        }
    }
    return m_vVirtuals.size();
}
//---------------------------------------------------------------------------
CFtVirtualTorrent *CFtVirtualTorrentMgr::GetVTorrent(UINT a_Index)
{
    if(a_Index >= m_vVirtuals.size())
        return 0;
    return &m_vVirtuals[a_Index];
}
//---------------------------------------------------------------------------
CFtVirtualTorrent *CFtVirtualTorrentMgr::CreateVTorrent(AnsiString a_FileName)
{
    CFtVirtualTorrent tmp;
    tmp.SetFileName(a_FileName);
    m_vVirtuals.push_back(tmp);
    return &m_vVirtuals[ m_vVirtuals.size()-1 ];
}
//---------------------------------------------------------------------------
CFtVirtualTorrent *CFtVirtualTorrentMgr::CreateVTorrent(CFtVirtualTorrent *a_VirtTot)
{
    CFtVirtualTorrent tmp = *a_VirtTot;
    m_vVirtuals.push_back(tmp);
    return &m_vVirtuals[ m_vVirtuals.size()-1 ];
}
//---------------------------------------------------------------------------
bool CFtVirtualTorrentMgr::DeleteVTorrent(UINT a_Index)
{
    if(a_Index >= m_vVirtuals.size())
        return false;
    m_vVirtuals.erase( m_vVirtuals.begin() + a_Index );
    return true;
}

//---------------------------------------------------------------------------
double CFtVirtualTorrentMgr::GetAllUploadedSize(void)
{
    double fRes = 0;
    UINT cou = GetCount();
    for(UINT a=0; a < cou; a++)
    {
        fRes += m_vVirtuals[a].GetUpload();
    }
    return fRes;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrentMgr::Load(AnsiString a_fname)
{
    m_vVirtuals.clear();
    TIniFile *ini = new TIniFile(a_fname);
    if(!ini) return false;

    AnsiString SectionName = "VirtualTorrents";
    UINT cou = ini->ReadInteger(SectionName, "Count", 0);
    for(UINT a=0; a < cou; a++)
    {
        AnsiString SectName = "VirtualTorrent_"+IntToStr(a);
        CFtVirtualTorrent tmp;
        tmp.Load(ini, SectName);
        m_vVirtuals.push_back(tmp);
    }
    return true;
}

//---------------------------------------------------------------------------
bool CFtVirtualTorrentMgr::Save(AnsiString a_fname)
{
    TIniFile *ini = new TIniFile(a_fname);
    if(!ini) return false;

    AnsiString SectionName = "VirtualTorrents";
    UINT cou = m_vVirtuals.size();
    ini->WriteInteger(SectionName, "Count", cou);
    for(UINT a=0; a < cou; a++)
    {
        AnsiString SectName = "VirtualTorrent_"+IntToStr(a);
        m_vVirtuals[a].Save(ini, SectName);
    }
    return true;
}


