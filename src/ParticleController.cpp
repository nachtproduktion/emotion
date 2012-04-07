#include "ParticleController.h"
#include <iostream>
#include "cinder/CinderMath.h"

ParticleController::ParticleController() {

    mMatrix   = ci::Matrix44f::zero();

}


void ParticleController::setCircleRadius( float _radius ) {
    
    mCircleRadius = _radius;
    
}

void ParticleController::addParticles( int _number, bool _statical ) {
    
    for(int i = 0; i < _number; i++) {             
            mParticles.push_back( Particle( &mMatrix, mCircleRadius, _statical ) );
    }     
    
}

void ParticleController::updateMatrix( ci::Matrix44f _matrix ) {
    
    mMatrix = _matrix;
    
}

void ParticleController::update( ci::Vec3f _mpoint ) {
    
    
    if(mParticles.size() == 0 ) addParticles( mCircleRadius*2, true );
    
    //addParticles( Rand::randInt(2) );
    
    for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ){
        
        if( ! p->mIsDead ) {
            p->setVelocity( _mpoint );
			p->update();
			++p;
		}
		else {
			p = mParticles.erase( p );
		}
        
	}
    
}

void ParticleController::render() {
    
    glColor4f( 1, 1, 1, 1 );
        
    for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->render();
	}
    
}

/*
void ParticleController::addParticles( int _number ) {
    
    if(mSphereRadius) {
        for(int i = 0; i < _number; i++) {
            float theta = Rand::randFloat(0,2*M_PI);
            float u = Rand::randFloat(-1,1);
            mParticles.push_back( Particle( mSphereRadius, mPosition, theta, u ) );
        }
    } else if(mCircleRadius) {
        for(int i = 0; i < _number; i++) {             
            mParticles.push_back( Particle( mPosition, mCircleRadius ) );
        }        
    } else {
        // PARTICLES
        for( int i = 0; i < _number; i++ ) {
            mParticles.push_back( Particle( mPosition, mVelocity ) );
        }
    }
    
}

void ParticleController::setSphere( float _radius ) {
    mSphereRadius = _radius;
    addParticles( _radius*_radius ); 
}

void ParticleController::setCircle( ci::Vec3f _pos, float _radius ) {
    
    mCircleRadius   = _radius;
    mPosition       = _pos;
    
    int particleNum = _radius;
    
    addParticles( particleNum ); 
}

void ParticleController::setTarget( ci::Vec3f _target ) {
    mTarget = _target;
}

void ParticleController::calcVelocity() {
    
    mVelocity += ( mTarget - mPosition - mVelocity ) * 0.25f;
    
}

void ParticleController::update( ci::Vec3f _pos ) {
    
    mPosition   = _pos;
    
    for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ){
       p->updatePosition( mPosition );        
	}
    
}


void ParticleController::update() {
    
    calcVelocity();
    
    //Set Position
    mPosition += mVelocity;
    
    if( !mSphereRadius && !mCircleRadius ) { addParticles(1); }
    
    for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ){
        
        if( ! p->mIsDead ) {
            //p->updatePosition( mPosition );
			p->update();
			++p;
		}
		else {
			p = mParticles.erase( p );
		}
        
	}
    
}

void ParticleController::draw() {
    
    glColor4f( 1, 1, 1, 1 );
    
    for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->render();
	}
    
}

*/