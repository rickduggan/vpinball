#include "stdafx.h"
#include "FlexDMDTwoLineScene.h"

FlexDMDTwoLineScene::FlexDMDTwoLineScene()
{
}

FlexDMDTwoLineScene::~FlexDMDTwoLineScene()
{
}

CComObject<FlexDMDTwoLineScene>* FlexDMDTwoLineScene::Create(FlexDMD* pFlexDMD)
{
   CComObject<FlexDMDTwoLineScene>* obj = nullptr;
   CComObject<FlexDMDTwoLineScene>::CreateInstance(&obj);
   obj->AddRef();

   obj->m_pFlexDMD = pFlexDMD;

   return obj;
}
