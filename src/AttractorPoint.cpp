#include "AttractorPoint.h"

extern float testwert;

AttractorPoint::AttractorPoint() {
    
    mRadius     = 5.0f;
    mMass       = 1.0f;
    
    mPosition   = ci::Vec3f::zero();
    
    mPhysics    = NULL;
    mAttractor  = NULL;
    
}

void AttractorPoint::create( MSA::Physics::World3D * _physics, ci::Vec3f _pos ) {
    mPosition = _pos;
    mPhysics  = _physics;
}

void AttractorPoint::activate() {
    
    mAttractor = mPhysics->makeParticle( mPosition );
    mAttractor->setRadius( mRadius )->setMass( mMass )->setBounce( 0.0f )->makeOutside( true )->makeFixed();
}

void AttractorPoint::deactivate() {
    if( mAttractor ) {
        mAttractor->kill();
        mAttractor = NULL;
    } 
}

void AttractorPoint::update() {
    if( mAttractor == NULL ) { return; }
    
    //mMass = testwert;
    
    if(mAttractor->getRadius() != mRadius) { mAttractor->setRadius( mRadius ); }
    if(mAttractor->getMass() != mMass) { mAttractor->setMass( mMass ); }
    if(mAttractor->getPosition() != mPosition) { mAttractor->moveTo( mPosition ); }
    
}

void AttractorPoint::render() {
    
    if( mAttractor == NULL ) { return; }
       
    ci::gl::color(1,1,1);
    gl::drawSphere(mAttractor->getPosition(), mAttractor->getRadius());
    
}