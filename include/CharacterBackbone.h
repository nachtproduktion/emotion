#ifndef emotion_CharacterBackbone_h
#define emotion_CharacterBackbone_h

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"

#include <vector>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "niko_functionen.h"
#include "CharacterPoint.h"

#include "Constants.h"
#include "CharacterBond.h"
#include "CharacterSpline.h"

///////////////////////////////////////////////////////////////
//////////// Backbone ///////////
///////////////////////////////////////////////////////////////

class Backbone {
    public:
        Backbone();
    
        void setPoints( std::vector < CharacterPoint* > _pointList );
        void setPhysics( MSA::Physics::World3D * _physics );
        
        void makeBond();
        Bond* getBond();
    
        void makeSpline();
        void updateSpline();
        CharacterSpline* getSpline();
    
        void update();
        void updateSpacers();
        void updatePositions(); 
        void calcMatrix();
    
        bool                    mDrawSpline;
    
    private:
        std::vector < CharacterPoint* > mPoints;
    
        std::vector<Vec3f>		mPs;			// Points
        std::vector<Matrix44f>	mFrames;		// Coordinate frame
    
        MSA::Physics::World3D * mPhysics;
        Bond                    mBackboneBond;
        
        CharacterSpline         mSpline; //mCharacterRoot
       
    
    
};

#endif
