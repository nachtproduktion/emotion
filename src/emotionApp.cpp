#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "Particle.h"
#include "Character.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"

#include "BeatController.h"

#include <list>

#define TOTAL_PARTICLES 500

using namespace ci;
using namespace ci::app;
using namespace std;

class emotionApp : public AppBasic {
  public:
    void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    void keyDown( KeyEvent event );
    
    std::list<Particle>	mParticles;
    Character mCharacter;
    int mCPoints, mFAmount;
    float mSphereRadius;
	
    BeatController mBeatController;
    int            mBPM;
    
	// PARAMS
	params::InterfaceGl	mParams;
    
    // CAMERA
	CameraPersp			mCam;
	Quatf				mSceneRotation;
	float				mCameraDistance;
	Vec3f				mEye, mCenter, mUp;
	
    
};

void emotionApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );
	settings->setFrameRate( 60.0f );
}

void emotionApp::setup()
{

    
    // SETUP CAMERA
	mCameraDistance = 500.0f;
	mEye			= Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCenter			= Vec3f::zero();
	mUp				= Vec3f::yAxis();
	mCam.setPerspective( 75.0f, getWindowAspectRatio(), 5.0f, 2000.0f );
	
	// SETUP PARAMS
	mParams = params::InterfaceGl( "Kamera", Vec2i( 200, 260 ) );
	mParams.addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
	mParams.addSeparator();
	mParams.addParam( "Eye Distance", &mCameraDistance, "min=50.0 max=1500.0 step=50.0 keyIncr=s keyDecr=w" );
    mParams.addSeparator();
    mParams.addParam( "S-Radius", &mSphereRadius, "min=20 max=500 step=1" );
    mParams.addParam( "C-Points", &mCPoints, "min=1 max=50 step=1" );
    mParams.addParam( "F-Amount", &mFAmount, "min=5 max=50 step=1" );
    mParams.addSeparator();
    mParams.addParam( "BPM", &mBPM, "min=1 max=200 step=1" );
    
    // PARTICLES
    for(int i=0; i<TOTAL_PARTICLES; i++) {
        float x = Rand::randFloat( app::getWindowWidth() );
        float y = Rand::randFloat( app::getWindowHeight() );
        float z = Rand::randFloat( 500 );
        mParticles.push_back( Particle( Vec3f(x,y,z), Vec3f(0,0,0) ) );
    }
    
    //CHARACTER
    mCPoints = 5;
    mFAmount = 10;
    mSphereRadius = 100.0f;
    
    //BeatController
    mBPM    = 90;
    
}

void emotionApp::update()
{
    
    // UPDATE CAMERA
	mEye = Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );
    
    
    //mSceneRotation *= Quatf(0,0.01f,0.0f);
    
    /////////////////
    for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		p->update();
	}
    
    
    ///////////
    mCharacter.setRadius(mSphereRadius);
    mCharacter.update();

    if( ci::app::getElapsedFrames() > 10 ) 
    mBeatController.update();
    
    //TEST
   // if( getElapsedFrames()%10 == 0 ) cout << getElapsedFrames() << endl;
   
}

void emotionApp::draw()
{
	// clear out the window with black
    gl::clear( Color( 0, 0, 0.01f ), true );
	gl::enableDepthRead();
	gl::enableDepthWrite();
    
    
    ///////////
    for( list<Particle>::iterator p = mParticles.begin(); p != mParticles.end(); ++p ){
		//p->draw();
	}
    
    mCharacter.draw();
    
    /*
    //DEBUG
    if( mBeatController.newBeat ) {
        gl::color(0, 255, 0);
        gl::drawSolidCircle(Vec2d(0,0), 500.0f);
    }
     */
    
    
    // DRAW PARAMS WINDOW
	params::InterfaceGl::draw();
}


////////////////////////////////////////////////////////////////
// KEY + MOUSE EVENTS
////////////////////////////////////////////////////////////////

void emotionApp::keyDown( KeyEvent event )
{
    switch(event.getChar()) {
        case 'm': 
            mBeatController.emtpyCharacter();
            mCharacter.createNewStructure(mCPoints);
            mBeatController.addCharacter(&mCharacter);
        break;
        case 'f': mCharacter.addRandomForce(mFAmount); break;
        case 't': mCharacter.test(); break;
        case 'b': mBeatController.setBPM( mBPM ); break;
        case ' ': mBeatController.bang(); break;
    }

}

void emotionApp::mouseDown( MouseEvent event )
{
    mCharacter.moveTo( event.getPos() );
}




CINDER_APP_BASIC( emotionApp, RendererGl )
