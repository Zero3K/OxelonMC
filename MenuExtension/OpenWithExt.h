/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Feb 07 00:40:20 2009
 */
/* Compiler settings for c:\Investigations\Projects\MyProjects\OxelonMC\MenuExtension\OpenWithExt.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __OpenWithExt_h__
#define __OpenWithExt_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IOpenWithCtxMenuExt_FWD_DEFINED__
#define __IOpenWithCtxMenuExt_FWD_DEFINED__
typedef interface IOpenWithCtxMenuExt IOpenWithCtxMenuExt;
#endif 	/* __IOpenWithCtxMenuExt_FWD_DEFINED__ */


#ifndef __OpenWithCtxMenuExt_FWD_DEFINED__
#define __OpenWithCtxMenuExt_FWD_DEFINED__

#ifdef __cplusplus
typedef class OpenWithCtxMenuExt OpenWithCtxMenuExt;
#else
typedef struct OpenWithCtxMenuExt OpenWithCtxMenuExt;
#endif /* __cplusplus */

#endif 	/* __OpenWithCtxMenuExt_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IOpenWithCtxMenuExt_INTERFACE_DEFINED__
#define __IOpenWithCtxMenuExt_INTERFACE_DEFINED__

/* interface IOpenWithCtxMenuExt */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IOpenWithCtxMenuExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2A7B8C7B-2D94-43EB-8A1A-4FFC1FAEF236")
    IOpenWithCtxMenuExt : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IOpenWithCtxMenuExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOpenWithCtxMenuExt __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOpenWithCtxMenuExt __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOpenWithCtxMenuExt __RPC_FAR * This);
        
        END_INTERFACE
    } IOpenWithCtxMenuExtVtbl;

    interface IOpenWithCtxMenuExt
    {
        CONST_VTBL struct IOpenWithCtxMenuExtVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOpenWithCtxMenuExt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOpenWithCtxMenuExt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOpenWithCtxMenuExt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IOpenWithCtxMenuExt_INTERFACE_DEFINED__ */



#ifndef __OPENWITHEXTLib_LIBRARY_DEFINED__
#define __OPENWITHEXTLib_LIBRARY_DEFINED__

/* library OPENWITHEXTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_OPENWITHEXTLib;

EXTERN_C const CLSID CLSID_OpenWithCtxMenuExt;

#ifdef __cplusplus

class DECLSPEC_UUID("AC94BA2C-8211-45D4-AB5C-C2A9BCCC8FB6")
OpenWithCtxMenuExt;
#endif
#endif /* __OPENWITHEXTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
