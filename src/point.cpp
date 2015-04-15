//
//  point.cpp
//  points_sphere
//
//  Created by Jaskirat Randhawa on 3/15/15.
//
//

#include "point.h"

//Implement constructor
points::points(float _x, float _y, float _z)
{
    x=_x;
    y=_y;
    z=_z;
    
    //radians = angleInDegrees * Math.PI / 180
//    s = s * PI / 180;   //radians
//    t = t * PI / 180;   //radians
//    
//    x = r * cos(s) * sin(t);
//    y = r * sin(s) * sin(t);
//    z = r * cos(t);
//    //    pos.set(x,y,z);
//    x+= center.x;
//    y+= center.y;
//    z+= center.z;
    //    cout<<pos<<"\n";
    pos.set(x,y,z);
//        cout<<pos<<"\n"<<"\n";
    //    sphere.setPosition(pos);
    
    
}
void points::create(){
    sphere.setPosition(pos);
}
void points::draw()
{
    ofNoFill();
    ofSetColor(255,255,255);
    ofSetSphereResolution(1);
    //    sphere.setPosition(pos);
//    sphere.setScale(1.01f);
    //    sphere.drawWireframe();
    sphere.setScale(1.5f);
    //    ofSetColor(0);
    sphere.draw();
}

void points::update(int _num)
{   int num = _num;
    int rest_len = 1;
    
    pos=pos+(1/num)*rest_len*num;
    //    x= ofGetAppPtr()->mouseX;
    //    y++;
    //    z++;
    //    pos.set(x,y,z);
    //    cout<<num;
}
