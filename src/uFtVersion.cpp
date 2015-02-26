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

#include "uFtVersion.h"

//---------------------------------------------------------------------------
//Список разрешенных кодов.
WORD g_ValidCode[500] =
    {
        0x69E9, 0x5752, 0xA3D3, 0x6244, 0xD6D5, 0xE428, 0xF160, 0x0E44, 0xBFD9, 0xC7F5, 0xB3B7, 0x20EF, 0xF4D8, 0xFAB4, 0xA923, 0xF656, 0x8664, 0x9E70, 0x9764, 0xD163, 0x4C48, 0x74CE, 0xC62C, 0x5750, 0x6100, 0x6581, 0x82A2, 0x102E, 0xA23C, 0xFBD5, 0x9BE1, 0xEA62, 0x3363, 0xE728, 0xE677, 0xB721, 0xF80C, 0x7C6A, 0xBCE8, 0xABFA, 0x1E85, 0x31A2, 0x772B, 0xE14A, 0x924E, 0x216A, 0xFA2A, 0xD6C7, 0x879D, 0x8C70,
        0x2845, 0xEAF8, 0xF2DA, 0x56F5, 0x6ACD, 0xE4B9, 0x0F22, 0x8550, 0x2B8B, 0x6DBA, 0xDCC6, 0xC469, 0x5D38, 0x92FE, 0x9ACB, 0xE441, 0xF405, 0x445E, 0x5169, 0x5867, 0x6812, 0xD906, 0x4C8E, 0xB782, 0x36F9, 0x8AF5, 0x975B, 0x3CCC, 0x5D43, 0xF57F, 0x86A3, 0x4892, 0xE3EF, 0x9BCD, 0x2E71, 0xDCB7, 0x50F0, 0x6259, 0xEC82, 0x2131, 0x2FAD, 0x1274, 0xDBC5, 0xE289, 0x9281, 0xD9C2, 0x60DF, 0x0E5D, 0x8D3B, 0xC9BB,
        0xA1AB, 0x39DF, 0xB79E, 0x572A, 0x07B4, 0x1A55, 0xAFE0, 0xDCAF, 0xC2BC, 0x0C99, 0x932C, 0x153F, 0x4337, 0x8F98, 0x8922, 0xA5A1, 0xC1CE, 0xC6D5, 0x3854, 0x90F0, 0xB6DE, 0xF9E6, 0x4DC9, 0xC01E, 0x61F9, 0x13F3, 0xF97B, 0x826D, 0x4562, 0x233C, 0x6FDB, 0x07F6, 0xA692, 0xDD92, 0x694A, 0xE525, 0x11B8, 0x18DB, 0xA2A0, 0x9639, 0xD332, 0x4F91, 0x5BA7, 0xF6C6, 0x7E18, 0x2341, 0xD89E, 0xC734, 0x69B0, 0xD50E,
        0xE2B9, 0x2E3F, 0x2FF8, 0xCE2E, 0x459D, 0x3677, 0x2E22, 0x1969, 0x52E7, 0x4AC0, 0x0711, 0xE59F, 0x5CA4, 0xBDD1, 0x22DF, 0x0641, 0x3CD8, 0x44B7, 0x2958, 0x73D5, 0xCFF9, 0x7851, 0x8A7E, 0xC774, 0x7573, 0x56D4, 0xF806, 0xC34E, 0x9224, 0xC146, 0xE8D7, 0xC822, 0x0FDF, 0xFD05, 0x1690, 0x5FC9, 0xD8EB, 0x343F, 0x0001, 0xB3AA, 0x5D83, 0xF379, 0x68AE, 0x1345, 0x1356, 0xAB0B, 0xD451, 0x73AB, 0xF207, 0x32A0,
        0x0F56, 0xEA02, 0xF7AC, 0x42B9, 0xA85F, 0x3303, 0xA150, 0xF729, 0x70C1, 0x2F7D, 0x7673, 0x5398, 0xFD03, 0x5FD2, 0x7E87, 0xBC02, 0x2C64, 0x6C77, 0xC473, 0x7A37, 0x9954, 0xA177, 0xDA11, 0x3B5A, 0x27F7, 0x6DEE, 0x54B2, 0x9164, 0x7CA8, 0xE490, 0x4A97, 0x71A1, 0x8471, 0x3D22, 0xD978, 0xB430, 0xEB96, 0x54FD, 0xA0FF, 0x9885, 0xA609, 0x2790, 0x454B, 0x3EE9, 0x69F5, 0x5BDD, 0xEF70, 0x2BF6, 0x2F36, 0xB6F9,
        0xCBF3, 0xC91D, 0xB8DD, 0xA48E, 0x8A50, 0xE522, 0x04AF, 0xB45B, 0xE077, 0x309D, 0xDBA4, 0x1AB0, 0x0448, 0xC079, 0x476A, 0x2DF5, 0x351D, 0x8F77, 0x16E1, 0xE822, 0x313F, 0xF9A9, 0x5A9F, 0x6C46, 0xBD6D, 0x41F9, 0xEB6C, 0x7A03, 0x225A, 0x0AA2, 0xE1C5, 0xFB0E, 0xAC2D, 0xE0B3, 0x1D5B, 0x7AB1, 0x2C01, 0xAE92, 0xC093, 0xA74B, 0x7565, 0x4775, 0xB005, 0xD4CC, 0xE1DB, 0xE0E8, 0x1800, 0x793B, 0xCB5B, 0x924C,
        0xE602, 0xB133, 0x14D0, 0xDB54, 0x4D42, 0xBD60, 0xA097, 0x1C89, 0x4228, 0x7E04, 0xD805, 0x1AC5, 0x02D1, 0xFD76, 0x28E0, 0x7FB9, 0x888E, 0xE59F, 0x80E3, 0x5869, 0x1964, 0x067E, 0xD53C, 0x96AC, 0xBB8C, 0xD2A3, 0xA00F, 0x90AF, 0xB7F2, 0xE1B5, 0x5BF3, 0x7D71, 0x1CB8, 0x6EEE, 0x0972, 0xFD47, 0x3EB2, 0xDA9C, 0xAD8B, 0x8F57, 0x31EA, 0x26A1, 0xD032, 0x1A1F, 0x501B, 0x7CF1, 0xFECC, 0x66ED, 0xBA37, 0xA518,
        0x47B5, 0xA266, 0xD1E3, 0x91B4, 0xC0BD, 0xDE02, 0xBCF8, 0xD4F5, 0x01A4, 0x908D, 0xE503, 0x2522, 0xA6A4, 0x11A5, 0x79C4, 0xE015, 0xD1E5, 0x1F40, 0xEDB0, 0x83BC, 0xA508, 0x5A25, 0x7570, 0x3398, 0xCA9F, 0xCA70, 0x8D62, 0x3640, 0xEEB2, 0x5611, 0x7215, 0x78DD, 0x3F3A, 0x4C4C, 0xA8CF, 0x09AD, 0xF4A6, 0xDD38, 0x7409, 0x9B82, 0xC45B, 0xA042, 0x60F0, 0x0643, 0x6B2E, 0x0DA9, 0xBCE4, 0xC568, 0x2BEA, 0x0101,
        0x1AFA, 0x797F, 0x46CF, 0x3EB2, 0xD07C, 0x64AB, 0x870F, 0xEA79, 0x7866, 0xED22, 0xB850, 0x0DE4, 0x70E8, 0x20AD, 0x2A2F, 0x16D8, 0x5CE9, 0x2C2A, 0x33B9, 0x550D, 0xA180, 0xE60B, 0xAAF3, 0x808E, 0xEA7B, 0x43D6, 0x6B51, 0x303B, 0xA6DA, 0xDFA5, 0xD740, 0x6349, 0x8652, 0x7BA6, 0x529E, 0xFABF, 0x01CD, 0x799A, 0xC53F, 0x3BF9, 0xD1E7, 0x6057, 0x1C68, 0x4E26, 0x7571, 0x5C48, 0xC856, 0xC1CC, 0xAFEF, 0xBCEA,
        0x3CB6, 0x0541, 0x051C, 0x754F, 0x712C, 0xF131, 0x4363, 0x9C9D, 0x64CE, 0x3D1D, 0xF565, 0xEE8E, 0x95A7, 0x06D8, 0xEF78, 0x49EF, 0x8837, 0x4F0A, 0xC824, 0x3223, 0x46F0, 0x7BB0, 0x58A6, 0x4FA0, 0xDBBA, 0xD698, 0x3F6F, 0xEC6E, 0xD76E, 0x0CE7, 0x60A3, 0x79AD, 0xCF5B, 0xAE94, 0x609D, 0x080E, 0xDBC6, 0xB2EC, 0x2968, 0xD157, 0x1864, 0xF10E, 0x6B45, 0xB0E2, 0x26E0, 0xA96F, 0x182A, 0xA31C, 0xA95A, 0xFB55
    };

//Каждый список содержит код для конкретного типа программы
WORD g_ValidCodeTypeNormal[150] =
    {
        0x69E9, 0x5752, 0xA3D3, 0x6244, 0xD6D5, 0xE428, 0xF160, 0x0E44, 0xBFD9, 0xC7F5, 0xB3B7, 0x20EF, 0xF4D8, 0xFAB4, 0xA923, 0xF656, 0x8664, 0x9E70, 0x9764, 0xD163, 0x4C48, 0x74CE, 0xC62C, 0x5750, 0x6100, 0x6581, 0x82A2, 0x102E, 0xA23C, 0xFBD5, 0x9BE1, 0xEA62, 0x3363, 0xE728, 0xE677, 0xB721, 0xF80C, 0x7C6A, 0xBCE8, 0xABFA, 0x1E85, 0x31A2, 0x772B, 0xE14A, 0x924E, 0x216A, 0xFA2A, 0xD6C7, 0x879D, 0x8C70,
        0x2845, 0xEAF8, 0xF2DA, 0x56F5, 0x6ACD, 0xE4B9, 0x0F22, 0x8550, 0x2B8B, 0x6DBA, 0xDCC6, 0xC469, 0x5D38, 0x92FE, 0x9ACB, 0xE441, 0xF405, 0x445E, 0x5169, 0x5867, 0x6812, 0xD906, 0x4C8E, 0xB782, 0x36F9, 0x8AF5, 0x975B, 0x3CCC, 0x5D43, 0xF57F, 0x86A3, 0x4892, 0xE3EF, 0x9BCD, 0x2E71, 0xDCB7, 0x50F0, 0x6259, 0xEC82, 0x2131, 0x2FAD, 0x1274, 0xDBC5, 0xE289, 0x9281, 0xD9C2, 0x60DF, 0x0E5D, 0x8D3B, 0xC9BB,
        0xA1AB, 0x39DF, 0xB79E, 0x572A, 0x07B4, 0x1A55, 0xAFE0, 0xDCAF, 0xC2BC, 0x0C99, 0x932C, 0x153F, 0x4337, 0x8F98, 0x8922, 0xA5A1, 0xC1CE, 0xC6D5, 0x3854, 0x90F0, 0xB6DE, 0xF9E6, 0x4DC9, 0xC01E, 0x61F9, 0x13F3, 0xF97B, 0x826D, 0x4562, 0x233C, 0x6FDB, 0x07F6, 0xA692, 0xDD92, 0x694A, 0xE525, 0x11B8, 0x18DB, 0xA2A0, 0x9639, 0xD332, 0x4F91, 0x5BA7, 0xF6C6, 0x7E18, 0x2341, 0xD89E, 0xC734, 0x69B0, 0xD50E
    };
WORD g_ValidCodeTypeProf[150] =
    {
        0xE2B9, 0x2E3F, 0x2FF8, 0xCE2E, 0x459D, 0x3677, 0x2E22, 0x1969, 0x52E7, 0x4AC0, 0x0711, 0xE59F, 0x5CA4, 0xBDD1, 0x22DF, 0x0641, 0x3CD8, 0x44B7, 0x2958, 0x73D5, 0xCFF9, 0x7851, 0x8A7E, 0xC774, 0x7573, 0x56D4, 0xF806, 0xC34E, 0x9224, 0xC146, 0xE8D7, 0xC822, 0x0FDF, 0xFD05, 0x1690, 0x5FC9, 0xD8EB, 0x343F, 0x0001, 0xB3AA, 0x5D83, 0xF379, 0x68AE, 0x1345, 0x1356, 0xAB0B, 0xD451, 0x73AB, 0xF207, 0x32A0,
        0x0F56, 0xEA02, 0xF7AC, 0x42B9, 0xA85F, 0x3303, 0xA150, 0xF729, 0x70C1, 0x2F7D, 0x7673, 0x5398, 0xFD03, 0x5FD2, 0x7E87, 0xBC02, 0x2C64, 0x6C77, 0xC473, 0x7A37, 0x9954, 0xA177, 0xDA11, 0x3B5A, 0x27F7, 0x6DEE, 0x54B2, 0x9164, 0x7CA8, 0xE490, 0x4A97, 0x71A1, 0x8471, 0x3D22, 0xD978, 0xB430, 0xEB96, 0x54FD, 0xA0FF, 0x9885, 0xA609, 0x2790, 0x454B, 0x3EE9, 0x69F5, 0x5BDD, 0xEF70, 0x2BF6, 0x2F36, 0xB6F9,
        0xCBF3, 0xC91D, 0xB8DD, 0xA48E, 0x8A50, 0xE522, 0x04AF, 0xB45B, 0xE077, 0x309D, 0xDBA4, 0x1AB0, 0x0448, 0xC079, 0x476A, 0x2DF5, 0x351D, 0x8F77, 0x16E1, 0xE822, 0x313F, 0xF9A9, 0x5A9F, 0x6C46, 0xBD6D, 0x41F9, 0xEB6C, 0x7A03, 0x225A, 0x0AA2, 0xE1C5, 0xFB0E, 0xAC2D, 0xE0B3, 0x1D5B, 0x7AB1, 0x2C01, 0xAE92, 0xC093, 0xA74B, 0x7565, 0x4775, 0xB005, 0xD4CC, 0xE1DB, 0xE0E8, 0x1800, 0x793B, 0xCB5B, 0x924C
    };
WORD g_ValidCodeTypeVip[150] =
    {
        0xE602, 0xB133, 0x14D0, 0xDB54, 0x4D42, 0xBD60, 0xA097, 0x1C89, 0x4228, 0x7E04, 0xD805, 0x1AC5, 0x02D1, 0xFD76, 0x28E0, 0x7FB9, 0x888E, 0xE59F, 0x80E3, 0x5869, 0x1964, 0x067E, 0xD53C, 0x96AC, 0xBB8C, 0xD2A3, 0xA00F, 0x90AF, 0xB7F2, 0xE1B5, 0x5BF3, 0x7D71, 0x1CB8, 0x6EEE, 0x0972, 0xFD47, 0x3EB2, 0xDA9C, 0xAD8B, 0x8F57, 0x31EA, 0x26A1, 0xD032, 0x1A1F, 0x501B, 0x7CF1, 0xFECC, 0x66ED, 0xBA37, 0xA518,
        0x47B5, 0xA266, 0xD1E3, 0x91B4, 0xC0BD, 0xDE02, 0xBCF8, 0xD4F5, 0x01A4, 0x908D, 0xE503, 0x2522, 0xA6A4, 0x11A5, 0x79C4, 0xE015, 0xD1E5, 0x1F40, 0xEDB0, 0x83BC, 0xA508, 0x5A25, 0x7570, 0x3398, 0xCA9F, 0xCA70, 0x8D62, 0x3640, 0xEEB2, 0x5611, 0x7215, 0x78DD, 0x3F3A, 0x4C4C, 0xA8CF, 0x09AD, 0xF4A6, 0xDD38, 0x7409, 0x9B82, 0xC45B, 0xA042, 0x60F0, 0x0643, 0x6B2E, 0x0DA9, 0xBCE4, 0xC568, 0x2BEA, 0x0101,
        0x1AFA, 0x797F, 0x46CF, 0x3EB2, 0xD07C, 0x64AB, 0x870F, 0xEA79, 0x7866, 0xED22, 0xB850, 0x0DE4, 0x70E8, 0x20AD, 0x2A2F, 0x16D8, 0x5CE9, 0x2C2A, 0x33B9, 0x550D, 0xA180, 0xE60B, 0xAAF3, 0x808E, 0xEA7B, 0x43D6, 0x6B51, 0x303B, 0xA6DA, 0xDFA5, 0xD740, 0x6349, 0x8652, 0x7BA6, 0x529E, 0xFABF, 0x01CD, 0x799A, 0xC53F, 0x3BF9, 0xD1E7, 0x6057, 0x1C68, 0x4E26, 0x7571, 0x5C48, 0xC856, 0xC1CC, 0xAFEF, 0xBCEA
    };


//---------------------------------------------------------------------------
#pragma package(smart_init)

TFtVersion::TFtVersion()
{
    m_TypeInfo.m_Type = 0000;
    m_TypeInfo.m_Code = "";
    m_minor = 0;
    m_major = 0;
    m_build = 0;
    m_release = 0;
    m_DateEnd = "";
    
    m_PassForCode = "QGT98b*($%F_V1";
    GetFromResource();
    if(ReadInfoWinReg())
    {
        //Проверяет уже установленный сертификат
        int nErrCode;
        if(!VerifySertificate(m_TypeInfo.m_Code, nErrCode))
        {
            //Сертификат не правильный. (скорее всего, просто закончился см.nErrCode)
            //m_TypeInfo.m_Code = "";
        }
    }
}
TFtVersion::~TFtVersion()
{
}
//---------------------------------------------------------------------------
void TFtVersion::GetFromResource(void)
{
    try
    {
        TResourceStream *rs = new TResourceStream((int)HInstance, "#1", RT_VERSION);
        if(rs)
        {
            if(rs->Size >= RC_VERSION_OFFSET+sizeof(WORD)*4)
            {
                rs->Position = RC_VERSION_OFFSET;
                rs->Read(&m_minor, sizeof(WORD));
                rs->Read(&m_major, sizeof(WORD));
                rs->Read(&m_build, sizeof(WORD));
                rs->Read(&m_release, sizeof(WORD));
            }
        }
    }
    catch(...)
    {    
    }
}
//---------------------------------------------------------------------------
AnsiString TFtVersion::GetVersionStr(int a_nVT)
{
    AnsiString sVer = "";
    switch(a_nVT)
    {
        case FTVT_MAJ:
        {
            sVer = IntToStr(m_major);
            break;
        }
        case FTVT_MAJ_MIN:
        {
            sVer = IntToStr(m_major)+"."+IntToStr(m_minor);
            break;
        }
        case FTVT_MAJ_MIN_REL:
        {
            sVer = IntToStr(m_major)+"."+IntToStr(m_minor)+"."+IntToStr(m_release);
            break;
        }
        case FTVT_MAJ_MIN_REL_BUI:
        {
            sVer = IntToStr(m_major)+"."+IntToStr(m_minor)+"."
                   +IntToStr(m_release)+"."+IntToStr(m_build);        
            break;
        }
    }
    return sVer;
}
//---------------------------------------------------------------------------
bool TFtVersion::GetVersion(WORD &a_major, WORD &a_minor, WORD &a_release, WORD &a_build)
{
    a_major = m_major;
    a_minor = m_minor;
    a_release = m_release;
    a_build = m_build;
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BYTE TFtVersion::GetProgrType(void)
{
    BYTE type = GetCurrProgrType();
    //Если сертификат уже не годный, то говорим, что мы Лайит :)
    int nErr;
    if(!VerifySertificate(m_TypeInfo.m_Code, nErr))
    {
        type = FTTYPE_VER_LIGHT;
    }
    return type;
}
//---------------------------------------------------------------------------
//Текущий тип программы в реестре
BYTE TFtVersion::GetCurrProgrType(void)
{
    BYTE type = FTTYPE_VER_LIGHT;

    //Проверяем на версию FTTYPE_VER_NORMAL
    if(type==FTTYPE_VER_LIGHT)
    for(int a=0; a < 150; a++)
        if(g_ValidCodeTypeNormal[a]==m_TypeInfo.m_Type)
            type = FTTYPE_VER_NORMAL;

    //Проверяем на версию FTTYPE_VER_PROF
    if(type==FTTYPE_VER_LIGHT)
    for(int a=0; a < 150; a++)
        if(g_ValidCodeTypeProf[a]==m_TypeInfo.m_Type)
            type = FTTYPE_VER_PROF;

    //Проверяем на версию FTTYPE_VER_VIP
    if(type==FTTYPE_VER_LIGHT)
    for(int a=0; a < 150; a++)
        if(g_ValidCodeTypeVip[a]==m_TypeInfo.m_Type)
            type = FTTYPE_VER_VIP;

    return type;
}
//---------------------------------------------------------------------------
void TFtVersion::SetProgrType(BYTE a_Type)
{
    WORD bt = 0x0000;
    if(a_Type == FTTYPE_VER_NORMAL)
    {
        bt = g_ValidCodeTypeNormal[ random(150) ];
    }
    if(a_Type == FTTYPE_VER_PROF)
    {
        bt = g_ValidCodeTypeProf[ random(150) ];
    }
    if(a_Type == FTTYPE_VER_VIP)
    {
        bt = g_ValidCodeTypeVip[ random(150) ];
    }
    m_TypeInfo.m_Type = bt;
}

//---------------------------------------------------------------------------
AnsiString TFtVersion::GetKey(void)
{
    return m_TypeInfo.m_Code;
}
AnsiString TFtVersion::GetDateEnd(void)
{
    return m_DateEnd;
}

//---------------------------------------------------------------------------
bool TFtVersion::SetSertificate(AnsiString a_UserCode, int &a_nErrCode)
{
    //декодируем сертификат
    AnsiString sert = CodeDecode(a_UserCode);
    if(sert=="")
    {
        a_nErrCode = FTSERT_ERR_INVALID;
        return false;
    }

    //Проверяем валидность сертификата по временам
    if(!VerifySertificate(a_UserCode, a_nErrCode))
    {
        return false;
    }

    //Ставим нужные форматы
    char datesep = DateSeparator;
    AnsiString shortdate = ShortDateFormat;
    DateSeparator = '.';
    ShortDateFormat = "yyyy.mm.dd";

    //Текущая дата (дни)
    int CurrDate = Date();
    int BeginDate;
    try
    {
        //Дата начала из сертификата
        AnsiString ss = StrToInt(sert.SubString(1, 8));
        AnsiString ssRes;
        for(int a=1; a <= ss.Length(); a++)
        {
            ssRes += ss[a];
            if(a==4||a==6)
                ssRes += ".";
        }
        TDateTime tmpDateBeg = StrToDate(ssRes);
        BeginDate = tmpDateBeg;
    }
    catch(...)
    {
        BeginDate = 0;
    }

    //Возвращаем прежние форматы
    DateSeparator = datesep;
    ShortDateFormat = shortdate;


    //Можно ли еще вводить этот сертификат?
    //Не прошло ли еще время, доступное на ввод сертификата (3 дня)
    if(CurrDate-BeginDate > FTSERT_MAX_INPUTLONGDATE)
    {
        a_nErrCode = FTSERT_ERR_ENDREGDATE;
        return false;
    }

    // **Вроде как прошли все прверки!!!**

    bool bIncrementType = true;
    if(m_TypeInfo.m_Code == a_UserCode)
    {
        //Повторный воод сертификата - тип версии не увеличиваем!
        bIncrementType = false;
    }
    //Запоминаем сертификат
    m_TypeInfo.m_Code = a_UserCode;

    //Увеличиваем тип версии
    if(bIncrementType)
    {
        /*
        BYTE  progType = GetProgrType();
        if(progType==FTTYPE_VER_LIGHT)
            SetProgrType(FTTYPE_VER_NORMAL);
        if(progType==FTTYPE_VER_NORMAL)
            SetProgrType(FTTYPE_VER_PROF);
        if(progType==FTTYPE_VER_PROF)
            SetProgrType(FTTYPE_VER_VIP);
        */
        //Сразу ставим VIP
        SetProgrType(FTTYPE_VER_VIP);
    }

    if(!WriteInfoWinReg())
    {
        //Не удалось сохранить сертификат в системе.
        //Нужны права администратора.
        a_nErrCode = FTSERT_ERR_NOWRITEREG;
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
//Проверяет уже установленный сертификат
bool TFtVersion::VerifySertificate(AnsiString a_UserCode, int &a_nErrCode)
{
    //декодируем сертификат
    AnsiString sert = CodeDecode(a_UserCode);
    if(sert=="")
    {
        a_nErrCode = FTSERT_ERR_INVALID;
        return false;
    }

    //Ставим нужные форматы
    char datesep = DateSeparator;
    AnsiString shortdate = ShortDateFormat;
    DateSeparator = '.';
    ShortDateFormat = "yyyy.mm.dd";

    //Текущая дата (дни c 0 года)
    int CurrDate = Date();
    int BeginDate;
    int EndDate;
    try
    {
        //Дата начала из сертификата
        AnsiString ss = StrToInt(sert.SubString(1, 8));
        AnsiString ssRes;
        for(int a=1; a <= ss.Length(); a++)
        {
            ssRes += ss[a];
            if(a==4||a==6)
                ssRes += ".";
        }
        TDateTime tmpDateBeg = StrToDate(ssRes);
        BeginDate = tmpDateBeg;

        //Дата конца из сертификата
        AnsiString ssE = StrToInt(sert.SubString(9, 8));
        AnsiString ssERes;
        for(int a=1; a <= ssE.Length(); a++)
        {
            ssERes += ssE[a];
            if(a==4||a==6)
                ssERes += ".";
        }
        TDateTime tmpDateEnd = StrToDate(ssERes);
        EndDate = tmpDateEnd;
    }
    catch(...)
    {
        BeginDate = 0;
        EndDate = 0;
    }

    //Возвращаем прежние форматы
    DateSeparator = datesep;
    ShortDateFormat = shortdate;

    //Проверяем все даты.
    if(EndDate < BeginDate)
    {
        a_nErrCode = FTSERT_ERR_NOBEGINEND;
        return false;
    }
    //Проверяем максимальную длительность сертификата
    if(EndDate-BeginDate > FTSERT_MAX_LONGDATE)
    {
        a_nErrCode = FTSERT_ERR_MORELONGDATE;
        return false;
    }
    //Наступил ли срок действия?
    if(CurrDate < BeginDate)
    {
        a_nErrCode = FTSERT_ERR_NOBEGINDATE;
        return false;
    }

    m_DateEnd = sert.SubString(15, 2)+"."+sert.SubString(13, 2)+"."+sert.SubString(9, 4);

    //Срок действия закончился?
    if(CurrDate > EndDate)
    {
        a_nErrCode = FTSERT_ERR_ENDDATE;
        return false;
    }


    return true;
}

//---------------------------------------------------------------------------
// Читаем/записываем инфу о типе версии программы и ключе в реестре винды.
bool TFtVersion::ReadInfoWinReg(void)
{
    bool bRes = false;
    TRegistry *reg = new TRegistry(KEY_READ);
    if(reg)
    {
        /*HKEY_LOCAL_MACHINE\Software\MRSUN\FT\SertificatesInfo*/
        reg->RootKey = HKEY_LOCAL_MACHINE;
        AnsiString tkey = "Software\\";
        tkey += "Mrsun\\";
        tkey += "Ft\\";
        tkey += "SertificatesInfo";
        try
        {
            if(reg->OpenKey(tkey, true))
            {
                m_TypeInfo.m_Type = reg->ReadInteger("Type");
                m_TypeInfo.m_Code = reg->ReadString("Code");
                bRes = true;
            }
        }
        catch(...)
        {
        }
        delete reg;
    }
    return bRes;
}
//---------------------------------------------------------------------------
//Пишем инфу о проге в реестр
bool TFtVersion::WriteInfoWinReg(void)
{
    bool bRes = false;
    TRegistry *reg = new TRegistry(KEY_ALL_ACCESS);
    if(reg)
    {
        /*HKEY_LOCAL_MACHINE\Software\MRSUN\FT\SertificatesInfo*/
        reg->RootKey = HKEY_LOCAL_MACHINE;
        AnsiString tkey = "Software\\";
        tkey += "Mrsun\\";
        tkey += "Ft\\";
        tkey += "SertificatesInfo";
        try
        {
            if(reg->OpenKey(tkey, true))
            {
                reg->WriteInteger("Type", m_TypeInfo.m_Type);
                reg->WriteString("Code", m_TypeInfo.m_Code);
                bRes = true;
            }
        }
        catch(...)
        {
        }
        delete reg;
    }
    return bRes;
}

//---------------------------------------------------------------------------
//закодируем
AnsiString TFtVersion::CodeEncode(AnsiString a_NoCode)
{
    //1. кодируем ддммггггддммгггг в ддмрмггрггдрдммргггг
    WORD vc = g_ValidCode[ random(500) ];
    AnsiString svc = IntToHex(vc, 4);
    AnsiString sNoCode = a_NoCode.SubString(1, 16);
    AnsiString sNoCodeVC;
    int i=0;
    int c=1;
    for(int a=1; a <= sNoCode.Length(); a++)
    {
        i++;
        sNoCodeVC += sNoCode[a];
        if(i==3)
        {
            if(c<=4)
                sNoCodeVC += svc[c];
            c++;
            i=0;
        }
    }

    //Добавляем строку до 20 символов 0-ми
    int len = sNoCodeVC.Length();
    if(len < 20)
    {
        for(int a=len; a < 20; a++)
            sNoCodeVC += "0";
    }


    //sNoCodeVC - строка из 20 символов цифровых (10 байт шестнадцатиричных)
    BYTE masNoCodeVC[10];
    memset(masNoCodeVC, 0, 10);
    try
    {
        for(int a=0; a < 10; a++)
        {
            AnsiString sByte = "0x"+sNoCodeVC.SubString((a*2)+1, 2);
            BYTE tmpByte = StrToInt(sByte);
            masNoCodeVC[a] = tmpByte;
        }
    }
    catch(...)
    {
        //Ошибка - неверный формат входных данных
        return "";
    }

    //2. Пропускаем masNoCodeVC через кодировщик с паролем
    AnsiString pass = m_PassForCode;
    BYTE masUserCode[10];
    memset(masUserCode, 0, 10);
    int passIndex = 0;
    char lastch = 0;
    for(int a=0; a < 10; a++)
    {
        passIndex ++;
        char ch = masNoCodeVC[a]+lastch+pass[passIndex];
        masUserCode[a] = ch;
        lastch = ch;
        if(passIndex == pass.Length())
            passIndex = 0;
    }

    //3. Преобразуем masUserCode в sUserCode
    int mc=0;
    int num=0;
    AnsiString sUserCode;
    for(int a=0; a < 10; a++)
    {
        mc++;
        sUserCode += IntToHex(masUserCode[a], 2);
        if(mc==2)
        {
            if(num<4)
                sUserCode += "-";
            num++;
            mc=0;
        }
    }

    return sUserCode;
}

//---------------------------------------------------------------------------
//раскодируем
AnsiString TFtVersion::CodeDecode(AnsiString a_UserCode)
{
    //1. Преобразуем sUserCode в masUserCode
    AnsiString sUserCode = AnsiReplaceStr(a_UserCode, "-", "");
    if(sUserCode.Length()!=20)
        return "";
    BYTE masUserCode[10];
    memset(masUserCode, 0, 10);
    for(int a=0; a < 10; a++)
    {
        AnsiString sByte = "0x"+sUserCode.SubString((a*2)+1, 2);
        BYTE tmpByte = StrToInt(sByte);
        masUserCode[a] = tmpByte;
    }

    //2. Пропускаем masUserCode через раскодировщик с паролем
    AnsiString pass = m_PassForCode;
    //unsigned char ucPass[16] = {0, 'Q','G','T','9','8','b','*','(','$','%','F','_','V','1',0};    
    BYTE masNoCodeVC[10];
    memset(masNoCodeVC, 0, 10);
    int passIndex = 10/pass.Length();
    passIndex = passIndex * pass.Length();
    passIndex = 10 - passIndex;
    if(passIndex==0) passIndex = pass.Length();
    passIndex ++;
    for(int a=9; a >= 0; a--)
    {
        passIndex --;
        unsigned char lastch = 0;
        if(a > 0) lastch = masUserCode[a-1];
        unsigned char ch_pass = pass[passIndex];
        unsigned char ch_tmp = masUserCode[a];
        unsigned char ch = ch_tmp - lastch - ch_pass;
        masNoCodeVC[a] = ch;
        if(passIndex == 1)
            passIndex = pass.Length()+1;
    }

    //3. Преобразуем masNoCodeVC в строку sNoCode и WORD wValidCode
    AnsiString sNoCodet;
    for(int a=0; a < 10; a++)
    {
        BYTE bm = masNoCodeVC[a];
        sNoCodet += IntToHex(bm, 2);
    }
    //Вынимаем из строки wValidCode
    AnsiString sNoCode;
    AnsiString sValidCode;
    WORD wValidCode;
    for(int a=1; a <= sNoCodet.Length(); a++)
    {
        if(a==4 || a==8 || a==12 || a==16)
        {
            sValidCode += sNoCodet[a];
        }
        else
        {
            sNoCode += sNoCodet[a];
        }
    }
    wValidCode = StrToInt("0x"+sValidCode);

    //Проверяем правильность ValidCode кода
    for(int a=0; a < 500; a++)
    {
        if(g_ValidCode[a]==wValidCode)
        {
            try
            {
                StrToInt64(sNoCode);
                return sNoCode;
            }
            catch(...)
            {
                return "";
            }
        }
    }
    return "";
}
