#include "Character.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

Character::Character()
{
    mCenterPosition = Vec3f::zero(); //Vec3f(20,100,5);
    mRotation   = Quatf();
    
    mRadius = 100.0f;
    
    mForceTimer = false;
    mNumberOfMainPoints = 0;
    mNextBeat = niko::getTimeMS();
    
    mDrawCharacter = true;
    
}

Character::Character( ci::Vec3f _pos, float _radius, Quatf _rotation )
{
    mCenterPosition = _pos;
    mRotation   = _rotation;
    
    mRadius = _radius;
    
    mForceTimer = false;
    mNumberOfMainPoints = 0;
    mNextBeat = niko::getTimeMS();
    
    mDrawCharacter = true;

}

void Character::mkPoint(MainPoint *lastPoint, bool firstPoint) {
    
    int newPoints = getRandPointNumber();

    if( newPoints > mMainPointsLeft) { newPoints = mMainPointsLeft; }
    
    if(firstPoint && mMainPointsLeft > 1) {
        //Dafür sorgen das Startpunkt immer 2 Nachbarn hat!
        if( newPoints < 2 ) {
            newPoints = 2;
        }
        mMainPointsLeft--;
        mNumberOfMainPoints--;
    }
    
    //cout << newPoints << endl;   //DEBUG

    
    for(int i = 0; i<newPoints; i++) {
        
        if( i == 1 && firstPoint ) { 
            mMainPointsLeft++; 
            mNumberOfMainPoints++;
        }
        
        //Wenn alle Punkte aufgebraucht sind
        if( mMainPointsLeft <= 0 ) { return; }
        
        if( newPoints > i+1 && newPoints > 1) { mOpenLines = true; }
        
        //Neue Koordinaten berechnen
        ci::Vec3f randVec = Rand::randVec3f();
        randVec.normalize();
        randVec *= Rand::randFloat(30.0f,80.0f);                   // BERECHNUNG?!?!?!??!!?
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
    
    
    ballImage = gl::Texture( loadImage( loadResource( "kopf.png" ) ) );
    
    mPerlin = Perlin();
    
    mParticleController.clear();
    
    //Physics
    createPhysics();
    mPhysics.clear();
    
    mMainPoints.clear();
    
    mNumberOfMainPoints = mMainPointsLeft = _num;
    mMainPoints.reserve( _num );    
    
    //First Point
    mMainPoints.push_back( MainPoint( CENTER, &mPhysics, mNumberOfMainPoints - mMainPointsLeft ) );
    mMainPointsLeft--;
    
    mMainPoints.back().setFixed();
    mMainPoints.back().setMass(40.0f);
    
    mOpenLines = false;
    mkPoint(&mMainPoints.back(), true);

    //cout << "erstellt:" << mMainPointsLeft << endl;    //debug
    
    // POST SETTINGS
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        p->postSettings();
    }
    
    //Create EmoAttractors
    mFrustrationAtt.create(&mPhysics, mRadius, CENTER);
    mFrustrationAtt.setFrustration();
    mEngagementAtt.create(&mPhysics, mRadius, CENTER);
    mEngagementAtt.setEngagement();
    
    //create ParticleController
    createParticleController();
    
}

void Character::createParticleController() {
    
    int counter = 0;
    
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        if( p->mEndOfLine ) {
            
            mParticleController.push_back( ParticleController( ) );
            p->setParticleControllerID( counter );
            counter++;
            
        }
    }
}

void Character::createPhysics() {
    
    //	physics.verbose = true;			// dump activity to log
    mPhysics.setGravity(Vec3f(0, GRAVITY, 0));
    
    // set world dimensions, not essential, but speeds up collision
    //int width = ci::app::getWindowWidth();
    //int height = ci::app::getWindowHeight();
   // mPhysics.setWorldSize(Vec3f(-width/2, -height, -width/2), Vec3f(width/2, height, width/2));
    //mPhysics.setWorldSize(Vec3f(-mRadius, -mRadius, -mRadius), Vec3f(mRadius, mRadius, mRadius));
    mPhysics.setWorldSphere(CENTER, mRadius);
    
    mPhysics.setSectorCount(SECTOR_COUNT);
    
    //Schwerfälligkeit
    mPhysics.setDrag(0.97f);
    mPhysics.setDrag(1);		// FIXTHIS
    mPhysics.setDrag(0.9f);
    
    //Collision
    mPhysics.enableCollision();
    
}

void Character::updateAttractors() {
    
    
    
    //float littleRadius = mRadius - mRadius/3;
    
    //Attractors
    /*
    mAttractors[0] = mPhysics.makeParticle(Vec3f(-littleRadius, -littleRadius, -littleRadius));
    mAttractors[1] = mPhysics.makeParticle(Vec3f(-littleRadius, littleRadius, littleRadius));
    mAttractors[2] = mPhysics.makeParticle(Vec3f(littleRadius, -littleRadius, littleRadius));
    mAttractors[3] = mPhysics.makeParticle(Vec3f(littleRadius, littleRadius, -littleRadius));
    mAttractors[4] = mPhysics.makeParticle(Vec3f(-littleRadius, -littleRadius, littleRadius));
    mAttractors[5] = mPhysics.makeParticle(Vec3f(-littleRadius, littleRadius, -littleRadius));
    mAttractors[6] = mPhysics.makeParticle(Vec3f(littleRadius, -littleRadius, -littleRadius));
    mAttractors[7] = mPhysics.makeParticle(Vec3f(littleRadius, littleRadius, littleRadius));
    
    for( int i=0; i<8; i++ ) {
        mAttractors[i]->setRadius(5.0f)->setMass(1.0f)->setBounce(0.0f)->makeFixed();
        
        for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
            if(!p->mEndOfLine) {
             mPhysics.makeAttraction(mAttractors[i],p->getParticle(), 5.0f);
            }
        }
    }
     */
}

void Character::updateEmotions( float _frustration, float _engagement,float _meditation, float _excitement ) {
    
    static float oldFrustration = 0;
    static float oldEngagement = 0;
//    static float oldMeditation = 0;
//    static float oldExcitement = 0;
    
    if(oldFrustration != _frustration) {
        oldFrustration = _frustration;
        mFrustrationAtt.update( _frustration );
    }
   
    if(oldEngagement != _engagement) {
        oldEngagement = _engagement;
        mEngagementAtt.update( _engagement );
    }
/*    
    if(oldMeditation != _meditation) {
        oldMeditation = _meditation;
    }
    
    if(oldExcitement != _excitement) {
        oldExcitement = _excitement;
    }
*/    
    
}

void Character::addRandomForce(float _f) {
    mForceTimer = _f;
    for(int i=0; i<mPhysics.numberOfParticles(); i++) {
        Physics::Particle3D *p = mPhysics.getParticle(i);
        if(p->isFree()) p->addVelocity(Vec3f(Rand::randFloat(-_f, _f), Rand::randFloat(-_f, _f), Rand::randFloat(-_f, _f)));
    }
}

void Character::setRadius( float _r ) {
    
    if( _r != mRadius ) {
    
        float scaleFactor = _r/mRadius;
        scale(scaleFactor);
    
        mRadius = _r;
        mPhysics.setWorldSphere(CENTER, mRadius);
        
     }
}

void Character::scale( float _s ) {
    
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        
        Vec3f pointVec = p->getPosition();
        pointVec *= _s;
        p->forceTo(pointVec);

	}
    
    mFrustrationAtt.changeWorld( CENTER, mRadius );
    mEngagementAtt.changeWorld( CENTER, mRadius );
    
}

void Character::setNextBeat( time_t _bang ) {
    
    mNextBeat = _bang;
    
}

void Character::dance() {
    
    Vec3f randVec = Rand::randVec3f() * Rand::randFloat(55);
    
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        if( !p->getActive() ) {
            if( p->mEndOfLine ) {
                
                randVec *= -1;
                
                
                Vec3f tmp = randVec + p->getPosition();
                p->moveTo(tmp, mNextBeat, false);
                
            }
        }
    }
}

void Character::move(Vec3f _position, Quatf _rotation) {

    mCenterPosition = _position;
    mRotation = _rotation;
     
    
}

void Character::update() {
    
    //Tanz funktion
    dance();
    
    //Mainpoint update
    for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
        p->update();
        
        int pcID = p->getParticleControllerID();
        if(pcID > -1) {
            mParticleController[pcID].setTarget(p->getPosition());
        }
        
    }
    
    //Particle Update
    for(  std::vector<ParticleController>::iterator p = mParticleController.begin(); p != mParticleController.end(); ++p ){
        p->update();
    }
    
    //physics update
    mPhysics.update();
}


//RENAME
void Character::test() {
    
    
    //alles nach außen
    
    mDrawCharacter = !mDrawCharacter;

    
    
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
	}
    */
}

void Character::draw() {

    //glEnable(GL_DEPTH_TEST);
    //glAlphaFunc(GL_GREATER, 0.5);
    
    gl::pushMatrices();
    
        gl::translate( mCenterPosition );
        gl::rotate( mRotation );
        
   
    
        gl::pushMatrices();
    
            //Schütteln
            if(mForceTimer) {
                float translateMax = mForceTimer;
                glTranslatef(Rand::randFloat(-translateMax, translateMax), Rand::randFloat(-translateMax, translateMax), Rand::randFloat(-translateMax, translateMax));
                mForceTimer--;
            }
    
         
            gl::color(0,1,0,0.4);
        
            gl::drawStrokedCircle(CENTER.xy(), mRadius);
            //gl::drawSphere(CENTER, mRadius, 64);
            gl::color(1,0,0);
        
            if(mDrawCharacter) {
        
                for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
                           
                    if(p->mEndOfLine) {
                        gl::color(0.5,0.5,1);
                        p->draw();
                    }
                    else {
                        gl::color(1,1,0);
                        p->draw();
                    }
                
                    for ( int i = 0; i<p->mNeighbours.size(); i++) {
                        gl::color(1,0.5,0);
                        gl::drawLine(p->getPosition(), p->mNeighbours[i]->getPosition());
                        gl::color(1,1,0);
                    }
                
               
                }
            }
        
            //Draw EmoAttractors
            mFrustrationAtt.render();
            mEngagementAtt.render();
        
        
            for(  std::vector<ParticleController>::iterator p = mParticleController.begin(); p != mParticleController.end(); ++p ){
                //p->draw();
            }
 
        gl::popMatrices();

    gl::popMatrices();
    
}