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

//Типы строковых версий
#define FTVT_MAJ_MIN_REL_BUI  0  //Полная версия из 4-х позиций
#define FTVT_MAJ_MIN_REL      1  //версия из 3-х позиций
#define FTVT_MAJ_MIN          2  //версия из 2-х позиций
#define FTVT_MAJ              3  //версия из 1 позиции

//Типы версий программы
#define FTTYPE_VER_LIGHT    0 //Версия лайт
#define FTTYPE_VER_NORMAL   1 //Версия нормальная
#define FTTYPE_VER_PROF     2 //Версия профи
#define FTTYPE_VER_VIP      3 //Версия ВИП

//Типы ошибок сертификата
#define FTSERT_ERR_INVALID       -1 //Не верный код сертификата
#define FTSERT_ERR_ENDREGDATE    -2 //Прошел период для ввода этого сертификата (2-3 дня)
#define FTSERT_ERR_ENDDATE       -3 //Прошел срок действия сертификата
#define FTSERT_ERR_NOBEGINDATE   -4 //Срок действия еще не наступил
#define FTSERT_ERR_MORELONGDATE  -5 //Слишком большой срок сертификата - невозможный срок.
#define FTSERT_ERR_NOBEGINEND    -6 //Конец наступил раньше, чем пришло начало сертификата
#define FTSERT_ERR_NOWRITEREG    -7 //Не удалось сохранить сертификат в реестре.

//Максимальный срок сертификата 1 год (в днях)
#define FTSERT_MAX_LONGDATE  366 //1 год

//Максимальный срок для ввода сертификата - 3 дня
#define FTSERT_MAX_INPUTLONGDATE  3 //Три дня

struct TFtTypeInfo
{
    WORD m_Type;   //Код (из массивов) типа версии программы
    AnsiString m_Code; //Регистрационный код 16 байт (символов)
                       //Код включает в себя Дату начала срока и дату конца срока
                       //закодированы специальным паролем...
                       //ддмрмггрггдрдммргггг  (NoCode | д-день, м-месяц, г-год, рррр-разрешенный_код(ValidCode)).
                       //хххх-хххх-хххх-хххх-хххх (UserCode | в таком виде выдается юзеру)

};

//---------------------------------------------------------------------------
class TFtVersion
{
private:
    WORD m_minor;
    WORD m_major;
    WORD m_build;
    WORD m_release;

    TFtTypeInfo m_TypeInfo;  //Тип версии программы (Лайт/нормал/профи/вип)
    AnsiString m_PassForCode;
    AnsiString m_DateEnd;//Дата окончания ключа.(только для отображения!!!)

public:
    TFtVersion();
    ~TFtVersion();

    AnsiString GetVersionStr(int a_nVT=FTVT_MAJ_MIN_REL_BUI); //FTVT_
    bool GetVersion(WORD &a_major, WORD &a_minor, WORD &a_release, WORD &a_build);

    BYTE GetProgrType(void); //Тип программы (зависит от валидности текущего ключа)
    BYTE GetCurrProgrType(void); //Текущий тип программы в реестре
    AnsiString GetKey(void);
    AnsiString GetDateEnd(void);
    bool SetSertificate(AnsiString a_UserCode, int &a_nErrCode);

    AnsiString CodeEncode(AnsiString a_NoCode); //закодируем
    AnsiString CodeDecode(AnsiString a_UserCode);//раскодируем

private:
    void GetFromResource(void);//Получаем номер версии из ресурсов
    // Читаем/записываем инфу о типе версии программы и ключе в реестре винды.
    bool ReadInfoWinReg(void);
    bool WriteInfoWinReg(void);

    void SetProgrType(BYTE a_Type);
    //Проверяет уже установленный сертификат
    bool VerifySertificate(AnsiString a_UserCode, int &a_nErrCode);

};

#endif
 