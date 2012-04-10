#include "Character.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/BSpline.h"
#include "Resources.h"
#include <math.h>

using namespace ci;
using namespace ci::app;
using namespace std;

Character::Character()
{
    mCenterPosition     = Vec3f::zero();
    mRotation           = Quatf();
    
    mRadius             = 100.0f;
    mMaxLevels          = 0;
    
    mForceTimer         = false;
    mNumberOfRootPoints = 0;
    
    mDrawCharacter      = true;
    mAlive              = false;

    
}

Character::Character( ci::Vec3f _pos, float _radius, Quatf _rotation )
{
    mCenterPosition     = _pos;
    mRotation           = _rotation;
    
    mRadius             = _radius;
    mMaxLevels          = 0;
    
    mForceTimer         = false;
    mNumberOfRootPoints = 0;
    
    mDrawCharacter      = true;
    mAlive              = false;

}

//////////////////////////////////////////////
// MAKE STRUCTURE ////////////////////////////
//////////////////////////////////////////////

void Character::createNewStructure( int _num ) {
    
    
    //Clear all Objects
    mParticleController.clear();
    mBonds.clear();
    mCharacterPoints.clear();
    mCharacterSplines.clear();
    
    //Physics
    createPhysics();
    mPhysics.clear();
    
    mNumberOfRootPoints = _num;
    
    createCharacter();
    
    //create pathes for splines
    createBackbone();   //set mBackbone
    createSplinesA();
    //createSplinesB();
    
    
    //Create Charactermovement
    mMovement.setup( &mCharacterPoints, &mBonds );
    mMovement.setStandBond( &mStandBonds );
    mMovement.setBackbone( &mBackbone );
    mMovement.setPhysics( &mPhysics );
    mMovement.initAttractorController();
    mMovement.initStandUp();
    
    //Create EmoAttractors
    mFrustrationAtt.create(&mPhysics, mRadius,  CENTER_POS);
    mFrustrationAtt.setFrustration();
    mEngagementAtt.create(&mPhysics, mRadius, CENTER_POS);
    mEngagementAtt.setEngagement();
    
    //Textures
    mParticleTexture	= gl::Texture( loadImage( loadResource( RES_PARTICLE ) ) );
    
    mAlive = true;
}

void Character::createCharacter() {
    
    int minRootPoints = 2; 
    int maxRootPoints = mNumberOfRootPoints;
    
    int rootPoints = Rand::randInt(minRootPoints, maxRootPoints);
    mNumberOfRootPoints = rootPoints;
    
    //Character Point Level
    // 0 - Root
    // 1 - Joint
    // 2 - Feeler
    
    //Character Point Function
    //
    // Root:
    // 0 - Middel
    // 1 - Start
    // 2 - End
    //
    // Feeler:
    // 0 - Normal
    // 1 - Stand
    
    
    //CharacterVec
    int PointCounter = rootPoints;
    
    std::vector < std::vector < int > > CharacterVector;
    for( int a = 0; a < rootPoints; a++ ) {
        
        int jointPoints = getRandPointNumber( 1, 3 );
        std::vector <int> jointFeelerSize;
        
        for( int b = 0; b < jointPoints; b++ ) {
            
            int feelerPoints = getRandPointNumber( 0, 5 );
            if( a == 0 || a == rootPoints-1 ) {
               feelerPoints = getRandPointNumber( 1, 5 ); 
            } else {
                //Spacer
                PointCounter++;
            }
            
            jointFeelerSize.push_back( feelerPoints );
            
            PointCounter += feelerPoints;
            
        } //for b
        
        CharacterVector.push_back( jointFeelerSize );
        PointCounter += jointPoints;
        
    } //for a
    
    //Set Charactervector Size
    mCharacterPoints.reserve( PointCounter );
    
    
    //BUILD CHARACTER
    CharacterPoint *pRootPoint = NULL;
    ci::Vec3f position = CENTER_POS;
    
    //backbone
    CharacterPoint * pBackboneTop = NULL;
    CharacterPoint * pBackboneBottom = NULL;
    
    //ROOT POINTS
    for( int a = 0; a < CharacterVector.size(); a++ ) {
        
        mCharacterPoints.push_back( CharacterPoint( position, &mPhysics, mCharacterPoints.size() ) );
        mCharacterPoints.back().mLevel = CPL_ROOT;
        
        if( a == 0 ) { 
            mCharacterPoints.back().mFunction = CPF_END;
            pBackboneBottom = &mCharacterPoints.back();
        }
        else if( a == CharacterVector.size() - 1 )  { 
            mCharacterPoints.back().mFunction = CPF_START; 
            pBackboneTop = &mCharacterPoints.back();
        }
        else { mCharacterPoints.back().mFunction = CPF_MIDDLE; }
        
        
        if(a > 0) {
            setRelations( pRootPoint , &mCharacterPoints.back() );
            setBonds( pRootPoint , &mCharacterPoints.back() );
        }
        
        pRootPoint = &mCharacterPoints.back();
        
        //Settings
        
        setProperties(pRootPoint, CPL_ROOT);
        
        CharacterPoint *pJointPoint = NULL;
        
        int jointDistance = Rand::randFloat(30.0f,80.0f);
        
        //JOINT POINTS
        for( int b = 0; b < CharacterVector[a].size(); b++ ) {
            
            position = pRootPoint->getPosition();
            
            CharacterPoint * pSpacerPoint = pRootPoint;
            
            float angle = (TWOPI / CharacterVector[a].size()) * b;
            
            //SPACER POINT
            if( pRootPoint->mFunction == CPF_MIDDLE ) {
                float radius = pRootPoint->getShellRadius();
                position += ci::Vec3f( cos( angle )*radius, 0, sin( angle )*radius );
                
                mCharacterPoints.push_back( CharacterPoint( position, &mPhysics, mCharacterPoints.size() ) );
                mCharacterPoints.back().mLevel = CPL_ROOT;
                mCharacterPoints.back().mFunction = CPF_SPACER;
                
                pSpacerPoint = &mCharacterPoints.back();
                
                setRelations( pRootPoint , pSpacerPoint );
                setBonds( pRootPoint , pSpacerPoint );
                
                //SPACER
                pRootPoint->addSpacer( pSpacerPoint );
                
                //settings
                setProperties(pSpacerPoint, CPL_ROOT);
            }
            
            position   += ci::Vec3f( cos( angle )*jointDistance, 0, sin( angle )*jointDistance );
            
            
            mCharacterPoints.push_back( CharacterPoint( position, &mPhysics, mCharacterPoints.size() ) );
            if( CharacterVector[a][b] == 0 ) {
                mCharacterPoints.back().mLevel = CPL_FEELER;
                mCharacterPoints.back().mFunction = CPF_NORMAL;
            }
            else { 
                mCharacterPoints.back().mLevel = CPL_JOINT;
                mCharacterPoints.back().mFunction = 0;
            }
            
            pJointPoint = &mCharacterPoints.back();
            
            
            
            setRelations( pSpacerPoint , pJointPoint );
            setBonds( pSpacerPoint , pJointPoint );
            
            //settings
            setProperties(pJointPoint, pJointPoint->mLevel);
            
            //FEELER POINTS
            for( int c = 0; c < CharacterVector[a][b]; c++ ) {
                
                position = pJointPoint->getPosition() + Rand::randVec3f() * Rand::randFloat(30.0f,80.0f);   // BERECHNUNG?!?!?!??!!?
                
                mCharacterPoints.push_back( CharacterPoint( position, &mPhysics, mCharacterPoints.size() ) );
                mCharacterPoints.back().mLevel = CPL_FEELER;
                
                CharacterPoint *pFeelerPoint = &mCharacterPoints.back();

                setRelations( pJointPoint , pFeelerPoint );
                setBonds( pJointPoint , pFeelerPoint );
                
                //settings
                setProperties(pFeelerPoint, CPL_FEELER);
                
                if( pRootPoint->mFunction == CPF_END ) { pFeelerPoint->mFunction = CPF_STAND; }
                else { pFeelerPoint->mFunction = CPF_NORMAL; }
                
            }
            
        }
                
        //Neue Koordinaten berechnen
        position = pRootPoint->getPosition() + ci::Vec3f(0,-50,0);
        
    }
    
    
    // POST SETTINGS
    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
        if(p->getEndOfLine()) {
            mBonds[p->getBondID(0)].mLevel = 1;
        }
    }
    
    
    //Stands
    mStandBonds.clear();
    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
        if( p->mLevel == CPL_FEELER && p->mFunction == CPF_STAND ) {
            
            float length = 0;
            
            for( int i = 0; i < p->getParent()->getNumberOfBonds(); i++ ) {
                if( mBonds[p->getParent()->getBondID(i)].mLevel == 2 ) {
                    length = mBonds[p->getParent()->getBondID(i)].getBondLength();
                    
                }
            }
             
            length += mBonds[p->getBondID(0)].getBondLength();
            
            mStandBonds.push_back( Bond(pBackboneBottom, &(*p)) );
            mStandBonds.back().mStrength = 0.3f;
            mStandBonds.back().makeBond( &mPhysics );
            mStandBonds.back().mSaveMaxLength = length;
            
            p->setStandBondID( mStandBonds.size()-1 );
        }
    }
    
}

void Character::setProperties( CharacterPoint *_point, int _typ ) {
    
    switch ( _typ ) {
        case CPL_ROOT:
            
            if( _point->mFunction == CPF_SPACER ) {
                _point->setRadius(3.0f);
            } else {
                _point->setRadius(12.0f);
            }
            
            _point->setMass(150.0f);
            _point->setEndOfLine(false);
            break;
        case CPL_JOINT:
            _point->setMass(20.0f);
            _point->setRadius(4.0f);
            _point->setEndOfLine(false);
            break;
        case CPL_FEELER:
            _point->setMass(5.0f);
            _point->setRadius(3.0f);
            _point->setEndOfLine(true);
            break;
    }
}

void Character::setRelations( CharacterPoint *_parent, CharacterPoint *_child ) {
    //set Parent
    _child->setParent( _parent );
    //add Child to Parent
    _parent->addChild( _child );
}

void Character::setBonds( CharacterPoint *_parent, CharacterPoint *_child ) {
    //set bonds
    mBonds.push_back( Bond( _parent , _child ) );
    mBonds.back().makeBond( &mPhysics );
    
    //add Bond IDs
    _child->addBondID( mBonds.size()-1 );
    _parent->addBondID( mBonds.size()-1 );
}

int Character::getRandPointNumber( int _min, int _max ) {
    
    int max = _max + 1; 
    if ( max > 5 ) { max = 5; }

    int min = _min;
    if ( min >= max ) { min = max - 1; }
    
    int percent[6] = { 0, 10, 40, 92, 97, 100 };
    
    int randomInt = Rand::randInt(percent[ min ], percent[ max ]);
    
    // 00 - 10; 10% NewPoints: 0
    // 10 - 40; 30% NewPoints: 1
    // 40 - 92; 52% NewPoints: 2
    // 92 - 97;  5% NewPoints: 3
    // 97 - 100; 3% NewPoints: 4
    
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
    
    float width = 2000.0f;
    float height = 200.0f;
    
    //physics.verbose = true;			// dump activity to log
    mPhysics.setGravity(Vec3f(0, GRAVITY, 0));
    mPhysics.setWorldSize(Vec3f(-width/2, -height*2, -width/2), Vec3f(width/2, height, width/2));
    //mPhysics.setWorldSphere(CENTER, mRadius);
    mPhysics.setSectorCount(SECTOR_COUNT);
    
    //Schwerfälligkeit
    mPhysics.setDrag(0.97f);
    mPhysics.setDrag(1);		// FIXTHIS
    mPhysics.setDrag(0.9f);
    
    //Collision
    mPhysics.enableCollision();
    
}

void Character::createBackbone() {
    
    std::vector<CharacterPoint*> rootPath;
    rootPath.clear();
    
    for( int z = 0; z < mCharacterPoints.size(); z++ ) {
        if( mCharacterPoints[z].mLevel == CPL_ROOT && mCharacterPoints[z].mFunction == CPF_START ) {
            
            CharacterPoint* lastPoint = &mCharacterPoints[z];
            while( lastPoint->mFunction != CPF_END ) {
                if( lastPoint->mFunction != CPF_SPACER ) {
                    rootPath.push_back( lastPoint );
                    lastPoint = lastPoint->getParent();   
                }
            }
            
            rootPath.push_back( lastPoint );
            
        }
    }
    
    //
    mBackbone.setPoints( rootPath );
    mBackbone.setPhysics( &mPhysics );
    mBackbone.makeBond();
    mBackbone.makeSpline();
    
}

void Character::createSplinesA() {
    
    mPaths.clear();
    
    for( int z = 0; z < mCharacterPoints.size(); z++ ) {
        
        if(mCharacterPoints[z].getEndOfLine()) {
            
            std::vector<CharacterPoint*> points;
            CharacterPoint* lastPoint = &mCharacterPoints[z];
            
            while( lastPoint->mLevel != CPL_ROOT ) {
                points.push_back( lastPoint );
                lastPoint = lastPoint->getParent();
            }
            
            points.push_back( lastPoint ); 
            mPaths.push_back( points );
            
        }

        
	} 
    
    //make CharacterSplines
    mCharacterSplines.clear();

    for( int i = 0; i < mPaths.size(); i++) {
        
        vector<Vec3f> newPoints;
        for( int u = 0; u < mPaths[i].size(); u++ ) {
            newPoints.push_back( mPaths[i][u]->getPosition() );
        } 
        
        mCharacterSplines.push_back( CharacterSpline( newPoints ) );
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
// DEL FUNCTIONS /////////////////////////////
//////////////////////////////////////////////

void Character::destroyStructure() {
    
    //Clear all Objects
    mParticleController.clear();
    mBonds.clear();
    mCharacterPoints.clear();
    mCharacterSplines.clear();
    mPhysics.clear();
    
    mAlive = false;
    
}

//////////////////////////////////////////////
// AUDIO FUNCTIONS ///////////////////////////
//////////////////////////////////////////////

void Character::startAnimation( time_t _duration ) {
    mMovement.startAnimation( _duration );
}

bool Character::waitforBass() {
    return mMovement.getWaitfor(W_BASS);
}

void Character::inputBass( PeakTimer _pt ) {
    mMovement.bass( _pt );
}

bool Character::waitforMidlow() {
    return mMovement.getWaitfor(W_MIDLOW);
}

void Character::inputMidlow( PeakTimer _pt ) {
    mMovement.midlow( _pt );
    cout << "input: " << _pt.mPeak << " - " << _pt.mTime - niko::getTimeMS() << endl;
}

bool Character::waitforMidHigh() {
    return mMovement.getWaitfor(W_MIDHIGH);
}

void Character::inputMidhigh( PeakTimer _pt ) {
    mMovement.midhigh( _pt );
}

bool Character::waitforHigh() {
    return mMovement.getWaitfor(W_HIGH);
}

void Character::inputHigh( PeakTimer _pt ) {
    mMovement.high( _pt );
}


//////////////////////////////////////////////
// SET FUNCTIONS /////////////////////////////
//////////////////////////////////////////////


//void Character::setRadius( float _r ) {
//    
//    if( _r != mRadius ) {
//    
//        float scaleFactor = _r/mRadius;
//        scale(scaleFactor);
//    
//        mRadius = _r;
//        //mPhysics.setWorldSphere(CENTER, mRadius);
//        
//     }
//}
//
//void Character::scale( float _s ) {
//     
//    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
//        
//        Vec3f pointVec = p->getPosition();
//        pointVec *= _s;
//        p->moveTo(pointVec);
//
//	}
//    
//    for(  std::vector<Bond>::iterator p = mBonds.begin(); p != mBonds.end(); ++p ){
//        p->setBondLength( p->getBondLength() * _s );
//        p->mSaveDistanceA *= _s;    
//    } 
//    
//    mFrustrationAtt.changeWorld( CENTER_POS, mRadius );
//    mEngagementAtt.changeWorld( CENTER_POS, mRadius );
//    
//}





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

void Character::gravity() {
    
    
     static int count = 0;
     
     
     if(count == 0) {
         for(  std::vector<Bond>::iterator p = mBonds.begin(); p != mBonds.end(); ++p ){ 
             p->turnOff();
         }   
         
         for(  std::vector<Bond>::iterator p = mStandBonds.begin(); p != mStandBonds.end(); ++p ){ 
             p->turnOff();
         }   
        
         mBackbone.getBond()->turnOff();
         
         count = 1;
         mPhysics.setGravity(Vec3f(0, 3, 0));
     }
     else {
         for(  std::vector<Bond>::iterator p = mBonds.begin(); p != mBonds.end(); ++p ){ 
             p->turnOn();
         }   
         
         for(  std::vector<Bond>::iterator p = mStandBonds.begin(); p != mStandBonds.end(); ++p ){ 
             p->turnOn();
         }   
         
         mBackbone.getBond()->turnOn();
         
         count = 0;
         mPhysics.setGravity(Vec3f(0, 0, 0));
     }
}

//RENAME
void Character::test() {
    
    
    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
        
        if( p->mLevel == CPL_FEELER && p->mFunction == CPF_STAND ) {
            
            p->setFixed();
            
            ci::Vec3f pos = p->getPosition();
            
            pos.y = 200.0f;
            
            p->moveTo(pos);
            
            int id = p->getStandBondID();
            mStandBonds[id].setBondLength(mStandBonds[id].mSaveMaxLength);
            
        }
    }

}

//////////////////////////////////////////////
// UPDATE FUNCTIONS //////////////////////////
//////////////////////////////////////////////

void Character::updateSplines() {
    
    for( int i = 0; i < mPaths.size(); i++) {
        
        vector<Vec3f> newPoints;
        for( int u = 0; u < mPaths[i].size(); u++ ) {
            newPoints.push_back( mPaths[i][u]->getPosition() );
        }
        
        mCharacterSplines[i].update( newPoints );
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
        
    if(!mAlive) { return; };

    mMovement.update();
    mBackbone.update();
    
    if(!mDrawCharacter) {
        
        //Splines Update
        updateSplines();
    }

    //physics update
    mPhysics.update();
    
    
    ///// najaaa
    if( mCharacterPoints.size() > 0) {
        mCenterPosition = mCharacterPoints[0].getPosition();
    }
}

//////////////////////////////////////////////
// RENDER FUNCTIONS //////////////////////////
//////////////////////////////////////////////

void Character::draw() {
    
    if(!mAlive) { return; };
    
    
    
    gl::pushMatrices();
    
        //gl::translate( mCenterPosition );
        //gl::rotate( mRotation );
        
   
    
        gl::pushMatrices();
    
            //Schütteln
            if(mForceTimer) {
                float translateMax = mForceTimer;
                glTranslatef(Rand::randFloat(-translateMax, translateMax), Rand::randFloat(-translateMax, translateMax), Rand::randFloat(-translateMax, translateMax));
                mForceTimer--;
            }
    
         
            //gl::color(0,1,0,0.4);
            //gl::enableWireframe();
            //gl::drawStrokedCircle(CENTER.xy(), mRadius);
            //gl::drawSphere(CENTER, mRadius, 64);
           // gl::disableWireframe();
            
            //gl::color(1,0,0);
        
            if(mDrawCharacter) {
        
                for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){ 
                           
                    if( p->mLevel == CPL_ROOT ) { gl::color(1,1,0); }
                    else if(p->mLevel == CPL_JOINT ) { gl::color(1,0,0); }
                    else { gl::color(0.5,0.5,1); }
                    
                    p->render();
                    
                }
            }
    
            //Draw Bonds
            if(mDrawCharacter) {
                for(  std::vector<Bond>::iterator p = mBonds.begin(); p != mBonds.end(); ++p ){
                    
                    gl::color(1,0.5,0);
                    p->render();
                }   
                
                for(  std::vector<Bond>::iterator p = mStandBonds.begin(); p != mStandBonds.end(); ++p ){
                    gl::color(0.0,0.5,1);
                    p->render();
                }
                
                
                
            }
    
    
    
    
            //Draw EmoAttractors
            //mFrustrationAtt.render();
            //mEngagementAtt.render();
        
            //DRAW SPLINES
            if(!mDrawCharacter) {             
                
                
                
//               const int numSegments = 30;
                gl::color( ColorA( 0.8f, 0.2f, 0.8f, 0.5f ) );
                glLineWidth( 2.0f );
                 
                
                for(  std::vector<CharacterSpline>::iterator p = mCharacterSplines.begin(); p != mCharacterSplines.end(); ++p ){ 
                    //p->drawFrameSlices( 2.25f );
                    //p->drawFrames( 1.5f );
                    p->drawVBO();
                }
                
                //mBackbone.getSpline()->drawFrames( 1.5f * 2 );
                //mBackbone.getSpline()->drawFrameSlices( 2.25f * 2 );
                mBackbone.getSpline()->drawVBO();
                
                
                glEnable( GL_TEXTURE_2D );
                
                gl::enableDepthWrite( false );
                glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
                
                gl::enableAdditiveBlending();
                mParticleTexture.bind();
                 
                for(  std::vector<CharacterSpline>::iterator p = mCharacterSplines.begin(); p != mCharacterSplines.end(); ++p ){ 
                    p->drawParticle();
                }
                
                mBackbone.getSpline()->drawParticle();
                
                mParticleTexture.unbind();
                
                gl::enableDepthWrite( true );
                gl::enableAlphaBlending();
                
                glDisable( GL_TEXTURE_2D );
            }
 
        gl::popMatrices();

    gl::popMatrices();
    
}

