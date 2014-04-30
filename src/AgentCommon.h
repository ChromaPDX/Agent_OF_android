//
//  AgentCommon.h
//  DoubleAgent
//
//  Created by Robby on 4/11/14.
//
//

#ifndef DoubleAgent_AgentCommon_h
#define DoubleAgent_AgentCommon_h


#define NUM_MSG_STRINGS 20
#define PORT 3456
#define NUM_GESTURES 17
#define NUM_PLACES 8
#define NUM_TURNS 3  // per round
#define ACTION_TIME 3000  // 3 seconds to execute action
#define SENSOR_DATA_ARRAY_SIZE 12
#define ERROR_MESSAGE_DURATION 2000

typedef enum
{
    StateWelcomeScreen,
    StateConnectionScreen,
    StateJoinScreen,
    StateReadyRoom,         // BEGIN GAME LOOP
    StateStartGame,         //
    StateCountdown,         //
    StateTurnScramble,      //  // BEGIN TURN LOOP
    StateTurnGesture,       //  //
    StateTurnComplete,      //  // END TURN LOOP
    StateDecide,            //
    StateGameOver           // END GAME LOOP
}
ProgramState;

typedef enum
{
    NetworkNone,
    NetworkWIFIConnect,
    NetworkHostAttempt,
    NetworkHostSuccess,
    NetworkJoinAttempt,
    NetworkJoinSuccess,
    NetworkLostConnection,      // try to make these 2 into 1
    NetworkServerDisconnected   //
}
NetworkState;

typedef enum
{
	RecordModeNothing,
    RecordModeTouch,
    RecordModeOrientation,
    RecordModeAccel,
    RecordModeGyro,
    RecordModeSound
}
RecordMode;

#endif
