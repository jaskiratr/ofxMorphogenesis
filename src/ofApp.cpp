#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofSetVerticalSync(true);
    ofBackground(20);
    
    bShowHelp = true;
    
    bFill       = true;
    bWireframe  = true;
    bDrawNormals= false;
    bDrawAxes   = false;
    bDrawLights = false;
    bInfoText   = true;
    bMousePressed = false;
    bSplitFaces = false;
    
    float width     = ofGetWidth() * .12;
    float height    = ofGetHeight() * .12;
    
    
    //---------------FORCES---------------
    springForce = true;
    planarForce = true;
    bulgeForce = true;
    collisionOffset = true;
    
    show_force = true;
    show_sphere = true;
    show_springf = false;
    show_planarf = false;
    show_bulgef = false;
    show_collisiont=false;
    
    
    play = false;
    
    // For initial points generation
    mag = 50;
    phi = phi * mag;
    
    // Factors for physics
    linked_len = 10;
    springFactor = .1;      // To maintain linked particles at equidistant
    planarFactor = 0.001 ;       // Restoration Factor = Pull towards the center
    bulgeFactor = 0;      // bulge outwards due to linked particles
    roi = 30;       // Radius of influence for Collision offset
    repulsion = 0.00;   // Factor for collision offset
    friction = 0;

    
    //---------------LIGHTS---------------
    ofSetSmoothLighting(true);
    
    pointLight.setDiffuseColor( ofFloatColor(.85, .85, .55) );
    pointLight.setSpecularColor( ofFloatColor(1.f, 1.f, 1.f));
    
    pointLight2.setDiffuseColor( ofFloatColor( 238.f/255.f, 57.f/255.f, 135.f/255.f ));
    pointLight2.setSpecularColor(ofFloatColor(.8f, .8f, .9f));
    
    pointLight3.setDiffuseColor( ofFloatColor(19.f/255.f,94.f/255.f,77.f/255.f) );
    pointLight3.setSpecularColor( ofFloatColor(18.f/255.f,150.f/255.f,135.f/255.f) );
    
    //-----------Intitialize points---------------------
    setup_points();
    
    //GUI
    gui0 = new ofxUISuperCanvas("FORCE FACTORS");
    gui0->addSlider("LINKED LEN", linked_len, 100.0, 10.0);
    gui0->addSlider("SPRING FACTOR", springFactor, 100.0, 0.01);
    gui0->addSlider("PLANAR FACTOR", planarFactor, .01, 0.001);
    gui0->addSlider("BULGE FACTOR", bulgeFactor,100, 0.001);
    
    gui0->addSlider("ROI", roi, 100.0, 0.0);
    
    gui0->addSlider("REPULSION FORCE", repulsion, 10.0, 0.0);
    gui0->addSlider("FRICTION", friction, 100.0, 0.0);
    
    
    gui0->addLabel("TOGGLES");
    gui0->addToggle("PLAY", false);
    gui0->addToggle("CAM", false);
    gui0->addLabel("RESET");
    gui0->addButton("RESET", false, 44, 44);

    gui0->autoSizeToFitWidgets();
    ofAddListener(gui0->newGUIEvent, this, &ofApp::guiEvent);
    gui0->loadSettings("settings.xml");

    
    
}

//--------------------------------------------------------------
void ofApp::update(){
    
    //---------------LIGHTS Movement---------------
    pointLight.setPosition((ofGetWidth()*.5)+ cos(ofGetElapsedTimef()*.5)*(ofGetWidth()*.3), ofGetHeight()/2, 500);
    pointLight2.setPosition((ofGetWidth()*.5)+ cos(ofGetElapsedTimef()*.15)*(ofGetWidth()*.3),
                            ofGetHeight()*.5 + sin(ofGetElapsedTimef()*.7)*(ofGetHeight()), -300);
    pointLight3.setPosition(
                            cos(ofGetElapsedTimef()*1.5) * ofGetWidth()*.5,
                            sin(ofGetElapsedTimef()*1.5f) * ofGetWidth()*.5,
                            cos(ofGetElapsedTimef()*.2) * ofGetWidth()
                            );
    
    //---------------Calculate Forces---------------
    //Spring Force --- Maintain constant distant from each other
    if(springForce)
    {
        
        for (int i=0; i<=pt.size(); i++)
        {   ofVec3f sum = ofVec3f(0,0,0);
            for (int j=0; j<=pt.size(); j++)
            {
                if((pt[j].pos).distance(pt[i].pos)<linked_len*2 && (pt[i].pos!=pt[j].pos)){           // Compare vectors to find nearest linked members
                    sum += linked_len * (pt[j].pos -pt[i].pos).getNormalized();
                }
            }
            pt[i].springTarget = pt[i].pos + sum/(pt.size());   // Calculate Target Positions for Respective Force
            
        }
        
    }
    
    //Planar Force --- Pull towards the center
    if(planarForce)
    {
        for (int i=0; i<pt.size(); i++)
        {   ofVec3f sum = ofVec3f(0,0,0);
            for (int j=0; j<pt.size(); j++)
            {
                if((pt[j].pos).distance(pt[i].pos)<linked_len*2 && (pt[i].pos!=pt[j].pos)){           // Compare vectors to find nearest linked members
                    sum += pt[j].pos;
                }
            }
            pt[i].planarTarget = sum/(pt.size()-1);
        }
    }
    
    //Bluge Force --- Push Outwards
    if (bulgeForce)
    {
        
        
        for (int i=0; i<pt.size(); i++)
        {   ofVec3f sum = ofVec3f(0,0,0);
            float dotNr;
            ofVec3f normal = ofVec3f(0,0,0);
            float sum_inner;
            ofVec3f bulge_dist = ofVec3f(0,0,0);
            float Lr_mag = 0;
            float normal_factor = 1/100;
            
            for (int j=0; j<pt.size(); j++)
            {
                if((pt[j].pos).distance(pt[i].pos)<linked_len*2 && (pt[i].pos!=pt[j].pos))
                {
                    dotNr = (pt[j].pos- pt[i].pos).dot(pt[i].pos* normal_factor);
                    Lr_mag = pow(pow(pt[j].pos.x,2)+pow(pt[j].pos.y,2)+pow(pt[j].pos.z,2), 1/2);
                    //cout<<"DOTNr : "<<dotNr<<"\n";
                    //sum_inner =(pow(rest_len,2.0)- pow(Lr_mag,2) + (dotNr*dotNr));
                    sum_inner += Lr_mag + dotNr ;
                    bulge_dist = bulge_dist + sum_inner + dotNr;
                }
                bulge_dist/= (pt.size()-1);
            }
            pt[i].bulgeTarget = pt[i].pos + bulge_dist*pt[i].pos.getNormalized();
            
        }
        
    }
   
    //    cout<<collisionOffset<<"\n";
    //Collision Offset
    if(collisionOffset){
//        ofVec3f sum = ofVec3f(0,0,0);       //WRONG??
//        ofVec3f diff = ofVec3f(0,0,0);
        for (int i=0; i<pt.size(); i++)
        {
                        ofVec3f sum = ofVec3f(0,0,0);     //RIGHT?
                        ofVec3f diff = ofVec3f(0,0,0);
            for (int j=0; j<pt.size(); j++)
            {
                if(((pt[j].pos).distance(pt[i].pos)<roi)&&((pt[j].pos).distance(pt[i].pos)>linked_len))
                {
                    diff = pt[i].pos-pt[j].pos;
                    sum+= ((roi*roi - pow((diff.x*diff.x + diff.y*diff.y + diff.z*diff.z), 1/2))/ pow(roi, 2.0f))* (pt[i].pos-pt[j].pos).getNormalized() ;
                }
            }
            pt[i].collisionTarget = repulsion * sum;
        }
    }
    //---------------POINTS NEW POS UPDATE---------------
    if(play){
        for (int i=0; i<pt.size(); i++)
        {   newPos = pt[i].pos + springFactor * (pt[i].springTarget-pt[i].pos)
            + planarFactor * (pt[i].planarTarget-pt[i].pos)
            + bulgeFactor  * (pt[i].bulgeTarget-pt[i].pos);
            newPos+= 20*pt[i].collisionTarget;
            
            if ( pt[i].pos.distance(newPos)>friction){
                pt[i].pos = newPos;
                
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    //----------------Video Recording Text----------------------
    stringstream ss;
    ss<< "FPS: " << ofGetFrameRate() << endl
    << (record?"pause":"start") << " recording: r" << endl
    << (record?"close current video file: e":"") << endl;
    ofSetColor(0,0,0,100);
    ofRect(0, 0, 260, 75);
    ofSetColor(255, 255, 255);
//    ofDrawBitmapString(ss.str(),15,15);       // DESCRIPTION
    cam.begin();

//        cam.enableOrtho();
//        cam.dolly(1.5);
    //    cam.rotateAround(1, camCenter, camCenter);
    ofRotateX(ofRadToDeg(.5));
    ofRotateY(ofRadToDeg(-.5));
    
    //----------------------------------------------------------
    
    ofEnableDepthTest();
    ofEnableLighting();
    
    pointLight.enable();
    pointLight2.enable();
    pointLight3.enable();
    
    ofSetColor(255);
    ofNoFill();
    
    ofSetSphereResolution(30);
    //Background Mesh
    ofSetLineWidth(1);
    ofDrawSphere(ofGetWidth()/2, ofGetHeight()/2, ofGetWidth()*2);
    
    ofPushMatrix();
    ofTranslate(0,0,20);
    
    // Drawing points
//    if (num>0)
    {
//        cout<<"DRAWING NOW";
        for (int i=0; i<pt.size(); i++)
        {
            pt[i].create();
            if(show_sphere)
            {
                pt[i].draw();
            }
            if(show_force){
                ofSetLineWidth(1);
                ofSetColor(0,0,255);
                ofLine(0, 0, 0, pt[i].pos.x, pt[i].pos.y, pt[i].pos.z);     // SHOW FORCES
                ofSetLineWidth(2);
                ofSetColor(255,10,140);
                
                for (int i=0; i<pt.size(); i++)
                {
                    for (int j=0; j<pt.size(); j++)
                    {
//                     `   ofRotateX(45);
//                        ofCircle(pt[i].pos, 20);
                        if((pt[j].pos).distance(pt[i].pos)<=(linked_len*2+5)){          // LINKED LENGTH X 2 ??
                            ofLine(pt[i].pos.x, pt[i].pos.y, pt[i].pos.z, pt[j].pos.x, pt[j].pos.y, pt[j].pos.z);
                        }
                    }
                }
            }
            
            //            cout<<"size: "<<pt.size()<<" pos:"<<i<<" : "<< pt[i].pos<<"\n";
            //            cout<<"FrameRate: "<<ofGetFrameRate()<<"\n";
            if(show_springf){
                ofSetLineWidth(3);
                ofSetColor(0,255,0);
                ofLine(pt[i].springTarget.x*2, pt[i].springTarget.y*2, pt[i].springTarget.z*2, pt[i].pos.x, pt[i].pos.y, pt[i].pos.z);     // show spring force
            }
            if(show_planarf){
                ofSetLineWidth(3);
                ofSetColor(255,0,0);
                ofLine(pt[i].planarTarget.x, pt[i].planarTarget.y, pt[i].planarTarget.z, pt[i].pos.x, pt[i].pos.y, pt[i].pos.z);     // show spring force
                //                cout<<"ShowPLanar"<<"\n";
            }
            if(show_bulgef){
                ofSetLineWidth(3);
                ofSetColor(255,0,0);
                ofLine(pt[i].bulgeTarget.x*1.5, pt[i].bulgeTarget.y*1.5, pt[i].bulgeTarget.z*1.5, pt[i].pos.x, pt[i].pos.y, pt[i].pos.z);
                //                cout<<pt[i].bulgeTarget.x<<" "<<pt[i].pos.x<<"\n";
            }
            if(show_collisiont){
                ofSetLineWidth(3);
                ofSetColor(255,255,0);
                ofLine(pt[i].collisionTarget.x*1.5, pt[i].collisionTarget.y*1.5, pt[i].collisionTarget.z*1.5, pt[i].pos.x, pt[i].pos.y, pt[i].pos.z);
                //                cout<<pt[i].bulgeTarget.x<<" "<<pt[i].pos.x<<"\n";
            }
            
        }
        
    }
    
    ofPopMatrix();
    //----------------------------CAM
    //    ofSetColor(255,0,0);
    //    ofFill();
    //    ofDrawBox(30);
    //    ofNoFill();
    //    ofSetColor(0);
    //    ofDrawBox(30);
    
    //    ofPushMatrix();
    //    ofTranslate(0,0,20);
    //    ofSetColor(0,0,255);
    //    ofFill();
    //    ofDrawBox(5);
    //    ofNoFill();
    //    ofSetColor(0);
    //    ofDrawBox(5);
    //    ofPopMatrix();
    cam.end();
//    drawInteractionArea();
    ofSetColor(255);
    //-------------------------------
    
    //    --------------------RECORD----------------------
    if (record == true){
        string frameCount = ofToString(20000+ofGetFrameNum());
        string fileName = "filmImages/" + currentDate + "/" + frameCount + ".png";
        ofSaveScreen(fileName);
        int recFrames = ofGetFrameNum()-recStart;
        float recTime = recFrames / 60; //or whatever your Framerate is
        cout << "Frames: " << recFrames << endl;
        cout << "Time: " << recTime << endl;
        //The above lines are so you can track the number of frames you've saved and the rough length of your final video
    }
    //-----------------CAM------------------
    string msg = string("Using mouse inputs to navigate (press 'c' to toggle): ") + (cam.getMouseInputEnabled() ? "YES" : "NO");
    msg += string("\nShowing help (press 'h' to toggle): ")+ (bShowHelp ? "YES" : "NO");
    if (bShowHelp) {
        msg += "\n\nLEFT MOUSE BUTTON DRAG:\nStart dragging INSIDE the yellow circle -> camera XY rotation .\nStart dragging OUTSIDE the yellow circle -> camera Z rotation (roll).\n\n";
        msg += "LEFT MOUSE BUTTON DRAG + TRANSLATION KEY (" + ofToString(cam.getTranslationKey()) + ") PRESSED\n";
        msg += "OR MIDDLE MOUSE BUTTON (if available):\n";
        msg += "move over XY axes (truck and boom).\n\n";
        msg += "RIGHT MOUSE BUTTON:\n";
        msg += "move over Z axis (dolly)";
    }
    msg += "\n\nfps: " + ofToString(ofGetFrameRate(), 2);
//    ofDrawBitmapStringHighlight(msg, 10, 20);
    
    
}
void ofApp::setup_points(){
    
    pt.push_back(points(0, 0, phi));  //1
    pt.push_back(points(0, 0, -phi));
    
    pt.push_back(points(0, phi, 0 ));   //2
    pt.push_back(points(0, -phi, 0));
    
    pt.push_back(points(phi, 0, 0));    //3
    pt.push_back(points(-phi, 0, 0));
    
    pt.push_back(points(mag/2, phi/2, (mag+phi)/2));
    pt.push_back(points(mag/2, phi/2, -(mag+phi)/2));
    pt.push_back(points(mag/2, -phi/2, (mag+phi)/2));
    pt.push_back(points(mag/2, -phi/2, -(mag+phi)/2));
    pt.push_back(points(-mag/2, phi/2, (mag+phi)/2));
    pt.push_back(points(-mag/2, phi/2, -(mag+phi)/2));
    pt.push_back(points(-mag/2, -phi/2, (mag+phi)/2));
    pt.push_back(points(-mag/2, -phi/2, -(mag+phi)/2));
    
    pt.push_back(points(phi/2, (mag+phi)/2, mag/2));
    pt.push_back(points(phi/2, (mag+phi)/2, -mag/2));
    pt.push_back(points(phi/2, -(mag+phi)/2, mag/2));
    pt.push_back(points(phi/2, -(mag+phi)/2, -mag/2));
    pt.push_back(points(-phi/2, (mag+phi)/2, mag/2));
    pt.push_back(points(-phi/2, (mag+phi)/2, -mag/2));
    pt.push_back(points(-phi/2, -(mag+phi)/2, mag/2));
    pt.push_back(points(-phi/2, -(mag+phi)/2, -mag/2));
    
    pt.push_back(points((mag+phi)/2, mag/2, phi/2));
    pt.push_back(points((mag+phi)/2, mag/2, -phi/2));
    pt.push_back(points((mag+phi)/2, -mag/2, phi/2));
    pt.push_back(points((mag+phi)/2, -mag/2, -phi/2));
    pt.push_back(points(-(mag+phi)/2, mag/2, phi/2));
    pt.push_back(points(-(mag+phi)/2, mag/2, -phi/2));
    pt.push_back(points(-(mag+phi)/2, -mag/2, phi/2));
    pt.push_back(points(-(mag+phi)/2, -mag/2, -phi/2));
}

void ofApp::drawInteractionArea(){
    ofRectangle vp = ofGetCurrentViewport();
    float r = MIN(vp.width, vp.height) * 0.5f;
    float x = vp.width * 0.5f;
    float y = vp.height * 0.5f;
    
    ofPushStyle();
    ofSetLineWidth(3);
    ofSetColor(255, 255, 0);
    ofNoFill();
    glDepthMask(false);
    ofCircle(x, y, r);
    glDepthMask(true);
    ofPopStyle();
}
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    if(e.getName() == "LINKED LEN")
    {
        ofxUISlider *slider = e.getSlider();
        linked_len= (slider->getScaledValue());
    }
    if(e.getName() == "SPRING FACTOR")
    {
        ofxUISlider *slider = e.getSlider();
        springFactor= (slider->getScaledValue());
    }
    if(e.getName() == "PLANAR FACTOR")
    {
        ofxUISlider *slider = e.getSlider();
        planarFactor= (slider->getScaledValue());
    }
    if(e.getName() == "BULGE FACTOR")
    {
        ofxUISlider *slider = e.getSlider();
        bulgeFactor= (slider->getScaledValue());
    }
    if(e.getName() == "REPULSION FORCE")
    {
        ofxUISlider *slider = e.getSlider();
        repulsion= (slider->getScaledValue());
    }
    if(e.getName() == "FRICTION")
    {
        ofxUISlider *slider = e.getSlider();
        friction= (slider->getScaledValue());
    }
    
    if(e.getName() == "PLAY")
    {
        ofxUIToggle *toggle = e.getToggle();
        play = (toggle->getValue());
        cam.disableMouseInput();
    }
    if(e.getName() == "CAM")
    {
        if(cam.getMouseInputEnabled()) cam.disableMouseInput();
        else cam.enableMouseInput();
    }
    if(e.getName() == "RESET")
    {
//        play = false;
        reset();
        
    }
   
}
void ofApp::reset()
{
    pt.clear();
    setup_points();
    cout<< pt.size()<<"\n";
}
//--------------------------------------------------------------
void ofApp::exit()
{
    gui0->saveSettings("settings.xml");
    delete gui0;
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    int r=radius;      // Radius
    
    if(key == 'v'){
        
        int size = pt.size();
        for(int i = 0; i< size; i++){
            float x = pt[i].x + ofRandom(-30, 30);    // Find direction from center
            float y = pt[i].y + ofRandom(-30, 30);    // Then add accordingly??
            float z = pt[i].z + ofRandom(-30, 30);
            
            pt.push_back(points(x,y,z));
            
            cout<< pt.size()<<"\n";
        }
        
    }
    
    // Initializing points
    
    //    cout<<num<<"\n";
    
    //    -----------VIDEO RECORD----------------
    if(key == 'R')
    {
        record = !record;
        recStart = ofGetFrameNum();
        currentDate = ofGetTimestampString();
    }
    
    if(key == 'a')
    {
        show_force = !show_force;
    }
    if(key == 's')
    {
        show_sphere=!show_sphere;
    }
    if(key == '1')
    {
        springForce=!springForce;
        cout<<"springForce: "<<springForce<<"\n";
    }
    if(key == '2')
    {
        planarForce=!planarForce;
        cout<<"planarForce: "<<planarForce<<"\n";
    }
    if(key == '3')
    {
        bulgeForce=!bulgeForce;
        cout<<"bulgeForce: "<<bulgeForce<<"\n";
    }
    if(key == '4')
    {
        collisionOffset=!collisionOffset;
        cout<<"CollisionOffset: "<<collisionOffset<<"\n";
    }
    if(key == 'q')
    {
        show_springf=!show_springf;
    }
    if(key == 'w')
    {
        show_planarf=!show_planarf;
    }
    if(key == 'e')
    {
        show_bulgef=!show_bulgef;
    }
    if(key == 'p')
    {
        play=!play;
    }
    if(key == 't')
    {
        show_collisiont=!show_collisiont;
    }
    
    if(key == ']'){
        radius++;
    }
    if(key == '['){
        radius--;
    }
    switch(key) {
        case 'C':
        case 'c':
            if(cam.getMouseInputEnabled()) cam.disableMouseInput();
            else cam.enableMouseInput();
            break;
            
        case 'F':
        case 'f':
            ofToggleFullscreen();
            break;
        case 'H':
        case 'h':
            bShowHelp ^=true;
            break;
        case 'i':
            springFactor++;
            cout<<springFactor<<"\n";
            break;
        case 'o':
            springFactor--;
            cout<<springFactor<<"\n";
            break;
        case 'g':
        {
            gui0->toggleVisible();
        }
            break;
        case 'r':
        {
            reset();
        }
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
   
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
   
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
