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

#ifndef uFtRulesH
#define uFtRulesH

#include <inifiles.hpp>
#include <vector.h>

#include "uFtFunctions.h"

//������������ �� ��������� (��� ���������� ������ ��������������)
#define FT_RULE_UPDEFCOEFF      1.2
#define FT_RULE_DOWNDEFPERCENT  75

//�������� �������� ������ (��� ������ ����� - ��� ������ �������)
#define FT_RSTATUS_HASHHOST  30 //������� � ���� � ��� ��������
#define FT_RSTATUS_HASH      20 //������ ��� ��������
#define FT_RSTATUS_HOST      10 //������ ���� ��������

//---------------------------------------------------------------------------
//�������
class CFtRule
{
private:
    AnsiString m_sName;        //�������� �������
    AnsiString m_sHost;        //����� ����� (��������: *.torrents.ru)
    AnsiString m_sTorrentHash; //��� �������-�����

    bool m_bUse;  //���������� �������.
    bool m_bUseKDownload;
    bool m_bUseKUpload;
    double m_fPercentDownload; //������������ ������� ������
    double m_fKUpload;   //����������� ������ ������

public:
    CFtRule();
    ~CFtRule();

    CFtRule & operator = ( const CFtRule & v)
    {
        m_sName = v.m_sName;
        m_sHost = v.m_sHost;
        m_sTorrentHash = v.m_sTorrentHash;
        m_bUse = v.m_bUse;
        m_bUseKDownload = v.m_bUseKDownload;
        m_bUseKUpload = v.m_bUseKUpload;
        m_fPercentDownload = v.m_fPercentDownload;
        m_fKUpload = v.m_fKUpload;
        return *this;
    }

public:
    bool SetName(AnsiString a_sName);
    AnsiString GetName(void);
    bool SetHost(AnsiString a_sHost);
    AnsiString GetHost(void);
    bool SetTorrentHash(AnsiString a_sTorrentHash);
    AnsiString GetTorrentHash(void);
    bool SetPercentDownload(double a_fPercentDownload);
    double GetPercentDownload(void);
    bool SetKUpload(double a_fKUpload);
    double GetKUpload(void);
    bool SetUseKDownload(bool a_bUseKDownload);
    bool GetUseKDownload(void);
    bool SetUseKUpload(bool a_bUseKUpload);
    bool GetUseKUpload(void);
    bool SetUse(bool a_bUse);
    bool GetUse(void);
};


//---------------------------------------------------------------------------
class CFtRulesMgr
{
private:
    vector<CFtRule> m_vRules;

public:
    CFtRulesMgr();
    ~CFtRulesMgr();

    CFtRulesMgr & operator = ( const CFtRulesMgr & v)
    {
        m_vRules = v.m_vRules;
        return *this;
    }

public:
    //�-��� ������� � ��������
    CFtRule *AddRule(CFtRule *a_Rule);
    CFtRule *AddRule(AnsiString a_sName);
    CFtRule *GetRule(UINT a_nIndex);
    CFtRule *GetRule(AnsiString a_Host, AnsiString a_Hash);    
    bool DeleteRule(UINT a_nIndex);    
    UINT GetCount();

    bool Load(AnsiString a_fname);
    bool Save(AnsiString a_fname);

private:
    //������� �������������� ����� ������ ��� �������...
    bool CreateDefaultRules(void);

};

//---------------------------------------------------------------------------
#endif
