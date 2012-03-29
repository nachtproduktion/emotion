#include "Character.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/BSpline.h"

#include <math.h>

using namespace ci;
using namespace ci::app;
using namespace std;

Character::Character()
{
    mCenterPosition = Vec3f::zero(); //Vec3f(20,100,5);
    mRotation   = Quatf();
    
    mRadius = 100.0f;
    mMaxLevels = 0;
    
    mForceTimer = false;
    mNumberOfCharacterPoints = 0;
    mNextBeat = niko::getTimeMS();
    
    mDrawCharacter = true;
    
}

Character::Character( ci::Vec3f _pos, float _radius, Quatf _rotation )
{
    mCenterPosition = _pos;
    mRotation   = _rotation;
    
    mRadius = _radius;
    mMaxLevels = 0;
    
    mForceTimer = false;
    mNumberOfCharacterPoints = 0;
    mNextBeat = niko::getTimeMS();
    
    mDrawCharacter = true;

}

//////////////////////////////////////////////
// MAKE STRUCTURE ////////////////////////////
//////////////////////////////////////////////

void Character::createNewStructure(int _num) {
    
    
    //Clear all Objects
    mParticleController.clear();
    mBonds.clear();
    mCharacterPoints.clear();
    
    //Physics
    createPhysics();
    mPhysics.clear();
    
    mNumberOfCharacterPoints = mCharacterPointsLeft = _num;
    mCharacterPoints.reserve( _num );    
    
    //First Point
    mCharacterPoints.push_back( CharacterPoint( CENTER, &mPhysics, mCharacterPoints.size() ) );
    mCharacterPointsLeft--;
    
    //mCharacterPoints.back().setFixed();
    //mCharacterPoints.back().setMass(40.0f);
    
    //mOpenLines = false;
    
    mkPoint(&mCharacterPoints.back(), 1);
    
    //cout << "erstellt:" << mCharacterPointsLeft << endl;    //debug

    
    // POST SETTINGS
    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
        p->postSettings();
        
        if(p->getEndOfLine()) {
            mBonds[p->getBondID(0)].mLevel = 1;
        }
        
    }
    
    //Create Charactermovement
    mMovement.setup( &mCharacterPoints, &mBonds );
    
    //Create EmoAttractors
    mFrustrationAtt.create(&mPhysics, mRadius, CENTER);
    mFrustrationAtt.setFrustration();
    mEngagementAtt.create(&mPhysics, mRadius, CENTER);
    mEngagementAtt.setEngagement();
    
    //create pathes for splines
    createSplinesB();
    
    //create ParticleController
    updateSplines();
    createParticleController();
}

void Character::mkPoint(CharacterPoint *lastPoint, int _level) {
    
    if( mMaxLevels < _level ) mMaxLevels = _level;
    
    int newPoints = 1;
    if( _level == 1 ) {
        if( mCharacterPointsLeft > 3 ) {
            newPoints = getRandPointNumber( 2, floor(mCharacterPointsLeft/2) );
            mCharacterPointsLeft -= (newPoints*2)-1;
        }
        else if( mCharacterPointsLeft > 0 ) { 
            newPoints = 1;
            mCharacterPointsLeft -= newPoints;
        } else {
            newPoints = 0;
        }
    } else if ( _level == 2 ) {
        newPoints = getRandPointNumber( 1, floor(mCharacterPointsLeft) );
        mCharacterPointsLeft -= newPoints;
    }
    else {
        if(mOpenLines) {
            newPoints = getRandPointNumber( 0, floor(mCharacterPointsLeft) );
        }
        else {
            newPoints = getRandPointNumber( 1, floor(mCharacterPointsLeft) );
        }
        mCharacterPointsLeft -= newPoints;
    }
       
    
    
    for(int i = 0; i<newPoints; i++) {
        
        if ( _level == 1 && i > 0 ) {
            mCharacterPointsLeft++;
        }
        
        //cout << "i:" << i << " level: " << _level << " left:" << mCharacterPointsLeft << " newP: " << newPoints << endl;   //DEBUG
        
        //Wenn alle Punkte aufgebraucht sind
        //if( mCharacterPointsLeft <= 0 ) { return; }
        
        if( newPoints > i+1 && newPoints > 1) { mOpenLines = true; }
        
        //Neue Koordinaten berechnen
        ci::Vec3f randVec = Rand::randVec3f();
        randVec.normalize();
        randVec *= Rand::randFloat(30.0f,80.0f);                   // BERECHNUNG?!?!?!??!!?
        randVec += lastPoint->getPosition();
        
        //randVec *= Rand::randFloat(mRadius);
        //randVec += mCenterPos;
        
        mCharacterPoints.push_back( CharacterPoint( randVec, &mPhysics, mCharacterPoints.size() ) );
        
        //set Parent + Level
        mCharacterPoints.back().setParent( lastPoint );
        mCharacterPoints.back().mLevel = _level;
        
        //add Child to Parent
        lastPoint->addChild( &mCharacterPoints.back() );

        //set bonds
        mBonds.push_back( Bond( lastPoint , &mCharacterPoints.back() ) );
        mBonds.back().makeBond( &mPhysics );
        
        mCharacterPoints.back().addBondID( mBonds.size()-1 );
        lastPoint->addBondID( mBonds.size()-1 );
        
        mkPoint( &mCharacterPoints.back(), _level + 1 );           
 
    }
    
    mOpenLines = false;
    
}

int Character::getRandPointNumber( int _min, int _max ) {
    
    int max = _max + 1; 
    if ( max > 5 ) { max = 5; }

    int min = _min;
    if ( min >= max ) { min = max - 1; }
    
    int percent[6] = { 0, 60, 72, 84, 94, 100 };
    
    int randomInt = Rand::randInt(percent[ min ], percent[ max ]);
    
    // 00 - 60; 60% NewPoints: 0
    // 60 - 72; 12% NewPoints: 1
    // 72 - 84; 12% NewPoints: 2
    // 84 - 94; 10% NewPoints: 3
    // 94 - 100; 6% NewPoints: 4
    
    if( randomInt < percent[1] ) {
        return 0;
    } else if( randomInt < percent[2] ) {
        return 1;
    } else if( randomInt < percent[3] ) {
        return 2;
    } else if( randomInt < percent[4] ) {
        return 3;
    } else {
        return 4;
    }
    
}

//////////////////////////////////////////////
// CREATE FUNCTIONS //////////////////////////
//////////////////////////////////////////////

void Character::createPhysics() {
    
    //physics.verbose = true;			// dump activity to log
    mPhysics.setGravity(Vec3f(0, GRAVITY, 0));
    mPhysics.setWorldSphere(CENTER, mRadius);
    mPhysics.setSectorCount(SECTOR_COUNT);
    
    //Schwerfälligkeit
    mPhysics.setDrag(0.97f);
    mPhysics.setDrag(1);		// FIXTHIS
    mPhysics.setDrag(0.9f);
    
    //Collision
    mPhysics.enableCollision();
    
}

void Character::createParticleController() {
    
    int numSegments = 20;
    for(  std::vector<BSpline3f>::iterator p = mSplines.begin(); p != mSplines.end(); ++p ){                
        for( int s = 0; s <= numSegments; ++s ) {
            mParticleController.push_back( ParticleController( ) );
        }
    }
    
    /* for ends
     
    int counter = 0;
     
    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){  
        if( p->getEndOfLine() ) {
            
            mParticleController.push_back( ParticleController( ) );
            p->setParticleControllerID( counter );
            counter++;
            
        }
    }
     */
}

void Character::createSplinesA() {
    
    mSplines.clear();
    
    for( int z = 0; z < mCharacterPoints.size(); z++ ) {
        
        if(mCharacterPoints[z].getEndOfLine()) {
            
            int pointlevel = mCharacterPoints[z].mLevel;
            
            // Pointer Variante
            //Vec3f * points[ pointlevel + 1 ];
            //points[mCharacterPoints[z].mLevel] = mCharacterPoints[z].getPositionPointer();
            
            vector<Vec3f> points;
            points.push_back( mCharacterPoints[z].getPosition() );
            
            CharacterPoint* lastPoint = &mCharacterPoints[z];
            for( int i = pointlevel - 1; i >= 0 ; i-- ) {
                
                lastPoint = lastPoint->getParent();
                
                // Pointer Variante
                //points[i] = lastPoint->getPositionPointer();
                
                points.push_back( lastPoint->getPosition() );
            }
             
            //Spline( int numControlPoints, const T *controlPoints, int degree, bool loop, const float *knots );
            BSpline3f newSpline = BSpline3f(points, pointlevel, false, true);
            mSplines.push_back( newSpline );
            
        }
	}   
}

void Character::createSplinesB() {
        
    //Vector Init
    mPaths.clear();
    
    for( int z = 0; z < mCharacterPoints.size(); z++ ) {
        
        if(mCharacterPoints[z].getEndOfLine()) {
            
            int pointlevel = mCharacterPoints[z].mLevel;
          
            std::vector<CharacterPoint*> points;
            points.push_back( &mCharacterPoints[z] );
            
            CharacterPoint* lastPoint = &mCharacterPoints[z];
                
            //do magic
            for( int i = 0; i < pointlevel; i++ ) {
                pathFinder( lastPoint, -1,points );
                lastPoint = lastPoint->getParent();
                points.push_back( lastPoint );
            }
        }
	}  
}

void Character::pathFinder( CharacterPoint *_lastPoint, int _childID, std::vector<CharacterPoint*> _path ) {
    
    CharacterPoint* curPoint = NULL;
    
    if( _childID == -1 ) {
        curPoint = _lastPoint->getParent();
    }
    else {
        curPoint = _lastPoint->getChild(_childID);
    }
    
    //Position setzen
    std::vector<CharacterPoint*> points = _path;
    points.push_back( curPoint );
    
    int numberOfChilds = curPoint->getNumberOfChilds();
    if( numberOfChilds == 0 ) {
        //Ende gefunden
        //Path Vector umkopieren
        bool found = false;
        for( int h = 0; h < mPaths.size(); h++ ) {
            if( mPaths[h].front() == points.back() && mPaths[h].back() == points.front() ) {
                found = true;
            }
        }
        
        if( !found ) {
            mPaths.push_back( points );
        }
    }
    
    
    for( int n = 0; n < numberOfChilds; n++ ) {
        if( _lastPoint == curPoint->getChild(n) ) { continue; }
        pathFinder( curPoint, n, points);
    }
}


int Character::countEnds() {
    int counter = 0;
    for( int z = 0; z < mCharacterPoints.size(); z++ ) {
        if(mCharacterPoints[z].getEndOfLine()) {
            counter++;
        }
    }
    return counter;
}

//////////////////////////////////////////////
// SET FUNCTIONS /////////////////////////////
//////////////////////////////////////////////


void Character::setRadius( float _r ) {
    
    if( _r != mRadius ) {
    
        float scaleFactor = _r/mRadius;
        scale(scaleFactor);
    
        mRadius = _r;
        mPhysics.setWorldSphere(CENTER, mRadius);
        
     }
}

void Character::scale( float _s ) {
     
    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
        
        Vec3f pointVec = p->getPosition();
        pointVec *= _s;
        p->moveTo(pointVec);

	}
    
    for(  std::vector<Bond>::iterator p = mBonds.begin(); p != mBonds.end(); ++p ){
        p->setBondLength( p->getBondLength() * _s );
        p->mSaveDistanceA *= _s;    
    } 
    
    mFrustrationAtt.changeWorld( CENTER, mRadius );
    mEngagementAtt.changeWorld( CENTER, mRadius );
    
}

void Character::setNextBeat( time_t _bang ) {
    
    mNextBeat = _bang;
    
}

void Character::dance() {
    
    /*
    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
        if( !p->getActive() ) {
            if( p->getEndOfLine() ) {
                
                Vec3f neighboursPos = p->mNeighbours[0]->getPosition();
                Vec3f particlePos = p->getPosition();
                float dist = particlePos.distance(neighboursPos);
                
                Vec3f randVec = (neighboursPos + Rand::randVec3f()) * dist;
                
                //p->moveTo(randVec, mNextBeat, false);
                
            }
        }
    }
     */
}

void Character::wince( int _amount ) {
    mMovement.wince( _amount );
}

void Character::jump( int _amount ) {
    mMovement.jump( niko::getTimeMS() + 2000, _amount );
}

void Character::center() {
    mMovement.moveToCenter();
}

void Character::sphere() {
    mMovement.moveOnSphere();
}


void Character::move(Vec3f _position, Quatf _rotation) {

    mCenterPosition = _position;
    mRotation = _rotation;
     
    
}

void Character::addRandomForce(float _f) {
    mForceTimer = _f;
    for(int i=0; i<mPhysics.numberOfParticles(); i++) {
        Physics::Particle3D *p = mPhysics.getParticle(i);
        if(p->isFree()) p->addVelocity(Vec3f(Rand::randFloat(-_f, _f), Rand::randFloat(-_f, _f), Rand::randFloat(-_f, _f)));
    }
}

//RENAME
void Character::test() {
    
    static int count = 0;
    
    
    if(count == 0) {
        for(  std::vector<Bond>::iterator p = mBonds.begin(); p != mBonds.end(); ++p ){ 
            p->turnOff();
        }   
        count = 1;
    }
    else {
        for(  std::vector<Bond>::iterator p = mBonds.begin(); p != mBonds.end(); ++p ){ 
            p->turnOn();
        }   
        count = 0;
    }
    
}

//////////////////////////////////////////////
// UPDATE FUNCTIONS //////////////////////////
//////////////////////////////////////////////

void Character::updateSplines() {
    mSplines.clear();
    
    //make Splines
    for( int i = 0; i < mPaths.size(); i++) {
        
        vector<Vec3f> newPoints;
        for( int u = 0; u < mPaths[i].size(); u++ ) {
            newPoints.push_back( mPaths[i][u]->getPosition() );
        }
        
        BSpline3f newSpline = BSpline3f(newPoints, mPaths[i].size()-1, false, true);
        mSplines.push_back( newSpline );     
    }
    
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

void Character::update() {
        
    //Movement Update
    mMovement.update();
    
    if(!mDrawCharacter) {
        //Splines Update
        updateSplines();

        //Particle Spline
        int numSegments = 20;
        int counter = 0;
        for(  std::vector<BSpline3f>::iterator p = mSplines.begin(); p != mSplines.end(); ++p ){                
            for( int s = 0; s <= numSegments; ++s ) {
                float t = s / (float)numSegments;
                mParticleController[counter].setTarget( p->getPosition( t ) );
                counter++;
            }
        }
    }
    
    
    //Mainpoint update
    /*
    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
       
        int pcID = p->getParticleControllerID();
        if(pcID > -1) {
            mParticleController[pcID].setTarget(p->getPosition());
        }
    }
    */
    
    //Particle Update
    for(  std::vector<ParticleController>::iterator p = mParticleController.begin(); p != mParticleController.end(); ++p ){
        p->update();
    }
    
    //physics update
    mPhysics.update();
}

//////////////////////////////////////////////
// RENDER FUNCTIONS //////////////////////////
//////////////////////////////////////////////

void Character::draw() {
    
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
            gl::enableWireframe();
            gl::drawStrokedCircle(CENTER.xy(), mRadius);
            //gl::drawSphere(CENTER, mRadius, 64);
            gl::disableWireframe();
            
            gl::color(1,0,0);
        
            if(mDrawCharacter) {
        
                for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
                           
                    if(p->getEndOfLine()) {
                        gl::color(0.5,0.5,1);
                        p->render();
                    }
                    else {
                        gl::color(1,1,0);
                        p->render();
                    }
                }
            }
    
            //Draw Bonds
            if(mDrawCharacter) {
                for(  std::vector<Bond>::iterator p = mBonds.begin(); p != mBonds.end(); ++p ){
                    p->render();
                }   
            }
    
            //Draw EmoAttractors
            //mFrustrationAtt.render();
            //mEngagementAtt.render();
        
            //DRAW SPLINES
            if(!mDrawCharacter) {             
                
                
                
                const int numSegments = 100;
                gl::color( ColorA( 0.8f, 0.2f, 0.8f, 0.5f ) );
                glLineWidth( 2.0f );
                /*
                glBegin( GL_LINE_LOOP );
                for( int s = 0; s <= numSegments; ++s ) {
                    float t = s / (float)numSegments;
                    
                    
                    
                    
                    
                    for(  std::vector<BSpline3f>::iterator p = mSplines.begin(); p != mSplines.end(); ++p ){ 
                    
                        cout << mSplines.size() << endl; 
                        gl::vertex( p->getPosition( t ) + Vec3f( 0.0f, 0.5f, 0.0f ) );
                    
                    }
                    
                    
                }
                 glEnd();
                 */
                
                
                for(  std::vector<BSpline3f>::iterator p = mSplines.begin(); p != mSplines.end(); ++p ){                
                    glBegin( GL_LINE_STRIP );
                    for( int s = 0; s <= numSegments; ++s ) {
                        float t = s / (float)numSegments;
                        gl::vertex( p->getPosition( t ) + Vec3f( 0.0f, 0.5f, 0.0f ) );
                    }
                    glEnd();
                }
                 
                 
                //PARTICLE
                for(  std::vector<ParticleController>::iterator p = mParticleController.begin(); p != mParticleController.end(); ++p ){
                    //p->draw();
                }
            }
 
        gl::popMatrices();

    gl::popMatrices();
    
}

