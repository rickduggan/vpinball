#include "stdafx.h"

#include <algorithm>

#include "FlexDMD.h"
#include "FlexDMDGroupActor.h"
#include "FlexDMDImageActor.h"
#include "FlexDMDFrameActor.h"
#include "FlexDMDVideoActor.h"
#include "FlexDMDLabelActor.h"
#include "FlexDMDFont.h"
#include "FlexDMDGIFImage.h"

#include "../alphanumeric/AlphaNumeric.h"

FlexDMD::FlexDMD()
{
   m_run = false;
   m_pThread = NULL;

   m_frameRate = 60;

   m_runtimeVersion = 1008;

   m_clear = false;
   m_renderLockCount = 0;

   m_pixels = NULL;
   m_pixelsSize = 0;

   m_colorPixels = NULL;
   m_colorPixelsSize = 0;

   m_renderMode = RenderMode::RenderMode_DMD_GRAY_4;

   m_dmdColor = RGB(255, 88, 32);
   m_dmdColorChanged = true;

   m_pStage = FlexDMDGroupActor::Create(this, "Stage");

   m_pZeDMDUtil = ZeDMDUtil::GetInstance();

   m_width = 128;
   m_height = 32;
}

FlexDMD::~FlexDMD()
{
   m_run = false;

   if (m_pThread) {
      m_pThread->join();
      delete m_pThread;
   }

   if (m_pixels)
      free(m_pixels);

   if (m_colorPixels)
      free(m_colorPixels);

   m_pStage->Release();
}

CComObject<FlexDMD>* FlexDMD::Create()
{
   CComObject<FlexDMD>* obj = nullptr;
   CComObject<FlexDMD>::CreateInstance(&obj);
   obj->AddRef();

   return obj;
}

STDMETHODIMP FlexDMD::get_Version(LONG *pRetVal)
{
   *pRetVal = 9999;

   return S_OK;
}

STDMETHODIMP FlexDMD::get_RuntimeVersion(LONG *pRetVal)
{
   *pRetVal = m_runtimeVersion;

   return S_OK;
}

STDMETHODIMP FlexDMD::put_RuntimeVersion(LONG pRetVal)
{
   m_runtimeVersion = pRetVal;

   return S_OK;
}

STDMETHODIMP FlexDMD::get_Run(VARIANT_BOOL *pRetVal)
{
   *pRetVal = m_run;

   return S_OK;
}

STDMETHODIMP FlexDMD::put_Run(VARIANT_BOOL pRetVal)
{
   if (!m_run && pRetVal == VARIANT_FALSE)
      return S_OK;

   if (m_run && pRetVal == VARIANT_TRUE)
      return S_OK;

   m_run = (pRetVal == VARIANT_TRUE);

   if (m_run) {
      PLOGI.printf("Starting render thread");

      m_pThread = new std::thread([this](){
         RenderLoop();

         PLOGI.printf("Render thread finished");
      });
   }
   else {
      PLOGI.printf("Stopping render thread");

      m_pThread->join();
      delete m_pThread;

      m_pThread = NULL;
   }

   return S_OK;
}

STDMETHODIMP FlexDMD::get_Show(VARIANT_BOOL *pRetVal)
{
   *pRetVal = VARIANT_FALSE;

   return S_OK;
}

STDMETHODIMP FlexDMD::put_Show(VARIANT_BOOL pRetVal)
{
   return S_OK;
}

STDMETHODIMP FlexDMD::get_GameName(BSTR *pRetVal)
{
   CComBSTR Val(m_szGameName.c_str());
   *pRetVal = Val.Detach();

   return S_OK;
}

STDMETHODIMP FlexDMD::put_GameName(BSTR pRetVal)
{
   m_szGameName = MakeString(pRetVal);

   PLOGI.printf("Game name set to %s", m_szGameName.c_str());

   return S_OK;
}

STDMETHODIMP FlexDMD::get_Width(unsigned short *pRetVal)
{
   *pRetVal = m_width;

   return S_OK;
}

STDMETHODIMP FlexDMD::put_Width(unsigned short pRetVal)
{
   m_width = pRetVal;

   return S_OK;
}

STDMETHODIMP FlexDMD::get_Height(unsigned short *pRetVal)
{
   *pRetVal = m_height;

   return S_OK;
}

STDMETHODIMP FlexDMD::put_Height(unsigned short pRetVal)
{
   m_height = pRetVal;

   return S_OK;
}

STDMETHODIMP FlexDMD::get_Color(OLE_COLOR *pRetVal)
{
   *pRetVal = m_dmdColor;

   return S_OK;
}

STDMETHODIMP FlexDMD::put_Color(OLE_COLOR pRetVal)
{
   m_dmdColor = pRetVal;
   m_dmdColorChanged = true;

   return S_OK;
}

STDMETHODIMP FlexDMD::get_RenderMode(RenderMode *pRetVal)
{
   *pRetVal = m_renderMode;

   return S_OK;
}

STDMETHODIMP FlexDMD::put_RenderMode(RenderMode pRetVal)
{
   m_renderMode = pRetVal;

   return S_OK;
}

STDMETHODIMP FlexDMD::get_ProjectFolder(BSTR *pRetVal)
{
   CComBSTR Val(m_szProjectFolder.c_str());
   *pRetVal = Val.Detach();

   return S_OK;
}

STDMETHODIMP FlexDMD::put_ProjectFolder(BSTR pRetVal)
{
   m_szProjectFolder = normalize_path_separators(MakeString(pRetVal));

   if (!m_szProjectFolder.ends_with(PATH_SEPARATOR_CHAR))
      m_szProjectFolder += PATH_SEPARATOR_CHAR;

   PLOGI.printf("Project folder set to %s", m_szProjectFolder.c_str());

   return S_OK;
}

STDMETHODIMP FlexDMD::get_TableFile(BSTR *pRetVal)
{
   CComBSTR Val(m_szTableFile.c_str());
   *pRetVal = Val.Detach();

   return S_OK;
}

STDMETHODIMP FlexDMD::put_TableFile(BSTR pRetVal)
{
   m_szTableFile = MakeString(pRetVal);

   PLOGI.printf("Table file set to %s", m_szTableFile.c_str());

   return S_OK;
}

STDMETHODIMP FlexDMD::get_Clear(VARIANT_BOOL *pRetVal)
{
   *pRetVal = m_clear ? VARIANT_TRUE : VARIANT_FALSE;

   return S_OK;
}

STDMETHODIMP FlexDMD::put_Clear(VARIANT_BOOL pRetVal)
{
   m_clear = (pRetVal == VARIANT_TRUE);

   return S_OK;
}

STDMETHODIMP FlexDMD::get_DmdColoredPixels(VARIANT *pRetVal)
{
   const UINT32 end = m_width * m_height;

   if (!m_colorPixels || m_colorPixelsSize != end) {
      if (m_colorPixels)
         free(m_colorPixels);

      m_colorPixels = (UINT32*)malloc(end * sizeof(UINT32));
      m_colorPixelsSize = end;

      return S_FALSE;
   }

   SAFEARRAY *psa = SafeArrayCreateVector(VT_VARIANT, 0, end);

   VARIANT *pData;
   SafeArrayAccessData(psa, (void **)&pData);

   for (UINT32 i = 0; i < end; i++) {
      V_VT(&pData[i]) = VT_UI4;
      V_UI4(&pData[i]) = m_colorPixels[i];
   }
   SafeArrayUnaccessData(psa);

   V_VT(pRetVal) = VT_ARRAY | VT_VARIANT;
   V_ARRAY(pRetVal) = psa;

   return S_OK;
}

STDMETHODIMP FlexDMD::get_DmdPixels(VARIANT *pRetVal)
{
   const UINT32 end = m_width * m_height;

   if (!m_pixels || m_pixelsSize != end) {
      if (m_pixels)
         free(m_pixels);

      m_pixels = (uint8_t*)malloc(end);
      m_pixelsSize = end;

      return S_FALSE;
   }

   SAFEARRAY *psa = SafeArrayCreateVector(VT_VARIANT, 0, end);

   VARIANT *pData;
   SafeArrayAccessData(psa, (void **)&pData);

   for (UINT32 i = 0; i < end; i++) {
      V_VT(&pData[i]) = VT_UI1;
      V_UI1(&pData[i]) = m_pixels[i];
   }
   SafeArrayUnaccessData(psa);

   V_VT(pRetVal) = VT_ARRAY | VT_VARIANT;
   V_ARRAY(pRetVal) = psa;

   return S_OK;
}

STDMETHODIMP FlexDMD::putref_Segments(VARIANT rhs)
{
   int size1 = 0;
   int size2 = 0;

   switch (m_renderMode) {
      case RenderMode::RenderMode_SEG_2x16Alpha:
         size1 = 2 * 16;
         break;
      case RenderMode::RenderMode_SEG_2x20Alpha:
         size1 = 2 * 20;
         break;
      case RenderMode::RenderMode_SEG_2x7Alpha_2x7Num:
         size1 = 2 * 7 + 2 * 7;
         break;
      case RenderMode::RenderMode_SEG_2x7Alpha_2x7Num_4x1Num:
         size1 = 2 * 7 + 2 * 7 + 4;
         break;
      case RenderMode::RenderMode_SEG_2x7Num_2x7Num_4x1Num:
         size1 = 2 * 7 + 2 * 7 + 4;
         break;
      case RenderMode::RenderMode_SEG_2x7Num_2x7Num_10x1Num:
         size1 = 2 * 7 + 2 * 7 + 4;
         size2 = 6;
         break;
      case RenderMode::RenderMode_SEG_2x7Num_2x7Num_4x1Num_gen7:
         size1 = 2 * 7 + 2 * 7 + 4;
         break;
      case RenderMode::RenderMode_SEG_2x7Num10_2x7Num10_4x1Num:
         size1 = 2 * 7 + 2 * 7 + 4;
         break;
      case RenderMode::RenderMode_SEG_2x6Num_2x6Num_4x1Num:
         size1 = 2 * 6 + 2 * 6 + 4;
         break;
      case RenderMode::RenderMode_SEG_2x6Num10_2x6Num10_4x1Num:
         size1 = 2 * 6 + 2 * 6 + 4;
         break;
      case RenderMode::RenderMode_SEG_4x7Num10:
         size1 = 4 * 7;
         break;
      case RenderMode::RenderMode_SEG_6x4Num_4x1Num:
         size1 = 6 * 4 + 4;
         break;
      case RenderMode::RenderMode_SEG_2x7Num_4x1Num_1x16Alpha:
         size1 = 2 * 7 + 4 + 1;
         break;
      case RenderMode::RenderMode_SEG_1x16Alpha_1x16Num_1x7Num:
         size1 = 1 + 1 + 7;
         break;
      default:
         break;
   }

   SAFEARRAY *psa = V_ARRAY(&rhs);

   VARIANT *p;
   SafeArrayAccessData(psa, (void **)&p);

   for (int i = 0; i < size1; i++)
      m_segData1[i] = V_I2(&p[i]);

   for (int i = 0; i < size2; i++)
      m_segData2[i] = V_I2(&p[size1 + i]);

   SafeArrayUnaccessData(psa);

   return S_OK;
}

STDMETHODIMP FlexDMD::get_Stage(IGroupActor **pRetVal)
{
   return m_pStage->QueryInterface(IID_IGroupActor, (void**)pRetVal);
}

STDMETHODIMP FlexDMD::LockRenderThread()
{
   m_renderLockCount++;

   return S_OK;
}

STDMETHODIMP FlexDMD::UnlockRenderThread()
{
   m_renderLockCount--;

   return S_OK;
}

STDMETHODIMP FlexDMD::NewGroup(BSTR Name, IGroupActor **pRetVal)
{
   CComObject<FlexDMDGroupActor>* obj = FlexDMDGroupActor::Create(this, MakeString(Name));
   return obj->QueryInterface(IID_IGroupActor, (void**)pRetVal);
}

STDMETHODIMP FlexDMD::NewFrame(BSTR Name, IFrameActor **pRetVal)
{
   CComObject<FlexDMDFrameActor>* obj = FlexDMDFrameActor::Create(this, MakeString(Name));
   return obj->QueryInterface(IID_IFrameActor, (void**)pRetVal); 
}

STDMETHODIMP FlexDMD::NewLabel(BSTR Name, IUnknown *Font,BSTR Text, ILabelActor **pRetVal)
{
   CComObject<FlexDMDLabelActor>* obj = FlexDMDLabelActor::Create(this, MakeString(Name), (FlexDMDFont*)Font, MakeString(Text));
   return obj->QueryInterface(IID_ILabelActor, (void**)pRetVal);
}

STDMETHODIMP FlexDMD::NewVideo(BSTR Name, BSTR video, IVideoActor **pRetVal)
{
   string szVideo = normalize_path_separators(MakeString(video));

   if (szVideo.find("|") != std::string::npos) {
      PLOGW.printf("Not implemented: Unable to load %s", szVideo.c_str());
   }
   else {
      string ext = extension_from_path(szVideo);

      if (ext == "gif") {
          CComObject<FlexDMDGIFImage>* obj = FlexDMDGIFImage::Create(this, MakeString(Name), szVideo);

          if (obj)
             return obj->QueryInterface(IID_IVideoActor, (void**)pRetVal);
      }
      else {
         PLOGE.printf("Unable to load %s", szVideo.c_str());
      }
   }

   *pRetVal = NULL;

   return S_OK;
}

STDMETHODIMP FlexDMD::NewImage(BSTR Name, BSTR image, IImageActor **pRetVal)
{
   string szImage = MakeString(image);

   if (szImage.find("|") != std::string::npos) {
      PLOGW.printf("Not implemented: Unable to load %s", szImage.c_str());
   }
   else {
      string ext = extension_from_path(szImage);

      if (szImage.starts_with("VPX.") || ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp") {
         CComObject<FlexDMDImageActor>* obj = FlexDMDImageActor::Create(this, MakeString(Name), szImage);

         if (obj)
            return obj->QueryInterface(IID_IImageActor, (void**)pRetVal);
      }
      else {
         PLOGE.printf("Unable to load %s", szImage.c_str());
      }
   }

   *pRetVal = NULL;

   return S_OK;
}

FlexDMDFont* FlexDMD::NewFont(string szFont, OLE_COLOR tint, OLE_COLOR borderTint, LONG borderSize)
{
   FlexDMDFont* pFont = NULL;

   if (szFont.find("|") != std::string::npos) {
      PLOGW.printf("Not implemented: Unable to load %s", szFont.c_str());
   }
   else {
      string ext = extension_from_path(szFont);

      if (ext == "fnt") {
         pFont = FlexDMDFont::Create(this, szFont, tint, borderTint, borderSize);
      }
      else {
         PLOGE.printf("Unable to load %s", szFont.c_str());
      }
   }

   return pFont;
}

STDMETHODIMP FlexDMD::NewFont(BSTR Font, OLE_COLOR tint, OLE_COLOR borderTint, LONG borderSize, IUnknown **pRetVal)
{
   string szFont = MakeString(Font);

   if (szFont.find("|") != std::string::npos) {
      PLOGW.printf("Not implemented: Unable to load %s", szFont.c_str());
   }
   else {
      string ext = extension_from_path(szFont);

      if (ext == "fnt") {
         FlexDMDFont* obj = FlexDMDFont::Create(this, szFont, tint, borderTint, borderSize);

         if (obj)
            return obj->QueryInterface(IID_FlexDMDFontInterface, (void**)pRetVal);
      }
      else {
         PLOGE.printf("Unable to load %s", szFont.c_str());
      }
   }

   *pRetVal = NULL;

   return S_OK;
}

STDMETHODIMP FlexDMD::NewUltraDMD(IUltraDMD **pRetVal) { return E_NOTIMPL; }

void FlexDMD::RenderLoop()
{
   if (m_pZeDMDUtil)
      m_pZeDMDUtil->Open(m_width, m_height);

   SDL_Surface* graphics = SDL_CreateRGBSurface(0, m_width, m_height, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0);

   m_pStage->SetSize(m_width, m_height);
   m_pStage->SetOnStage(true);

   UINT8* bpFrame = (UINT8*)malloc(m_width * m_height);

   memset(m_segData1, 0, sizeof(m_segData1));
   memset(m_segData2, 0, sizeof(m_segData2));

   double elapsedMs = 0.0;

   while (m_run) {
      Uint64 startTime = SDL_GetTicks64();

      if (!m_renderLockCount) {
         if (m_clear)
            SDL_FillRect(graphics, NULL, SDL_MapRGB(graphics->format, 0, 0, 0));

         m_pStage->Update((float)(elapsedMs / 1000.0));
         m_pStage->Draw(graphics);

         UINT8* pAlphaNumericBuffer = NULL;

         switch (m_renderMode) {
            case RenderMode::RenderMode_DMD_GRAY_2:
            {
               UINT8* dst = bpFrame;
               UINT8* ptr = (UINT8*)graphics->pixels;
               int pos = 0;
               int x, y, r, g, b, v;
               for (y = 0; y < m_height; y++) {
                  for (x = 0; x < m_width; x++) {
                     r = *ptr++;
                     g = *ptr++;
                     b = *ptr++;
                     v = (int)(0.2126f * r + 0.7152f * g + 0.0722f * b);
                     if (v > 255) v = 255;
                     dst[pos] = v >> 6;
                     if (m_pixels) m_pixels[pos] = v;
                     pos++;
                  }
               }
               if (m_dmdColorChanged) {
                  generate_palette(m_palette, 2, m_dmdColor);
                  if (m_pZeDMDUtil)
                     m_pZeDMDUtil->SetPalette(m_palette);

                  m_dmdColorChanged = false;
               }
               if (m_pZeDMDUtil)
                  m_pZeDMDUtil->RenderGray2(dst);
            }
            break;

            case RenderMode::RenderMode_DMD_GRAY_4:
            {
               UINT8* dst = bpFrame;
               UINT8* ptr = (UINT8*)graphics->pixels;
               int pos = 0;
               int x, y, r, g, b, v;
               for (y = 0; y < m_height; y++) {
                  for (x = 0; x < m_width; x++) {
                     r = *ptr++;
                     g = *ptr++;
                     b = *ptr++;
                     v = (int)(0.2126f * r + 0.7152f * g + 0.0722f * b);
                     if (v > 255) v = 255;
                     dst[pos] = v >> 4;
                     if (m_pixels) m_pixels[pos] = v;
                     pos++;
                  }
               }
               if (m_dmdColorChanged) {
                  generate_palette(m_palette, 4, m_dmdColor);
                  if (m_pZeDMDUtil)
                     m_pZeDMDUtil->SetPalette(m_palette);

                  m_dmdColorChanged = false;
               }
               if (m_pZeDMDUtil)
                  m_pZeDMDUtil->RenderGray4(dst);
            }
            break;

            case RenderMode::RenderMode_DMD_RGB:
            {
               if (m_pixels) {
                  UINT8* ptr = (UINT8*)graphics->pixels;
                  int pos = 0;
                  int x, y, r, g, b, v;
                  for (y = 0; y < m_height; y++) {
                     for (x = 0; x < m_width; x++) {
                        r = *ptr++;
                        g = *ptr++;
                        b = *ptr++;
                        v = (int)(0.2126f * r + 0.7152f * g + 0.0722f * b);
                        if (v > 255) v = 255;
                        m_pixels[pos] = v;
                        pos++;
                     }
                  }
               }
               if (m_pZeDMDUtil)
                  m_pZeDMDUtil->RenderRgb24((UINT8*)graphics->pixels);
            }
            break;

            case RenderMode::RenderMode_SEG_2x16Alpha:
            case RenderMode::RenderMode_SEG_2x20Alpha:
            case RenderMode::RenderMode_SEG_2x7Alpha_2x7Num:
            case RenderMode::RenderMode_SEG_2x7Alpha_2x7Num_4x1Num:
            case RenderMode::RenderMode_SEG_2x7Num_2x7Num_4x1Num:
            case RenderMode::RenderMode_SEG_2x7Num_2x7Num_10x1Num:
            case RenderMode::RenderMode_SEG_2x7Num_2x7Num_4x1Num_gen7:
            case RenderMode::RenderMode_SEG_2x7Num10_2x7Num10_4x1Num:
            case RenderMode::RenderMode_SEG_2x6Num_2x6Num_4x1Num:
            case RenderMode::RenderMode_SEG_2x6Num10_2x6Num10_4x1Num:
            case RenderMode::RenderMode_SEG_4x7Num10:
            case RenderMode::RenderMode_SEG_6x4Num_4x1Num:
            case RenderMode::RenderMode_SEG_2x7Num_4x1Num_1x16Alpha:
            case RenderMode::RenderMode_SEG_1x16Alpha_1x16Num_1x7Num:
            {
                if (m_width == 128 && m_height == 32) {
                   pAlphaNumericBuffer = AlphaNumeric::Render((NumericalLayout)(m_renderMode - 2), m_segData1, m_segData2, true);
                   UINT8* ptr = pAlphaNumericBuffer;
                   int pos = 0;
                   int x, y, v;
                   for (y = 0; y < m_height; y++) {
                      for (x = 0; x < m_width; x++) {
                        v = (int)((*ptr++ / 3) * 255);
                        if (v > 255) v = 255;
                        if (m_pixels) m_pixels[pos] = v;
                        pos++;
                      }
                   }
                   if (m_dmdColorChanged) {
                      generate_palette(m_palette, 2, m_dmdColor);
                      if (m_pZeDMDUtil)
                         m_pZeDMDUtil->SetPalette(m_palette);

                      m_dmdColorChanged = false;
                   }
                   if (m_pZeDMDUtil)
                      m_pZeDMDUtil->RenderGray2(pAlphaNumericBuffer);
                }
            }
            break;

            default:
            break;
         }

         if (m_colorPixels) {
            int pos = 0;
            int x, y, r, g, b;
            if (!pAlphaNumericBuffer) {
               UINT8* ptr = (UINT8*)graphics->pixels;
               for (y = 0; y < m_height; y++) {
                  for (x = 0; x < m_width; x++) {
                     r = *ptr++;
                     g = *ptr++;
                     b = *ptr++;
                     m_colorPixels[pos++] = ((b << 16) + (g << 8) + r);
                  }
               }
            }
            else {
               UINT8* ptr = pAlphaNumericBuffer;
               int index;
               for (y = 0; y < m_height; y++) {
                  for (x = 0; x < m_width; x++) {
                     index = *ptr++ * 3;
                     r = m_palette[index];
                     g = m_palette[index + 1];
                     b = m_palette[index + 2];
                     m_colorPixels[pos++] = ((b << 16) + (g << 8) + r);
                  }
               }
            }
         }
      }

      double renderingDuration = SDL_GetTicks64() - startTime;

      int sleepMs = (1000 / m_frameRate) - (int)renderingDuration;

      if (sleepMs > 1)
         SDL_Delay(sleepMs);

      elapsedMs = SDL_GetTicks64() - startTime;

      if (elapsedMs > 4000 / m_frameRate) {
         PLOGI.printf("Abnormally long elapsed time between frames of %fs (rendering lasted %fms, sleeping was %dms), limiting to %dms", 
            elapsedMs / 1000.0, renderingDuration, std::max(0, sleepMs), 4000 / m_frameRate);

         elapsedMs = 4000 / m_frameRate;
      }
   }

   if (m_pZeDMDUtil)
      m_pZeDMDUtil->Close();

   m_pStage->SetOnStage(false);

   free(bpFrame);

   SDL_FreeSurface(graphics);
}