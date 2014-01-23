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

	agent.setup();

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

	lastAttitude = ofMatrix3x3(1,0,0,  0,1,0,  0,0,1);

	//sender.setup("192.168.1.255", 1234);

}

//--------------------------------------------------------------
void testApp::update(){

	//ofLogNotice("UPDATE") << "updating . . .";

	//agent.updateAccel(ofxAccelerometer.getForce());

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

	ofLogNotice("SETUP") << "setIPAddress call";

	agent.setIpAddress(ipAddress);

	//oscDest = new string((const char*)broadcast);
	//std::string oscDest ((const char*)broadcast);

}


void testApp::updateRotationMatrix(float m00, float m10,float m20,float m30,
                    float m01,float m11,float m21,float m31,
                    float m02,float m12,float m22,float m32,
                    float m03,float m13,float m23, float m33){

//        agent.attitudeMatrix = ofMatrix4x4(m00, m10, m20, m30,
//             -m02, -m12, -m22, -m32,
//             m01, m11, m21, m31,
//              m03, m13, m23, 1.);

        attitude = ofMatrix3x3(m00, m10, m20,
        		m01, m11, m21,
        		m02, m12, m22);

        normalized = attitude * lastAttitude;  // results in the identity matrix plus perturbations between polling cycles
        correctNormalization();  // if near 0 or 1, force into 0 and 1

        agent.updateOrientation(attitude, normalized);  // send data to game controller

        lastAttitude = attitude;   // store last polling cycle to compare next time around
        lastAttitude.transpose(); //getInverse(attitude);  // transpose is the same as inverse for orthogonal matrices. and much easier


//        logSensorOrientation3x3(normalizedMatrix);

}


void testApp::correctNormalization(){

    float BOUNDS = 0.01;

    if(normalized.a < BOUNDS && normalized.a > -BOUNDS ) normalized.a = 0.0;
    else if(normalized.a < 1+BOUNDS && normalized.a > 1-BOUNDS ) normalized.a = 1.0;
    if(normalized.b < BOUNDS && normalized.b > -BOUNDS ) normalized.b = 0.0;
    else if(normalized.b < 1+BOUNDS && normalized.b > 1-BOUNDS ) normalized.b = 1.0;
    if(normalized.c < BOUNDS && normalized.c > -BOUNDS ) normalized.c = 0.0;
    else if(normalized.c < 1+BOUNDS && normalized.c > 1-BOUNDS ) normalized.c = 1.0;
    if(normalized.d < BOUNDS && normalized.d > -BOUNDS ) normalized.d = 0.0;
    else if(normalized.d < 1+BOUNDS && normalized.d > 1-BOUNDS ) normalized.d = 1.0;
    if(normalized.e < BOUNDS && normalized.e > -BOUNDS ) normalized.e = 0.0;
    else if(normalized.e < 1+BOUNDS && normalized.e > 1-BOUNDS ) normalized.e = 1.0;
    if(normalized.f < BOUNDS && normalized.f > -BOUNDS ) normalized.f = 0.0;
    else if(normalized.f < 1+BOUNDS && normalized.f > 1-BOUNDS ) normalized.f = 1.0;
    if(normalized.g < BOUNDS && normalized.g > -BOUNDS ) normalized.g = 0.0;
    else if(normalized.g < 1+BOUNDS && normalized.g > 1-BOUNDS ) normalized.g = 1.0;
    if(normalized.h < BOUNDS && normalized.h > -BOUNDS ) normalized.h = 0.0;
    else if(normalized.h < 1+BOUNDS && normalized.h > 1-BOUNDS ) normalized.h = 1.0;
    if(normalized.i < BOUNDS && normalized.i > -BOUNDS ) normalized.i = 0.0;
    else if(normalized.i < 1+BOUNDS && normalized.i > 1-BOUNDS ) normalized.i = 1.0;

}


ofMatrix3x3 testApp::getInverse(ofMatrix3x3 x){
	float determinant;
	determinant = x.a*x.e*x.i + x.b*x.f*x.g + x.c*x.d*x.h
				- x.c*x.e*x.g - x.b*x.d*x.i - x.a*x.f*x.h;

	ofMatrix3x3 co = ofMatrix3x3((x.e*x.i-x.f*x.h),-(x.d*x.i-x.g*x.f), (x.d*x.h-x.g*x.e),
						  	    -(x.b*x.i-x.h*x.c), (x.a*x.i-x.g*x.c),-(x.a*x.h-x.g*x.b),
							     (x.b*x.f-x.c*x.e),-(x.a*x.f-x.c*x.d), (x.a*x.e-x.d*x.b));

	ofMatrix3x3 adj = ofMatrix3x3(co.a, co.d, co.g,
							      co.b, co.e, co.h,
							      co.c, co.f, co.i);

	float d = 1/determinant;

	ofMatrix3x3 inverse = ofMatrix3x3(adj.a*d, adj.b*d, adj.c*d,
									  adj.d*d, adj.e*d, adj.f*d,
									  adj.g*d, adj.h*d, adj.i*d);
	return inverse;
}

void testApp::logSensorOrientation3x3(ofMatrix3x3 attitudeMatrix){
    static int timeIndex;
    timeIndex++;
    if(timeIndex % 30 == 0)
        ofLogNotice("ORIENTATION") <<
        "\n[ " << attitudeMatrix.a << " " << attitudeMatrix.b << " " << attitudeMatrix.c << " ]" <<
        "\n[ " << attitudeMatrix.d << " " << attitudeMatrix.e << " " << attitudeMatrix.f << " ]" <<
        "\n[ " << attitudeMatrix.g << " " << attitudeMatrix.h << " " << attitudeMatrix.i << " ]" <<
        "\n++++++++++++++++++++++++++++++++";
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
