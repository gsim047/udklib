// This file was created by MkExe ver. 1.9.19 [2004-12-01 15:14:23]

#include "fString.h"
//#include "fFile.h"
#include "tFile.h"
#include "tConfiguration.h"

//#include <stdlib.h>

#include "nError.h"
#include "tHKey.h"
#include <windows.h>


static int silent = 0;


static void About()
{
	if ( silent )  return;

	MessageBox(NULL, 
	           "Exec : открывает/выполняет заданый файл через ShellExecute\n"
	           "Вызов : exec.exe [key [...]] filename\n\n"
	           "Допустимые ключи:\n"
	           "  -?                 - вывод этой справки\n"
	           "  --mode             - режим отображения (по умолчанию 1)\n"
	           "  --cmd              - команда (по умолчанию 'open')\n"
	           "  --silent           - не выводить сообщения об ошибках\n"
	           , 
	           "About",
	           MB_ICONINFORMATION | MB_OK | MB_APPLMODAL | MB_SETFOREGROUND);
}// About


static void MBox(const string &ss)
{
	if ( silent )  return;

	string s = "Ошибка !" + ss + "\nРабота невозможна\n"
	           "Для справки выполните  Exec -?";
	MessageBox(NULL, s.c_str(), "Exec",
	           MB_ICONERROR | MB_OK | MB_APPLMODAL | MB_SETFOREGROUND);
}// MBox


//WINAPI WinMain(HINSTANCE h, HINSTANCE, LPSTR, int)
int WinMain(HINSTANCE h, HINSTANCE, LPSTR, int)
{
	int ret = 0;
	try{
		config.loadCmdLine(__argc, __argv);
		string configFileName = config.get("option", "cfg");      // config file
		if ( configFileName.empty() ){
			config.load(config.getfile("cfg"));
		}else{
			config.load(configFileName);
		}

		silent = config.get("option", "silent", 0);

		if ( config.get("option", "?", 0) ){
			About();
			return ret;
		}

		string cmd = config.getPath("cmdline", "1");
		if ( cmd.empty() ){
			nError::Throw("Не задано имя запускаемого файла");
		}
		{
			string cmd2 = GetFullPath(cmd);
			if ( !tFILE::FileExists(cmd2) ){
				nError::Throw("Не найден файл '" + cmd + "'");
			}
			cmd = cmd2;
		}
		{
			int mode = config.get("option", "mode", 1);
			string op = config.get("option", "cmd");
			if ( op.empty() ){
				op = "open";
				/*
				tHKey hk(tHKey::CLASSES_ROOT, "." + ExtractFileExt(cmd));
				if ( hk.valid() ){
					string br = hk.getValStr("");
					tHKey hk2(tHKey::CLASSES_ROOT, br);
					...
				} */
			}
			ShellExecute(HWND_DESKTOP, op.c_str(), cmd.c_str(), NULL, NULL, mode);
		}

	}catch ( nError::Message &e ){
		MBox("\n" + e);
		ret = 0x0F000001;
	}catch ( ... ){
		MBox("!!");
		ret = 0x0F000002;
	}
	return ret;
}// WinMain
