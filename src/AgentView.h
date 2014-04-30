//
//  AgentView.h
//  DoubleAgent
//
//  Created by Robby on 4/11/14.
//
//

#ifndef __DoubleAgent__AgentView__
#define __DoubleAgent__AgentView__

#include <iostream>
#include "ofMain.h"

#include "AgentCommon.h"

#define NUM_AVATAR_COORDS 9

class agentController;

class AgentView{
    
public:
    
    agentController *controller;  // delegate

    void    setup(); // must be called
    void    draw(ProgramState state, NetworkState networkState, long elapsedMillis, long stateBeginTime,
                 bool transitionActive, long transitionDuration, long transitionEndTime,
                 bool errorMessageActive, string errorMessage, long errorBeginTime);
    
    void    setWIFIExist(bool w);
    void    setIsServer(bool s);
    void    setIsSpy(bool s);
    int     primaryColor;       // what is your color this round?

private:
    
    int     width, height;
    int     centerX, centerY;  // screen Coords

    bool    WIFIExist;
    bool    isServer;
    bool    isSpy;
    
    ofImage avatars[9];
    ofColor primaries[7];   // 7 colors
    int     complementaries[21]; // indexes of primaries[] of complements to the primaries
    
    float   transition;  // 0 - 1, from beginning of transition to end, use for animations

    float   avatarCoords[NUM_AVATAR_COORDS*2]; // first 9 avatar positions in the ready room. the rest are randomly placed
    
    char    spymess[5];  // scrambled text
    string  scrambleStrings[7] = {"", ".", "..", "...", "....", ".....", "......"};

    ofTrueTypeFont  font;
    ofTrueTypeFont  fontTiny;
    ofTrueTypeFont  fontSmall;
    ofTrueTypeFont  fontMedium;
    ofTrueTypeFont  fontLarge;
    ofImage         reticleCompass;
    ofImage         reticleOutline;
    ofImage         reticleInside;
    ofImage         fingerPrint;
    ofImage         insideCircle;
    ofImage         increment;
    ofImage         decrement;
    
    ofImage         wifiImage;
    ofImage         circleWhite;
    ofImage         circleBlack;
    ofImage         circleShadow;
    ofImage         lightShadow;  // entry page, spotlight
    
    float spin = 0.0;  // spin angle for end animation
    float spinSpeed = 0.0;
    
};

#endif /* defined(__DoubleAgent__AgentView__) */
