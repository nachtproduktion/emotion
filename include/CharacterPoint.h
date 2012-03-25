#ifndef emotion_CharacterPoint_h
#define emotion_CharacterPoint_h

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Sphere.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

#include <list>
#include <vector>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "niko_functionen.h"

extern int fps;

class CharacterPoint {
    public:
        CharacterPoint();
        CharacterPoint( Vec3f _pos, MSA::Physics::World3D * _physics, int _pID );
        
        void postSettings();
    
        void setParticle();
        Physics::Particle3D* getParticle();
    
        void setNeighbours( CharacterPoint* _neighbour );
        int  getNeighboursSize();
    
        void addBondID( int _id );
        int getNumberOfBonds();
        int getBondID( int _index );
    
        void setParticleControllerID( int _id );
        int getParticleControllerID();
        
        void setRadius( float _r );
        float getRadius();
        void setMass( float _m );
        float getMass();
    
        void setFixed();
        bool getFixed();
        void setFree();
    
        Vec3f getPosition();
    
        bool getEndOfLine();
        bool getActive();
    
        void moveTo( Vec3f _target );
        void moveBy( Vec3f _dir );
        
        void render();
        
        std::vector<  CharacterPoint* > mNeighbours;
    
        Vec3f savePosition;
        Vec3f saveTarget;
    
    private:
    
        bool                    mEndOfLine;
        bool                    mActive;
        int                     mParticleControllerID;
        std::vector <int>       mBondIDs;
    
        //Physic
        Physics::World3D*       mPhysic;
        Physics::Particle3D*    mParticle;
        int                     mParticleID;
    
        //std::map< MainPoint*, float > mNeighbours;
};

#endif