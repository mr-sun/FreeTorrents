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
#include <stdio.h>
#pragma hdrstop

#include "uFtLogger.h"

#pragma package(smart_init)

TCriticalSection *g_CSLogger = new TCriticalSection; 

//---------------------------------------------------------------------------
CFtLogger::CFtLogger()
{
    ClearAll();
    m_nBufStartPointer = 0;
    m_bBuffering = false;
    m_bSaveToFile = false;
    m_bNewData = true;
}
CFtLogger::~CFtLogger()
{
}
//---------------------------------------------------------------------------
void CFtLogger::ClearAll(void)
{
    m_vBuffer.clear();
}
//---------------------------------------------------------------------------
bool CFtLogger::IsNewData(void)
{
    if(m_bNewData)
    {
        m_bNewData = false;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool CFtLogger::AddString(AnsiString a_sStr, AnsiString a_sLabel)
{
    CFTLogData tmpData;
    tmpData.m_String = a_sStr;
    tmpData.m_Time = DateToStr(Date()) +" "+  TimeToStr(Time());
    tmpData.m_Label = a_sLabel;

    if(m_bBuffering)
    {
        if(m_vBuffer.size() < FT_BUFFERING_LAST_SIZE)
        {
            m_vBuffer.push_back(tmpData);
            m_nBufStartPointer = 0;
        }
        else
        {
            m_vBuffer[m_nBufStartPointer] = tmpData;
            m_nBufStartPointer++;
        }
        if(m_nBufStartPointer == FT_BUFFERING_LAST_SIZE)
            m_nBufStartPointer = 0;

        m_bNewData = true;
    }

    //Дописываем новую строку в файл
    if(m_bSaveToFile)
    {
        AddStringToFile(&tmpData);
    }

    return true;
}

//---------------------------------------------------------------------------
bool CFtLogger::AddStringToFile(CFTLogData *a_Data)
{
    AnsiString fdir = ExtractFilePath(Application->ExeName) + "\\logs";
    if(!DirectoryExists(fdir))
        ForceDirectories(fdir);

    AnsiString fname = fdir + "\\" + FT_DEFAULT_LOGFNAME;
    FILE *fl = fopen(fname.c_str(), "a");
    if(fl)
    {
        AnsiString sStr = "["+a_Data->m_Time+"] ["+a_Data->m_Label+"]\r\n"+a_Data->m_String;
        sStr+="\r\n\r\n";
        fwrite(sStr.c_str(), sStr.Length(), 1, fl);
        fclose(fl);
        return true;
    }
    return false;
}

//---------------------------------------------------------------------------
UINT CFtLogger::GetItemsCount(void)
{
    return m_vBuffer.size();
}

//---------------------------------------------------------------------------
CFTLogData *CFtLogger::GetItem(UINT a_Index)
{
    if(a_Index >= m_vBuffer.size())
        return 0;
    //Вычисляем реальный индекс в циклическом буфере
    UINT realIndex = m_nBufStartPointer + a_Index;
    if(realIndex >= FT_BUFFERING_LAST_SIZE)
    {
        realIndex = realIndex - FT_BUFFERING_LAST_SIZE;
    }
    return &m_vBuffer[realIndex];
}

//---------------------------------------------------------------------------
bool CFtLogger::SetSaveToFile(bool a_bSaveToFile)
{
    m_bSaveToFile = a_bSaveToFile;
    return true;
}

//---------------------------------------------------------------------------
bool CFtLogger::SetBuffering(bool a_bBuffering)
{
    m_bBuffering = a_bBuffering;
    return true;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtSrvLogger
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CFtSrvLogger::Init(CFtOtherOpt *a_pOtherOpt)
{
    if(!a_pOtherOpt) return false;
    bool bLogToFile = a_pOtherOpt->GetLogToFile();
    SetSaveToFile(bLogToFile);
    return true;
}
//---------------------------------------------------------------------------
bool CFtSrvLogger::UnInit(void)
{
    SetSaveToFile(false);
    SetBuffering(false);
    return true;
}

//---------------------------------------------------------------------------

