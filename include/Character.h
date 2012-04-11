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
#include "cinder/Timeline.h"

#include <list>
#include <vector>
#include <sys/time.h>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "cinder/Perlin.h"

#include "niko_functionen.h"
#include "ParticleController.h"
#include "EmoAttractor.h"
#include "CharacterBond.h"
#include "CharacterPoint.h"
#include "CharacterSpline.h"
#include "CharacterMovement.h"
#include "CharacterBackbone.h"

#include "Constants.h"
#include "Structs.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace MSA;


extern int fps;

class Character {
 public:
	Character();
    Character( ci::Vec3f _pos, float _radius, Quatf _rotation );
    
    void createNewStructure( int _num = 5);
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
    
    void destroyStructure();
    
    //AUDIO
    void startAnimation( time_t _duration );
    
    bool waitforBass();
    void inputBass( PeakTimer _pt );
    bool waitforMidlow();
    void inputMidlow( PeakTimer _pt );
    bool waitforMidHigh();
    void inputMidhigh( PeakTimer _pt );
    bool waitforHigh();
    void inputHigh( PeakTimer _pt );
    
    //RENAME
    void test();
    
    //movement
    void addRandomForce( float _f );
    void move(Vec3f _position, Quatf _rotation);
    
    void sphere();
    void moveToCenter();
    void jump( PeakTimer _pt );
    void gravity( PeakTimer _pt );
     
    void particleDrop();
    
    
    void updateSplines();
    void updateEmotions( float _frustration, float _engagement,float _meditation, float _excitement );
    void update();
    
    void draw();
    
    //VARS///////////////
    
    bool        mAlive;
    
    bool        mDrawCharacter;
    
    float       mRadius;
    int         mNumberOfRootPoints;
    bool        mOpenLines;
    int         mMaxLevels;

    std::vector<CharacterPoint> mCharacterPoints;
    CharacterMovement           mMovement;
    
    //VIEWING
    ci::Vec3f   mCenterPosition;
    Quatf       mRotation;
    
    //Spline & Path
    std::vector< std::vector<CharacterPoint*> > mPaths;
    std::vector<CharacterSpline>                mCharacterSplines;
    
    Backbone                                    mBackbone;
    
    //Particle
    std::vector<ParticleController>  mParticleController;
    gl::Texture                      mParticleTexture;
    
    //Physics
    Physics::World3D        mPhysics;
    int                     mForceTimer;
    std::vector<Bond>       mBonds;
    std::vector<Bond>       mStandBonds;
    
    //EmoAttractos
    EmoAttractor            mFrustrationAtt;
    EmoAttractor            mEngagementAtt;   
    
    //Anim
    Anim<float>             mGravityAnim;
    
};

#endif