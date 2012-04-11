#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"

#include <vector>

#include "cinder/Matrix.h"

class Particle {
 public:
    
	Particle( ci::Vec3f _pos, ci::Vec3f _vel );
    Particle( float _radius, ci::Vec3f _pos, float _theta, float _u );
    Particle( ci::Matrix44f* _matrix, float _radius, bool _statical = false );
        
    void setVelocity( ci::Vec3f _mpoint );
    
    void calcPosition();
    
    void findPerlin();
	void findVelocity();
    
    void falldown();
    
    void updatePosition( ci::Vec3f _pos );
    void update();
    
	void render();
    
	
    ci::Vec3f	mVelocity;
    ci::Vec3f   mPerlin;
	
	float		mRadius;
    ci::ColorA  mColor;
    
	float		mAge;
	float		mLifeSpan;
	
	bool		mIsDead;
    bool        mSphere;
    bool        mCircle;
    
    float theta, u;
    float vTheta, vU;
    float radius;
    
private:
    
    ci::Vec3f                   mPosition;
    ci::Vec3f                   mPositionOnCircle;
    float                       mCircleRadius;	
    ci::Matrix44f*              mControllerMatrix;
    
    bool                        mStatical;
    bool                        mFallDown;
    

    
};