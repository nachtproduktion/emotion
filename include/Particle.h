#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"

#include <vector>

class Particle {
 public:
	Particle( ci::Vec3f _pos, ci::Vec3f _vel );
    void findPerlin();
	void findVelocity();
    void setPosition();
    void update();
	void render();
    
	
	ci::Vec3f	mPosition;
    ci::Vec3f	mVelocity;
    ci::Vec3f   mPerlin;
	
	float		mRadius;
    ci::ColorA  mColor;
    
	float		mAge;
	float		mLifeSpan;
	
	bool		mIsDead;
    
    
};