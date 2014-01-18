//
//  agentController.cpp
//  DoubleAgent
//
//  Created by Chroma Developer on 12/30/13.
//
//

#include "agentController.h"

#import "testApp.h"
#import "ofxTimer.h"

void agentController::setup() {

	font.loadFont("digital-7.ttf", ofGetWidth() / 9., true, true);   // 4.2
    spyfont.loadFont("webdings.ttf", ofGetWidth() / 5., true, true);
    fontSmall.loadFont("verdana.ttf", 40, true, true);
    spymess[0] = rand()%127;
    spymess[1] = rand()%127;
    spymess[2] = rand()%127;
    spymess[3] = rand()%127;
//	font.setLetterSpacing(1.037);

//    elapsed = ofGetElapsedTimeMillis();
    state = GameStateWaitingForSignIn;

    sphere.setRadius( ofGetWidth()  );

    //ofSetSmoothLighting(true);

    ofSetSphereResolution(24);
}

#pragma mark NETWORK

void agentController::updateTCP() {

	if (isServer){

        connectedAgents = 0;

	    for(int i = 0; i < server.getLastID(); i++) { // getLastID is UID of all clients

            if( server.isClientConnected(i) ) { // check and see if it's still around
                //connectedAgents++;
                connectedAgents = server.getNumClients() + 1;

                // maybe the client is sending something
                string str = server.receive(i);
                //server.send(i, "You sent: "+str);

                if (str.length()){

                	strcpy( mouseButtonState, str.c_str() );
                    ofLogNotice("TCP") << "Agent:" + ofToString(i) + " : " + str;

                    if (strcmp(mouseButtonState, "pickedAgent") == 0) {
                        pickedAgent(i+1);
                    }
//                    else  if (strcmp(mouseButtonState, "login") == 0) {
//                        activeAgents++;  // TODO needs a point where activeAgents--
//                    }
                    else {// must be a number
                        recordedTimes[i+1] = ofToInt(str);
                    }
                }
            }
	    }
	}
    else if (isClient){

        string str = client.receive();

        if (str.length()){

	    	ofLogNotice("TCP") << "Received From Server: " + str;

            strcpy( mouseButtonState, str.c_str() );

            if (strcmp(mouseButtonState, "startGame") == 0) {
                startGame();
//                sendMessage("login");
            }
            else if (strcmp(mouseButtonState, "execute") == 0) {
//                 ((testApp*) ofGetAppPtr())->vibrate(true);
                 execute(mainMessage);
            }
            else if (strcmp(mouseButtonState, "PICKER") == 0) {
                ((testApp*) ofGetAppPtr())->vibrate(true);
                //execute(mainMessage);
                mainMessage = "PICK";
                state = GameStateDeciding;
            }
            else if (strcmp(mouseButtonState, "PICK") == 0) {
                ((testApp*) ofGetAppPtr())->vibrate(true);
                //execute(mainMessage);
                mainMessage = "AGENT";
                state = GameStateDeciding;
            }
            else if (strcmp(mouseButtonState, "spy") == 0) {
                isSpy = true;
            }
            else if (strcmp(mouseButtonState, "notspy") == 0) {
                isSpy = false;
            }
            else if (strcmp(mouseButtonState, "SPY CAPTURED!") == 0) {
                mainMessage = "SPY CAPTURED!";
            }
            else if (strcmp(mouseButtonState, "NOPE!") == 0) {
                mainMessage = "NOPE!";
            }
            else {
                bool wasActionMove = false;
                for (int g = 0; g < NUM_GESTURES; g++) {
                    if (strcmp(mouseButtonState, actionString[g].c_str()) == 0) {
                        ((testApp*) ofGetAppPtr())->vibrate(true);
                        //if (isSpy)
                            useSpyFont = true;  // everybody's appears scrambled to begin
                        mainMessage = str;
                        wasActionMove = true;
                    }
                }
                for (int g = 0; g < NUM_PLACES; g++) {
                    if (strcmp(mouseButtonState, placeString[g].c_str()) == 0) {
                        useSpyFont = false;
                        mainMessage = str;
                        wasActionMove = true;
                    }
                }
                if(!wasActionMove){  // must be a number
                    connectedAgents = ofToInt(str);
                }
            }
        }
    }
}

void agentController::updateSlowTCP(){
    if(oneSecond != ofGetSeconds()){   // only runs once/second
        oneSecond = ofGetSeconds();

        if(isServer){  //  send number of clients
            sendMessage(ofToString(1+server.getNumClients()));
        }
        else if (isClient){


        }

    }
}

void agentController::sendMessage(string message){

	if (isServer){
	    for(int i = 0; i < server.getLastID(); i++) // getLastID is UID of all clients
	    {
            if( server.isClientConnected(i) ) { // check and see if it's still around
                server.send(i,message);
            }
	    }
	}
	else if (isClient){
		client.send(message);
	}
}

void agentController::countDown(int curstep) {

    if (!curstep) {  // possible for server and clients
        step = 0;
        stepInterval = 1000;
        numSteps = 5;
        stepTimer = ofGetElapsedTimeMillis();

        stepFunction = &agentController::countDown;

        return;
    }

    switch (curstep) {

        case 1:
            mainMessage = "5";
            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 2:
            mainMessage = "4";
//            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 3:
            mainMessage = "3";
//            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 4:
            mainMessage = "2";
//            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 5:
            mainMessage = "1";
//            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 6:
            //mainMessage = "";

            state = GameStatePlaying;
            stepInterval = 0;

            if (isServer) {
                serveRound(0);
            }

            break;

        default:
            break;
    }
}

// server only function
void agentController::serveRound(int curstep){

    // initiate round
    if (!curstep) {
        for(int i = 0; i < NUM_TURNS; i++)
            previousActions[i] = "";
        step = 0;
        stepInterval = 5000;
        numSteps = NUM_TURNS*3;
        stepTimer = ofGetElapsedTimeMillis();
        stepFunction = &agentController::serveRound;
    }

    if (curstep == numSteps) {

        if (pickerAccordingToServer == 0) {
            mainMessage = "PICK";
        }
        else {
            mainMessage = "AGENT";
        }

        for(int i = 0; i < server.getLastID(); i++) {  // getLastID is UID of all clients

            if( server.isClientConnected(i) ) { // check and see if it's still around
                if (i+1 == pickerAccordingToServer) {
                    server.send(i,"PICKER");
                }
                else {
                    server.send(i,"PICK");
                }
            }
	    }
        stepInterval = 0;
        state = GameStateDeciding;
    }

    else if (curstep %3 == 0) { // MESSAGE

        for (int i = 0 ; i < 16; i++) {
            recordedTimes[i] = 5000 + i;
        }

        do {
            mainMessage = actionString[rand()%(NUM_GESTURES-1) + 1];
        } while (actionHasOccurred(mainMessage));

        bool placed = false;
        for(int i = 0; i < NUM_TURNS; i++){
            if(!placed && strcmp(previousActions[i].c_str(), "") == 0){
                previousActions[i] = mainMessage;
                placed = true;
            }
        }

        //if (isSpy)
            useSpyFont = true;   // everyone begins with scrambled text
        sendMessage(mainMessage);
        ((testApp*) ofGetAppPtr())->vibrate(true);

        stepInterval = 1000 + rand() % 3000;
    }
    else if (curstep%3 == 1) { // EXCECUTE

        sendMessage("execute");

//        ((testApp*) ofGetAppPtr())->vibrate(true);

        stepInterval = 3000;

        execute(mainMessage);
    }
    else if (curstep%3 == 2) { // RESULTS

        countScores();

        useSpyFont = false;

        // sendMessage(mainMessage);

        stepInterval = 6000;
    }
}

bool agentController::actionHasOccurred(string message){
    for(int i = 0; i < NUM_TURNS; i++){
        if(strcmp(previousActions[i].c_str(), message.c_str()) == 0)
            return true;
    }
    return false;
}

// server only function
void agentController::countScores(){

    int places[16];

    for (int p = 0; p < 16; p++) {
        places[p] = -1;
    }

    for(int i = 0; i < server.getLastID() + 1; i++) {   // getLastID is UID of all clients

        int lowestTime = 100000;

        for(int j = 0; j < server.getLastID() + 1; j++) // getLastID is UID of all clients
        {
            if (recordedTimes[j] <= lowestTime) {

                bool shouldRecord = true;

                for (int p = 0; p < 16; p++) {
                    if (places[p] == j) {
                        shouldRecord = false;
                    }
                }

                if (shouldRecord) {
                    lowestTime = recordedTimes[j];
                    places[i] = j;
                }
            }
        }
        ofLogNotice("PLACES") << ofToString(places[i]) + " is in " + ofToString(i) + " place with " + ofToString(recordedTimes[places[i]]) + "time";
    }

    for(int i = 0; i < server.getLastID() + 1; i++) { // getLastID is UID of all clients
        if (places[i] != 0) {

            if( server.isClientConnected(places[i] - 1) ) { // check and see if it's still around
                server.send(places[i]-1, placeString[i]);
            }
        }
        else {
            mainMessage = placeString[i];
        }
    }
}

void agentController::execute(string gesture){

    if(!isSpy){
        useSpyFont = false;
    }

    ofLogNotice("RECORD MODE") << "RECORDING: " + gesture;

    char mess[128];

    strcpy(mess, gesture.c_str());

    if (strcmp(mess, "TOUCH") == 0) {
        recordMode = GameActionTouch;
    }

    else if (strcmp(mess, "SHAKE") == 0) {
        recordMode = GameActionShake;
    }

    else if (strcmp(mess, "JUMP") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "SPIN") == 0) {
        recordMode = GameActionSpin;
    }

    else if (strcmp(mess, "LANDSCAPE MODE") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "SET PHONE DOWN") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "PUNCH") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "CROUCH") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "STAND ON 1 LEG") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "TOUCH YOUR NOSE") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "DRAW A CIRCLE") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "RUN IN PLACE") == 0) {
        recordMode = GameActionJump;
    }

    else {
        recordMode = GameActionFreeze;
    }

    turnTime = ofGetElapsedTimeMillis();

}

//void agentController::detectShakeGesture(){
//
//}

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
//    logMatrix3x3(deltaOrientation);
}


void agentController::updateAccel(ofVec3f newAccel){

    if (newAccel.x != accel.x ) {

        accel = newAccel;
        normAccel = accel.getNormalized();

        accelIndex++;
        if (accelIndex > 127) {
            accelIndex = 0;
        }

        float alpha = 0.9f;

        filteredAccel.x = alpha * filteredAccel.x + (1 - alpha) * normAccel.x;
        filteredAccel.y = alpha * filteredAccel.y + (1 - alpha) * normAccel.y;
        filteredAccel.z = alpha * filteredAccel.z + (1 - alpha) * normAccel.z;

        userAccelerationArray[accelIndex] = filteredAccel;

        bool didIt = false;

        if (recordMode == GameActionJump || recordMode == GameActionShake) {
            if (accel.z > .5) {
                didIt = true;
            }
        }

        else if (recordMode == GameActionSpin) {
            if (accel.y > .5) {
                didIt = true;
            }
        }

        if (didIt) {

            recordMode = 0;
            recordedTimes[0] = ofGetElapsedTimeMillis() - turnTime;
            ((testApp*) ofGetAppPtr())->vibrate(true);

            if (isClient) {
                sendMessage(ofToString(recordedTimes[0]));
            }
        }
    }
}


#pragma mark - DRAW

void agentController::draw() {

    ofClear(0, 0, 0);

    drawAnimatedBackground();

    if (isServer || isClient) {

        float x = ofGetWidth();
        float y = ofGetHeight();
        float origx = x/2.;
        float origy = y/2.;

        ofEnableAlphaBlending();

        for (int r = 0; r < 8; r++){

            int nStarPts = r+3;
            float angleChangePerPt = TWO_PI / (float)nStarPts;
            float angle = (sin(ofGetElapsedTimef() / 400.) * 360) + r*30;
            float outerRadius = 120 + (200*(fabs(filteredAccel.z))) - (r*5. + (fabs(filteredAccel.y)*30.));

            ofSetColor(50+(20*r), (20*filteredAccel.x + filteredAccel.y), 50+(20*r), 200);

            ofBeginShape();
            for (int i = 0; i < nStarPts; i++){
                float x = origx + outerRadius * cos(angle);
                float y = origy + outerRadius * sin(angle);
                ofVertex(x,y);
                angle += angleChangePerPt;
            }
            ofEndShape();
        }

        ofSetLineWidth(1.);

        for (int r = 0; r < 12; r++){

            float angle = (sin(ofGetElapsedTimef() / 400.) * 360) + r*30;
            ofSetColor(50+(20*r), (20*filteredAccel.x + filteredAccel.y), 100+(20*r), 200);
            float x = origx + (filteredAccel.y*ofGetWidth()  + r*30) * cos(-angle);
            float y = origy + (filteredAccel.y*ofGetWidth()  + r*30) * sin(-angle);
            ofLine(origx,origy,x,y);

            angle += 15;
        }

//        ofSetColor(50, 50, 50,200);
//        ofBeginShape();
//
//        ofVertex(x*.1,y*.33);
//        ofVertex(x*.9,y*.33);
//        ofVertex(x*.9,y*.66);
//        ofVertex(x*.1,y*.66);
//
//        ofEndShape();

        ofDisableAlphaBlending();

        // BACK TO BUSINESS

        if (mainMessage.length()){
            ofSetColor(255,255,255);

            if (useSpyFont) {
                //if(rand()%2 == 0){
                    int index = rand()%4;
                    spymess[index] = rand()%127;
                //}
                font.drawString(ofToString(spymess),ofGetWidth()/2 - font.stringWidth(mainMessage)/2.,ofGetHeight()/2 + font.stringHeight(ofToString(spymess))/2.);
            }

            else
                font.drawString(mainMessage,ofGetWidth()/2 - font.stringWidth(mainMessage)/2.,ofGetHeight()/2 + font.stringHeight(mainMessage)/2.);
        }
    }
    else {  // if not server or client
        mainMessage = "NO LINK";
        ofSetColor(255,255,255);
        font.drawString(mainMessage,ofGetWidth()/2 - font.stringWidth(mainMessage)/2.,ofGetHeight()/2 + font.stringHeight(mainMessage)/2.);
    }
//    if (isServer) { // SERVER ONLY DRAW

        if (connectedAgents){   // CONNECTED AGENTS
            string count = ofToString(connectedAgents);
            ofSetColor(200,200,200);
            fontSmall.drawString(count,ofGetWidth()*.9 - fontSmall.stringWidth(count)/2.,ofGetHeight()*.9 + fontSmall.stringHeight(count)/2.);
        }
//    }
}

//bool agentController::processAcceleration() {
//
//    bool foundHighZ;
//    bool foundLowZ;
//
//    for (int i = 0; i < 128; i++) {
//
//        if (userAccelerationArray[i].z > 1) {
//            foundHighZ = true;
//        }
//        if (userAccelerationArray[i].z < -1) {
//            foundLowZ = true;
//        }
//    }
//
//    if (foundLowZ && foundHighZ) {
//        return 1;
//    }
//    else return 0;
//}

void agentController::drawAnimatedBackground() {

    float spinX = sin(ofGetElapsedTimef()*.35f);
    float spinY = cos(ofGetElapsedTimef()*.075f);
    sphere.setPosition(ofGetWidth()*.5, ofGetHeight()*.5, 0);
    sphere.rotate(spinX, 1.0, 0.0, 0.0);
    sphere.rotate(spinY, 0, 1.0, 0.0);

//    ofEnableDepthTest();   //  presently unnecessary, processor intensive
    ofEnableLighting();
    ofNoFill();
    ofSetColor(255);
    sphere.drawWireframe();
    ofFill();
    ofDisableLighting();
//    ofDisableDepthTest();
}

#pragma mark - LOGIC

void agentController::update() {

    if (isClient || isServer) {

      	updateTCP();
        updateSlowTCP();

        if (stepInterval){

            if (ofGetElapsedTimeMillis() - stepTimer > stepInterval ){

                step++;

                if (step > 0){
                    if (stepFunction != NULL) {
                        (this->*stepFunction)(step);
                    }
                }
                if (step > numSteps){
                    stepInterval = 0;
                }
                stepTimer = ofGetElapsedTimeMillis();
            }
        }
    }
}

void agentController::pickedAgent(int agent) {

    if(agent == spyAccordingToServer){
        //sendMessage("agentDiscovered");
        mainMessage = "SPY CAPTURED!";
        sendMessage(mainMessage);
    }
    else {
        //sendMessage("agentNotDiscovered");
        mainMessage = "NOPE!";
        sendMessage(mainMessage);
    }

    state = GameStateWaitingForSignIn;
}

void agentController::startGame(){

    if (isServer){

        state = GameStatePlaying;
        spyAccordingToServer = rand() % (server.getLastID() + 1);
        pickerAccordingToServer = rand() % (server.getLastID() + 1);

        if (spyAccordingToServer != 0){
            while (!server.isClientConnected(spyAccordingToServer-1)){
                spyAccordingToServer = rand() % (server.getLastID() + 1);
            }
        }
        while (pickerAccordingToServer == spyAccordingToServer){
            pickerAccordingToServer = rand() % (server.getLastID() + 1);
        }
        ofLogNotice("AGENT") << "PICKER IS: " + ofToString(pickerAccordingToServer);

        for(int i = 0; i < server.getLastID() + 1; i++) { // getLastID is UID of all clients

            if (i == spyAccordingToServer) {
                if (i == 0) {
                    isSpy = true;
                }
                else {
                    server.send(i-1, "spy");
                }
            }
            else {
                if (i == 0) {
                    isSpy = false;
                }
                else {
                    server.send(i-1, "notspy");
                }
            }
        }
    }
    countDown(0);
}


#pragma mark UX

//--------------------------------------------------------------
void agentController::touchBegan(int x, int y, int id){

    ofLogNotice("TOUCH: ") << strcmp("PICK", "PICKER") << "  " << strcmp("PICKER", "PICK") << "  " << strcmp("PICKER 12", "PICKER");
    ofLogNotice("TOUCH: ") << strcmp("PICKER", "P") << "  " << strcmp("PICKER", "PI") << "  " << strcmp("PICKER", "PIC");
    ofLogNotice("TOUCH: ") << strcmp("PICKER", "PICK") << "  " << strcmp("PICKER", "PICKE") << "  " << strcmp("PICKER", "PICKER");

    switch (state) {
        case GameStateWaitingForSignIn:
            if (isServer) {
                sendMessage("startGame");
                //sleep(250);
                startGame();
            }
            break;
        case GameStatePlaying:
            if (recordMode == GameActionTouch) {

                recordMode = 0;
                recordedTimes[0] = ofGetElapsedTimeMillis() - turnTime;
                ((testApp*) ofGetAppPtr())->vibrate(true);
                if (isClient) {
                    sendMessage(ofToString(recordedTimes[0]));
                }
            }
            break;
        case GameStateDeciding:
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

	//sendMessage("touch down");
}

//--------------------------------------------------------------
void agentController::touchMoved(int x, int y, int id){

//    ofxOscMessage m;
//    m.setAddress( "/mouse/position" );
//    m.addIntArg( x );
//    m.addIntArg( y );
//    sender.sendMessage( m );

}

//--------------------------------------------------------------
void agentController::touchEnded(int x, int y, int id){

//	sendMessage("touch up");

}

#pragma mark - SYSTEM


void agentController::setIpAddress(const char* ipAddress){

    if (ipAddress != NULL){

        if (ipAddress[0] != 0){

            localIP = std::string(ipAddress);

            ofLogNotice("OSC") << "OF LOCAL IP ADDRESS:" + localIP + " on port:" << ofToString(PORT);

            char broadcast[16];
            std::vector<std::string> result;

            result = ofSplitString(std::string(ipAddress),".");

            const char* last = result[3].c_str();

            if (last[0] == '1'){

                if (server.setup(PORT)) {
                    isServer = true;
                    ofLogNotice("TCP") << "IS SERVER AT:" + localIP + " on port:" << ofToString(PORT);
                    mainMessage = "AGENT";
                }
                else {
                    server.close();
                    ofLogNotice("TCP") << "SERVER FAILED !!";
                    mainMessage = "RESTART";
                }
            }

            // open an outgoing connection to HOST:PORT

            // DO CLIENT

            if (!isServer){

                int index = 0;
                for (int i = 0; i < 3; i++){
                    for (int j = 0; j < result[i].length(); j++){
                        broadcast[index] = ipAddress[index];
                        index++;
                    }

                    broadcast[index] = '.';
                    index++;
                }

                char s[4] = {'1','\0'};

                for (int i = 0; i < 2; i++){
                    broadcast[index] = s[i];
                    index++;
                }

                serverIP = std::string(broadcast);

                if (client.setup (serverIP, PORT)){
                    isClient = true;
                    ofLogNotice("TCP") << "connect to server at " + serverIP + " port: " << ofToString(PORT) << "\n";
                    mainMessage = "AGENT";
                }
            }
        }
    }
	else {
		ofLogNotice("OSC") << "Didn't receive IP address from ANDROID ENV";
	}
}



void agentController::pause(){

}

void agentController::resume(){

	if (isServer){

		if (server.setup(PORT)){
			ofLogNotice("TCP") << "Successfully resumed Server";
		}
	}
	if (isClient){

		if (!client.isConnected()){
            if (client.setup(serverIP, PORT)){
                ofLogNotice("TCP") << "Reconnected Client";
            }
		}
	}
}

void agentController::exit() {
	if (isServer){
        ofLogNotice("TCP") << "Shutting down Server";
        server.close();

	}
	if (isClient){
		ofLogNotice("TCP") << "Shutting down Client";
		client.close();
	}
}
