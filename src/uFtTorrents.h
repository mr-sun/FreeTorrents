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

#ifndef uFtTorrentsH
#define uFtTorrentsH

#include <SyncObjs.hpp>
#include <vector.h>

#include "uFtLogger.h"
#include "uFtFunctions.h"
#include "uFtRules.h"
#include "uFtTorrentInfo.h"
#include "uFtOptions.h"

//Action
#define FT_ACTION_NONE  0 //нет никакого действия

/* DATAPIECE - Кусок передаваемых данных 16 KB.
  Этот кусок не имеет ничего общего с "кусочками" из торрент-файла.
  Данный кусок (16КБ) это минимальный объем данных которыми обмениваются
  участники сети (например сидер с личером).
  В связи с этим в статистике все передаваемые значения Downloaded и Uploaded
  обязательно должны быть всегда (в 99%) случаев кратны этому значению (16КБ.)
  */
#define FT_DATAPIECE  16384 

enum FtTorrentState
{
    FTTS_STOPPED=10,   //Остановлен
    FTTS_STARTED       //Запущен
};

//---------------------------------------------------------------------------
class CFtTorrent
{
private:
    AnsiString m_Hash;     //хеш
    AnsiString m_Tracker;  //Трекер
    BYTE m_mHash20[20];    //Байтовый хеш (20 байт)
    AnsiString m_sHash20;  //Байтовый хеш (20 байт)

    //Реальные значения (переданные от торрент-клиента)
    double m_LastUpload;    //
    double m_LastDownload;  //
    double m_LastLeft;      //
    //Измененные значения (переданные трекеру)
    double m_LastNewUpload;    //
    double m_LastNewDownload;  //
    double m_LastNewLeft;      //


    CFtRule *m_Rule;  //Правило для данного торрента
    CFtTorrentInfo *m_Info; //Ссылка на инфо для данного торрента

    FtTorrentState m_nState; //Состояние торрента (запущен, отключен...) FtTorrentState

    bool m_bNewData; //Флаг наличия новых данных (для отображения)

public:
    CFtTorrent();
    ~CFtTorrent();

    CFtTorrent & operator = ( const CFtTorrent & v)
    {
        m_Hash = v.m_Hash;
        m_sHash20 = v.m_sHash20;
        memcpy(m_mHash20, v.m_mHash20, 20);
        m_Tracker = v.m_Tracker;
        m_LastUpload = v.m_LastUpload;
        m_LastDownload = v.m_LastDownload;
        m_LastLeft = v.m_LastLeft;
        m_LastNewUpload = v.m_LastNewUpload;
        m_LastNewDownload = v.m_LastNewDownload;
        m_LastNewLeft = v.m_LastNewLeft;
        m_nState = v.m_nState;
        m_bNewData = v.m_bNewData;
        m_Rule = 0;
        m_Info = 0;
        return *this;
    }

public:
    bool Init(CFtRulesMgr *a_RulesMgr, CFtTorrentInfoMgr *a_TorrentMgr);
    bool UnInit(void);

    bool Process(__int64 &a_nUpload, __int64 &a_nDownload, __int64 &a_nLeft, AnsiString &a_sAction);

    //Активны или нет правила?...
    bool IsActiveRules(void);

    //Состояние торрента
    FtTorrentState GetState(void);

    //Есть новые данные?...
    bool IsNewData(void);

    //Получаем имя торрента из TorrentInfo
    AnsiString GetName(void);

    AnsiString GetHash(void);
    AnsiString GetHash20(void);
    bool SetHash(AnsiString a_Hash);
    AnsiString GetTracker(void);
    bool SetTracker(AnsiString a_Tracker);

    double GetLastUpload(void);
    double GetLastDownload(void);
    double GetLastLeft(void);

    double GetLastNewUpload(void);
    double GetLastNewDownload(void);
    double GetLastNewLeft(void);

    CFtRule *GetRule(void);
    CFtTorrentInfo *GetTorrentInfo(void);

private:
    bool SetState(FtTorrentState a_nState);
};

//---------------------------------------------------------------------------
class CFtTorrentsMgr
{
private:
    vector<CFtTorrent> m_vTorrents;
    CFtRulesMgr *m_RulesMgr;
    CFtTorrentInfoMgr *m_TorrentMgr;
    CFtLogger *m_Logger;
    CFtTorrentsOpt *m_TorrentsOpt;

    double m_AllUpload;
    double m_AllNewUpload;
    double m_AllDownload;
    double m_AllNewDownload;

public:
    CFtTorrentsMgr();
    ~CFtTorrentsMgr();

public:
    UINT GetCount(void);
    CFtTorrent *GetTorrent(UINT a_Index);
    CFtTorrent *CreateTorrent(AnsiString a_Hash, AnsiString a_Tracker);

    bool GetAllStatistic(double &a_AllUpload, double &a_AllNewUpload,
                         double &a_AllDownload, double &a_AllNewDownload);

    bool Init(CFtRulesMgr *a_RulesMgr, CFtTorrentInfoMgr *a_TorrentMgr,
              CFtLogger *a_Logger, CFtTorrentsOpt *a_TorrentsOpt);
    bool UnInit(void);

    AnsiString ModifingData(AnsiString a_sData);

private:
    void CalcAllTraffic(void);

};

//---------------------------------------------------------------------------
extern TCriticalSection *g_CSTorrents;
//---------------------------------------------------------------------------

#endif
