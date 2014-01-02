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
#define NUM_GESTURES 5
#define NUM_PLACES 8

typedef enum
{
	GameStateWaitingForSignIn,
	GameStatePlaying,
    GameStateDeciding,
	GameStateGameOver
}
GameState;

typedef enum
{
	GameActionFreeze,
    GameActionTouch,
    GameActionJump,
    GameActionShake,
    GameActionSpin
}
GameAction;



typedef struct{

	float x;
	float y;
	bool bBeingDragged;
	bool bOver;
	float radius;

} draggableVertex;

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
    ofTrueTypeFont fontSmall;

    int connectedAgents;

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

    bool isClient = false;
    bool isServer = false;

    // STUFF RELATED TO SECRET AGENT

    GameState state;

    string mainMessage;

    unsigned long long elapsed, turnTime, recordedTime;
    bool recordMode;
    bool isSpy;
    int spyIndexAccordingToServer;
    int pickerIndexAccordingToServer;

    string placeString[NUM_PLACES] = {"1st","2nd","3rd","4th","5th","6th","7th","8th"};

    string actionString[NUM_GESTURES] = {"FREEZE","JUMP","TOUCH","SHAKE","SPIN"};


    ofVec3f userAccelerationArray[8];


    // GAME STATE FUNCTIONS

    void pickedAgent();
    void buttonPress();
    void startGame();
    void serveRound(int curstep);
    void execute(string gesture);

    void countDown(int curstep);


    // ART DEPT

    int nCurveVertices;
    draggableVertex curveVertices[7];
    draggableVertex bezierVertices[4];

};

#endif /* defined(__DoubleAgent__agentController__) */
