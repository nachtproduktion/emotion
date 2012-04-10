#include "AttractorController.h"

AttractorController::AttractorController() {
    mPhysics    = NULL;    
}

void AttractorController::init( MSA::Physics::World3D * _physics, std::vector < CharacterPoint >* _cpointList ) {
    mPhysics = _physics;
    mPointList = _cpointList;
    
    
}

void AttractorController::addStart() {
    
    if( mPhysics == NULL ) { return; }
    
    mStartAttractions.clear();
    
    mStartAttractor.create( mPhysics, Vec3f(0, 0, 0) );
    mStartAttractor.activate();
    
    if( mStartAttractor.mAttractor == NULL ) { return; }
    for(  std::vector<CharacterPoint>::iterator p = mPointList->begin(); p != mPointList->end(); ++p ){
        
        MSA::Physics::Attraction3D* newAtt = mPhysics->makeAttraction( mStartAttractor.mAttractor, p->getParticle(), 5.0f );
        mStartAttractions.push_back( newAtt );
    }
}

void AttractorController::removeStart() {
    
    for(int i=0; i < mStartAttractions.size(); i++) {
        mStartAttractions[i]->kill();   
    }

    mStartAttractions.clear();
    mStartAttractor.deactivate();
}

AttractorPoint * AttractorController::getStartAttractor() {
    return &mStartAttractor;
}

void AttractorController::update() {
    mStartAttractor.update();
}