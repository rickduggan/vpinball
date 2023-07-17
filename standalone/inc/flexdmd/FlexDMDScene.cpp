#include "stdafx.h"
#include "FlexDMDScene.h"

FlexDMDScene::FlexDMDScene()
{
   m_visible = true;
   m_active = false;
}

FlexDMDScene::~FlexDMDScene()
{
}

CComObject<FlexDMDScene>* FlexDMDScene::Create(FlexDMD* pFlexDMD, AnimationType animateIn, float pauseS, AnimationType animateOut, string id)
{
   CComObject<FlexDMDScene>* obj = nullptr;
   CComObject<FlexDMDScene>::CreateInstance(&obj);
   obj->AddRef();

   obj->m_pFlexDMD = pFlexDMD;
   obj->m_animateIn = animateIn;
   obj->m_animateOut = animateOut;
   obj->m_pause = pauseS;
   obj->m_szName = id;

   return obj;
}

bool FlexDMDScene::GetVisible()
{
   return m_visible;
}

void FlexDMDScene::SetVisible(bool visible)
{
   m_visible = visible;
   UpdateState();
}

void FlexDMDScene::OnStageStateChanged()
{
   FlexDMDGroupActor::OnStageStateChanged();
   UpdateState();
}

void FlexDMDScene::UpdateState()
{
   bool shouldBeActive = m_visible && m_onStage;
   if (shouldBeActive && !m_active) {
      m_active = true;
      SetSize(GetParent()->GetWidth(), GetParent()->GetHeight());
      m_inAnimLength = StartAnimation(m_animateIn);
      m_outAnimLength = -1;
      Begin();
   }
   else if (!shouldBeActive && m_active) {
      m_active = false;
      //_tweener.Cancel();
   }
}

void FlexDMDScene::Begin()
{
}

float FlexDMDScene::StartAnimation(AnimationType animation)
{
   float alphaLength = 0.5f;
   float scrollWLength = 0.5f;
   float scrollHLength = scrollWLength * m_height / m_width;
   // TODO Missing animations: ZoomIn = 2, ZoomOut = 3
   switch (animation) {
      case AnimationType_FadeIn:
      {
         //FadeOverlay fade = new FadeOverlay();
         //AddActor(fade);
         //fade.Alpha = 1f;
         //fade.Color = Color.Black;
         //_tweener.Tween(fade, new { Alpha = 0f }, alphaLength);
         //return alphaLength;
      }
      case AnimationType_FadeOut:
      {
         //FadeOverlay fade = new FadeOverlay();
         //AddActor(fade);
         //fade.Alpha = 0f;
         //fade.Color = Color.Black;
         //_tweener.Tween(fade, new { Alpha = 1f }, alphaLength);
         //return alphaLength;
      }
      case AnimationType_ScrollOffLeft:
      {
         //X = 0;
         //_tweener.Tween(this, new { X = -Width }, scrollWLength);
         //   return scrollWLength;
      }
      case AnimationType_ScrollOffRight:
      {
         //X = 0;
         //_tweener.Tween(this, new { X = Width }, scrollWLength);
         //return scrollWLength;
      }
      case AnimationType_ScrollOnLeft:
      {
         //X = Width;
         //_tweener.Tween(this, new { X = 0f }, scrollWLength);
         //return scrollWLength;
      }
      case AnimationType_ScrollOnRight:
      {
         //X = -Width;
         //_tweener.Tween(this, new { X = 0f }, scrollWLength);
         //return scrollWLength;
      }
      case AnimationType_ScrollOffUp:
      {
         //Y = 0;
         //_tweener.Tween(this, new { Y = -Height }, scrollHLength);
         //return scrollHLength;
      }
      case AnimationType_ScrollOffDown:
      {
         //Y = 0;
         //_tweener.Tween(this, new { Y = Height }, scrollHLength);
         //return scrollHLength;
      }
      case AnimationType_ScrollOnUp:
      {
         //Y = Height;
         //_tweener.Tween(this, new { Y = 0f }, scrollHLength);
         //return scrollHLength;
      }
      case AnimationType_ScrollOnDown:
      {
         //Y = -Height;
         //_tweener.Tween(this, new { Y = 0f }, scrollHLength);
         //return scrollHLength;
      }
      case AnimationType_FillFadeIn:
      {
        //FadeOverlay fade = new FadeOverlay();
        //AddActor(fade);
        //fade.Alpha = 1f;
        //fade.Color = Color.Black;
        //fade = new FadeOverlay();
        //AddActor(fade);
        ///fade.Alpha = 0f;
        //fade.Color = Color.White;
        //_tweener.Tween(fade, new { Alpha = 1f }, alphaLength);
        //return alphaLength;
      }
      case AnimationType_FillFadeOut:
      {
        //FadeOverlay fade = new FadeOverlay();
        //AddActor(fade);
        //fade.Alpha = 1f;
        //fade.Color = Color.White;
        //fade = new FadeOverlay();
        //AddActor(fade);
        //fade.Alpha = 0f;
        //fade.Color = Color.Black;
        //_tweener.Tween(fade, new { Alpha = 1f }, alphaLength);
        //return alphaLength;
      }
      case AnimationType_None:
         return 0.f;
      default:
         PLOGE.printf("Unsupported animation in scene '%s': %d", m_szName.c_str(), animation);
         return 0.f;
   }
}

void FlexDMDScene::Update(float secondsElapsed)
{
   FlexDMDGroupActor::Update(secondsElapsed);
   m_time += secondsElapsed;
   if (m_pause >= 0.f && m_outAnimLength < 0 && m_time >= m_inAnimLength + m_pause)
      m_outAnimLength = StartAnimation(m_animateOut);
   //_tweener.Update(secondsElapsed);
}

bool FlexDMDScene::IsFinished()
{
   return m_pause >= 0.f && m_outAnimLength >= 0 && m_time >= m_inAnimLength + m_pause + m_outAnimLength;
}