#ifndef emotion_Character_h
#define emotion_Character_h

#include "cinder/app/AppBasic.h"
#include "cinder/Channel.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Sphere.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Easing.h"
#include "cinder/gl/Texture.h"

#include <list>
#include <vector>


#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "cinder/Perlin.h"

#include "niko_functionen.h"
#include "ParticleController.h"
#include "EmoAttractor.h"
#include "CharacterStructs.h"
#include "CharacterPoint.h"
#include "CharacterMovement.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MSA;

//PHYSICS
#define	SPRING_STRENGTH         0.01f
#define BOUNCE                  0.2f

#define	GRAVITY					0.0f
#define FORCE_AMOUNT			10.0f

#define SECTOR_COUNT			1		// currently there is a bug at sector borders

#define CENTER                  ci::Vec3f::zero()

extern int fps;

class Character {
 public:
	Character();
    Character( ci::Vec3f _pos, float _radius, Quatf _rotation );
    
    void createNewStructure( int _num );
    void mkPoint(CharacterPoint *lastPoint, int _level);
    int  getRandPointNumber( int _min = 0, int _max = 4 );
    
    void createPhysics();
    void createParticleController(); 
    void createSplinesA();
    void createSplinesB();
    void pathFinder( CharacterPoint *_lastPoint, int _childID, std::vector<CharacterPoint*> _path );
    int countEnds(); 
    
    
    void setRadius( float _r );
    void scale( float _s );
    void addRandomForce( float _f );
    void move(Vec3f _position, Quatf _rotation);
    void dance();
    void wince( int _amount = 50 );
    void jump( int _amount = 50 );
    void center();
    void sphere();
    //RENAME
    void test();
    void setNextBeat( time_t _bang );    
  
    void updateSplines();
    void updateEmotions( float _frustration, float _engagement,float _meditation, float _excitement );
    void update();
    
    void draw();
    

    bool        mDrawCharacter;
    
    float       mRadius;
    int         mNumberOfCharacterPoints;
    int         mCharacterPointsLeft;
    bool        mOpenLines;
    int         mMaxLevels;
    
    time_t      mNextBeat;

    std::vector<CharacterPoint> mCharacterPoints;
    CharacterMovement           mMovement;
    
    //VIEWING
    ci::Vec3f   mCenterPosition;
    Quatf       mRotation;
    
    //Spline & Path
    std::vector< std::vector<CharacterPoint*> > mPaths;
    std::vector<BSpline3f>  mSplines;
    
    //Particle
    std::vector<ParticleController>  mParticleController;
    
    //Physics
    Physics::World3D        mPhysics;
    int                     mForceTimer;
    std::vector<Bond>       mBonds;
    
    //EmoAttractos
    EmoAttractor            mFrustrationAtt;
    EmoAttractor            mEngagementAtt;    
    
};

#endif