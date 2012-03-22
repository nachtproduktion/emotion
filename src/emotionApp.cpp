#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "Character.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"

#include "BeatController.h"
#include "ParticleController.h"

#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

int fps;

class emotionApp : public AppBasic {
  public:
    void prepareSettings( Settings *settings );
	void setup();
	void mouseDown( MouseEvent event );	
    void mouseDrag( MouseEvent event );
	void update();
    void checkEmotions();
	void draw();
    void keyDown( KeyEvent event );
    
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
    
    //ParticleController mParticleController;
	
    //Emotion
    float               mFrustration;
    float               mEngagement;
    float               mExcitement;
    float               mMeditation;
    
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
	mParams = params::InterfaceGl( "Kamera", Vec2i( 200, 350 ) );
    mParams.addParam( "Framerate", &fps, "", true);
    mParams.addSeparator();
	mParams.addParam( "Scene Rotation", &mSceneRotation, "opened=1" );
	mParams.addParam( "Eye Distance", &mCameraDistance, "min=50.0 max=1500.0 step=50.0 keyIncr=s keyDecr=w" );
    mParams.addSeparator();
    mParams.addParam( "S-Radius", &mSphereRadius, "min=20 max=500 step=1" );
    mParams.addParam( "C-Points", &mCPoints, "min=1 max=50 step=1" );
    mParams.addParam( "F-Amount", &mFAmount, "min=5 max=50 step=1" );
    mParams.addSeparator();
    mParams.addParam( "BPM", &mBPM, "min=1 max=200 step=1" );
    mParams.addSeparator();
    mParams.addParam( "Frustration", &mFrustration, "min=0 max=100 step=1" );
    mParams.addParam( "Engagement", &mEngagement, "min=0 max=100 step=1" );
    mParams.addParam( "Meditation", &mMeditation, "min=0 max=100 step=1" );
    mParams.addParam( "Excitement", &mExcitement, "min=0 max=100 step=1" );

    //CHARACTER
    mCPoints = 5;
    mFAmount = 10;
    mSphereRadius = 100.0f;
        
    //BeatController
    mBPM    = 90;
    
    //Emotion
    mFrustration = 0;
    mEngagement = 0;
    mMeditation = 0;
    mExcitement = 0;
    
    //Random
    Rand::randomize();
    
}

void emotionApp::update()
{
    fps = getAverageFps();
    
    // UPDATE CAMERA
	mEye = Vec3f( 0.0f, 0.0f, mCameraDistance );
	mCam.lookAt( mEye, mCenter, mUp );
	gl::setMatrices( mCam );
	gl::rotate( mSceneRotation );
    
    
    //mSceneRotation *= Quatf(0,0.01f,0.0f);
    //mParticleController.update();
    
    //Emotion UPDATe
    checkEmotions();
    
    ///////////
    mCharacter.setRadius(mSphereRadius);
    mCharacter.update();

    if( ci::app::getElapsedFrames() > 10 ) 
    mBeatController.update();
    
    //TEST
   // if( getElapsedFrames()%10 == 0 ) cout << getElapsedFrames() << endl;
   
}

void emotionApp::checkEmotions() {
    
    mCharacter.updateEmotions(mFrustration, mEngagement, mMeditation, mExcitement);

}

void emotionApp::draw()
{
	// clear out the window with black
    gl::clear( Color( 0, 0, 0.01f ), true );
	gl::enableDepthRead();
	gl::enableDepthWrite();
    
    
    mCharacter.draw();
    
    //mParticleController.draw();
    
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

void emotionApp::mouseDrag( MouseEvent event ) {
    mouseDown( event );
}


void emotionApp::mouseDown( MouseEvent event )
{
    //mCharacter.moveTo( event.getPos() );
    
    //Transform to World
    
    float width = ci::app::getWindowWidth();
    float height = ci::app::getWindowHeight();
    
    float posX = niko::mapping(event.getX(), 0, width, -width/2, width/2, true);
    float posY = niko::mapping(event.getY(), 0, height, height/2, -height/2, true);
    float posZ = 0;
    
    ci::Vec3f t = ci::Vec3f( posX, posY, posZ);
    
    //mParticleController.setTarget( t );
    
}




CINDER_APP_BASIC( emotionApp, RendererGl )
