//
//  EmoAttractor.cpp
//  emotion
//

//  Created by Nikolas on 15.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.

//

#include "EmoAttractor.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

#include "niko_functionen.h"

#define EXTRA 30.0f

EmoAttractor::EmoAttractor() {
    
    mARadius    = 5.0f;
    mAMass      = 1.0f;
    mLevel      = 0.0f;
    
    mExists      = false;
    
    for( int i = 0; i<4; i++) {
        mAPositions[i] = ci::Vec3f::zero();
    }
    

}

void EmoAttractor::create( MSA::Physics::World3D * _physics, float _worldRadius, ci::Vec3f _worldCenter ) {
    
    mPhysics = _physics;
    mWorldRadius = _worldRadius;
    mWorldCenter = _worldCenter;
    
    calcStartPositions();    
    makeAttractors();
    
    mExists = true;
    
    // cout << "EmoAttractor erstellt " << endl;
    
}

void EmoAttractor::setFrustration() {
    mTyp = 1;
}

void EmoAttractor::setEngagement() {
    mTyp = 2;
}

void EmoAttractor::calcStartPositions() {
    
    ci::Vec3f vec = ci::Vec3f( mWorldRadius + EXTRA, 0.0f, 0.0f ) + mWorldCenter;

    
    for( int i = 0; i<4; i++) {
        mAPositions[i] = vec;
        vec -= mWorldCenter;
        vec.rotateY(ci::toRadians(90.0f));
        vec += mWorldCenter;
    }
    
}

void EmoAttractor::makeAttractors() {

    for( int i = 0; i<4; i++) {
        mAttractor[i] = mPhysics->makeParticle(mAPositions[i]);
        mAttractor[i]->setRadius(mARadius)->setMass(mAMass)->setBounce(0.0f)->makeOutside(true)->makeFixed();
    }

}

void EmoAttractor::update( float _level, float _min, float _max ) {
    if(!mExists) return;
    
    mLevel = niko::mapping( _level, _min, _max, 0.0f, PI*0.5f);
    movePositions();
    setPositions();
    
}

void EmoAttractor::movePositions() {
    
    float x = (mWorldRadius + EXTRA) * cos(mLevel);
    float y = (mWorldRadius + EXTRA) * sin(mLevel);
    
    //Frustratrion
    if(mTyp == 1) {
        x *= -1;
        y *= -1;
    }
    
    ci::Vec3f vec;
    
    vec = mAttractor[0]->getPosition();
    vec.x = x + mWorldCenter.x;
    vec.y = y + mWorldCenter.y;
    mAPositions[0] = vec;
    
    vec = mAttractor[1]->getPosition();
    vec.z = x + mWorldCenter.x;
    vec.y = y + mWorldCenter.y;
    mAPositions[1] = vec;
    
    vec = mAttractor[2]->getPosition();
    vec.x = x + mWorldCenter.x;
    vec.x *= -1;
    vec.y = y + mWorldCenter.y;
    mAPositions[2] = vec;
    
    vec = mAttractor[3]->getPosition();
    vec.z = x + mWorldCenter.x;
    vec.z *= -1;
    vec.y = y + mWorldCenter.y;
    mAPositions[3] = vec;
    
}

void EmoAttractor::changeWorld( ci::Vec3f _wCenter, float _wRadius ) {
    mWorldCenter = _wCenter;
    mWorldRadius = _wRadius;
    
    calcStartPositions();
    movePositions();
    setPositions();
}

void EmoAttractor::setPositions() {
    for( int i = 0; i<4; i++) {
        mAttractor[i]->moveTo(mAPositions[i]);
    }
}

void EmoAttractor::render() {
    if(!mExists) return;
    
    ci::gl::color(1,1,1);
     
    for( int i = 0; i<4; i++) {
        ci::gl::drawSphere(mAttractor[i]->getPosition(), mAttractor[i]->getRadius());
    }
}