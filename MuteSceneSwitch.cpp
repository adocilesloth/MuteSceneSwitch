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
bool olddeskmuted;
bool oldmicmuted;
int trigger;
bool force;
bool scene;
string suffix;
wstring oldscene;
//config popout stuff
HINSTANCE   hInstance;

INT_PTR CALLBACK ConfigDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wstring wsuff;
	wsuff = wstring(suffix.begin(), suffix.end());

switch(message)
	{
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd, IDC_ENABLE), BM_SETCHECK, enabled ? BST_CHECKED : BST_UNCHECKED, 0);
		SendMessage(GetDlgItem(hWnd, IDC_FORCE), BM_SETCHECK, force ? BST_CHECKED : BST_UNCHECKED, 0);

		if(trigger == 0)
		{
			SendMessage(GetDlgItem(hWnd, IDC_DESK), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_MIC), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
			if(enabled)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_FORCE), true);
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_FORCE), false);
			}
		}
		else if(trigger == 1)
		{
			SendMessage(GetDlgItem(hWnd, IDC_DESK), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_MIC), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
			if(enabled)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_FORCE), true);
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_FORCE), false);
			}
		}
		else if(trigger == 2)
		{
			SendMessage(GetDlgItem(hWnd, IDC_DESK), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_MIC), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
			EnableWindow(GetDlgItem(hWnd, IDC_FORCE), false);
		}

		if(!scene)
		{
			SendMessage(GetDlgItem(hWnd, IDC_MUTE), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
		}
		else
		{
			SendMessage(GetDlgItem(hWnd, IDC_MUTE), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
		}

		if(wsuff.length() != 0)
		{
			HWND SUFFInput = GetDlgItem(hWnd, IDC_SUFF);
			SetWindowText(SUFFInput, wsuff.c_str());
		}

		if(enabled)
		{
			EnableWindow(GetDlgItem(hWnd, IDC_SUFF), true);
			EnableWindow(GetDlgItem(hWnd, IDC_DESK), true);
			EnableWindow(GetDlgItem(hWnd, IDC_MIC), true);
			EnableWindow(GetDlgItem(hWnd, IDC_MUTE), true);
			EnableWindow(GetDlgItem(hWnd, IDC_AFK), true);
			EnableWindow(GetDlgItem(hWnd, IDC_SWITCHTXT), true);
			EnableWindow(GetDlgItem(hWnd, IDC_TOTXT), true);
			EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), true);
			EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), true);
			if(!scene)
			{
				ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_HIDE);
			}
			else
			{
				ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_SHOW);
			}
		}
		else
		{
			EnableWindow(GetDlgItem(hWnd, IDC_SUFF), false);
			EnableWindow(GetDlgItem(hWnd, IDC_DESK), false);
			EnableWindow(GetDlgItem(hWnd, IDC_MIC), false);
			EnableWindow(GetDlgItem(hWnd, IDC_MUTE), false);
			EnableWindow(GetDlgItem(hWnd, IDC_AFK), false);
			EnableWindow(GetDlgItem(hWnd, IDC_SWITCHTXT), false);
			EnableWindow(GetDlgItem(hWnd, IDC_TOTXT), false);
			EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), false);
			EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), false);
			if(!scene)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), false);
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), true);
			}
		}

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
			enabled = SendMessage(GetDlgItem(hWnd, IDC_ENABLE), BM_GETCHECK, 0, 0) == BST_CHECKED;
			bool bdesk = SendMessage(GetDlgItem(hWnd, IDC_DESK), BM_GETCHECK, 0, 0) == BST_CHECKED;
			bool bmic = SendMessage(GetDlgItem(hWnd, IDC_MIC), BM_GETCHECK, 0, 0) == BST_CHECKED;
			force = SendMessage(GetDlgItem(hWnd, IDC_FORCE), BM_GETCHECK, 0, 0) == BST_CHECKED;
			scene = SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_GETCHECK, 0, 0) == BST_CHECKED;
			
			string path = OBSGetPluginDataPath().CreateUTF8String();
			wofstream create;
			create.open(path + "\\mutesceneswitch.ini");
			create << enabled << endl;

			if(!bdesk && bmic)
			{
				trigger = 1;
			}
			else if(bdesk && bmic)
			{
				trigger = 2;
			}
			else	//just desktop or nothing
			{
				trigger = 0;
			}
			create << trigger << endl;

			create << scene << endl;

			create << force << endl;

			if(enabled)
			{
				HWND SUFFOutput = GetDlgItem(hWnd, IDC_SUFF);
				int length = GetWindowTextLength(SUFFOutput) + 1;
				TCHAR * suffbuf = (TCHAR *)GlobalAlloc(GPTR, length * sizeof(TCHAR));					
				GetWindowText(SUFFOutput, suffbuf, length);
				create << suffbuf;		//write to file
				wstring wsuffix = suffbuf;
				suffix = string(wsuffix.begin(), wsuffix.end());
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
			bool bdesk = SendMessage(GetDlgItem(hWnd, IDC_DESK), BM_GETCHECK, 0, 0) == BST_CHECKED;
			bool bmic = SendMessage(GetDlgItem(hWnd, IDC_MIC), BM_GETCHECK, 0, 0) == BST_CHECKED;

			if(bsuff)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_SUFF), true);
				EnableWindow(GetDlgItem(hWnd, IDC_DESK), true);
				EnableWindow(GetDlgItem(hWnd, IDC_MIC), true);
				EnableWindow(GetDlgItem(hWnd, IDC_MUTE), true);
				EnableWindow(GetDlgItem(hWnd, IDC_AFK), true);
				EnableWindow(GetDlgItem(hWnd, IDC_SWITCHTXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_TOTXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), true);
				
				if(bdesk && bmic)
				{
					EnableWindow(GetDlgItem(hWnd, IDC_FORCE), false);
				}
				else if(bdesk && !bmic)
				{
					EnableWindow(GetDlgItem(hWnd, IDC_FORCE), true);
				}
				else if(!bdesk && bmic)
				{
					EnableWindow(GetDlgItem(hWnd, IDC_FORCE), true);
				}
			}
			else// if(!bsuff)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_SUFF), false);
				EnableWindow(GetDlgItem(hWnd, IDC_DESK), false);
				EnableWindow(GetDlgItem(hWnd, IDC_MIC), false);
				EnableWindow(GetDlgItem(hWnd, IDC_MUTE), false);
				EnableWindow(GetDlgItem(hWnd, IDC_AFK), false);
				EnableWindow(GetDlgItem(hWnd, IDC_SWITCHTXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_TOTXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), false);
				
				EnableWindow(GetDlgItem(hWnd, IDC_FORCE), false);
			}
			}

		case IDC_DESK:
			{
			bool bsuff = SendMessage(GetDlgItem(hWnd, IDC_ENABLE), BM_GETCHECK, 0, 0) == BST_CHECKED;
			bool bdesk = SendMessage(GetDlgItem(hWnd, IDC_DESK), BM_GETCHECK, 0, 0) == BST_CHECKED;
			bool bmic = SendMessage(GetDlgItem(hWnd, IDC_MIC), BM_GETCHECK, 0, 0) == BST_CHECKED;
			if(bsuff)
			{
				if(bdesk && bmic)
				{
					EnableWindow(GetDlgItem(hWnd, IDC_FORCE), false);
				}
				else if(!bdesk && !bmic)
				{
					SendMessage(GetDlgItem(hWnd, IDC_MIC), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
					EnableWindow(GetDlgItem(hWnd, IDC_FORCE), true);
				}
				else
				{
					EnableWindow(GetDlgItem(hWnd, IDC_FORCE), true);
				}
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_FORCE), false);
			}
			}

		case IDC_MIC:
			{
			bool bsuff = SendMessage(GetDlgItem(hWnd, IDC_ENABLE), BM_GETCHECK, 0, 0) == BST_CHECKED;
			bool bdesk = SendMessage(GetDlgItem(hWnd, IDC_DESK), BM_GETCHECK, 0, 0) == BST_CHECKED;
			bool bmic = SendMessage(GetDlgItem(hWnd, IDC_MIC), BM_GETCHECK, 0, 0) == BST_CHECKED;
			if(bsuff)
			{
				if(bdesk && bmic)
				{
					EnableWindow(GetDlgItem(hWnd, IDC_FORCE), false);
				}
				else if(!bdesk && !bmic)
				{
					SendMessage(GetDlgItem(hWnd, IDC_DESK), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
					EnableWindow(GetDlgItem(hWnd, IDC_FORCE), true);
				}
				else
				{
					EnableWindow(GetDlgItem(hWnd, IDC_FORCE), true);
				}
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_FORCE), false);
			}
			}

		case IDC_MUTE:
			{
			bool bmutescn = SendMessage(GetDlgItem(hWnd, IDC_MUTE), BM_GETCHECK, 0, 0) == BST_CHECKED;
			if(bmutescn)
			{
				SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
				ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_HIDE);
			}
			else if(!bmutescn)
			{
				SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
				ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_SHOW);
			}
			}

		case IDC_AFK:
			{
			bool bafkscn = SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_GETCHECK, 0, 0) == BST_CHECKED;
			if(bafkscn)
			{
				SendMessage(GetDlgItem(hWnd, IDC_MUTE), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
				ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_SHOW);
			}
			else if(!bafkscn)
			{
				SendMessage(GetDlgItem(hWnd, IDC_MUTE), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
				ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_HIDE);
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
		create << 1 << endl;	//on/off
		create << 0 << endl;	//0 is desktop, 1 is mic, 2 is both
		create << 0 << endl;	//0 is suffix, 1 is scene
		create << 0 << endl;	//1 is force other mute
		create << " Mute";
		create.close();
		enabled = true;
		trigger = 0;
		force = false;
		scene = false;
		suffix = " Mute";
	}
	else
	{
		settings >> enabled;
		settings >> trigger;
		settings >> scene;
		settings >> force;
		settings.ignore(10, '\n');
		getline(settings, suffix);
		settings.close();
	}

	olddeskmuted = OBSGetDesktopMuted();
	oldmicmuted = OBSGetMicMuted();
	oldscene = OBSGetSceneName();
	AppWarning(TEXT("MuteSceneSwitch Loaded"));
	return true;
}

void UnloadPlugin()
{
	return;
}

CTSTR GetPluginName()
{
	return TEXT("AFK/Mute Scene Switcher");
}

CTSTR GetPluginDescription()
{
	return TEXT("Swiches to an AFK or Mute scene when away or audio outputs are muted");
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
	if(trigger == 1)	//break if mic is trigger
	{
		if(OBSGetDesktopMuted() != olddeskmuted)	//if muted status is changed
		{
			olddeskmuted = OBSGetDesktopMuted();	//set oldmuted
		}
		return;
	}
	if(!stream)		//break if not streaming
	{
		if(OBSGetDesktopMuted() != olddeskmuted)	//if muted status is changed
		{
			olddeskmuted = OBSGetDesktopMuted();	//set oldmuted
		}
		return;
	}
	if(OBSGetDesktopMuted() == 0 && olddeskmuted == 0)	//break if just volume is changed
	{
		return;
	}
	if(trigger == 2 && OBSGetMicMuted() == 0)	//break if double trigger and mic is not muted
	{
		olddeskmuted = OBSGetDesktopMuted();
		return;
	}

	if(trigger == 0 && force)
	{
		if(OBSGetDesktopMuted() != OBSGetMicMuted())	//match dektop and mic muted status
		{
			OBSToggleMicMute();
		}
	}

	wstring wsuffix = wstring(suffix.begin(), suffix.end());
	int sufflen = wsuffix.length();

	if(OBSGetDesktopMuted())	//if muted
	{
		//switch to mute scene
		if(!scene)	//if switching to suffix
		{
			wstring scenename = OBSGetSceneName();
			scenename.append(wsuffix);
			OBSSetScene(scenename.c_str(), 1);
		}
		else	//if switching to scene
		{
			oldscene = OBSGetSceneName();
			wstring scenename = wsuffix;
			OBSSetScene(scenename.c_str(), 1);
		}
	}
	else	//if unmuted
	{
		//switch to normal scene
		if(!scene)	//if switching from suffix
		{
			wstring scenename = OBSGetSceneName();
			int length = scenename.length();
			scenename = scenename.substr(0, length - sufflen);
			OBSSetScene(scenename.c_str(), 1);
		}
		else	//if switching to old scene
		{
			OBSSetScene(oldscene.c_str(), 1);
		}
	}

	olddeskmuted = OBSGetDesktopMuted();

	return;
}

void OnMicVolumeChanged()
{
	if(!enabled)	//break if plugin is disabled
	{
		return;
	}
	if(trigger == 0)	//break if desk is trigger
	{
		if(OBSGetMicMuted() != oldmicmuted)	//if muted status is changed
		{
			oldmicmuted = OBSGetMicMuted();	//set oldmuted
		}
		return;
	}
	if(!stream)		//break if not streaming
	{
		if(OBSGetMicMuted() != oldmicmuted)	//if muted status is changed
		{
			oldmicmuted = OBSGetMicMuted();	//set oldmuted
		}
		return;
	}
	if(OBSGetMicMuted() == 0 && oldmicmuted == 0)	//break if just volume is changed
	{
		return;
	}
	if(trigger == 2 && OBSGetDesktopMuted() == 0)	//break if double trigger and desk is not muted
	{
		oldmicmuted = OBSGetMicMuted();
		return;
	}

	if(trigger == 1 && force)
	{
		if(OBSGetMicMuted() != OBSGetDesktopMuted())	//match dektop and mic muted status
		{
			OBSToggleDesktopMute();
		}
	}

	wstring wsuffix = wstring(suffix.begin(), suffix.end());
	int sufflen = wsuffix.length();

	if(OBSGetMicMuted())	//if muted
	{
		//switch to mute scene
		if(!scene)	//if switching to suffix
		{
			wstring scenename = OBSGetSceneName();
			scenename.append(wsuffix);
			OBSSetScene(scenename.c_str(), 1);
		}
		else	//if switching to scene
		{
			oldscene = OBSGetSceneName();
			wstring scenename = wsuffix;
			OBSSetScene(scenename.c_str(), 1);
		}
	}
	else	//if unmuted
	{
		//switch to normal scene
		if(!scene)	//if switching from suffix
		{
			wstring scenename = OBSGetSceneName();
			int length = scenename.length();
			scenename = scenename.substr(0, length - sufflen);
			OBSSetScene(scenename.c_str(), 1);
		}
		else	//if switching to old scene
		{
			OBSSetScene(oldscene.c_str(), 1);
		}
	}

	oldmicmuted = OBSGetMicMuted();

	return;
}

BOOL CALLBACK DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		hInstance = hinstDLL;

	return TRUE;
}