#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Perlin.h"
#include "niko_functionen.h"

using namespace ci;

Perlin sPerlin( 2 );

Particle::Particle( ci::Vec3f _pos, ci::Vec3f _vel)
{
	
    mPosition       = _pos + Rand::randVec3f();// * Rand::randFloat( 20.0f );
    mVelocity       = _vel * 0.01 + Rand::randVec3f() * 0.02;
    
    mPerlin         = Vec3f::zero();
	
	mRadius			= Rand::randFloat( 0.5f, 1.0f );
    	
	mIsDead			= false;
	mAge			= 0.0f;
	mLifeSpan		= Rand::randFloat( 80.0f, 100.0f );
	
    mColor  = ColorA(1.0f,1.0f,1.0f,1.0f);
    
}

void Particle::findPerlin()
{
    int counter = ci::app::getElapsedFrames();
	Vec3f noise = sPerlin.dfBm( mPosition * 0.01f + ci::Vec3f( 0, 0, counter / 100.0f ) );
	mPerlin = noise.normalized() * 0.005f;
}

void Particle::findVelocity()
{

	mVelocity += mPerlin;
    
}

void Particle::setPosition() {
    mPosition += mVelocity * 0.5;
}
	
void Particle::update()
{
    /*
    static ci::Vec3f oldPosition = mPosition;
    
    //update Velocity
    mVelocity = mPosition - oldPosition;
    oldPosition = mPosition;
     */
    
    findPerlin();
    findVelocity();
    setPosition();
    
    //AGING
	mAge ++;
    mColor.a = niko::mapping(mAge, 0, mLifeSpan, 1.0, 0.0, true);
    
	if( mAge > mLifeSpan ){
		mIsDead = true;
	}
    
}

void Particle::render()
{
	gl::color( mColor );
	gl::drawSphere( mPosition, mRadius );
}
