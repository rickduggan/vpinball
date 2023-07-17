#pragma once

#include "flexdmd_i.h"

#include <thread>
#include <map>

#include "../zedmd/ZeDMDUtil.h"

class FlexDMDGroupActor;
class FlexDMDImageAsset;
class FlexDMDFont;

class FlexDMD :
   public IDispatchImpl<IFlexDMD, &IID_IFlexDMD, &LIBID_FlexDMD>,
   public CComObjectRoot,
   public CComCoClass<FlexDMD, &CLSID_FlexDMD>
{
public:
   STDMETHOD(GetIDsOfNames)(REFIID /*riid*/, LPOLESTR* rgszNames, UINT cNames, LCID lcid,DISPID* rgDispId);
   STDMETHOD(Invoke)(DISPID dispIdMember, REFIID /*riid*/, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr);
   STDMETHOD(GetDocumentation)(INT index, BSTR *pBstrName, BSTR *pBstrDocString, DWORD *pdwHelpContext, BSTR *pBstrHelpFile);

public:
   FlexDMD();
   ~FlexDMD();

   BEGIN_COM_MAP(FlexDMD)
      COM_INTERFACE_ENTRY(IFlexDMD)
      COM_INTERFACE_ENTRY(IDispatch)
   END_COM_MAP()

   STDMETHOD(get_Version)(LONG *pRetVal);
   STDMETHOD(get_RuntimeVersion)(LONG *pRetVal);
   STDMETHOD(put_RuntimeVersion)(LONG pRetVal);
   STDMETHOD(get_Run)(VARIANT_BOOL *pRetVal);
   STDMETHOD(put_Run)(VARIANT_BOOL pRetVal);
   STDMETHOD(get_Show)(VARIANT_BOOL *pRetVal);
   STDMETHOD(put_Show)(VARIANT_BOOL pRetVal);
   STDMETHOD(get_GameName)(BSTR *pRetVal);
   STDMETHOD(put_GameName)(BSTR pRetVal);
   STDMETHOD(get_Width)(unsigned short *pRetVal);
   STDMETHOD(put_Width)(unsigned short pRetVal);
   STDMETHOD(get_Height)(unsigned short *pRetVal);
   STDMETHOD(put_Height)(unsigned short pRetVal);
   STDMETHOD(get_Color)(OLE_COLOR *pRetVal);
   STDMETHOD(put_Color)(OLE_COLOR pRetVal);
   STDMETHOD(get_RenderMode)(RenderMode *pRetVal);
   STDMETHOD(put_RenderMode)(RenderMode pRetVal);
   STDMETHOD(get_ProjectFolder)(BSTR *pRetVal);
   STDMETHOD(put_ProjectFolder)(BSTR pRetVal);
   STDMETHOD(get_TableFile)(BSTR *pRetVal);
   STDMETHOD(put_TableFile)(BSTR pRetVal);
   STDMETHOD(get_Clear)(VARIANT_BOOL *pRetVal);
   STDMETHOD(put_Clear)(VARIANT_BOOL pRetVal);
   STDMETHOD(get_DmdColoredPixels)(VARIANT *pRetVal);
   STDMETHOD(get_DmdPixels)(VARIANT *pRetVal);
   STDMETHOD(putref_Segments)(VARIANT rhs);
   STDMETHOD(get_Stage)(IGroupActor **pRetVal);
   STDMETHOD(LockRenderThread)();
   STDMETHOD(UnlockRenderThread)();
   STDMETHOD(NewGroup)(BSTR Name, IGroupActor **pRetVal);
   STDMETHOD(NewFrame)(BSTR Name, IFrameActor **pRetVal);
   STDMETHOD(NewLabel)(BSTR Name, IUnknown *Font,BSTR Text, ILabelActor **pRetVal);
   STDMETHOD(NewVideo)(BSTR Name, BSTR video, IVideoActor **pRetVal);
   STDMETHOD(NewImage)(BSTR Name, BSTR image, IImageActor **pRetVal);
   STDMETHOD(NewFont)(BSTR Font, OLE_COLOR tint, OLE_COLOR borderTint, LONG borderSize, IUnknown **pRetVal);
   STDMETHOD(NewUltraDMD)(IUltraDMD **pRetVal);

   FlexDMDFont* NewFont(string szFont, OLE_COLOR tint, OLE_COLOR borderTint, LONG borderSize);

   static CComObject<FlexDMD>* Create();

   void RenderLoop();

   string GetProjectFolder() { return m_szProjectFolder; }
   
   std::map<string, FlexDMDImageAsset*> m_bitmapMap;

   CComObject<FlexDMDGroupActor>* m_pStage;

   RenderMode m_renderMode;
   OLE_COLOR m_dmdColor;

private:
   string m_szGameName;
   string m_szProjectFolder;
   string m_szTableFile;

   int m_frameRate;

   int m_width;
   int m_height;

   bool m_run;
   LONG m_runtimeVersion;

   bool m_clear;
   int m_renderLockCount;

   UINT8 m_palette[64 * 3];

   bool m_dmdColorChanged;

   UINT16 m_segData1[128];
   UINT16 m_segData2[128];

   UINT8* m_pixels;
   UINT32 m_pixelsSize;

   UINT32* m_colorPixels;
   UINT32 m_colorPixelsSize;

   std::thread* m_pThread;

   ZeDMDUtil* m_pZeDMDUtil;
};