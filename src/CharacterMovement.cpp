#include "CharacterMovement.h"
#include "cinder/Rand.h"

#include <math.h>

using namespace ci;
using namespace ci::app;
using namespace std;

#define WINCE  0
#define SPHERE 1

CharacterMovement::CharacterMovement() {
    
    for( int i = 0; i < niko::getArrLength(mStartTimes); i++ ) {
        mStartTimes[i]      = 0;
        mTargetTimes[i]     = 0;
        mActive[i]          = false;
        mTargetPositions[i] = ci::Vec3f::zero();
    }
    
}

void CharacterMovement::setup(  std::vector<CharacterPoint> * _pCP,  std::vector<Bond> * _pBonds ) {
    //Setze PointerVectoren
    mpCharacterPoints = _pCP;
    mpBonds = _pBonds;
}

void CharacterMovement::wince( int _amount, bool _soft ) {
        
    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){ 
        if( p->getEndOfLine() ) {
        
            float bondLength = (*mpBonds)[p->getBondID()].getBondLength();
            
            if(!mActive[WINCE]) {
                mpBonds->at(p->getBondID()).mSaveDistance = bondLength;
            }
            
            float newLength = niko::mapping( _amount, 0, 100, 0, bondLength );
            mpBonds->at(p->getBondID()).setBondLength( newLength );
            
            if( !_soft ) {
                Vec3f pos = p->getPosition();
                pos -= p->mNeighbours[0]->getPosition();
                pos.normalize();
                pos *= newLength;
                pos += p->mNeighbours[0]->getPosition();
                p->moveTo( pos );
            }

        }
    }
    
    mActive[WINCE] = true;
    
}

void CharacterMovement::moveOnSphere( Vec3f _target, Vec3f _parent, time_t _ms ) {

    mStartTimes[SPHERE] = niko::getTimeMS();
        
    //Set Target Time
    if( _ms == 0 ) { mTargetTimes[SPHERE] = mStartTimes[SPHERE] + 500; }
    else { mTargetTimes[SPHERE] = _ms; }
    
    //Set Target Position
    mTargetPositions[SPHERE] = _target;
    mParentSphere            = _parent;
    
    //Start Moving
    mActive[SPHERE] = true;
}

////////////////////////
//UPDATE
////////////////////////

void CharacterMovement::update() {

  
    //Moving
    if( mActive[WINCE] ) { 
        _wince();
    }    
    
    if( mActive[SPHERE] ) { 
       // mCurrentPosition = _moveOnSphere();
    }

}


////////////////////////
//PRIVATE
////////////////////////

void CharacterMovement::_wince() {
    
    bool ready = true;

    for(  std::vector<CharacterPoint>::iterator p = mpCharacterPoints->begin(); p != mpCharacterPoints->end(); ++p ){ 
        if( p->getEndOfLine() ) {
            
            float bondLength = (*mpBonds)[p->getBondID()].getBondLength();
            float saveBondLength = (*mpBonds)[p->getBondID()].mSaveDistance;
            
            float difference = saveBondLength - bondLength;
            
            if( fabs( difference ) <= 1 ) {
                mpBonds->at(p->getBondID()).setBondLength( saveBondLength );
            } else {
                ready = false;
                mpBonds->at(p->getBondID()).setBondLength( bondLength + difference/10 ); 
            }
            
        }
    }
    
    if(ready) {
        mActive[WINCE] = false;
    }
}

ci::Vec3f CharacterMovement::_moveOnSphere() {
    
    // SLERP
    // position t von 0 - 1;
    // a.slerp( t, b )
    
    time_t timeDelta = mTargetTimes[SPHERE] - niko::getTimeMS();
    cout << "fertig: " << timeDelta << endl;
    if ( timeDelta <= 0 ) { 
        
        mActive[SPHERE] = false;
        return mTargetPositions[SPHERE] + mParentSphere;
    }
    
    float t = niko::mapping( timeDelta, 0, mTargetTimes[SPHERE] - mStartTimes[SPHERE], 1, 0, true);
        
    Vec3f mVecA = mCurrentPosition - mParentSphere;
    Vec3f mVecB = mTargetPositions[SPHERE];
    
    float length = mVecA.length();
    
    mVecA.normalize();
    mVecB.normalize();

    ci::Vec3f newPosition = mVecA.slerp( t, mVecB );
    
    newPosition *= length;
    newPosition += mParentSphere;
    
    return newPosition;
  
}
