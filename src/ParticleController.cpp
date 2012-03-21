#include "ParticleController.h"
#include <iostream>

ParticleController::ParticleController() {
    mPosition = ci::Vec3f::zero();
    mVelocity = ci::Vec3f::zero();
}

void ParticleController::addParticles( int _number ) {
    
    // PARTICLES
	for( int i = 0; i < _number; i++ ) {
		mParticles.push_back( Particle( mPosition, mVelocity ) );
	}
    
}

void ParticleController::setTarget( ci::Vec3d _target ) {
    mTarget = _target;
}

void ParticleController::calcVelocity() {
    
    mVelocity += ( mTarget - mPosition - mVelocity ) * 0.25f;
    
}

void ParticleController::update() {
    
    calcVelocity();
    
    //Set Position
    mPosition += mVelocity;
    
    addParticles(1);
    
    for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ){
        
        if( ! p->mIsDead ) {
			p->update();
			++p;
		}
		else {
			p = mParticles.erase( p );
		}
        
	}
    
}

void ParticleController::draw() {
    
    ///////////
    for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->render();
	}
    
}