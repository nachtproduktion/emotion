#include "AttractorController.h"

extern float testwert;

Perlin amPerlin( 3 );

AttractorController::AttractorController() {
    mPhysics    = NULL;    
    mDanceRotation = 0;
    mPointList  = NULL;
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

/// DANCE 
void AttractorController::makeDanceAttractors() {
    
    if( mPhysics == NULL ) { return; }
    
    mDanceAttractor.clear();
    
    for(  std::vector<CharacterPoint>::iterator p = mPointList->begin(); p != mPointList->end(); ++p ){
        
        if( p->mLevel == CPL_ROOT) {
            if( p->mFunction == CPF_MIDDLE || p->mFunction == CPF_START) {
            
                FourPoints newAttractors;
                ci::Vec3f vec = ci::Vec3f( 350.0f, 0.0f, 0.0f );

                for( int i = 0; i < 4; i++) {
                    
                    newAttractors.aps[i].create( mPhysics, vec + p->getPosition() );
                    newAttractors.point = &*p;
                    vec.rotateY(ci::toRadians(90.0f));
                    
                }
                
                mDanceAttractor.push_back( newAttractors );
            }
        }
        
    }
}

void AttractorController::calcDanceAttractors() {
    
    if( mPointList == NULL ) { return; }
    
    
    
    for( int i = 0; i < mDanceAttractor.size(); i++ ) {
        
        int frames = ci::app::getElapsedFrames();
        ci::Vec3f noise = amPerlin.dfBm( mDanceAttractor[i].point->getPosition() * 0.01f + ci::Vec3f( 0, 0, frames / 100.0f ) );
        ci::Vec3f mPerlin = noise.normalized() * 5;
        
        
        ci::Vec3f vec = ci::Vec3f( 300.0f, -10.f, 0.0f );
        
        if(mDanceAttractor[i].point->mFunction == CPF_START) {
            vec = ci::Vec3f( 300.0f, mPerlin.x * mPerlin.y * mPerlin.z, 0.0f );
        } 
        
        vec.rotateY(ci::toRadians( mDanceRotation + mPerlin.z * mPerlin.x ));
        
        for( int z = 0; z < 4; z++) {
        
            mDanceAttractor[i].aps[z].mPosition = vec + mDanceAttractor[i].point->getPosition();
            mDanceAttractor[i].aps[z].mMass = Rand::randFloat(5.0f, 10.0f);
            mDanceAttractor[i].aps[z].update();
            
            vec.rotateY( ci::toRadians(90.0f + mPerlin.z * mPerlin.y ) );
            

        }
    }
}

void AttractorController::activateDanceAttractors() {
        
    mDanceAttractions.clear();
    
    for( int i = 0; i < mDanceAttractor.size(); i++ ) {
        
        
        
        for( int z = 0; z < 4; z++ ) {
            mDanceAttractor[i].aps[z].activate();
        }
            
        for(  std::vector<CharacterPoint>::iterator p = mPointList->begin(); p != mPointList->end(); ++p ){
                
            if( p->getSuperParent() ==  mDanceAttractor[i].point ) {
                    
                int randa = Rand::randInt(4);
                
                MSA::Physics::Attraction3D* newAtt = mPhysics->makeAttraction( mDanceAttractor[i].aps[randa].mAttractor, p->getParticle(), 2.0f );
                mDanceAttractions.push_back( newAtt );
                
                
                int randb = Rand::randInt(4);
                if( randa != randb ) {
                    MSA::Physics::Attraction3D* newAtt = mPhysics->makeAttraction( mDanceAttractor[i].aps[randb].mAttractor, p->getParticle(), 2.0f );
                    mDanceAttractions.push_back( newAtt );
                }
         
            }
        }
    }
}

void AttractorController::deactivateDanceAttractors() {
    
    for(int i=0; i < mDanceAttractions.size(); i++) {
        mDanceAttractions[i]->kill();   
    }
    
    mDanceAttractions.clear();
    
    for( int i = 0; i < mDanceAttractor.size(); i++ ) {
        for( int z = 0; z < 4; z++) {
            
            mDanceAttractor[i].aps[z].deactivate();
            
        }
    }
    
}


void AttractorController::update() {
    mStartAttractor.update();
    
    if( mDanceAttractor.size() > 0 ) {
        calcDanceAttractors();
    }
    
}

void AttractorController::render() {
    
//    for( int i = 0; i < mDanceAttractor.size(); i++ ) {
//        for( int z = 0; z < 4; z++) {
//            
//            mDanceAttractor[i].aps[z].render();
//            
//        }
//    }
}