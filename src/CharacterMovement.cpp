#include "CharacterMovement.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"

#include <math.h>

using namespace ci;
using namespace ci::app;
using namespace std;

#define WINCE  0
#define JUMP   1
#define SPHERE 2
#define BACK   3
#define CENTER 4

Perlin smPerlin( 2 );

//GLOBAL FOR TIMELINE
bool endStartAnimation;
//AUDIO + TIMELINE
bool mWaitfor[W_COUNT];


CharacterMovement::CharacterMovement() {
    
    for( int i = 0; i < niko::getArrLength(mStartTimes); i++ ) {
        mStartTimes[i]      = 0;
        mTargetTimes[i]     = 0;
        mActive[i]          = false;
    }
    
    mpBackboneBond = NULL;
    mpCharacterPoints = NULL;
    mpBonds = NULL;
    mpStandBonds = NULL;
    mPhysic = NULL;
    mStandUpPoint = NULL;
    mpBackbone = NULL;
    
    mPerlin         = Vec3f::zero();
    
    for( int i = 0; i < W_COUNT; i++ ) {
        mWaitfor[i] = false;
    }
    
    endStartAnimation = false;
    mDoMove        = false;
}

void CharacterMovement::setup(  std::vector<CharacterPoint> * _pCP,  std::vector<Bond> * _pBonds ) {
    //Setze PointerVectoren
    mpCharacterPoints = _pCP;
    mpBonds = _pBonds;
}

void CharacterMovement::setPhysics( Physics::World3D* _p ) {
    mPhysic = _p;
}

void CharacterMovement::setStandBond( std::vector<Bond> * _pBonds ) {
    mpStandBonds = _pBonds;
    
}

void CharacterMovement::setBackbone( Backbone * _pBackbone ) {
    mpBackbone      = _pBackbone;
    mpBackboneBond  = _pBackbone->getBond();
}

void CharacterMovement::initAttractorController() {
    mAttractorController.init( mPhysic, mpCharacterPoints);
}

void CharacterMovement::initStandUp() {
    if( mPhysic == NULL) { return; }
    if( mpBackboneBond == NULL) { return; }
        
    ci::Vec3f pos = mpBackboneBond->b->getPosition();
    pos.y = -300.0f;
    
    mStandUpPoint = mPhysic->makeParticle(pos);    
    mStandUpPoint->makeOutside(true);
    mStandUpPoint->setMass(50.0f)->setBounce(0.0f)->setRadius(5.0f)->makeFixed();
    
    
    mStandUpBond = Bond( mStandUpPoint, mpBackboneBond->a );
    mStandUpBond.makeBond(mPhysic);
    mStandUpBond.turnOff();
    mStandUpBond.mStrength = 0.5f;
}

void CharacterMovement::standUp() {
    
    if( mStandUpPoint == NULL ) {
        initStandUp();
        return;
    }
    
    Vec3f pA = mpBackboneBond->a->getPosition();   // TOP
    Vec3f pB = mpBackboneBond->b->getPosition();   // BOTTOM
    
    ci::Vec3f upPos = mStandUpPoint->getPosition();
    //set new StandUpPoint Position
    upPos.x = pB.x;
    upPos.z = pB.z;
    mStandUpPoint->moveTo(upPos);
    
    
    float diff = (pA.y - pB.y) * -1;
    float lnbb = mpBackboneBond->getBondLength();
    if( lnbb * 0.8 > diff ) {
        //zurück holen
        mStandUpBond.setBondLength( upPos.distance(pB) - lnbb );
        mStandUpBond.turnOn();
    }
    else  {
        mStandUpBond.turnOff();
    }
    
}

/////////////////////////////////////////////
// MOVEMENTS ////////////////////////////////
/////////////////////////////////////////////

/////////////////////////////
/// START ANIMATION /////////
/////////////////////////////
void sStartAnimation()
{
	endStartAnimation = true;
}


void CharacterMovement::startAnimation( time_t _duration ) {
    
    if(mpBackboneBond == NULL || mpBonds == NULL || mpStandBonds == NULL) return;
    
    //Andere Bewegungen Stoppen
    mDoMove = false;
    
    for( int i = 0; i < W_COUNT; i++ ) {
        mWaitfor[i] = false;
    }
    
    //Alle Punkte Trennen
    for( int i = 0; i < mpBonds->size(); i++ ) {
        mpBonds->at(i).turnOff();
    }
    
    for( int i = 0; i < mpStandBonds->size(); i++ ) {
        mpStandBonds->at(i).turnOff();
    }
    
    mpBackboneBond->turnOff();
    mStandUpBond.turnOff();
    
    //Zufallszeit bis zum erstellen
    float randMS = Rand::randFloat( (_duration - niko::getTimeMS())/1.3, _duration - niko::getTimeMS() );
    
    //ZufallsPunkte im Raum + Gravity an
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){
        
        if( p->getEndOfLine() && p->mFunction == CPF_STAND ) {
        
            Vec3f sp = p->getPosition();
            sp.y = 200;
            p->savePosition = sp;
            
        }
        
        //Rand Position
        Vec3f randVec = Rand::randVec3f();
        randVec.x *= Rand::randInt(0,1000);
        randVec.z *= Rand::randInt(0,1000);
        randVec.y *= Rand::randInt(0,200);
        
        p->moveTo( randVec );
       
    }
        
    //Make Attraction
    mAttractorController.addStart();
    mAttractorController.mStartAttractor.mMass = 2.0f;
    
    timeline().apply( &mAttractorController.mStartAttractor.mMass, 0.0f, randMS/1000, EaseInSine() )
            .finishFn( sStartAnimation ); 
    
}

void CharacterMovement::endOfStartAnimation() {
    
    mAttractorController.removeStart();
    
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){
        
        if( p->getEndOfLine() && p->mFunction == CPF_STAND ) {
            
            p->moveTo( p->savePosition );
            p->setFixed();
            
            int id = p->getStandBondID();
            mpStandBonds->at(id).setBondLength(mpStandBonds->at(id).mSaveMaxLength);
            
        }
    }
    
    //Alle Punkte Verbinden
    for( int i = 0; i < mpBonds->size(); i++ ) {
        mpBonds->at(i).turnOn();
    }
    
    for( int i = 0; i < mpStandBonds->size(); i++ ) {
        mpStandBonds->at(i).turnOn();
    }
    
    mpBackboneBond->turnOn();
    mStandUpBond.turnOn();
    
    for( int i = 0; i < W_COUNT; i++ ) {
        mWaitfor[i] = true;
    }
    
}


//////////////////////////////////
/// AUDIO INPUT ANIMATION ////////
//////////////////////////////////

bool CharacterMovement::getWaitfor( int _w ) {
    return mWaitfor[ _w ];
}

void stopBass()
{
    mWaitfor[W_BASS] = true;
}


void CharacterMovement::bass( PeakTimer _pt ) {
    mWaitfor[W_BASS] = false;
    
    int frames = ci::app::getElapsedFrames();
    Vec3f noise = smPerlin.dfBm( mpBackbone->getBottom()->getPosition() * 0.01f + ci::Vec3f( 0, 0, frames / 100.0f ) );
    mPerlin = noise.normalized() * niko::mapping( _pt.mPeak, 0, 10, 1.0f, 20.0f);
    
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){
        
        if( p->mLevel == CPL_FEELER && p->mFunction == CPF_STAND ) {
            
            Vec3f target = mPerlin + Rand::randVec3f() * 5;
            
            p->posToSave();
            p->mAnimPos = ci::Vec3f::zero();
            
            //Time Calc
            float dur = _pt.mTime - niko::getTimeMS();
            
            int easeRand = Rand::randInt(5);
            EaseFn easeFunction = EaseNone();
            
            switch (easeRand) {
                case 1: easeFunction = EaseInSine(); break;
                case 2: easeFunction = EaseInOutBounce(); break;
                case 3: easeFunction = EaseOutCirc(); break;
                case 4: easeFunction = EaseInOutElastic( 2, 1 ); break;
            }
            
            target.y = mPerlin.y * 0.4;

            timeline().apply( &p->mAnimPos, target, dur/1000, easeFunction )
                    .finishFn( stopBass ); 
            
            target.x = Rand::randFloat(-500,500);
            target.y = 0;
            target.z = Rand::randFloat(-500,500);
            
            timeline().appendTo( &p->mAnimPos, target, 0.5f, EaseNone() );
            
            p->setFixed();
            
        }
    }
    
}

void stopMidlow()
{
    mWaitfor[W_MIDLOW] = true;
}

void CharacterMovement::midlow( PeakTimer _pt ) {
    mWaitfor[W_MIDLOW] = false;
    
    Vec3f randTarget = Rand::randVec3f();
    //randTarget.y = 0;
    randTarget *= niko::mapping( _pt.mPeak, 0, 3, 5.0f, 10.0f);
    
    CharacterPoint * cpt = mpBackbone->getBottom();
    cpt->posToSave();
    cpt->mAnimPos = ci::Vec3f::zero();
    
    //Time Calc
    float dur = _pt.mTime - niko::getTimeMS();
    
    timeline().apply( &cpt->mAnimPos, randTarget, dur/1000, EaseInOutBack() )
                .finishFn( stopMidlow ); 
    
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){
        if( p->getEndOfLine() && p->mFunction == CPF_STAND ) {
            p->setFree();
        }
    }
}

void CharacterMovement::midhigh( PeakTimer _pt ) {
    mWaitfor[W_MIDHIGH] = false;
    
    //dauer bewegung
    
}

void CharacterMovement::high( PeakTimer _pt ) {
    mWaitfor[W_HIGH] = false;
    
    
    
}


////////////////

void CharacterMovement::wince( int _amount, bool _soft ) {
        
    //Keine Aktion wenn Springen akitiviert ist
    if(mActive[JUMP]) { return; }
    
    
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){ 
        if( p->getEndOfLine() ) {
        
            float bondLength = (*mpBonds)[p->getBondID(0)].getBondLength();
            
            if(!mActive[WINCE]) {
                mpBonds->at(p->getBondID(0)).mSaveDistanceA = bondLength;
            }
            
            float newLength = niko::mapping( _amount, 0, 100, 0, bondLength );
            mpBonds->at(p->getBondID(0)).setBondLength( newLength );
            
            if( !_soft ) {
                Vec3f pos = p->getPosition();
                pos -= p->getParent()->getPosition();
                pos.normalize();
                pos *= newLength;
                pos += p->getParent()->getPosition();
                p->moveTo( pos );
            }

        }
    }
    
    mActive[WINCE] = true;
    
}

void CharacterMovement::jump( time_t _ms, int _amount ) {
        
    mStartTimes[JUMP] = niko::getTimeMS();
    mTargetTimes[JUMP] = _ms;
    
    if(mActive[JUMP]) { return; }
    
    for(  std::vector<Bond>::iterator p = mpBonds->begin(); p != mpBonds->end(); ++p ){ 
        
        float bondLength = p->getBondLength();
        
        if(!mActive[WINCE]) {
            p->mSaveDistanceA = bondLength;
        }
        
        p->mSaveDistanceB = bondLength * 0.75;
    }
    
    mActive[WINCE] = false;
    mActive[JUMP] = true;
}

void CharacterMovement::moveOnSphere( float _angle, time_t _ms, bool _stand ) {

    mStartTimes[SPHERE] = niko::getTimeMS();
        
    //Set Target Time
    if( _ms == 0 ) { mTargetTimes[SPHERE] = mStartTimes[SPHERE] + 500; }
    else { mTargetTimes[SPHERE] = _ms; }
    
    float angleInRadiansX = ci::toRadians( Rand::randFloat(_angle) );
    float angleInRadiansY = ci::toRadians( Rand::randFloat(_angle) );
    float angleInRadiansZ = ci::toRadians( Rand::randFloat(_angle) );
    
    mfuncfunc = CPF_STAND;
    if( _stand ) { mfuncfunc = CPF_NORMAL; };
    
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){
        
        
        
        if( p->getEndOfLine() && p->mFunction != mfuncfunc) {
             
            //zufall ziel
            p->savePosition = p->getPosition();
            
            Vec3f target = p->savePosition - p->getParent()->getPosition();
            target.normalize();
            target.rotateX(angleInRadiansX);
            target.rotateY(angleInRadiansY);
            target.rotateZ(angleInRadiansZ);
            
            target *= mpBonds->at(p->getBondID(0)).getBondLength();
            target += p->getParent()->getPosition();
            
            //Save Position & Target
            p->saveTarget = target;    
        }   
        
        
    }
    
    //Start Moving
    mActive[SPHERE] = true;
}

void CharacterMovement::moveToCenter( time_t _ms ) {
    
    mStartTimes[CENTER] = niko::getTimeMS();
    if( _ms == 0 ) { mTargetTimes[CENTER] = mStartTimes[CENTER] + 500; }
    else { mTargetTimes[CENTER] = _ms; }
    
    Vec3f targetDirection = mpCharacterPoints->at(0).getPosition();
    
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){ 
        //Save Position & Target
        p->savePosition = p->getPosition();
        p->saveTarget = p->getPosition() - targetDirection;        
    }
    
    mActive[CENTER] = true;
 
}

void CharacterMovement::setBack( time_t _ms ) {
    
    if( _ms != 0 ) {
        mTargetTimes[BACK] = _ms;
        mStartTimes[BACK] = niko::getTimeMS();
    }
    
    time_t timeDelta = mTargetTimes[BACK] - niko::getTimeMS();
    
    if ( timeDelta <= 0 ) { 
        for(  std::vector<Bond>::iterator p = mpBonds->begin(); p != mpBonds->end(); ++p ){  
            p->setBondLength( p->mSaveDistanceA );
        }
        mActive[BACK] = false;
        return;
    }
    
    float t = niko::mapping( timeDelta, 0, mTargetTimes[BACK] - mStartTimes[BACK], 1, 0, true);
    t = ci::easeInQuad( t ); 
    
    for(  std::vector<Bond>::iterator p = mpBonds->begin(); p != mpBonds->end(); ++p ){  
        float distLenght = niko::mapping( t, 0, 1, p->mSaveDistanceB, p->mSaveDistanceA);
        p->setBondLength( distLenght );
    }
    
}

void CharacterMovement::bass( float _input ) {
    
//moveOnSphere( 90, 10, true );
    
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){
        
        if( p->mLevel == CPL_FEELER && p->mFunction == CPF_STAND ) {
            
           
            int counter = ci::app::getElapsedFrames();
            Vec3f pos = p->getPosition();
            
            Vec3f noise = smPerlin.dfBm( pos * 0.01f + ci::Vec3f( 0, 0, counter / 100.0f ) );
            mPerlin = noise.normalized() * 0.005f;
            
            pos += mPerlin * _input;
            
            p->moveTo(pos);
           // p->setFree();
            
        }
    }
    
}

////////////////////////
//UPDATE
////////////////////////

void CharacterMovement::update() {

    //START ANIMATION
    if( endStartAnimation == true ) {
        endOfStartAnimation();
        endStartAnimation = false;
        mDoMove = true;
    }
    
    standUp();
    mAttractorController.update();
    
    if( mDoMove == false ) { return; }
    
    //STAND BASS UPDATE
    if( mWaitfor[W_BASS] == false ) {
        for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){
            if( p->mLevel == CPL_FEELER && p->mFunction == CPF_STAND ) {
                p->addAnimToPosition();
            }
        }
    } 
    
    // BACKBONE MIDLOW UPDATE
    if( mWaitfor[W_MIDLOW] == false ) {
        CharacterPoint * cpt = mpBackbone->getBottom();
        cpt->addAnimToPosition();
    }
    
    
    if( mActive[BACK] ) {
        setBack();
    }
    
    if( mActive[CENTER] ) {
        _moveToCenter();
    }
    
    if( mActive[WINCE] && !mActive[JUMP] ) { 
        _wince();
    }    
    
    if( mActive[JUMP] ) {
        _jump();
    }
    
    if( mActive[SPHERE] ) { 
       _moveOnSphere();
    }

}


////////////////////////
//PRIVATE
////////////////////////

void CharacterMovement::_wince() {
    
    bool ready = true;

    for(  std::vector<Bond>::iterator p = mpBonds->begin(); p != mpBonds->end(); ++p ){  
        if( p->mLevel == 1 ) {
        
            float bondLength = p->getBondLength();
            float saveBondLength = p->mSaveDistanceA;
            
            float difference = saveBondLength - bondLength;
            
            if( fabs( difference ) <= 1 ) {
                p->setBondLength( saveBondLength );
            } else {
                ready = false;
                p->setBondLength( bondLength + difference/10 ); 
            }
        }
    }
    
    if(ready) {
        mActive[WINCE] = false;
    }
}

void CharacterMovement::_jump() {
    
    time_t timeDelta = mTargetTimes[JUMP] - niko::getTimeMS();
    
    if ( timeDelta <= 0 ) { 
        //auseinander
        for(  std::vector<Bond>::iterator p = mpBonds->begin(); p != mpBonds->end(); ++p ){  
            float newLength = p->getBondLength() * 1.5;
            p->setBondLength( newLength );
            p->mSaveDistanceB = newLength;
        }
        
        //und zurück
        mStartTimes[BACK] = niko::getTimeMS();
        mTargetTimes[BACK] = mStartTimes[BACK] + 1500;
        mActive[BACK] = true;
        mActive[JUMP] = false;
        return;
    } else {     
        
        float t = niko::mapping( timeDelta, (mTargetTimes[JUMP] - mStartTimes[JUMP])/10, mTargetTimes[JUMP] - mStartTimes[JUMP], 1, 0, true);
        t = ci::easeNone( t ); 
        
        for(  std::vector<Bond>::iterator p = mpBonds->begin(); p != mpBonds->end(); ++p ){  
            float distLenght = niko::mapping( t, 0, 1, p->mSaveDistanceA, p->mSaveDistanceB);
            p->setBondLength( distLenght );
        }
        
        
    }
}

void CharacterMovement::_moveOnSphere() {
   
    // SLERP
    // position t von 0 - 1;
    // a.slerp( t, b )
       
    time_t timeDelta = mTargetTimes[SPHERE] - niko::getTimeMS();
    
    if ( timeDelta <= 0 ) { 
        
        for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){ 
            if( p->getEndOfLine() && p->mFunction != mfuncfunc) {
                p->moveTo( p->saveTarget );
            }
        }
        
        mActive[SPHERE] = false;
        return;
    }
    
    
    
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){ 
        if( p->getEndOfLine() && p->mFunction != mfuncfunc ) {

            float t = niko::mapping( timeDelta, 0, mTargetTimes[SPHERE] - mStartTimes[SPHERE], 1, 0, true);
            t = ci::easeInOutQuad( t ); 
            
            
            Vec3f mVecA = p->savePosition - p->getParent()->getPosition();
            Vec3f mVecB = p->saveTarget - p->getParent()->getPosition();;
            
            mVecA.normalize();
            mVecB.normalize();
            
            ci::Vec3f newPosition = mVecA.slerp( t, mVecB );
            
            newPosition *= mpBonds->at(p->getBondID(0)).getBondLength();
            newPosition += p->getParent()->getPosition();
            
            p->moveTo( newPosition );
            
        }   
    }    
}

void CharacterMovement::_moveToCenter() {
    
    time_t timeDelta = mTargetTimes[CENTER] - niko::getTimeMS();
    
    if ( timeDelta <= 0 ) { 
        
        for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){ 
            p->moveTo(p->saveTarget);
        }
        
        mActive[CENTER] = false;
        return;    
    }
    
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){ 
        
        float t = niko::mapping( timeDelta, 0, mTargetTimes[CENTER] - mStartTimes[CENTER], 0, 1, true);
        t = ci::easeInBack( t ); 
        
        Vec3f newPosition = p->saveTarget + (p->savePosition - p->saveTarget)*t;
        p->moveTo(newPosition);
           
    }
}






