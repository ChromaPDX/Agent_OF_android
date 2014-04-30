//
//  AgentView.cpp
//  DoubleAgent
//
//  Created by Robby on 4/11/14.
//
//

#include "AgentView.h"
#include "agentController.h"

void AgentView::setup(){
    width = ofGetWidth();
    height = ofGetHeight();
    centerX = ofGetWidth()/2.;
    centerY = ofGetHeight()/2.;
    // AvenirNextCondensed.ttf
    font.loadFont("Brandon_blk.ttf", ofGetWidth() / 12., true, true);   // 4.2
    fontTiny.loadFont("Brandon_blk.ttf", ofGetWidth() / 24., true, true);
    fontSmall.loadFont("Brandon_blk.ttf", ofGetWidth() / 16., true, true);
    fontMedium.loadFont("Brandon_blk.ttf", ofGetWidth() / 12., true, true);
    fontLarge.loadFont("Brandon_blk.ttf", ofGetWidth() / 6., true, true);
    font.setLetterSpacing(.9);
    fontSmall.setLetterSpacing(.9);
    fontMedium.setLetterSpacing(.9);
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
    wifiImage.loadImage("wifi.png");
    wifiImage.setAnchorPercent(.5, .5);
    circleWhite.loadImage("circleWhite.png");
    circleWhite.setAnchorPercent(.5, .5);
    circleBlack.loadImage("circleBlack.png");
    circleBlack.setAnchorPercent(.5, .5);
    circleShadow.loadImage("circleShadow.png");
    circleShadow.setAnchorPercent(.5, .5);
    
    lightShadow.loadImage("lightShadow.png");
    lightShadow.setAnchorPercent(0.5, 0.5);
    
    avatarCoords[0*2+0] = centerX;      avatarCoords[0*2+1] = height*.75;
    avatarCoords[1*2+0] = centerX;      avatarCoords[1*2+1] = height*.5;
    avatarCoords[2*2+0] = width*.25;    avatarCoords[2*2+1] = height*.625;
    avatarCoords[3*2+0] = width*.75;    avatarCoords[3*2+1] = height*.625;
    avatarCoords[4*2+0] = width*.25;    avatarCoords[4*2+1] = height*.375;
    avatarCoords[5*2+0] = width*.75;    avatarCoords[5*2+1] = height*.375;
    avatarCoords[6*2+0] = centerX;      avatarCoords[6*2+1] = height*.25;
    avatarCoords[7*2+0] = width*.75;    avatarCoords[7*2+1] = height*.2;
    avatarCoords[8*2+0] = width*.25;    avatarCoords[8*2+1] = height*.2;
    
    spymess[0] = rand()%23+65;
    spymess[1] = rand()%23+65;
    spymess[2] = rand()%23+65;
    spymess[3] = rand()%23+65;
    spymess[4] = NULL;
    
    primaries[0] = ofColor(255, 0, 210);    // pink  0
    primaries[1] = ofColor(17, 188, 61);    // green  1
    primaries[2] = ofColor(178, 44, 255);   // purple  2
    primaries[3] = ofColor(255, 96, 0);     // orange   3
    primaries[4] = ofColor(255, 0, 0);      // red       4
    primaries[5] = ofColor(255, 174, 0);    // yellow     5
    primaries[6] = ofColor(44, 138, 255);   // light blue  6
    
    primaryColor = ofRandom(0, 7);
    complementaries[0*3+0] = 1;     complementaries[0*3+1] = 5;     complementaries[0*3+2] = 6;
    complementaries[1*3+0] = 0;     complementaries[1*3+1] = 3;     complementaries[1*3+2] = 5;
    complementaries[2*3+0] = 0;     complementaries[2*3+1] = 3;     complementaries[2*3+2] = 5;
    complementaries[3*3+0] = 6;     complementaries[3*3+1] = 2;     complementaries[3*3+2] = 5;
    complementaries[4*3+0] = 6;     complementaries[4*3+1] = 2;     complementaries[4*3+2] = 5;
    complementaries[5*3+0] = 2;     complementaries[5*3+1] = 0;     complementaries[5*3+2] = 6;
    complementaries[6*3+0] = 0;     complementaries[6*3+1] = 5;     complementaries[6*3+2] = 2;

    avatars[1].loadImage("ape.png");
    avatars[2].loadImage("bear.png");
    avatars[3].loadImage("eagle.png");
    avatars[4].loadImage("horse.png");
    avatars[5].loadImage("lion.png");
    avatars[6].loadImage("moth.png");
    avatars[7].loadImage("owl.png");
    avatars[8].loadImage("snake.png");
    avatars[1].setAnchorPercent(.5f, .5f);
    avatars[2].setAnchorPercent(.5f, .5f);
    avatars[3].setAnchorPercent(.5f, .5f);
    avatars[4].setAnchorPercent(.5f, .5f);
    avatars[5].setAnchorPercent(.5f, .5f);
    avatars[6].setAnchorPercent(.5f, .5f);
    avatars[7].setAnchorPercent(.5f, .5f);
    avatars[8].setAnchorPercent(.5f, .5f);
}
void AgentView::setWIFIExist(bool w){
    WIFIExist = w;
}
void AgentView::setIsServer(bool s){
    isServer = s;
}
void AgentView::setIsSpy(bool s){
    isSpy = s;
}

void AgentView::draw(ProgramState state, NetworkState networkState, long elapsedMillis, long stateBeginTime, bool transitionActive, long transitionDuration, long transitionEndTime, bool errorMessageActive, string errorMessage, long errorBeginTime){
    
    ofClear(primaries[primaryColor]);
    
    // from 1 to 0
    if(transitionActive)
        transition = (transitionEndTime-elapsedMillis)/(float)transitionDuration;
    else
        transition = 1.0;
    
    // network states
//        NetworkNone,
//        NetworkHostAttempt,
//        NetworkHostSuccess,
//        NetworkJoinAttempt,
//        NetworkJoinSuccess,
//        NetworkLostConnection,      // try to make these 2 into 1
//        NetworkServerDisconnected   //
    
    if(state == StateWelcomeScreen){
        ofSetColor(255, 255);
        lightShadow.draw(centerX, centerY);
        fontLarge.drawString("DOUBLE", centerX - fontLarge.stringWidth("DOUBLE")/2., height*.2);
        fontLarge.drawString("AGENT",  centerX - fontLarge.stringWidth("AGENT")/2., height*.2 + fontLarge.stringHeight("AGENT")*1.25);
        fontTiny.drawString("A GAME OF", centerX - fontTiny.stringWidth("A GAME OF")/2., height*.45);
        fontTiny.drawString("RIDICULOUS GESTURES", centerX - fontTiny.stringWidth("RIDICULOUS GESTURES")*.5, height*.45 + fontTiny.stringHeight("AGENT")*1.5);

        fontSmall.drawString("BEGIN", centerX - fontSmall.stringWidth("BEGIN")/2.-2, height*.77 + fontSmall.stringHeight("BEGIN")*.5);

        ofDrawPlane(width*.5, height-50, width, 100);
        ofSetColor(0, 255);
        string wifistr;
        if(WIFIExist) wifistr = "WIFI CONNECTED";
        else wifistr = "I NEED WIFI";
        fontTiny.drawString(wifistr, centerX - fontTiny.stringWidth(wifistr)*.5, height - 50 + fontTiny.stringHeight(wifistr)*.5);
        if(!WIFIExist)
            ofSetColor(0, 66);
        wifiImage.draw(centerX - fontTiny.stringWidth(wifistr)*.5 - 50, height - 50, 50, 50);

    }
    else if(state == StateConnectionScreen){
        int alpha = 255;
        if(transitionActive){
            alpha = 255*transition;
        }
        ofSetColor(255, alpha);
        lightShadow.draw(centerX, centerY);
        fontLarge.drawString("DOUBLE", centerX - fontLarge.stringWidth("DOUBLE")/2., height*.2);
        fontLarge.drawString("AGENT",  centerX - fontLarge.stringWidth("AGENT")/2., height*.2 + fontLarge.stringHeight("AGENT")*1.25);
        font.drawString("HOST", width*.3 - font.stringWidth("HOST")/2., height*.6 - font.stringHeight("HOST")/2.);
        font.drawString("JOIN", width*.7 - font.stringWidth("JOIN")/2., height*.6 - font.stringHeight("JOIN")/2.);
       
        ofSetColor(255, 255);
        circleShadow.draw(centerX, height*.77, 235, 235);
        ofSetColor(primaries[primaryColor]);
        circleWhite.draw(centerX, height*.77, 200, 200);
        ofSetColor(255, 255);
        avatars[controller->avatarSelf].draw(centerX, height*.77, 150, 150);

        float yPos = height-50;
        if(transitionActive){
            float speed = 7; // speed to progress through animation curve
            float time = (transition) * speed;
            float curve = cosf(time-PI)/(9*powf(2, time-PI)) + 1;
            yPos += (1-curve)*150;
        }
        
        ofSetColor(255, 255);
        ofDrawPlane(width*.5, yPos, width, 100);
        ofSetColor(0,255);
        fontTiny.drawString("ONLY ONE HOST IS NEEDED", width*.5 - fontTiny.stringWidth("ONLY ONE HOST IS NEEDED")/2., yPos + fontTiny.stringHeight("ONLY ONE HOST IS NEEDED")*.5);
    }
    else if(state == StateJoinScreen){
        
//        avatars[controller->avatarSelf].draw(centerX, height*.9, 100, 100);
        
//        if(networkState == NetworkJoinAttempt){

        const int NUDGE = 5;  // ofStringWidth()*.5 is still a little off from center
        
        string clientString = controller->getCodeFromInt(controller->loginCode);
        string digit1 = clientString.substr(0,1);
        string digit2 = clientString.substr(2,1);
        string digit3 = clientString.substr(4,1);
        ofSetColor(255, 255);
        fontLarge.drawString("JOIN",  centerX - fontLarge.stringWidth("JOIN")*.5, height*.66 + fontLarge.stringHeight("JOIN")*.5);
        fontMedium.drawString("QUIT", fontMedium.stringWidth("QUIT")*.15, height - fontMedium.stringHeight("QUIT")*.5);
        fontLarge.drawString(digit1, width*.25 - fontLarge.stringWidth(digit1)*.5 - NUDGE, height*.42 - fontLarge.stringHeight(digit1)*.5);
        fontLarge.drawString(digit2, width*.5 - fontLarge.stringWidth(digit2)*.5 - NUDGE, height*.42 - fontLarge.stringHeight(digit2)*.5);
        fontLarge.drawString(digit3, width*.75 - fontLarge.stringWidth(digit3)*.5 - NUDGE, height*.42 - fontLarge.stringHeight(digit3)*.5);
        increment.draw(width*.25, height*.21, width*.2, width*.2);
        increment.draw(width*.5, height*.21, width*.2, width*.2);
        increment.draw(width*.75, height*.21, width*.2, width*.2);
        decrement.draw(width*.25, height*.45, width*.2, width*.2);
        decrement.draw(width*.5, height*.45, width*.2, width*.2);
        decrement.draw(width*.75, height*.45, width*.2, width*.2);
    }
    else if(state == StateReadyRoom){
    
        ofSetColor(255, 70);
        wifiImage.draw(centerX, centerY, height, height);

        ofSetColor(255, 255);

        int count = 0;   // increment through activeConnections
        if(isServer){
            ofSetColor(255, 255);
            circleShadow.draw(centerX, height*.77, 235, 235);
            ofSetColor(primaries[primaryColor]);
            circleWhite.draw(centerX, height*.77, 200, 200);
            ofSetColor(255, 255);
            avatars[controller->avatarSelf].draw(centerX, height*.77, 150, 150);
            count++;
        }
        for(int i = 0; i < 256; i++){
            if(controller->avatarIcons[i] != 0){
                float posX, posY;
                if(count < NUM_AVATAR_COORDS){
                    posX = avatarCoords[count*2+0];
                    posY = avatarCoords[count*2+1];
                }
                else{
                    posX = ofRandom(width*.2, width*.8);
                    posY = ofRandom(height*.2, height*.8);
                }
                ofSetColor(255, 255);
                circleShadow.draw(posX, posY, 235, 235);
                ofSetColor(primaries[controller->avatarColors[i]-1]);
                circleWhite.draw(posX, posY, 200, 200);
                ofSetColor(255, 255);
                avatars[controller->avatarIcons[i]].draw(posX, posY, 150, 150);
                count++;
            }
        }
        if (isServer){
            ofSetColor(255);
            
            string clientString = controller->getCodeFromIp();
            fontTiny.drawString("JOIN CODE", width*.5 - fontTiny.stringWidth("JOIN CODE")*.5, fontTiny.stringHeight("JOIN CODE")*1.5);
            fontLarge.drawString(clientString, width*.5 - fontLarge.stringWidth(clientString)*.5, fontLarge.stringHeight(clientString)*1.75);
            
            fontMedium.drawString("START", width - fontMedium.stringWidth("START")*1.1, height - fontMedium.stringHeight("START")*.5);
        }
        
        fontMedium.drawString("LEAVE", fontMedium.stringWidth("LEAVE")*.1, height - fontMedium.stringHeight("LEAVE")*.5);

        if(transitionActive){
            int alpha = 255*(1.0-transition);
            ofSetColor(0, alpha);
            ofRect(0, 0, width, height);
        }
    }
    else if(state == StateStartGame){
        ofClear(0, 255);
        ofSetColor(255, 255);
        if(isSpy){
            if(elapsedMillis > stateBeginTime + 1000){
                font.drawString("shhh", centerX-font.stringWidth("shhh")/2., centerY);
            }
            if(elapsedMillis > stateBeginTime + 2000){
                fontSmall.drawString("you are the double agent",centerX - fontSmall.stringWidth("you are the double agent")/2., centerY+font.stringHeight("you are the double agent"));
            }
        }
        else{
            if(elapsedMillis > stateBeginTime + 750){
                font.drawString("shhh", centerX-font.stringWidth("shhh")/2., centerY-font.stringHeight("shhh"));
            }
            if(elapsedMillis > stateBeginTime + 1500){
                fontSmall.drawString("(relax)", centerX-fontSmall.stringWidth("(relax)")/2., centerY);
            }
            if(elapsedMillis > stateBeginTime + 2250){
                fontSmall.drawString("you are NOT the double agent", centerX - fontSmall.stringWidth("you are NOT the double agent")/2., centerY+font.stringHeight("you are NOT the double agent"));
            }
        }
    }
    else if(state == StateCountdown){

        if(elapsedMillis < stateBeginTime + 2000){
            float fade = (elapsedMillis-stateBeginTime-1000)/1000.0;
            if(fade < 0) fade = 0;
            ofSetColor(0, 255-fade*255);
            ofRect(0, 0, width, height);
        }
        
        float diameter = width*.75;
        if(elapsedMillis < stateBeginTime + 500){
            float speed = 7; // speed to progress through animation curve
            float time = (elapsedMillis-stateBeginTime)/500.0 * speed;
            float curve = cosf(time-PI)/(9*powf(2, time-PI)) + 1;
            diameter *= curve;
        }

        ofSetColor(255, 255);
        circleShadow.draw(centerX, centerY, diameter, diameter);
        ofSetColor(primaries[primaryColor]);
        circleWhite.draw(centerX, centerY, diameter*.83, diameter*.83);
        ofSetColor(255, 255);
        avatars[controller->avatarSelf].draw(centerX, centerY, diameter*.625, diameter*.625);

        ofSetColor(255, 255);
        string countdownString;
        if(elapsedMillis > stateBeginTime + 7000){
            countdownString = "";
        }
        else if(elapsedMillis > stateBeginTime + 6000){
            countdownString = "1";
            float fader = (elapsedMillis - (stateBeginTime+6000)) / 1000.0;
            ofSetColor(255, (1-fader) * 255);
        }
        else if(elapsedMillis > stateBeginTime + 5000){
            countdownString = "2";
            float fader = (elapsedMillis - (stateBeginTime+5000)) / 1000.0;
            ofSetColor(255, (1-fader) * 255);
        }
        else if(elapsedMillis > stateBeginTime + 4000){
            countdownString = "3";
            float fader = (elapsedMillis - (stateBeginTime+4000)) / 1000.0;
            ofSetColor(255, (1-fader) * 255);
        }
        else {
            countdownString = "";
        }
        fontLarge.drawString(countdownString, centerX-fontLarge.stringWidth(countdownString)*.5, height*.83 + fontLarge.stringHeight("3")*.75);

        if(elapsedMillis < stateBeginTime + 6000){
            float fader = (elapsedMillis - (stateBeginTime+5000)) / 1000.0;
            if(fader > 0)
                ofSetColor(255, (1-fader) * 255);
            else
                ofSetColor(255, 255);
            if(elapsedMillis > stateBeginTime + 2000){
                fontMedium.drawString("REMEMBER", centerX - fontMedium.stringWidth("REMEMBER")*.5, height*.15);
            }
            if(elapsedMillis > stateBeginTime + 3000){
                fontTiny.drawString("WATCH YOUR AGENTS CLOSELY", centerX - fontTiny.stringWidth("WATCH YOUR AGENTS CLOSELY") * .5, height * .2);
            }
        }
    }
    else if(state == StateTurnScramble){
        static int count = 0;
        count++;
        if(count >= 7) count = 0;
        font.drawString(scrambleStrings[count], centerX-font.stringWidth(scrambleStrings[count])*.5, height*.83+font.stringHeight(scrambleStrings[count])*.5);
    }
    else if(state == StateTurnGesture){

    }
    else if(state == StateTurnComplete){

    }
    else if(state == StateDecide){
        
        // not an elegant solution
        spin = 0.0;
        spinSpeed = 0.0;
        // not an elegant solution
        
        float yPos = height-50;
        if(elapsedMillis < stateBeginTime + 500){
            float speed = 7; // speed to progress through animation curve
            float time = (elapsedMillis-stateBeginTime)/500.0 * speed;
            float curve = cosf(time-PI)/(9*powf(2, time-PI)) + 1;
            yPos += (1-curve)*150;
        }
        ofSetColor(255, 255);
        font.drawString("WHO IS IT?", centerX-font.stringWidth("WHO IS IT?")*.5, height*.2-font.stringHeight("WHO IS IT?")*.5);
        ofDrawPlane(width*.5, yPos, width, 100);
        ofSetColor(0, 255);
        fontTiny.drawString("TURN SCREEN TOWARD PLAYERS", centerX - fontTiny.stringWidth("TURN SCREEN TOWARD PLAYERS")*.5, yPos + fontTiny.stringHeight("TURN SCREEN TOWARD PLAYERS")*.5);
    }
    else if(state == StateGameOver){
        if (controller->mainMessage.compare("WIN") == 0) {

            float diameter = width*.75;
            if(elapsedMillis < stateBeginTime + 500){
                float speed = 7; // speed to progress through animation curve
                float time = (elapsedMillis-stateBeginTime)/500.0 * speed;
                float curve = cosf(time-PI)/(9*powf(2, time-PI)) + 1;
                diameter *= curve;
                
                float shrinking = width*.75 * (1 - (elapsedMillis-stateBeginTime)/250.0);
                if(shrinking < 1) shrinking = 1;
                ofSetColor(255, 255);
                circleShadow.draw(centerX, centerY, shrinking, shrinking);
                ofSetColor(primaries[primaryColor]);
                circleWhite.draw(centerX, centerY, shrinking*.83, shrinking*.83);
                ofSetColor(255, 255);
                avatars[controller->avatarSelf].draw(centerX, centerY, shrinking*.625, shrinking*.625);

            }
            ofSetColor(255, 255);
            circleShadow.draw(centerX, centerY, diameter, diameter);
            ofSetColor(primaries[controller->spyColor-1]);
            circleWhite.draw(centerX, centerY, diameter*.83, diameter*.83);
            ofSetColor(255, 255);
            avatars[controller->spyAvatar].draw(centerX, centerY, diameter*.625, diameter*.625);
        
            font.drawString("GOT 'EM", centerX-font.stringWidth("GOT 'EM")*.5, height*.83-font.stringHeight("GOT 'EM")*.5);

        }
        else if (controller->mainMessage.compare("CAPTURED") == 0) {  // same as WIN, but this means you were the double agent
            font.drawString("CAPTURED", centerX-font.stringWidth("CAPTURED")*.5, height*.83-font.stringHeight("CAPTURED")*.5);
            
            float diameter = width*.75;
            ofSetColor(255, 255);
            circleShadow.draw(centerX, centerY, diameter, diameter);
            ofSetColor(primaries[primaryColor]);
            circleWhite.draw(centerX, centerY, diameter*.83, diameter*.83);
            ofSetColor(255, 255);
            avatars[controller->avatarSelf].draw(centerX, centerY, diameter*.625, diameter*.625);

        }
        else if (controller->mainMessage.compare("LOSE") == 0) {
            font.drawString("THEY GOT AWAY", centerX-font.stringWidth("THEY GOT AWAY")*.5, height*.83-font.stringHeight("THEY GOT AWAY")*.5);
            
            float iconScale = 1.0;
            if(elapsedMillis-stateBeginTime < 750){
                if(elapsedMillis - stateBeginTime > 500 && elapsedMillis - stateBeginTime < 750){
                    iconScale = 1 - (elapsedMillis-stateBeginTime-500)/250.0;
                }
                if(spinSpeed < 30)
                    spinSpeed +=.33;
                spin += spinSpeed;
                ofPushMatrix();
                ofTranslate(centerX, centerY);
                ofRotate(spin, 0, 0, 1);
                ofScale(iconScale, iconScale);
                float diameter = width*.75;
                ofSetColor(255, 255);
                circleShadow.draw(0, 0, diameter, diameter);
                ofSetColor(primaries[primaryColor]);
                circleWhite.draw(0, 0, diameter*.83, diameter*.83);
                ofSetColor(255, 255);
                avatars[controller->avatarSelf].draw(0, 0, diameter*.625, diameter*.625);
                ofPopMatrix();
            }
            else{
                spin = 1.0;
            }
        }
    }
    
    if(state == StateTurnComplete || state == StateTurnGesture || state == StateTurnScramble || state == StateDecide){
        int i1 = controller->turnNumber - 1;
        if(i1 < 0) i1 = 0;
        if(i1 >= NUM_TURNS) i1 = NUM_TURNS-1;
        
        // bezier curves are complicated, and this code is messy, sorry!
        for(int t = 0; t <= i1; t++){
            ofSetColor(primaries[complementaries[primaryColor*3+(t)]]);
            ofFill();
            ofBeginShape();
            float innerRadius = width*.3;//1125;
            float outerRadius = width*.18;
            float deltaAngle = TWO_PI / (float)SENSOR_DATA_ARRAY_SIZE;
            float angle = 0;
            
            float max = 3;
            for(int i = 0; i < SENSOR_DATA_ARRAY_SIZE; i++){
                if(controller->recordedSensorData[t][i] > max)
                    max = controller->recordedSensorData[t][i];
            }
            float x = centerX + innerRadius * sin(angle-deltaAngle);
            float y = centerY + innerRadius * -cos(angle-deltaAngle);
            float presentMagnitude, previousMagnitude = 0;  // to make a bezier vertex, you need to bulid the previous point's control point's magnitude
            bool notTheLastOne = 1;
            ofVertex(x, y);
            for(int i = 0; i < SENSOR_DATA_ARRAY_SIZE; i++){
                if(i == SENSOR_DATA_ARRAY_SIZE - 1) notTheLastOne = 0;  // forces the last entry back to the circle to prevent an overhanging edge
                presentMagnitude = controller->recordedSensorData[t][i] / max * notTheLastOne;
                x = centerX + innerRadius * sin(angle)  + outerRadius * sin(angle) * presentMagnitude;
                y = centerY + innerRadius * -cos(angle) + outerRadius * -cos(angle) * presentMagnitude;
//                ofVertex(x,y);
                ofBezierVertex(centerX + innerRadius * sin(angle-.6666*deltaAngle) + outerRadius * sin(angle-.6666*deltaAngle) * previousMagnitude,
                               centerY + innerRadius * -cos(angle-.6666*deltaAngle) + outerRadius * -cos(angle-.6666*deltaAngle) * previousMagnitude,
                               centerX + innerRadius * sin(angle-.3333*deltaAngle) + outerRadius * sin(angle-.3333*deltaAngle) * presentMagnitude,
                               centerY + innerRadius * -cos(angle-.3333*deltaAngle) + outerRadius * -cos(angle-.3333*deltaAngle) * presentMagnitude,
                               x,
                               y);
                previousMagnitude = presentMagnitude;
                angle += deltaAngle;
            }
            
            ofEndShape();
        }
    }

    // CLOCK DIAL
    // AND GESTURE TEXT
    if(state == StateTurnScramble){
        float length = width*.425;
        if(elapsedMillis < stateBeginTime + 250){
            float time = (elapsedMillis-stateBeginTime)/250.0 * 7;
            float curve = cosf(time-PI)/(9*powf(2, time-PI)) + 1;
            length *= curve;
        }
        ofSetLineWidth(20);
        ofSetColor(255, 255);
        ofLine(centerX, centerY, centerX, centerY-length);
        ofSetLineWidth(0);
        ofCircle(centerX, centerY-length, 7);
    }
    else if(state == StateTurnGesture){
        float turnTime = PI*2*(elapsedMillis-stateBeginTime)/(float)ACTION_TIME;
        ofSetLineWidth(20);
        ofSetColor(255, 255);
        float endX = centerX+sinf(turnTime)*width*.425;
        float endY = centerY-cosf(turnTime)*width*.425;
        ofLine(centerX, centerY, endX, endY);
        ofSetLineWidth(0);
        ofCircle(endX, endY, 7);
        
        // GESTURE TEXT
        float lineHeight = font.stringHeight(controller->mainMessage)*.75;
        if(controller->mainMessage.find('\n') != -1)
            lineHeight = 0;
        if(!controller->isSpy)
            font.drawString(controller->mainMessage, centerX-font.stringWidth(controller->mainMessage)*.5, height*.83+lineHeight);

    }
    else if(state == StateTurnComplete) {
        float length = width*.425;
        if(elapsedMillis < stateBeginTime + 250){
            float time = 7 - ((elapsedMillis-stateBeginTime)/250.0 * 7);
            float curve = cosf(time-PI)/(9*powf(2, time-PI)) + 1;
            length *= curve;

            ofSetLineWidth(20);
            ofSetColor(255, 255);
            ofLine(centerX, centerY, centerX, centerY-length);
            ofSetLineWidth(0);
            ofCircle(centerX, centerY-length, 7);
        }
    }
    // END CLOCK DIAL

    if(state == StateTurnScramble || state == StateTurnGesture || state == StateTurnComplete || state == StateDecide){
        
        float diameter = width*.75;
        ofSetColor(255, 255);
        circleShadow.draw(centerX, centerY, diameter, diameter);
        ofSetColor(primaries[primaryColor]);
        circleWhite.draw(centerX, centerY, diameter*.83, diameter*.83);  // .6225
        ofSetColor(255, 255);
        avatars[controller->avatarSelf].draw(centerX, centerY, diameter*.625, diameter*.625);
    }
    
    
    // ERROR MESSAGE DELIVERY
    
    if(errorMessageActive){
        float fade = (elapsedMillis - (errorBeginTime + 1500))/(ERROR_MESSAGE_DURATION*.25);
        if(fade < 0) fade = 0;
        fade = 1-fade;
        ofSetColor(0, 80*fade);
        ofRect(0, 0, width, height);
        ofSetColor(255, 255*fade);
        ofRect(centerX-width*.475, centerY-width*.15, width*.95, width*.3);
        ofSetColor(255, 0, 0, 255*fade);
        fontSmall.drawString(errorMessage, centerX-fontSmall.stringWidth(errorMessage)*.5, centerY+fontSmall.stringHeight(errorMessage)*.5);
    }
}
