

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Fri Dec 10 11:40:25 2004
 */
/* Compiler settings for .\ZViewerAgent.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __ZViewerAgent_h__
#define __ZViewerAgent_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IZViewMenuExt_FWD_DEFINED__
#define __IZViewMenuExt_FWD_DEFINED__
typedef interface IZViewMenuExt IZViewMenuExt;
#endif 	/* __IZViewMenuExt_FWD_DEFINED__ */


#ifndef __ZViewMenuExt_FWD_DEFINED__
#define __ZViewMenuExt_FWD_DEFINED__

#ifdef __cplusplus
typedef class ZViewMenuExt ZViewMenuExt;
#else
typedef struct ZViewMenuExt ZViewMenuExt;
#endif /* __cplusplus */

#endif 	/* __ZViewMenuExt_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IZViewMenuExt_INTERFACE_DEFINED__
#define __IZViewMenuExt_INTERFACE_DEFINED__

/* interface IZViewMenuExt */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IZViewMenuExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C9568C1B-74EE-42CF-B30F-7B7D09A9B447")
    IZViewMenuExt : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IZViewMenuExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IZViewMenuExt * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IZViewMenuExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IZViewMenuExt * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IZViewMenuExt * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IZViewMenuExt * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IZViewMenuExt * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IZViewMenuExt * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IZViewMenuExtVtbl;

    interface IZViewMenuExt
    {
        CONST_VTBL struct IZViewMenuExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IZViewMenuExt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IZViewMenuExt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IZViewMenuExt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IZViewMenuExt_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IZViewMenuExt_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IZViewMenuExt_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IZViewMenuExt_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IZViewMenuExt_INTERFACE_DEFINED__ */



#ifndef __ZViewerAgentLib_LIBRARY_DEFINED__
#define __ZViewerAgentLib_LIBRARY_DEFINED__

/* library ZViewerAgentLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ZViewerAgentLib;

EXTERN_C const CLSID CLSID_ZViewMenuExt;

#ifdef __cplusplus

class DECLSPEC_UUID("B03E70D6-2EC9-4ECA-957E-8F9CD684C487")
ZViewMenuExt;
#endif
#endif /* __ZViewerAgentLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


