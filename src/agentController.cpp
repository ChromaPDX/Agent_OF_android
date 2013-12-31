//
//  agentController.cpp
//  DoubleAgent
//
//  Created by Chroma Developer on 12/30/13.
//
//

#include "agentController.h"


void agentController::setup() {
    

}

void agentController::update() {

	// hide old messages
    for ( int i=0; i<NUM_MSG_STRINGS; i++ )
    {
        if ( timers[i] < ofGetElapsedTimef() )
            msg_strings[i] = "";
    }

    // check for waiting messages
    while( receiver.hasWaitingMessages() )
    {
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage( &m );

        // check for mouse moved message
        if ( strcmp(m.getAddress().c_str(), "/mouse/position" ) == 0 )
        {
            // both the arguments are int32's
            mouseX = m.getArgAsInt32( 0 );
            mouseY = m.getArgAsInt32( 1 );
        }
        // check for mouse button message
        else if ( strcmp(m.getAddress().c_str(), "/mouse/button" ) == 0 )
        {
            // the single argument is a string
            strcpy( mouseButtonState, m.getArgAsString( 0 ).c_str() );
        }
        else
        {
            // unrecognized message: display on the bottom of the screen
            char msg_string[16384];
            strcpy( msg_string, m.getAddress().c_str() );
            strcat( msg_string, ": " );
            for ( int i=0; i<m.getNumArgs(); i++ )
            {
                // get the argument type
                strcat( msg_string, m.getArgTypeName( i ).c_str() );
                strcat( msg_string, ":" );
                // display the argument - make sure we get the right type
                if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
                    sprintf( msg_string, "%s%d ", msg_string,m.getArgAsInt32( i ) );
                else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
                    sprintf( msg_string, "%s%f ", msg_string, m.getArgAsFloat( i ) );
                else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
                    sprintf( msg_string, "%s\"%s\" ", msg_string, m.getArgAsString( i ).c_str() );
                else
                    strcat( msg_string, "unknown" );
            }
        }

        m.clear();

    }

}

void agentController::draw() {
    
    ofClear(0, 0, 0);

	ofSetColor(255,255,0);
    font.drawString(messages[0],10,font.stringHeight(messages[0])+20);
    ofSetColor(255,0,255);
    font.drawString(messages[1],10,(font.stringHeight(messages[0])+20)*2);
    ofSetColor(0,255,255);
    font.drawString(messages[2],10,(font.stringHeight(messages[0])+20)*3);

    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);

    ofSetColor(255,255,0);
    ofLine(0,0,normAccel.x*ofGetWidth()/2,0);
    ofSetColor(255,0,255);
    ofLine(0,0,0,-normAccel.y*ofGetHeight()/2);
    // we don't draw z as the perspective might be confusing
    // but it's approximately one when the device is still and parallel
    // to the ground
    ofPopMatrix();

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

//--------------------------------------------------------------
void agentController::touchBegan(int x, int y, int id){

	ofLogNotice("UI") << "Touch Down";

	ofxOscMessage m;
    m.setAddress( "/mouse/button" );
    m.addStringArg( "down" );
    this->sender.sendMessage( m );

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

    ofLogNotice("UI") << "Touch Up";

    ofxOscMessage m;
    m.setAddress( "/mouse/button" );
    m.addStringArg( "up" );
    sender.sendMessage( m );

}

void agentController::setIpAddress(const char* ipAddress){

    if (ipAddress[0] != 0){

		localIP = std::string(ipAddress);

        ofLogNotice("OSC") << "OF LOCAL IP ADDRESS:" + localIP + " on port:" << ofToString(PORT);

        receiver.setup( PORT );


        char broadcast[16];

        std::vector<std::string> result;

        result = ofSplitString(std::string(ipAddress),".");

        int index = 0;

        for (int i = 0; i < 3; i++){
            for (int j = 0; j < result[i].length(); j++){
                broadcast[index] = ipAddress[index];
                index++;
            }

            broadcast[index] = '.';
            index++;

        }

        char b[4] = {'2','5','5','\0'};

        for (int i = 0; i < 4; i++){
            broadcast[index] = b[i];
            index++;
        }

        broadcastIP = std::string(broadcast);

        // ofLogNotice("OSC") << "BROADCAST IP ADDRESS: " << broadcast ;

        ofLogNotice("OSC") << "init host broadcast at " + broadcastIP + " port: " << ofToString(PORT) << "\n";

        sender.setup(broadcast, PORT );

        //sender.setup("10.1.10.23", 1234);
        // this->sender.setup("192.168.1.140", 1234);


        // open an outgoing connection to HOST:PORT




	}
	else {
		ofLogNotice("OSC") << "Didn't receive IP address from ANDROID ENV";
	}

}

void agentController::exit() {
    
}
