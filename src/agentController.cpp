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

	font.loadFont("verdana.ttf", ofGetWidth() / 4., true, true);
//	font.setLineHeight(18.0f);
//	font.setLetterSpacing(1.037);

    fontSmall.loadFont("verdana.ttf", 40, true, true);


    elapsed = ofGetElapsedTimeMillis();
    state = GameStateWaitingForSignIn;


    // DO POLYGONS

    nCurveVertices = 7;

	curveVertices[0].x = 326;
	curveVertices[0].y = 209;
	curveVertices[1].x = 306;
	curveVertices[1].y = 279;
	curveVertices[2].x = 265;
	curveVertices[2].y = 331;
	curveVertices[3].x = 304;
	curveVertices[3].y = 383;
	curveVertices[4].x = 374;
	curveVertices[4].y = 383;
	curveVertices[5].x = 418;
	curveVertices[5].y = 309;
	curveVertices[6].x = 345;
	curveVertices[6].y = 279;

	for(int i = 0; i < nCurveVertices; i++){
		curveVertices[i].bOver 			= false;
		curveVertices[i].bBeingDragged 	= false;
		curveVertices[i].radius = 4;
	}

}


//void agentController::updateOSC() {
//	// hide old messages
//    for ( int i=0; i<NUM_MSG_STRINGS; i++ )
//    {
//        if ( timers[i] < ofGetElapsedTimef() )
//            msg_strings[i] = "";
//    }
//
//    // check for waiting messages
//    while( receiver.hasWaitingMessages() )
//    {
//        // get the next message
//        ofxOscMessage m;
//        receiver.getNextMessage( &m );
//
//        // check for mouse moved message
//        if ( strcmp(m.getAddress().c_str(), "/mouse/position" ) == 0 )
//        {
//            // both the arguments are int32's
//            mouseX = m.getArgAsInt32( 0 );
//            mouseY = m.getArgAsInt32( 1 );
//        }
//        // check for mouse button message
//        else if ( strcmp(m.getAddress().c_str(), "/mouse/button" ) == 0 )
//        {
//            // the single argument is a string
//            strcpy( mouseButtonState, m.getArgAsString( 0 ).c_str() );
//
//            if (strcmp(mouseButtonState, "down") == 0) {
//                ofLogNotice("UI") << "Touch Down";
//            }
//            else {
//                ofLogNotice("UI") << "Touch Up";
//            }
//
//        }
//        else
//        {
//            // unrecognized message: display on the bottom of the screen
//            char msg_string[16384];
//            strcpy( msg_string, m.getAddress().c_str() );
//            strcat( msg_string, ": " );
//            for ( int i=0; i<m.getNumArgs(); i++ )
//            {
//                // get the argument type
//                strcat( msg_string, m.getArgTypeName( i ).c_str() );
//                strcat( msg_string, ":" );
//                // display the argument - make sure we get the right type
//                if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
//                    sprintf( msg_string, "%s%d ", msg_string,m.getArgAsInt32( i ) );
//                else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
//                    sprintf( msg_string, "%s%f ", msg_string, m.getArgAsFloat( i ) );
//                else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
//                    sprintf( msg_string, "%s\"%s\" ", msg_string, m.getArgAsString( i ).c_str() );
//                else
//                    strcat( msg_string, "unknown" );
//            }
//        }
//
//        m.clear();
//
//    }
//}

#pragma mark NETWORK




void agentController::updateTCP() {

	if (isServer){

        connectedAgents = 0;

	    for(int i = 0; i < server.getLastID(); i++) // getLastID is UID of all clients
	    {

            if( server.isClientConnected(i) ) { // check and see if it's still around
                connectedAgents++;

                // maybe the client is sending something
                string str = server.receive(i);
                //server.send(i, "You sent: "+str);

                if (str.length()){

                	strcpy( mouseButtonState, str.c_str() );

                    ofLogNotice("TCP") << "Received From Client" + ofToString(i) + " : " + str;


                    if (strcmp(mouseButtonState, "pickedAgent") == 0) {
                        pickedAgent();
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
            }

            else if (strcmp(mouseButtonState, "execute") == 0) {
                 ((testApp*) ofGetAppPtr())->vibrate(true);
                 execute(mainMessage);

            }

            else if (strcmp(mouseButtonState, "PICK") == 0) {
                ((testApp*) ofGetAppPtr())->vibrate(true);
                //execute(mainMessage);
                mainMessage = "PICK";
            }

            for (int g = 0; g < NUM_GESTURES; g++) {
                if (strcmp(mouseButtonState, actionString[g].c_str()) == 0) {
                    mainMessage = str;
                }

            }

            for (int g = 0; g < NUM_PLACES; g++) {
                if (strcmp(mouseButtonState, placeString[g].c_str()) == 0) {
                    mainMessage = str;
                }

            }


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

    switch (curstep) {

        case 1:
            mainMessage = "3";
            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 2:
            mainMessage = "2";
            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 3:
            mainMessage = "1";
            ((testApp*) ofGetAppPtr())->vibrate(true);
            break;

        case 4:
            mainMessage = "";

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

void agentController::serveRound(int curstep){

    if (!curstep) {
        step = 0;
        stepInterval = 5000;
        numSteps = 4*3;
        stepTimer = ofGetElapsedTimeMillis();
        stepFunction = &agentController::serveRound;
    }


    if (curstep == numSteps) {
        mainMessage = "PICK";
        sendMessage("PICK");
        stepInterval = 0;
        state = GameStateDeciding;
    }

    else if (curstep %3 == 0) { // MESSAGE

        mainMessage = actionString[rand()%3 + 1];
        sendMessage(mainMessage);

        stepInterval = 3000 + rand() % 3000;

    }

    else if (curstep%3 == 1) { // EXCECUTE

        sendMessage("execute");

        ((testApp*) ofGetAppPtr())->vibrate(true);

        stepInterval = 3000;

    }

    else if (curstep%3 == 2) { // RESULTS

        mainMessage = placeString[rand()%3];
        sendMessage(mainMessage);

        stepInterval = 6000;


    }





}

void agentController::execute(string gesture){

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

    else {
        recordMode = GameActionFreeze;
    }

    turnTime = ofGetElapsedTimeMillis();


}

#pragma mark - DRAW

void agentController::draw() {

    ofClear(0, 0, 0);

    ofFill();

    if (isServer || isClient) {


        float alpha = (float)0.9;

        filteredAccel.x = alpha * filteredAccel.x + (1 - alpha) * normAccel.x;
        filteredAccel.y = alpha * filteredAccel.y + (1 - alpha) * normAccel.y;
        filteredAccel.z = alpha * filteredAccel.z + (1 - alpha) * normAccel.z;



        int nTips = 5;// + xPct * 60;



        //float innerRadius = 10;//0 + yPct*80;

        //        float outerRadius = 80;
        float origx = ofGetWidth()/2.;
        float origy = ofGetHeight()/2.;

        ofEnableAlphaBlending();

        for (int r = 0; r < 8; r++){

            int nStarPts = r+3;

            float angleChangePerPt = TWO_PI / (float)nStarPts;

            float angle = (sin(ofGetElapsedTimef() / 400.) * 360) + r*30;

            float outerRadius = 120 + (200*(fabs(filteredAccel.z))) - (r*5. + (fabs(filteredAccel.y)*30.));
            //            float origx = ofGetWidth()/2. + (filteredAccel.x) + rand()%80 - 40;
            //            float origy = ofGetHeight()/2. + (filteredAccel.y) + rand() % 80 - 40;


            ofSetColor(50+(20*r), (20*filteredAccel.x + filteredAccel.y), 50+(20*r), 200);
            // ofSetHexColor(0xa16bca);
            ofBeginShape();
            for (int i = 0; i < nStarPts; i++){
                // if (i % 2 == 0) {
                // inside point:
                //                float x = origx + innerRadius * cos(angle);
                //                float y = origy + innerRadius * sin(angle);
                //                ofVertex(x,y);
                //            } else {
                //                // outside point
                float x = origx + outerRadius * cos(angle);
                float y = origy + outerRadius * sin(angle);
                ofVertex(x,y);
                //            }
                angle += angleChangePerPt;
            }
            ofEndShape();

        }


        ofSetLineWidth(1.);

        for (int r = 0; r < 12; r++){


            float angle = (sin(ofGetElapsedTimef() / 400.) * 360) + r*30;

            //            float origx = ofGetWidth()/2. + (filteredAccel.x) + rand()%80 - 40;
            //            float origy = ofGetHeight()/2. + (filteredAccel.y) + rand() % 80 - 40;


            ofSetColor(50+(20*r), (20*filteredAccel.x + filteredAccel.y), 100+(20*r), 200);
            // ofSetHexColor(0xa16bca);


            float x = origx + (filteredAccel.y*ofGetWidth()  + r*30) * cos(-angle);
            float y = origy + (filteredAccel.y*ofGetWidth()  + r*30) * sin(-angle);
            ofLine(origx,origy,x,y);

            angle += 15;

        }


        ofSetColor(50, 50, 50,200);
        ofBeginShape();

        float x = ofGetWidth();
        float y = ofGetHeight();

        ofVertex(x*.1,y*.33);
        ofVertex(x*.9,y*.33);
        ofVertex(x*.9,y*.66);
        ofVertex(x*.1,y*.66);

        ofEndShape();









        ofDisableAlphaBlending();

        //-------------------------------------

        //	ofSetColor(255,255,0);
        //    font.drawString(messages[0],10,font.stringHeight(messages[0])+20);
        //    ofSetColor(255,0,255);
        //    font.drawString(messages[1],10,(font.stringHeight(messages[0])+20)*2);
        //    ofSetColor(0,255,255);
        //    font.drawString(messages[2],10,(font.stringHeight(messages[0])+20)*3);

        //    ofPushMatrix();
        //    ofTranslate(ofGetWidth()*.5, ofGetHeight()*.5);
        //
        ////    ofSetColor(200,200,200);
        ////    ofLine(0,0,filteredAccel.x*ofGetWidth()/2,0);
        ////    ofLine(0,0,0,-filteredAccel.y*ofGetHeight()/2);
        //
        //        ofSetLineWidth(1.);
        //
        //    for (int i = 0; i < 8; i++){
        //
        //        ofSetColor(100 + (i*20),100,100 + (i*20));
        //
        //        ofLine(0,0,filteredAccel.x*ofGetWidth()/(8. - i),filteredAccel.x*ofGetWidth()/(float)i);
        //
        //        ofLine(0,0,filteredAccel.y*ofGetWidth()/(8. - i),-filteredAccel.y*ofGetWidth()/(float)i);
        //        //ofLine(0,0,normAccel.y*ofGetHeight()/2,-normAccel.y*ofGetHeight()/3);
        //
        //    }
        //
        //    //ofLine(0,0,0,-normAccel.y*ofGetHeight()/3);
        //
        //    // we don't draw z as the perspective might be confusing
        //    // but it's approximately one when the device is still and parallel
        //    // to the ground
        //    ofPopMatrix();

        //        ofFill();
        //        ofSetHexColor(0xe0be21);
        //
        //        //------(a)--------------------------------------
        //        //
        //        // 		draw a star
        //        //
        //        // 		use poly winding odd, the default rule
        //        //
        //        // 		info about the winding rules is here:
        //        //		http://glprogramming.com/red/images/Image128.gif
        //        //
        //        ofSetPolyMode(OF_POLY_WINDING_ODD);	// this is the normal mode
        //        ofBeginShape();
        //		ofVertex(200,135);
        //		ofVertex(15,135);
        //		ofVertex(165,25);
        //		ofVertex(105,200);
        //		ofVertex(50,25);
        //        ofEndShape();
        //
        //
        //        //------(b)--------------------------------------
        //        //
        //        // 		draw a star
        //        //
        //        // 		use poly winding nonzero
        //        //
        //        // 		info about the winding rules is here:
        //        //		http://glprogramming.com/red/images/Image128.gif
        //        //
        //        ofSetHexColor(0xb5de10);
        //        ofSetPolyMode(OF_POLY_WINDING_NONZERO);
        //        ofBeginShape();
        //		ofVertex(400,135);
        //		ofVertex(215,135);
        //		ofVertex(365,25);
        //		ofVertex(305,200);
        //		ofVertex(250,25);
        //        ofEndShape();
        //        //-------------------------------------



        //------(c)--------------------------------------
        //
        // 		draw a star dynamically
        //
        // 		use the mouse position as a pct
        //		to calc nPoints and internal point radius
        //
        //        float xPct = (float)(ofGetWidth()) / 20.;
        //        float yPct = (float)(ofGetHeight()) / 20.;

        //        float xPct = .1;
        //        float yPct = .1;
        //



        // BACK TO BUSINESS

        if (mainMessage.length()){
            ofSetColor(255,255,255);
            font.drawString(mainMessage,ofGetWidth()/2 - font.stringWidth(mainMessage)/2.,ofGetHeight()/2 + font.stringHeight(mainMessage)/2.);
        }


    }

    if (isServer) { // SERVER ONLY DRAW

        // CONNECTED AGENTS

        if (connectedAgents){

            string count = ofToString(connectedAgents);
            ofSetColor(200,200,200);
            fontSmall.drawString(count,ofGetWidth()*.9 - fontSmall.stringWidth(count)/2.,ofGetHeight()*.9 + fontSmall.stringHeight(count)/2.);

        }


    }

    //    char buf[256];
    //    sprintf( buf, "listening for osc messages on port %d", PORT );
    //    ofDrawBitmapString( buf, 10, 20 );
    //
    //    // draw mouse state
    //    sprintf( buf, "mouse: % 4d % 4d", mouseX, mouseY );
    //    ofDrawBitmapString( buf, 0, 220 );
    //    ofDrawBitmapString( mouseButtonState, 0, 320 );
    //
    //
    //    for ( int i=0; i<NUM_MSG_STRINGS; i++ )
    //    {
    //        ofDrawBitmapString( (char*)msg_strings[i].c_str(), 10, 40+15*i );
    //    }

}

#pragma mark - LOGIC

void agentController::update() {

    if (isClient || isServer) {
      	updateTCP();



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

void agentController::pickedAgent() {


        if(isSpy){
            sendMessage("agentDiscovered");
            mainMessage = "GOT HIM!";
        }
        else {
            sendMessage("agentNotDiscovered");
            mainMessage = "NOPE!";
        }


        state = GameStateWaitingForSignIn;


}

void agentController::startGame(){

    step = 0;
    stepInterval = 2000;
    numSteps = 3;
    stepTimer = ofGetElapsedTimeMillis();


    stepFunction = &agentController::countDown;


    //    // for self (server)
    //    [delegate flashTextShort:@"3"];
    //    [(id)delegate performSelector:@selector(flashTextShort:) withObject:@"2" afterDelay:1.0];
    //    [(id)delegate performSelector:@selector(flashTextShort:) withObject:@"1" afterDelay:2.0];
    //
    //    // send
    //    [self serverSendString:@"3"];
    //    [self performSelector:@selector(serverSendString:) withObject:@"2" afterDelay:1.0];
    //    [self performSelector:@selector(serverSendString:) withObject:@"1" afterDelay:2.0];
    //    [self performSelector:@selector(serverSendString:) withObject:@"begin" afterDelay:3.0];
    //    [self performSelector:@selector(beginRoundForServer) withObject:nil afterDelay:4.0];



}




#pragma mark UX

//--------------------------------------------------------------
void agentController::touchBegan(int x, int y, int id){


	unsigned long long time = ofGetElapsedTimeMillis();

	sendMessage(ofToString(time - lastTime));

	lastTime = time;

    //	GameStateWaitingForSignIn,
    //	GameStatePlaying,
    //    GameStateDeciding,
    //	GameStateGameOver




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
                recordedTime = ofGetElapsedTimeMillis() - turnTime;
                ((testApp*) ofGetAppPtr())->vibrate(true);

                if (isClient) {
                    sendMessage(ofToString(recordedTime));
                }

            }

            break;


        case GameStateDeciding:

            if (isServer) {

                pickedAgent();

            }
            else if (isClient){
                sendMessage("pickedAgent");
            }



            break;


        default:
            break;


    }







	//sendMessage("touch down");



    //	ofxOscMessage m;
    //    m.setAddress( "/mouse/button" );
    //    m.addStringArg( "down" );
    //    this->sender.sendMessage( m );



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
//
//	((testApp*) ofGetAppPtr())->vibrate(false);
//

    //    ofxOscMessage m;
    //    m.setAddress( "/mouse/button" );
    //    m.addStringArg( "up" );
    //    sender.sendMessage( m );

}

#pragma mark - SYSTEM


void agentController::setIpAddress(const char* ipAddress){

    if (ipAddress[0] != 0){

		localIP = std::string(ipAddress);

        ofLogNotice("OSC") << "OF LOCAL IP ADDRESS:" + localIP + " on port:" << ofToString(PORT);

        // receiver.setup( PORT );

        char broadcast[16];

        std::vector<std::string> result;

        result = ofSplitString(std::string(ipAddress),".");

        const char* last = result[3].c_str();

        if (last[0] == '1'){
        	isServer = true;
        	server.setup(PORT);
        	ofLogNotice("TCP") << "IS SERVER AT:" + localIP + " on port:" << ofToString(PORT);
        }

        //        int index = 0;
        //
        //        for (int i = 0; i < 3; i++){
        //            for (int j = 0; j < result[i].length(); j++){
        //                broadcast[index] = ipAddress[index];
        //                index++;
        //            }
        //
        //            broadcast[index] = '.';
        //            index++;
        //
        //        }
        //
        //        char b[4] = {'2','5','5','\0'};
        //
        //        for (int i = 0; i < 4; i++){
        //            broadcast[index] = b[i];
        //            index++;
        //        }
        //
        //        broadcastIP = std::string(broadcast);
        //
        //        // ofLogNotice("OSC") << "BROADCAST IP ADDRESS: " << broadcast ;
        //
        //        ofLogNotice("OSC") << "init host broadcast at " + broadcastIP + " port: " << ofToString(PORT) << "\n";
        //
        //        sender.setup(broadcast, PORT );

        //sender.setup("10.1.10.23", 1234);
        // this->sender.setup("192.168.1.140", 1234);


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
            }
        }



	}
	else {
		ofLogNotice("OSC") << "Didn't receive IP address from ANDROID ENV";
	}

}



void agentController::pause(){
	if (isServer){
        ofLogNotice("TCP") << "Shutting down Server";
        server.close();

	}
	if (isClient){
		ofLogNotice("TCP") << "Shutting down Client";
		client.close();
	}

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


}
