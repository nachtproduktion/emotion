//
//  BeatController.h
//  emotion
//
//  Created by Nikolas on 15.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef emotion_BeatController_h
#define emotion_BeatController_h

#include "Character.h"
#include <vector>
#include "niko_functionen.h"
#include <sys/time.h>

class BeatController {
    public:
        BeatController();
        void setBPM( int _bpm );
        void calcTimeOffset();
        void calcNextBeat();
        void addCharacter( Character * _character );
        void delCharacter( int _id );
        void emtpyCharacter();
        void bang();
        void update();
    
        int mBPM;
        int mTimeOffset;
        //int mFrameOffset;
        int mCalcFrameRate;
        int mBangFrame;
        time_t mBangTime;
        time_t mNextBang;
        std::vector<Character *>  mCharacterList;
        bool newBeat;   

};

#endif
