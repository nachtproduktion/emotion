#ifndef emotion_ParticleController_h
#define emotion_ParticleController_h

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"

#include "cinder/BSpline.h"
#include "cinder/Matrix.h"
#include "cinder/TriMesh.h"

#include <list>
#include <vector>

#include "Particle.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ParticleController {
    public:
        ParticleController();
        
        void setCircleRadius( float _radius );
    
        void addParticles( int _number, bool _statical = false );
    
        void updateMatrix( ci::Matrix44f _matrix );
        void update( ci::Vec3f _mpoint );
    
        void render();
    
    private:
    
        float                               mCircleRadius;			// points to tube (read extrude)
        ci::Matrix44f                       mMatrix;
    
        std::list<Particle>	mParticles;
    
    /*
        void setSphere( float _radius );
        void setCircle( ci::Vec3f _pos, float _radius );
    
        
        void setTarget( ci::Vec3f _target ) ;
        void calcVelocity();
    
        void update( ci::Vec3f _pos );
        void update();
        void draw();
    
        
        ci::Vec3f       mPosition;
        ci::Vec3f       mTarget;
        ci::Vec3f       mVelocity;
    
        float mSphereRadius;
        float mCircleRadius;
     
     */

};

#endif
