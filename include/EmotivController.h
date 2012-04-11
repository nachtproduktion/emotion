#ifndef emotion_EmotivController_h
#define emotion_EmotivController_h

#include "OscListener.h"

#include <vector>
#include <sys/time.h>

#include "Character.h"
#include "niko_functionen.h"
#include "Structs.h"

class EmotivController {
    public:
        EmotivController();
    
        void init( int _port );
    
        //PeakTimer getNextPeak( std::string _fq );
    
        void update();
        void updateOSC();
        
    
    private:
    
        //OSC 
        osc::Listener                   mOSCListener;
        
    
    /*
        
        //Character
        std::vector<Character *>        mCharacterList;

        //AUDIO
        std::map< std::string, std::vector < PeakTimer > >   mFrequenz;
     */
    
};

#endif
