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