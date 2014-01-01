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

	agent.accel = ofxAccelerometer.getForce();

	agent.messages[0] = "g(x) = " + ofToString(agent.accel.x,2);
	agent.messages[1] = "g(y) = " + ofToString(agent.accel.y,2);
	agent.messages[2] = "g(z) = " + ofToString(agent.accel.z,2);
	agent.normAccel = agent.accel.getNormalized();

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



extern "C"{
    void
    Java_cc_openframeworks_DoubleAgent_OFActivity_setIpAddress( JNIEnv*  env, jobject  obj, jstring ipAddress ){

        //char* ip = env->GetStringUTFChars(env,ipAddress,NULL);

        //const char *nativeString = ofGetJNIEnv()->GetStringUTFChars(ipAddress,0);

    	const char *nativeString = env->GetStringUTFChars(ipAddress,0);

    	ofLogNotice("OSC") << "TEST APP &:" << &theApp;

    	((testApp*) ofGetAppPtr())->setIpAddress(nativeString);

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
