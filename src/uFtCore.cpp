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
#include <tlhelp32.h>
#include <Registry.hpp>
#pragma hdrstop

#include "uFtCore.h"

TFtVersion *g_Ver = 0;

#pragma package(smart_init)

//---------------------------------------------------------------------------
CFtCore::CFtCore()
{
    m_OnUpdates = 0;
    m_OnProcessUpd = 0;
    g_Ver = &m_Ver;
}
CFtCore::~CFtCore()
{
}

//---------------------------------------------------------------------------
CFtServer *CFtCore::GetServer(void)
{
    return &m_Server;
}
//---------------------------------------------------------------------------
CFtTorrentsMgr *CFtCore::GetTorrnetsMgr(void)
{
    return &m_TorrnetsMgr;
}
//---------------------------------------------------------------------------
CFtVirtualTorrentMgr *CFtCore::GetVirtTorrnetsMgr(void)
{
    return &m_VTorrentsMgr;
}
//---------------------------------------------------------------------------
CFtOptions *CFtCore::GetOptions(void)
{
    return &m_Opt;
}
//---------------------------------------------------------------------------
CFtRulesMgr *CFtCore::GetRulesMgr(void)
{
    return &m_RulesMgr;
}
//---------------------------------------------------------------------------
CFtLogger *CFtCore::GetLogger(void)
{
    return &m_SrvLogger;
}
//---------------------------------------------------------------------------
AnsiString CFtCore::GetVer(int a_nVT)
{
    return m_Ver.GetVersionStr(a_nVT);
}

//---------------------------------------------------------------------------
bool CFtCore::Init(WORD a_Flags)
{
    UnInit(a_Flags);
    bool bRes = true;
    g_CSOptions->Enter();

    if((a_Flags&FT_IL_LOGGER)==FT_IL_LOGGER)
    {
        g_CSLogger->Enter();
        m_SrvLogger.Init(m_Opt.GetOtherOpt());
        g_CSLogger->Leave();
    }

    if((a_Flags&FT_IL_TORRENTS)==FT_IL_TORRENTS)
    {
        g_CSTorrents->Enter();
        bRes &= m_TorrnetsMgr.Init( &m_RulesMgr , &m_TorrentInfoMgr, &m_SrvLogger,
                                     m_Opt.GetTorrentsOpt());
        g_CSTorrents->Leave();
    }

    if((a_Flags&FT_IL_SERVER)==FT_IL_SERVER)
    {
        bRes &= m_Server.Init( m_Opt.GetNetOpt(), &m_TorrnetsMgr, &m_SrvLogger);
    }

    if((a_Flags&FT_IL_UPDATER)==FT_IL_UPDATER)
    {
        bRes &= m_Updater.Init( &m_Opt );
    }

    if((a_Flags&FT_IL_VTORRENTS)==FT_IL_VTORRENTS)
    {
        bRes &= m_VTorrentsMgr.Init( m_Opt.GetNetOpt(), m_Opt.GetVirtTorrentsOpt());
    }

    if(bRes)
    {
        VerifyStartWithWin();
        m_Server.Start();
    }

    g_CSOptions->Leave();
    return bRes;
}

//---------------------------------------------------------------------------
bool CFtCore::UnInit(WORD a_Flags)
{
    bool bRes = true;

    if((a_Flags&FT_IL_TORRENTS)==FT_IL_TORRENTS)
    {
        g_CSTorrents->Enter();
        bRes &= m_TorrnetsMgr.UnInit();
        g_CSTorrents->Leave();
    }

    if((a_Flags&FT_IL_UPDATER)==FT_IL_UPDATER)
    {
        bRes &= m_Updater.UnInit();
    }

    if((a_Flags&FT_IL_SERVER)==FT_IL_SERVER)
    {
        bRes &= m_Server.UnInit();
    }

    if((a_Flags&FT_IL_VTORRENTS)==FT_IL_VTORRENTS)
    {
        bRes &= m_VTorrentsMgr.UnInit();
    }

    if((a_Flags&FT_IL_LOGGER)==FT_IL_LOGGER)
    {
        g_CSLogger->Enter();
        m_SrvLogger.UnInit();
        g_CSLogger->Leave();
    }

    return bRes;
}

//---------------------------------------------------------------------------
bool CFtCore::Load(AnsiString a_fname)
{
    AnsiString opt_fname = ExtractFilePath(Application->ExeName)+FT_DEFAULT_OPT_FNAME;
    AnsiString rls_fname = ExtractFilePath(Application->ExeName)+FT_DEFAULT_RULES_FNAME;
    AnsiString virt_fname = ExtractFilePath(Application->ExeName)+FT_DEFAULT_VIRTUAL_FNAME;    
    if(a_fname != "")
    {
        if( DirectoryExists(ExtractFilePath(a_fname)) )
        {
            opt_fname = a_fname;
            rls_fname = a_fname;
            virt_fname = a_fname;
        }
    }

    bool bRes = true;

    g_CSOptions->Enter();
    bRes &= m_Opt.Load( opt_fname );
    g_CSOptions->Leave();

    bRes &= m_RulesMgr.Load( rls_fname );
    bRes &= m_VTorrentsMgr.Load(virt_fname);

    return bRes;
}

//---------------------------------------------------------------------------
bool CFtCore::Save(AnsiString a_fname)
{
    AnsiString opt_fname = ExtractFilePath(Application->ExeName)+FT_DEFAULT_OPT_FNAME;
    AnsiString rls_fname = ExtractFilePath(Application->ExeName)+FT_DEFAULT_RULES_FNAME;
    AnsiString virt_fname = ExtractFilePath(Application->ExeName)+FT_DEFAULT_VIRTUAL_FNAME;
    if(a_fname != "")
    {
        if( DirectoryExists(ExtractFilePath(a_fname)) )
        {
            opt_fname = a_fname;
            rls_fname = a_fname;
            virt_fname = a_fname;            
        }
    }

    bool bRes = true;

    g_CSOptions->Enter();
    bRes &= m_Opt.Save( opt_fname );
    g_CSOptions->Leave();

    bRes &= m_RulesMgr.Save( rls_fname );
    bRes &= m_VTorrentsMgr.Save(virt_fname);

    return bRes;
}

//---------------------------------------------------------------------------
bool CFtCore::InitializeOncePreInit(void)
{
    bool bRes = true;

    m_Updater.SetOnSearchUpdates(m_OnUpdates);
    m_Updater.SetOnProcessUpdates(m_OnProcessUpd);
    //Копируем файлы по местам после обновления (если необходимо)
    m_Updater.CopyUpdatingFiles();

    //Собираем инфу о торрент-файлах
    bRes &= m_TorrentInfoMgr.Init( m_Opt.GetTorrentCltOpt()->GetPaths() );

    return bRes;
}
//---------------------------------------------------------------------------
bool CFtCore::InitializeOnceAfterInit(void)
{
    bool bRes = true;

    //Загружаем плагинчики
    bRes &= m_Moduls.LoadModuls();

    //Если необходимо, то запускаем торрент-клиент
    g_CSOptions->Enter();
    bRes &= StartTorrentClient();
    g_CSOptions->Leave();    

    //Проверяем наличие обновлений...
    if(m_Opt.GetOtherOpt()->GetSearchUpdate())
    {
        m_Updater.SearchAfterTimeout(FT_STARTUPDATE_TIMEOUT);
    }

    return bRes;
}

//---------------------------------------------------------------------------
bool CFtCore::UnInitializeOnce(void)
{
    bool bRes = true;

    //Выгружаем плагинчики
    bRes &= m_Moduls.FreeModuls();

    bRes &= m_TorrentInfoMgr.UnInit();

    //Если необходимо, то выгружаем торрент-клинт (закрываем)
    g_CSOptions->Enter();
    bRes &= CloseTorrentClient();
    g_CSOptions->Leave();    

    return bRes;
}

//---------------------------------------------------------------------------
//Запустить поиск обновления
bool CFtCore::StartSearchUpdate(void)
{
    return m_Updater.SerchUpdate();
}
//---------------------------------------------------------------------------
//Запустить обновление
bool CFtCore::StartUpdate(void)
{
    return m_Updater.UpdateAfterTimeout(3000);
}

//---------------------------------------------------------------------------
bool CFtCore::StartTorrentClient(void)
{
    //Если необходимо, то запускаем торрент-клиент
    CFtTorrentCltOpt *cltOpt = m_Opt.GetTorrentCltOpt();
    if(cltOpt)
    {
        if(cltOpt->GetAutoStart())
        {
            AnsiString cltFname = cltOpt->GetTrntClientFileName();
            if(cltFname!="")
            if(FileExists(cltFname))
            {
                //Запускаем его
                UINT nRes = WinExec(cltFname.c_str(), SW_SHOWDEFAULT);
                if(nRes < 31)//то была ошибка...
                {
                    AnsiString strErr = "";                
                    switch(nRes)
                    {
                        case 0: strErr="The system is out of memory or resources."; break;
                        case ERROR_BAD_FORMAT: strErr="The .EXE file is invalid (non-Win32 .EXE or error in .EXE image)."; break;
                        case ERROR_FILE_NOT_FOUND: strErr="The specified file was not found."; break;
                        case ERROR_PATH_NOT_FOUND: strErr="The specified path was not found."; break;
                        default: strErr="Unknown error of start Torrent-client program ("+IntToStr(nRes)+").";
                    }
                    MessageBox(NULL, strErr.c_str(), "Ошибка запуска программы торрент-клиента.", MB_OK|MB_ICONERROR);
                    return false;
                }
            }
            else
            {
                MessageBox(NULL, "Файл программы не найден.", "Ошибка запуска программы торрент-клиента.", MB_OK|MB_ICONERROR);
                return false;
            }
        }
    }
    return true;
}

//---------------------------------------------------------------------------
bool CFtCore::CloseTorrentClient(void)
{
    CFtTorrentCltOpt *cltOpt = m_Opt.GetTorrentCltOpt();
    if(cltOpt)
    {
        if(cltOpt->GetAutoClose())
        {
            AnsiString cltFname = cltOpt->GetTrntClientFileName();
            if(FileExists(cltFname))
            {

                //Ищем нужный процесс в списке процессов...
                HINSTANCE hKernel;
                HANDLE (WINAPI * _CreateToolhelp32Snapshot)(DWORD, DWORD);
                BOOL (WINAPI * _Process32First)(HANDLE, PROCESSENTRY32 *);
                BOOL (WINAPI * _Process32Next)(HANDLE, PROCESSENTRY32 *);

                // получаем описатель KERNEL32.DLL
                hKernel = GetModuleHandle(_T("kernel32.dll"));
                if(hKernel)
                {
                    // находим необходимые функции в KERNEL32.DLL
                    *(FARPROC *)&_CreateToolhelp32Snapshot =
                        GetProcAddress(hKernel, "CreateToolhelp32Snapshot");
                    #ifdef UNICODE
                      *(FARPROC *)&_Process32First =
                          GetProcAddress(hKernel, "Process32FirstW");
                      *(FARPROC *)&_Process32Next =
                          GetProcAddress(hKernel, "Process32NextW");
                    #else
                      *(FARPROC *)&_Process32First =
                          GetProcAddress(hKernel, "Process32First");
                      *(FARPROC *)&_Process32Next =
                          GetProcAddress(hKernel, "Process32Next");
                    #endif

                    if (_CreateToolhelp32Snapshot == NULL || _Process32First == NULL
                        || _Process32Next == NULL)
                      return false;

                    HANDLE hSnapshot;
                    PROCESSENTRY32 Entry;
                    // создаем моментальный снимок процессов
                    hSnapshot = _CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                    if (hSnapshot == INVALID_HANDLE_VALUE)
                        return false;

                    // получаем информацию о первом процессе
                    Entry.dwSize = sizeof(Entry);
                    if (!_Process32First(hSnapshot, &Entry))
                    {
                        CloseHandle(hSnapshot);
                        return false;
                    }

                    cltFname = ExtractFileName(cltFname).LowerCase();

                    // перечисляем остальные процессы
                    do
                    {
                        LPTSTR pszProcessName = NULL;
                        if (Entry.dwSize > offsetof(PROCESSENTRY32, szExeFile))
                        {
                            pszProcessName = _tcsrchr(Entry.szExeFile, _T('\\'));
                            if (pszProcessName == NULL)
                                pszProcessName = Entry.szExeFile;
                        }
                        AnsiString strProc = AnsiString(pszProcessName).LowerCase();
                        if(cltFname == strProc)
                        {
                            //Вероятнее всего это запущенная программа торрент-клиент
                            //нужно ее выгрузить...
                            //Получаем хэндл процесса по его PID
                            HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, Entry.th32ProcessID);
                            if(hProc)
                            {
                                //todo
                                // DWORD idThread=0;
                                // PostThreadMessage(idThread,WM_QUIT, 0,0);

                                HANDLE hThreadSnap;
                                THREADENTRY32 te32;
                                hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
                                if( hThreadSnap == INVALID_HANDLE_VALUE )
                                    return false;

                                te32.dwSize = sizeof(THREADENTRY32);
                                if( !Thread32First( hThreadSnap, &te32 ) )
                                {
                                    CloseHandle( hThreadSnap );
                                    return false;
                                }

                                do
                                {
                                    if( te32.th32OwnerProcessID == Entry.th32ProcessID )
                                    {
                                        if(PostThreadMessage(te32.th32ThreadID, WM_QUIT, 0, 0))
                                        {
                                            Sleep(1000);
                                        }
                                        else
                                        {
                                            //Убиваем процесс (но лучше бы сначала корректно закрыть)
                                            //TerminateProcess(hProc, -1);
                                        }
                                    }
                                }
                                while( Thread32Next(hThreadSnap, &te32 ) );

                                CloseHandle( hThreadSnap );
                                CloseHandle(hProc);
                            }
                        }
                        Entry.dwSize = sizeof(Entry);
                    }
                    while (_Process32Next(hSnapshot, &Entry));
                    CloseHandle(hSnapshot);
                    return true;
                }
            }
        }
    }
    return false;
}

//---------------------------------------------------------------------------
bool CFtCore::VerifyStartWithWin(void)
{
    CFtOtherOpt *other = m_Opt.GetOtherOpt();
    if(!other) return false;
    bool bSWW = other->GetStartWithWin();
    bool bRes = true;
    //Проверяем ключ автозапуска в реестре...
    TRegistry *reg = new TRegistry(KEY_ALL_ACCESS);
    if(reg)
    {
        //HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run
        reg->RootKey = HKEY_CURRENT_USER;
        AnsiString tkey = "Software\\";
        tkey += "Microsoft\\";
        tkey += "Windows\\";
        tkey += "CurrentVersion\\";
        tkey += "Run";

        try
        {
            if(reg->OpenKey(tkey, false))
            {
                if(bSWW)
                {
                    AnsiString sApplFname = Application->ExeName;
                    //Необходим автозапуск
                    if(!reg->ValueExists(FT_REGKEYVAL))
                    {
                        reg->WriteString(FT_REGKEYVAL, sApplFname);
                    }
                    else
                    {
                        AnsiString str = reg->ReadString(FT_REGKEYVAL);
                        if(sApplFname != str)
                        {
                            reg->WriteString(FT_REGKEYVAL, sApplFname);
                        }
                    }
                }
                else
                {
                    //Не нужен автозапуск
                    if(reg->ValueExists(FT_REGKEYVAL))
                    {
                        reg->DeleteValue(FT_REGKEYVAL);
                    }
                }
            }
        }
        catch(...)
        {
            bRes = false;
        }
        delete reg;
    }
    return bRes;
}

//---------------------------------------------------------------------------
bool CFtCore::SetOnSearchUpdates(TOnSearchUpdates a_OnUpdates)
{
    m_OnUpdates = a_OnUpdates;
    return true;
}
//---------------------------------------------------------------------------
bool CFtCore::SetOnProcessUpdates(TOnProcessUpdates a_OnProcessUpd)
{
    m_OnProcessUpd = a_OnProcessUpd;
    return true;
}
