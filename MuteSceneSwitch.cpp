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

int tim = 250;

bool enabled;
bool stream;
bool olddeskmuted;
bool oldmicmuted;
int trigger;
bool force;
int scene;
string suffix;
wstring oldscene;

int lasttime;
bool lastswitchondesk;
bool mutescene;
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

		if(scene == 0)
		{
			SendMessage(GetDlgItem(hWnd, IDC_MUTE), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_SCENE), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
		}
		else if(scene == 1)
		{
			SendMessage(GetDlgItem(hWnd, IDC_MUTE), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_SCENE), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
		}
		else
		{
			SendMessage(GetDlgItem(hWnd, IDC_MUTE), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_SETCHECK, false ? BST_CHECKED : BST_UNCHECKED, 0);
			SendMessage(GetDlgItem(hWnd, IDC_SCENE), BM_SETCHECK, true ? BST_CHECKED : BST_UNCHECKED, 0);
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
			EnableWindow(GetDlgItem(hWnd, IDC_SCENE), true);
			EnableWindow(GetDlgItem(hWnd, IDC_SWITCHTXT), true);
			EnableWindow(GetDlgItem(hWnd, IDC_TOTXT), true);
			EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), true);
			EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), true);
			EnableWindow(GetDlgItem(hWnd, IDC_SCENETXT), true);
			if(scene == 0)
			{
				ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_SCENETXT), SW_HIDE);
			}
			else if(scene == 1)
			{
				ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_SHOW);
				ShowWindow(GetDlgItem(hWnd, IDC_SCENETXT), SW_HIDE);
			}
			else
			{
				ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_HIDE);
				ShowWindow(GetDlgItem(hWnd, IDC_SCENETXT), SW_SHOW);
			}
		}
		else
		{
			EnableWindow(GetDlgItem(hWnd, IDC_SUFF), false);
			EnableWindow(GetDlgItem(hWnd, IDC_DESK), false);
			EnableWindow(GetDlgItem(hWnd, IDC_MIC), false);
			EnableWindow(GetDlgItem(hWnd, IDC_MUTE), false);
			EnableWindow(GetDlgItem(hWnd, IDC_AFK), false);
			EnableWindow(GetDlgItem(hWnd, IDC_SCENE), false);
			EnableWindow(GetDlgItem(hWnd, IDC_SWITCHTXT), false);
			EnableWindow(GetDlgItem(hWnd, IDC_TOTXT), false);
			EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), false);
			EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), false);
			EnableWindow(GetDlgItem(hWnd, IDC_SCENETXT), false);
			if(scene == 0)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_SCENETXT), false);
			}
			else if(scene == 1)
			{
				EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_SCENETXT), false);
			}
			else
			{
				EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_SCENETXT), true);
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
			bool bsuff = SendMessage(GetDlgItem(hWnd, IDC_MUTE), BM_GETCHECK, 0, 0) == BST_CHECKED;
			bool bafk = SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_GETCHECK, 0, 0) == BST_CHECKED;

			if(bsuff)
			{
				scene = 0;
			}
			else if(bafk)
			{
				scene = 1;
			}
			else
			{
				scene = 2;
			}
			
			wstring path = OBSGetPluginDataPath().Array();
			wofstream create;
			create.open(path + L"\\mutesceneswitch.ini");
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
				EnableWindow(GetDlgItem(hWnd, IDC_SCENE), true);
				EnableWindow(GetDlgItem(hWnd, IDC_SWITCHTXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_TOTXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), true);
				EnableWindow(GetDlgItem(hWnd, IDC_SCENETXT), true);
				
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
				EnableWindow(GetDlgItem(hWnd, IDC_SCENE), false);
				EnableWindow(GetDlgItem(hWnd, IDC_SWITCHTXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_TOTXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_MUTETXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_AFKTXT), false);
				EnableWindow(GetDlgItem(hWnd, IDC_SCENETXT), false);
				
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
					ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_SHOW);
					ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_HIDE);
					ShowWindow(GetDlgItem(hWnd, IDC_SCENETXT), SW_HIDE);
				}
			}

		case IDC_AFK:
			{
				bool bafkscn = SendMessage(GetDlgItem(hWnd, IDC_AFK), BM_GETCHECK, 0, 0) == BST_CHECKED;
				if(bafkscn)
				{
					ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_HIDE);
					ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_SHOW);
					ShowWindow(GetDlgItem(hWnd, IDC_SCENETXT), SW_HIDE);
				}
			}

		case IDC_SCENE:
			{
				bool bmutesrc = SendMessage(GetDlgItem(hWnd, IDC_SCENE), BM_GETCHECK, 0, 0) == BST_CHECKED;
				if(bmutesrc)
				{
					ShowWindow(GetDlgItem(hWnd, IDC_MUTETXT), SW_HIDE);
					ShowWindow(GetDlgItem(hWnd, IDC_AFKTXT), SW_HIDE);
					ShowWindow(GetDlgItem(hWnd, IDC_SCENETXT), SW_SHOW);
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
	wstring path = OBSGetPluginDataPath().Array();
	settings.open(path + L"\\mutesceneswitch.ini");

	if (!settings.is_open())
	{
		ofstream create(path + L"\\mutesceneswitch.ini");
		create << 1 << endl;	//on/off
		create << 0 << endl;	//0 is desktop, 1 is mic, 2 is both
		create << 0 << endl;	//0 is suffix, 1 is scene, 2 is source
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
	lasttime = OSGetTime();
	lastswitchondesk = true;

	return true;
}

void UnloadPlugin()
{
	return;
}

CTSTR GetPluginName()
{
	return TEXT("AFK/Mute Scene/Source Switcher");
}

CTSTR GetPluginDescription()
{
	return TEXT("Swiches to an AFK or Mute scene or adds an AFK or Mute source when audio outputs are muted");
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
	if(trigger == 2 && OBSGetMicMuted() == 0 && (OSGetTime() - lasttime) >= tim)	//break if double trigger and mic is not muted
	{
		olddeskmuted = OBSGetDesktopMuted();
		return;
	}

	if(trigger == 2 && !lastswitchondesk && (OSGetTime() - lasttime) < tim)
	{
		return;
	}

	if((OSGetTime() - lasttime) < tim)
	{
		OBSToggleDesktopMute();
		if(force && trigger != 2)
		{
			if(OBSGetDesktopMuted())
			{
				if(OBSGetMicMuted() != OBSGetDesktopMuted())
				{
					OBSToggleMicMute();
				}
			}
			else
			{
				if(OBSGetMicMuted() == OBSGetDesktopMuted())
				{
					OBSToggleMicMute();
				}
			}
		}
		if(lastswitchondesk && trigger == 2)
		{
			Sleep(tim);
			if(OBSGetDesktopMuted())
			{
				if(OBSGetMicMuted() == OBSGetDesktopMuted())
				{
					OBSToggleMicMute();
				}
			}
			else
			{
				if(OBSGetMicMuted() != OBSGetDesktopMuted())
				{
					OBSToggleMicMute();
				}
			}
			if(mutescene)
			{
				OBSToggleMicMute();
			}
		}
		lasttime -= (tim + 1);
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
		/*wstring thingy = L"Mute+Deafened";
		OBSSetSourceRender(thingy.c_str(), true);*/
		//switch to mute scene
		if(scene == 0)	//if switching to suffix
		{
			wstring scenename = OBSGetSceneName();
			scenename.append(wsuffix);
			OBSSetScene(scenename.c_str(), 1);
		}
		else if(scene == 1)	//if switching to scene
		{
			oldscene = OBSGetSceneName();
			wstring scenename = wsuffix;
			OBSSetScene(scenename.c_str(), 1);
		}
		else	//if switching source on
		{
			OBSSetSourceRender(wsuffix.c_str(), true);
		}
	}
	else	//if unmuted
	{
		//switch to normal scene
		if(scene == 0)	//if switching from suffix
		{
			wstring scenename = OBSGetSceneName();
			int length = scenename.length();
			scenename = scenename.substr(0, length - sufflen);
			OBSSetScene(scenename.c_str(), 1);
		}
		else if(scene == 1)	//if switching to old scene
		{
			OBSSetScene(oldscene.c_str(), 1);
		}
		else	//if switching source off
		{
			OBSSetSourceRender(wsuffix.c_str(), false);
		}
	}

	olddeskmuted = OBSGetDesktopMuted();
	lasttime = OSGetTime();
	lastswitchondesk = true;
	mutescene = OBSGetDesktopMuted();

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
	if(trigger == 2 && OBSGetDesktopMuted() == 0 && (OSGetTime() - lasttime) >= tim)	//break if double trigger and desk is not muted
	{
		oldmicmuted = OBSGetMicMuted();
		return;
	}

	if(trigger == 2 && lastswitchondesk && (OSGetTime() - lasttime) < tim)
	{
		return;
	}

	if((OSGetTime() - lasttime) < tim)
	{
		OBSToggleMicMute();
		bool shouldbe = OBSGetMicMuted();
		if(force && trigger != 2)
		{
			if(OBSGetMicMuted())
			{
				if(OBSGetDesktopMuted() == OBSGetMicMuted())
				{
					OBSToggleDesktopMute();
				}
			}
			else
			{
				if(OBSGetDesktopMuted() != OBSGetMicMuted())
				{
					OBSToggleDesktopMute();
				}
			}
		}
		if(!lastswitchondesk && trigger == 2)
		{
			Sleep(tim);
			if(OBSGetMicMuted())
			{
				if(OBSGetDesktopMuted() != OBSGetMicMuted())
				{
					OBSToggleDesktopMute();
				}
			}
			else if(!OBSGetMicMuted())
			{
				if(OBSGetDesktopMuted() == OBSGetMicMuted())
				{
					OBSToggleDesktopMute();
				}
			}
		}
		lasttime -= (tim + 1);
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
		if(scene == 0)	//if switching to suffix
		{
			wstring scenename = OBSGetSceneName();
			scenename.append(wsuffix);
			OBSSetScene(scenename.c_str(), 1);
		}
		else if(scene == 1)	//if switching to scene
		{
			oldscene = OBSGetSceneName();
			wstring scenename = wsuffix;
			OBSSetScene(scenename.c_str(), 1);
		}
		else	//if switching source on
		{
			OBSSetSourceRender(wsuffix.c_str(), true);
		}
	}
	else	//if unmuted
	{
		//switch to normal scene
		if(scene == 0)	//if switching from suffix
		{
			wstring scenename = OBSGetSceneName();
			int length = scenename.length();
			scenename = scenename.substr(0, length - sufflen);
			OBSSetScene(scenename.c_str(), 1);
		}
		else if(scene == 1)	//if switching to old scene
		{
			OBSSetScene(oldscene.c_str(), 1);
		}
		else	//if switching source off
		{
			OBSSetSourceRender(wsuffix.c_str(), false);
		}
	}

	oldmicmuted = OBSGetMicMuted();
	lasttime = OSGetTime();
	lastswitchondesk = false;
	mutescene = OBSGetMicMuted();

	return;
}

BOOL CALLBACK DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		hInstance = hinstDLL;

	return TRUE;
}