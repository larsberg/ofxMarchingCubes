//
//  ofxMarchingCubes.h
//  ofxMarchingCubes
//
//  Created by lars berg on 12/28/12.
//
//

/*
 TODO::
 -transform matrix
 -get worldposition
 -add iso value at world position
 
 Done:
 -refactor vector filling and rendering
 -simple normal shader in example
 
 the no cry sleep solution
 */

/*
 ISSUES:
 -smooth normals are only correct for cube shaped voxels
 -seems like we should use openCL for this...
 */


#pragma once

#include "ofMain.h"
#include "mcTables.h"

class ofxMarchingCubes{
public:
	ofxMarchingCubes();
	~ofxMarchingCubes();
	
	void setMaxVertexCount( int _maxVertexCount );
	
	void update(float _threshold );
	
	void draw(){ drawArrays( &vertices, &normals, false );;}
	void drawWireframe(){ drawArrays( &vertices, &normals, true );}
	
	void drawArrays( vector<ofVec3f>* _vertices, vector<ofVec3f>* _normals=NULL, bool wireframe=false);

	void setResolution( int _x=10, int _y=10, int _z=10 );
	void polygonise( int i, int j, int k );
	void computeNormal( int i, int j, int k );
	void vertexInterp(float threshold, int i1, int j1, int k1, int i2, int j2, int k2, ofPoint& v, ofPoint& n);
	
	void setIsoValue( int x, int y, int z, float value);
	float getIsoValue(int x, int y, int z);
	
	
	void flipNormals(){
		flipNormalsValue *= -1;
	}
	bool getSmoothing(){	return bSmoothed;}
	void setSmoothing( bool _bSmooth ){
		bSmoothed = _bSmooth;
	}
	
	void wipeIsoValues( float value=0.f);
	
	void clear();//deletes all the data. use whip
	
	void drawGrid( bool drawGridPoints=true);
	
	void setSize( float _x, float _y, float _z){
		scale.set( _x, _y, _z );
		ofVec3f halfSize = scale / 2;
		ofVec3f cellDim = 1. / ofVec3f(resX-1, resY-1, resZ-1 );
		
		for(int i=0; i<gridPoints.size(); i++){
			for(int j=0; j<gridPoints[i].size(); j++){
				for(int k=0; k<gridPoints[i][j].size(); k++){
					gridPoints[i][j][k].set(float(i)*cellDim.x, float(j)*cellDim.y, float(k)*cellDim.z);
					gridPoints[i][j][k] -= .5;
					normalVals[i][j][k].set( 0, 0, 0);
				}
			}
		}
	}
	
	void updateTransformMatrix(){
		transform.makeScaleMatrix( scale );
		transform.rotate( orientation );
		transform.setTranslation( position );
	}
	
	//private:
	ofMatrix4x4 transform;
	int	resX, resY, resZ, numTriangles;
	int resXm1, resYm1, resZm1;
	float flipNormalsValue;
	vector< vector< vector<float> > > isoVals;
	vector< vector< vector<ofVec3f> > > gridPoints;
	vector< vector< vector<ofVec3f> > > normalVals;
	vector< vector< vector<bool> > > voxelComputed;
	
	float* iso;
	ofVec3f* gp;
	ofVec3f* norm;
	
	vector< ofVec3f > vertices;
	vector< ofVec3f > normals;
	vector< ofVec3f > gridVertices;
	int vertexCount, maxVertexCount;
	bool alreadyWarned;
	
	ofVec3f vertList[12], normList[12];
	
	float threshold;
	bool bSmoothed;
	
	//transforms
	ofVec3f position, scale, up;
	ofQuaternion orientation;
	
	ofMatrix3x3 normalMatrix;
	ofMatrix4x4 modelMatrix, worldMatrix;
};