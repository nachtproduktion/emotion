#ifndef emotion_CharacterMovement_h
#define emotion_CharacterMovement_h

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Easing.h"
#include "cinder/Perlin.h"
#include "cinder/Timeline.h"

#include <vector>

#include "niko_functionen.h"
#include "CharacterBond.h"
#include "CharacterPoint.h"
#include "AttractorController.h"
#include "CharacterBackbone.h"

#include "Structs.h"
#include "Constants.h"
#define MOVEMENTS 5

extern int fps;

class CharacterMovement {
 public:
	CharacterMovement();
    void        setup(  std::vector<CharacterPoint> * _pCP,  std::vector<Bond> * _pBonds );
    
    void        setPhysics( Physics::World3D* _p );
    void        setStandBond( std::vector<Bond> * _pBonds );
    void        setBackbone( Backbone * _pBackbone );
    void        initAttractorController();
    
    //start animation
    void        startAnimation( time_t _duration );
    void        endOfStartAnimation();
    
    //audio Input
    bool        getWaitfor( int _w );
    
    void        bass( PeakTimer _pt );
    void        midlow( PeakTimer _pt );
    void        midhigh( PeakTimer _pt );
    void        high( PeakTimer _pt );
    

    
    
    void        wince( int _amount = 50, bool _soft = true );
    void        jump( time_t _ms, int _amount = 50 );
    void        moveOnSphere( float _angle = 90, time_t _ms = 0, bool _stand = false );
    void        moveToCenter( time_t _ms = 0 );
    void        setBack( time_t _ms = 0 );
        
    void        update();
    
    void        initStandUp();
    void        standUp();
    
    void        bass( float _input );
    
private:
    
    //FUNC
    void        _wince();
    void        _jump();
    void        _moveOnSphere();
    void        _moveToCenter();
    
    //VAR
    MSA::Physics::World3D*          mPhysic;
    MSA::Physics::Particle3D*       mStandUpPoint;
    Bond                            mStandUpBond;
    
    std::vector<CharacterPoint> *   mpCharacterPoints;
    std::vector<Bond> *             mpBonds;
    std::vector<Bond> *             mpStandBonds;
    Bond *                          mpBackboneBond;
    Backbone *                      mpBackbone;
    
    AttractorController             mAttractorController;
    
    time_t      mStartTimes[MOVEMENTS];
    time_t      mTargetTimes[MOVEMENTS];
    
    bool        mActive[MOVEMENTS];
    
    int         mfuncfunc;
    
    ci::Vec3f   mPerlin;
    
    bool        mDoMove;
    
};

#endif