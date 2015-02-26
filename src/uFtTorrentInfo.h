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

#ifndef uFtTorrentInfoH
#define uFtTorrentInfoH

#include <vector.h>
#include "uFtOptions.h"
#include "sha1.h"
#include "uFtFunctions.h"

#define DEFAULT_PIECE_LENGTH  262144 //256 KB
#define TR_PIECE_LENGTH_TEXT  "12:piece lengthi"
#define TR_INFO_TEXT          "4:infod"
#define TR_ANNOUNCE_TEXT      "8:announce"
#define TR_ANNOUNCELIST_TEXT  "13:announce-list"

//---------------------------------------------------------------------------
//Структура описания одного треккера
struct TFtTracker
{
    bool m_bUse;
    AnsiString m_sAnnounce;
    AnsiString m_sTracker;//Host
    UINT m_nPort;
};
typedef vector<TFtTracker> TFtTrackerList;

//---------------------------------------------------------------------------
class CFtTorrentInfo
{
private:
    AnsiString m_FileName; //Вместе с путем...
    AnsiString m_Info;
    BYTE m_mHash[20];     //Байтовый хеш (20 байт)
    AnsiString m_InfoHash; //Строковый хеш

    UINT m_Pieces;
    UINT m_PieceLength;
    TFtTrackerList m_vTrackers;

public:
    CFtTorrentInfo();
    ~CFtTorrentInfo();

    CFtTorrentInfo & operator = ( const CFtTorrentInfo & v )
    {
        m_FileName = v.m_FileName;
        m_Info =v.m_Info;
        m_InfoHash =v.m_InfoHash;
        m_Pieces = v.m_Pieces;
        m_PieceLength = v.m_PieceLength;
        m_vTrackers = v.m_vTrackers;
        memcpy(m_mHash, v.m_mHash, 20);
        return *this;
    }

public:
    bool Init(void);
    bool UnInit(void);    

public:
    AnsiString GetFileName(void);
    bool SetFileName(AnsiString a_FileName);
    AnsiString GetInfo(void);
    bool SetInfo(AnsiString a_Info);
    AnsiString GetInfoHash(void);
    bool SetInfoHash(AnsiString a_InfoHash);
    UINT GetPieces(void);
    bool SetPieces(UINT a_Pieces);
    UINT GetPieceLength(void);
    bool SetPieceLength(UINT a_PieceLength);
    
    UINT GetTrackersCount(void);
    TFtTracker *GetTracker(UINT a_nIndex);

private:
    bool ReadTorrentInfo(void);
    bool SHA1Hash(unsigned char *a_data, int a_length, BYTE *a_resData);
    bool IsUnicalTracker(AnsiString a_sAnnounce);    
};


//---------------------------------------------------------------------------
class CFtTorrentInfoMgr
{
private:
    vector<CFtTorrentInfo> m_vInfoMgr;

public:
    CFtTorrentInfoMgr();
    ~CFtTorrentInfoMgr();

public:
    //Собирает инфу со всех .torrent файлов по указанным путям
    bool Initialize(TFtPathsList a_Paths);
    bool Init(TFtPathsList a_Paths);
    bool UnInit(void);

    CFtTorrentInfo *GetTorrentInfo(AnsiString a_Hash);

private:
    //Поиск файлов .torrent
    bool Search(AnsiString a_Path);
};

//---------------------------------------------------------------------------
#endif
