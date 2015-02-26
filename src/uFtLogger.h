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

#ifndef uFtLoggerH
#define uFtLoggerH

#include <SyncObjs.hpp>
#include <vector.h>
#include "uFtOptions.h"

#define FT_BUFFERING_LAST_SIZE   500 //���-�� ����� �������� � ������
#define FT_DEFAULT_LOGFNAME      "freetorrents.log"

//---------------------------------------------------------------------------
struct CFTLogData
{
    AnsiString m_Time;    //�����
    AnsiString m_String;  //������ ���� (������)
    AnsiString m_Label;   //����� (���)
};

//---------------------------------------------------------------------------
class CFtLogger
{
private:
    UINT m_nBufStartPointer; //������ ������ ������ � ����������� ������
    vector<CFTLogData> m_vBuffer;

    bool m_bBuffering; //���������� � ������ (��/���)
    bool m_bSaveToFile; //���������� ��� � ���� (��/���)

    bool m_bNewData;

public:
    CFtLogger();
    ~CFtLogger();

public:
    bool IsNewData(void);
    bool AddString(AnsiString a_sStr, AnsiString a_sLabel);

    UINT GetItemsCount(void);
    CFTLogData *GetItem(UINT a_Index);

    bool SetSaveToFile(bool a_bSaveToFile);
    bool SetBuffering(bool a_bBuffering);

private:
    void ClearAll(void);
    bool AddStringToFile(CFTLogData *a_Data);
};

//---------------------------------------------------------------------------
class CFtSrvLogger : public CFtLogger
{
public:
    bool Init(CFtOtherOpt *a_pOtherOpt);
    bool UnInit(void);
};

//---------------------------------------------------------------------------
extern TCriticalSection *g_CSLogger;
//---------------------------------------------------------------------------
#endif
