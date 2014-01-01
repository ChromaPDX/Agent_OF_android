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
#include "ofxNetwork.h"

#define NUM_MSG_STRINGS 20
#define PORT 3456

typedef enum
{
	GameStateWaitingForSignIn,
	GameStatePlaying,
    GameStateDeciding,
	GameStateGameOver
}
GameState;

class agentController {
	
public:
    void setup();
    void update();

    void updateOSC();
    void updateTCP();

    void draw();
    void exit();
    
	void pause();
	void resume();

    void touchBegan(int x, int y, int id);
    void touchMoved(int x, int y, int id);
    void touchEnded(int x, int y, int id);

    void setIpAddress(const char *ipAddress);

    void sendMessage(string message);

	ofxOscSender sender;
	ofxOscReceiver  receiver;

	ofxTCPServer server;
	ofxTCPClient client;


	ofVec3f accel, normAccel;

	string messages[3];

    ofTrueTypeFont font;

    unsigned long long lastTime;

private:


    int step;
    int numSteps;
    unsigned long stepInterval;
    unsigned long long stepTimer;

    typedef void (*StepFunctionPtr)(int);

    //StepFunctionPtr stepFunction;

    void (agentController::*stepFunction)(int);

    //void (*stepFunction)(int) = NULL;

    ofVec3f filteredAccel;

	//char localIP[16];
	std::string localIP;
	std::string broadcastIP;
	std::string serverIP;


    int                             current_msg_string;
    std::string             msg_strings[NUM_MSG_STRINGS];
    float                   timers[NUM_MSG_STRINGS];

    int                             mouseX, mouseY;
    char                    mouseButtonState[128];

    bool isClient;
    bool isServer;

    // STUFF RELATED TO SECRET AGENT

    GameState state;

    string mainMessage;

    unsigned long long elapsed, turnTime;
    bool recordMode;
    bool isSpy;
    int spyIndexAccordingToServer;
    int pickerIndexAccordingToServer;

    string placeString[8] = {"first","second","third","fourth","fifth","sixth","seventh","eighth"};

    ofVec3f userAccelerationArray[8];


    // GAME STATE FUNCTIONS

    void pickedAgent();
    void buttonPress();
    void startGame();

    void countDown(int curstep);

};

#endif /* defined(__DoubleAgent__agentController__) */
