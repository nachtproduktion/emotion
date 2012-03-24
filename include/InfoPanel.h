
#pragma once

#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class InfoPanel {
 public:
	InfoPanel();
	void createTexture( ci::Vec2f _position );
	void update();
	void render();
	void toggleState();
	
	ci::Vec2f			mLoc;
	
	float				mOpacity;
	float				mCounter;
	bool				mState;
	ci::gl::Texture	mTexture;
};
