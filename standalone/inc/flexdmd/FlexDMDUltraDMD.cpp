#include "stdafx.h"

#include "FlexDMDUltraDMD.h"
#include "FlexDMDSingleLineScene.h"
#include "FlexDMDTwoLineScene.h"
#include "FlexDMDBackgroundScene.h"

FlexDMDUltraDMD::FlexDMDUltraDMD()
{
   m_pFlexDMD = FlexDMD::Create();
   m_pQueue = FlexDMDSequence::Create(m_pFlexDMD);
   m_pQueue->put_FillParent(VARIANT_TRUE);

   m_pScoreFontText = new FontDef("FlexDMD.Resources.udmd-f4by5.fnt", RGB(168, 168, 168), RGB(255, 255, 255));
   m_pScoreFontNormal = new FontDef("FlexDMD.Resources.udmd-f5by7.fnt", RGB(168, 168, 168), RGB(255, 255, 255));
   m_pScoreFontHighlight = new FontDef("FlexDMD.Resources.udmd-f6by12.fnt", RGB(255, 255, 255), RGB(255, 255, 255));

   m_pSingleLineFont[0] = new FontDef("FlexDMD.Resources.udmd-f14by26.fnt", RGB(255, 255, 255), RGB(255, 255, 255));
   m_pSingleLineFont[1] = new FontDef("FlexDMD.Resources.udmd-f12by24.fnt", RGB(255, 255, 255), RGB(255, 255, 255));
   m_pSingleLineFont[2] = new FontDef("FlexDMD.Resources.udmd-f7by13.fnt", RGB(255, 255, 255), RGB(255, 255, 255));

   m_pTwoLinesFontTop = new FontDef("FlexDMD.Resources.udmd-f5by7.fnt", RGB(255, 255, 255), RGB(255, 255, 255));
   m_pTwoLinesFontBottom = new FontDef("FlexDMD.Resources.udmd-f6by12.fnt", RGB(255, 255, 255), RGB(255, 255, 255));

   FlexDMDFont* pScoreFontText = m_pFlexDMD->NewFont(m_pScoreFontText->m_szPath, m_pScoreFontText->m_tint, m_pScoreFontText->m_borderTint, m_pScoreFontText->m_borderSize);
   FlexDMDFont* pScoreFontNormal = m_pFlexDMD->NewFont(m_pScoreFontNormal->m_szPath, m_pScoreFontNormal->m_tint, m_pScoreFontNormal->m_borderTint, m_pScoreFontNormal->m_borderSize);
   FlexDMDFont* pScoreFontHighlight = m_pFlexDMD->NewFont(m_pScoreFontHighlight->m_szPath, m_pScoreFontHighlight->m_tint, m_pScoreFontHighlight->m_borderTint, m_pScoreFontHighlight->m_borderSize);

   m_pScoreBoard = FlexDMDScoreBoard::Create(m_pFlexDMD, pScoreFontText, pScoreFontNormal, pScoreFontHighlight);
   m_pScoreBoard->SetVisible(false);

   m_pFlexDMD->m_pStage->AddActor((FlexDMDBaseActor*)m_pScoreBoard);
   m_pFlexDMD->m_pStage->AddActor((FlexDMDBaseActor*)m_pQueue);

   m_stretchMode = 0;
}

FlexDMDUltraDMD::~FlexDMDUltraDMD()
{
   m_pFlexDMD->Release();

   delete m_pScoreFontText;
   delete m_pScoreFontNormal;
   delete m_pScoreFontHighlight;

   delete m_pSingleLineFont[0];
   delete m_pSingleLineFont[1];
   delete m_pSingleLineFont[2];

   delete m_pTwoLinesFontTop;
   delete m_pTwoLinesFontBottom;
}

CComObject<FlexDMDUltraDMD>* FlexDMDUltraDMD::Create()
{
   CComObject<FlexDMDUltraDMD>* obj = nullptr;
   CComObject<FlexDMDUltraDMD>::CreateInstance(&obj);
   obj->AddRef();

   return obj;
}

STDMETHODIMP FlexDMDUltraDMD::get_FloatProperty(single *pRetVal)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::put_FloatProperty(single pRetVal)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::Init()
{
   return m_pFlexDMD->put_Run(VARIANT_TRUE);
}

STDMETHODIMP FlexDMDUltraDMD::Uninit()
{
   return m_pFlexDMD->put_Run(VARIANT_FALSE);
}

STDMETHODIMP FlexDMDUltraDMD::GetMajorVersion(LONG *pRetVal)
{
   *pRetVal = 1;

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::GetMinorVersion(LONG *pRetVal)
{
   *pRetVal = 9999;

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::GetBuildNumber(LONG *pRetVal)
{
   *pRetVal = 9999;

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::SetVisibleVirtualDMD(VARIANT_BOOL bHide, VARIANT_BOOL *pRetVal)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::SetFlipY(VARIANT_BOOL flipY, VARIANT_BOOL *pRetVal)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::IsRendering(VARIANT_BOOL *pRetVal)
{
   *pRetVal = !m_pQueue->IsFinished() ? VARIANT_TRUE : VARIANT_FALSE;

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::CancelRendering()
{
   m_pQueue->RemoveAllScenes();

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::CancelRenderingWithId(BSTR sceneId)
{
   m_pQueue->RemoveScene(MakeString(sceneId));

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::Clear()
{
   //m_pFlexDMD->Graphics.Clear(Color.Black);
   m_pScoreBoard->SetVisible(false);
   if (m_pQueue->IsFinished())
      m_pQueue->SetVisible(false);

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::SetProjectFolder(BSTR basePath)
{
   return m_pFlexDMD->put_ProjectFolder(basePath);
}

STDMETHODIMP FlexDMDUltraDMD::SetVideoStretchMode(LONG mode)
{
   m_stretchMode = mode;

   return S_OK;
}

FlexDMDFont* FlexDMDUltraDMD::GetFont(string path, float brightness, float outlineBrightness)
{
   brightness = brightness > 1.f ? 1.f : brightness;
   outlineBrightness = outlineBrightness > 1.f ? 1.f : outlineBrightness;
   OLE_COLOR baseColor = m_pFlexDMD->m_renderMode == RenderMode_DMD_RGB ? m_pFlexDMD->m_dmdColor : RGB(255, 255, 255);

   UINT8 baseColorR = baseColor & 0xFF;
   UINT8 baseColorG = (baseColor >> 8) & 0xFF;
   UINT8 baseColorB = (baseColor >> 16) & 0xFF;

   OLE_COLOR tint = brightness >= 0.f ? RGB((int)(baseColorR * brightness), (int)(baseColorG * brightness), (int)(baseColorB * brightness)) : RGB(0, 0, 0);
   if (outlineBrightness >= 0.f) {
      OLE_COLOR borderTint = RGB((int)(baseColorR * outlineBrightness), (int)(baseColorG * outlineBrightness), (int)(baseColorB * outlineBrightness));
      return m_pFlexDMD->NewFont(path, tint, borderTint, 1);
   }

   return m_pFlexDMD->NewFont(path, tint, RGB(255, 255, 255), 0);
}

STDMETHODIMP FlexDMDUltraDMD::SetScoreboardBackgroundImage(BSTR filename, LONG selectedBrightness, LONG unselectedBrightness)
{
   m_pScoreBoard->SetBackground(ResolveImage(MakeString(filename), false));

   m_pScoreBoard->SetFonts(
      GetFont(m_pScoreFontNormal->m_szPath, unselectedBrightness / 15.f, -1),
      GetFont(m_pScoreFontHighlight->m_szPath, selectedBrightness / 15.f, -1),
      GetFont(m_pScoreFontText->m_szPath, unselectedBrightness / 15.f, -1));

   return S_OK;
}

CComObject<FlexDMDImageActor>* FlexDMDUltraDMD::ResolveImage(string filename, bool useFrame)
{
  CComObject<FlexDMDImageActor>* pImageActor;

  if (filename.find("|") != std::string::npos) {
      PLOGW.printf("Not implemented: Unable to load %s", filename.c_str());
   }
   else {
      string ext = extension_from_path(filename);

      if (filename.starts_with("VPX.") || ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "bmp") {
         pImageActor = FlexDMDImageActor::Create(m_pFlexDMD, "", filename);
      }
      else {
         PLOGE.printf("Unable to load %s", filename.c_str());
      }
   }

   return pImageActor;
}

STDMETHODIMP FlexDMDUltraDMD::CreateAnimationFromImages(LONG fps, VARIANT_BOOL loop, BSTR imagelist, LONG *pRetVal)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::RegisterVideo(LONG videoStretchMode, VARIANT_BOOL loop, BSTR videoFilename, LONG *pRetVal)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::RegisterFont(BSTR file, LONG *pRetVal)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::UnregisterFont(LONG registeredFont)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::DisplayVersionInfo()
{
   m_pScoreBoard->SetVisible(false);
   m_pQueue->SetVisible(false);

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::DisplayScoreboard(LONG cPlayers, LONG highlightedPlayer, LONG score1, LONG score2, LONG score3, LONG score4, BSTR lowerLeft, BSTR lowerRight)
{
   m_pScoreBoard->SetNPlayers(cPlayers);
   m_pScoreBoard->SetHighlightedPlayer(highlightedPlayer);
   m_pScoreBoard->SetScore(score1, score2, score3, score4);
   m_pScoreBoard->m_pLowerLeft->put_Text(lowerLeft);
   m_pScoreBoard->m_pLowerRight->put_Text(lowerRight);
   if (m_pQueue->IsFinished()) {
      m_pQueue->SetVisible(false);
      m_pScoreBoard->SetVisible(true);
   }

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::DisplayScoreboard00(LONG cPlayers, LONG highlightedPlayer, LONG score1, LONG score2, LONG score3, LONG score4, BSTR lowerLeft, BSTR lowerRight)
{
   return DisplayScoreboard(cPlayers, highlightedPlayer, score1, score2, score3, score4, lowerLeft, lowerRight);
}

STDMETHODIMP FlexDMDUltraDMD::DisplayScene00(BSTR background, BSTR toptext, LONG topBrightness, BSTR bottomtext, LONG bottomBrightness, LONG animateIn, LONG pauseTime, LONG animateOut)
{
   BSTR sceneId = SysAllocString(L"");
   HRESULT hres = DisplayScene00ExWithId(sceneId, false, background, toptext, topBrightness, -15, bottomtext, bottomBrightness, -15, animateIn, pauseTime, animateOut);
   SysFreeString(sceneId);

   return hres;
}

STDMETHODIMP FlexDMDUltraDMD::DisplayScene00Ex(BSTR background, BSTR toptext, LONG topBrightness, LONG topOutlineBrightness, BSTR bottomtext, LONG bottomBrightness, LONG bottomOutlineBrightness, LONG animateIn, LONG pauseTime, LONG animateOut)
{
   BSTR sceneId = SysAllocString(L"");
   HRESULT hres = DisplayScene00ExWithId(sceneId, false, background, toptext, topBrightness, topOutlineBrightness, bottomtext, bottomBrightness, bottomOutlineBrightness, animateIn, pauseTime, animateOut);
   SysFreeString(sceneId);

   return hres;
}

STDMETHODIMP FlexDMDUltraDMD::DisplayScene00ExWithId(BSTR sceneId, VARIANT_BOOL cancelPrevious, BSTR background, BSTR toptext, LONG topBrightness, LONG topOutlineBrightness, BSTR bottomtext, LONG bottomBrightness, LONG bottomOutlineBrightness, LONG animateIn, LONG pauseTime, LONG animateOut)
{
   string szSceneId = (sceneId != NULL) ? MakeString(sceneId) : "";

   if (cancelPrevious == VARIANT_TRUE && szSceneId.length() > 0) {
      FlexDMDScene* pScene = m_pQueue->m_pActiveScene;
      if (pScene != NULL && pScene->GetName() == szSceneId)
         m_pQueue->RemoveScene(szSceneId);
   }
   m_pScoreBoard->SetVisible(false);
   m_pQueue->SetVisible(true);

   string szTopText = (toptext != NULL) ? MakeString(toptext) : "";
   string szBottomText = (bottomtext != NULL) ? MakeString(bottomtext) : "";

   if (szTopText.length() > 0 && szBottomText.length() > 0) {
      FlexDMDFont* pFontTop = GetFont(m_pTwoLinesFontTop->m_szPath, topBrightness / 15.f, topOutlineBrightness / 15.f);
      FlexDMDFont* pFontBottom = GetFont(m_pTwoLinesFontTop->m_szPath, bottomBrightness / 15.f, bottomOutlineBrightness / 15.f);
      //FlexDMDTwoLineScene* pScene = new TwoLineScene(_flexDMD, ResolveImage(background, true), toptext, fontTop, bottomtext, fontBottom, (AnimationType)animateIn, pauseTime / 1000f, (AnimationType)animateOut, sceneId);
      FlexDMDTwoLineScene* pScene = FlexDMDTwoLineScene::Create(m_pFlexDMD);
      m_pQueue->Enqueue(pScene);
   }
   else if (szTopText.length() > 0) {
      //var font = GetFittedLabel(toptext, topBrightness / 15f, topOutlineBrightness / 15f).Font;
      //var scene = new SingleLineScene(_flexDMD, ResolveImage(background, true), toptext, font, (AnimationType)animateIn, pauseTime / 1000f, (AnimationType)animateOut, false, sceneId);
      FlexDMDSingleLineScene* pScene = FlexDMDSingleLineScene::Create(m_pFlexDMD);
      m_pQueue->Enqueue(pScene);
   }
   else if (szBottomText.length() > 0) {
      //var font = GetFittedLabel(bottomtext, bottomBrightness / 15f, bottomOutlineBrightness / 15f).Font;
      //var scene = new SingleLineScene(_flexDMD, ResolveImage(background, true), bottomtext, font, (AnimationType)animateIn, pauseTime / 1000f, (AnimationType)animateOut, false, sceneId);
      FlexDMDSingleLineScene* pScene = FlexDMDSingleLineScene::Create(m_pFlexDMD);
      m_pQueue->Enqueue(pScene);
   }
   else {
      //var scene = new BackgroundScene(_flexDMD, ResolveImage(background, true), (AnimationType)animateIn, pauseTime / 1000f, (AnimationType)animateOut, sceneId);
      //_queue.Enqueue(scene);
      FlexDMDBackgroundScene* pScene = FlexDMDBackgroundScene::Create(m_pFlexDMD);
      m_pQueue->Enqueue(pScene);
   }

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::ModifyScene00(BSTR id, BSTR toptext, BSTR bottomtext)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::ModifyScene00Ex(BSTR id, BSTR toptext, BSTR bottomtext, LONG pauseTime)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::DisplayScene01(BSTR sceneId, BSTR background, BSTR text, LONG textBrightness, LONG textOutlineBrightness, LONG animateIn, LONG pauseTime, LONG animateOut)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::DisplayText(BSTR text, LONG textBrightness, LONG textOutlineBrightness)
{
   PLOGW << "Not implemented";

   return S_OK;
}

STDMETHODIMP FlexDMDUltraDMD::ScrollingCredits(BSTR background, BSTR text, LONG textBrightness, LONG animateIn, LONG pauseTime, LONG animateOut)
{
   PLOGW << "Not implemented";

   return S_OK;
}