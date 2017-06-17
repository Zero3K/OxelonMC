/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Feb 07 00:40:20 2009
 */
/* Compiler settings for c:\Investigations\Projects\MyProjects\OxelonMC\MenuExtension\OpenWithExt.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IOpenWithCtxMenuExt = {0x2A7B8C7B,0x2D94,0x43EB,{0x8A,0x1A,0x4F,0xFC,0x1F,0xAE,0xF2,0x36}};


const IID LIBID_OPENWITHEXTLib = {0xF822821B,0x152B,0x475F,{0xBF,0xAC,0x1A,0xD0,0x8A,0xD8,0x1C,0x09}};


const CLSID CLSID_OpenWithCtxMenuExt = {0xAC94BA2C,0x8211,0x45D4,{0xAB,0x5C,0xC2,0xA9,0xBC,0xCC,0x8F,0xB6}};


#ifdef __cplusplus
}
#endif

