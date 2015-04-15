
//
//  point.h
//  points_sphere
//
//  Created by Jaskirat Randhawa on 3/15/15.
//
//

#ifndef __points_sphere__point__
#define __points_sphere__point__

#include <stdio.h>
#include "ofMain.h"

class points{
public:
    
    //Methods
    void update(int);
    void draw();
    void create();
    
    
    //variables
    float x;
    float y;
    float z;
    float r;    // Radius
    float s;    //angle around the z-axis
    float t;    //height angle, measured 'down' from the z-axis.
    ofVec3f center;
    ofVec3f pos;
    ofSpherePrimitive sphere;
    
    ofVec3f springTarget;
    ofVec3f planarTarget;
    ofVec3f bulgeTarget;
    ofVec3f collisionTarget;
    
    //Constructor
    points(float, float, float);
    
    
private:
    
};

#endif /* defined(__points_sphere__point__) */
