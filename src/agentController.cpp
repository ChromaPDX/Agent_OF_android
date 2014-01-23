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

    //	font.loadFont("digital-7.ttf", ofGetWidth() / 9., true, true);   // 4.2
    font.loadFont("Avenir.ttf", ofGetWidth() / 12., true, true);   // 4.2
    spyfont.loadFont("webdings.ttf", ofGetWidth() / 5., true, true);
    fontSmall.loadFont("AvenirNextCondensed.ttf", 24, true, true);
    fontMedium.loadFont("AvenirNextCondensed.ttf", 36, true, true);
    spymess[0] = rand()%23+65;
    spymess[1] = rand()%23+65;
    spymess[2] = rand()%23+65;
    spymess[3] = rand()%23+65;
    spymess[4] = NULL;
    //	font.setLetterSpacing(1.037);

    //    elapsed = ofGetElapsedTimeMillis();
    gameState = GameStateWaitingForSignIn;                                                                                  // gameState  :  waiting
    turnState = TurnStateNotActive;                                                                                         // turnState  :  not active
    ofLogNotice("+++ GameState updated:") << "Waiting For Sign In, setup()";

    sphere.setRadius( ofGetWidth()  );
    ofSetSphereResolution(24);

    reticleOutline.loadImage("reticle_outline.png");
    reticleOutline.setAnchorPercent(.5, .5);
    reticleCompass.loadImage("reticle_compass.png");
    reticleCompass.setAnchorPercent(.5, .5);
    fingerPrint.loadImage("fingerprint.png");
    fingerPrint.setAnchorPercent(.5, .5);
    insideCircle.loadImage("inside_circle.png");
    insideCircle.setAnchorPercent(.5, .5);
    reticleInside.loadImage("reticle_inside.png");
    reticleInside.setAnchorPercent(0, 0);
    reticleInside.crop(reticleInside.width/2., reticleInside.height/2., reticleInside.width/2., reticleInside.height/2.);

    centerX = ofGetWidth()/2.;
    centerY = ofGetHeight()/2.;

    //ofSetSmoothLighting(true);
}

#pragma mark NETWORK

void agentController::updateTCP() {

	if (isServer){

        connectedAgents = server.getNumClients() + 1;

	    for(int i = 0; i < server.getLastID(); i++) { // getLastID is UID of all clients

            if( server.isClientConnected(i) ) { // check and see if it's still around
                //connectedAgents++;

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
                mainMessage = "OPERATIVE I.D.";  // "PICK"
                gameState = GameStateDeciding;                                                                                  // gameState  :  deciding
            }
            else if (strcmp(mouseButtonState, "PICK") == 0) {
                ((testApp*) ofGetAppPtr())->vibrate(true);
                //execute(mainMessage);
                mainMessage = "OPERATIVE I.D.";  // "AGENT"
                gameState = GameStateDeciding;                                                                                  // gameState  :  deciding
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

void agentController::updateSlowTCP(){
    if(oneSecond != ofGetSeconds()){   // only runs once/second
        oneSecond = ofGetSeconds();

        if(isServer){  //  send number of clients
            connectedAgents = server.getNumClients() + 1;
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
    }
    else if (curstep%3 == 1) { // EXCECUTE

        sendMessage("execute");

        //        ((testApp*) ofGetAppPtr())->vibrate(true);

        stepInterval = 3000;

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

    else if (strcmp(mess, "LANDSCAPE\nMODE") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "SET PHONE\nFACE DOWN") == 0) {
        recordMode = GameActionJump;
    }

    else if (strcmp(mess, "PUNCH") == 0) {
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

    else if (strcmp(mess, "DRAW A\nCIRCLE") == 0) {
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


#pragma mark - DRAW

void agentController::draw() {

    ofClear(30, 30, 30);
    if(gameState == GameStateWaitingForSignIn){
        drawAnimatedSphereBackground();
    }
    else if(gameState == GameStateReadyRoom){
        drawAnimatedSphereBackground();
        float fade = 255;
        if(step == 1 || step == 9)
            fade = 255. * (float)(ofGetElapsedTimeMillis() - stepTimer) / stepInterval;
        else if(step == 7 || step == 15)
            fade = 255. * (1 - (float)(ofGetElapsedTimeMillis() - stepTimer) / stepInterval );
        ofSetColor(255, 255, 255, fade);
        if(step >= 1 && step < 8)
            fontMedium.drawString("one of you has been turned\nagainst us. to purge the\ndouble agent, HQ will dispatch\n3 commands for all to\nexecute simultaneously.", 20, centerY-100);
        else if(step >= 9 && step < 16)
            fontMedium.drawString("the double agent will\nonly intercept scrambled\ncommands. it is up to them\nto fake it by watching\ntheir fellow agents.", 20, centerY-100);
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
            ofNoFill();
            if(turnState == TurnStateReceivingScrambled || turnState == TurnStateAction || gameState == GameStateDeciding)
                ofSetColor(195, 17, 134);   // pink
            else
                ofSetColor(10, 223, 255, 255);   // light blue
            ofBeginShape();
            float outerRadius = centerX*.52;
            int resolution = 64;
            float deltaAngle = TWO_PI / (float)resolution;
            float angle = 0;
            for(int i = 0; i <= resolution; i++){
                float x = centerX + outerRadius * cos(angle);
                float y = centerY + outerRadius * sin(angle);
                ofVertex(x,y);
                angle += deltaAngle;
            }
            ofEndShape();



            if(gameState == GameStateDeciding)
                fingerPrint.draw(centerX, centerY,insideCircle.width*.75, insideCircle.height*.75);

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
        mainMessage = "NO LINK";
        ofSetColor(255,255,255);
        font.drawString(mainMessage,ofGetWidth()/2 - font.stringWidth(mainMessage)/2.,ofGetHeight()/2 + font.stringHeight(mainMessage)/2.);
    }
    if(gameState == GameStateWaitingForSignIn){
        lowerTextLine1 = "SEARCHING";
        lowerTextLine2 = "NEARBY HOSTS";
        lowerTextLine3 = "Nothing yet..";
    }
    if(gameState == GameStateReadyRoom){
        lowerTextLine1 = "CONNECTED";
        lowerTextLine2 = "READY ROOM";
        lowerTextLine3 = "Waiting for the stragglers";
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


    if (connectedAgents){   // CONNECTED AGENTS
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
    insideCircle.draw(centerX, centerY);
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

void agentController::update() {

    if (isClient || isServer) {

      	updateTCP();
        updateSlowTCP();

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
            //        case GameStateWaitingForSignIn:
        case GameStateReadyRoom:
            if (isServer) {
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

            char broadcast[16];
            std::vector<std::string> result;

            result = ofSplitString(std::string(ipAddress),".");

            const char* last = result[3].c_str();

            if (last[0] == '1'){

                if (server.setup(PORT)) {
                    isServer = true;
                    ofLogNotice("TCP") << "IS SERVER AT:" + localIP + " on port:" << ofToString(PORT);
                    mainMessage = "";  // "Agent"

                    gameState = GameStateReadyRoom;                                                                            // gameState  :  connected
                    stepInterval = 1000;
                    step = 0;
                    stepTimer = ofGetElapsedTimeMillis();
                }
                else {
                    server.close();
                    ofLogNotice("TCP") << "SERVER FAILED !!";
                    mainMessage = "RESTART";

                    gameState = GameStateWaitingForSignIn;                                                                            // gameState  :  disconnected

                    ofLogNotice("+++ GameState updated:") << "Waiting For Sign In  setIPAddress1";

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
                    mainMessage = "";  // "Agent"

                    gameState = GameStateReadyRoom;                                                                            // gameState  :  connected
                    ofLogNotice("+++ GameState updated:") << "Ready Room, setIPAddress()";
                    stepInterval = 1000;
                    step = 0;
                    stepTimer = ofGetElapsedTimeMillis();

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
        server.close();
        gameState = GameStateWaitingForSignIn;                                                                            // gameState  :  disconnected
        ofLogNotice("+++ GameState updated:") << "Waiting For Sign In, exit()";
	}
	if (isClient){
		ofLogNotice("TCP") << "Shutting down Client";
		client.close();
        gameState = GameStateWaitingForSignIn;                                                                            // gameState  :  disconnected
        ofLogNotice("+++ GameState updated:") << "Waiting For Sign In, exit()";
	}
}
