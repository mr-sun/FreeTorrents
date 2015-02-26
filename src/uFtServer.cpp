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

#include "uFtServer.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtServer
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtServer::CFtServer()
{
    m_TCPServer = new TIdTCPServer(NULL);
    m_TCPServer->OnExecute = OnExecute;
    m_TCPServer->OnConnect = OnConnect;
    m_TCPServer->OnDisconnect = OnDisconnect;
    UnInit();
}
CFtServer::~CFtServer()
{
    UnInit();
    delete m_TCPServer;
}

//---------------------------------------------------------------------------
bool CFtServer::IsRunning(void)
{
    return m_TCPServer->Active;
}

//---------------------------------------------------------------------------
//Инициализация сервера
bool CFtServer::Init(CFtNetOpt *a_NetOpt, CFtTorrentsMgr *a_TorrentsMgr, CFtLogger *a_Logger)
{
    UnInit();
    if(!m_TCPServer) return false;
    if(!a_NetOpt) return false;
    if(!a_TorrentsMgr) return false;
    m_NetOpt = a_NetOpt;
    m_TorrentsMgr = a_TorrentsMgr;
    m_Logger = a_Logger;
    m_TCPServer->DefaultPort = FT_DEFAULT_PORT;

    TIdSocketHandle *sock = m_TCPServer->Bindings->Add();
    if(sock)
    {
        g_CSOptions->Enter();
        sock->IP = "127.0.0.1"; //localhost
        sock->Port = m_NetOpt->GetPort();
        g_CSOptions->Leave();        
    }
    return true;
}

//---------------------------------------------------------------------------
bool CFtServer::UnInit(void)
{
    if(!m_TCPServer) return false;
    Stop();
    m_TCPServer->Bindings->Clear();
    m_NetOpt = 0;
    m_TorrentsMgr = 0;
    m_Logger = 0;    
    return true;
}
//---------------------------------------------------------------------------
bool CFtServer::Start(void)
{
    if(!m_TCPServer) return false;
    try
    {
        m_TCPServer->Active = true;

        if(m_Logger)
        {
            g_CSLogger->Enter();
            m_Logger->AddString("FreeTorrents Start.", "CFtServer");
            g_CSLogger->Leave();
        }
        return true;
    }
    catch(...)
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool CFtServer::Stop(void)
{
    if(!m_TCPServer) return false;
    try
    {
        m_TCPServer->Active = false;
        if(m_Logger)
        {
            g_CSLogger->Enter();
            m_Logger->AddString("FreeTorrents Stop.", "CFtServer");
            g_CSLogger->Leave();            
        }
        return true;
    }
    catch(...)
    {
        return false;
    }
}

//---------------------------------------------------------------------------
//Подготавливаем данные для последующего проксирования
bool CFtServer::PreparingData(TIdPeerThread *a_Thread, AnsiString &a_sData, int &a_nPort, AnsiString &a_sHost)
{
    if(!a_Thread) return false;

    /* Если используется внешний прокси-сервер и его тип HTTP,
       то нет необходимости вырезать из заголовков "прокси-информацию",
       т.к. она все ранво понадобится для дальнейшего проксирования. */
    bool bRemoveProxData = true;
    g_CSOptions->Enter();
    if( m_NetOpt->GetUseProxy() && m_NetOpt->GetProxyType() == PRTYPE_HTTP )
        bRemoveProxData = false;
    g_CSOptions->Leave();        
        
    if(!GetAddressFromRequest(a_sData, a_nPort, a_sHost, bRemoveProxData))
        return false;

    return true;
}

//---------------------------------------------------------------------------
//изменяем, модифицируем и т.п.
AnsiString CFtServer::ModifingData(TIdPeerThread *a_Thread, AnsiString a_sData)
{
    if(!m_TorrentsMgr) return a_sData;
    return m_TorrentsMgr->ModifingData(a_sData);
}

//---------------------------------------------------------------------------
//Проксируем измененные данные
bool CFtServer::ProxingData(TIdPeerThread *a_Thread, AnsiString a_sData, int a_nCltPort, AnsiString a_sCltHost)
{
    if(!a_Thread) return false;

    TIdTCPClient *clt = new TIdTCPClient(NULL);
    if(!clt) return false;

    /* Если не используется внешний прокси-сервер, то Порт и Хост подключения
       устанавливаем полученные из заголовков запроса, иначе они
       должны соответствовать адресу и порту внешнего прокси-сервера */
    g_CSOptions->Enter();
    if(m_NetOpt->GetUseProxy() && m_NetOpt->GetProxyType() == PRTYPE_HTTP)
    {
        clt->Port = m_NetOpt->GetProxyPort();
        clt->Host = m_NetOpt->GetProxyHost();
    }
    else
    {
        clt->Port = a_nCltPort;
        clt->Host = a_sCltHost;
    }
    clt->SocksInfo->Version = svNoSocks;
    if(m_NetOpt->GetUseProxy() && m_NetOpt->GetProxyType() != PRTYPE_HTTP)
    {
        clt->SocksInfo->Port = m_NetOpt->GetProxyPort();
        clt->SocksInfo->Host = m_NetOpt->GetProxyHost();
        switch( m_NetOpt->GetProxyType() )
        {
            case PRTYPE_SOCKS4:
            {
                clt->SocksInfo->Version = svSocks4;
                break;
            }
            case PRTYPE_SOCKS4a:
            {
                clt->SocksInfo->Version = svSocks4A;
                break;
            }
            case PRTYPE_SOCKS5:
            {
                clt->SocksInfo->Version = svSocks5;
                break;
            }
        }
    }
    g_CSOptions->Leave();    

    a_sData += "\r\n\r\n";
    AnsiString full_buf = "";

    try
    {
        //Если клиент еще не подключен - подключаем
        clt->Connect();
        if(clt->Connected())
        {
            clt->WriteBuffer(a_sData.c_str(), a_sData.Length(), true);
            Sleep(50);            
            DWORD StartTick = GetTickCount();
            while(clt->Connected())
            {
                full_buf += clt->CurrentReadBuffer();
                //Проверяем таймаут
                if( GetTickCount() >= StartTick + FT_CLT_READ_TIMEOUT )
                    break;
                Sleep(50);
            }
            full_buf += clt->CurrentReadBuffer();            
            clt->Disconnect();
        }
    }
    catch(...)
    {
        if(clt->Connected())
            clt->Disconnect();
        delete clt;
        if(a_Thread->Connection->Connected())
          a_Thread->Connection->WriteBuffer(full_buf.c_str(), full_buf.Length(), true);
        return false;
    }

    delete clt;
    a_Thread->Connection->WriteBuffer(full_buf.c_str(), full_buf.Length(), true);
    return true;
}

//---------------------------------------------------------------------------
void __fastcall CFtServer::OnExecute(TIdPeerThread *a_Thread)
{
    if(!a_Thread) return;
    if(!a_Thread->Connection) return;
    if(!a_Thread->Connection->Connected()) return;

    AnsiString sRequest = "";
    try
    {
        sRequest = a_Thread->Connection->ReadLn("\r\n\r\n");
        if(sRequest == "") sRequest = a_Thread->Connection->ReadLn("\n\n");
        if(sRequest == "") return;
    }
    catch(...)
    {
        //...
        return;
    }

    //Имеем полностью Header запроса со всеми заголовками в sRequest

    //Подготавливаем данные для последующего проксирования
    int nProxPort;
    AnsiString sProxHost;
    PreparingData(a_Thread, sRequest, nProxPort, sProxHost);

    g_CSTorrents->Enter();
    //изменяем, модифицируем и т.п.
    AnsiString sModifRequest = ModifingData(a_Thread, sRequest);
    g_CSTorrents->Leave();    

    //Проксируем модифицированные данные
    ProxingData(a_Thread, sModifRequest, nProxPort, sProxHost);

    a_Thread->Connection->Disconnect();
    a_Thread->Terminate();
}

//---------------------------------------------------------------------------
void __fastcall CFtServer::OnConnect(TIdPeerThread *a_Thread){}
//---------------------------------------------------------------------------
void __fastcall CFtServer::OnDisconnect(TIdPeerThread *a_Thread){}

//-----------------------------------------------------------------------------
//Получить адрес и порт из заголовков запроса
bool CFtServer::GetAddressFromRequest(AnsiString &a_Request, int &a_Port,
                                      AnsiString &a_Host, bool a_bRemoveProxData)
{
    a_Port = 80;
    if(_GetAddressFromRequestA(a_Request, a_Port, a_Host, a_bRemoveProxData))
    {
        //...
    }

    int pos = a_Request.AnsiPos("\nHost:");
    int posEnd = 0;
    if(!pos) return false;
    AnsiString substr = a_Request.SubString(pos+6, a_Request.Length());
    AnsiString tmpHost = "";
    int cou = substr.Length();
    for(int a=1; a < cou; a++)
    {
        if(substr[a]!='\r' && substr[a]!='\n' && substr[a]!=';')
        {
            tmpHost += substr[a];
            posEnd++;
        }
        else
            break;
    }
    int sep = tmpHost.AnsiPos(":");
    if(sep)
    {
        a_Host = tmpHost.SubString(1, sep-1).Trim();
        AnsiString sPort = tmpHost.SubString(sep+1, tmpHost.Length()).Trim();
        __try
        {
            a_Port = StrToInt(sPort);
        }
        catch(...)
        {
        }
    }
    else
        a_Host = tmpHost.Trim();

    if(a_bRemoveProxData)
    {
        AnsiString remstr = a_Request.SubString(pos, posEnd+6);
        a_Request = AnsiReplaceStr(a_Request, remstr, AnsiString("\nHost: ")+a_Host);
    }

    return true;
}

//-----------------------------------------------------------------------------
//Выбираем адрес и порт из заголовка метода GET (GET http://www.myhost.com:8081/index.html ...)
bool CFtServer::_GetAddressFromRequestA(AnsiString &a_Request, int &a_Port,
                                        AnsiString &a_Host, bool a_bRemoveProxData)
{
    UINT cou = a_Request.Length();
    int PosEnd = 0;
    for(UINT a=1; a <= cou; a++)
    {
        if(a_Request[a]!='\r' && a_Request[a]!='\n')
            PosEnd = a;
        else
            break;
    }
    AnsiString str = a_Request.SubString(1, PosEnd);
    int httpPos = str.AnsiPos("http://");
    if(httpPos)
    {
        bool needLastSlash = false;
        //Найдена такая строка... значит это прокси-запрос
        //Вытаскиваем из строки имя хоста и порт, если есть...
        UINT scou = str.Length();
        int sPosE = 0;
        for(UINT b=httpPos+7; b <= scou; b++)
        {
            if(str[b]!='/'  && str[b]!=' ')
                sPosE++;
            else
            {
                if(str[b]==' ')
                    needLastSlash = true;
                break;
            }
        }
        AnsiString shost = str.SubString(httpPos+7, sPosE);
        int sep = shost.AnsiPos(":");
        if(sep)
        {
            a_Host = shost.SubString(1, sep-1).Trim();
            AnsiString sPort = shost.SubString(sep+1, shost.Length()).Trim();
            __try
            {
                a_Port = StrToInt(sPort);
            }
            catch(...)
            {
            }
        }
        else
          a_Host = shost.Trim();

        if(a_bRemoveProxData)
        {
            //Заменяем в запросе строку
            AnsiString replstr = "http://" + shost;
            AnsiString replaced = "";
            if(needLastSlash) replaced = "/";
            AnsiString rStr = AnsiReplaceStr(str, replstr, replaced);
            a_Request = AnsiReplaceStr(a_Request, str, rStr);
        }
    }
    return true;
}


