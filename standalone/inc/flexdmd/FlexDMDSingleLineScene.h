#pragma once

#include "flexdmd_i.h"

#include "FlexDMDScene.h"

class FlexDMDSingleLineScene : public FlexDMDScene
{
public:
   FlexDMDSingleLineScene();
   ~FlexDMDSingleLineScene();

   static CComObject<FlexDMDSingleLineScene>* Create(FlexDMD* pFlexDMD);
};