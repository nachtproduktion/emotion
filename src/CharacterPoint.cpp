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
    
    mPosition   = ci::Vec3f::zero();
    mAnimPos    = mPosition;
    
    mShellRadius = 0.0f;
    
    mSpacers.clear();
    mChilds.clear();
    mBondIDs.clear();
    
    mSuperParent = NULL;
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
    
    savePosition    = _pos;
    
    mPosition       = _pos;
    mAnimPos        = mPosition;
    
    mSpacers.clear();
    mChilds.clear();
    mBondIDs.clear();
    
    Physics::Particle3D* p = mPhysic->makeParticle(_pos);        
    p->setMass(4.0f)->setBounce(0.5f)->setRadius(5.0f)->enableCollision()->makeFree();
    
    setParticle();
    calcShellRadius();
    
    mSuperParent = NULL;
    
}

///////////////////////////////////
//PARTICLES
Physics::Particle3D* CharacterPoint::getParticle() {
    return mParticle;
}

void CharacterPoint::setParticle() {
    mParticle = mPhysic->getParticle(mParticleID);
}

///////////////////////////////////
//CHILDS AND PARENTS

void CharacterPoint::setSuperParent( CharacterPoint* _superparent ) {
    mSuperParent = _superparent;
}

CharacterPoint* CharacterPoint::getSuperParent() {
    return mSuperParent;
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

///////////////////////////////////
//SPACER
void CharacterPoint::addSpacer( CharacterPoint* _spacer ){
    mSpacers.push_back( _spacer );
}

void CharacterPoint::clearSpacers(){
    mSpacers.clear();
}

CharacterPoint* CharacterPoint::getSpacer( int _index ){
    if(mSpacers.size() > _index) {
        return mSpacers[ _index ]; 
    }
    else { return NULL; }
}

int CharacterPoint::getNumberOfSpacers() {
    return mSpacers.size();
}

/////////////////////////////////////
int CharacterPoint::getID() {
    return mParticleID;
}

///////////////////////////////////
//BONDS
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

///////////////////////////////////
//PARTICLES
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

///////////////////////////////////
//SETTINGS
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
    mShellRadius = getRadius() * 3;
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

void CharacterPoint::addAnimToPosition() {
    Vec3f newPos = mParticle->getPosition() + mAnimPos;
    mParticle->moveTo( newPos );
}

void CharacterPoint::posToSave() {
    savePosition = mParticle->getPosition();
}

void CharacterPoint::render() {
    gl::drawSphere ( getPosition(), getRadius() );   
}