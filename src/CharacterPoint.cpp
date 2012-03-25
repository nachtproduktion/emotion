#include "CharacterPoint.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

CharacterPoint::CharacterPoint() {
    mParticleID     = -1;
    mPhysic         = NULL;
    mParticle       = NULL;
    
    mEndOfLine     = true;
    mActive        = false;
    
    mParticleControllerID = -1;
    savePosition = ci::Vec3f::zero();
}

CharacterPoint::CharacterPoint( Vec3f _pos, MSA::Physics::World3D * _physics, int _pID ) {
    
    mParticleID     = _pID;
    mPhysic         = _physics;
    mParticle       = NULL;
    
    mEndOfLine     = true;
    mActive        = false;
    
    mParticleControllerID = -1;
    savePosition = _pos;
    
    Physics::Particle3D* p = mPhysic->makeParticle(_pos);        
    p->setMass(4.0f)->setBounce(0.2f)->setRadius(5.0f)->enableCollision()->makeFree();
    
    setParticle();
    
}

void CharacterPoint::postSettings() {
    
    if( getNeighboursSize() == 1 ) { 
        mEndOfLine = true;
        setMass(4.0f);
    }
    else {
        mEndOfLine = false; 
        
        if(getFixed()) { //Startpunkt
            setMass(150.0f);
            setRadius(10.0f);
            setFree();
        }
        else { 
            setMass(10.0f);
            setRadius(7.0f);
        }
        
    } 
    
}

Physics::Particle3D* CharacterPoint::getParticle() {
    return mParticle;
}

void CharacterPoint::setParticle() {
    mParticle = mPhysic->getParticle(mParticleID);
}

void CharacterPoint::setNeighbours( CharacterPoint* _neighbour ) {
    
    mNeighbours.push_back( _neighbour );
    
    //Physics::Particle3D *a = getParticle();
    //Physics::Particle3D *b = _neighbour->getParticle();
    //mPhysic->makeSpring(a, b, SPRING_STRENGTH, a->getPosition().distance(b->getPosition()));
}


int CharacterPoint::getNeighboursSize() {
    return mNeighbours.size();
}

void CharacterPoint::addBondID( int _id ) {
    mBondIDs.push_back( _id );
}

int CharacterPoint::getNumberOfBonds() {
    return mBondIDs.size();
}

int CharacterPoint::getBondID( int _index ) {
    if(getNumberOfBonds() > _index) {
        return mBondIDs[ _index ];
    }
    else { return -1; }
}

void CharacterPoint::setParticleControllerID( int _id ) {
    mParticleControllerID = _id;
}

int CharacterPoint::getParticleControllerID() {
    return mParticleControllerID;
}

void CharacterPoint::setRadius( float _r ) {
    mParticle->setRadius( _r );
}

float CharacterPoint::getRadius() {
    return mParticle->getRadius();
}

void CharacterPoint::setMass( float _m ) {
    mParticle->setMass( _m );
}

float CharacterPoint::getMass() {
    return mParticle->getMass();
}

void CharacterPoint::setFixed() {
    mParticle->makeFixed();
}

bool CharacterPoint::getFixed() {
    return mParticle->isFixed();
}

void CharacterPoint::setFree() {
    mParticle->makeFree();
}

Vec3f CharacterPoint::getPosition() {
    return mParticle->getPosition();
}

bool CharacterPoint::getEndOfLine() {
    return mEndOfLine;
}

bool CharacterPoint::getActive() {
    return mActive;
}

void CharacterPoint::moveTo( Vec3f _target ) {
    mParticle->moveTo( _target );
}
void CharacterPoint::moveBy( Vec3f _dir ) {
    mParticle->moveBy( _dir );
}

void CharacterPoint::render() {
    gl::drawSphere ( getPosition(), getRadius() );
}