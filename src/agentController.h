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
#define NUM_GESTURES 13
#define NUM_PLACES 8
#define NUM_TURNS 3  // per round
#define ACTION_TIME 3000  // 3 seconds to execute action
#define SENSOR_DATA_ARRAY_SIZE 128

typedef enum
{
	GameStateLogin,
    GameStateReadyRoom,
	GameStatePlaying,
    GameStateDeciding,
	GameStateGameOver
}
GameState;

typedef enum
{
    LoginStateChoose,
    LoginStateClient,
    LoginStateServer,
    LoginStateConnecting,
    LoginStateFailed,
    LoginStateNoIP
}
LoginStateState;

typedef enum
{
    TurnStateNotActive,
    TurnStateReceivingScrambled,
    TurnStateAction,
    TurnStateActionSuccess,
    TurnStateWaiting
}
TurnState;

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
    void updateOrientation(ofMatrix3x3 newOrientationMatrix, ofMatrix3x3 newDeltaOrientationMatrix);
//    void detectShakeGesture();
//    bool gestureCompleted;

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
    ofVec3f userAccelerationArray[SENSOR_DATA_ARRAY_SIZE];
    float recordedSensorData[SENSOR_DATA_ARRAY_SIZE];
    int accelIndex = 0;  // filter array index
    float getMaxSensorScale(); // grab max value from deltaOrientation;
        //updated sensor
    ofMatrix3x3 orientation; // device orientation
    ofMatrix3x3 deltaOrientation;  // change in orientation. at rest, is the identity matrix

    void logMatrix3x3(ofMatrix3x3 matrix);
    void logMatrix4x4(ofMatrix4x4 matrix);

    // STUFF RELATED TO SECRET AGENT
    GameState gameState;
    TurnState turnState;
    typedef void (*StepFunctionPtr)(int);
    void (agentController::*stepFunction)(int);
    void countDown(int curstep);  // can be stepFunction
    void serveRound(int curstep); // can be stepFunction   (server only function)
    int step;  // game loop interval. used for countdowns and rounds, increments to 3 for countdown, increments to TURNS*3 for rounds
    int numSteps;  // sets the ceiling of each countdown and round. 3 for countdowns, TURNS*3 for rounds
    int currentTurn;   // resets to 0 each new round
    unsigned long stepInterval;  // period between a step
    unsigned long long stepTimer;  // timestamp beginning of a step to offset against

    string mainMessage;   // the action command, used for display and orientation within the game loop
    string placeString[NUM_PLACES] = {"DATA SENT","DATA SENT","DATA SENT","DATA SENT","DATA SENT","DATA SENT","DATA SENT","DATA SENT"};//{"1st","2nd","3rd","4th","5th","6th","7th","8th"};
    string actionString[NUM_GESTURES] = {"NOTHING","JUMP","TOUCH SCREEN","SHAKE PHONE","SPIN","HIGH FIVE\nNEIGHBOR","POINT AT\nAN AGENT","FREEZE","CROUCH","STAND ON\nONE LEG","TOUCH YOUR\nNOSE","RAISE\nA HAND","RUN IN PLACE"};
    string connectedAgentsStrings[NUM_PLACES+1] = {"", ".", ". .", ". . .", ". . . .", ". . . . .", ". . . . . .", ". . . . . . .", ". . . . . . . ."};
    char spymess[5];  // scrambled text
    bool actionHasOccurred(string message);     // prevent repeating actions per round
    string previousActions[NUM_TURNS];  // prevent repeating actions per round, history of moves. gets cleared every round start

    unsigned long long turnTime; //elapsed;  // beginning of each turn. for calculating reaction time
    unsigned long long recordedTimes[16];  // index [0] is always for self. server utilizes all the rest of the indexes, correlates to clientID

    int recordMode;  // GameAction type   // is 0 being used properly?
    bool isSpy;    // set when client receives "spy" or "notspy"
    bool useScrambledText;    // (true: display spyMess, false: mainMessage) dynamically switches on DAs phone, and everybody elses, remains true on DAs phone during execute function
    bool animatedScrambleFont;    // turns to false on everybody's phone the moment the execute function happens
    bool preGameCountdownSequence;
    int pickerAccordingToServer;
    int spyAccordingToServer;


    void startGame();        // initiated by server with "startGame", used by clients and servers
    void execute(string gesture);   // the moment a turn begins, timers start
    void countScores();       // server only
    void pickedAgent(int agent);

    // LOG IN DATA

    int clientConnect();
    int serverConnect();

    string getCodeFromIp();
    string getCodeFromInt(int num);

    void drawLoginScreen();
    int loginState = 0;
    int loginCode = 0;
    int hostIp;
    float screenScale;

    // OF / UI / UX
    int mouseX, mouseY;
    int centerX, centerY;  // screen Coords
    int width, height;
    char mouseButtonState[128];
    ofTrueTypeFont font;
    ofTrueTypeFont fontSmall;
    ofTrueTypeFont fontMedium;
    ofSpherePrimitive sphere;
    ofImage reticleCompass;
    ofImage reticleOutline;
    ofImage reticleInside;
    ofImage fingerPrint;
    ofImage insideCircle;

    ofImage increment;
    ofImage decrement;
    string lowerTextLine1, lowerTextLine2, lowerTextLine3;

    void drawAnimatedSphereBackground();
    void drawInGameBackground();
};

#endif /* defined(__DoubleAgent__agentController__) */
