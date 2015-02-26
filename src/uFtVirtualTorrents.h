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

#ifndef uFtVirtualTorrentsH
#define uFtVirtualTorrentsH

#include <ExtCtrls.hpp>
#include <StrUtils.hpp>
#include <IdHTTP.hpp>
#include <inifiles.hpp>
#include <vector.h>

#include "uFtOptions.h"
#include "uFtTorrentInfo.h"
#include "uFtTorrents.h"

#define FT_VIRT_UPLOAD_SPEED  0.1 //Скорость отдачи (в Кб/сек)
#define FT_VIRT_ANNOUNCE_TIMEOUT1 60000   // 1 мин. в милисекундах
#define FT_VIRT_ANNOUNCE_TIMEOUT2 2100000  // 35 мин. в милисекундах

//Состояния работы виртуального торрента.
#define FT_VIRTST_STOPPED   0 //Остановлен
#define FT_VIRTST_UPLOADED  1 //Отдается

//Events
#define FT_VIRT_EVENT_NONE     ""
#define FT_VIRT_EVENT_STARTED  "started"
#define FT_VIRT_EVENT_STOPPED  "stopped"
#define FT_VIRT_EVENT_COMPLIT  "complited"

//---------------------------------------------------------------------------
class CFtVirtualTorrent
{
private:
    AnsiString m_FileName; //Имя торрент-файла
    double m_fUploadSpeed; //Скорость раздачи 
    bool m_bUsed;
    CFtTorrentInfo m_TorrInfo;
    TFtTrackerList m_vTrackers;
    AnsiString m_sHash;
    AnsiString m_sHash20;

    bool m_bNewData;
    __int64 m_fUpload;

    //Опции
    CFtNetOpt *m_NetOpt;  //сетевые
    CFtVirtTorrentsOpt *m_VirtTorrentsOpt; //вирт.торрента

    //Отвечающие за процесс работы торрента
    UINT m_State;
    UINT m_LastAnnounceTime; //время последнего обновления...
    UINT m_LastCalculateTime; //время последнего калькулирования трафика
    bool m_bNeedStarted; //Нужен пакетик "стартед"

    AnsiString m_sEvent;
    UINT m_nAnnounceTimeout;

public:
    CFtVirtualTorrent();
    ~CFtVirtualTorrent();

    CFtVirtualTorrent & operator = ( const CFtVirtualTorrent & v)
    {
        m_FileName = v.m_FileName;
        m_fUploadSpeed = v.m_fUploadSpeed;
        m_bNewData = v.m_bNewData;
        m_bUsed = v.m_bUsed;
        m_TorrInfo = v.m_TorrInfo;
        m_vTrackers = v.m_vTrackers;
        m_fUpload = 0;
        m_sHash = v.m_sHash;
        m_sHash20 = v.m_sHash20;

        m_State = FT_VIRTST_STOPPED;
        m_LastAnnounceTime = 0;
        m_LastCalculateTime = 0;
        m_NetOpt = v.m_NetOpt;
        m_VirtTorrentsOpt = v.m_VirtTorrentsOpt;
        return *this;
    }

public:
    bool SetFileName(AnsiString a_FileName);
    AnsiString GetFileName(void);
    bool SetUploadSpeed(double a_fUploadSpeed);
    double GetUploadSpeed(void);
    bool GetUsed(void);
    bool SetUsed(bool a_bUsed);

    UINT GetTrackersCount(void);
    TFtTracker *GetTracker(UINT a_nIndex);
    TFtTracker *GetFirstActTracker(void);

    AnsiString GetHash(void);
    AnsiString GetHash20(void);    
    bool IsNewData(void);
    __int64 GetUpload(void);

public:
    bool Initialize(void);
    bool Init(CFtNetOpt *a_NetOpt, CFtVirtTorrentsOpt *a_VirtTorrentsOpt);
    bool UnInit(void);
    bool Process(void);

    bool Save(TIniFile *a_ini, AnsiString a_Section);
    bool Load(TIniFile *a_ini, AnsiString a_Section);

public:  //Ф-ции отвечающие за процесс отдачи...
    bool Start(void);
    bool Stop(void);

private:
    bool SetState(UINT a_nState);
    bool SendScrape(void);
    bool SendAnnounce(void);
    //Подсчитываем число байт для закачки
    bool CalculateUpDataSize(void);
    bool ConfigureHttpClt(TIdHTTP *a_Clt, AnsiString a_Host, UINT a_nPort);
    AnsiString GetPeerId(void);
    AnsiString GetKey(void);    
};

//---------------------------------------------------------------------------
class CFtVirtualTorrentMgr
{
private:
    vector<CFtVirtualTorrent> m_vVirtuals;
    TTimer *m_Timer;

public:
    CFtVirtualTorrentMgr();
    ~CFtVirtualTorrentMgr();

    CFtVirtualTorrentMgr & operator = ( const CFtVirtualTorrentMgr & v)
    {
        m_vVirtuals = v.m_vVirtuals;
        return *this;
    }

public:
    bool Init(CFtNetOpt *a_NetOpt, CFtVirtTorrentsOpt *a_VirtTorrentsOpt);
    bool UnInit(void);

    bool Start(void);
    bool Stop(void);

    bool Load(AnsiString a_fname);
    bool Save(AnsiString a_fname);    

    UINT GetCount(void);
    CFtVirtualTorrent *GetVTorrent(UINT a_Index);
    CFtVirtualTorrent *CreateVTorrent(AnsiString a_FileName);
    CFtVirtualTorrent *CreateVTorrent(CFtVirtualTorrent *a_VirtTot);    
    bool DeleteVTorrent(UINT a_Index);    

    double GetAllUploadedSize(void);

private:
    void __fastcall OnTimer(TObject *Sender);

};

//---------------------------------------------------------------------------
#endif
