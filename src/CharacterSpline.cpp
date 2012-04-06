//
//  CharacterSpline.cpp
//  emotion
//
//  Created by Nikolas on 05.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "CharacterSpline.h"
#include "cinder/gl/gl.h"

CharacterSpline::CharacterSpline( std::vector<ci::Vec3f> _pointList ) {
    
    //Create Spline:
    mSpline = BSpline3f(_pointList, _pointList.size()-1, false, true);
    
    mNumSegs    = 20;
    
    //Create ParticleController
    createParticleController();
    
    //Circle profile - radius - segments
//    makeCircleProfile( mProf, 5.0f, 16 );
    
    //SetParticleController Profile
    for( int i = 0; i < mNumSegs; ++i ) {	
        mParticleController[i].setCircleRadius( 5.0f );          
    }    
    
}


void CharacterSpline::createParticleController() {
    
    mParticleController.clear();
    
    for( int i = 0; i < mNumSegs; ++i ) {	
        mParticleController.push_back( ParticleController() );            
    }
    
}

//void CharacterSpline::makeCircleProfile( std::vector<ci::Vec3f>& prof, float rad, int segments )
//{
//	prof.clear();
//	float dt = 6.28318531f/(float)segments;
//	for( int i = 0; i < segments; ++i ) {
//		float t = i*dt;
//		prof.push_back( ci::Vec3f( cos( t )*rad, sin( t )*rad, 0 ) );
//	}
//    
//}

void CharacterSpline::buildVectors() {
   
	mPs.clear();
	mTs.clear();
    
	float dt = 1.0f/(float)mNumSegs;
	for( int i = 0; i < mNumSegs; ++i ) {
		float t = i*dt;
        
        ci::Vec3f P = mSpline.getPosition( t );
		mPs.push_back( P );
		
		ci::Vec3f T = mSpline.getDerivative( t );
		mTs.push_back( T.normalized() );
	}
}


void CharacterSpline::buildPTF() {
	mFrames.clear();
    
	int n = mPs.size();
	// Make sure we have at least 3 points because the first frame requires it
	if( n >= 3 ) {
		mFrames.resize( n );
		// Make the parallel transport frame
		mFrames[0] = firstFrame( mPs[0], mPs[1],  mPs[2] );
		// Make the remaining frames - saving the last
		for( int i = 1; i < n - 1; ++i ) {
			ci::Vec3f prevT = mTs[i - 1];
			ci::Vec3f curT  = mTs[i];
			mFrames[i] = nextFrame( mFrames[i - 1], mPs[i - 1], mPs[i], prevT, curT );
		}
		// Make the last frame
		mFrames[n - 1] = lastFrame( mFrames[n - 2], mPs[n - 2], mPs[n - 1] );
	}
}

void CharacterSpline::update( std::vector<ci::Vec3f> _pointList ) { 
    
    mSpline = BSpline3f(_pointList, _pointList.size()-1, false, true);
    
    buildVectors();
    buildPTF();
    
    updateParticle();
    
}	


void CharacterSpline::updateParticle() {

    for( int i = 0; i < mNumSegs; ++i ) {
        mParticleController[i].updateMatrix( mFrames[i] );
        mParticleController[i].update();
    }

}

void CharacterSpline::drawParticle() {
    
    for( int i = 0; i < mNumSegs; ++i ) {
        mParticleController[i].render();
    }
    
}

/*
void CharacterSpline::drawParticle() {

    if( ( mPs.size() != mFrames.size() ) || mFrames.size() < 3 || mProf.empty() ) 
		return;
    
    
	for( int i = 0; i < mPs.size() - 1; ++i ) {
		ci::Matrix44f mat = mFrames[i];
        
		float r = sin( (float)(i + 0)/(float)(mPs.size() - 1)*3.141592f );
		float rs = (2 - 1)*r + 1;
        
		for( int ci = 0; ci < mProf.size(); ++ci ) {
			int idx0 = ci;
			int idx1 = (ci == (mProf.size() - 1)) ? 0 : ci + 1;
			ci::Vec3f P0 = mat*(mProf[idx0]*rs);
			ci::Vec3f P1 = mat*(mProf[idx1]*rs);
            
            glBegin( GL_LINES );
            
                glVertex3f( P0 );
                glVertex3f( P1 );
            
            glEnd();
		}
	}
    
//    
//    
//    ci::gl::color( ci::Color( 1, 1, 1 ) );
//    
//    for( int i = 0; i < ( mPs.size() - 1 ); ++i ) {	
//    
//        
//        ci::Vec3f xAxis = mFrames[i].transformVec( ci::Vec3f::xAxis() );
//		ci::Vec3f yAxis = mFrames[i].transformVec( ci::Vec3f::yAxis() );
//		ci::Vec3f zAxis = mFrames[i].transformVec( ci::Vec3f::zAxis() );
//        
//        ci::Vec3f pos = mPs[i];
//    
//        
//        glBegin( GL_LINES );
//        for( int z = 0; z < 360; z++) {
//            
//            
//            float angle = ci::toRadians( (float) z);
//            float x = 10 * cos(angle);
//            float y = 10 * sin(angle);
//            
//            
//            ci::Vec3f newParticle = pos;
//            
//            newParticle.x += x;
//            newParticle.y += y;
//            
//            
//            newParticle += (mPs[i+1] - mPs[i]);
//            newParticle.rotate(zAxis, angle);
//            
//
//            
//            glVertex3f( newParticle );
//        }
//        glEnd();
//         
//        
//    }
    
//    for( int i = 0; i < mFrames.size(); ++i ) {	
//		ci::gl::pushModelView();
//		ci::gl::multModelView( mFrames[i] );
//        
//        mParticleController[i].draw();
//        
//		ci::gl::popModelView();
//	}

}
*/
 
void CharacterSpline::drawFrames( float lineLength, float lineWidth ) {
	if( mPs.empty() || mFrames.empty() )
		return;
    
	glLineWidth( lineWidth );
	glBegin( GL_LINES );
	for( int i = 0; i < ( mPs.size() - 1 ); ++i ) {	
        
		ci::Vec3f xAxis = mFrames[i].transformVec( ci::Vec3f::xAxis() );
		ci::Vec3f yAxis = mFrames[i].transformVec( ci::Vec3f::yAxis() );
		ci::Vec3f zAxis = mFrames[i].transformVec( ci::Vec3f::zAxis() );
        
		glLineWidth( lineWidth );
		ci::gl::color( ci::Color( 1, 0.5f, 0 ) );
 		glVertex3f( mPs[i] );
 		glVertex3f( mPs[i] + xAxis*lineLength );
		
		ci::gl::color( ci::Color( 1, 0, 1 ) );
 		glVertex3f( mPs[i] );
 		glVertex3f( mPs[i] + yAxis*lineLength );
        
		glLineWidth( 2*lineWidth );
		ci::gl::color( ci::Color( 0, 1, 1 ) );
 		glVertex3f( mPs[i] );
 		glVertex3f( mPs[i] + zAxis*lineLength );		
	}
	glEnd();
}

void CharacterSpline::drawFrameSlices( float scale ) {
	ci::gl::color( ci::ColorA( 1, 1, 1, 0.15f ) );
	for( int i = 0; i < mFrames.size(); ++i ) {	
		ci::gl::pushModelView();
		ci::gl::multModelView( mFrames[i] );
        
        
		glBegin( GL_QUADS );
        
		glVertex3f( ci::Vec3f( -1,  1, 0 )*scale );
		glVertex3f( ci::Vec3f(  1,  1, 0 )*scale );
		glVertex3f( ci::Vec3f(  1, -1, 0 )*scale );
		glVertex3f( ci::Vec3f( -1, -1, 0 )*scale );
        
		glEnd();
		ci::gl::popModelView();
	}
    
	ci::gl::color( ci::ColorA( 1, 1, 1, 0.75f ) );
	for( int i = 0; i <  mFrames.size(); ++i ) {	
		ci::gl::pushModelView();
		ci::gl::multModelView( mFrames[i] );
		
		glBegin( GL_LINES );
		
		glVertex3f( ci::Vec3f( -1,  1, 0 )*scale );
		glVertex3f( ci::Vec3f(  1,  1, 0 )*scale );
        
		glVertex3f( ci::Vec3f(  1,  1, 0 )*scale );
		glVertex3f( ci::Vec3f(  1, -1, 0 )*scale );
        
		glVertex3f( ci::Vec3f(  1, -1, 0 )*scale );
		glVertex3f( ci::Vec3f( -1, -1, 0 )*scale );
        
		glVertex3f( ci::Vec3f( -1, -1, 0 )*scale );
		glVertex3f( ci::Vec3f( -1,  1, 0 )*scale );			
		
		glEnd();
		ci::gl::popModelView();
	}
}