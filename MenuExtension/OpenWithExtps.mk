
OpenWithExtps.dll: dlldata.obj OpenWithExt_p.obj OpenWithExt_i.obj
	link /dll /out:OpenWithExtps.dll /def:OpenWithExtps.def /entry:DllMain dlldata.obj OpenWithExt_p.obj OpenWithExt_i.obj \
		kernel32.lib rpcndr.lib rpcns4.lib rpcrt4.lib oleaut32.lib uuid.lib \

.c.obj:
	cl /c /Ox /DWIN32 /D_WIN32_WINNT=0x0400 /DREGISTER_PROXY_DLL \
		$<

clean:
	@del OpenWithExtps.dll
	@del OpenWithExtps.lib
	@del OpenWithExtps.exp
	@del dlldata.obj
	@del OpenWithExt_p.obj
	@del OpenWithExt_i.obj
