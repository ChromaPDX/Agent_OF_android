//
//  agentController.cpp
//  DoubleAgent
//
//  Created by Chroma Developer on 12/30/13.
//
//

#include "agentController.h"

#import "testApp.h"
//#import "ofxTimer.h"

#define TRANSITION_BEGIN_GAME 1500  // fade out to reveal spy roles and start game

string paddedString(int num){
    if (num >= 100)     return ofToString(num);
    else if (num >= 10) return "0" + ofToString(num);
    else                return "00" + ofToString(num);
}

void agentController::setup() {
    
    // GAME / PROGRAM
    updateState(StateWelcomeScreen);
    networkState = NetworkNone;
    turnNumber = 0;

    // TOUCHES
    width = ofGetWidth();
    height = ofGetHeight();
    centerX = ofGetWidth()/2.;
    centerY = ofGetHeight()/2.;
    
    // SOUNDS
    clickDown.loadSound("clickDown.mp3");
    clickUp.loadSound("clickUp.mp3");
    
    agentView.setup();
    agentView.controller = this;
}

void agentController::draw() {
    agentView.draw(state, networkState, elapsedMillis, stateBeginTime, transitionActive, transitionDuration, transitionEndTime, errorMessageActive, errorMessage, errorBeginTime);
}

void agentController::pause(){
    exit();
}

void agentController::resume(){
	if (isServer)
		server.setup(PORT);
	if (isClient && !client.isConnected())
        clientConnect(serverIP);
}

void agentController::exit() {
    updateState(StateConnectionScreen);
}


#pragma mark NETWORK

void agentController::updateSlowTCP(){
    if(isServer){
        // send list of clients
        string clientString;
        clientString.push_back('$');
        clientString.push_back(avatarSelf);
        clientString.push_back(agentView.primaryColor+1);
        for(int i = 0; i < server.getLastID(); i++){
            if(server.isClientConnected(i)){
                clientString.push_back('$');
                clientString.push_back(avatarIcons[i]);
                clientString.push_back(avatarColors[i]);
            }
        }
        sendMessage(clientString);
        
        // send whoever is the spy
        
        if(state == StateDecide || state == StateTurnComplete || state == StateTurnGesture || state == StateTurnScramble){
            string whoisspy;
            whoisspy.push_back('&');
            whoisspy.push_back(spyAvatar);
            whoisspy.push_back(spyColor);
            sendMessage(whoisspy);
        }
//        sendMessage(ofToString(connectedAgents));   // send number of clients
    }
//    else if (isClient) { }
}


void agentController::updateTCP() {
    
	if (isServer){
        // simultaneously count connectedAgents
        // and check if anyone is sending a message
        connectedAgents = 1;
	    for(int i = 0; i < server.getLastID(); i++) { // getLastID is UID of all clients
            if( server.isClientConnected(i) ) { // check and see if it's still around
                connectedAgents++;
                // maybe the client is sending something
                Rx = server.receive(i);
                if (Rx.length()){
                	strcpy( receivedText, Rx.c_str() );
                    ofLogNotice("TCP") << "Agent:" + ofToString(i) + " : " + Rx;
                    if (strcmp(receivedText, "pickedAgent") == 0) {
                        // client's screen got touched during the end of game decision time
                        pickedAgent(i+1);
                    }
                    else if (Rx[0] == '$'){
                        // dollar sign encoding: connected players info, $cc$cc$cc$cc... etc.
                        // first char is avatar animal, second is color
                        // first element is server
                        if(Rx.size() == 3){
                            avatarIcons[i] = Rx[1];
                            avatarColors[i] = Rx[2];
                            printf("client connected! %d %d",avatarIcons[i], avatarColors[i]);
                        }
                    }
                    else {
                        // must be the client sending back performance data for turn gesture
                        recordedTimes[i+1] = ofToInt(Rx);
                    }
                }
            }
            else{
                // manage avatars, clear disconnected clients off the stack
                avatarIcons[i] = 0;
                avatarColors[i] = 0;
            }
	    }
	}
    else if (isClient){
        
    	if (!client.isConnected()){
            // client was once connected, but no longer. feel free to blame the server. this is a terrible restaurant.
    		client.close();
    		isClient = false;
    		connectedAgents = 0;
            networkState = NetworkLostConnection;
            updateState(StateConnectionScreen);
    		return;
    	}
        
        Rx = client.receive();
        if (Rx.length()){
	    	ofLogNotice("TCP") << "Received From Server: " + Rx;
            strcpy( receivedText, Rx.c_str() );
            if (strcmp(receivedText, "stateStartGame") == 0) {
                updateStateWithTransition(StateStartGame, TRANSITION_BEGIN_GAME);
            }
            else if (strcmp(receivedText, "execute") == 0) {
                execute(mainMessage);
            }
            else if (strcmp(receivedText, "spy") == 0) {
                isSpy = true;
                agentView.setIsSpy(isSpy);
            }
            else if (strcmp(receivedText, "notspy") == 0) {
                isSpy = false;
                agentView.setIsSpy(isSpy);
            }
            else if (strcmp(receivedText, "stateDecide") == 0) {
                ((testApp*) ofGetAppPtr())->vibrate(true);
                mainMessage = "OPERATIVE I.D.";  // "PICK"
                updateState(StateDecide);
            }
            else if (strcmp(receivedText, "WIN") == 0) {
                mainMessage = "WIN";
                updateState(StateGameOver);
            }
            else if (strcmp(receivedText, "CAPTURED") == 0) {  // same as WIN, but this means you were the double agent
                mainMessage = "CAPTURED";
                updateState(StateGameOver);
            }
            else if (strcmp(receivedText, "LOSE") == 0) {
                mainMessage = "LOSE";
                updateState(StateGameOver);
            }
            else if (Rx[0] == '&'){
                // server is sendind you the icon and color of the spy
                spyAvatar = Rx[1];
                spyColor = Rx[2];
            }
            else if (Rx[0] == '$'){
                // dollar sign encoding: connected players info, $cc$cc$cc$cc... etc.
                // first char is avatar animal, second is color
                // first element is server
                std::vector<std::string> players;
                players = ofSplitString(Rx,"$");
                printf("$ FIRST CHAR DOLLAR SIGN $\n");
                avatarNum = 0;
                for(int i = 0; i < players.size(); i++){
                    if(players[i].size() == 2){
                        avatarIcons[avatarNum] = players[i][0];
                        avatarColors[avatarNum] = players[i][1];
                        avatarNum++;
                    }
                }
                printf("UPDATED FRIEND COUNT: %d",avatarNum);
                // make sure all the other spots don't have ghosts of former players
                do{
                    avatarIcons[avatarNum] = 0;
                    avatarNum++;
                } while (avatarNum < 256);
            }
            else {
                // improve this.
                // check receivedText against all the gesture commands
                bool wasTurnRelated = false;
                for (int g = 0; g < NUM_GESTURES; g++) {
                    if (strcmp(receivedText, actionString[g].c_str()) == 0) {
                        ((testApp*) ofGetAppPtr())->vibrate(true);
                        useScrambledText = true;  // everybody's appears scrambled to begin
                        animatedScrambleFont = true;
                        mainMessage = Rx;
                        wasTurnRelated = true;
                        updateState(StateTurnScramble);
                    }
                }
                if(!wasTurnRelated){  // nothing. can we count on this being a number? maybe.
                    connectedAgents = ofToInt(Rx);
                }
            }
        }
    }
}

bool agentController::isConnectedToWIFI(){
    if (localIP.compare("error") == 0 || !localIP.length())
        return false;
    return true;
}

bool agentController::serverConnect(){
    if(!isConnectedToWIFI()) return false;
    bool success = server.setup(PORT);
    if (!success)
        server.close();
    ofLogNotice("TCP") << "SERVER STATUS (0/1): " << isServer << " AT:" + localIP + " PORT:" << ofToString(PORT) << "\n";
    return success;
}

bool agentController::clientConnect(string IP){
    if(!isConnectedToWIFI()) return false;
    bool success = client.setup(IP, PORT);
    if (!success)
        client.close();
    ofLogNotice("TCP") << "CLIENT STATUS (0/1):" << isClient << " AT:" + IP + " PORT: " << ofToString(PORT) << "\n";
    return success;
}

string agentController::makeServerIPString(){
    char serverString[16];
    std::vector<std::string> result;
    result = ofSplitString(localIP,".");
    int index = 0;
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < result[i].length(); j++){
            serverString[index] = localIP.c_str()[index];
            index++;
        }
        serverString[index] = '.';
        index++;
    }
    serverString[index] = '\0';
    string serverIP = std::string(serverString);
    serverIP += ofToString(loginCode);
    return serverIP;
}

void agentController::stopServer(){
//	if (server.isConnected()){
        for(int i = 0; i < server.getLastID(); i++){ // getLastID is UID of all clients
            if( server.isClientConnected(i) )
                server.disconnectClient(i);
        }
        // clear server game data
        for(int i = 0; i < 256; i++)
            avatarIcons[i] = 0;
        server.close();
        isServer = false;
        agentView.setIsServer(isServer);
//	}
}

void agentController::sendMessage(string message){
	if (isServer){
	    for(int i = 0; i < server.getLastID(); i++){ // getLastID is UID of all clients
            if( server.isClientConnected(i) )  // check and see if it's still around
                server.send(i,message);
	    }
	}
	else if (isClient){
		client.send(message);
	}
}

#pragma mark GAME

void agentController::generateNewSpyRoles(){
    if (isServer){
        // random spy role from list of IDs, which includes disconnected clients, repeat, until selected a connected client
        do {
            spyAccordingToServer = rand() % (server.getLastID() + 1);
        } while (!server.isClientConnected(spyAccordingToServer - 1) && spyAccordingToServer != 0);
        
        // save the spy's identity, and tell everyone that they are the spy or not the spy
        if(spyAccordingToServer == 0){
            spyAvatar = avatarSelf;
            spyColor = agentView.primaryColor + 1;
            isSpy = true;
            agentView.setIsSpy(isSpy);
        }
        else {
            spyAvatar = avatarIcons[spyAccordingToServer-1];
            spyColor = avatarColors[spyAccordingToServer-1];
            isSpy = false;
            agentView.setIsSpy(isSpy);
        }
        for(int i = 1; i < server.getLastID() + 1; i++){
            if(server.isClientConnected(i-1)){
                if( spyAccordingToServer == i) server.send(i-1, "spy");
                else server.send(i-1, "notspy");
            }
        }
    }
}

void agentController::serverInitiateRound(){
    // decide on a new gesture
    do {
        mainMessage = actionString[rand()%(NUM_GESTURES-1) + 1];
    } while (gestureHasOccurred(mainMessage));
    // store it in the first vacant spot in the previousActions array
    bool placed = false;
    for(int i = 0; i < NUM_TURNS; i++){
        if(!placed && strcmp(previousActions[i].c_str(), "") == 0){
            previousActions[i] = mainMessage;
            placed = true;
        }
    }
    
    useScrambledText = true;   // everyone begins with scrambled text
    animatedScrambleFont = true;
    sendMessage(mainMessage);
    ((testApp*) ofGetAppPtr())->vibrate(true);
}

bool agentController::gestureHasOccurred(string message){
    for(int i = 0; i < NUM_TURNS; i++){
        if(strcmp(previousActions[i].c_str(), message.c_str()) == 0)
            return true;
    }
    return false;
}

void agentController::execute(string gesture){

    animatedScrambleFont = false;
    if(!isSpy){
        useScrambledText = false;
    }
    
    ofLogNotice("RECORD MODE") << "RECORDING: " + gesture;
    
    char mess[128];
    strcpy(mess, gesture.c_str());
    
    if (strcmp(mess, "TOUCH SCREEN") == 0 ||
        strcmp(mess, "TOUCH SCREEN\nWITH NOSE")) {
        recordMode = RecordModeTouch;
    }
    else if (strcmp(mess, "SHAKE ME") == 0 ||
             strcmp(mess, "JUMP") == 0 ||
             strcmp(mess, "FREEZE") == 0 ||
             strcmp(mess, "RUN IN PLACE") == 0 ||
             strcmp(mess, "CROUCH") == 0 ||
             strcmp(mess, "CHICKEN DANCE")) {
        recordMode = RecordModeAccel;
    }
    else if (strcmp(mess, "SPIN") == 0 ||
             strcmp(mess, "COVER YOUR EYES") ||
             strcmp(mess, "RAISE\nA HAND") == 0) {
        recordMode = RecordModeOrientation;
    }
    else if (strcmp(mess, "HIGH FIVE\nNEIGHBOR") == 0 ||
             strcmp(mess, "POINT AT\nAN AGENT") == 0 ||
             strcmp(mess, "STAND ON\nONE LEG") == 0 ||
             strcmp(mess, "MAKE YOUR\nANIMAL'S SOUND") ||
             strcmp(mess, "TOUCH YOUR TOES")) {
        recordMode = RecordModeNothing;
    }
    else {
        recordMode = RecordModeNothing;
    }
    turnTime = ofGetElapsedTimeMillis();
    
    updateState(StateTurnGesture);
}

// server only function
void agentController::pickedAgent(int agent) {
    
    if(agent == spyAccordingToServer){
        if(isSpy)
            mainMessage = "CAPTURED";
        else
            mainMessage = "WIN";
        for(int i = 0; i < server.getLastID(); i++){ // getLastID is UID of all clients
            if( server.isClientConnected(i) ){
                if(i == agent-1)
                    server.send(i, "CAPTURED");
                else
                    server.send(i, "WIN");
            }
        }
    }
    else {
        mainMessage = "LOSE";
        sendMessage(mainMessage);
    }
    updateState(StateGameOver);
}

#pragma mark SCRIPTS

// transition program immediately into another scene.
// cusomize each type of transition

void agentController::updateState(ProgramState newState){
    state = newState;
    stateBeginTime = ofGetElapsedTimeMillis();
    
    if(state == StateWelcomeScreen);
    else if(state == StateConnectionScreen){
        if (server.isConnected()){ stopServer(); }
        if (client.isConnected()){ client.close(); isClient = false; }
        networkState = NetworkNone;
        printf("STATE connectionScreen\n");
    }
    else if(state == StateJoinScreen) printf("STATE joinScreen\n");
    else if(state == StateReadyRoom){
//        if(isClient){
//            for(int i = 0; i < 256; i++)
//                avatarIcons[i] = 0;
//        }
        printf("STATE readyRoom\n");
    }
    else if(state == StateStartGame){                               // initiated by server sendMessage("stateStartGame")
        for(int j = 0; j < NUM_TURNS; j++){
            for(int i = 0; i < SENSOR_DATA_ARRAY_SIZE; i++)
                recordedSensorData[j][i] = 0.0;
        }
        turnNumber = 0;
        printf("STATE startGame\n");
    }
    else if(state == StateCountdown) printf("STATE countDown\n");
    else if(state == StateTurnScramble){ printf("STATE TURN scramble\n");   // server initiated by sendMessage(gesture)
        // increment turn
        turnNumber++;
    }
    else if(state == StateTurnGesture) printf("STATE TURN gesture\n");  // server initiated at the end of execute()
    else if(state == StateTurnComplete){ printf("STATE TURN complete\n");

    }
    else if(state == StateDecide) printf("STATE decide\n");          // initiated by server sendMessage("stateDecide")
    else if(state == StateGameOver) printf("STATE gameOver\n");        // initiated by server sendMessage "WIN" / "LOSE"
}

// call updateState() after a delay
void agentController::updateStateWithTransition(ProgramState newState, long delay){
    if(!transitionActive){
        transitionActive = true;
        transitionTarget = newState;
        transitionEndTime = elapsedMillis + delay;
        transitionDuration = delay;
    }
}

// the update loop. once/drawTime
// inside each block, update stuff first, then only at the end call updateState(newState)

void agentController::update() {
    elapsedMillis = ofGetElapsedTimeMillis();  // reduce calls to ofGetElapsedTimeMillis();
    
    // check if transitions are in process. advance if they have ended
    if(transitionActive){
        if(elapsedMillis > transitionEndTime){  // transition over
            updateState(transitionTarget);
            transitionActive = false;
        }
    }
    
    // script time-based transitions
    if(state == StateWelcomeScreen);
    else if(state == StateConnectionScreen);
    else if(state == StateJoinScreen);
    else if(state == StateReadyRoom);
    else if(state == StateStartGame){       // initiated by server sendMessage("stateStartGame")
        if(elapsedMillis > stateBeginTime + 5000){
            updateState(StateCountdown);
        }
    }
    else if(state == StateCountdown){
        if(elapsedMillis > stateBeginTime + 7000){
            if (isServer){
                // setup new game
                for(int i = 0; i < NUM_TURNS; i++)
                    previousActions[i] = "";
                // begin game
                serverInitiateRound();
                updateState(StateTurnScramble);
            }
        }
    }
    else if(state == StateTurnScramble){
        if(isServer && elapsedMillis > stateBeginTime + 1500){
            // delay by random amount
            // test feeling for duration
            //if(ofRandom(0, 40) == 0){
                sendMessage("execute");
                execute(mainMessage);
            //}
        }
    }
    else if(state == StateTurnGesture){
        int i1 = turnNumber - 1;
        int i2 = SENSOR_DATA_ARRAY_SIZE * (elapsedMillis-stateBeginTime)/(float)ACTION_TIME;
        if(i1 < 0) i1 = 0;
        if(i1 >= NUM_TURNS) i1 = NUM_TURNS-1;
        if(i2 < 0) i2 = 0;
        if(i2 >= SENSOR_DATA_ARRAY_SIZE) i2 = SENSOR_DATA_ARRAY_SIZE-1;
        recordedSensorData[i1][i2] += sumSensors();
        
        if(elapsedMillis > stateBeginTime + ACTION_TIME)
            updateState(StateTurnComplete);
    }
    else if(state == StateTurnComplete){
        if(isServer && elapsedMillis > stateBeginTime + 4000){
            if(turnNumber < NUM_TURNS){
                serverInitiateRound();
                updateState(StateTurnScramble);
            }
            else{
                for(int i = 0; i < server.getLastID(); i++) {  // getLastID is UID of all clients
                    if( server.isClientConnected(i) ) { // check and see if it's still around
                        server.send(i,"stateDecide");
                    }
                }
                mainMessage = "OPERATIVE I.D.";  // everyone
                updateState(StateDecide);
            }
        }
    }
    else if(state == StateDecide);          // initiated by server sendMessage("stateDecide")
    else if(state == StateGameOver){        // initiated by server sendMessage "WIN" / "LOSE"
        if(elapsedMillis > stateBeginTime + 5000){
            updateState(StateReadyRoom);
        }
    }
    ////////////////////////////////////////////////////////////
    // once / second updates
    if(oneSecond != ofGetSeconds()){   // only runs once/second
        oneSecond = ofGetSeconds();
        
        if(isClient || isServer){
            updateSlowTCP();
        }
        if(state == StateWelcomeScreen){
            agentView.setWIFIExist(isConnectedToWIFI());
        }
    }
    ////////////////////////////////////////////////////////////
        
    if (isClient || isServer) {
        updateTCP();
    }
    
    if(errorMessageActive){
        if(elapsedMillis > errorBeginTime + 2000){
            errorMessageActive = false;
        }
    }

}

#pragma mark SENSORS

//--------------------------------------------------------------
void agentController::touchBegan(int x, int y, int id){
    
    // this is all copy pasted from touchEnd(), where the code is more functional
    // this code is just for sound
    
    if(state == StateWelcomeScreen){
        clickDown.play();
    }
    else if(state == StateConnectionScreen){
        if(y > height * .5 && y < height * .7)
            clickDown.play();
    }
    else if(state == StateJoinScreen){
        // top row, increment
        if(y > height * .1 && y < height * .3)
            if(( x > width * .15 && x < width * .35) || (x > width * .4 && x < width * .6) || (x > width * .65 && x < width * .85) )
                clickDown.play();
        // bottom row, decrement
        if(y > height * .35 && y < height * .55)
            if( (x > width * .15 && x < width * .35) || (x > width * .4 && x < width * .6) || (x > width * .65 && x < width * .85) )
                clickDown.play();
        // back button
        if(x < centerX && y > height * .9)
            clickDown.play();
        // join button
        if(y > height * .6 && y < height * .72)
            clickDown.play();
    }
    else if(state == StateReadyRoom);
    else if(state == StateStartGame);       // initiated by server sendMessage("stateStartGame")
    else if(state == StateCountdown);
    else if(state == StateTurnScramble);
    else if(state == StateTurnGesture);
    else if(state == StateTurnComplete);
    else if(state == StateDecide);          // initiated by server sendMessage("stateDecide")
    else if(state == StateGameOver);        // initiated by server sendMessage "WIN" / "LOSE"

    
    // network states
//        NetworkNone,
//        NetworkHostAttempt,
//        NetworkHostSuccess,
//        NetworkJoinAttempt,
//        NetworkJoinSuccess,
//        NetworkLostConnection,      // try to make these 2 into 1
//        NetworkServerDisconnected   //

    switch (state) {
            
        case StateTurnGesture:
            if (recordMode == RecordModeTouch) {
//                turnState = TurnStateActionSuccess;                                                                         // turnState  :  action success
                updateState(StateTurnComplete);  // should this be happening here? is that what "complete" means?
                recordMode = RecordModeNothing;        // turn off recording
                recordedTimes[0] = ofGetElapsedTimeMillis() - turnTime;
                ((testApp*) ofGetAppPtr())->vibrate(true);
                if (isClient) {
                    sendMessage(ofToString(recordedTimes[0]));
                }
            }
            break;
            
        case StateDecide:
            if (isServer) {
                pickedAgent(0);
            }
            else if (isClient){
                sendMessage("pickedAgent");
            }
            break;
            
        default:
            break;
    }
}

void agentController::touchMoved(int x, int y, int id) { }

void agentController::touchEnded(int x, int y, int id) {
//    clickUp.play();
    if(state == StateWelcomeScreen){
        if(isConnectedToWIFI()){
            avatarSelf = ofRandom(1, 9);
            printf("AVATAR CHOSE: %d\n",avatarSelf);
            updateState(StateConnectionScreen);
        }
        else{
            errorMessageActive = true;
            errorMessage = "game requires WIFI";
            errorBeginTime = elapsedMillis;
        }
    }
    else if(state == StateConnectionScreen){
        if(y > height * .5 && y < height * .7){
            if(x < centerX){
                isServer = serverConnect();
                agentView.setIsServer(isServer);
                if(isServer){
                    ofLogNotice("choose server") << "yes!";
                    networkState = NetworkHostSuccess;
                    for(int i = 0; i < 256; i++)
                        avatarIcons[i] = 0;
//                    short   avatarNum;  // osmething wtiht this?

                    updateStateWithTransition(StateReadyRoom, 500);
                }
                else {
                    networkState = NetworkNone;
//                loginState = LoginStateFailed;
                }
            }
        }
        if(x > centerX){
            updateStateWithTransition(StateJoinScreen, 500);
        }
    }
    else if(state == StateJoinScreen)
    {
        int tens = loginCode - ((loginCode / 100) * 100);
        int ones = tens - ((tens / 10) * 10);
        // top row, increment
        if(y > height * .1 && y < height * .3){
            if(x > width * .15 && x < width * .35)
                loginCode >= 200 ? loginCode -= 200 : loginCode += 100;
            if(x > width * .4 && x < width * .6)
                tens >= 90 ? loginCode -= 90 : loginCode += 10;
            if(x > width * .65 && x < width * .85)
                ones >= 9 ? loginCode -= 9 : loginCode += 1;
        }
        // bottom row, decrement
        if(y > height * .35 && y < height * .55){
            if(x > width * .15 && x < width * .35)
                loginCode < 100 ? loginCode += 200 : loginCode -= 100;
            if(x > width * .4 && x < width * .6)
                tens < 10 ? loginCode += 90 : loginCode -= 10;
            if(x > width * .65 && x < width * .85)
                ones < 1 ? loginCode += 9 : loginCode -= 1;
        }
        // back button
        if(x < centerX && y > height * .9){
            updateState(StateConnectionScreen);
        }
        // join button
        if(y > height * .6 && y < height * .72){
            serverIP = makeServerIPString();
            isClient = clientConnect(serverIP);
            if(isClient){
                char identity[3];
                identity[0] = '$';
                identity[1] = avatarSelf;
                identity[2] = agentView.primaryColor+1;
                sendMessage(identity);
                mainMessage = "";  // "Agent"
                updateState(StateReadyRoom);                                                                          // gameState  :  connected
            }
            else {
                // deliver error message
            }
        }
    }
    else if(state == StateReadyRoom){
        if(x < centerX && y > height * .85){
            updateState(StateConnectionScreen);
        }
        // start game
        if (isServer && x > centerX && y > height * .85){
#warning change connectedAgents > 2
            if(connectedAgents > 1){
                generateNewSpyRoles();
                sendMessage("stateStartGame");
                updateStateWithTransition(StateStartGame, TRANSITION_BEGIN_GAME);
            }
            else{
                // deliver message: "game requires at least 3 players"
                errorMessage = "requires at least 3 players";
                errorMessageActive = true;
                errorBeginTime = elapsedMillis;
            }
        }
    }
    else if(state == StateStartGame);       // initiated by server sendMessage("stateStartGame")
    else if(state == StateCountdown);
    else if(state == StateTurnScramble);
    else if(state == StateTurnGesture);
    else if(state == StateTurnComplete);
    else if(state == StateDecide);          // initiated by server sendMessage("stateDecide")
    else if(state == StateGameOver);        // initiated by server sendMessage "WIN" / "LOSE"

}

float agentController::getMaxSensorScale(){
    float max = 0.;
    if(fabs(deltaOrientation.b) > max) max = fabs(deltaOrientation.b);
    if(fabs(deltaOrientation.c) > max) max = fabs(deltaOrientation.c);
    if(fabs(deltaOrientation.d) > max) max = fabs(deltaOrientation.d);
    if(fabs(deltaOrientation.f) > max) max = fabs(deltaOrientation.f);
    if(fabs(deltaOrientation.g) > max) max = fabs(deltaOrientation.g);
    if(fabs(deltaOrientation.h) > max) max = fabs(deltaOrientation.h);
    return max;
}

float agentController::sumSensors(){
    return fabs(deltaOrientation.b) + fabs(deltaOrientation.c) + fabs(deltaOrientation.d) + fabs(deltaOrientation.f) + fabs(deltaOrientation.g) + fabs(deltaOrientation.h);
}

void agentController::logMatrix3x3(ofMatrix3x3 matrix){
    static int timeIndex;
    timeIndex++;
    if(timeIndex % 15 == 0){
        char b, c, d, f, g, h;
        b = c = d = f = g = h = ' ';
        if(matrix.b > 0) b = '+'; else if (matrix.b < 0) b = '-';
        if(matrix.c > 0) c = '+'; else if (matrix.c < 0) c = '-';
        if(matrix.d > 0) d = '+'; else if (matrix.d < 0) d = '-';
        if(matrix.f > 0) f = '+'; else if (matrix.f < 0) f = '-';
        if(matrix.g > 0) g = '+'; else if (matrix.g < 0) g = '-';
        if(matrix.h > 0) h = '+'; else if (matrix.h < 0) h = '-';
        ofLogNotice("") <<
        "\n[ " <<'1'<< " " << b << " " << c << " ]   [ " << matrix.a << " " << matrix.b << " " << matrix.c << " ]" <<
        "\n[ " << d << " " <<'1'<< " " << f << " ]   [ " << matrix.d << " " << matrix.e << " " << matrix.f << " ]" <<
        "\n[ " << g << " " << h << " " <<'1'<< " ]   [ " << matrix.g << " " << matrix.h << " " << matrix.i << " ]";
    }
}

void agentController::logMatrix4x4(ofMatrix4x4 matrix){
    static int timeIndex;
    timeIndex++;
    if(timeIndex % 15 == 0)
        ofLogNotice("") <<
        "\n[ " << matrix._mat[0].x << " " << matrix._mat[1].x << " " << matrix._mat[2].x << " ]" <<
        "\n[ " << matrix._mat[0].y << " " << matrix._mat[1].y << " " << matrix._mat[2].y << " ]" <<
        "\n[ " << matrix._mat[0].z << " " << matrix._mat[1].z << " " << matrix._mat[2].z << " ]";
}

void agentController::updateOrientation(ofMatrix3x3 newOrientationMatrix, ofMatrix3x3 newDeltaOrientationMatrix){
    orientation = newOrientationMatrix;
    deltaOrientation = newDeltaOrientationMatrix;
}

#pragma mark SYSTEM

void agentController::setIpAddress(const char* ipAddress){
    
    if (ipAddress != NULL){
        if (ipAddress[0] != 0){
            localIP = std::string(ipAddress);
            ofLogNotice("OSC") << "OF LOCAL IP ADDRESS:" + localIP + " on port:" << ofToString(PORT);
            // open an outgoing connection to HOST:PORT
            // DO CLIENT
        }
    }
	else {
		ofLogNotice("OSC") << "Didn't receive IP address from ANDROID ENV";
	}
}

string agentController::getCodeFromInt(int num){
    
    string codeString = paddedString(num);
    char code[6];
    // spaces inbetween 3 number characters
    code[1] = ' ';
    code[3] = ' ';
    const char* last = codeString.c_str();
    code[0] = last[0];
    code[2] = last[1];
    code[4] = last[2];
    code[5] = '\0';
    return string(code);
}

string agentController::getCodeFromIp(){
    if (localIP.compare("error") == 0 || !localIP.length()) return "error";
    
    std::vector<std::string> result;
    result = ofSplitString(localIP,".");
    char code[6];
    // spaces inbetween 3 number characters
    code[1] = ' ';
    code[3] = ' ';
    if (result[3].length() < 2){
        const char* last = result[3].c_str();
        code[0] = '0';
        code[2] = '0';
        code[4] = last[0];
    }
    if (result[3].length() < 3){
        const char* last = result[3].c_str();
        code[0] = '0';
        code[2] = last[0];
        code[4] = last[1];
    }
    else {
        const char* last = result[3].c_str();
        code[0] = last[0];
        code[2] = last[1];
        code[4] = last[2];
    }
    code[5] = '\0';
    return string(code);
}
