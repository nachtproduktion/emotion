#ifndef emotion_CharacterStructs_h
#define emotion_CharacterStructs_h

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"

#include <vector>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "niko_functionen.h"
#include "CharacterPoint.h"

#include "Constants.h"


///////////////////////////////////////////////////////////////
//////////// Backbone ///////////
///////////////////////////////////////////////////////////////

struct Backbone {
    
    Backbone () {
        //

    }
    
    void setPoints( std::vector < CharacterPoint* > _pointList ) {
        
        mPoints = _pointList;
                
    }
    
    void update() {
        
        if(mPoints.size() <= 2) { return; }
        
        updatePositions();
        calcMatrix();
        
        updateSpacers();
    }
    
    void updateSpacers() {
        
        int counter = 0;
        for( int i = 0; i < mPoints.size(); ++i ) {
                        
            if( mPoints[i]->mFunction == CPF_MIDDLE ) {
                
                
                
                int nSpacer = mPoints[i]->getNumberOfSpacers();
                float r = mPoints[i]->getShellRadius();
                
                for( int z = 0; z < nSpacer; z++ ) {
                    float angle = ( TWOPI / nSpacer ) * z;
                    ci::Vec3f position = ci::Vec3f( cos( angle )*r, 0, sin( angle )*r );
                    
                    mPoints[i]->getSpacer( z )->moveTo( mFrames[counter] * position );
                }
                
            }
            
            if( mPoints[i]->mFunction != CPF_SPACER ) {
                counter++;
            }
            
        }
        
    }
    
    void updatePositions()
    {
        
        mPs.clear();
        
        for( int i = 0; i < mPoints.size(); ++i ) {
            if( mPoints[i]->mFunction != CPF_SPACER ) {
                
                Vec3f P = mPoints[i]->getPosition();
                mPs.push_back( P );
                
            }
        }
        
    }
    
    void calcMatrix() {
        
        mFrames.clear();
        
        int n = mPs.size();
        mFrames.resize( n );
        
        for( int i = 0; i < n; ++i ) {
            Vec3f p0, p1, p2;		
            if( i < (n - 2) ) {
                p0 = mPs[i];
                p1 = mPs[i + 1];
                p2 = mPs[i + 2];
            }
            else if( i == (n - 2) ) {
                p0 = mPs[i - 1];
                p1 = mPs[i];
                p2 = mPs[i + 1];
            }	
            else if( i == (n - 1) ) {
                p0 = mPs[i - 3];
                p1 = mPs[i - 2];
                p2 = mPs[i - 1];
            }
            
            
            Vec3f t = (p1 - p0).normalized();
            Vec3f n = t.cross(p2 - p0).normalized();
           
            if( t == (p2 - p0).normalized() ) { //n = Vec3f(0,0,-1);
            
            //if( n.length() == 0.0f ) {
                int ix = fabs( t[0] ) < fabs( t[1] ) ? 0 : 1;
                if( fabs( t[2] ) < fabs( t[ix] ) ) 
                    ix = 2;
				
                Vec3f v( 0.0f, 0.0f, 0.0f ); 
                v[ix] = 1.0;
                n = t.cross( v ).normalized();
            }
            Vec3f b = t.cross( n );	
            
            //b = Vec3f(1,0,0);
            
            Matrix44f& m = mFrames[i];
            m.at( 0, 0 ) = b.x;
            m.at( 1, 0 ) = b.y;
            m.at( 2, 0 ) = b.z;
            m.at( 3, 0 ) = 0;
            
            m.at( 0, 1 ) = n.x;
            m.at( 1, 1 ) = n.y;
            m.at( 2, 1 ) = n.z;
            m.at( 3, 1 ) = 0;
            
            m.at( 0, 2 ) = t.x;
            m.at( 1, 2 ) = t.y;
            m.at( 2, 2 ) = t.z;
            m.at( 3, 2 ) = 0;
            
            m.at( 0, 3 ) = mPs[i].x;
            m.at( 1, 3 ) = mPs[i].y;
            m.at( 2, 3 ) = mPs[i].z;
            m.at( 3, 3 ) = 1;
        }
    }
    
    std::vector < CharacterPoint* > mPoints;
    
    std::vector<Vec3f>		mPs;			// Points
	std::vector<Matrix44f>	mFrames;		// Coordinate frame
    
};


///////////////////////////////////////////////////////////////
//////////// BOND ///////////
///////////////////////////////////////////////////////////////
struct Bond {

    Bond() {
        a = NULL;
        b = NULL;
        s = NULL;
    }
    Bond( CharacterPoint * _particleA, CharacterPoint * _particleB ) {
        
        a = _particleA->getParticle();
        b = _particleB->getParticle();
        s = NULL;
        
        init();
    }
    
    Bond( MSA::Physics::Particle3D * _particleA, MSA::Physics::Particle3D * _particleB ) {
        
        a = _particleA;
        b = _particleB;
        s = NULL;
        
        init();
    }
    
    void init() {
        
        //Level 1 = Außen
        //Level 2 = Innen
        //...
        
        mLevel = 2;
        
        mStrength = SPRING_STRENGTH;
        mSaveDistanceA = a->getPosition().distance(b->getPosition());
        mSaveDistanceB = mSaveDistanceA;
        
    }
    
    void makeBond( MSA::Physics::World3D * _physics ) {
        s = _physics->makeSpring(a, b, mStrength, mSaveDistanceA);
    }
    
    void killBond( MSA::Physics::World3D * _physics ) {
        if(s) s->kill();
    }
    
    void setBondLength( float _len ) {
        if(s) s->setRestLength(_len);
    }
    
    float getBondLength() {
        if(s) return s->getRestLength();
        else  return 0.0f;
    }
    
    void turnOff() {
        if(s) s->turnOff();
    }
    
    void turnOn() {
        if(s) s->turnOn();
    }
    
    void render() {
        gl::drawLine(a->getPosition(), b->getPosition());
    }
    
    int   mLevel;
    float mStrength;
    float mSaveDistanceA;
    float mSaveDistanceB;
    float mSaveMaxLength;
    MSA::Physics::Particle3D * a;
    MSA::Physics::Particle3D * b;
    MSA::Physics::Spring3D *s;
    
};


#endif