#include "CharacterBackbone.h"

Backbone::Backbone () {
    //
    mPhysics = NULL;
    mDrawSpline = false;
    
}

void Backbone::setPoints( std::vector < CharacterPoint* > _pointList ) {
    
    mPoints = _pointList;
    
}

void Backbone::setPhysics( MSA::Physics::World3D * _physics ) {
    mPhysics = _physics;
}

void Backbone::makeBond() {
    
    //backbone
    CharacterPoint * pBackboneTop = NULL;
    CharacterPoint * pBackboneBottom = NULL;
    
    for( int i = 0; i < mPoints.size(); ++i ) {
        
        if( mPoints[i]->mFunction == CPF_START ) { pBackboneTop = mPoints[i]; };
        if( mPoints[i]->mFunction == CPF_END ) { pBackboneBottom = mPoints[i]; };
        
    }
    
    if(pBackboneBottom != NULL && pBackboneTop != NULL) {
        mBackboneBond = Bond(pBackboneTop, pBackboneBottom);
        mBackboneBond.makeBond( mPhysics );
    } 
    
}

Bond* Backbone::getBond() {
    return &mBackboneBond;
}

void Backbone::makeSpline() {
    
    //Make Root Spline
    vector<Vec3f> newPoints;
    for( int u = 0; u < mPoints.size(); u++ ) {
        newPoints.push_back( mPoints[u]->getPosition() );
    }
    
    mSpline = CharacterSpline( newPoints );
    mSpline.setRoot();
    
}

void Backbone::updateSpline() {
    
    vector<Vec3f> newPoints;
    for( int u = 0; u < mPoints.size(); u++ ) {
        newPoints.push_back( mPoints[u]->getPosition() );
    }
    
    mSpline.update( newPoints );
    
}

CharacterSpline* Backbone::getSpline() {
    
    return &mSpline;
    
}

void Backbone::update() {
    
    if(mPoints.size() <= 2) { return; }
    
    updatePositions();
    calcMatrix();
    
    updateSpacers();
    
    if(mDrawSpline) {
        updateSpline();
    } 
}

void Backbone::updateSpacers() {
    
    for( int i = 0; i < mPoints.size(); ++i ) {
        
        if( mPoints[i]->mFunction == CPF_MIDDLE ) {
            
            int nSpacer = mPoints[i]->getNumberOfSpacers();
            float r = mPoints[i]->getShellRadius();
            
            for( int z = 0; z < nSpacer; z++ ) {
                float angle = ( TWOPI / nSpacer ) * z;
                ci::Vec3f position = ci::Vec3f( cos( angle )*r, 0, sin( angle )*r );
                
                mPoints[i]->getSpacer( z )->moveTo( mFrames[i] * position );
            }
            
        }
        
    }
    
}

void Backbone::updatePositions()
{
    
    mPs.clear();
    
    for( int i = 0; i < mPoints.size(); ++i ) {
        
        Vec3f P = mPoints[i]->getPosition();
        mPs.push_back( P );
    
    }
    
}

void Backbone::calcMatrix() {
    
    mFrames.clear();
    
    int n = mPs.size();
    mFrames.resize( n );
    
    for( int i = 0; i < n; ++i ) {
        Vec3f p0, p1, p2;		
        if( i < (n - 2) ) {
            p0 = mPs[i];
            p1 = mPs[i + 1];
            p2 = mPs[i + 2];
        }
        else if( i == (n - 2) ) {
            p0 = mPs[i - 1];
            p1 = mPs[i];
            p2 = mPs[i + 1];
        }	
        else if( i == (n - 1) ) {
            p0 = mPs[i - 3];
            p1 = mPs[i - 2];
            p2 = mPs[i - 1];
        }
        
        
        Vec3f t = (p1 - p0).normalized();
        Vec3f n = t.cross(p2 - p0).normalized();
        
        if( t == (p2 - p0).normalized() ) { //n = Vec3f(0,0,-1);
            
            //if( n.length() == 0.0f ) {
            int ix = fabs( t[0] ) < fabs( t[1] ) ? 0 : 1;
            if( fabs( t[2] ) < fabs( t[ix] ) ) 
                ix = 2;
            
            Vec3f v( 0.0f, 0.0f, 0.0f ); 
            v[ix] = 1.0;
            n = t.cross( v ).normalized();
        }
        Vec3f b = t.cross( n );	
        
        //b = Vec3f(1,0,0);
        
        Matrix44f& m = mFrames[i];
        m.at( 0, 0 ) = b.x;
        m.at( 1, 0 ) = b.y;
        m.at( 2, 0 ) = b.z;
        m.at( 3, 0 ) = 0;
        
        m.at( 0, 1 ) = n.x;
        m.at( 1, 1 ) = n.y;
        m.at( 2, 1 ) = n.z;
        m.at( 3, 1 ) = 0;
        
        m.at( 0, 2 ) = t.x;
        m.at( 1, 2 ) = t.y;
        m.at( 2, 2 ) = t.z;
        m.at( 3, 2 ) = 0;
        
        m.at( 0, 3 ) = mPs[i].x;
        m.at( 1, 3 ) = mPs[i].y;
        m.at( 2, 3 ) = mPs[i].z;
        m.at( 3, 3 ) = 1;
    }
}
