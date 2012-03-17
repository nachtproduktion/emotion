#pragma once
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"

#include <vector>

class Particle {
 public:
	Particle();
	Particle( ci::Vec3f _pos, ci::Vec3f _vel );
	void update();
	void draw();
	
	ci::Vec3f	mPos;
    ci::Vec3f	mVel;
	
	float		mRadius;
	ci::Color	mColor;
};