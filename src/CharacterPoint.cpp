#include "CharacterPoint.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

CharacterPoint::CharacterPoint() {
    mParticleID     = -1;
    mPhysic         = NULL;
    mParticle       = NULL;
    
    mParent         = NULL;
    
    mEndOfLine     = true;
    mActive        = false;
    
    mParticleControllerID = -1;
    mStandBondID          = -1;
    
    savePosition = ci::Vec3f::zero();
    
    mPosition = ci::Vec3f::zero();
    mShellRadius = 0.0f;
}

CharacterPoint::CharacterPoint( Vec3f _pos, MSA::Physics::World3D * _physics, int _pID ) {
    
    mParticleID     = _pID;
    mPhysic         = _physics;
    mParticle       = NULL;
    
    mParent         = NULL;
    
    mEndOfLine     = true;
    mActive        = false;
    
    mParticleControllerID = -1;
    mStandBondID          = -1;
    
    savePosition = _pos;
    
    mPosition      = _pos;
    
    Physics::Particle3D* p = mPhysic->makeParticle(_pos);        
    p->setMass(4.0f)->setBounce(0.5f)->setRadius(5.0f)->enableCollision()->makeFree();
    
    setParticle();
    calcShellRadius();
    
}

Physics::Particle3D* CharacterPoint::getParticle() {
    return mParticle;
}

void CharacterPoint::setParticle() {
    mParticle = mPhysic->getParticle(mParticleID);
}

void CharacterPoint::setParent( CharacterPoint* _parent ) {
    mParent = _parent;
}

void CharacterPoint::addChild( CharacterPoint* _child ) {
    mChilds.push_back( _child );
}

void CharacterPoint::clearChilds() {
    mChilds.clear();
}

CharacterPoint* CharacterPoint::getParent() {
    return mParent;
}

CharacterPoint* CharacterPoint::getChild( int _index ) {
    
    if(getNumberOfChilds() > _index) {
        return mChilds[ _index ];
    }
    else { return NULL; }
}

int CharacterPoint::getNumberOfChilds() {
    return mChilds.size();
}

//void CharacterPoint::setNeighbours( CharacterPoint* _neighbour ) {
//    
//    mNeighbours.push_back( _neighbour );
//    
//}
//
//
//int CharacterPoint::getNeighboursSize() {
//    return mNeighbours.size();
//}

int CharacterPoint::getID() {
    return mParticleID;
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
    else { return NULL; }
}

void CharacterPoint::setStandBondID( int _id ) {
    mStandBondID = _id;
}

int CharacterPoint::getStandBondID() {
    return mStandBondID;
}

void CharacterPoint::setParticleControllerID( int _id ) {
    mParticleControllerID = _id;
}

int CharacterPoint::getParticleControllerID() {
    return mParticleControllerID;
}

void CharacterPoint::setRadius( float _r ) {
    mParticle->setRadius( _r );
    calcShellRadius();
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

Vec3f* CharacterPoint::getPositionPointer() {
    mPosition = getPosition();
    return &mPosition;
}

void CharacterPoint::setEndOfLine( bool _eol ) {
    mEndOfLine = _eol;
}


bool CharacterPoint::getEndOfLine() {
    return mEndOfLine;
}

bool CharacterPoint::getActive() {
    return mActive;
}

void CharacterPoint::calcShellRadius() {
    mShellRadius = getRadius() * 4;
}

float CharacterPoint::getShellRadius() {
    return mShellRadius;
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