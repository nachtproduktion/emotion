#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;

Particle::Particle()
{
}

Particle::Particle( ci::Vec3f _pos, ci::Vec3f _vel )
{
	mPos	= _pos;
	mVel	= _vel;
	mRadius	= 4.0f;
    mColor  = Color(255,255,255);
}	
	
void Particle::update()
{
    
    mVel = Rand::randVec3f();    
	mPos += mVel;
    
}

void Particle::draw()
{
	gl::color( mColor );
	gl::drawSolidCircle( mPos.xy(), mRadius*mPos.z/500 );
}
