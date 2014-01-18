#pragma once

#include "ofMain.h"

#include "ofxAndroid.h"
#include "ofxAndroidVibrator.h"

#include "ofxAccelerometer.h"




#include <jni.h>


#include "agentController.h"





class testApp : public ofxAndroidApp{
	
	public:

//		static testApp& getGlobal()
//	        {
//	            static testApp global1;
//	            return global1;
//	        }


		void setIpAddress(const char *ipAddress);

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);

		void touchDown(int x, int y, int id);
		void touchMoved(int x, int y, int id);
		void touchUp(int x, int y, int id);
		void touchDoubleTap(int x, int y, int id);
		void touchCancelled(int x, int y, int id);
		void swipe(ofxAndroidSwipeDir swipeDir, int id);

		void pause();
		void stop();
		void resume();
		void reloadTextures();

		bool backPressed();
		void okPressed();
		void cancelPressed();


		agentController agent;

		void vibrate(bool on);

		void updateRotationMatrix(float m00, float m10,float m20,float m30,
		                    float m01,float m11,float m21,float m31,
		                    float m02,float m12,float m22,float m32,
		                    float m03,float m13,float m23, float m33);

		ofxAndroidVibrator vibrator;

	    ofMatrix3x3 attitude;
	    ofMatrix3x3 lastAttitude;
	    ofMatrix3x3 normalized;
	    void correctNormalization();

		ofMatrix3x3 getInverse(ofMatrix3x3 inputMatrix);

		void logSensorOrientation(ofMatrix4x4 attitudeMatrix);
		void logSensorOrientation3x3(ofMatrix3x3 attitudeMatrix);

		void oscUpdate();

	private:




};


static testApp theApp;
