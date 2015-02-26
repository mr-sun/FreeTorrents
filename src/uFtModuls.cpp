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
#include<dirent.h>
#pragma hdrstop

#include "uFtModuls.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CFtModulsMgr
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtModul::CFtModul()
{
    m_Plugin = 0;
    m_hLib = 0;
}
CFtModul::~CFtModul()
{
}

//---------------------------------------------------------------------------
bool CFtModul::LoadModul(AnsiString a_ModulFile)
{
    m_ModulFile = a_ModulFile;
    if(!FileExists(m_ModulFile)) return false;

    //Пытаемся загрузить dll-ку
    m_hLib = LoadLibrary(m_ModulFile.c_str());
    if(!m_hLib) return false;

    TGPIPROC *GetPluginInfo;
    TCPCPROC *CreatePluginClass;

    //Пробуем получить инфу о плагине
    GetPluginInfo = (TGPIPROC*)GetProcAddress(m_hLib, "GetPluginInfo");
    if(!GetPluginInfo) GetPluginInfo = (TGPIPROC*)GetProcAddress(m_hLib, "_GetPluginInfo");
    if(!GetPluginInfo)
    {
        FreeModul();
        return false;
    }
    GetPluginInfo(&m_info);

    //Создаем экземпляр класса плагина..
    CreatePluginClass = (TCPCPROC*)GetProcAddress(m_hLib, "CreatePluginClass");
    if(!CreatePluginClass) CreatePluginClass = (TCPCPROC*)GetProcAddress(m_hLib, "_CreatePluginClass");
    if(!CreatePluginClass)
    {
        FreeModul();    
        return false;
    }
    m_Plugin = CreatePluginClass();
    if(!m_Plugin)
    {
        FreeModul();    
        return false;
    }
    ICreate();
    return true;
}

//---------------------------------------------------------------------------
bool CFtModul::FreeModul(void)
{
    IClose();

    if(m_hLib)
    {
        if(m_Plugin)
        {
            m_Plugin->Release();        
            TDPCPROC *DestroyPluginClass;
            DestroyPluginClass = (TDPCPROC*)GetProcAddress(m_hLib, "DestroyPluginClass");
            if(!DestroyPluginClass) DestroyPluginClass = (TDPCPROC*)GetProcAddress(m_hLib, "_DestroyPluginClass");
            if(DestroyPluginClass) DestroyPluginClass(m_Plugin);
        }
    }

    if(m_hLib)
        FreeLibrary(m_hLib);

    m_Plugin = 0;
    m_hLib = 0;
    m_ModulFile = "";
    return true;    
}

//---------------------------------------------------------------------------
AnsiString CFtModul::GetModulFile(void)
{
    return m_ModulFile;
}
//---------------------------------------------------------------------------
PLUGININFO *CFtModul::GetInfo(void)
{
    return &m_info;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CFtModul::ICreate(void)
{
    if(!m_Plugin) return false;
    return m_Plugin->Create();
}

//---------------------------------------------------------------------------
bool CFtModul::IClose(void)
{
    if(!m_Plugin) return false;
    return m_Plugin->Close();
}

//---------------------------------------------------------------------------
bool CFtModul::IInit(void)
{
    if(!m_Plugin) return false;
    return m_Plugin->Init();
}

//---------------------------------------------------------------------------
bool CFtModul::IUnInit(void)
{
    if(!m_Plugin) return false;
    return m_Plugin->UnInit();
}

//---------------------------------------------------------------------------
bool CFtModul::IProcess(void)
{
    if(!m_Plugin) return false;
    return m_Plugin->Process();
}

//---------------------------------------------------------------------------
bool CFtModul::INotify(DWORD lParam, DWORD wParam)
{
    if(!m_Plugin) return false;
    return m_Plugin->Notify(lParam, wParam);
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CFtModulsMgr
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtModulsMgr::CFtModulsMgr()
{
}
CFtModulsMgr::~CFtModulsMgr()
{
}

//---------------------------------------------------------------------------
bool CFtModulsMgr::FreeModuls(void)
{
    bool bRes = true;
    UINT cou = m_vModuls.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vModuls[a].FreeModul();
    }
    m_vModuls.clear();
    return bRes;
}

//---------------------------------------------------------------------------
//Загрузка найденных плагинов
bool CFtModulsMgr::LoadModuls(void)
{
    AnsiString DirForSearch = ExtractFilePath(Application->ExeName)
                              + FT_PLUGINS_DIR;
                              
    if(!DirectoryExists(DirForSearch)) return true;
    
    //Перебираем все файлы в этой директории
    struct dirent *ent;
    DIR *dir = opendir(DirForSearch.c_str());
    if(dir)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            //получили внутреннюю структуру папок и файлов
            AnsiString sdir = ent->d_name;
            if(sdir!="." && sdir!="..")
            {
                //Файлы и папки в директории
                AnsiString fname = DirForSearch + "\\" + sdir;
                AnsiString ext =ExtractFileExt(fname);
                if(ext == ".dll")
                {
                    if(FileExists(fname))
                    {
                        //Это dll-файл, пробуем загрузить его как плагин
                        CFtModul tmpModul;
                        if(tmpModul.LoadModul(fname))
                        {
                            //Если удалось загрузить его как плагин, то добавляем в список
                            m_vModuls.push_back(tmpModul);
                        }
                        else
                        {
                            //Эта dll-ка не плагин программы.
                        }
                    }
                }
            }
        }
        closedir(dir);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
UINT CFtModulsMgr::GetCount(void)
{
    return m_vModuls.size();
}
//---------------------------------------------------------------------------
PLUGININFO *CFtModulsMgr::GetPluginInfo(UINT a_nIndex)
{
    if(a_nIndex >= m_vModuls.size()) return 0;
    return m_vModuls[a_nIndex].GetInfo();
}

//---------------------------------------------------------------------------
bool CFtModulsMgr::IInit(void)
{
    bool bRes = true;
    UINT cou = m_vModuls.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vModuls[a].IInit();
    }
    return bRes;
}
//---------------------------------------------------------------------------
bool CFtModulsMgr::IUnInit(void)
{
    bool bRes = true;
    UINT cou = m_vModuls.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vModuls[a].IUnInit();
    }
    return bRes;
}
//---------------------------------------------------------------------------
bool CFtModulsMgr::IProcess(void)
{
    bool bRes = true;
    UINT cou = m_vModuls.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vModuls[a].IProcess();
    }
    return bRes;
}
//---------------------------------------------------------------------------
bool CFtModulsMgr::INotify(DWORD lParam, DWORD wParam)
{
    bool bRes = true;
    UINT cou = m_vModuls.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vModuls[a].INotify(lParam, wParam);
    }
    return bRes;
}





 