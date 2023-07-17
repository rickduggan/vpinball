#pragma once

#include "flexdmd_i.h"

#include "FlexDMDScene.h"

class FlexDMDBackgroundScene : public FlexDMDScene
{
public:
   FlexDMDBackgroundScene();
   ~FlexDMDBackgroundScene();

   static CComObject<FlexDMDBackgroundScene>* Create(FlexDMD* pFlexDMD);
};