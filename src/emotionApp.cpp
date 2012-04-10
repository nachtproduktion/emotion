#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "Character.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Utilities.h"
#include "cinder/params/Params.h"
#include "cinder/Camera.h"
#include "cinder/Arcball.h"
#include "OscListener.h"

#include "AudioController.h"
#include "ParticleController.h"
#include "InfoPanel.h"
#include "Structs.h"

#include <list>
#include <map>

using namespace ci;
using namespace ci::app;
using namespace std;

int fps;
//Billboard
ci::Vec3f bup;
ci::Vec3f br;

// KONST //////////////////
#define OSC_AUDIO_PORT  3000
#define OSC_EMOTIV_PORT 4000



class emotionApp : public AppBasic {
  public:
    void prepareSettings( Settings *settings );
	void setup();
    void resize( ResizeEvent event );
    
    void update();
    void checkEmotions();
    void updateCharacters();
    
	void draw();
    void drawRoom();
    void drawInfoPanel();
    
    void keyDown( KeyEvent event );
    void keyUp( KeyEvent event );
    void mouseDown( MouseEvent event );	
    void mouseDrag( MouseEvent event );
    
    //Spezial
    char                mKeyDown;
    Vec2i               mMouseDown;
    InfoPanel           mInfoPanel;
    
    //Controller
    AudioController     mAudioController;
    
    //Character
    Character           mCharacter;
    int                 mFAmount;
    
    
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
    
    //Controller
    mAudioController.init( OSC_AUDIO_PORT );
    mAudioController.addCharacter(&mCharacter);
    
    
    //CHARACTER
    mFAmount            = 10;
    
    mCharPosition       = ci::Vec3f::zero();
    mLastCharPosition   = ci::Vec3f::zero();
    
    mToggleDance        = false;

    
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
    mParams.addParam( "F-Amount", &mFAmount, "min=5 max=50 step=1" );
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

////////////////////////////////////////////////////////////////
// UPDATE
////////////////////////////////////////////////////////////////

void emotionApp::update()
{
    
    fps = getAverageFps();

    //Controller UPDATE
    mAudioController.update();
    
    
    // UPDATE CAMERA
    Quatf quat = mArcball.getQuat();
    //quat.w *= -1.0f; // reverse rotation
    
    Vec3f cam_target = Vec3f(0,0,0);
    Vec3f cam_offset = quat * Vec3f(0,0,mCameraDistance);
    Vec3f cam_eye    = cam_target - cam_offset;
    Vec3f cam_up     = quat * Vec3f(0,-1,0);
    mCam.lookAt(cam_eye, cam_target, cam_up);
    
	//mCam.lookAt( Vec3f( 0, 0, mCameraDistance ), Vec3f::zero() );
        
    // EMOTION UPDATE
    checkEmotions();
    
    
    if(mCharacter.mAlive) {
        updateCharacters();
    }
}

void emotionApp::updateCharacters() {
    
    //Check ob Character neuen Beat braucht
    PeakTimer pt;
    
    if( mCharacter.waitforBass() ) {
        pt = mAudioController.getNextPeak( "/bass" );
        if( pt.mTime != 0 ) {
            mCharacter.inputBass( pt );
        }
    }
    
    if( mCharacter.waitforMidlow() ) {
        pt = mAudioController.getNextPeak( "/midlow" );
        if( pt.mTime != 0 ) {
            mCharacter.inputMidlow( pt );
        }
    }
    
    if( mCharacter.waitforMidHigh() ) {
        pt = mAudioController.getNextPeak( "/midhigh" );
        if( pt.mTime != 0 ) {
            mCharacter.inputMidhigh( pt );
        } 
    }
    
    if( mCharacter.waitforHigh() ) {
        pt = mAudioController.getNextPeak( "/high" );
        if( pt.mTime != 0 ) {
            mCharacter.inputHigh( pt );
        }
    }
    
    
    mCharacter.move(mCharPosition, mArcball.getQuat());
    mCharacter.update();
    
    
    //if( mToggleDance ) { mCharacter.dance(); }

}

void emotionApp::checkEmotions() {
    
    mCharacter.updateEmotions(mFrustration, mEngagement, mMeditation, mExcitement);

}


////////////////////////////////////////////////////////////////
// DRAW
////////////////////////////////////////////////////////////////

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

    //ROOM
    drawRoom();
    
    // CHARACTER
    mCharacter.draw();

    
    // DRAW PARAMS WINDOW + INFO PANEL
	params::InterfaceGl::draw();
    drawInfoPanel();
}

void emotionApp::drawRoom() {
    
    float width = 2000.0f;
    float height = 200.0f;
    
    float a = 0.2f;
    
    glBegin(GL_QUADS);
    
    /*
     // draw right wall
     glColor4f(0.9, 0.9, 0.9, a);		glVertex3f(width/2, height+1, width/2);
     glColor4f(1, 1, 1, a);				glVertex3f(width/2, -height, width/2);
     glColor4f(0.95, 0.95, 0.95, a);	glVertex3f(width/2, -height, -width/2);
     glColor4f(0.85, 0.85, 0.85, a);	glVertex3f(width/2, height+1, -width/2);
     
     // back wall
     glColor4f(0.9, 0.9, 0.9, a);		glVertex3f(width/2, height+1, -width/2);
     glColor4f(1, 1, 1, a);				glVertex3f(width/2, -height, -width/2);
     glColor4f(0.95, 0.95, 0.95, a);	glVertex3f(-width/2, -height, -width/2);
     glColor4f(0.85, 0.85, 0.85, a);	glVertex3f(-width/2, height+1, -width/2);
     
     // left wall
     glColor4f(0.9, 0.9, 0.9, a);		glVertex3f(-width/2, height+1, -width/2);
     glColor4f(1, 1, 1, a);				glVertex3f(-width/2, -height, -width/2);
     glColor4f(0.95, 0.95, 0.95, a);	glVertex3f(-width/2, -height, width/2);
     glColor4f(0.85, 0.85, 0.85, a);	glVertex3f(-width/2, height+1, width/2);
     
     // front wall
     glColor4f(0.95, 0.95, 0.95, a);	glVertex3f(width/2, -height, width/2);
     glColor4f(0.85, 0.85, 0.85, a);	glVertex3f(width/2, height+1, width/2);
     glColor4f(0.9, 0.9, 0.9, a);		glVertex3f(-width/2, height+1, width/2);
     glColor4f(1, 1, 1, a);				glVertex3f(-width/2, -height, width/2);
     */
    
    // floor
    glColor4f(.8, 0.8, 0.8, a);
    glVertex3f(width/2, height+1, width/2);
    glVertex3f(width/2, height+1, -width/2);
    glVertex3f(-width/2, height+1, -width/2);
    glVertex3f(-width/2, height+1, width/2);
    glEnd();
    
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
        case 'm': mCharacter.createNewStructure(); break;
        case 'f': mCharacter.addRandomForce(mFAmount); break;
        case 't': mCharacter.test(); break;
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
        case '6': mCharacter.gravity(); break;
        case 'd': 
            mCharacter.mDrawCharacter = !mCharacter.mDrawCharacter; 
            mCharacter.mBackbone.mDrawSpline = !mCharacter.mBackbone.mDrawSpline;
        break;
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
