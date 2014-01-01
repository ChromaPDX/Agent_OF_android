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
    
	font.loadFont("verdana.ttf", 200, true, true);
	font.setLineHeight(18.0f);
	font.setLetterSpacing(1.037);

    elapsed = ofGetElapsedTimeMillis();
    state = GameStateWaitingForSignIn;


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

void agentController::updateTCP() {

	if (isServer){
	    for(int i = 0; i < server.getLastID(); i++) // getLastID is UID of all clients
	    {

            if( server.isClientConnected(i) ) { // check and see if it's still around
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

    else {

        string str = client.receive();

        if (str.length()){

	    	ofLogNotice("TCP") << "Received From Server: " + str;

            strcpy( mouseButtonState, str.c_str() );


            if (strcmp(mouseButtonState, "startGame") == 0) {
                startGame();
            }



        }

    }

}



void agentController::update() {

	updateTCP();


	if (stepInterval){

		if (ofGetElapsedTimeMillis() - stepTimer > stepInterval ){

            if (step > 0){

                if (stepFunction != NULL) {

                    (this->*stepFunction)(step);

                }


            }



            if (step > numSteps){
                stepInterval = 0;

            }

            stepTimer = ofGetElapsedTimeMillis();
            step++;


        }

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

            break;

        default:
            break;
    }


}

void agentController::draw() {
    

    float alpha = (float)0.8;

    filteredAccel.x = alpha * filteredAccel.x + (1 - alpha) * normAccel.x;
    filteredAccel.y = alpha * filteredAccel.y + (1 - alpha) * normAccel.y;
    filteredAccel.z = alpha * filteredAccel.z + (1 - alpha) * normAccel.z;



    ofClear(0, 0, 0);

    //	ofSetColor(255,255,0);
    //    font.drawString(messages[0],10,font.stringHeight(messages[0])+20);
    //    ofSetColor(255,0,255);
    //    font.drawString(messages[1],10,(font.stringHeight(messages[0])+20)*2);
    //    ofSetColor(0,255,255);
    //    font.drawString(messages[2],10,(font.stringHeight(messages[0])+20)*3);

    ofPushMatrix();
    ofTranslate(ofGetWidth()*.5, ofGetHeight()*.5);

    ofSetColor(200,200,200);
    ofLine(0,0,filteredAccel.x*ofGetWidth()/2,0);
    ofLine(0,0,0,-filteredAccel.y*ofGetHeight()/2);

    for (int i = 0; i < 8; i++){

        ofSetColor(100,100,100 + (i*20));

        ofLine(0,0,filteredAccel.x*ofGetWidth()/(8. - i),filteredAccel.x*ofGetWidth()/(float)i);

        ofLine(0,0,filteredAccel.y*ofGetWidth()/(8. - i),-filteredAccel.y*ofGetWidth()/(float)i);
        //ofLine(0,0,normAccel.y*ofGetHeight()/2,-normAccel.y*ofGetHeight()/3);

    }

    //ofLine(0,0,0,-normAccel.y*ofGetHeight()/3);

    // we don't draw z as the perspective might be confusing
    // but it's approximately one when the device is still and parallel
    // to the ground
    ofPopMatrix();


    if (mainMessage.length()){
        ofSetColor(255,255,255);
        font.drawString(mainMessage,ofGetWidth()/2 - font.stringWidth(mainMessage)/2.,ofGetHeight()/2 + font.stringHeight(mainMessage)/2.);
    }



    char buf[256];
    sprintf( buf, "listening for osc messages on port %d", PORT );
    ofDrawBitmapString( buf, 10, 20 );

    // draw mouse state
    sprintf( buf, "mouse: % 4d % 4d", mouseX, mouseY );
    ofDrawBitmapString( buf, 0, 220 );
    ofDrawBitmapString( mouseButtonState, 0, 320 );


    for ( int i=0; i<NUM_MSG_STRINGS; i++ )
    {
        ofDrawBitmapString( (char*)msg_strings[i].c_str(), 10, 40+15*i );
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
    stepInterval = 1000;
    numSteps = 3;

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

	sendMessage("touch up");

	((testApp*) ofGetAppPtr())->vibrate(false);


    //    ofxOscMessage m;
    //    m.setAddress( "/mouse/button" );
    //    m.addStringArg( "up" );
    //    sender.sendMessage( m );

}

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
