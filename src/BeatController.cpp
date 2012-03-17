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
    mBangTime       = 0;
    mNextBang       = 0;
    
    newBeat         = false;
    
}

void BeatController::setBPM( int _bpm ) {
    
    mBPM = _bpm;
    calcTimeOffset();
    
}

void BeatController::calcTimeOffset() {
    
    // 1 min = 60 000 ms
    float msProBeat = 60000 / mBPM;
    
    mTimeOffset = (int) msProBeat;
    
    //bang();
    
    //Get Frames pro MS
   // mCalcFrameRate = ci::app::getFrameRate();
    //float fpms = mCalcFrameRate / 1000;
        
    //Frames Pro Beat
   // float framesProBeat = fpms * msProBeat;
    
    
    //mFrameOffset = (int) framesProBeat;
    
    //cout << "mTimeOffset: " << mTimeOffset << " - mFrameOffset: " << mFrameOffset << " / " << ci::app::getFrameRate() << endl;
    
}

void BeatController::calcNextBeat() {
    
    time_t curTime = niko::getTimeMS();
    
    if( curTime > mNextBang ) { 
        mNextBang += mTimeOffset;
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

    mBangTime = mNextBang = niko::getTimeMS();
  
}

void BeatController::update() {

    static time_t lastBang = mNextBang;
    
    if( mBangTime == 0 ) return;
    
    // wenn neuer Beat dann update
    calcNextBeat();
    if( lastBang != mNextBang ) {
        newBeat = true;
        lastBang = mNextBang;
        
        for( int i = 0; i < mCharacterList.size(); i++ ) { 
            mCharacterList[i]->setNextBeat( mNextBang );
        }  
    }
    else {
        newBeat = false;
    }
    

}