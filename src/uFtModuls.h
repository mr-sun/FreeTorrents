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

#ifndef uFtModulH
#define uFtModulH

#include <vector.h>
#include "iftplugin.h"

//Типы функций обязательно экспортируемые DLL-плагином
typedef IFtPlugin * (_cdecl TCPCPROC)(void);  //DestroyPluginClass
typedef bool (_cdecl TDPCPROC)(IFtPlugin*);   //CreatePluginClass
typedef void (_cdecl TGPIPROC)(LPPLUGININFO); //GetPluginInfo

#define FT_PLUGINS_DIR  "Plugins"

//---------------------------------------------------------------------------
class CFtModul
{
private:
    PLUGININFO m_info;
    AnsiString m_ModulFile;

    HINSTANCE m_hLib;
    IFtPlugin *m_Plugin;

public:
    CFtModul();
    ~CFtModul();

    CFtModul & operator = ( const CFtModul & v)
    {
        m_ModulFile = v.m_ModulFile;
        m_info = v.m_info;
        m_hLib = v.m_hLib;
        m_Plugin = v.m_Plugin;
        return *this;
    }

public:
    AnsiString GetModulFile(void);
    PLUGININFO *GetInfo(void);


    bool LoadModul(AnsiString a_ModulFile);
    bool FreeModul(void);    

public: //IFtPlugin
    bool ICreate(void);
    bool IClose(void);   
    bool IInit(void);
    bool IUnInit(void);
    bool IProcess(void);
    bool INotify(DWORD lParam, DWORD wParam);
};


//---------------------------------------------------------------------------
class CFtModulsMgr
{
private:
    vector<CFtModul> m_vModuls;
    
public:
    CFtModulsMgr();
    ~CFtModulsMgr();

public:
    bool LoadModuls(void);
    bool FreeModuls(void);

    UINT GetCount(void);
    PLUGININFO *GetPluginInfo(UINT a_nIndex);

public:    
    bool IInit(void);
    bool IUnInit(void);
    bool IProcess(void);
    bool INotify(DWORD lParam, DWORD wParam);
};


#endif
