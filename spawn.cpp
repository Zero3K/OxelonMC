#include <windows.h>

#include "spawn.h"

int CSpawner::SpawnProcess(TCHAR *szCmdLine) {
	BOOL res;
	SECURITY_ATTRIBUTES pipe_attr;
	STARTUPINFO si;
	
	pipe_attr.nLength=sizeof(SECURITY_ATTRIBUTES);
	pipe_attr.lpSecurityDescriptor=NULL;
	pipe_attr.bInheritHandle=TRUE;
	
//	res=CreatePipe(&hStdinRead,&hStdinWrite,&pipe_attr,0);
//	if(!res) return -1;
	res=CreatePipe(&hStdoutRead,&hStdoutWrite,&pipe_attr,0);
	if(!res) return -1;
	
	ZeroMemory(&si,sizeof(STARTUPINFO));
	si.cb=sizeof(STARTUPINFO);
	si.dwFlags=STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW|STARTF_USESIZE;
	si.hStdInput=0;//hStdinRead;
	si.hStdOutput=hStdoutWrite;
	si.hStdError=hStdoutWrite;
	si.wShowWindow = SW_HIDE;
	si.dwXSize = 0;
	si.dwYSize = 0;
	
	res=CreateProcess(NULL,szCmdLine,NULL,NULL,TRUE,CREATE_NO_WINDOW,NULL,NULL,&si,&pi);
	if(!res) return -1;
	
	created=true;
	
	CloseHandle(hStdoutWrite);
	
	return 0;
}

int CSpawner::DespawnProcess() {
	TerminateProcess(pi.hProcess,0);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	
//	CloseHandle(hStdinRead);
//	CloseHandle(hStdinWrite);
	CloseHandle(hStdoutRead);
//	CloseHandle(hStdoutWrite);
	
	created=false;
	
	return 0;
}

int CSpawner::writestr(char *szStr) {
	char *p;
	int iRest;
	DWORD dwBytesWritten;
	
	p=szStr;
	iRest=lstrlenA(szStr);
	while(iRest) {
		WriteFile(hStdinWrite,p,iRest,&dwBytesWritten,NULL);
		p+=dwBytesWritten;
		iRest-=dwBytesWritten;
	}
	WriteFile(hStdinWrite,"\x0A",1,&dwBytesWritten,NULL);
	
	return 0;
}

int CSpawner::readstr(char *buf,int iBufSize) {
	int i;
	DWORD dwBytesRead;
	
	for(i=0;i<=iBufSize-1;i++) {
		ReadFile(hStdoutRead,&buf[i],1,&dwBytesRead,NULL);
		if(buf[i]=='\x0D'||buf[i]=='\x0A'||buf[i]=='\0') {
			if(i) {
				buf[i]='\x0';
				break;
			}
			else i--; 
		}
		else if(!dwBytesRead) break;
	}
	
	return 0;
}

void CSpawner::read(char *buf,int size)
{
	DWORD dw;
	ReadFile(hStdoutRead,buf,1024,&dw,NULL);
	while (dw!=0)
	{
		buf=buf+dw;
		ReadFile(hStdoutRead,buf,1024,&dw,NULL);
	}

}