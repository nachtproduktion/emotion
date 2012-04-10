#ifndef emotion_AttractorPoint_h
#define emotion_AttractorPoint_h

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Vector.h"

#include <vector>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "niko_functionen.h"
#include "CharacterPoint.h"

class AttractorPoint {
    public:
        AttractorPoint();
        void create( MSA::Physics::World3D * _physics, ci::Vec3f _pos = ci::Vec3f::zero() );
    
        void activate();
        void deactivate();
    
        void update();
    
        void render();
    
        MSA::Physics::World3D * mPhysics;
        MSA::Physics::Particle3D * mAttractor;
    
        Anim<float>       mRadius;
        Anim<float>       mMass;
        Anim<ci::Vec3f>   mPosition;
    
};

#endif
