#include "testApp.h"

ofShader normalShader;
bool wireframe;

int differentDemos;

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(30, 32, 35);
	glEnable(GL_DEPTH_TEST);
	
	differentDemos = 0;
	mc.setMaxVertexCount(100000);
	mc.setResolution(20,20,20);
	mc.setSize( 300, 300, 300 );
		
	normalShader.load("shaders/normalShader");
}

//--------------------------------------------------------------
void testApp::update(){
	float elapsedTime = ofGetElapsedTimef();
	
	
	
	if(differentDemos == 1){
		//NOISE
		float noiseStep = elapsedTime;// * .1;
		float noiseScale = .1;
		for(int i=0; i<mc.resX; i++){
			for(int j=0; j<mc.resY; j++){
				for(int k=0; k<mc.resZ; k++){
					//noise
					float nVal = ofNoise(float(i)*noiseScale, float(j)*noiseScale, float(k)*noiseScale + noiseStep);
					if(nVal > 0.)	nVal *= ofNoise(float(i)*noiseScale*2, float(j)*noiseScale*2, float(k)*noiseScale*2 + noiseStep);
					mc.setIsoValue( i, j, k, nVal );
				}
			}
		}
	}
	else if( differentDemos == 0){
		//SPHERES. not technically spheres... but they're roundish
		float stepI = PI * 6./mc.resX;
		float stepJ = PI * 6./mc.resY;
		float stepK = PI * 6./mc.resZ;
		for(int i=0; i<mc.resX; i++){
			for(int j=0; j<mc.resY; j++){
				for(int k=0; k<mc.resZ; k++){;
					mc.setIsoValue( i, j, k, sin(float(i)*stepI) * sin(float(j+elapsedTime)*stepJ) * sin(float(k+elapsedTime)*stepK));
				}
			}
		}
	}
	else{
		//SIN
		float sinScale = 1.;
		for(int i=0; i<mc.resX; i++){
			for(int j=0; j<mc.resY; j++){
				for(int k=0; k<mc.resZ; k++){
					mc.setIsoValue( i, j, k, sin(float(i)*sinScale) + cos(float(j)*sinScale) + sin(float(k)*sinScale + elapsedTime));
				}
			}
		}
	}
	
	
	//update the mesh
	mc.update(.3);
	
}

//--------------------------------------------------------------
void testApp::draw(){
    float elapsedTime = ofGetElapsedTimef();
    ofSetWindowTitle( ofToString( ofGetFrameRate() ) );
	
	camera.begin();
	
	//draw the mesh
	normalShader.begin();
	if(wireframe){
		mc.drawWireframe();
	}else{
		mc.draw();
	}
	normalShader.end();
	
	//draw the voxel grid
	mc.drawGrid();
	
	camera.end();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key) {
		case 'w':
			wireframe = !wireframe;
			break;
			
		case 'f':
			mc.flipNormals();
			break;
			
		case 's':
			mc.setSmoothing( !mc.getSmoothing() );
			break;
			
		case ' ':
			differentDemos++;
			if(differentDemos>=3){
				differentDemos = 0;
			}
			break;
			
		default:
			break;
	}
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
	
}