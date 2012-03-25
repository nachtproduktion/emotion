#ifndef emotion_CharacterMovement_h
#define emotion_CharacterMovement_h

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Easing.h"
#include "cinder/Perlin.h"

#include <vector>

#include "niko_functionen.h"
#include "CharacterStructs.h"
#include "CharacterPoint.h"

#define MOVEMENTS 2

extern int fps;

class CharacterMovement {
 public:
	CharacterMovement();
    void        setup(  std::vector<CharacterPoint> * _pCP,  std::vector<Bond> * _pBonds );
    
    void        wince( int _amount = 50, bool _soft = true );
    void        moveOnSphere( ci::Vec3f _target, ci::Vec3f _parent, time_t _ms = 0 );
    
    void        update();
    
    
private:
    
    //FUNC
    void        _wince();
    ci::Vec3f   _moveOnSphere();
    
    //VAR
    std::vector<CharacterPoint> * mpCharacterPoints;
    std::vector<Bond> * mpBonds;
    
    time_t      mStartTimes[MOVEMENTS];
    time_t      mTargetTimes[MOVEMENTS];
    
    ci::Vec3f   mCurrentPosition;
    ci::Vec3f   mTargetPositions[MOVEMENTS];
    
    ci::Vec3f   mParentSphere;
    
    bool        mActive[MOVEMENTS];
    
};

#endif