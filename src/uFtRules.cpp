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
#pragma hdrstop

#include "uFtRules.h"
#include "uFtCore.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CFtRule
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtRule::CFtRule()
{
    m_bUse = true;
    m_bUseKDownload = true;
    m_bUseKUpload = true;
    m_fPercentDownload = FT_RULE_DOWNDEFPERCENT; //Коэффициент объема скачки
    m_fKUpload = FT_RULE_UPDEFCOEFF;   //Коэффициент объема отдачи
}
CFtRule::~CFtRule()
{
}
//---------------------------------------------------------------------------
bool CFtRule::SetName(AnsiString a_sName)
{
    m_sName = a_sName;
    return true;
}
//---------------------------------------------------------------------------
AnsiString CFtRule::GetName(void)
{
    return m_sName;
}
//---------------------------------------------------------------------------
bool CFtRule::SetHost(AnsiString a_sHost)
{
    m_sHost = a_sHost;
    return true;
}
//---------------------------------------------------------------------------
AnsiString CFtRule::GetHost(void)
{
    return m_sHost;
}
//---------------------------------------------------------------------------
bool CFtRule::SetTorrentHash(AnsiString a_sTorrentHash)
{
    m_sTorrentHash = a_sTorrentHash;
    return true;
}
//---------------------------------------------------------------------------
AnsiString CFtRule::GetTorrentHash(void)
{
    return m_sTorrentHash;
}
//---------------------------------------------------------------------------
bool CFtRule::SetPercentDownload(double a_fPercentDownload)
{
    if(a_fPercentDownload > 100) a_fPercentDownload = 100;
    m_fPercentDownload = a_fPercentDownload;
    return true;
}
//---------------------------------------------------------------------------
double CFtRule::GetPercentDownload(void)
{
    double perc = m_fPercentDownload;
    if(g_Ver)
    {
        if(g_Ver->GetProgrType()==FTTYPE_VER_LIGHT)
        {
            if(m_fPercentDownload < FT_RULE_DOWNDEFPERCENT)
                perc = FT_RULE_DOWNDEFPERCENT;
        }
    }
    return perc;
}
//---------------------------------------------------------------------------
bool CFtRule::SetKUpload(double a_fKUpload)
{
    m_fKUpload = a_fKUpload;
    return true;
}
//---------------------------------------------------------------------------
double CFtRule::GetKUpload(void)
{
    double fUp = m_fKUpload;
    if(g_Ver)
    {
        if(g_Ver->GetProgrType()==FTTYPE_VER_LIGHT)
        {
            if(fUp > FT_RULE_UPDEFCOEFF)
                fUp = FT_RULE_UPDEFCOEFF;
        }
    }
    return fUp;
}
//---------------------------------------------------------------------------
bool CFtRule::SetUseKDownload(bool a_bUseKDownload)
{
    m_bUseKDownload = a_bUseKDownload;
    return true;
}
//---------------------------------------------------------------------------
bool CFtRule::GetUseKDownload(void)
{
    return m_bUseKDownload;
}
//---------------------------------------------------------------------------
bool CFtRule::SetUseKUpload(bool a_bUseKUpload)
{
    m_bUseKUpload = a_bUseKUpload;
    return true;
}
//---------------------------------------------------------------------------
bool CFtRule::GetUseKUpload(void)
{
    return m_bUseKUpload;
}
//---------------------------------------------------------------------------
bool CFtRule::SetUse(bool a_bUse)
{
    m_bUse = a_bUse;
    return true;
}
//---------------------------------------------------------------------------
bool CFtRule::GetUse(void)
{
    return m_bUse;
}



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// CFtRulesMgr
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CFtRulesMgr::CFtRulesMgr()
{
}
CFtRulesMgr::~CFtRulesMgr()
{
}
//---------------------------------------------------------------------------
CFtRule *CFtRulesMgr::AddRule(CFtRule *a_Rule)
{
    if(!a_Rule) return 0;
    CFtRule tmp = *a_Rule;
    m_vRules.push_back(tmp);
    return &m_vRules[ m_vRules.size() -1 ];
}
//---------------------------------------------------------------------------
CFtRule *CFtRulesMgr::AddRule(AnsiString a_sName)
{
    CFtRule tmp;
    tmp.SetName(a_sName);
    m_vRules.push_back(tmp);
    return &m_vRules[ m_vRules.size() -1 ];
}
//---------------------------------------------------------------------------
CFtRule *CFtRulesMgr::GetRule(UINT a_nIndex)
{
    if(a_nIndex >= m_vRules.size())
        return 0;
    return &m_vRules[a_nIndex];
}

//---------------------------------------------------------------------------
//Получить Правило для конкретного торрента по хосту и хешу
CFtRule *CFtRulesMgr::GetRule(AnsiString a_Host, AnsiString a_Hash)
{
    int tmpStatus = 0;
    CFtRule *tmp = 0;
    UINT cou = GetCount();
    for(UINT a=0; a < cou; a++)
    {
        CFtRule *rule = &m_vRules[a];
        AnsiString sHash = rule->GetTorrentHash();
        AnsiString sHostPattern = rule->GetHost();

        //проверка вхождения хоста в подмножество по шаблону правила.
        bool isInHosts = CompareHosts(a_Host.Trim(), sHostPattern.Trim());

        if(isInHosts && a_Hash==sHash)
        {
            //tmpStatus = FT_RSTATUS_HASHHOST;
            return rule;
        }
        else if((a_Hash==sHash) && (sHostPattern=="") && tmpStatus < FT_RSTATUS_HASHHOST)
        {
            tmpStatus = FT_RSTATUS_HASH;
            tmp = rule;
        }
        else if(isInHosts && (sHash=="") && tmpStatus < FT_RSTATUS_HASH)
        {
            tmpStatus = FT_RSTATUS_HOST;
            tmp = rule;
        }
    }
    return tmp;
}

//---------------------------------------------------------------------------
bool CFtRulesMgr::DeleteRule(UINT a_nIndex)
{
    if(a_nIndex >= m_vRules.size())
        return false;
    m_vRules.erase( m_vRules.begin() + a_nIndex);
    return true;
}
//---------------------------------------------------------------------------
UINT CFtRulesMgr::GetCount()
{
    return m_vRules.size();
}

//---------------------------------------------------------------------------
bool CFtRulesMgr::Load(AnsiString a_fname)
{
    TIniFile *ini = new TIniFile(a_fname);
    if(!ini) return false;

    m_vRules.clear();

    //Первый раз (если файла еще нет) создаем набор по умолчанию
    if(!FileExists(a_fname))
        CreateDefaultRules();
    
    AnsiString Section = "MainRules";
    UINT cou = ini->ReadInteger(Section, "Count", 0);
    for(UINT a=0; a < cou; a++)
    {
        AnsiString Section = "Rule_" + IntToStr(a);
        CFtRule rule;

        rule.SetHost( ini->ReadString(Section, "Host", "") );
        rule.SetName( ini->ReadString(Section, "Name", "") );
        rule.SetTorrentHash( ini->ReadString(Section, "TorrentHash", "") );

        rule.SetUse( ini->ReadBool(Section, "Use", true) );
        rule.SetUseKDownload( ini->ReadBool(Section, "UseKDownload", true) );
        rule.SetUseKUpload( ini->ReadBool(Section, "UseKUpload", true) );

        rule.SetPercentDownload( ini->ReadFloat(Section, "PercentDownload", 100) );
        rule.SetKUpload( ini->ReadFloat(Section, "KUpload", 1) );

        m_vRules.push_back(rule);
    }
    return true;
}
//---------------------------------------------------------------------------
bool CFtRulesMgr::Save(AnsiString a_fname)
{
    TIniFile *ini = new TIniFile(a_fname);
    if(!ini) return false;

    AnsiString Section = "MainRules";
    UINT cou = m_vRules.size();
    ini->WriteInteger(Section, "Count", cou);
    for(UINT a=0; a < cou; a++)
    {
        AnsiString Section = "Rule_" + IntToStr(a);
        CFtRule *rule = &m_vRules[a];
        if(!rule) continue;

        ini->WriteString(Section, "Host", rule->GetHost());
        ini->WriteString(Section, "Name", rule->GetName());
        ini->WriteString(Section, "TorrentHash", rule->GetTorrentHash());

        ini->WriteBool(Section, "Use", rule->GetUse());
        ini->WriteBool(Section, "UseKDownload", rule->GetUseKDownload());
        ini->WriteBool(Section, "UseKUpload", rule->GetUseKUpload());

        ini->WriteFloat(Section, "PercentDownload", rule->GetPercentDownload());
        ini->WriteFloat(Section, "KUpload", rule->GetKUpload());        
    }

    return true;
}

//---------------------------------------------------------------------------
//Создает первоначальный набор правил для примера...
bool CFtRulesMgr::CreateDefaultRules(void)
{
    CFtRule rule;
    rule.SetName("По умолчанию");
    rule.SetHost("*");
    rule.SetTorrentHash("");
    rule.SetUse(true);
    rule.SetUseKDownload(true);
    rule.SetUseKUpload(true);
    rule.SetKUpload(FT_RULE_UPDEFCOEFF);
    rule.SetPercentDownload(FT_RULE_DOWNDEFPERCENT);
    m_vRules.push_back(rule);

    rule.SetName("Для torrents.ru");
    rule.SetHost("*.torrents.ru");
    rule.SetTorrentHash("");
    rule.SetUse(true);
    rule.SetUseKDownload(true);
    rule.SetUseKUpload(true);
    rule.SetKUpload(FT_RULE_UPDEFCOEFF);
    rule.SetPercentDownload(FT_RULE_DOWNDEFPERCENT);
    m_vRules.push_back(rule);

    return true;
}


