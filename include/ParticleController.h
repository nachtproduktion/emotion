#ifndef emotion_ParticleController_h
#define emotion_ParticleController_h

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"

#include <list>
#include <vector>

#include "Particle.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ParticleController {
    public:
        ParticleController();
        void addParticles( int _number );
        void setTarget( ci::Vec3f _target ) ;
        void calcVelocity();
        void update();
        void draw();
    
        std::list<Particle>	mParticles;
        ci::Vec3f mPosition;
        ci::Vec3f mTarget;
        ci::Vec3f mVelocity;


};

#endif
