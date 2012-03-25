
#include "InfoPanel.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"

using namespace ci;

InfoPanel::InfoPanel()
{
	mState		= true;
	mOpacity	= 1.0f;
}


void InfoPanel::createTexture( ci::Vec2f _position )
{
    mLoc = _position;
    
	TextLayout layout;
	
	layout.setFont( Font( "Arial-BoldMT", 14 ) );
	layout.setColor( Color( 1.0f, 1.0f, 0.0f ) );
	layout.addLine( "Niko Super Programm" );
	
	layout.setFont( Font( "ArialMT", 10 ) );
	layout.setColor( Color( 1.0f, 0.7f, 0.0f ) );
	layout.addLine( "Mausklick + Bewegung: Charakter drehen" );
	layout.addLine( "'Q' + Mausklick + Bewegung: Charakter XY-Achse verschieben" );
    layout.addLine( "'W' + Mausklick + Bewegung: Charakter Z-Achse verschieben" );
    layout.addLine( "'R' Ansicht zurücksetzen" );
	layout.setColor( Color( 0.5f, 0.2f, 0.0f ) );
	layout.addLine( "'M' neuer Charakter" );
	layout.addLine( "'F' Charakter schütteln" );
    layout.addLine( "'D' Charakter Ein/Ausblenden" );
	layout.addLine( "'B' BPM aktualisieren" );
	layout.addLine( "' ' Beat Schlag" );
	layout.addLine( "'I' Info Ein/Ausblenden" );
	
	mTexture = gl::Texture( layout.render( true ) );
}


void InfoPanel::update()
{
	if( mState ){
		mOpacity -= ( mOpacity - 1.0f ) * 0.1f;
	} else {
		mOpacity -= ( mOpacity - 0.0f ) * 0.1f;	
	}
	
	if( mOpacity > 0.01f ){
		render();
	}
}


void InfoPanel::render()
{
	glColor4f( 1, 1, 1, mOpacity );
	gl::draw( mTexture, mLoc );
}


void InfoPanel::toggleState()
{
	mState = ! mState;
}

