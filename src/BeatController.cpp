//
//  BeatController.cpp
//  emotion
//

//  Created by Nikolas on 15.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.

//

#include "BeatController.h"
#include <iostream>

BeatController::BeatController() {
    
    mBPM            = 120;
    mTimeOffset     = 500;
    //mFrameOffset    = 30;
    mCounter        = 0;
    mBangTime       = 0;
    
}

void BeatController::setBPM( int _bpm ) {
    
    mBPM = _bpm;
    calcTimeOffset();
    
}

void BeatController::calcTimeOffset() {
    
    // 1 min = 60 000 ms
    float msProBeat = 60000 / mBPM;
    
    //Get Frames pro MS
   // mCalcFrameRate = ci::app::getFrameRate();
    //float fpms = mCalcFrameRate / 1000;
        
    //Frames Pro Beat
   // float framesProBeat = fpms * msProBeat;
    
    mTimeOffset = (int) msProBeat;
    //mFrameOffset = (int) framesProBeat;
    
    //cout << "mTimeOffset: " << mTimeOffset << " - mFrameOffset: " << mFrameOffset << " / " << ci::app::getFrameRate() << endl;
    
}

time_t BeatController::getNextBeat() {
    
    static time_t nextBang = 0;
    time_t curTime = niko::getTimeMS();
    
    if( curTime <= nextBang ) { 
        return nextBang; 
    }
    else {
        nextBang = mBangTime + mTimeOffset * mCounter;
        mCounter++;
        return nextBang;
    }

}

void BeatController::addCharacter( Character * _character ) {
    
    mCharacterList.push_back( _character );
    
}

void BeatController::delCharacter( int _id ) {
    
}

void BeatController::emtpyCharacter() {
    
    mCharacterList.clear();

}

void BeatController::bang() {

    mBangTime = niko::getTimeMS();
    mCounter = 1;
  
}

void BeatController::update() {
    static time_t lastbangTime = getNextBeat();
    
    if( mBangTime == 0 ) return;
    
    time_t nextBangTime = getNextBeat();
    
    if( lastbangTime != nextBangTime ) {
        
        lastbangTime = nextBangTime;
        
        for( int i = 0; i < mCharacterList.size(); i++ ) { 
            mCharacterList[i]->dance( nextBangTime );
        }   
    }

}