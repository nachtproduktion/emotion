#ifndef emotion_CharacterPoint_h
#define emotion_CharacterPoint_h

#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Sphere.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Timeline.h"

#include <list>
#include <vector>

#include "MSAPhysics.h"
#include "MSAPhysics3D.h"

#include "niko_functionen.h"
#include "Constants.h"

extern int fps;

class CharacterPoint {
    public:
        CharacterPoint();
        CharacterPoint( Vec3f _pos, MSA::Physics::World3D * _physics, int _pID );
    
        void setParticle();
        Physics::Particle3D* getParticle();
    
        void setParent( CharacterPoint* _parent );
        void addChild( CharacterPoint* _child );
        void clearChilds();
    
    
        CharacterPoint* getSuperParent();
        void setSuperParent( CharacterPoint* _superparent );
    
        CharacterPoint* getParent();
        CharacterPoint* getChild( int _index );
        int getNumberOfChilds();

        void addSpacer( CharacterPoint* _spacer );
        void clearSpacers();
        CharacterPoint* getSpacer( int _index );
        int getNumberOfSpacers();
    
        int getID();
    
        void addBondID( int _id );
        int getNumberOfBonds();
        int getBondID( int _index );
    
        void setStandBondID( int _id );
        int getStandBondID();
    
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
        Vec3f* getPositionPointer();
    
        void setEndOfLine( bool _eol );
        bool getEndOfLine();
        bool getActive();
    
        void calcShellRadius();
        float getShellRadius();
    
        void moveTo( Vec3f _target );
        void moveBy( Vec3f _dir );
    
        void addAnimToPosition();
        void posToSave();
    
        void render();

    
        Vec3f savePosition;
        Vec3f saveTarget;
    
        int   mLevel;
        int   mFunction;
    
        Anim<Vec3f>		mAnimPos;
    
        
    
    private:
        CharacterPoint *        mSuperParent;
        CharacterPoint *        mParent;
        std::vector<  CharacterPoint* > mChilds;
    
        std::vector<  CharacterPoint* > mSpacers;
    
        Vec3f                   mPosition;
    
        bool                    mEndOfLine;
        bool                    mActive;
        int                     mParticleControllerID;
        std::vector <int>       mBondIDs;
        int                     mStandBondID;
    
        float                   mShellRadius;
    
        //Physic
        Physics::World3D*       mPhysic;
        Physics::Particle3D*    mParticle;
        int                     mParticleID;
    
        //std::map< MainPoint*, float > mNeighbours;
};

#endif