package cc.openframeworks.DoubleAgent;

import java.util.List;

import android.os.Bundle;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import cc.openframeworks.OFAndroid;
import cc.openframeworks.DoubleAgent.ipUtils;

import android.util.Log;
import android.content.Context;

import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorManager;
import android.hardware.SensorEventListener;

public class OFActivity extends cc.openframeworks.OFActivity implements SensorEventListener{

	

    private SensorManager sensorManager;
private Sensor accelerometerSensor;
private Sensor compassSensor;

    float[] accelerometer = new float[3];
    float[] compass = new float[3];
    float[] gyroscope = new float[3];
    float[] mOrientation = new float[3]; 
    float[] mRotationM = new float[16];
    float[] mRemapedRotationM = new float[16]; 


    
    Boolean bool = false, bool1 = false;
    
    
	@Override
    public void onCreate(Bundle savedInstanceState)
    { 
        super.onCreate(savedInstanceState);
        String packageName = getPackageName();

        System.setProperty("java.net.preferIPv4Stack" , "true");
       
        
        ofApp = new OFAndroid(packageName,this);
        
        		
        //WifiManager wm = (WifiManager) getSystemService(WIFI_SERVICE);
        
        //System.out.println("WIFI: IPv4 ADDRESS: " + wm.getConnectionInfo().getIpAddress());
        //System.out.println("IPv6 ADDRESS: " + ipUtils.getIPAddress(false));
        
        //System.out.println("IPv4 ADDRESS: " + ipUtils.getIPAddress(true));
        sensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
        
        
    }
	
    public static void ofSetup() {
    	String ip = ipUtils.getIPAddress(true);
    	
    	System.out.println("FETCHING IPv4 ADDRESS: " + ip);
    	
    	setIpAddress(ip);
    	
    }
    
    float gravity[] = new float [3];
	
    @Override
	public void onSensorChanged(SensorEvent event) {
//		System.out.println("SENSOR EVENT CHANGED");
		 switch (event.sensor.getType()) {
		    case Sensor.TYPE_ACCELEROMETER:
		    	
		   
		              // alpha is calculated as t / (t + dT)
		              // with t, the low-pass filter's time-constant
		              // and dT, the event delivery rate

		              final float alpha = (float)0.8;

		              gravity[0] = alpha * gravity[0] + (1 - alpha) * event.values[0];
		              gravity[1] = alpha * gravity[1] + (1 - alpha) * event.values[1];
		              gravity[2] = alpha * gravity[2] + (1 - alpha) * event.values[2];

		              accelerometer[0] = event.values[0] - gravity[0];
		              accelerometer[1] = event.values[1] - gravity[1];
		              accelerometer[2] = event.values[2] - gravity[2];
		         
		
		        //onAccelChanged(accelerometer[0], accelerometer[1],accelerometer[2] );
		    
//		        onFilteredAccelerometerChanged();
//		        	System.out.println("Accel: " + String.valueOf(accelerometer[0]) + " " + String.valueOf(accelerometer[1]) + " " + String.valueOf(accelerometer[2]));
		        

		        break;

		    case Sensor.TYPE_MAGNETIC_FIELD:
		        for(int i=0; i<3; i++){
		            compass[i] = event.values[i];
		        }
//		        if(compass[2] != 0){
//		        	//onCompassChanged(compass[0], compass[1],compass[2]);
//		            //System.out.println("Compass: " + String.valueOf(compass[0]) + " " + String.valueOf(compass[1]) + " " + String.valueOf(compass[2]));
//		        }
		        break;
		    

		 }
		 boolean success = SensorManager.getRotationMatrix(mRotationM, null, gravity, compass);

		    //SensorManager.getRotationMatrixFromVector(deltaRotationMatrix, deltaRotationVector);

			if	(success){

		        updateRotationMatrix(mRotationM[0], mRotationM[1], mRotationM[2], mRotationM[3], 
		        		mRotationM[4], mRotationM[5], mRotationM[6], mRotationM[7], 
		        		mRotationM[8], mRotationM[9], mRotationM[10], mRotationM[11], 
		        		mRotationM[12], mRotationM[13], mRotationM[14], mRotationM[15]		        		
		        		);
		       
//		        System.out.println(mRotationM[0] + " " + mRotationM[1] + " " + mRotationM[2] + " " + mRotationM[3]);
//		        System.out.println(mRotationM[4] + " " + mRotationM[5] + " " + mRotationM[6] + " " + mRotationM[7]);
//		        System.out.println(mRotationM[8] + " " + mRotationM[9] + " " + mRotationM[10] + " " + mRotationM[11]); 
//		        System.out.println(mRotationM[12] + " " + mRotationM[13] + " " + mRotationM[14] + " " + mRotationM[15]);
			}
			else {
				System.out.println("COMPASS FAIL" + compass[0] +compass[1]);
			}


	}
	
//	float lastAccel[] = new float[3];
//	float accelFilter[] = new float[3];
//	
//	
//	float lpAccel[] = new float[3];
//	float lpTemp[] = new float[3];
//	
//	float lastCompass[] = new float[3];
//	float compassFilter[] = new float[3];
//	
//	public void onFilteredAccelerometerChanged() {
//		
//		float aFilteringFactor = (float).1;
//		float gFilteringFactor = (float).2;
//
//	    lpAccel[0] = accelerometer[0] * aFilteringFactor + lpAccel[0] * (1.0f - aFilteringFactor);
//	    lpAccel[1] = accelerometer[1] * aFilteringFactor + lpAccel[1] * (1.0f - aFilteringFactor);
//	    lpAccel[2] = accelerometer[2] * aFilteringFactor + lpAccel[2] * (1.0f - aFilteringFactor);
//		
//		lastCompass[0] = compass[0] * gFilteringFactor + lastCompass[0] * (1.0f - gFilteringFactor);
//		lastCompass[1] = compass[1] * gFilteringFactor + lastCompass[1] * (1.0f - gFilteringFactor);
//		lastCompass[2] = compass[2] * gFilteringFactor + lastCompass[2] * (1.0f - gFilteringFactor);
//		
////	    compassFilter[0] = compass[0] - lastCompass[0];
////	    compassFilter[1] = compass[1] - lastCompass[1];
////	    compassFilter[2] = compass[2] - lastCompass[2];
//		
//		 boolean success = SensorManager.getRotationMatrix(mRotationM, null, accelerometer, compass);
//
//		    //SensorManager.getRotationMatrixFromVector(deltaRotationMatrix, deltaRotationVector);
//
//			if	(success){
//
//		        updateRotationMatrix(mRotationM[0], mRotationM[1], mRotationM[2], mRotationM[3], 
//		        		mRotationM[4], mRotationM[5], mRotationM[6], mRotationM[7], 
//		        		mRotationM[8], mRotationM[9], mRotationM[10], mRotationM[11], 
//		        		mRotationM[12], mRotationM[13], mRotationM[14], mRotationM[15]		        		
//		        		);
//		       
//		        System.out.println(mRotationM[0] + " " + mRotationM[1] + " " + mRotationM[2] + " " + mRotationM[3]);
//		        System.out.println(mRotationM[4] + " " + mRotationM[5] + " " + mRotationM[6] + " " + mRotationM[7]);
//		        System.out.println(mRotationM[8] + " " + mRotationM[9] + " " + mRotationM[10] + " " + mRotationM[11]); 
//		        System.out.println(mRotationM[12] + " " + mRotationM[13] + " " + mRotationM[14] + " " + mRotationM[15]);
//			}
//			else {
//				System.out.println("COMPASS FAIL" + lastCompass[0] + lastCompass[1]);
//			}
//		        
//	}
//	private static final boolean ADAPTIVE_ACCEL_FILTER = true;
//
//	
//	public void onAccelChanged(float accelX, float accelY, float accelZ) {
////	    // high pass filter
//	    float updateFreq = 30; // match this to your update speed
//	    float cutOffFreq = 0.9f;
//	    float RC = 1.0f / cutOffFreq;
//	    float dt = 1.0f / updateFreq;
//	    float filterConstant = RC / (dt + RC);
//	    float alpha = filterConstant; 
//	    float kAccelerometerMinStep = 0.033f;
//	    float kAccelerometerNoiseAttenuation = 3.0f;
//
//	    if(ADAPTIVE_ACCEL_FILTER)
//	    {
//	        float d = Clamp(Math.abs(Norm(compassFilter[0], compassFilter[1], compassFilter[2]) - Norm(accelX, accelY, accelZ)) / kAccelerometerMinStep - 1.0f, 0.0f, 1.0f);
//	        alpha = (d * filterConstant / kAccelerometerNoiseAttenuation + (1.0f - d) * filterConstant);
//	    }
//
//	    accelFilter[0] = (float) (alpha * (accelFilter[0] + accelX - lastAccel[0]));
//	    accelFilter[1] = (float) (alpha * (accelFilter[1] + accelY - lastAccel[1]));
//	    accelFilter[2] = (float) (alpha * (accelFilter[2] + accelZ - lastAccel[2]));
//
//	    lastAccel[0] = accelX;
//	    lastAccel[1] = accelY;
//	    lastAccel[2] = accelZ;
//	    
//		
//	}
//	
//
//	
//	public float Norm(float x, float y, float z)
//	{
//	    return (float)Math.sqrt(x * x + y * y + z * z);
//	}
//	 
//	public float Clamp(float v, float min, float max)
//	{
//	    if(v > max)
//	        return max;
//	    else if(v < min)
//	        return min;
//	    else
//	        return v;
//	}
    
	@Override
	public void onDetachedFromWindow() {
	}
	
    @Override
    protected void onPause() {
    	ofApp.pause();
        super.onPause();
        sensorManager.unregisterListener(this);
    }

    @Override
    protected void onResume() {
        super.onResume();
        ofApp.resume();
        

  		List<Sensor> sensors = sensorManager.getSensorList(Sensor.TYPE_ACCELEROMETER);
        if(sensors.size() > 0)
        {
        	accelerometerSensor = sensors.get(0);
        	sensorManager.registerListener(this, accelerometerSensor, SensorManager.SENSOR_DELAY_GAME);
        	Log.v("OF", "accelerometer set up correctly");
        }else{
        	Log.e("OF","no accelerometer available");
        }
        sensors = sensorManager.getSensorList(Sensor.TYPE_MAGNETIC_FIELD);
        if(sensors.size() > 0)
        {
        	compassSensor = sensors.get(0);
        	sensorManager.registerListener(this, compassSensor, SensorManager.SENSOR_DELAY_GAME);
        	Log.v("OF", "compass set up correctly");
        }else{
        	Log.e("OF","no compass available");
        }


        
    }
    

    
    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
	if (OFAndroid.keyDown(keyCode, event)) {
	    return true;
	} else {
	    return super.onKeyDown(keyCode, event);
	}
    }
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
	if (OFAndroid.keyUp(keyCode, event)) {
	    return true;
	} else {
	    return super.onKeyUp(keyCode, event);
	}
    }


	OFAndroid ofApp;
    
	
	
    // Menus
    // http://developer.android.com/guide/topics/ui/menus.html
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	// Create settings menu options from here, one by one or infalting an xml
        return super.onCreateOptionsMenu(menu);
    }
    
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	// This passes the menu option string to OF
    	// you can add additional behavior from java modifying this method
    	// but keep the call to OFAndroid so OF is notified of menu events
    	if(OFAndroid.menuItemSelected(item.getItemId())){
    		
    		return true;
    	}
    	return super.onOptionsItemSelected(item);
    }
    

    @Override
    public boolean onPrepareOptionsMenu (Menu menu){
    	// This method is called every time the menu is opened
    	//  you can add or remove menu options from here
    	return  super.onPrepareOptionsMenu(menu);
    }
	
    // NATIVE DEFS
    
    public static native void setIpAddress(String ipAddress);
    
    public native void updateRotationMatrix(float m00, float m10, float m20, float m30,
            float m01, float m11, float m21, float m31,
            float m02, float m12, float m22, float m32,
            float m03, float m13, float m23, float m33);

	@Override
	public void onAccuracyChanged(Sensor sensor, int accuracy) {
		// TODO Auto-generated method stub
		
	}


        
}



