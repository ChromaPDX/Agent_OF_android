#include "testApp.h"

#define HOST "localhost"
// listen on port 1234



//--------------------------------------------------------------
void testApp::setup(){

	ofLogNotice("SETUP") << "Setup testApp &:" << this;

	// initialize the accelerometer
	ofxAccelerometer.setup();


	ofSetLineWidth(10);
	ofBackground(0,0,0);
	// listen on the given port

	JNIEnv *env;

    if (ofGetJavaVMPtr()->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ofLogError("DoubleAgent") << "setup(): failed to get environment using GetEnv()";
        return;
    }

    jclass javaClass = env->FindClass("cc/openframeworks/DoubleAgent/OFActivity");
    if(javaClass==0){
        ofLogError("DoubleAgent") << "setup(): couldn't find OFAndroid java class";
        return;
    }

	jmethodID ofSetup = env->GetStaticMethodID(javaClass,"ofSetup","()V");
	if(!ofSetup){
        ofLogError("DoubleAgent") << "setup(): couldn't find method";
        return;
    }

	ofLogNotice("SETUP") << "Calling JAVA Setup";

	env->CallStaticVoidMethod(javaClass,ofSetup);

	ofLogNotice("SETUP") << "Setup finished";



	agent.setup();
	//sender.setup("192.168.1.255", 1234);

}

//--------------------------------------------------------------
void testApp::update(){

	//ofLogNotice("UPDATE") << "updating . . .";

	agent.updateAccel(ofxAccelerometer.getForce());

	agent.update();


}

void testApp::vibrate(bool on){

	if (on){
		vibrator.vibrate(250);
	}
	else {
		vibrator.stop();
	}

}




//--------------------------------------------------------------
void testApp::draw(){

	agent.draw();

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::touchDown(int x, int y, int id){
	agent.touchBegan(x,y,id);
}

//--------------------------------------------------------------
void testApp::touchMoved(int x, int y, int id){
	agent.touchMoved(x,y,id);
}

//--------------------------------------------------------------
void testApp::touchUp(int x, int y, int id){
	agent.touchEnded(x,y,id);
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(int x, int y, int id){

}

//--------------------------------------------------------------
void testApp::touchCancelled(int x, int y, int id){

}

//--------------------------------------------------------------
void testApp::swipe(ofxAndroidSwipeDir swipeDir, int id){

}

//--------------------------------------------------------------
void testApp::pause(){
	agent.pause();
}

//--------------------------------------------------------------
void testApp::stop(){



}

//--------------------------------------------------------------
void testApp::resume(){
	agent.resume();
}

//--------------------------------------------------------------
void testApp::reloadTextures(){

}

//--------------------------------------------------------------
bool testApp::backPressed(){
	return false;
}

//--------------------------------------------------------------
void testApp::okPressed(){

}

//--------------------------------------------------------------
void testApp::cancelPressed(){

}

void testApp::setIpAddress(const char* ipAddress){

	agent.setIpAddress(ipAddress);

	//oscDest = new string((const char*)broadcast);
	//std::string oscDest ((const char*)broadcast);

}


void testApp::updateRotationMatrix(float m00, float m10,float m20,float m30,
                    float m01,float m11,float m21,float m31,
                    float m02,float m12,float m22,float m32,
                    float m03,float m13,float m23, float m33){

        agent.attitudeMatrix = ofMatrix4x4(m00, m10, m20, m30,
             -m02, -m12, -m22, -m32,
             m01, m11, m21, m31,
              m03, m13, m23, 1.);

        logSensorOrientation(agent.attitudeMatrix);
}

void testApp::logSensorOrientation(ofMatrix4x4 attitudeMatrix){
    static int timeIndex;
    timeIndex++;
    if(timeIndex % 30 == 0)
        ofLogNotice("ORIENTATION") <<
        "\n[ " << attitudeMatrix._mat[0].x << " " << attitudeMatrix._mat[1].x << " " << attitudeMatrix._mat[2].x << " ]" <<
        "\n[ " << attitudeMatrix._mat[0].y << " " << attitudeMatrix._mat[1].y << " " << attitudeMatrix._mat[2].y << " ]" <<
        "\n[ " << attitudeMatrix._mat[0].z << " " << attitudeMatrix._mat[1].z << " " << attitudeMatrix._mat[2].z << " ]" <<
        "\n++++++++++++++++++++++++++++++++";

}

extern "C"{
    void
    Java_cc_openframeworks_DoubleAgent_OFActivity_setIpAddress( JNIEnv*  env, jobject  obj, jstring ipAddress ){

        //char* ip = env->GetStringUTFChars(env,ipAddress,NULL);

        //const char *nativeString = ofGetJNIEnv()->GetStringUTFChars(ipAddress,0);

    	const char *nativeString = env->GetStringUTFChars(ipAddress,0);

    	ofLogNotice("OSC") << "TEST APP &:" << &theApp;

    	((testApp*) ofGetAppPtr())->setIpAddress(nativeString);

    }

    void Java_cc_openframeworks_DoubleAgent_OFActivity_updateRotationMatrix( JNIEnv*  env, jobject  thiz,
                                                                jfloat m00, jfloat m10, jfloat m20, jfloat m30,
                                                                jfloat m01, jfloat m11, jfloat m21, jfloat m31,
                                                                jfloat m02, jfloat m12, jfloat m22, jfloat m32,
                                                                jfloat m03, jfloat m13, jfloat m23, jfloat m33){



    	 ((testApp*)ofGetAppPtr())->updateRotationMatrix(m00, m10, m20, m30,
                 m01, m11, m21, m31,
                  m02, m12, m22, m32,
                   m03, m13, m23, m33);



    }

}

//JNIEXPORT void JNICALL Java_cc_openframeworks_androidAccelerometerExample_OFActivity_setIpAddress
//        (JNIEnv *env, jobject obj, jstring javaString) {
//    // printf("%s", javaString);        // INCORRECT: Could crash VM!
//
//    // Correct way: Create and release native string from Java string
//    const char *nativeString = (*env)->GetStringUTFChars(env, javaString, 0);
//    printf("%s", nativeString);
//    (*env)->ReleaseStringUTFChars(env, javaString, nativeString);
//}
