#pragma once

#include "flexdmd_i.h"

#include "FlexDMDGroupActor.h"
#include "FlexDMDScene.h"

class FlexDMDSequence : public FlexDMDGroupActor
{
public:
   FlexDMDSequence();
   ~FlexDMDSequence();

   static CComObject<FlexDMDSequence>* Create(FlexDMD* pFlexDMD);

   void Enqueue(FlexDMDScene* scene);
   void RemoveAllScenes();
   void RemoveScene(string name);
   bool IsFinished();

   virtual void Update(float delta);
   virtual void Draw(SDL_Surface* graphics);

   vector<FlexDMDScene*> m_pendingScenes;
   bool m_finished;
   FlexDMDScene* m_pActiveScene;
};