/********************************************************************************
Copyright (C) 2014 William Pearson <adocilesloth@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
********************************************************************************/
#include "MuteSceneSwitch.h"
#include <string>
#include <fstream>

using namespace std;

ifstream settings;

bool enabled;
bool stream;
bool oldmuted;
//config popout stuff
HINSTANCE   hInstance;

INT_PTR CALLBACK ConfigDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	string ssuff;
	wstring wsuff;
	string path = OBSGetPluginDataPath().CreateUTF8String();

	settings.open(path + "\\mutesceneswitch.ini");
	getline(settings, ssuff);
	getline(settings, ssuff);
	wsuff = wstring(ssuff.begin(), ssuff.end());
	settings.close();


switch(message)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_ENABLE), BM_SETCHECK, enabled ? BST_CHECKED : BST_UNCHECKED, 0);

		if(wsuff.length() != 0)
		{
			HWND SUFFInput = GetDlgItem(hWnd, IDC_SUFF);
			SetWindowText(SUFFInput, wsuff.c_str());
		}
		if(enabled)
		{
			EnableWindow(GetDlgItem(hWnd, IDC_SUFF), true);
		}
		else
		{
			EnableWindow(GetDlgItem(hWnd, IDC_SUFF), false);
		}

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
			enabled = SendMessage(GetDlgItem(hWnd, IDC_ENABLE), BM_GETCHECK, 0, 0) == BST_CHECKED;
			
			wofstream create;
			create.open(path + "\\mutesceneswitch.ini");
			create << enabled << endl;

			if(enabled)
			{
				HWND SUFFOutput = GetDlgItem(hWnd, IDC_SUFF);
				int length = GetWindowTextLength(SUFFOutput) + 1;
				TCHAR * suffbuf = (TCHAR *)GlobalAlloc(GPTR, length * sizeof(TCHAR));					
				GetWindowText(SUFFOutput, suffbuf, length);
				create << suffbuf;		//write to file	
				GlobalFree(suffbuf);	//clear buffer
			}
			else
			{
				create << wsuff;
			}

			create.close();

			EndDialog(hWnd, LOWORD(wParam));
			break;
			}

		case IDCANCEL:
			EndDialog(hWnd, LOWORD(wParam));
			break;

		case IDC_ENABLE:
			{
			bool bsuff = SendMessage(GetDlgItem(hWnd, IDC_ENABLE), BM_GETCHECK, 0, 0) == BST_CHECKED;
			if(bsuff)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_SUFF), true);
			}
			else if(!bsuff)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_SUFF), false);
			}
			}
		}
	}

return 0;
}

void ConfigPlugin(HWND hWnd)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MSSDLG), hWnd, ConfigDlgProc);
}

bool LoadPlugin()
{
	string path = OBSGetPluginDataPath().CreateUTF8String();
	settings.open(path + "\\mutesceneswitch.ini");

	if (!settings.is_open())
	{
		ofstream create(path + "\\mutesceneswitch.ini");
		create << 0 << endl;
		create << " Mute";
		create.close();
		enabled = false;
	}
	else
	{
		settings >> enabled;
		settings.close();
	}

	oldmuted = OBSGetDesktopMuted();
	AppWarning(TEXT("MuteSceneSwitch Loaded"));
	return true;
}

void UnloadPlugin()
{
	return;
}

CTSTR GetPluginName()
{
	return TEXT("Mute Scene Switcher");
}

CTSTR GetPluginDescription()
{
	return TEXT("Swiches to a Mute Scene and mutes microphone when desktop is muted");
}

void OnStartStream()
{
	stream = true;
	return;
}

void OnStopStream()
{
	stream = false;
	return;
}

void OnDesktopVolumeChanged()
{
	if(!enabled)	//break if plugin is disabled
	{
		return;
	}
	if(!stream)	//break if not streaming
	{
		if(OBSGetDesktopMuted() != oldmuted)	//if muted status is changed
		{
			oldmuted = OBSGetDesktopMuted();	//set oldmuted
		}
		return;
	}
	if(OBSGetDesktopMuted() == 0 && oldmuted == 0)	//break if just volume is changed
	{
		return;
	}

	if(OBSGetDesktopMuted() != OBSGetMicMuted())	//match dektop and mic muted status
	{
		OBSToggleMicMute();
	}

	string ssuffix;

	string path = OBSGetPluginDataPath().CreateUTF8String();
	settings.open(path + "\\mutesceneswitch.ini");
	getline(settings, ssuffix);
	getline(settings, ssuffix);
	settings.close();

	wstring wsuffix = wstring(ssuffix.begin(), ssuffix.end());
	int sufflen = wsuffix.length();

	if(OBSGetDesktopMuted())	//if muted
	{
		//switch to mute scene
		wstring scenename = OBSGetSceneName();
		scenename.append(wsuffix);
		OBSSetScene(scenename.c_str(), 1);
	}
	else	//if unmuted
	{
		//switch to normal scene
		wstring scenename = OBSGetSceneName();
		int length = scenename.length();
		scenename = scenename.substr(0, length - sufflen);
		OBSSetScene(scenename.c_str(), 1);
	}

	oldmuted = OBSGetDesktopMuted();

	return;
}

BOOL CALLBACK DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		hInstance = hinstDLL;

	return TRUE;
}