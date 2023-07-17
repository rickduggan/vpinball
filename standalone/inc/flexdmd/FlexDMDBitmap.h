#pragma once

#include "flexdmd_i.h"

#include "FlexDMD.h"

DEFINE_GUID(IID_FlexDMDBitmapInterface, 0x01234567, 0x89AB, 0xCDEF, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF);

class FlexDMDBitmapInterface : public IUnknown {
public:
};

class FlexDMDBitmap : public IUnknown {
public:
    FlexDMDBitmap();
    ~FlexDMDBitmap();

   STDMETHOD(QueryInterface)(REFIID iid, void** ppv) {
       if (iid == IID_FlexDMDBitmapInterface) {
          *ppv = reinterpret_cast<FlexDMDBitmapInterface*>(this);
          AddRef();
          return S_OK;
       } else {
          *ppv = nullptr;
          return E_NOINTERFACE;
       }
   }

   STDMETHOD_(ULONG, AddRef)() {
      m_dwRef++; 
      return m_dwRef;
   }

   STDMETHOD_(ULONG, Release)() {
      m_dwRef--;

      if (m_dwRef == 0)
         delete this;

      return m_dwRef;
   }

   static FlexDMDBitmap* Create(FlexDMD* pFlexDMD, SDL_Surface* pSurface);

   int GetWidth() { return m_pSurface->w; }
   int GetHeight()  { return m_pSurface->h; }

   SDL_Surface* m_pSurface;

protected:
   FlexDMD* m_pFlexDMD;

private:
   LONG m_dwRef = 0;
};