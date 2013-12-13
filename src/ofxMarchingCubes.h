//
//  ofxMarchingCubes.h
//  ofxMarchingCubes

//http://paulbourke.net/geometry/polygonise/
//

/*
TODO::
 -get worldposition in grid
 -add iso value at world position
 */



#pragma once

#include "ofMain.h"
#include "mcTables.h"

class ofxMarchingCubes{
public:
	ofxMarchingCubes();
	~ofxMarchingCubes();
	
	void setMaxVertexCount( int _maxVertexCount = 100000 );
	
	void setup( int resX=30, int resY=20, int resZ=30, int _maxVertexCount=100000);
	void update(){		update( threshold );}
	void update(float _threshold);
	
	void draw( GLenum renderType = GL_TRIANGLES );
	void drawWireframe();
	void drawArrays( vector<ofVec3f>* _vertices, vector<ofVec3f>* _normals=NULL );
	void drawGrid( bool drawGridPoints=true);
	
	void flipNormals(){	flipNormalsValue *= -1;}
	void setResolution( int _x=10, int _y=10, int _z=10 );
	void polygonise( int i, int j, int k );
	void computeNormal( int i, int j, int k );
	void vertexInterp(float threshold, int i1, int j1, int k1, int i2, int j2, int k2, ofPoint& v, ofPoint& n);
	
	void setIsoValue( int x, int y, int z, float value);
	void addToIsoValue( int x, int y, int z, float value){
		getIsoValue(x,y,z) += value;
        bUpdateMesh = true;
	}
	
	bool getSmoothing(){	return bSmoothed;}
	void setSmoothing( bool _bSmooth ){		bSmoothed = _bSmooth;}
	
	
	void wipeIsoValues( float value=0.f);
	
	void clear();//deletes all the data. use whip
	
	
	void setGridPoints( float _x, float _y, float _z);
	
	void updateTransformMatrix(){
		transform.makeScaleMatrix( scale );
		transform.rotate( orientation );
		transform.setTranslation( position );
	}
	
	inline float& getIsoValue( int x, int y, int z){
		return isoVals[ x*resY*resZ+ y*resZ + z ];
	}
	inline ofVec3f& getGridPoint( int x, int y, int z){
		return gridPoints[ x*resY*resZ+ y*resZ + z ];
	}
	inline ofVec3f& getNormalVal( int x, int y, int z){
		return normalVals[ x*resY*resZ+ y*resZ + z ];
	}
	inline unsigned int& getGridPointComputed( int x, int y, int z){
		return gridPointComputed[ x*resY*resZ+ y*resZ + z ];
	}
	
	void exportObj( string fileName );
	
	//private:
	ofMatrix4x4 transform;
	int	resX, resY, resZ;
	int resXm1, resYm1, resZm1;
	float flipNormalsValue;
	ofVec3f cellDim;
	vector<float> isoVals;
	vector<ofVec3f> gridPoints;
	vector<ofVec3f> normalVals;
	vector<unsigned int> gridPointComputed;

	
	vector< ofVec3f > vertices;
	vector< ofVec3f > normals;
	vector<float> boundryBox;
	int vertexCount, maxVertexCount;
	
	ofVec3f vertList[12], normList[12];
	
	float threshold;
	bool bSmoothed, beenWarned;
	
	//transforms
	ofVec3f position, scale, up;
	ofQuaternion orientation;
	
	ofMatrix3x3 normalMatrix;
	
	ofVbo vbo;
	bool bUpdateMesh;
};