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

    font.loadFont("Avenir.ttf", ofGetWidth() / 12., true, true);   // 4.2
    fontSmall.loadFont("AvenirNextCondensed.ttf", 24, true, true);
    fontMedium.loadFont("AvenirNextCondensed.ttf", 36, true, true);
    spymess[0] = rand()%23+65;
    spymess[1] = rand()%23+65;
    spymess[2] = rand()%23+65;
    spymess[3] = rand()%23+65;
    spymess[4] = NULL;
    font.setLetterSpacing(.9);
    fontSmall.setLetterSpacing(.9);
    fontMedium.setLetterSpacing(.9);

    //    elapsed = ofGetElapsedTimeMillis();
    gameState = GameStateLogin;
    loginState = LoginStateChoose;
    turnState = TurnStateNotActive;                                                                                         // turnState  :  not active
    currentTurn = 0;
    ofLogNotice("+++ GameState updated:") << "Waiting For Sign In, setup()";

    sphere.setRadius( ofGetWidth()  );
    ofSetSphereResolution(24);

    reticleOutline.loadImage("reticle_outline.png");
    reticleOutline.setAnchorPercent(.5, .5);
    reticleCompass.loadImage("reticle_compass.png");
    reticleCompass.setAnchorPercent(.5, .5);
    fingerPrint.loadImage("fingerprint.png");
    fingerPrint.setAnchorPercent(.5, .5);
    insideCircle.loadImage("inside_circle_w_fade.png");
    insideCircle.setAnchorPercent(.5, .5);
    reticleInside.loadImage("reticle_inside.png");
    reticleInside.setAnchorPercent(0, 0);
    reticleInside.crop(reticleInside.width/2., reticleInside.height/2., reticleInside.width/2., reticleInside.height/2.);

    increment.loadImage("increment.png");
    increment.setAnchorPercent(.5, .5);
    decrement.loadImage("decrement.png");
    decrement.setAnchorPercent(.5, .5);

    width = ofGetWidth();
    height = ofGetHeight();
    centerX = ofGetWidth()/2.;
    centerY = ofGetHeight()/2.;

    //ofSetSmoothLighting(true);
}

#pragma mark NETWORK

void agentController::updateTCP() {

	if (isServer){

        connectedAgents = 1;

	    for(int i = 0; i < server.getLastID(); i++) { // getLastID is UID of all clients

            if( server.isClientConnected(i) ) { // check and see if it's still around
                connectedAgents++;

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

    	if (!client.isConnected()){
    		client.close();
    		isClient = false;
    		connectedAgents = 0;
    		gameState = GameStateLogin;
    		loginState = LoginStateServerQuit;
    		return;
    	}

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
                mainMessage = "OPERATIVE I.D.";  // "PICK"
                gameState = GameStateDeciding;                                                                                  // gameState  :  deciding
                turnState = TurnStateNotActive;                                                                                  // turnState  :  not active
            }
            else if (strcmp(mouseButtonState, "PICK") == 0) {
                ((testApp*) ofGetAppPtr())->vibrate(true);
                //execute(mainMessage);
                mainMessage = "OPERATIVE I.D.";  // "AGENT"
                gameState = GameStateDeciding;                                                                                  // gameState  :  deciding
                turnState = TurnStateNotActive;                                                                                  // turnState  :  not active
            }
            else if (strcmp(mouseButtonState, "spy") == 0) {
                isSpy = true;
            }
            else if (strcmp(mouseButtonState, "notspy") == 0) {
                isSpy = false;
            }
            else if (strcmp(mouseButtonState, "SPY CAPTURED!") == 0) {
                mainMessage = "SPY CAPTURED!";
                gameState = GameStateGameOver;
                stepTimer = ofGetElapsedTimeMillis();
                stepInterval = 5000;
            }
            else if (strcmp(mouseButtonState, "NOPE!") == 0) {
                mainMessage = "NOPE!";
                gameState = GameStateGameOver;
                stepTimer = ofGetElapsedTimeMillis();
                stepInterval = 5000;
            }
            else {
                bool wasTurnRelated = false;
                for (int g = 0; g < NUM_GESTURES; g++) {
                    if (strcmp(mouseButtonState, actionString[g].c_str()) == 0) {
                        ((testApp*) ofGetAppPtr())->vibrate(true);
                        //if (isSpy)
                        useScrambledText = true;  // everybody's appears scrambled to begin
                        animatedScrambleFont = true;
                        mainMessage = str;
                        wasTurnRelated = true;
                        turnState = TurnStateReceivingScrambled;                                                                // turnState  :  scrambled    (client)
                        currentTurn++;
                    }
                }
                for (int g = 0; g < NUM_PLACES; g++) {
                    if (strcmp(mouseButtonState, placeString[g].c_str()) == 0) {
                        useScrambledText = false;
                        mainMessage = str;
                        wasTurnRelated = true;
                        turnState = TurnStateWaiting;                                                                           // turnState  :  waiting      (client)
                    }
                }
                if(!wasTurnRelated){  // must be a number
                    connectedAgents = ofToInt(str);
                }
            }
        }
    }
}

void agentController::stopServer(){

	if (isServer){
		    for(int i = 0; i < server.getLastID(); i++) // getLastID is UID of all clients
		    {
	            if( server.isClientConnected(i) ) {
	            	server.disconnectClient(i);
	            }
		    }

		    server.close();
		    isServer = false;
	}

}
void agentController::updateSlowTCP(){
    if(oneSecond != ofGetSeconds()){   // only runs once/second
        oneSecond = ofGetSeconds();

        if(isServer){  //  send number of clients
            //connectedAgents = server.getNumClients() + 1;
            sendMessage(ofToString(connectedAgents));
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

    gameState = GameStatePlaying;                                                                                                // gameState  :  playing

    if (!curstep) {  // possible for server and clients
        step = 0;
        stepInterval = 1000;
        numSteps = 10;
        stepTimer = ofGetElapsedTimeMillis();
        preGameCountdownSequence = true;
        stepFunction = &agentController::countDown;
        currentTurn = 0;
        return;
    }

    switch (curstep) {

        case 6:
            mainMessage = "5";
            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 7:
            mainMessage = "4";
            //            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 8:
            mainMessage = "3";
            //            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 9:
            mainMessage = "2";
            //            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 10:
            mainMessage = "1";
            //            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 11:
            //mainMessage = "";

            stepInterval = 0;
            preGameCountdownSequence = false;
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
            mainMessage = "OPERATIVE I.D.";   // "PICK"
        }
        else {
            mainMessage = "OPERATIVE I.D.";  // "AGENT"
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
        gameState = GameStateDeciding;                                                                                      // gameState  :  deciding
        turnState = TurnStateNotActive;
    }

    else if (curstep %3 == 0) { // MESSAGE

        for (int i = 0 ; i < 16; i++) {
            recordedTimes[i] = 5000 + i;
        }
        // find a gesture which has not yet occurred
        do {
            mainMessage = actionString[rand()%(NUM_GESTURES-1) + 1];
        } while (actionHasOccurred(mainMessage));
        // store it in the first vacant spot in the previousActions array
        bool placed = false;
        for(int i = 0; i < NUM_TURNS; i++){
            if(!placed && strcmp(previousActions[i].c_str(), "") == 0){
                previousActions[i] = mainMessage;
                placed = true;
            }
        }

        //if (isSpy)
        useScrambledText = true;   // everyone begins with scrambled text
        animatedScrambleFont = true;
        sendMessage(mainMessage);
        ((testApp*) ofGetAppPtr())->vibrate(true);
        turnState = TurnStateReceivingScrambled;                                                                            // turnState  :  scrambled       (server)
        stepInterval = 1000 + rand() % 3000;
        currentTurn++;
    }
    else if (curstep%3 == 1) { // EXCECUTE

        sendMessage("execute");

        //        ((testApp*) ofGetAppPtr())->vibrate(true);

        stepInterval = ACTION_TIME;

        execute(mainMessage);
    }
    else if (curstep%3 == 2) { // RESULTS

        countScores();

        useScrambledText = false;

        // sendMessage(mainMessage);
        turnState = TurnStateWaiting;                                                                                       // turnState  :  waiting        (server)
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

    turnState = TurnStateAction;                                                                                            // turnState  :  action
    animatedScrambleFont = false;
    if(!isSpy){
        useScrambledText = false;
    }

    // clear recorded sensor array before every turn  // TODO separate between events which need recording and those which don't
    for(int i = 0; i < SENSOR_DATA_ARRAY_SIZE; i++)
        recordedSensorData[i] = 1.;

    ofLogNotice("RECORD MODE") << "RECORDING: " + gesture;

    char mess[128];

    strcpy(mess, gesture.c_str());

    if (strcmp(mess, "TOUCH SCREEN") == 0) {
        recordMode = GameActionTouch;
    }

    else if (strcmp(mess, "SHAKE PHONE") == 0) {
        recordMode = GameActionShake;
    }

    else if (strcmp(mess, "JUMP") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "SPIN") == 0) {
        recordMode = GameActionSpin;
    }

    else if (strcmp(mess, "HIGH FIVE\nNEIGHBOR") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "POINT AT\nAN AGENT") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "FREEZE") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "CROUCH") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "STAND ON\nONE LEG") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "TOUCH YOUR\nNOSE") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "RAISE\nA HAND") == 0) {
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
            turnState = TurnStateActionSuccess;                                                                        // turnState  :  action Success
            recordMode = 0;
            recordedTimes[0] = ofGetElapsedTimeMillis() - turnTime;
            ((testApp*) ofGetAppPtr())->vibrate(true);

            if (isClient) {
                sendMessage(ofToString(recordedTimes[0]));
            }
        }
    }
}



string paddedString(int num){
    if (num >= 100){
        return ofToString(num);
    }
    else if (num >= 10){
        return "0" + ofToString(num);
    }
    else {
        return "00" + ofToString(num);
    }
}

#pragma mark - DRAW

void agentController::drawLoginScreen() {

    string hostString = "HOST";
    string clientString = "JOIN";
    string backString = "< BACK";
    string thirdString;

    ofSetColor(255,255,255,255);
    ofEnableAlphaBlending();

    switch (loginState) {
        case LoginStateChoose:
            font.drawString(hostString,ofGetWidth()/2 - font.stringWidth(hostString)/2.,ofGetHeight()*.4 - font.stringHeight(hostString)/2.);
            font.drawString(clientString,ofGetWidth()/2 - font.stringWidth(clientString)/2.,ofGetHeight()*.6 - font.stringHeight(clientString)/2.);
            break;

        case LoginStateServer:


            hostString = "JOIN CODE";
            clientString = getCodeFromIp();

            fontMedium.drawString(backString,fontMedium.stringWidth(backString)*.35,ofGetHeight()*.1 - fontMedium.stringHeight(backString)/2.);
            font.drawString(hostString,ofGetWidth()/2 - font.stringWidth(hostString)/2.,ofGetHeight()*.4 - font.stringHeight(hostString)/2.);
            font.drawString(clientString,ofGetWidth()/2 - font.stringWidth(clientString)/2.,ofGetHeight()*.6 - font.stringHeight(clientString)/2.);


            break;

        case LoginStateClient:


            hostString = "CODE";
            clientString = getCodeFromInt(loginCode);

            fontMedium.drawString(backString,fontMedium.stringWidth(backString)*.35,ofGetHeight()*.1 - fontMedium.stringHeight(backString)/2.);
            font.drawString(hostString,ofGetWidth()/2 - font.stringWidth(hostString)/2.,ofGetHeight()*.2 - font.stringHeight(hostString)/2.);
            font.drawString(clientString,ofGetWidth()/2 - font.stringWidth(clientString)/2.,ofGetHeight()*.475 - font.stringHeight(clientString)/2.);

            increment.draw(width*.31, height*.325, width*.1, width*.1);
            increment.draw(width*.5, height*.325, width*.1, width*.1);
            increment.draw(width*.7, height*.325, width*.1, width*.1);

            decrement.draw(width*.31, height*.525, width*.1, width*.1);
            decrement.draw(width*.5, height*.525, width*.1, width*.1);
            decrement.draw(width*.7, height*.525, width*.1, width*.1);

            hostString = "JOIN";
            font.drawString(hostString,ofGetWidth()/2 - font.stringWidth(hostString)/2.,ofGetHeight()*.75 - font.stringHeight(hostString)/2.);


            break;

        case LoginStateConnecting:

            hostString = "CONNECTING TO";
            clientString = getCodeFromInt(loginCode);

            font.drawString(hostString,ofGetWidth()/2 - font.stringWidth(hostString)/2.,ofGetHeight()*.4 - font.stringHeight(hostString)/2.);
            font.drawString(clientString,ofGetWidth()/2 - font.stringWidth(clientString)/2.,ofGetHeight()*.6 - font.stringHeight(clientString)/2.);


            break;

        case LoginStateFailed:

            hostString = "BACK";
            clientString = "CONNECTION";
            thirdString = "FAILED";

            font.drawString(hostString,ofGetWidth()/2 - font.stringWidth(hostString)/2.,ofGetHeight()*.15 - font.stringHeight(hostString)/2.);
            font.drawString(clientString,ofGetWidth()/2 - font.stringWidth(clientString)/2.,ofGetHeight()*.5 - font.stringHeight(clientString)/2.);
            font.drawString(thirdString,ofGetWidth()/2 - font.stringWidth(thirdString)/2.,ofGetHeight()*.6 - font.stringHeight(thirdString)/2.);

            break;

        case LoginStateServerQuit:

            hostString = "BACK";
            clientString = "HOST STOPPED";
            thirdString = "THE GAME";

            font.drawString(hostString,ofGetWidth()/2 - font.stringWidth(hostString)/2.,ofGetHeight()*.15 - font.stringHeight(hostString)/2.);
            font.drawString(clientString,ofGetWidth()/2 - font.stringWidth(clientString)/2.,ofGetHeight()*.5 - font.stringHeight(clientString)/2.);
            font.drawString(thirdString,ofGetWidth()/2 - font.stringWidth(thirdString)/2.,ofGetHeight()*.6 - font.stringHeight(thirdString)/2.);

            break;

        case LoginStateNoIP:


            hostString = "NO ADDRESS";
            clientString = "CHECK WIFI ?";

            fontMedium.drawString(backString,fontMedium.stringWidth(backString)*.35,ofGetHeight()*.1 - fontMedium.stringHeight(backString)/2.);
            font.drawString(hostString,ofGetWidth()/2 - font.stringWidth(hostString)/2.,ofGetHeight()*.4 - font.stringHeight(hostString)/2.);
            font.drawString(clientString,ofGetWidth()/2 - font.stringWidth(clientString)/2.,ofGetHeight()*.6 - font.stringHeight(clientString)/2.);


            break;


        default:
            break;
    }
    ofDisableAlphaBlending();

}

void agentController::draw() {

    ofClear(30, 30, 30);
    if(gameState == GameStateLogin){
        drawAnimatedSphereBackground();
    }
    else if(gameState == GameStateReadyRoom){
        drawAnimatedSphereBackground();



        string backString = "< BACK";
        fontMedium.drawString(backString,fontMedium.stringWidth(backString)*.35,ofGetHeight()*.1 - fontMedium.stringHeight(backString)/2.);

        float fade = 255;
        if(step == 1 || step == 9)
            fade = 255. * (float)(ofGetElapsedTimeMillis() - stepTimer) / stepInterval;
        else if(step == 7 || step == 15)
            fade = 255. * (1 - (float)(ofGetElapsedTimeMillis() - stepTimer) / stepInterval );
        ofSetColor(255, 255, 255, fade);
        if(step >= 1 && step < 8)
            fontMedium.drawString("MESSAGE FROM HQ:\n\nWe have a detected\na double agent in your\nmidst. HQ will send\nthree encrypted commands\nfor all agents to\nenact simultaneously.", 20, centerY-200);
        else if(step >= 9 && step < 16)
            fontMedium.drawString("The double agent will\nonly receive scrambled\ncommands and will attempt\nto mimic the movements\nof the other agents.\nWatch your team closely!\n\nEND OF MESSAGE", 20, centerY-200);

        if (isServer){
        ofSetColor(255);


                string hostString = "JOIN CODE";
                string clientString = getCodeFromIp();
                fontMedium.drawString(hostString,width*.75 - fontMedium.stringWidth(hostString)/2.,ofGetHeight()*.1 - fontMedium.stringHeight(hostString)/2.);
                font.drawString(clientString,width*.5 - font.stringWidth(clientString)/2.,ofGetHeight()*.25 - font.stringHeight(clientString)/2.);


        backString = "START";
        fontMedium.drawString(backString,width*.5 - fontMedium.stringWidth(backString)*.5,height*.75 - fontMedium.stringHeight(backString)/2.);
        }
    }

    if (gameState == GameStatePlaying || gameState == GameStateDeciding || gameState == GameStateGameOver) {

        if(preGameCountdownSequence && step < 5){
            if(step >1){
                if(isSpy)
                    font.drawString("shhh", centerX-font.stringWidth("shhh")/2., centerY);
                else
                    font.drawString("AGENT", centerX-font.stringWidth("AGENT")/2., centerY);
            }
            if(step > 2){
                if(isSpy)
                    fontSmall.drawString("you are the double agent",centerX - fontSmall.stringWidth("you are the double agent")/2., centerY+font.stringHeight("shhh"));
                else
                    fontSmall.drawString("prepare yourself", centerX - fontSmall.stringWidth("prepare yourself")/2., centerY+font.stringHeight("AGENT"));
            }
        }
        else{

            ofSetColor(255, 255, 255, 255);
            ofEnableBlendMode( OF_BLENDMODE_ALPHA );

            ofEnableAlphaBlending();

            drawInGameBackground();

            //ofSetColor(6, 179, 210);   // light blue motion shape
            ofSetLineWidth(3.);

            // for drawing a circle path
            float outerRadius;
            int resolution;
            float deltaAngle;
            float angle;

            if(turnState == TurnStateAction || turnState == TurnStateActionSuccess || turnState == TurnStateWaiting){
                ofSetColor(6, 140, 210, 100);   // blue motion shape
                //ofSetColor(74,193,255, 50); // blue motion shape border
                ofFill();
                ofBeginShape();
                outerRadius = centerX*.55;
                deltaAngle = TWO_PI / (float)SENSOR_DATA_ARRAY_SIZE;
                angle = 0;
                float turnProgress = (float)(ofGetElapsedTimeMillis() - turnTime) / ACTION_TIME;   // from 0 to 1
                for(int i = 0; i < SENSOR_DATA_ARRAY_SIZE; i++){
                    if((float)i/SENSOR_DATA_ARRAY_SIZE < turnProgress){
                        float x = centerX + outerRadius * sin(angle) * recordedSensorData[i];
                        float y = centerY + outerRadius * -cos(angle) * recordedSensorData[i];
                        ofVertex(x,y);
                        angle += deltaAngle;
                    }
                }
                ofEndShape();

            }
            ofSetColor(255, 255, 255,255);

            insideCircle.draw(centerX, centerY);
            if(gameState == GameStateDeciding){
                ofSetColor(195, 17, 134);
                fingerPrint.draw(centerX, centerY,insideCircle.width*.5, insideCircle.height*.5);
            }

            if (( gameState == GameStatePlaying && !preGameCountdownSequence ) || gameState == GameStateDeciding || gameState == GameStateGameOver){
                ofNoFill();
                if(turnState == TurnStateReceivingScrambled || turnState == TurnStateAction || gameState == GameStateDeciding)
                    ofSetColor(195, 17, 134);   // pink
                else
                    ofSetColor(10, 223, 255, 255);   // light blue
                ofBeginShape();
                outerRadius = centerX*.52;
                resolution = 64;
                deltaAngle = TWO_PI / (float)resolution;
                angle = 0;
                float roundProgress = (float)currentTurn / NUM_TURNS;
                for(int i = 0; i <= resolution; i++){
                    if((float)i/resolution <= roundProgress){
                        float x = centerX + outerRadius * sin(angle);
                        float y = centerY + outerRadius * -cos(angle);
                        ofVertex(x,y);
                        angle += deltaAngle;
                    }
                }
                ofEndShape();
            }

            ofDisableAlphaBlending();

            // BACK TO BUSINESS

            if (mainMessage.length()){
                ofSetColor(255,255,255,255);

                if (animatedScrambleFont) {
                    //if(rand()%2 == 0){
                    int index = rand()%4;
                    spymess[index] = rand()%23+65;
                    //}
                    font.drawString(ofToString(spymess),ofGetWidth()/2 - font.stringWidth(spymess)/2.,ofGetHeight()/2 + font.stringHeight(ofToString(spymess))/2.);
                }
                else if (useScrambledText){
                    font.drawString(ofToString(spymess),ofGetWidth()/2 - font.stringWidth(spymess)/2.,ofGetHeight()/2 + font.stringHeight(ofToString(spymess))/2.);
                }
                else
                    font.drawString(mainMessage,ofGetWidth()/2 - font.stringWidth(mainMessage)/2.,ofGetHeight()/2 + font.stringHeight(mainMessage)/2.);
            }
        }
    }

    if (!isServer && !isClient) {  // if not server or client

        drawLoginScreen();

    }

    if(gameState == GameStateLogin){
        lowerTextLine1 = "INTEL WELCOMES YOU TO";
        lowerTextLine2 = "DOUBLE AGENT";
        lowerTextLine3 = "Please host or join a game";
    }

    if(gameState == GameStateReadyRoom){
        if (isServer) {
            lowerTextLine1 = "YOU ARE THE HOST";
            lowerTextLine2 = getCodeFromIp();
            lowerTextLine3 = "Share this code for others to log in";
        }
        else {
            lowerTextLine1 = "YOU ARE CONNECTED";
            lowerTextLine2 = "to host: " + getCodeFromInt(loginCode);
            lowerTextLine3 = "Waiting for host to start the game";
        }
    }

    if(gameState == GameStatePlaying){
        if(preGameCountdownSequence){
            if(step > 6){
                lowerTextLine1 = "AGENT ID";
                lowerTextLine2 = "AGENT";
                lowerTextLine3 = "Purpose: to identify the double agent";
            }
            else{
                lowerTextLine1 = lowerTextLine2 = lowerTextLine3 = "";
            }
        }
        else{
            lowerTextLine1 = "AGENT ID";
            lowerTextLine2 = "AGENT";
            lowerTextLine3 = "Purpose: to identify the double agent";
        }
    }
    if(gameState == GameStateDeciding){
        lowerTextLine1 = "TARGET";
        lowerTextLine2 = "DOUBLE AGENT";
        lowerTextLine3 = "Select operative identified as double agent";
    }
    if(gameState == GameStateGameOver){
        lowerTextLine1 = "MISSION";
        if(strcmp(mainMessage.c_str(), "SPY CAPTURED!") == 0){
            lowerTextLine2 = "SUCCESS";
            lowerTextLine3 = "You have sucessfully uncovered the double agent";
        }
        else if(strcmp(mainMessage.c_str(), "NOPE!") == 0){
            lowerTextLine2 = "FAIL";
            lowerTextLine3 = "the double agent got away";
        }
    }

    ofSetColor(60, 60, 60, 255);
    fontSmall.drawString(lowerTextLine1, 60, centerY+centerY*.66);
    ofSetColor(180, 180, 180, 255);
    fontSmall.drawString(lowerTextLine2, 60, centerY+centerY*.66+40);
    fontSmall.drawString(lowerTextLine3, 60, centerY+centerY*.66+80);


    if (connectedAgents > 1){   // CONNECTED AGENTS
        string count = connectedAgentsStrings[connectedAgents];//ofToString(connectedAgents);
        ofSetColor(200,200,200);
        font.drawString(count, centerX-font.stringWidth(count)/2.,ofGetHeight() - font.stringHeight(count));
    }
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

void agentController::drawInGameBackground(){
    //    reticle.draw(0, 0);
    ofSetColor(255, 255, 255, 75);
    reticleOutline.draw(centerX, centerY);
    ofPushMatrix();
    float compassAngle = atan2f(orientation.d, orientation.e);
    ofTranslate(centerX, centerY);
    ofRotate(compassAngle*180/PI);
    reticleCompass.draw(0,0);
    ofPopMatrix();
    ofPushMatrix();
    float reticleInsideAngle = asinf(orientation.f);
    ofTranslate(centerX, centerY);
    ofRotate(reticleInsideAngle*180/PI);
    reticleInside.draw(0, 0);
    ofPopMatrix();
}

void agentController::drawAnimatedSphereBackground() {

    float spinX = sin(ofGetElapsedTimef()*.35f);
    float spinY = cos(ofGetElapsedTimef()*.075f);
    sphere.setPosition(ofGetWidth()*.5, ofGetHeight()*.5, 200.);
    sphere.rotate(spinX, 1.0, 0.0, 0.0);
    sphere.rotate(spinY, 0, 1.0, 0.0);

    //    ofEnableDepthTest();   //  presently unnecessary, processor intensive
    ofEnableLighting();
    ofNoFill();
    ofSetColor(255);
    ofSetLineWidth(.1);
    sphere.drawWireframe();
    ofFill();
    ofDisableLighting();
    //    ofDisableDepthTest();
}

#pragma mark - LOGIC

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

void agentController::update() {

    if (isClient || isServer) {

      	updateTCP();
        updateSlowTCP();

        if(turnState == TurnStateAction){
            float turnProgress = (float)(ofGetElapsedTimeMillis() - turnTime) / ACTION_TIME;   // from 0 to 1
            int index = SENSOR_DATA_ARRAY_SIZE*turnProgress;
            if(index < 0) index = 0;
            if(index >= SENSOR_DATA_ARRAY_SIZE) index = SENSOR_DATA_ARRAY_SIZE-1;
            float maxScale = 4*getMaxSensorScale() + 1.;
            recordedSensorData[index] = maxScale;
        }

        if(gameState == GameStateGameOver){
            if (ofGetElapsedTimeMillis() - stepTimer > stepInterval ){
                gameState = GameStateReadyRoom;
                mainMessage = "";
                stepInterval = 1000;
                step = 0;
                stepTimer = ofGetElapsedTimeMillis();
            }
        }
        else if (gameState == GameStateReadyRoom){
            if (ofGetElapsedTimeMillis() - stepTimer > stepInterval ){
                step++;

                if (step > 16){//numSteps){
                    //stepInterval = 0;
                    step = 0;
                }
                stepTimer = ofGetElapsedTimeMillis();
            }
        }
        else{
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
}

// server only function, i think
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
    gameState = GameStateGameOver;                                                                                          // gameState
    stepTimer = ofGetElapsedTimeMillis();
    stepInterval = 5000;
}

void agentController::startGame(){

    if (isServer){

        gameState = GameStatePlaying;                                                                                       // gameState
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

    switch (gameState) {
        case GameStateLogin:

            switch (loginState) {

                case LoginStateChoose:

                    if (y < centerY) {

                        int con = serverConnect();

                        if (con == -1){
                            loginState = LoginStateNoIP;
                            ofLogNotice("choose screen") << "no ip";
                        }
                        else if (con == 0) {
                            loginState = LoginStateFailed;
                        }
                        else {
                            ofLogNotice("chose server") << "yes!";
                             //loginState = LoginStateServer;

                            isServer = true;
                            gameState = GameStateReadyRoom;
                            stepInterval = 1000;
                            step = 0;
                            stepTimer = ofGetElapsedTimeMillis();

                        }

                    }
                    else {
                        loginState = LoginStateClient;
                    }
                    break;

                case LoginStateServer:


                    break;

                case LoginStateClient:

                    if (y < height * .2) {
                        loginState = LoginStateChoose;
                    }

                    else if (y < height * .65){

                        if (x < width * .4) {

                            if (y < height * .5) { // inc 100's
                                loginCode >= 200 ? loginCode -= 200 : loginCode += 100;
                            }
                            else {
                                loginCode < 100 ? loginCode += 200 : loginCode -= 100;
                            }
                        }

                        else if (x > width *.4 && x < width * .6){

                             int tens = loginCode - ((loginCode / 100) * 100);

                            if (y < height * .5) { // inc 100's
                                tens >= 90 ? loginCode -= 90 : loginCode += 10;
                            }
                            else {
                                tens < 10 ? loginCode += 90 : loginCode -= 10;
                            }

                        }
                        else if (x > width * .6){

                            int tens = loginCode - ((loginCode / 100) * 100);
                            int ones = tens - ((tens / 10) * 10);

                            if (y < height * .5) { // inc 100's
                                ones >= 9 ? loginCode -= 9 : loginCode += 1;
                            }
                            else {
                                ones < 1 ? loginCode += 9 : loginCode -= 1;
                            }

                        }

                    }

                    else {

                        int con = clientConnect();

                        if (con == 0) {
                            loginState = LoginStateFailed;
                        }
                        else if (con == -1){
                            loginState = LoginStateNoIP;
                        }
                        else {
                            isClient = true;
                            gameState = GameStateReadyRoom;                                                                            // gameState  :  connected
                            ofLogNotice("+++ GameState updated:") << "Ready Room, setIPAddress()";
                            stepInterval = 1000;
                            step = 0;
                            stepTimer = ofGetElapsedTimeMillis();
                        }
                    }

                    break;

                case LoginStateConnecting:

                    break;

                case LoginStateFailed: case LoginStateNoIP: case LoginStateServerQuit:

                    loginState = LoginStateChoose;

                    break;



                default:
                    break;
            }
            break;

        case GameStateReadyRoom:

                if (y < height * .2) {
                    gameState = GameStateLogin;
                    loginState = LoginStateChoose;
                    if (isServer){stopServer(); isServer = 0;}
                    if (isClient){client.close(); isClient = 0;}
                }
                else if (isServer && connectedAgents > 1){
                    sendMessage("startGame");
                    //sleep(250);
                    startGame();
                }


            break;
        case GameStatePlaying:
            if (recordMode == GameActionTouch) {
                turnState = TurnStateActionSuccess;                                                                         // turnState  :  action success
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

    char code[8];

    code[1] = ' ';
    code[2] = ' ';
    code[4] = ' ';
    code[5] = ' ';

    const char* last = codeString.c_str();
    code[0] = last[0];
    code[3] = last[1];
    code[6] = last[2];
    code[7] = '\0';

    return string(code);

}



string agentController::getCodeFromIp(){

    if (localIP.compare("error") == 0 || !localIP.length()) return "error";

    std::vector<std::string> result;

    result = ofSplitString(localIP,".");


    if (result[3].length() < 3){

        char code[8];

        code[1] = ' ';
        code[2] = ' ';
        code[4] = ' ';
        code[5] = ' ';

        const char* last = result[3].c_str();
        code[0] = '0';
        code[3] = last[0];
        code[6] = last[1];
        code[7] = '\0';

        return string(code);

    }

    else {
        char code[8];

        code[1] = ' ';
        code[2] = ' ';
        code[4] = ' ';
        code[5] = ' ';

        const char* last = result[3].c_str();
        code[0] = last[0];
        code[3] = last[1];
        code[6] = last[2];
        code[7] = '\0';

        return string(code);
    }



}

int agentController::serverConnect(){

    if (localIP.compare("error") == 0 || !localIP.length()) {
        ofLogNotice("TCP") << "SERVER FAILED ! NO IP";
        return -1;
    }

    if (server.setup(PORT)) {
        ofLogNotice("TCP") << "IS SERVER AT:" + localIP + " on port:" << ofToString(PORT);
        return 1;
    }

    else {
        server.close();
        ofLogNotice("TCP") << "SERVER FAILED !!";
        ofLogNotice("+++ GameState updated:") << "Waiting For Sign In  setIPAddress1";
        return 0;
    }

}

int agentController::clientConnect(){

    if (localIP.compare("error") == 0 || !localIP.length()) {
        ofLogNotice("TCP") << "CLIENT FAILED ! NO IP";
        return -1;
    }

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

        serverIP = std::string(serverString);

        serverIP += ofToString(loginCode);


        ofLogNotice("+++ Connecting to server:") << serverIP;


        if (client.setup (serverIP, PORT)){
            ofLogNotice("TCP") << "connect to server at " + serverIP + " port: " << ofToString(PORT) << "\n";
            mainMessage = "";  // "Agent"

            return 1;
        }

        return 0;



}


void agentController::pause(){
    exit();
}

void agentController::resume(){

	//setup();

	if (isServer){
		if (server.setup(PORT)){
			ofLogNotice("TCP") << "Successfully resumed Server";
            gameState = GameStateReadyRoom;                                                                            // gameState  :  connected
            stepInterval = 1000;
            step = 0;
            stepTimer = ofGetElapsedTimeMillis();

		}
	}
	if (isClient){
		if (!client.isConnected()){
            if (client.setup(serverIP, PORT)){
                ofLogNotice("TCP") << "Reconnected Client";
                // this is not flushed out. game could be in progress, not in ready room
                gameState = GameStateReadyRoom;                                                                         // gameState  :  connected
                stepInterval = 1000;
                step = 0;
                stepTimer = ofGetElapsedTimeMillis();

            }
		}
	}
}

void agentController::exit() {
	if (isServer){
        ofLogNotice("TCP") << "Shutting down Server";
        stopServer();
        gameState = GameStateLogin;                                                                            // gameState  :  disconnected
        ofLogNotice("+++ GameState updated:") << "Waiting For Sign In, exit()";
	}
	if (isClient){
		ofLogNotice("TCP") << "Shutting down Client";
		client.close();
        gameState = GameStateLogin;                                                                            // gameState  :  disconnected
        ofLogNotice("+++ GameState updated:") << "Waiting For Sign In, exit()";
	}
}
