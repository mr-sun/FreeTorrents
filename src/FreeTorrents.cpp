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
//---------------------------------------------------------------------------
USEFORM("uFormMain.cpp", FormMain);
USEFORM("uFormProperty.cpp", FormProperty);
USEFORM("uFormEditPath.cpp", FormEditPath);
USEFORM("uFormEditRules.cpp", FormEditRules);
USEFORM("uFormEditRule.cpp", FormEditRule);
USEFORM("uFormVirtualMgr.cpp", FormVirtualMgr);
USEFORM("uFormVirtualTorrent.cpp", FormVirtualTorrent);
USEFORM("uFormUpdateAsk.cpp", FormUpdateAsk);
USEFORM("uFormAbout.cpp", FormAbout);
USEFORM("uFormEditStr.cpp", FormEditStr);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
		try
		{
			Application->Initialize();
			Application->CreateForm(__classid(TFormMain), &FormMain);
			Application->Run();
		}
		catch (Exception &exception)
		{
				Application->ShowException(&exception);
		}
		catch (...)
		{
				try
				{
						 throw Exception("");
				}
				catch (Exception &exception)
				{
						 Application->ShowException(&exception);
				}
		}
		return 0;
}
//---------------------------------------------------------------------------
