void updateAttractors() {
    
    
    
    //float littleRadius = mRadius - mRadius/3;
    
    //Attractors
    /*
     mAttractors[0] = mPhysics.makeParticle(Vec3f(-littleRadius, -littleRadius, -littleRadius));
     mAttractors[1] = mPhysics.makeParticle(Vec3f(-littleRadius, littleRadius, littleRadius));
     mAttractors[2] = mPhysics.makeParticle(Vec3f(littleRadius, -littleRadius, littleRadius));
     mAttractors[3] = mPhysics.makeParticle(Vec3f(littleRadius, littleRadius, -littleRadius));
     mAttractors[4] = mPhysics.makeParticle(Vec3f(-littleRadius, -littleRadius, littleRadius));
     mAttractors[5] = mPhysics.makeParticle(Vec3f(-littleRadius, littleRadius, -littleRadius));
     mAttractors[6] = mPhysics.makeParticle(Vec3f(littleRadius, -littleRadius, -littleRadius));
     mAttractors[7] = mPhysics.makeParticle(Vec3f(littleRadius, littleRadius, littleRadius));
     
     for( int i=0; i<8; i++ ) {
     mAttractors[i]->setRadius(5.0f)->setMass(1.0f)->setBounce(0.0f)->makeFixed();
     
     for(  std::vector<MainPoint>::iterator p = mMainPoints.begin(); p != mMainPoints.end(); ++p ){ 
     if(!p->mEndOfLine) {
     mPhysics.makeAttraction(mAttractors[i],p->getParticle(), 5.0f);
     }
     }
     }
     */
}



/*
 // loop through all particles and add attraction to mouse
 // (doesn't matter if we attach attraction from mouse-mouse cos it won't be added internally
 for(int i=0; i<physics.numberOfParticles(); i++) {
 physics.makeAttraction(&mouseNode, physics.getParticle(i), Rand::randFloat(MIN_ATTRACTION, MAX_ATTRACTION));
 }
 else {
 // loop through all existing attractsions and delete them
 for(int i=0; i<physics.numberOfAttractions(); i++) physics.getAttraction(i)->kill();
 }
 */

//void Character::createParticleController() {
    
    //Kreise an Splines
    
    
    //    
    //    
    //    int numSegments = 50;
    //    for(  std::vector<BSpline3f>::iterator p = mSplines.begin(); p != mSplines.end(); ++p ){  
    //        for( int s = 0; s <= numSegments; ++s ) {
    //            float t = s / (float)numSegments;
    //            mParticleController.push_back( ParticleController( ) );
    //            mParticleController.back().mPosition = p->getPosition( t );
    //            
    //            
    //            float sradius = niko::mapping(s, 0, numSegments, 10.0f, 20.0f);
    //            sradius = Rand::randFloat(sradius-2, sradius+2);
    //            
    //            
    //            mParticleController.back().setCircle( sradius );
    //        }
    //    }
    
    
    
    
    //Kugeln an Splines
    //    int numSegments = 20;
    //    for(  std::vector<BSpline3f>::iterator p = mSplines.begin(); p != mSplines.end(); ++p ){  
    //        for( int s = 0; s <= numSegments; ++s ) {
    //            float t = s / (float)numSegments;
    //            mParticleController.push_back( ParticleController( ) );
    //            mParticleController.back().mPosition = p->getPosition( t );
    //            
    //            
    //            float sradius = niko::mapping(s, 0, 18, 20.0f, 10.0f);
    //            sradius = Rand::randFloat(sradius-5, sradius+5);
    //            if(s > 17) sradius = 2.0f;
    //            
    //            
    //            mParticleController.back().setSphere( sradius );
    //        }
    //    }
    
    
    //Kugeln an Knotenpunkten
    //    int counter = 0;
    //     
    //    for(  std::vector<CharacterPoint>::iterator p = mCharacterPoints.begin(); p != mCharacterPoints.end(); ++p ){  
    //        
    //        mParticleController.push_back( ParticleController( ) );
    //        mParticleController.back().mPosition = p->getPosition();
    //        mParticleController.back().setSphere( p->getShellRadius() );
    //        p->setParticleControllerID( counter );
    //        counter++;
    //
    //    }
    
//}
