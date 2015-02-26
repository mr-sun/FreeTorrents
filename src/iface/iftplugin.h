/****************************************************************************
* FreeTorrents - is free software for free torrents )
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

#ifndef iftpluginH
#define iftpluginH

//Нотифаи плагинов
enum{
    PN_ENTERFTOPTCFG,     //Вход в настройки программы
    PN_LEAVEFTOPTCFG,     //Выход из настроек программы
    PN_ENTERFTRULESCFG,   //Вход в настройки правил
    PN_LEAVEFTRULESCFG,   //Выход из настроек правил

    PN_BEFORSTART,
    PN_AFTERSTART,
    PN_BEFORSTOP,
    PN_AFTERSTOP
};

//Информация о плагине
typedef struct {
  char name[101];
  char describe[201];
  char vendor[201];
  short version;
  short subversion;
}PLUGININFO;
typedef PLUGININFO* LPPLUGININFO;

// {5866D632-0C4B-14d4-8136-00E029395A6F}
static const GUID IID_IFtPlugin =
{ 0x5866d632, 0xc4b, 0x14d4, { 0x81, 0x36, 0x0, 0xe0, 0x29, 0x39, 0x5a, 0x6f } };

//---------------------------------------------------------------------------
class IFtPlugin : public IUnknown
{
public:
    virtual bool STDMETHODCALLTYPE Create(void)=0;
    virtual bool STDMETHODCALLTYPE Close(void)=0;
    virtual bool STDMETHODCALLTYPE Init(void)=0;
    virtual bool STDMETHODCALLTYPE UnInit(void)=0;
    virtual bool STDMETHODCALLTYPE Process(void)=0;
    virtual bool STDMETHODCALLTYPE Notify(DWORD lParam, DWORD wParam)=0;
};


#endif
