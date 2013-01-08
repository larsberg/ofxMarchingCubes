# ofxMarchingCubes -- an implamentation of //http://paulbourke.net/geometry/polygonise/

By Lars Berg

#usage

ofxMarchingCubes mc;

void testApp::setup(){
	mc.setResolution(32,16,32);
	mc.scale.set( 500, 250, 500 );
}

void testApp::update(){
	//3D noise
	float noiseStep = ofGetElapsedTimef() * .5;
	float noiseScale = .06;
	float noiseScale2 = noiseScale * 2.;
	for(int i=0; i<mc.resX; i++){
		for(int j=0; j<mc.resY; j++){
			for(int k=0; k<mc.resZ; k++){
				//noise
				float nVal = ofNoise(float(i)*noiseScale, float(j)*noiseScale, float(k)*noiseScale + noiseStep);
				if(nVal > 0.)	nVal *= ofNoise(float(i)*noiseScale2, float(j)*noiseScale2, float(k)*noiseScale2 + noiseStep);
				mc.setIsoValue( i, j, k, nVal );
			}
		}
	}

	mc.update(.3);// update( float threshold )
}

void testApp::draw(){
	camera.begin();
	mc.draw();
	mc.drawGrid();
	camera.end();
}