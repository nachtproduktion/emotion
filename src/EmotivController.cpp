#include "EmotivController.h"

EmotivController::EmotivController() {
    
}

////// INIT //////

void EmotivController::init( int _port ) {
    
    //OSC
    mOSCListener.setup( _port );
    
}

////// GETS ////////
//PeakTimer EmotivController::getNextPeak( std::string _fq ) {
//    
//    PeakTimer pt;
//
//    bool found = false;
//    while ( found == false && mFrequenz[_fq].size() > 0) {
//        
//        time_t n = niko::getTimeMS();
//        
//        if( n < mFrequenz[_fq].front().mTime + 20 ) { 
//            pt = mFrequenz[_fq].front();
//            found = true;
//        }
//        
//        mFrequenz.erase(mFrequenz.begin());
//    } 
//    
//    return pt;
//    
//}

////// UPDATE //////
void EmotivController::update() {
    
    //OSC UPDATE
    updateOSC();
}

void EmotivController::updateOSC() {

    
    while (mOSCListener.hasWaitingMessages()) {
        
		osc::Message message;
		mOSCListener.getNextMessage(&message);
        
       // std::string adress = message.getAddress();
        //if( adress == "/start" ) {
        
        console() << "New message received" << std::endl;
		console() << "Address: " << message.getAddress() << std::endl;
		console() << "Num Arg: " << message.getNumArgs() << std::endl;
		for (int i = 0; i < message.getNumArgs(); i++) {
			console() << "-- Argument " << i << std::endl;
			console() << "---- type: " << message.getArgTypeName(i) << std::endl;
			if (message.getArgType(i) == osc::TYPE_INT32){
				try {
					console() << "------ value: "<< message.getArgAsInt32(i) << std::endl;
				}
				catch (...) {
					console() << "Exception reading argument as int32" << std::endl;
				}
				
			}else if (message.getArgType(i) == osc::TYPE_FLOAT){
				try {
					console() << "------ value: " << message.getArgAsFloat(i) << std::endl;
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