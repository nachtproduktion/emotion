#ifndef emotion_CharacterSpline_h
#define emotion_CharacterSpline_h

#include "cinder/BSpline.h"
#include "cinder/Matrix.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"

#include <vector>

#include "niko_functionen.h"
#include "ParticleController.h"

class CharacterSpline {
    public:
    
        CharacterSpline( std::vector<ci::Vec3f> _pointList );
    
        void createParticleController();
//        void makeCircleProfile( std::vector<ci::Vec3f>& prof, float rad = 5.25f, int segments = 16 );
    
        void buildVectors();
        void buildPTF();
        
        void update( std::vector<ci::Vec3f> _pointList );
        void updateParticle();
    
        void drawParticle();
        
        void drawFrames( float lineLength = 1.0f, float lineWidth = 1.0f  );
        void drawFrameSlices( float scale = 1.0f );
       
            
    private:
    
        int                                 mNumSegs;                
    
        ci::BSpline3f                       mSpline;
        std::vector<ci::Vec3f>              mProf;			// points to tube (read extrude)
    
        std::vector<ci::Vec3f>              mPs;			// Points in b-spline sample
        std::vector<ci::Vec3f>              mTs;			// Tangents in b-spline sample
        std::vector<ci::Matrix44f>          mFrames;		// Coordinate frame at each b-spline sample
    
        std::vector<ParticleController>     mParticleController;
};

#endif
