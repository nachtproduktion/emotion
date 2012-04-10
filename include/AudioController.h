#ifndef emotion_AudioController_h
#define emotion_AudioController_h

#include "OscListener.h"

#include <vector>
#include <sys/time.h>

#include "Character.h"
#include "niko_functionen.h"
#include "Structs.h"

class AudioController {
    public:
        AudioController();
    
        void init( int _port );
        void makeStructure( int _index = -1 );
        void deleteStructure();
        
        void addCharacter( Character * _character );
        void delCharacter( int _id );
        void emtpyCharacter();
    
        bool getPlay();
        PeakTimer getNextPeak( std::string _fq );
    
        void update();
        void updateOSC();
        
    
    private:
        
        bool                            mPlay;
    
        //Character
        std::vector<Character *>        mCharacterList;
    
        //OSC 
        osc::Listener                   mOSCListener;
    
    
        //AUDIO
        float                           mDelay;
        std::map< std::string, std::vector < PeakTimer > >   mFrequenz;
    
};

#endif
