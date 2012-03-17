#include "Character.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"

using namespace ci;


Character::Character()
{
    mCenterPos = Vec3f::zero();
    mRadius = 100.0f;
    
    mForceTimer = false;
    mNumberOfMainPoints = 0;

}

Character::Character( ci::Vec3f _pos, float _radius )
{
    mCenterPos = _pos;
    mRadius = _radius;
    
    mForceTimer = false;
    mNumberOfMainPoints = 0;

}

void Character::mkPoint(MainPoint *lastPoint) {
    
    int newPoints = getRandPointNumber();
        
    //cout << newPoints << endl;   //DEBUG
    
    if( newPoints > mMainPointsLeft) { newPoints = mMainPointsLeft; }
    
    for(int i = 0; i<newPoints; i++) {
        
        //Wenn alle Punkte aufgebraucht sind
        if( mMainPointsLeft <= 0 ) { return; }
        
        if( newPoints > i+1 && newPoints > 1) { mOpenLines = true; }
        
        //Neue Koordinaten berechnen
        ci::Vec3f randVec = Rand::randVec3f();
        randVec.normalize();
        randVec *= Rand::randFloat(30.0f,100.0f);                   // BERECHNUNG?!?!?!??!!?
        randVec += lastPoint->getPosition();
        
        //randVec *= Rand::randFloat(mRadius);
        //randVec += mCenterPos;
        
        mMainPoints.push_back( MainPoint( randVec, &mPhysics, mNumberOfMainPoints - mMainPointsLeft ) );
        mMainPointsLeft--;
        
        //lastPoint->mNeighbours.push_back( &mMainPoints.back() );
        lastPoint->setNeighbours( &mMainPoints.back() );
        
        mkPoint( &mMainPoints.back() );
    }
    
    mOpenLines = false;
    
}

int Character::getRandPointNumber() {
    
    int randomInt = 0;
    
    if( mOpenLines ) {
        randomInt = Rand::randInt(100);
    } else {
        randomInt = Rand::randInt(60,100);
    }
    
    // 00 - 60; 60% NewPoints: 0
    // 60 - 72; 12% NewPoints: 1
    // 72 - 84; 12% NewPoints: 2
    // 84 - 94; 10% NewPoints: 3
    // 94 - 100; 6% NewPoints: 4
    
    if( randomInt < 60 ) {
        return 0;
    } else if( randomInt < 72 ) {
        return 1;
    } else if( randomInt < 84 ) {
        return 2;
    } else if( randomInt < 94 ) {
        return 3;
    } else {
        return 4;
    }
    
}

void Character::createNewStructure(int _num) {
    
    mPerlin = Perlin();
    
    //Physics
    createPhysics();
    mPhysics.clear();
    
    mMainPoints.clear();
    
    mNumberOfMainPoints = mMainPointsLeft = _num;
    mMainPoints.reserve( _num );    
    
    //First Point
    ci::Vec3f randVec = Rand::randVec3f();
    randVec.normalize();
    randVec *= Rand::randFloat(mRadius);
    randVec += mCenterPos;
    
    mMainPoints.push_back( MainPoint( randVec, &mPhysics, mNumberOfMainPoints - mMainPointsLeft ) );
    mMainPointsLeft--;
    
    mOpenLines = false;
    mkPoint(&mMainPoints.back());

    //cout << "erstellt:" << mMainPointsLeft << endl;    //debug
    
    // POST SETTINGS
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        p->postSettings();
    }
    
}

void Character::createPhysics() {
    
    //	physics.verbose = true;			// dump activity to log
    mPhysics.setGravity(Vec3f(0, GRAVITY, 0));
    
    // set world dimensions, not essential, but speeds up collision
    //int width = ci::app::getWindowWidth();
    //int height = ci::app::getWindowHeight();
   // mPhysics.setWorldSize(Vec3f(-width/2, -height, -width/2), Vec3f(width/2, height, width/2));
    mPhysics.setWorldSize(Vec3f(-mRadius, -mRadius, -mRadius), Vec3f(mRadius, mRadius, mRadius));
    //mPhysics.addConstraint(<#ConstraintT<cinder::Vec3<float> > *c#>);
    
    mPhysics.setSectorCount(SECTOR_COUNT);
    
    //Schwerfälligkeit
    mPhysics.setDrag(0.97f);
    mPhysics.setDrag(1);		// FIXTHIS
    mPhysics.setDrag(0.9f);
    
    //Collision
    mPhysics.enableCollision();
        
}

void Character::addRandomForce(float _f) {
    mForceTimer = _f;
    for(int i=0; i<mPhysics.numberOfParticles(); i++) {
        Physics::Particle3D *p = mPhysics.getParticle(i);
        if(p->isFree()) p->addVelocity(Vec3f(Rand::randFloat(-_f, _f), Rand::randFloat(-_f, _f), Rand::randFloat(-_f, _f)));
    }
}

void Character::setRadius(float _r) {
    
    if( _r != mRadius ) {
    
        float scaleFactor = _r/mRadius;
        scale(scaleFactor);
    
        mRadius = _r;
        mPhysics.setWorldSize(Vec3f(-mRadius, -mRadius, -mRadius), Vec3f(mRadius, mRadius, mRadius));
        
     }
}

void Character::scale(float _s) {
    
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        
        Vec3f pointVec = p->getPosition();
        pointVec *= _s;
        p->forceTo(pointVec);

	}
}

void Character::dance( int _t ) {
    
    if(mNumberOfMainPoints > 0) {
        
        Vec3f randVec = Rand::randVec3f() * Rand::randFloat(mRadius);
        
        //mMainPoints[0].moveTo(_mousePos);
        mMainPoints[0].moveTo(randVec, _t, false);
    }
    
    /*
    Vec3f randVec = Rand::randVec3f() * 10;
    
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
            
      
        if(p->mEndOfLine) {
            
            
            Vec3f pointVec = randVec + p->getPosition();
            cout << pointVec << endl;
            
            p->moveTo(pointVec);
            
        }
      
	}
    
 */
}

void Character::moveTo(Vec2f _mousePos) {

    
    
    if(mNumberOfMainPoints > 0) {
       
        //mMainPoints[0].moveTo(_mousePos);
        mMainPoints[0].moveTo(Vec3f(0,0,0), 2000, true);
    }
     
    
}

void Character::update() {
    
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        p->update();
    }
    
    mPhysics.update();
}

//RENAME
void Character::test() {

    //dance();
    
    /*
    // loop through all particles and add attraction to mouse
    // (doesn't matter if we attach attraction from mouse-mouse cos it won't be added internally
    for(int i=0; i<physics.numberOfParticles(); i++) {
        physics.makeAttraction(&mouseNode, physics.getParticle(i), Rand::randFloat(MIN_ATTRACTION, MAX_ATTRACTION));
    }
    else {
        // loop through all existing attractsions and delete them
        for(int i=0; i<physics.numberOfAttractions(); i++) physics.getAttraction(i)->kill();
    }
    */
    
    
    /*
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        
        if(p->mEndOfLine) {
            
            Vec3f pointVec = p->getPosition();
            pointVec.normalize();
            pointVec *= mRadius;
            
            p->moveTo(pointVec);
            
        }
	}*/
    
}

void Character::draw() {

    glEnable(GL_DEPTH_TEST);
    
    glPushMatrix();
    
    //Schütteln
    if(mForceTimer) {
        float translateMax = mForceTimer;
        glTranslatef(Rand::randFloat(-translateMax, translateMax), Rand::randFloat(-translateMax, translateMax), Rand::randFloat(-translateMax, translateMax));
        mForceTimer--;
    }
    
    gl::enableAlphaBlending();
    gl::color(0,1,0,0.4);
    gl::drawStrokedCircle(mCenterPos.xy(), mRadius);
    gl::disableAlphaBlending();
    
    gl::color(1,0,0);
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        
        if(p->mEndOfLine) {
            gl::color(0.5,0.5,1);
        }
        else {
            gl::color(1,1,0);
        }
        p->draw();
        
        
        
        
        for ( int i = 0; i<p->mNeighbours.size(); i++) {
            gl::color(1,0.5,0);
            gl::drawLine(p->getPosition(), p->mNeighbours[i]->getPosition());
            gl::color(1,1,0);
        }
       
	}
    
    
    glPopMatrix();
    
    
}