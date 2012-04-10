
#ifndef emotion_Structs_h
#define emotion_Structs_h

// FOR AUDIOCONTROLLER
struct PeakTimer {
    
    PeakTimer() {
        mTime = 0;
        mPeak = 0;
    }
    
    PeakTimer( float _peak, time_t _time ) {
        mTime = _time;
        mPeak = _peak;
    }
    
    time_t  mTime;
    float   mPeak;
};



#endif
