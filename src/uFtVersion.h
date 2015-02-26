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

#ifndef uFtVersionH
#define uFtVersionH

#include <Classes.hpp>
#include <StrUtils.hpp>
#include <Registry.hpp>

#define RC_VERSION_OFFSET   48

//���� ��������� ������
#define FTVT_MAJ_MIN_REL_BUI  0  //������ ������ �� 4-� �������
#define FTVT_MAJ_MIN_REL      1  //������ �� 3-� �������
#define FTVT_MAJ_MIN          2  //������ �� 2-� �������
#define FTVT_MAJ              3  //������ �� 1 �������

//���� ������ ���������
#define FTTYPE_VER_LIGHT    0 //������ ����
#define FTTYPE_VER_NORMAL   1 //������ ����������
#define FTTYPE_VER_PROF     2 //������ �����
#define FTTYPE_VER_VIP      3 //������ ���

//���� ������ �����������
#define FTSERT_ERR_INVALID       -1 //�� ������ ��� �����������
#define FTSERT_ERR_ENDREGDATE    -2 //������ ������ ��� ����� ����� ����������� (2-3 ���)
#define FTSERT_ERR_ENDDATE       -3 //������ ���� �������� �����������
#define FTSERT_ERR_NOBEGINDATE   -4 //���� �������� ��� �� ��������
#define FTSERT_ERR_MORELONGDATE  -5 //������� ������� ���� ����������� - ����������� ����.
#define FTSERT_ERR_NOBEGINEND    -6 //����� �������� ������, ��� ������ ������ �����������
#define FTSERT_ERR_NOWRITEREG    -7 //�� ������� ��������� ���������� � �������.

//������������ ���� ����������� 1 ��� (� ����)
#define FTSERT_MAX_LONGDATE  366 //1 ���

//������������ ���� ��� ����� ����������� - 3 ���
#define FTSERT_MAX_INPUTLONGDATE  3 //��� ���

struct TFtTypeInfo
{
    WORD m_Type;   //��� (�� ��������) ���� ������ ���������
    AnsiString m_Code; //��������������� ��� 16 ���� (��������)
                       //��� �������� � ���� ���� ������ ����� � ���� ����� �����
                       //������������ ����������� �������...
                       //��������������������  (NoCode | �-����, �-�����, �-���, ����-�����������_���(ValidCode)).
                       //����-����-����-����-���� (UserCode | � ����� ���� �������� �����)

};

//---------------------------------------------------------------------------
class TFtVersion
{
private:
    WORD m_minor;
    WORD m_major;
    WORD m_build;
    WORD m_release;

    TFtTypeInfo m_TypeInfo;  //��� ������ ��������� (����/������/�����/���)
    AnsiString m_PassForCode;
    AnsiString m_DateEnd;//���� ��������� �����.(������ ��� �����������!!!)

public:
    TFtVersion();
    ~TFtVersion();

    AnsiString GetVersionStr(int a_nVT=FTVT_MAJ_MIN_REL_BUI); //FTVT_
    bool GetVersion(WORD &a_major, WORD &a_minor, WORD &a_release, WORD &a_build);

    BYTE GetProgrType(void); //��� ��������� (������� �� ���������� �������� �����)
    BYTE GetCurrProgrType(void); //������� ��� ��������� � �������
    AnsiString GetKey(void);
    AnsiString GetDateEnd(void);
    bool SetSertificate(AnsiString a_UserCode, int &a_nErrCode);

    AnsiString CodeEncode(AnsiString a_NoCode); //����������
    AnsiString CodeDecode(AnsiString a_UserCode);//�����������

private:
    void GetFromResource(void);//�������� ����� ������ �� ��������
    // ������/���������� ���� � ���� ������ ��������� � ����� � ������� �����.
    bool ReadInfoWinReg(void);
    bool WriteInfoWinReg(void);

    void SetProgrType(BYTE a_Type);
    //��������� ��� ������������� ����������
    bool VerifySertificate(AnsiString a_UserCode, int &a_nErrCode);

};

#endif
 