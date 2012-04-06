#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "Character.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/Arcball.h"

#include "BeatController.h"
#include "ParticleController.h"
#include "InfoPanel.h"

#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

int fps;
//Billboard
ci::Vec3f bup;
ci::Vec3f br;

class emotionApp : public AppBasic {
  public:
    void prepareSettings( Settings *settings );
	void setup();
    void resize( ResizeEvent event );
	
    void update();
    void checkEmotions();
    void updateCharacters();
    
	void draw();
    void drawInfoPanel();
    
    void keyDown( KeyEvent event );
    void keyUp( KeyEvent event );
    void mouseDown( MouseEvent event );	
    void mouseDrag( MouseEvent event );
    
    //Spezial
    char                mKeyDown;
    Vec2i               mMouseDown;
    InfoPanel           mInfoPanel;
    
    //Character
    Character           mCharacter;
    int                 mCPoints, mFAmount;
    float               mSphereRadius;
	
    BeatController      mBeatController;
    int                 mBPM;
    
    bool                mToggleDance;
    
	// PARAMS
	params::InterfaceGl	mParams;
    
    // CAMERA
	CameraPersp			mCam;
	float				mCameraDistance;
    float               mLastCameraDistance;
    
    //ArcBall
    Arcball             mArcball;
    Vec3f               mCharPosition;
    Vec3f               mLastCharPosition;
    	
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
	mCam.setPerspective( 50.0f, getWindowAspectRatio(), 0.1f, 10000.0f );
    mCam.lookAt( Vec3f( 0, 0, mCameraDistance ), Vec3f::zero() );
    
    
    //CHARACTER
    mCPoints            = 5;
    mFAmount            = 10;
    mSphereRadius       = 100.0f;
    
    mCharPosition       = ci::Vec3f::zero();
    mLastCharPosition   = ci::Vec3f::zero();
    
    mToggleDance        = false;
    
    
    //BeatController
    mBPM                = 90;
    
    //Emotion
    mFrustration        = 0;
    mEngagement         = 0;
    mMeditation         = 0;
    mExcitement         = 0;
    
    //Random
    Rand::randomize();
	
	// SETUP PARAMS
	mParams = params::InterfaceGl( "Kamera", Vec2i( 200, 300 ) );
    mParams.addParam( "Framerate", &fps, "", true);
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
    
    //SPEZIAL
    mKeyDown = false;
    mInfoPanel.createTexture( Vec2i( 20, 350 ) );
    
    //GL
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableAlphaBlending();
    
}

void emotionApp::resize( ResizeEvent event )
{
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( getWindowCenter() );
	mArcball.setRadius( 200 );
    
    mCam.setPerspective( 50.0f, getWindowAspectRatio(), 0.1f, 10000.0f );
	gl::setMatrices( mCam );	    
}

void emotionApp::update()
{
    fps = getAverageFps();
    
    // UPDATE CAMERA
    
    Quatf quat = mArcball.getQuat();
    //quat.w *= -1.0f; // reverse rotation
    
    Vec3f cam_target = mCharacter.mCenterPosition;
    Vec3f cam_offset = quat * Vec3f(0,0,mCameraDistance);
    Vec3f cam_eye    = cam_target - cam_offset;
    Vec3f cam_up     = quat * Vec3f(0,-1,0);
    mCam.lookAt(cam_eye, cam_target, cam_up);
    
	//mCam.lookAt( Vec3f( 0, 0, mCameraDistance ), Vec3f::zero() );
        
    // EMOTION UPDATE
    checkEmotions();
    updateCharacters();
    
    // BEAT CONTROLLER
    /*
    if( ci::app::getElapsedFrames() > 10 ) 
        mBeatController.update();
   */
}

void emotionApp::updateCharacters() {
    
    mCharacter.setRadius(mSphereRadius);
    mCharacter.move(mCharPosition, mArcball.getQuat());

    if( mToggleDance ) { mCharacter.dance(); }
    
    mCharacter.update();

}

void emotionApp::checkEmotions() {
    
    mCharacter.updateEmotions(mFrustration, mEngagement, mMeditation, mExcitement);

}

void emotionApp::draw()
{  
    
    glClearColor( 0.0025f, 0.1f, 0.2f, 1 );
	gl::enableDepthWrite( true );
	gl::enableDepthRead( true );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	gl::enableAlphaBlending();
    
    
    // KAMERA
    gl::setMatrices( mCam ); 
    
    mCam.getBillboardVectors(&br, &bup);

    
    // CHARACTER
    mCharacter.draw();

    
    // DRAW PARAMS WINDOW + INFO PANEL
	params::InterfaceGl::draw();
    drawInfoPanel();
}

void emotionApp::drawInfoPanel() {

    gl::enableDepthWrite( false );
	gl::setMatricesWindow( getWindowSize() );
    
	glEnable( GL_TEXTURE_2D );
	mInfoPanel.update();
	glDisable( GL_TEXTURE_2D );

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
        case 'q': 
        case 'w': mKeyDown = event.getChar(); break;
        case 'r': //RESET VIEW
            mArcball.resetQuat();
            mCharPosition = ci::Vec3f::zero();
        break;
        case 'i': mInfoPanel.toggleState(); break;
        case '1': mToggleDance = !mToggleDance; break;
        case '2': mCharacter.wince(); break;
        case '3': mCharacter.jump(); break;
        case '4': mCharacter.center(); break;
        case '5': mCharacter.sphere(); break;
        case 'd': mCharacter.mDrawCharacter = !mCharacter.mDrawCharacter; break;
    }

}

void emotionApp::keyUp( KeyEvent event ) {
    
    mKeyDown = false;
    
}

void emotionApp::mouseDown( MouseEvent event )
{
	Vec2i P = event.getPos();
	//P.y = getWindowHeight() - P.y;
	mArcball.mouseDown( P );
    
    mMouseDown = event.getPos();
    mLastCharPosition = mCharPosition;
    mLastCameraDistance = mCameraDistance;
}


void emotionApp::mouseDrag( MouseEvent event ) {
	
    if(!mKeyDown) {
        Vec2i P = event.getPos();
       // P.y = getWindowHeight() - P.y;
        mArcball.mouseDrag( P );
    } else if( mKeyDown == 'q') {
        //x+y achse
        //mCharPosition.x = mLastCharPosition.x + (event.getX() - mMouseDown.x);
        //mCharPosition.y = mLastCharPosition.y - (event.getY() - mMouseDown.y);
    } else if( mKeyDown == 'w') {
        //z achse
        mCameraDistance = mLastCameraDistance + (event.getY() - mMouseDown.y); 
        //mCharPosition.z = mLastCharPosition.z + (event.getY() - mMouseDown.y);
    } 
  
}


CINDER_APP_BASIC( emotionApp, RendererGl )
