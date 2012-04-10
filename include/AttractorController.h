#ifndef emotion_AttractorController_h
#define emotion_AttractorController_h

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

#include <vector>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "niko_functionen.h"
#include "CharacterPoint.h"
#include "AttractorPoint.h"

class AttractorController {
    public:
        AttractorController();
        void init( MSA::Physics::World3D * _physics, std::vector < CharacterPoint >* _cpointList );
    
        void addStart();
        void removeStart();
        AttractorPoint * getStartAttractor();
    
        void update();
        
    
        MSA::Physics::World3D *         mPhysics;
        std::vector < CharacterPoint >* mPointList;
        AttractorPoint                  mStartAttractor;
        
        std::vector < MSA::Physics::Attraction3D* > mStartAttractions;
    
};

#endif
