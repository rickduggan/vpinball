#include "stdafx.h"
#include "FlexDMDBackgroundScene.h"

FlexDMDBackgroundScene::FlexDMDBackgroundScene()
{
}

FlexDMDBackgroundScene::~FlexDMDBackgroundScene()
{
}

CComObject<FlexDMDBackgroundScene>* FlexDMDBackgroundScene::Create(FlexDMD* pFlexDMD)
{
   CComObject<FlexDMDBackgroundScene>* obj = nullptr;
   CComObject<FlexDMDBackgroundScene>::CreateInstance(&obj);
   obj->AddRef();

   obj->m_pFlexDMD = pFlexDMD;

   return obj;
}