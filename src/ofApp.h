#pragma once

#include "ofMain.h"
#include "point.h"
#include "ofxUI.h"
#include "ofxMetaballs.h"


class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    void exit(); 
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    void setup_points();
    void reset();
    
    bool bFill;
    bool bWireframe;
    bool bDrawNormals;
    bool bDrawAxes;
    bool bDrawLights;
    bool bInfoText;
    bool bMousePressed;
    bool bSplitFaces;
    
    int mag = 1;
    float phi = 1.6180339887;
    
    
    int num_points = 0;
    float angle1 = 10;
    float angle2 = 3;
    
//    int rest_len = 2;
    float linked_len = 0;
    float friction =0 ;
    
    bool springForce;
    bool planarForce;
    bool bulgeForce;
    bool collisionOffset;
    
    bool show_force;
    bool show_sphere;
    bool show_springf;
    bool show_planarf;
    bool show_bulgef;
    bool show_collisiont;
    bool play;
    
    ofVec3f newPos;
    
    
    float springFactor = 0;
    float planarFactor = 0;
    float bulgeFactor = 0;
    float roi = 0;
    float repulsion = 0;
    
    vector<points> pt;
    float r,s,t;
    int num=0;
    int radius = 100; // Radius of initializing spheres
    float max_dist = 0;
    
    ofLight pointLight;
    ofLight pointLight2;
    ofLight pointLight3;
    
    
    
    
    
    bool record;
    float recStart;
    string currentDate;
    
    //Easycam
    void drawInteractionArea();
    bool bShowHelp;
    ofEasyCam cam; // add mouse controls for camera movement
    
    ofxUISuperCanvas *gui0;
    ofxUISuperCanvas *gui1;
    void guiEvent(ofxUIEventArgs &e);
    bool drawPadding;
    float red, green, blue;
    
    MarchingCubes iso;
    
    
};
