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
#include "ofxNetwork.h"
#include "AgentCommon.h"
#include "AgentView.h"

class agentController {
	
public:
    
// NETWORKING
    void    setIpAddress(const char *ipAddress);
    
// SENSORS
//    void    updateAccel(ofVec3f newAccel);
    void    updateOrientation(ofMatrix3x3 newOrientationMatrix, ofMatrix3x3 newDeltaOrientationMatrix);
//    bool gestureCompleted;
    
// OPENFRAMEWORKS
    void    setup();
    void    update();
    void    draw();
    void    exit();
	void    pause();
	void    resume();
    void    touchBegan(int x, int y, int id);
    void    touchMoved(int x, int y, int id);
    void    touchEnded(int x, int y, int id);
    
// all the things i needed to make public to break out the display function
    string  getCodeFromIp();
    string  getCodeFromInt(int num);
    int     loginCode = 0;
    bool    preGameCountdownSequence;
    float   recordedSensorData[NUM_TURNS][SENSOR_DATA_ARRAY_SIZE];
    string  mainMessage;   // the action command, used for display and orientation within the game loop
    bool    animatedScrambleFont;    // turns to false on everybody's phone the moment the execute function happens
    int     connectedAgents;  // client stores from server server.getNumClients()
    bool    useScrambledText;    // (true: display spyMess, false: mainMessage) dynamically switches on DAs phone, and everybody elses, remains true on DAs phone during execute function
    char    avatarSelf = 1;  // your own avatar
    // storing your friends' avatar information
    char    avatarIcons[256];
    char    avatarColors[256];
    short   avatarNum;
    
    ofMatrix3x3 orientation; // device orientation
    unsigned long long turnTime;  // beginning of each turn. for calculating reaction time
    
    short   spyAvatar;
    short   spyColor;
    
    
    int         turnNumber;   // initialized to 0 at GameStateStartGame or GameStateBeginCountdown
                              // but immediately at StateTurnScramble, it goes to 1.
                              // increments every StateTurnScramble
                              // so really, turnNumber counts up from 1, turn 1 is 1, turn 3 is 3.

    bool        isSpy;    // set when client receives "spy" or "notspy"

private:
    
    AgentView agentView;

// DOUBLE AGENT GAME
    // cover your eyes
    // make your animal's sound
    
    // TO ADD MORE GESTURES:
    // increment NUM_GESTURES in AgentCommon.h
    // in execute(), assign it to a gesture record type
    //
    // strings programmed to be able to handle one \n, and display it properly
    
    string      actionString[NUM_GESTURES] = {
        "NOTHING",
        "JUMP",
        "TOUCH SCREEN",
        "SHAKE ME",
        "SPIN",
        "HIGH FIVE\nNEIGHBOR",
        "POINT AT\nAN AGENT",
        "FREEZE",
        "CROUCH",
        "STAND ON\nONE LEG",
        "TOUCH SCREEN\nWITH NOSE",
        "RAISE\nA HAND",
        "RUN IN PLACE",
        "COVER YOUR EYES",
        "MAKE YOUR\nANIMAL'S SOUND",
        "CHICKEN DANCE",
        "TOUCH YOUR TOES"};
    
    RecordMode  recordMode;  // gesture type, what kind of motion data to capture   // is 0 being used properly?
    bool        gestureHasOccurred(string message);     // prevent duplicating gestures per round
    string      previousActions[NUM_TURNS];  // prevent repeating actions per round, history of moves. gets cleared every round start
    int         spyAccordingToServer;  // relates to connectedclient
    void        execute(string gesture);   // the moment a turn begins, timers start
    void        updateOnceASecond();
    int         oneSecond;  // keep track for updateOnceASecond()
    
    // server-only functions
    void        serverInitiateRound();
    void        generateNewSpyRoles();   // called during "startGame"
    void        countScores();
    void        pickedAgent(int agent);
    
// SOUNDS
    ofSoundPlayer   clickUp, clickDown;
    
// SCRIPTING
    long            elapsedMillis;  // reduce calls to ofGetElapsedMillis()
    ProgramState    state = StateWelcomeScreen;
    NetworkState    networkState = NetworkNone;
    void            updateState(ProgramState newState);
    long            stateBeginTime;
    
    // delayed transitions
    void            updateStateWithTransition(ProgramState newState, long delay);
    ProgramState    transitionTarget;
    long            transitionEndTime;
    long            transitionDuration;
    bool            transitionActive = false;
    
// ERROR HANDLING
    bool            errorMessageActive = false;
    string          errorMessage;
    long            errorBeginTime;
    
// NETWORKING
	ofxTCPServer    server;
	ofxTCPClient    client;
	std::string     localIP;
	std::string     serverIP;
    string          makeServerIPString();
    string          Rx;
    char            receivedText[128];  // c string of Rx
    bool            isClient = false;
    bool            isServer = false;
    void            updateTCP();  // packet sniffer, server and client
    void            updateSlowTCP();  // packet sniffer, server and client, only called once per second
    void            sendMessage(string message);   // if client, send to server.  if server, send to all clients
    bool            isConnectedToWIFI();
    int             hostIp;
    // connect
    bool            serverConnect();
    void            stopServer();
    bool            clientConnect(string serverIP);

// SENSORS
	ofVec3f         accel, normAccel, filteredAccel;
    int             accelIndex = 0;  // filter array index
    float           getMaxSensorScale(); // grab max value from deltaOrientation;
    float           sumSensors();  // a general sense for how much movement is happening
    //updated sensor
    ofMatrix3x3     deltaOrientation;  // change in orientation. at rest, is the identity matrix
    void            logMatrix3x3(ofMatrix3x3 matrix);
    void            logMatrix4x4(ofMatrix4x4 matrix);

    // UNUSED?
    float screenScale;
    unsigned long long recordedTimes[16];  // index [0] is always for self. server utilizes all the rest of the indexes, correlates to clientID
    int mouseX, mouseY;
    
    // these are duplicated from the View. presently required for touch. try to get these out of here
    int width, height;
    int centerX, centerY;  // screen Coords
};

#endif /* defined(__DoubleAgent__agentController__) */
