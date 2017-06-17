#ifndef __SPAWN_H__
#define __SPAWN_H__

#include <windows.h>

class CSpawner {
	bool created;
	HANDLE hStdinRead, hStdinWrite;
	HANDLE hStdoutRead, hStdoutWrite;
	PROCESS_INFORMATION pi;
public:
	CSpawner() {
		created=false;
	}
	~CSpawner() {
		if(created) DespawnProcess();
	}
	int SpawnProcess(TCHAR *szCmdLine);
	int DespawnProcess();
	
	int writestr(char *buf);
	int readstr(char *buf,int iBufSize);
	void read(char *buf,int size);
};

#endif
