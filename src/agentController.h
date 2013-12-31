//
//  agentController.h
//  DoubleAgent
//
//  Created by Chroma Developer on 12/30/13.
//
//

#ifndef __DoubleAgent__agentController__
#define __DoubleAgent__agentController__

#include "ofMain.h"
#include "ofxOsc.h"

#define NUM_MSG_STRINGS 20
#define PORT 7777

class agentController {
	
public:
    void setup();
    void update();
    void draw();
    void exit();
    
    void touchBegan(int x, int y, int id);
    void touchMoved(int x, int y, int id);
    void touchEnded(int x, int y, int id);

    void setIpAddress(const char *ipAddress);

	ofxOscSender sender;
	ofxOscReceiver  receiver;

	ofVec3f accel, normAccel;

	string messages[3];

    ofTrueTypeFont font;

private:



	//char localIP[16];
	std::string localIP;
	std::string broadcastIP;

    int                             current_msg_string;
    std::string             msg_strings[NUM_MSG_STRINGS];
    float                   timers[NUM_MSG_STRINGS];

    int                             mouseX, mouseY;
    char                    mouseButtonState[128];


};

#endif /* defined(__DoubleAgent__agentController__) */
