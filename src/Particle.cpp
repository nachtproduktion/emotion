#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/Perlin.h"
#include "niko_functionen.h"
#include "cinder/CinderMath.h"

using namespace ci;

extern ci::Vec3f bup;
extern ci::Vec3f br;

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
    
    mSphere = false;
    mCircle = false;
}

Particle::Particle( float _radius, ci::Vec3f _pos, float _theta, float _u ) {
    mSphere = true;
    
    radius = _radius;
    theta = _theta;
    u = _u;
    vTheta = 0;
    vU = 0;
    
    mRadius             = 1.0f; //Rand::randFloat( 0.5f, 1.0f );
    
    radius              = Rand::randFloat(_radius*0.9f, _radius);
    
    ci::Vec3f randomPos = Rand::randVec3f() * radius;
    
    mPosition           = _pos;// + randomPos;
    
    mColor              = ColorA(1.0f,1.0f,1.0f,1.0f);
    
    mIsDead             = false;
    
}

Particle::Particle( ci::Matrix44f* _matrix, float _radius ) {
    
    mControllerMatrix   = _matrix;
    mCircleRadius       = _radius;
    
    float randAngle     = Rand::randFloat(0.0,6.28318531f);
    mPositionOnCircle   = ci::Vec3f( cos( randAngle )*mCircleRadius, sin( randAngle )*mCircleRadius, 0 );
    mPositionOnCircle.z = Rand::randFloat( -5.0f, 5.0f );

    calcPosition();
    
    mPerlin         = Vec3f::zero();
	
	mRadius			= Rand::randFloat( 0.5f, 1.0f );
    
	mIsDead			= false;
	mAge			= 0.0f;
	mLifeSpan		= Rand::randFloat( 80.0f, 100.0f );
	
    mColor          = ColorA(1.0f,1.0f,1.0f,1.0f);
    
}

void Particle::calcPosition() {
    
    if(mControllerMatrix == NULL) return;

    //float r = sin( (float)(i + 0)/(float)(mPs.size() - 1)*3.141592f );
    //float rs = (2 - 1)*r + 1;
    
    ci::Matrix44f mat = *mControllerMatrix;
    mPosition = mat * mPositionOnCircle;

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
    
    calcPosition();
    
    
    
    //AGING
	mAge ++;
    mColor.a = niko::mapping(mAge, 0, mLifeSpan, 1.0, 0.0, true);
    
	if( mAge > mLifeSpan ){
		mIsDead = true;
	}
    
    /*
    if(mSphere) {
        
        vTheta += 0;//Rand::randFloat(-0.01,0.01);
        theta += vTheta;
        
        if (theta<0||theta>2*M_PI) {
            vTheta*=-1;
        }
        
        vU += 0; //Rand::randFloat(-0.01,0.01);
        u += vU;
        
        if (u<-1||u>1) vU*=-1;
        
        vU *= 0.95;
        vTheta *= 0.95;
        
        mPosition.x += radius*cos(theta)*sqrt(1-(u*u));
        mPosition.y += radius*sin(theta)*sqrt(1-(u*u));
        mPosition.z += radius*u;
        
        return;
    }
    
    if(mCircle) {
                
        //calcPosition();
        return;
    }
    
    
    
    findPerlin();
    findVelocity();
    setPosition();
    
    */
}

void Particle::render()
{
    gl::color( mColor );

    gl::drawBillboard(mPosition, Vec2f(mRadius,mRadius), 0.0, br, bup);
    
    /*
    glTexCoord2f( 0, 0 );
	glVertex3f( mPosition.x - mRadius, mPosition.y - mRadius, mPosition.z );
	
	glTexCoord2f( 1, 0 );
	glVertex3f( mPosition.x + mRadius, mPosition.y - mRadius, mPosition.z );
	
	glTexCoord2f( 1, 1 );
	glVertex3f( mPosition.x + mRadius, mPosition.y + mRadius, mPosition.z );
	
	glTexCoord2f( 0, 1 );
	glVertex3f( mPosition.x - mRadius, mPosition.y + mRadius, mPosition.z );
   */
	//gl::drawSphere( mPosition, mRadius );
}
