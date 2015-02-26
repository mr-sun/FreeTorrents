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

#include<vcl.h>
#include<dirent.h>
#include<stdio.h>
#pragma hdrstop

#include "uFtTorrentInfo.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
CFtTorrentInfo::CFtTorrentInfo()
{
    m_Pieces = 0;
    m_PieceLength = 0;
}
CFtTorrentInfo::~CFtTorrentInfo()
{
    UnInit();
}

//---------------------------------------------------------------------------
/* Читаем и заполняем сруктуру из файла .torrent */
bool CFtTorrentInfo::Init(void)
{
    if(!FileExists(m_FileName)) return false;
    return ReadTorrentInfo();
}

//---------------------------------------------------------------------------
bool CFtTorrentInfo::UnInit(void)
{
    m_Pieces = 0;
    m_PieceLength = 0;
    m_Info = "";
    m_InfoHash = "";
    m_vTrackers.clear();
    return true;
}

//---------------------------------------------------------------------------
bool CFtTorrentInfo::ReadTorrentInfo(void)
{
    m_vTrackers.clear();
    int iFile = FileOpen(m_FileName, fmOpenRead);
    if(iFile)
    {
        int fsize = FileSeek(iFile, 0, 2);
        if(fsize > 0)
        {
            FileSeek(iFile, 0, 0);
            BYTE *buf = new BYTE[fsize+1];
            memset(buf, 0, fsize+1);
            FileRead(iFile, buf, fsize);
            FileClose(iFile);
            char *buf_end = buf+fsize; //Указатель на конец буфера (на нулевой байт)

            //*********************************************************
            //Парсим полученный буфер, выбирая инфу о размере "кусочка"
            //TR_PIECE_LENGTH_TEXT
            char *piece_len = StrPos(buf, TR_PIECE_LENGTH_TEXT);
            if(piece_len)
            {
                piece_len += strlen(TR_PIECE_LENGTH_TEXT);
                char *e = StrPos(piece_len, "e");
                int sz = e-piece_len;
                char piece_len_buf[256];
                memset(piece_len_buf, 0, 256);
                strncpy(piece_len_buf, piece_len, sz);
                AnsiString sPieceLength = piece_len_buf;
                try
                {
                    m_PieceLength = sPieceLength.ToInt();
                }
                catch(...)
                {
                    //Устанавливаем значение кусочка по умолчанию
                    m_PieceLength = DEFAULT_PIECE_LENGTH;
                }
            }
            else
            {
                //Устанавливаем значение кусочка по умолчанию
                m_PieceLength = DEFAULT_PIECE_LENGTH;
            }

            //****************************************************
            //Ищем список трекеров... (announce)
            char *announce = StrPos(buf, TR_ANNOUNCE_TEXT);
            if(announce)
            {
                announce += strlen(TR_ANNOUNCE_TEXT);
                char *e = StrPos(announce, ":");
                if(e)
                {
                    int sz = e-announce;
                    char buf[512];
                    memset(buf, 0, 512);
                    strncpy(buf, announce, sz);
                    announce += sz + 1;                    
                    AnsiString strLen = buf;
                    int slen = 0;
                    try
                    {
                        slen = strLen.ToInt();
                        memset(buf, 0, 512);
                        strncpy(buf, announce, slen);
                        AnsiString sannounce = buf;
                        AnsiString sHost, sReq;
                        UINT nPort;
                        GetHostPortFromURL(sannounce, sReq, sHost, nPort);
                        TFtTracker tr;
                        tr.m_bUse = true;
                        tr.m_sAnnounce = sReq;
                        tr.m_sTracker = sHost;
                        tr.m_nPort = nPort;
                        if(sHost!="" && sReq!="" && IsUnicalTracker(sHost))
                            m_vTrackers.push_back(tr);
                    }
                    catch(...){ /* Ошибка формата файла */ }
                }
            }

            //****************************************************
            //Ищем список трекеров... (announce-list)
            char *anlist = StrPos(buf, TR_ANNOUNCELIST_TEXT);
            if(anlist)
            {
                anlist += strlen(TR_ANNOUNCELIST_TEXT);
                UINT beg_cou = 0;
                //for(char *p = anlist; p[0]!='\x00';p++)
                for(char *p = anlist; p < buf_end; p++)
                {
                    if(p[0]=='l')
                    {
                        beg_cou ++;
                        continue;
                    }
                    if(p[0]=='0' || p[0]=='1' || p[0]=='2' || p[0]=='3' || p[0]=='4'
                       || p[0]=='5' || p[0]=='6' || p[0]=='7' || p[0]=='8' || p[0]=='9')
                    {
                        char *e = StrPos(p, ":");
                        if(e)
                        {
                            int sz = e-p;
                            char buf[512];
                            memset(buf, 0, 512);
                            strncpy(buf, p, sz);
                            AnsiString strLen = buf;
                            int slen;
                            try
                            {
                                slen = strLen.ToInt();
                                memset(buf, 0, 512);
                                strncpy(buf, e+1, slen);
                                AnsiString sannounce = buf;
                                AnsiString sReq, sHost;
                                UINT nPort;
                                GetHostPortFromURL(sannounce, sReq, sHost, nPort);
                                TFtTracker tr;
                                tr.m_bUse = false;
                                tr.m_sAnnounce = sReq;
                                tr.m_sTracker = sHost;
                                tr.m_nPort = nPort;
                                if(sHost!="" && sReq!="" && IsUnicalTracker(sHost))
                                    m_vTrackers.push_back(tr);
                            }
                            catch(...)
                            {
                                //Ошибка формата файла
                                break;
                            }
                            p = e + slen;
                            continue;
                        }
                        else
                        {
                            //Ошибка формата файла
                            break;
                        }
                    }
                    if(p[0]=='e')
                    {
                        beg_cou --;
                        if(beg_cou == 0)
                        {
                            //ВЫХОД...
                            break;
                        }
                        continue;
                    }
                }
            }

            
            //****************************************************
            //Парсим, выделяя раздел "info" для создания info_hash
            AnsiString sInfo = "";
            char *info = StrPos(buf, TR_INFO_TEXT);
            if(info)
            {
                //Пробегаем в цикле по всей структуре "info" и считаем кол-во
                //входов во вложения и выходов из них.
                //Захватываем начальное и конечное 4:info[d...e]
                UINT begin_cou = 0;
                char *info_start = info+strlen(TR_INFO_TEXT)-1;
                char *info_end = info_start;
                //for(char *p=info_start; p[0]!='\x00'; p++ )
                for(char *p=info_start; p < buf_end; p++ )
                {
                    if(p[0]=='d' || p[0]=='l')
                    {
                        begin_cou ++;
                        continue;
                    }
                    if(p[0]=='i')
                    {
                        //Пробегаем до ближайшего "e"
                        char *e = StrPos(p, "e");
                        if(e)
                        {
                          p=e;
                          continue;
                        }
                        else
                        {
                            //Ошибка формата
                            break;
                        }
                    }
                    if(p[0]=='0' || p[0]=='1' || p[0]=='2' || p[0]=='3' || p[0]=='4'
                       || p[0]=='5' || p[0]=='6' || p[0]=='7' || p[0]=='8' || p[0]=='9')
                    {
                        char *e = StrPos(p, ":");
                        if(e)
                        {
                            int sz = e-p;
                            char dig_buf[256];
                            memset(dig_buf, 0, 256);
                            strncpy(dig_buf, p, sz);
                            AnsiString strLen = dig_buf;
                            int slen;
                            try{
                            slen = strLen.ToInt();
                            }catch(...)
                            {
                                //Ошибка формата файла
                                break;
                            }
                            p = e + slen;
                            continue;
                        }
                        else
                        {
                            //Ошибка формата файла
                            break;
                        }
                    }
                    if(p[0]=='e')
                    {
                        begin_cou --;
                        if(begin_cou == 0)
                        {
                            //ВЫХОД...
                            info_end = p;
                            break;
                        }
                        continue;
                    }

                    //Ошибка формата - выход
                    break;
                }

                if(info_end != info_start)
                {
                    //вот он, кусочек Info
                    int sz = info_end - info_start;
                    SHA1Hash(info_start, sz+1, m_mHash);
                    m_InfoHash = FtURLEncode( m_mHash );
                    //RFC 1738
                    //m_InfoHash = FtURLEncode(m_InfoHash);
                }
            }
            delete [] buf;
        }
    }
    return true;
}

//---------------------------------------------------------------------------
bool CFtTorrentInfo::SHA1Hash(unsigned char *a_data, int a_length, BYTE *a_resData)
{
    if(!a_resData) return false;

    SHA1_CTX context;
    unsigned char digest[21];
    digest[20] = 0;
    SHA1Init(&context);
    SHA1Update(&context, a_data, a_length);
    SHA1Final(digest, &context);
    memcpy(a_resData, digest, 20);
    return true;
}

//---------------------------------------------------------------------------
bool CFtTorrentInfo::IsUnicalTracker(AnsiString a_sAnnounce)
{
    UINT cou = m_vTrackers.size();
    for(UINT a=0; a < cou; a++)
    {
        if(m_vTrackers[a].m_sTracker == a_sAnnounce)
        {
            return false;
        }
    }
    return true;
}

//---------------------------------------------------------------------------
AnsiString CFtTorrentInfo::GetFileName(void)
{
    return m_FileName;
}
bool CFtTorrentInfo::SetFileName(AnsiString a_FileName)
{
    m_FileName = a_FileName;
    return true;
}
//---------------------------------------------------------------------------
AnsiString CFtTorrentInfo::GetInfo(void)
{
    return m_Info;
}
bool CFtTorrentInfo::SetInfo(AnsiString a_Info)
{
    m_Info = a_Info;
    return true;    
}
//---------------------------------------------------------------------------
AnsiString CFtTorrentInfo::GetInfoHash(void)
{
    return m_InfoHash;
}
bool CFtTorrentInfo::SetInfoHash(AnsiString a_InfoHash)
{
    m_InfoHash = a_InfoHash;
    return true;
}
//---------------------------------------------------------------------------
UINT CFtTorrentInfo::GetPieces(void)
{
    return m_Pieces;
}
bool CFtTorrentInfo::SetPieces(UINT a_Pieces)
{
    m_Pieces = a_Pieces;
    return true;
}
//---------------------------------------------------------------------------
UINT CFtTorrentInfo::GetPieceLength(void)
{
    return m_PieceLength;
}
bool CFtTorrentInfo::SetPieceLength(UINT a_PieceLength)
{
    m_PieceLength = a_PieceLength;
    return true;
}
//---------------------------------------------------------------------------
UINT CFtTorrentInfo::GetTrackersCount(void)
{
    return m_vTrackers.size();
}
TFtTracker *CFtTorrentInfo::GetTracker(UINT a_nIndex)
{
    if(a_nIndex >= m_vTrackers.size())
        return 0;
    return &m_vTrackers[a_nIndex];
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//  CFtTorrentInfoMgr
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtTorrentInfoMgr::CFtTorrentInfoMgr()
{
}
CFtTorrentInfoMgr::~CFtTorrentInfoMgr()
{
}

//---------------------------------------------------------------------------
bool CFtTorrentInfoMgr::Init(TFtPathsList a_Paths)
{
    bool bRes = Initialize(a_Paths);

    UINT cou = m_vInfoMgr.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vInfoMgr[a].Init();
    }
    return bRes;
}

//---------------------------------------------------------------------------
bool CFtTorrentInfoMgr::UnInit(void)
{
    bool bRes = true;
    UINT cou = m_vInfoMgr.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= m_vInfoMgr[a].UnInit();
    }
    return bRes;
}

//---------------------------------------------------------------------------
//Собирает инфу со всех .torrent файлов по указанным путям
bool CFtTorrentInfoMgr::Initialize(TFtPathsList a_Paths)
{
    bool bRes = true;
    UINT cou = a_Paths.size();
    for(UINT a=0; a < cou; a++)
    {
        bRes &= Search(a_Paths[a]);
    }
    return bRes;
}

//---------------------------------------------------------------------------
bool CFtTorrentInfoMgr::Search(AnsiString a_Path)
{
    if(!DirectoryExists(a_Path)) return false;
    //Перебираем все файлы в этой директории
    struct dirent *ent;
    DIR *dir = opendir(a_Path.c_str());
    if(dir)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            //получили внутреннюю структуру папок и файлов
            AnsiString sdir = ent->d_name;
            if(sdir!="." && sdir!="..")
            {
                //Файлы и папки в директории
                AnsiString fname = a_Path + "\\" + sdir;
                AnsiString ext =ExtractFileExt(fname);
                if(ext == ".torrent")
                {
                    if(FileExists(fname))
                    {
                        //Это torrent-файл, добавляем его в список
                        CFtTorrentInfo tmp;
                        tmp.SetFileName(fname);
                        m_vInfoMgr.push_back(tmp);
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
CFtTorrentInfo *CFtTorrentInfoMgr::GetTorrentInfo(AnsiString a_Hash)
{
    UINT cou = m_vInfoMgr.size();
    for(UINT a=0; a < cou; a++)
    {
        if(m_vInfoMgr[a].GetInfoHash() == a_Hash)
          return &m_vInfoMgr[a];
    }
    return 0;
}


