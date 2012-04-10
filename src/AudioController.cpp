#include "AudioController.h"

AudioController::AudioController() {
    
    mPlay           = false;
    mDelay          = 500;
    
}

////// INIT //////

void AudioController::init( int _port ) {
    
    //OSC
    mOSCListener.setup( _port );
    
}

/////// CHARACTER //////

void AudioController::makeStructure( int _index ) {
    
    if( _index == -1 ) {
        
        for( int i = 0; i < mCharacterList.size(); i++ ) {
            mCharacterList[ i ]->createNewStructure();
            mCharacterList[ i ]->startAnimation( niko::getTimeMS() + 200 );
        }
        
    }
    else {
        if( _index < mCharacterList.size() ) {
            mCharacterList[ _index ]->createNewStructure();
            mCharacterList[ _index ]->startAnimation( niko::getTimeMS() + 200 );
        }
    }
    
}

void AudioController::deleteStructure() {
    
    for( int i = 0; i < mCharacterList.size(); i++ ) {
        mCharacterList[ i ]->destroyStructure();
    }
    
}

void AudioController::addCharacter( Character * _character ) {
    mCharacterList.push_back( _character );
    
    if( mPlay ) {
        makeStructure( mCharacterList.size()-1 );
    }
}

void AudioController::delCharacter( int _id ) {
    // FEHLT NOCH
}

void AudioController::emtpyCharacter() {
    mCharacterList.clear();
}

////// GETS ////////

bool AudioController::getPlay() {
    return mPlay;
}

PeakTimer AudioController::getNextPeak( std::string _fq ) {
    
    PeakTimer pt;

    bool found = false;
    while ( found == false && mFrequenz[_fq].size() > 0) {
        
        time_t n = niko::getTimeMS();
        
        if( n < mFrequenz[_fq].front().mTime + 20 ) { 
            pt = mFrequenz[_fq].front();
            found = true;
        }
        
        mFrequenz.erase(mFrequenz.begin());
    } 
    
    return pt;
    
}

////// UPDATE //////
void AudioController::update() {
    
    //OSC UPDATE
    updateOSC();
    
}

void AudioController::updateOSC() {

    
    while (mOSCListener.hasWaitingMessages()) {
		osc::Message message;
		mOSCListener.getNextMessage(&message);
        
        std::string adress = message.getAddress();
        
        if( adress == "/start" ) {
            
            console() << "music start" << endl;
            
            makeStructure();
            mPlay = true;
            
        } else if( adress == "/stop" ) {
            
            console() << "music stop" << endl;
            
            deleteStructure();
            mPlay = false;
            
        } else {
            
            for (int i = 0; i < message.getNumArgs(); i++) {
                
                if (message.getArgType(i) == osc::TYPE_INT32){
                    try {
                        
                        time_t nowA = niko::getTimeMS();
                        PeakTimer newPeakA = PeakTimer(message.getArgAsInt32(i), nowA);
                        mFrequenz[adress].push_back(newPeakA);
                        
                    }
                    catch (...) {
                        console() << "Exception reading argument as int32" << std::endl;
                    }
                    
                }else if (message.getArgType(i) == osc::TYPE_FLOAT){
                    try {
                        
                        time_t nowB = niko::getTimeMS();
                        PeakTimer newPeakB = PeakTimer(message.getArgAsFloat(i), nowB);
                        mFrequenz[adress].push_back(newPeakB);
                        
                    }
                    catch (...) {
                        console() << "Exception reading argument as float" << std::endl;
                    }
                }else if (message.getArgType(i) == osc::TYPE_STRING){
                    try {
                        console() << "------ value: " << message.getArgAsString(i).c_str() << std::endl;
                    }
                    catch (...) {
                        console() << "Exception reading argument as string" << std::endl;
                    }
                    
                }
            }
        }
		
	}
    
}