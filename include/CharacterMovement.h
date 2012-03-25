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

#define MOVEMENTS 5

extern int fps;

class CharacterMovement {
 public:
	CharacterMovement();
    void        setup(  std::vector<CharacterPoint> * _pCP,  std::vector<Bond> * _pBonds );
    
    void        wince( int _amount = 50, bool _soft = true );
    void        jump( time_t _ms, int _amount = 50 );
    void        moveOnSphere( float _angle = 90, time_t _ms = 0 );
    void        moveToCenter( time_t _ms = 0 );
    void        setBack( time_t _ms = 0 );
    
    void        update();
    
    
private:
    
    //FUNC
    void        _wince();
    void        _jump();
    void        _moveOnSphere();
    void        _moveToCenter();
    
    //VAR
    std::vector<CharacterPoint> * mpCharacterPoints;
    std::vector<Bond> * mpBonds;
    
    time_t      mStartTimes[MOVEMENTS];
    time_t      mTargetTimes[MOVEMENTS];
    
    bool        mActive[MOVEMENTS];
    
};

#endif