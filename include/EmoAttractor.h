//
//  EmoAttractor.h
//  emotion
//
//  Created by Nikolas on 15.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef emotion_EmoAttractor_h
#define emotion_EmoAttractor_h

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"

#include <vector>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

class EmoAttractor {
    public:
        EmoAttractor();
        void create( MSA::Physics::World3D * _physics, float _worldRadius, ci::Vec3f _worldCenter );
        void setFrustration();
        void setEngagement();
        void calcStartPositions();    
        void makeAttractors();
        void update( float _level, float _min = 0, float _max = 100  );
        void movePositions();
        void changeWorld( ci::Vec3f _wCenter, float _wRadius );
        void setPositions();
        void render();
    
    
        MSA::Physics::World3D * mPhysics;
    
        float       mWorldRadius;
        ci::Vec3f   mWorldCenter;
    
        float       mARadius;
        float       mAMass;
        ci::Vec3f   mAPositions[4];
        float       mLevel;
    
        bool        mExists;
        int         mTyp;   //1 = Frustration // 2 = Engagement
        
        MSA::Physics::Particle3D * mAttractor[4];
        
};

#endif
