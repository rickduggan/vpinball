#include "stdafx.h"
#include "FlexDMDSingleLineScene.h"

FlexDMDSingleLineScene::FlexDMDSingleLineScene()
{
}

FlexDMDSingleLineScene::~FlexDMDSingleLineScene()
{
}

CComObject<FlexDMDSingleLineScene>* FlexDMDSingleLineScene::Create(FlexDMD* pFlexDMD)
{
   CComObject<FlexDMDSingleLineScene>* obj = nullptr;
   CComObject<FlexDMDSingleLineScene>::CreateInstance(&obj);
   obj->AddRef();

   obj->m_pFlexDMD = pFlexDMD;

   return obj;
}