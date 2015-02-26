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

#ifndef uFtUpdaterH
#define uFtUpdaterH

#include <Classes.hpp>
#include <IdHTTP.hpp>
#include "uFtOptions.h"

//---------------------------------------------------------------------------
typedef void (__closure *TOnSearchUpdates)(bool a_bNewUpdates);
typedef void (__closure *TOnProcessUpdates)(UINT a_nPercent);

#define FTUPD_CLT_READ_TIMEOUT  2000 //мсек

#define FTUPD_UAGENT         "ftUpdater/1.0.0"
#define FTUPD_SERVPORT       80
#define FTUPD_SERVHOST       "ft.mrsun.ru"
#define FTUPD_SERVPATH       "/updates"
#define FTUPD_SERVINFOFILE   "updates.php?tester"
#define FTUPD_LOCALINFOFILE  "updates.info"
#define FTUPD_LOCALTMPDIR    "Updates"
#define FTUPD_LOCALPLGDIR    "Plugins"

typedef enum
{
    FTUPD_OTT_UPDATE=0,  //обновить
    FTUPD_OTT_SEARCH     //проверить наличие обновлений
}TFtUpdOnTimerType;

//---------------------------------------------------------------------------
class CFtUpdater
{
private:
    CFtOptions *m_Opt;
    TIdHTTP *m_Clt;
    TTimer *m_Timer;
    TFtUpdOnTimerType m_OnTimerType;
    bool m_bUpdating; //Идет обновление (или проверка)

public:
    CFtUpdater();
    ~CFtUpdater();

public:
    bool Init(CFtOptions *a_Opt);
    bool UnInit(void);

    //Копируем по нужным местам обновленные файлы
    bool CopyUpdatingFiles(void);

    //Проверить наличие обновлений
    bool SerchUpdate(void);
    //Обновить...
    bool Update(void);
    bool UpdateAfterTimeout(UINT a_msec);
    bool SearchAfterTimeout(UINT a_msec);        

    bool SetOnSearchUpdates(TOnSearchUpdates a_OnUpdates);
    bool SetOnProcessUpdates(TOnProcessUpdates a_OnProcessUpd);

private:
    void __fastcall OnTerminate(TObject *Sender);
    void __fastcall OnTimer(TObject *Sender);    

    TOnSearchUpdates m_OnUpdates;
    TOnProcessUpdates m_OnProcessUpd;

    AnsiString GetUserAgent(void);
};



#define FTTH_ACTION_SEARCHUPDATE  0
#define FTTH_ACTION_UPDATE        1
//---------------------------------------------------------------------------
class TThreadUpdater : public TThread
{
private:
    int m_nAction; //FTTH_ACTION
    TIdHTTP *m_Clt;
    bool m_bHttpProxy;
    CFtUpdaterOpt *m_UpdOpt;

protected:
    void __fastcall Execute();

public:
    __fastcall TThreadUpdater(bool CreateSuspended, int a_nAction, bool a_bHttpProxy,
                              TIdHTTP *a_Clt, CFtUpdaterOpt *a_UpdOpt);

private:
    bool m_bNewUpdates;
    TOnSearchUpdates m_OnUpdates;
    void OnSearchUpdates(bool a_bNewUpdates);
    void __fastcall SynchOnUpdates(void);

    UINT m_nPercent;
    TOnProcessUpdates m_OnProcessUpd;
    void OnProcessUpdates(UINT a_nPercent);
    void __fastcall SynchOnProcessUpd(void);    

public:
    bool SetOnSearchUpdates(TOnSearchUpdates a_OnUpdates);
    bool SetOnProcessUpdates(TOnProcessUpdates a_OnProcessUpd);    

private:
    void SearchUpdate(void); //Проверить наличие обновлений
    void Update(void);  //Обновить

    bool _SearchUpdate(void);
    bool _Update(void);        
    bool DownloadFile(AnsiString a_sLocalFile, AnsiString a_sFile);
    UINT GetUpdateNumber(AnsiString a_fname);

};

#endif
