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

#ifndef uFtCoreH
#define uFtCoreH

#include "uFtVersion.h"
#include "uFtLogger.h"
#include "uFtOptions.h"
#include "uFtServer.h"
#include "uFtRules.h"
#include "uFtTorrents.h"
#include "uFtTorrentInfo.h"
#include "uFtVirtualTorrents.h"
#include "uFtModuls.h"
#include "uFtUpdater.h"

#define FT_STARTUPDATE_TIMEOUT  30000 //30 сек

#define FT_DEFAULT_OPT_FNAME     "ft_default.cfg" //Файл настроек по умолчанию
#define FT_DEFAULT_RULES_FNAME   "ft_default_rules.ftr" //Файл правил по умолчанию
#define FT_DEFAULT_VIRTUAL_FNAME   "ft_default_virtuals.ftv" //Файл виртуальных торрентов

#define FT_REGKEYVAL  "FreeTorrents"

//Флаги инициализации и деинициализации
enum
{
    FT_IL_TORRENTS  = 0x0001, //для спсика торрентов
    FT_IL_VTORRENTS = 0x0002, //для списка виртуальных торрентов
    FT_IL_UPDATER   = 0x0004, //для обновлялки
    FT_IL_SERVER    = 0x0008, //для сервера
    FT_IL_LOGGER    = 0x0010, //для логгера

    FT_IL_ALL = FT_IL_TORRENTS |   //Все
                FT_IL_VTORRENTS |
                FT_IL_UPDATER |
                FT_IL_SERVER |
                FT_IL_LOGGER
};

//---------------------------------------------------------------------------
class CFtCore
{
private:
    TFtVersion m_Ver; //Версия ПО
    CFtOptions m_Opt;
    CFtRulesMgr m_RulesMgr;
    CFtTorrentInfoMgr m_TorrentInfoMgr;
    CFtServer m_Server;
    CFtTorrentsMgr m_TorrnetsMgr;
    CFtVirtualTorrentMgr m_VTorrentsMgr;
    CFtSrvLogger m_SrvLogger;
    CFtModulsMgr m_Moduls; //Менеджер Плагинов
    CFtUpdater m_Updater;


public:
    CFtCore();
    ~CFtCore();

    bool Init(WORD a_Flags=FT_IL_ALL);
    bool UnInit(WORD a_Flags=FT_IL_ALL);

    bool Load(AnsiString a_fname="");
    bool Save(AnsiString a_fname="");

public:
    CFtServer *GetServer(void);
    CFtTorrentsMgr *GetTorrnetsMgr(void);
    CFtVirtualTorrentMgr *GetVirtTorrnetsMgr(void);
    CFtOptions *GetOptions(void);
    CFtRulesMgr *GetRulesMgr(void);
    CFtLogger *GetLogger(void);

    AnsiString GetVer(int a_nVT);

public:
    bool InitializeOncePreInit(void);
    bool InitializeOnceAfterInit(void);    
    bool UnInitializeOnce(void);    

    //Запустить поиск обновления
    bool StartSearchUpdate(void);
    //Запустить обновление
    bool StartUpdate(void);

private:
    bool StartTorrentClient(void);
    bool CloseTorrentClient(void);    
    bool VerifyStartWithWin(void);

private:
    TOnSearchUpdates m_OnUpdates;
    TOnProcessUpdates m_OnProcessUpd;    

public:
    bool SetOnSearchUpdates(TOnSearchUpdates a_OnUpdates);
    bool SetOnProcessUpdates(TOnProcessUpdates a_OnProcessUpd);    

};

//---------------------------------------------------------------------------
extern TFtVersion *g_Ver;

#endif
