//#pragma once
#ifndef NIKOSUPERFUNCTION
#define NIKOSUPERFUNCTION

#include "cinder/Vector.h"
#include "cinder/Rand.h"
#include "float.h"

#include <list>
#include <vector>

#include <sys/time.h>

using namespace ci;
using namespace std;

namespace niko {
 
    inline float mapping(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp = false) {
        
        if (fabs(inputMin - inputMax) < FLT_EPSILON){
            return outputMin;
        } else {
            float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
            
            if( clamp ){
                if(outputMax < outputMin){
                    if( outVal < outputMax )outVal = outputMax;
                    else if( outVal > outputMin )outVal = outputMin;
                }else{
                    if( outVal > outputMax )outVal = outputMax;
                    else if( outVal < outputMin )outVal = outputMin;
                }
            }
            return outVal;
        }
        
    }

    inline time_t getTimeMS() {
        
        timeval time;
        gettimeofday(&time, NULL);
        time_t msec = (time.tv_sec * 1000) + (time.tv_usec / 1000);
        
        return msec;
    }
    
    template<typename T, int size>
    inline int getArrLength(T(&)[size]){ return size; }

}

#endif