#ifndef emotion_CharacterStructs_h
#define emotion_CharacterStructs_h

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"

#include <vector>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "niko_functionen.h"
#include "CharacterPoint.h"

#include "Constants.h"


///////////////////////////////////////////////////////////////
//////////// BOND ///////////
///////////////////////////////////////////////////////////////
struct Bond {

    Bond() {
        a = NULL;
        b = NULL;
        s = NULL;
    }
    Bond( CharacterPoint * _particleA, CharacterPoint * _particleB ) {
        
        a = _particleA->getParticle();
        b = _particleB->getParticle();
        s = NULL;
        
        init();
    }
    
    Bond( MSA::Physics::Particle3D * _particleA, MSA::Physics::Particle3D * _particleB ) {
        
        a = _particleA;
        b = _particleB;
        s = NULL;
        
        init();
    }
    
    void init() {
        
        //Level 1 = Außen
        //Level 2 = Innen
        //...
        
        mLevel = 2;
        
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
        if( s->isOff() ) { return; }
        
        gl::drawLine(a->getPosition(), b->getPosition());
    }
    
    int   mLevel;
    float mStrength;
    float mSaveDistanceA;
    float mSaveDistanceB;
    float mSaveMaxLength;

    Anim<float> mBondLength;
    
    MSA::Physics::Particle3D * a;
    MSA::Physics::Particle3D * b;
    MSA::Physics::Spring3D *s;
    
};


#endif