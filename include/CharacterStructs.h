#ifndef emotion_CharacterStructs_h
#define emotion_CharacterStructs_h

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"

#include <vector>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "niko_functionen.h"
#include "CharacterPoint.h"

#define	SPRING_STRENGTH         0.01f


struct Bond {
    Bond( CharacterPoint * _particleA, CharacterPoint * _particleB ) {
        
        a = _particleA->getParticle();
        b = _particleB->getParticle();
        s = NULL;
        
        mStrength = SPRING_STRENGTH;
        mSaveDistanceA = a->getPosition().distance(b->getPosition());
        mSaveDistanceB = mSaveDistanceA;
    }
    
    void makeBond( MSA::Physics::World3D * _physics ) {
        s = _physics->makeSpring(a, b, mStrength, mSaveDistanceA);
    }
    
    void killBond( MSA::Physics::World3D * _physics ) {
        if(s) s->kill();
    }
    
    void setBondLength( float _len ) {
        if(s) s->setRestLength(_len);
    }
    
    float getBondLength() {
        if(s) return s->getRestLength();
        else  return 0.0f;
    }
    
    void turnOff() {
        if(s) s->turnOff();
    }
    
    void turnOn() {
        if(s) s->turnOn();
    }
    
    void render() {
        gl::color(1,0.5,0);
        gl::drawLine(a->getPosition(), b->getPosition());
    }
    
    float mStrength;
    float mSaveDistanceA;
    float mSaveDistanceB;
    MSA::Physics::Particle3D * a;
    MSA::Physics::Particle3D * b;
    MSA::Physics::Spring3D *s;
    
};

//struct MainPoint {
//	MainPoint() {}
//	MainPoint( Vec3f _pos, MSA::Physics::World3D * _physics, int _pID ) {
//        
//        mParticleID = _pID;
//        mPhysic = _physics;
//        
//        mMass	= 4.0f;
//		mBounce	= 0.2f;
//		mRadius	= 5.0f;
//        
//        mEndOfLine     = true;
//        
//        mTarget        = _pos;
//        mTargetTime    = 0;
//        mMoveBack      = false;
//        mActive        = false;
//        
//        mParticleControllerID = -1;
//                
//        Physics::Particle3D* p = mPhysic->makeParticle(_pos);        
//        p->setMass(mMass)->setBounce(mBounce)->setRadius(mRadius)->enableCollision()->makeFree();
//    }
//    
//    void setNeighbours( MainPoint* _neighbour ) {
//        mNeighbours.push_back( _neighbour );
//                
//        Physics::Particle3D *a = getParticle();
//		Physics::Particle3D *b = _neighbour->getParticle();
//        mPhysic->makeSpring(a, b, SPRING_STRENGTH, a->getPosition().distance(b->getPosition()));
//
//    }
//    
//    void setParticleControllerID( int _id ) {
//        mParticleControllerID = _id;
//    }
//    
//    int getParticleControllerID() {
//        return mParticleControllerID;
//    }
//    
//    void setRadius( float _r ) {
//        mRadius      = _r;
//        Physics::Particle3D *p = getParticle();
//        p->setRadius( mRadius );
//        
//    }
//    
//    void setMass( float _m ) {
//        mMass      = _m;
//        Physics::Particle3D *p = getParticle();
//        p->setMass( mMass );
//        
//    }
//    
//    void setFixed() {
//        Physics::Particle3D *p = getParticle();
//        p->makeFixed();
//    } 
//    
//    bool getFixed() {
//        Physics::Particle3D *p = getParticle();
//        return p->isFixed();
//    } 
//    
//    void setFree() {
//        Physics::Particle3D *p = getParticle();
//        p->makeFree();
//    } 
//    
//    void postSettings() {
//        if( mNeighbours.size() == 1 ) { 
//            mEndOfLine = true;
//            setMass(4.0f);
//        }
//        else {
//            mEndOfLine = false; 
//            
//            if(getFixed()) { //Startpunkt
//                setMass(150.0f);
//                setRadius(10.0f);
//                setFree();
//            }
//            else { 
//                setMass(10.0f);
//                setRadius(7.0f);
//            }
//            
//        }        
//    }
//    
//    Vec3f getPosition() {
//        Physics::Particle3D* p = getParticle();
//        return p->getPosition();
//    }
//    
//    Physics::Particle3D* getParticle() {
//        Physics::Particle3D* p = mPhysic->getParticle(mParticleID);
//        return p;
//    }
//    
//    void moveTo( Vec2f _target ) { 
//        float z = getPosition().z;
//        moveTo( Vec3f( _target.x, _target.y, z ) );
//    };
//    
//    ////////////////////////////////////////////////
//    //Bewegungsberechnung //////////////////////////
//    ////////////////////////////////////////////////
//    
//    void moveTo( Vec3f _target ) { 
//        mActive = true;
//        mTarget = _target;
//        mTargetTime = 0;
//    };
//
//    void moveTo( Vec3f _target, time_t _ms, bool _back = false) { 
//        
//        mActive = true;
//        mMoveBack = false;
//        
//        if( _back ) {
//            mMoveBack = true;
//            mOldPosition = mTarget;
//        }
//        
//        mTarget = _target;
//        
//        mTargetTime = _ms;
//        mSetTime    = niko::getTimeMS();
//    };
//    
//    //Direkt zum Punkt
//    void forceTo( Vec3f _target ) { 
//        Physics::Particle3D* p = getParticle();
//        mTarget = _target;
//        p->moveTo(_target);
//        
//        mActive = false;
//        
//    };
//    
//    void moveBy( Vec3f _dir ) {
//        Physics::Particle3D* p = getParticle();
//        p->moveBy( _dir ); 
//    }
//    
//    void moveOnSphere( Vec3f _target, time_t _ms = 0 ) {
//        
//        static bool active          = false;
//        static time_t targetTime    = 0;
//        static time_t startTime     = 0;
//        static Vec3f targetPosition = ci::Vec3f::zero();
//        
//        if( !active ) {
//            if( _ms == 0 ) { targetTime = niko::getTimeMS() + 500; }
//            else { targetTime = _ms; }
//            
//            startTime      = niko::getTimeMS();
//            targetPosition = _target;
//            
//            active = true;
//        }
//        
//        time_t timeDelta = targetTime - niko::getTimeMS();
//        
//        float t = niko::mapping( timeDelta, 0, targetTime - startTime, 0, 1, true);
//        //float distLength = 1 - ci::easeInQuad( t ); 
//        //float distLength = 1 - ci::easeInSine( t );
//        float distLength = 1 - ci::easeNone( t );  
//        
//        
//        
//    }
//    
//    void doMoveOnSphere() {
//        
//    }
//    
//    
//    
//    
//    bool getActive() { return mActive; }
//    
//    void update() {
//    
//        //cout << "update: " << mTarget << endl;
//        
//        if( !mActive ) {
//            return;
//        } 
//        
//        
//        Physics::Particle3D* p = getParticle();
//        Vec3f pPosition = p->getPosition();
//        
//        if( mTargetTime == 0) {
//            if( mTarget.distance(pPosition) < 1 ) {
//                forceTo(mTarget);
//            }
//            else {
//                Vec3f newPosition = pPosition;
//                newPosition += (mTarget - pPosition) * 0.20;
//                p->moveTo(newPosition);
//            }
//        } else {
//            
//            time_t timeDelta = mTargetTime - niko::getTimeMS();
//            
//            if( timeDelta <= 0 ) { 
//                
//                forceTo(mTarget);
//                mTargetTime = 0;
//                
//                if(mMoveBack) {
//                    mMoveBack = false;
//                    moveTo(mOldPosition);
//                }
//                
//            }
//            else {
//                
//                float t = niko::mapping( timeDelta, 0, mTargetTime - mSetTime, 0, 1, true);
//               // float distLength = 1 - ci::easeInQuad( t ); 
//                //float distLength = 1 - ci::easeInSine( t );
//                float distLength = 1 - ci::easeNone( t );
//                
//                
//                Vec3f newPosition = pPosition;
//                newPosition += (mTarget - pPosition) * distLength;
//                
//                //EaseInExpo();
//                
//                
//                //float time = math<float>::clamp( fmod( getElapsedSeconds() * TWEEN_SPEED, 1 ) * 1.5f, 0, 1 );
//                 /*
//
//                //Get Frames pro MS
//                float fpms = fps / 1000;
//                
//                //check Frame in timeDelta
//                float fptd = fpms * timeDelta;
//                
//                //check strecke pro Frame
//                Vec3f newPosition = pPosition;
//                newPosition += (mTarget - pPosition) / fptd;
//                
//                 */
//                //set new Pos
//                p->moveTo(newPosition);
//            }
//        }
//    }
//    
//    void draw() { 
//        Physics::Particle3D* p = getParticle();
//        gl::drawSphere (p->getPosition(), mRadius);
//        
//        /* DEBUG
//        if(mEndOfLine) {
//        gl::drawString( toString(mParticleID), Vec2f(p->getPosition().x, p->getPosition().y), ColorA(1,1,1,1));
//        }
// 
//        if(!mActive) {
//            gl::color(255, 0, 0);
//            gl::drawSphere(Vec3f(0,0,0), 500);
//        }
//         */
//        
//    }
//      
//    float   mRadius, mMass, mBounce;
//    bool    mEndOfLine;
//    Vec3f   mTarget, mOldPosition;
//    time_t  mTargetTime;  // in ms
//    time_t  mSetTime;
//    bool    mMoveBack;
//    bool    mActive;
//    int     mParticleControllerID;
//    
//    
//    std::vector<  MainPoint* > mNeighbours;
//    
//    //Physic
//    int mParticleID;
//    Physics::World3D* mPhysic;
//
//    //std::map< MainPoint*, float > mNeighbours;
//
//};




#endif