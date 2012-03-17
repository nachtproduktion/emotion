#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Sphere.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Easing.h"
#include "cinder/gl/Texture.h"

#include <list>
#include <vector>


#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "cinder/Perlin.h"

#include "niko_functionen.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MSA;

//PHYSICS
#define	SPRING_STRENGTH         0.01f
#define BOUNCE                  0.2f

#define	GRAVITY					0.0f
#define FORCE_AMOUNT			10.0f

#define MAX_ATTRACTION			10.0f
#define MIN_ATTRACTION			3.0f

#define SECTOR_COUNT			1		// currently there is a bug at sector borders


struct MainPoint {
	MainPoint() {}
	MainPoint( Vec3f _pos, MSA::Physics::World3D * _physics, int _pID ) {
        
        mParticleID = _pID;
        mPhysic = _physics;
        
        mMass	= 8.0f;
		mBounce	= 0.5f;
		mRadius	= 20.0f;
        
        mEndOfLine     = true;
        
        mTarget        = _pos;
        mTargetTime    = 0;
        mMoveBack      = false;
        mActive        = false;
                
        Physics::Particle3D* p = mPhysic->makeParticle(_pos);
        p->setMass(mMass)->setBounce(mBounce)->setRadius(mRadius)->enableCollision()->makeFree();
    }
    
    void setNeighbours( MainPoint* _neighbour ) {
        mNeighbours.push_back( _neighbour );
                
        Physics::Particle3D *a = getParticle();
		Physics::Particle3D *b = _neighbour->getParticle();
        mPhysic->makeSpring(a, b, SPRING_STRENGTH, a->getPosition().distance(b->getPosition()));

    }
    
    void postSettings() {
        if( mNeighbours.size() == 0 ) { mEndOfLine = true; }
        else if( mNeighbours.size() == 1 && mParticleID == 0 ) { 
            mEndOfLine = true; 
            
            mMass      = 6.0f;
            Physics::Particle3D *p = getParticle();
            p->setMass( mMass );
            
        }
        else { 
            mEndOfLine = false; 
            
            mMass      = 120.0f;
            Physics::Particle3D *p = getParticle();
            p->setMass( mMass );
            p->isFixed();
        }
    }
    
    Vec3f getPosition() {
        Physics::Particle3D* p = getParticle();
        return p->getPosition();
    }
    
    Physics::Particle3D* getParticle() {
        Physics::Particle3D* p = mPhysic->getParticle(mParticleID);
        return p;
    }
    
    void moveTo( Vec2f _target ) { 
        float z = getPosition().z;
        moveTo( Vec3f( _target.x, _target.y, z ) );
    };
    
    //Bewegungsberechnung
    void moveTo( Vec3f _target ) { 
        mActive = true;
        mTarget = _target;
        mTargetTime = 0;
    };

    void moveTo( Vec3f _target, time_t _ms, bool _back = false) { 
        
        mActive = true;
        mMoveBack = false;
        
        if( _back ) {
            mMoveBack = true;
            mOldPosition = mTarget;
        }
        
        mTarget = _target;
        
        mTargetTime = _ms;
        mSetTime    = niko::getTimeMS();
    };
    
    //Direkt zum Punkt
    void forceTo( Vec3f _target ) { 
        Physics::Particle3D* p = getParticle();
        mTarget = _target;
        p->moveTo(_target);
        
        mActive = false;
        
    };
    
    void moveBy( Vec3f _dir ) {
        Physics::Particle3D* p = getParticle();
        p->moveBy( _dir ); 
    }
    
    bool getActive() { return mActive; }
    
    void update() {
    
        //cout << "update: " << mTarget << endl;
        
        if( !mActive ) {
            return;
        } 
        
        
        Physics::Particle3D* p = getParticle();
        Vec3f pPosition = p->getPosition();
        
        if( mTargetTime == 0) {
            if( mTarget.distance(pPosition) < 1 ) {
                forceTo(mTarget);
            }
            else {
                Vec3f newPosition = pPosition;
                newPosition += (mTarget - pPosition) * 0.20;
                p->moveTo(newPosition);
            }
        } else {
            
            time_t timeDelta = mTargetTime - niko::getTimeMS();
            
            if( timeDelta <= 0 ) { 
                
                forceTo(mTarget);
                mTargetTime = 0;
                
                if(mMoveBack) {
                    mMoveBack = false;
                    moveTo(mOldPosition);
                }
                
            }
            else {
                
                float t = niko::mapping( timeDelta, 0, mTargetTime - mSetTime, 0, 1, true);
               // float distLength = 1 - ci::easeInQuad( t ); 
                //float distLength = 1 - ci::easeInSine( t );
                float distLength = 1 - ci::easeNone( t );
                
                
                Vec3f newPosition = pPosition;
                newPosition += (mTarget - pPosition) * distLength;
                
                //EaseInExpo();
                
                /*
                //float time = math<float>::clamp( fmod( getElapsedSeconds() * TWEEN_SPEED, 1 ) * 1.5f, 0, 1 );
                
                //Get Frames pro MS
                float fpms = ci::app::getFrameRate() / 1000;
                
                //check Frame in timeDelta
                float fptd = fpms * timeDelta;
                
                //check strecke pro Frame
                Vec3f newPosition = pPosition;
                newPosition += (mTarget - pPosition) / fptd;
                
                 */
                 
                //set new Pos
                p->moveTo(newPosition);
            }
        }
    }
    
    void draw() { 
        Physics::Particle3D* p = getParticle();
        gl::drawSphere (p->getPosition(), 4.0f);
        
        /* DEBUG
        if(mEndOfLine) {
        gl::drawString( toString(mParticleID), Vec2f(p->getPosition().x, p->getPosition().y), ColorA(1,1,1,1));
        }
 
        if(!mActive) {
            gl::color(255, 0, 0);
            gl::drawSphere(Vec3f(0,0,0), 500);
        }
         */
        
    }
      
    float   mRadius, mMass, mBounce;
    bool    mEndOfLine;
    Vec3f   mTarget, mOldPosition;
    time_t  mTargetTime;  // in ms
    time_t  mSetTime;
    bool    mMoveBack;
    bool    mActive;
    
    
    std::vector<  MainPoint* > mNeighbours;
    
    //Physic
    int mParticleID;
    Physics::World3D* mPhysic;

    //std::map< MainPoint*, float > mNeighbours;

};


class Character {
 public:
	Character();
    Character(ci::Vec3f _pos, float _radius);
    
    void draw();
    void update();
    void mkPoint(MainPoint *lastPoint);
    void createNewStructure(int _num);
    void createPhysics();
    void setRadius(float _r);
    void scale(float _s);
    int  getRandPointNumber();
    void addRandomForce(float _f);
    void moveTo(Vec2f _mousePos);
    void dance();
    //RENAME
    void test();
    void setNextBeat( time_t _bang );
   
    
    ci::Vec3f	mCenterPos;
    float       mRadius;
    int         mNumberOfMainPoints;
    int         mMainPointsLeft;
    bool        mOpenLines;
    time_t      mNextBeat;
    int         counter;
    
    std::vector<MainPoint>  mMainPoints;
    Perlin      mPerlin;
    
    
    //Physics
    Physics::World3D        mPhysics;
    int                     mForceTimer;
    
    
    //
    gl::Texture         ballImage;
    
};