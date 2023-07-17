#pragma once

#include "flexdmd_i.h"

#include "FlexDMDScene.h"

class FlexDMDTwoLineScene : public FlexDMDScene
{
public:
   FlexDMDTwoLineScene();
   ~FlexDMDTwoLineScene();

   static CComObject<FlexDMDTwoLineScene>* Create(FlexDMD* pFlexDMD);
};