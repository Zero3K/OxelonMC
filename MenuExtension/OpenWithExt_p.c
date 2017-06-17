/* this ALWAYS GENERATED file contains the proxy stub code */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Feb 07 00:40:20 2009
 */
/* Compiler settings for c:\Investigations\Projects\MyProjects\OxelonMC\MenuExtension\OpenWithExt.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "OpenWithExt.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IOpenWithCtxMenuExt, ver. 0.0,
   GUID={0x2A7B8C7B,0x2D94,0x43EB,{0x8A,0x1A,0x4F,0xFC,0x1F,0xAE,0xF2,0x36}} */


extern const MIDL_STUB_DESC Object_StubDesc;


#pragma code_seg(".orpc")

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x50100a4, /* MIDL Version 5.1.164 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    1,  /* Flags */
    0,  /* Reserved3 */
    0,  /* Reserved4 */
    0   /* Reserved5 */
    };

CINTERFACE_PROXY_VTABLE(3) _IOpenWithCtxMenuExtProxyVtbl = 
{
    0,
    &IID_IOpenWithCtxMenuExt,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _IOpenWithCtxMenuExtStubVtbl =
{
    &IID_IOpenWithCtxMenuExt,
    0,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

#pragma data_seg(".rdata")

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf, more than 32 methods in the interface.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };

const CInterfaceProxyVtbl * _OpenWithExt_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IOpenWithCtxMenuExtProxyVtbl,
    0
};

const CInterfaceStubVtbl * _OpenWithExt_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IOpenWithCtxMenuExtStubVtbl,
    0
};

PCInterfaceName const _OpenWithExt_InterfaceNamesList[] = 
{
    "IOpenWithCtxMenuExt",
    0
};


#define _OpenWithExt_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _OpenWithExt, pIID, n)

int __stdcall _OpenWithExt_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_OpenWithExt_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo OpenWithExt_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _OpenWithExt_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _OpenWithExt_StubVtblList,
    (const PCInterfaceName * ) & _OpenWithExt_InterfaceNamesList,
    0, // no delegation
    & _OpenWithExt_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
