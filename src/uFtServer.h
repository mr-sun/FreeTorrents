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

#ifndef uFtServerH
#define uFtServerH

#include <IdTCPServer.hpp>
#include <IdTCPClient.hpp>
#include <vector.h>
#include <StrUtils.hpp>

#include "uFtLogger.h"
#include "uFtOptions.h"
#include "uFtTorrents.h"

#define FT_CLT_READ_TIMEOUT  2000 //мсек

//---------------------------------------------------------------------------
class CFtServer
{
private:
    TIdTCPServer *m_TCPServer;
    CFtNetOpt *m_NetOpt;
    CFtTorrentsMgr *m_TorrentsMgr;
    CFtLogger *m_Logger;

public:
    CFtServer();
    ~CFtServer();

public:
    bool Init(CFtNetOpt *a_NetOpt, CFtTorrentsMgr *a_TorrentsMgr, CFtLogger *a_Logger);
    bool UnInit(void);

    bool Start(void);
    bool Stop(void);

    bool IsRunning(void);

private:
    //Подготавливаем данные для последующего проксирования
    bool PreparingData(TIdPeerThread *a_Thread, AnsiString &a_sData, int &a_nPort, AnsiString &a_sHost);
    //изменяем, модифицируем и т.п.
    AnsiString ModifingData(TIdPeerThread *a_Thread, AnsiString a_sData);
    //Проксируем измененные данные
    bool ProxingData(TIdPeerThread *a_Thread, AnsiString a_sData, int a_nCltPort, AnsiString a_sCltHost);

    //Получить адрес и порт из заголовков запроса
    bool GetAddressFromRequest(AnsiString &a_Request, int &a_Port, AnsiString &a_Host, bool a_bRemoveProxData=true);
    bool _GetAddressFromRequestA(AnsiString &a_Request, int &a_Port, AnsiString &a_Host, bool a_bRemoveProxData);

private:
    void __fastcall OnExecute(TIdPeerThread *a_Thread);
    void __fastcall OnConnect(TIdPeerThread *a_Thread);
    void __fastcall OnDisconnect(TIdPeerThread *a_Thread);
};


//---------------------------------------------------------------------------
#endif
