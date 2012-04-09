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
#include "CharacterSpline.h"
#include "CharacterMovement.h"

#include "Constants.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MSA;


extern int fps;

class Character {
 public:
	Character();
    Character( ci::Vec3f _pos, float _radius, Quatf _rotation );
    
    void createNewStructure( int _num );
    void createCharacter();
    void setProperties( CharacterPoint *_point, int _typ );
    void setRelations( CharacterPoint *_parent, CharacterPoint *_child );
    void setBonds( CharacterPoint *_parent, CharacterPoint *_child ); 
    
    int  getRandPointNumber( int _min = 0, int _max = 4 );
    
    void createPhysics();
    void createBackbone();
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
    void gravity();
    void setNextBeat( time_t _bang );    
  
    void updateRootSpline();
    void updateSplines();
    void updateEmotions( float _frustration, float _engagement,float _meditation, float _excitement );
    void update();
    
    void drawRoom(); 
    void draw();
    

    bool        mDrawCharacter;
    
    float       mRadius;
    int         mNumberOfRootPoints;
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
    std::vector<CharacterSpline>                mCharacterSplines;
    std::vector<CharacterPoint*>                mRootPath;
    CharacterSpline                             mCharacterRoot;
    
    Backbone                                    mBackbone;
    
    //Particle
    std::vector<ParticleController>  mParticleController;
    gl::Texture                      mParticleTexture;
    
    //Physics
    Physics::World3D        mPhysics;
    int                     mForceTimer;
    std::vector<Bond>       mBonds;
    std::vector<Bond>       mStandBonds;
    Bond                    mBackboneBond;
    
    //EmoAttractos
    EmoAttractor            mFrustrationAtt;
    EmoAttractor            mEngagementAtt;    
    
};

#endif