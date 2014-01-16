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
#define NUM_TURNS 4  // per round

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

class agentController {
	
public:

    // NETWORKING
    void setIpAddress(const char *ipAddress);

    // SENSORS
    void updateAccel(ofVec3f newAccel);
    void updateMatrix(ofMatrix3x3 newMatrix);

    // OPENFRAMEWORKS
    void setup();
    void update();
    void draw();
    void exit();
	void pause();
	void resume();
    void touchBegan(int x, int y, int id);
    void touchMoved(int x, int y, int id);
    void touchEnded(int x, int y, int id);

    ofMatrix4x4 attitudeMatrix;

private:

    // NETWORKING
	ofxTCPServer server;
	ofxTCPClient client;
    int connectedAgents;  // client stores from server server.getNumClients()
	std::string localIP;
	std::string serverIP;
    bool isClient = false;
    bool isServer = false;
    void updateTCP();  // packet sniffer, server and client
    void updateSlowTCP();  // packet sniffer, server and client, only called once per second
    void sendMessage(string message);   // if client, send to server.  if server, send to all clients
    int oneSecond;  // tracking time, preventing updateSlowTCP() redundant calls

    // SENSORS
	ofVec3f accel, normAccel;
    ofVec3f filteredAccel;
    ofVec3f userAccelerationArray[128];
    int accelIndex = 0;  // filter array index
        //updated sensor
    ofMatrix3x3 orientation;

    void logMatrix3x3(ofMatrix3x3 matrix);
    void logMatrix4x4(ofMatrix4x4 matrix);

    // STUFF RELATED TO SECRET AGENT
    GameState state;
    typedef void (*StepFunctionPtr)(int);
    void (agentController::*stepFunction)(int);
    void countDown(int curstep);  // can be stepFunction
    void serveRound(int curstep); // can be stepFunction   (server only function)
    int step;  // game loop interval. used for countdowns and rounds, increments to 3 for countdown, increments to TURNS*3 for rounds
    int numSteps;  // sets the ceiling of each countdown and round. 3 for countdowns, TURNS*3 for rounds
    unsigned long stepInterval;  // period between a step
    unsigned long long stepTimer;  // timestamp beginning of a step to offset against

    string mainMessage;   // the action command, used for display and orientation within the game loop
    string placeString[NUM_PLACES] = {"1st","2nd","3rd","4th","5th","6th","7th","8th"};
    string actionString[NUM_GESTURES] = {"FREEZE","JUMP","TOUCH","SHAKE","SPIN"};
    char spymess[4];  // scrambled text

    unsigned long long turnTime; //elapsed;  // beginning of each turn. for calculating reaction time
    unsigned long long recordedTimes[16];  // index [0] is always for self. server utilizes all the rest of the indexes, correlates to clientID

    int recordMode;  // GameAction type   // is 0 being used properly?
    bool isSpy;    // set when client receives "spy" or "notspy"
    bool useSpyFont;    // dynamically switches on DAs phone, represents scrambled text or unscrambled
    int pickerAccordingToServer;
    int spyAccordingToServer;

    void startGame();        // initiated by server with "startGame", used by clients and servers
    void execute(string gesture);   // the moment a turn begins, timers start
    void countScores();       // server only
    void pickedAgent(int agent);

    // OF / UI / UX
    int mouseX, mouseY;
    char mouseButtonState[128];
    ofTrueTypeFont font;
    ofTrueTypeFont spyfont;
    ofTrueTypeFont fontSmall;
    ofSpherePrimitive sphere;
    void drawAnimatedBackground();
};

#endif /* defined(__DoubleAgent__agentController__) */
