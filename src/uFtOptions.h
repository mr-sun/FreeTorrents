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

#ifndef uFtOptionsH
#define uFtOptionsH

#include <SyncObjs.hpp>
#include <inifiles.hpp>
#include <vector.h>

#define FT_DEFAULT_PORT        14000 //���� �� ���������
#define FT_DEFAULT_PROXYPORT   3128 //���� ������ �� ���������
#define FT_DEFAULT_PROXYHOST   "192.168.0.1" //���� ������ �� ���������

//���� �������� ������
enum
{
    PRTYPE_HTTP=0,
    PRTYPE_SOCKS4=1,
    PRTYPE_SOCKS4a=2,
    PRTYPE_SOCKS5=3
};



//---------------------------------------------------------------------------
//������� ��������� ���������.
class CFtNetOpt
{
private:
    int m_nPort;  //���� �� ������� �������� ��

    bool m_bUseProxy;   //������������ ������� ������
    int  m_nProxyType;  //��� �������� ������
    int  m_nProxyPort;  //���� ������
    AnsiString m_sProxyHost;  //����� �������� ������

public:
    CFtNetOpt();
    ~CFtNetOpt();

    CFtNetOpt & operator = ( const CFtNetOpt & v)
    {
        m_nPort = v.m_nPort;
        m_bUseProxy = v.m_bUseProxy;
        m_nProxyType = v.m_nProxyType;
        m_nProxyPort = v.m_nProxyPort;
        m_sProxyHost = v.m_sProxyHost;
        return *this;
    }

public:
    int GetPort(void);
    bool SetPort(int a_nPort);
    bool GetUseProxy(void);
    bool SetUseProxy(bool a_bUseProxy);
    int GetProxyType(void);
    bool SetProxyType(int a_nProxyType);
    int GetProxyPort(void);
    bool SetProxyPort(int a_nProxyPort);
    AnsiString GetProxyHost(void);
    bool SetProxyHost(AnsiString a_sProxyHost);

public:
    bool Load(TIniFile *a_ini);
    bool Save(TIniFile *a_ini);
};


typedef vector<AnsiString> TFtPathsList;
//---------------------------------------------------------------------------
//��������� ������������� �������-�������
class CFtTorrentCltOpt
{
private:
    bool m_bAutoStart; //��������� �������-������ ��� ������� ��.
    bool m_bAutoClose; //��������� �������-������ ��� ������ �� ��.
    AnsiString m_TrntClientFileName; //���� � �������-������� (��� �����������)

    //������ ����� � ���������� ��� ������ �������-������...
    TFtPathsList m_vPaths; //ToTorrentFiles

public:
    CFtTorrentCltOpt();
    ~CFtTorrentCltOpt();

    CFtTorrentCltOpt & operator = ( const CFtTorrentCltOpt & v)
    {
        m_bAutoStart = v.m_bAutoStart;
        m_bAutoClose = v.m_bAutoClose;
        m_TrntClientFileName = v.m_TrntClientFileName;
        m_vPaths = v.m_vPaths;
        return *this;
    }

public:    
    bool GetAutoStart(void);
    bool SetAutoStart(bool a_bAutoStart);
    bool GetAutoClose(void);
    bool SetAutoClose(bool a_bAutoClose);
    AnsiString GetTrntClientFileName(void);
    bool SetTrntClientFileName(AnsiString a_TrntClientFileName);

    TFtPathsList GetPaths(void);
    UINT GetPathsCount(void);
    AnsiString GetPath(UINT a_Index);
    bool DeletePath(UINT a_Index);
    bool EditPath(UINT a_Index, AnsiString a_newPath);    
    bool AddPath(AnsiString a_Path);

public:
    bool Load(TIniFile *a_ini);
    bool Save(TIniFile *a_ini);
};


typedef vector<AnsiString> TFtStringList;
//---------------------------------------------------------------------------
//��������� ��� ���������..
//---------------------------------------------------------------------------
class CFtTorrentsOpt
{
private:
    TFtStringList m_vExcTrackers; //������ ������������ ��������

public:
    CFtTorrentsOpt();
    ~CFtTorrentsOpt();

    CFtTorrentsOpt & operator = ( const CFtTorrentsOpt & v)
    {
        m_vExcTrackers = v.m_vExcTrackers;
        return *this;
    }

public:
    UINT GetExcTrackersCount(void);
    AnsiString GetExcTracker(UINT a_nIndex);
    bool DeleteExcTracker(UINT a_nIndex);
    bool AddExcTracker(AnsiString a_Host);
    bool EditExcTracker(UINT a_nIndex, AnsiString a_Host);
    bool IsExcHost(AnsiString a_str); //��������� �� �������� �� ���� �����������

public:
    bool Load(TIniFile *a_ini);
    bool Save(TIniFile *a_ini);

private:
    void CreateDefaultExcTrackers(void);
};



//---------------------------------------------------------------------------
//���� � ��������� �������-�������
struct TFtClientInfo
{
    AnsiString m_Name;
    AnsiString m_UserAgent;
    AnsiString m_PeerId; //%xx - ������ ������ ��������� ������(������)
};
//---------------------------------------------------------------------------
//��������� ��� ����������� ���������..
//---------------------------------------------------------------------------
class CFtVirtTorrentsOpt
{
private:
    //�������������� ��� �������
    UINT m_nVirtPort; //"��������������" �������� ����
    UINT m_nCurrentCltIndex; //������ �������� �������
    vector<TFtClientInfo> m_vCltInfo; //���� � �������-��������    

public:
    CFtVirtTorrentsOpt();
    ~CFtVirtTorrentsOpt();

    CFtVirtTorrentsOpt & operator = ( const CFtVirtTorrentsOpt & v)
    {
        m_nVirtPort = v.m_nVirtPort;
        m_nCurrentCltIndex = v.m_nCurrentCltIndex;
        m_vCltInfo = v.m_vCltInfo;
        return *this;
    }

public:
    UINT GetVirtPort(void);
    bool SetVirtPort(UINT a_nVirtPort);

    UINT GetCltInfoCount(void);
    TFtClientInfo *GetCltInfo(UINT a_nIndex);
    UINT GetCurrClt(void);
    bool SetCurrClt(UINT a_nCurrentCltIndex);

public:
    bool Load(TIniFile *a_ini);
    bool Save(TIniFile *a_ini);

private:
    //������ �������������� �������-��������
    void CreateCientsList(void);
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct CFtUpdateItem
{
    AnsiString m_sName; //�������� �������� (�����)
    UINT m_nLastUPdNumber; //����� ���������� ���������� ����� (������)
};
//---------------------------------------------------------------------------
//��������� �������� ���������...
//---------------------------------------------------------------------------
class CFtUpdaterOpt
{
private:
    UINT m_nLastUpdateNumber; //����� ���������� ����������
    vector<CFtUpdateItem> m_vItems;

public:
    CFtUpdaterOpt();
    ~CFtUpdaterOpt();

    CFtUpdaterOpt & operator = ( const CFtUpdaterOpt & v)
    {
        m_nLastUpdateNumber = v.m_nLastUpdateNumber;
        m_vItems = v.m_vItems;
        return *this;
    }

public:
    UINT GetLastUpdateNumber(void);
    bool SetLastUpdateNumber(UINT a_nLastUpdateNumber);

    UINT GetCountItems(void);
    CFtUpdateItem *GetItem(UINT a_nIndex);
    CFtUpdateItem *AddItem(void);
    CFtUpdateItem *GetCreateItemByName(AnsiString a_sName);

    bool Load(TIniFile *a_ini);
    bool Save(TIniFile *a_ini);    
};


//---------------------------------------------------------------------------
//������ ���������...
class CFtOtherOpt
{
private:
    bool m_bStartWithWin; //��������� �� � ����������� Windows
    bool m_bStartMinimized; //��������� � ��������� ����
    bool m_bCloseToTray; //��������� �� � ����.
    bool m_bLogToFile; //����� ��� � ����

    bool m_bSearchUpdate;    //��������� ���������� ��
    bool m_bFTUpdateAsk;     //���������� ���������� �� ���������� FT

public:
    CFtOtherOpt();
    ~CFtOtherOpt();

    CFtOtherOpt & operator = ( const CFtOtherOpt & v)
    {
        m_bStartWithWin = v.m_bStartWithWin;
        m_bStartMinimized = v.m_bStartMinimized;
        m_bCloseToTray = v.m_bCloseToTray;
        m_bSearchUpdate = v.m_bSearchUpdate;
        m_bFTUpdateAsk = v.m_bFTUpdateAsk;
        m_bLogToFile = v.m_bLogToFile;
        return *this;
    }

public:
    bool GetStartWithWin(void);
    bool SetStartWithWin(bool a_bStartWithWin);
    bool GetStartMinimized(void);
    bool SetStartMinimized(bool a_bStartMinimized);

    bool GetCloseToTray(void);
    bool SetCloseToTray(bool a_bCloseToTray);
    bool GetSearchUpdate(void);
    bool SetSearchUpdate(bool a_bSearchUpdate);
    bool GetFTUpdateAsk(void);
    bool SetFTUpdateAsk(bool a_bFTUpdateAsk);

    bool GetLogToFile(void);
    bool SetLogToFile(bool a_bLogToFile);

public:
    bool Load(TIniFile *a_ini);
    bool Save(TIniFile *a_ini);
};




//---------------------------------------------------------------------------
//��������� ��� ���������� ����������� �����
struct CFtViewFormStruct
{
};
//---------------------------------------------------------------------------
//��������� �����������
class CFtViewOpt
{
private:
    CFtViewFormStruct m_ViewForm;

public:
    CFtViewOpt();
    ~CFtViewOpt();

    CFtViewOpt & operator = ( const CFtViewOpt & v)
    {
        m_ViewForm = v.m_ViewForm;
        return *this;
    }

public:    
    CFtViewFormStruct *GetViewFormStruct(void);
};



//---------------------------------------------------------------------------
//��� ��������� ���������
class CFtOptions
{
private:
    CFtNetOpt           m_NetOpt;
    CFtTorrentCltOpt    m_TorrentCltOpt;
    CFtTorrentsOpt      m_TorrentsOpt;
    CFtVirtTorrentsOpt  m_VirtTorrentsOpt;
    CFtOtherOpt         m_OtherOpt;
    CFtViewOpt          m_ViewOpt;
    CFtUpdaterOpt       m_UpdaterOpt;

public:
    CFtOptions();
    ~CFtOptions();

    CFtOptions & operator = ( const CFtOptions & v)
    {
        m_NetOpt = v.m_NetOpt;
        m_TorrentCltOpt = v.m_TorrentCltOpt;
        m_TorrentsOpt = v.m_TorrentsOpt;
        m_VirtTorrentsOpt = v.m_VirtTorrentsOpt;
        m_OtherOpt = v.m_OtherOpt;
        m_ViewOpt = v.m_ViewOpt;
        m_UpdaterOpt = v.m_UpdaterOpt;
        return *this;
    }

public:
    CFtNetOpt *GetNetOpt(void);
    CFtTorrentCltOpt *GetTorrentCltOpt(void);
    CFtTorrentsOpt *GetTorrentsOpt(void);
    CFtVirtTorrentsOpt *GetVirtTorrentsOpt(void);
    CFtOtherOpt *GetOtherOpt(void);
    CFtViewOpt *GetViewOpt(void);
    CFtUpdaterOpt *GetUpdaterOpt(void);    

public:
    bool Load(AnsiString a_fname);
    bool Save(AnsiString a_fname);
};

//---------------------------------------------------------------------------
extern TCriticalSection *g_CSOptions;
//---------------------------------------------------------------------------

#endif
