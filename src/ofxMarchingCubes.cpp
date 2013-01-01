//
//  ofxMarchingCubes.cpp
//  ofxMarchingCubes
//
//  Created by lars berg on 12/28/12.
//
//

#include "ofxMarchingCubes.h"

ofxMarchingCubes::ofxMarchingCubes(){
	threshold = .5;
	bSmoothed = true;
	flipNormalsValue = -1;
	
	setResolution( 10, 10, 10 );
	
	maxVertexCount = 20000;
	vertices.resize( maxVertexCount );
	normals.resize( maxVertexCount );
	vertexCount = 0;
	alreadyWarned = true;
};
ofxMarchingCubes::~ofxMarchingCubes(){};

void ofxMarchingCubes::setMaxVertexCount( int _maxVertexCount ){
	maxVertexCount = _maxVertexCount;
	vertices.resize( maxVertexCount );
	normals.resize( maxVertexCount );
}

void ofxMarchingCubes::update(float _threshold ){
	threshold = _threshold;
	numTriangles = 0;
	
	for(int i=0; i<normalVals.size(); i++){
		for(int j=0; j<normalVals[i].size(); j++){
			std::fill(normalVals[i][j].begin(), normalVals[i][j].end(), ofVec3f());
			std::fill(voxelComputed[i][j].begin(), voxelComputed[i][j].end(), false );
		}
	}
	
	vertexCount = 0;
	for(int x=0; x<gridPoints.size(); x++){
		for(int y=0; y<gridPoints[x].size(); y++){
			for(int z=0; z<gridPoints[x][y].size(); z++){
				polygonise( x,y,z );
			}
		}
	}
	
	updateTransformMatrix();
}

void ofxMarchingCubes::polygonise( int i, int j, int k ){
	
	if( vertexCount <= maxVertexCount-1){
		/*
		 Determine the index into the edge table which
		 tells us which vertices are inside of the surface
		 */
		int cubeindex = 0;
		int i1 = min(i+1, resXm1), j1 = min(j+1, resYm1), k1 = min(k+1, resZm1);
		cubeindex |= isoVals[i][j][k] > threshold ?   1 : 0;
		cubeindex |= isoVals[i1][j][k] > threshold ?   2 : 0;
		cubeindex |= isoVals[i1][j1][k] > threshold ?   4 : 0;
		cubeindex |= isoVals[i][j1][k] > threshold ?   8 : 0;
		cubeindex |= isoVals[i][j][k1] > threshold ?  16 : 0;
		cubeindex |= isoVals[i1][j][k1] > threshold ?  32 : 0;
		cubeindex |= isoVals[i1][j1][k1] > threshold ?  64 : 0;
		cubeindex |= isoVals[i][j1][k1] > threshold ? 128 : 0;
		
		/* Cube is entirely in/out of the surface */
		if (edgeTable[cubeindex] == 0)		return;
		
		/* Find the vertices where the surface intersects the cube */
		
		if (edgeTable[cubeindex] & 1)		vertexInterp(threshold, i,j,k, i1,j,k, vertList[0], normList[0]);
		if (edgeTable[cubeindex] & 2)		vertexInterp(threshold, i1,j,k, i1,j1,k, vertList[1], normList[1]);
		if (edgeTable[cubeindex] & 4)		vertexInterp(threshold, i1,j1,k, i,j1,k, vertList[2], normList[2]);
		if (edgeTable[cubeindex] & 8)		vertexInterp(threshold, i,j1,k, i,j,k, vertList[3], normList[3]);
		if (edgeTable[cubeindex] & 16)		vertexInterp(threshold, i,j,k1, i1,j,k1, vertList[4], normList[4]);
		if (edgeTable[cubeindex] & 32)		vertexInterp(threshold, i1,j,k1, i1,j1,k1, vertList[5], normList[5]);
		if (edgeTable[cubeindex] & 64)		vertexInterp(threshold, i1,j1,k1, i,j1,k1, vertList[6], normList[6]);
		if (edgeTable[cubeindex] & 128)		vertexInterp(threshold, i,j1,k1, i,j,k1, vertList[7], normList[7]);
		if (edgeTable[cubeindex] & 256)		vertexInterp(threshold, i,j,k, i,j,k1, vertList[8], normList[8]);
		if (edgeTable[cubeindex] & 512)		vertexInterp(threshold, i1,j,k, i1,j,k1, vertList[9], normList[9]);
		if (edgeTable[cubeindex] & 1024)	vertexInterp(threshold, i1,j1,k, i1,j1,k1, vertList[10], normList[10]);
		if (edgeTable[cubeindex] & 2048)	vertexInterp(threshold, i,j1,k, i,j1,k1, vertList[11], normList[11]);
		
		
		for (i=0;triTable[cubeindex][i]!=-1;i+=3) {
			if(bSmoothed){
				//smoothed normals
				normals[vertexCount] = normList[triTable[cubeindex][i]];
				normals[vertexCount+1] = normList[triTable[cubeindex][i+1]];
				normals[vertexCount+2] = normList[triTable[cubeindex][i+2]];
				
			}
			else{
				//faceted
				ofVec3f a = vertList[triTable[cubeindex][i+1]] - vertList[triTable[cubeindex][i]];
				ofVec3f b = vertList[triTable[cubeindex][i+2]] - vertList[triTable[cubeindex][i+1]];
				
				normals[vertexCount] = normals[vertexCount+1] = normals[vertexCount+2] = a.crossed(b).normalize() * flipNormalsValue;
			}
			
			vertices[vertexCount] = vertList[triTable[cubeindex][i]];
			vertices[vertexCount+1] = vertList[triTable[cubeindex][i+1]];
			vertices[vertexCount+2] = vertList[triTable[cubeindex][i+2]];
			vertexCount += 3;
			
			numTriangles++;
		}
	}
	else{
		if(!alreadyWarned){
			ofLogError( "ofxMarhingCubes: maximum vertex("+ofToString(maxVertexCount)+") count exceded. try increasinf the maxVertexCount with setMaxVertexCount()");
			alreadyWarned = true;
		}
	}
}

void ofxMarchingCubes::vertexInterp(float threshold, int i1, int j1, int k1, int i2, int j2, int k2, ofPoint& v, ofPoint& n){
	
	ofVec3f& p1 = gridPoints[i1][j1][k1];
	ofVec3f& p2 = gridPoints[i2][j2][k2];
	
	float& iso1 = isoVals[i1][j1][k1];
	float& iso2 = isoVals[i2][j2][k2];
	
	if(bSmoothed){
		computeNormal( i1, j1, k1 );
		computeNormal( i2, j2, k2 );
		
		ofVec3f& n1 = normalVals[i1][j1][k1];
		ofVec3f& n2 = normalVals[i2][j2][k2];
		
		if (abs(threshold-iso1) < 0.00001){
			v = p1;
			n = n1;
			return;
		}
		if (abs(threshold-iso2) < 0.00001){
			v = p2;
			n = n2;
			return;
		}
		if (abs(iso1-iso2) < 0.00001){
			v.set(p1.x, p1.y, p1.z);
			n = n1;
			return;
		}
		
		//lerp
		float t = (threshold - iso1) / (iso2 - iso1);
		v = p1 + (p2-p1) * t;
		n = n1 + (n2-n1) * t;
	}
	
	else{
		if (abs(threshold-iso1) < 0.00001){
			v = p1;
			return;
		}
		if (abs(threshold-iso2) < 0.00001){
			v = p2;
			return;
		}
		if (abs(iso1-iso2) < 0.00001){
			v.set(p1.x, p1.y, p1.z);
			return;
		}
		
		//lerp
		v = p1 + (p2-p1) * (threshold - iso1) / (iso2 - iso1);
	}
}

void ofxMarchingCubes::computeNormal( int i, int j, int k ) {
	
	
	//TODO:: devise a better way to tell if a voxel has been computed.
	if(!voxelComputed[i][j][k]){
		ofVec3f& n = normalVals[i][j][k];
		n.set(isoVals[min(resXm1, i+1)][j][k] - isoVals[max(0,i-1)][j][k],
			  isoVals[i][min(resYm1, j+1)][k] - isoVals[i][max(0,j-1)][k],
			  isoVals[i][j][min(resZm1, k+1)] - isoVals[i][j][max(0,k-1)]);
		
		n.normalize();
		n *= flipNormalsValue;
		voxelComputed[i][j][k] = true;
	}
};

void ofxMarchingCubes::drawArrays( vector<ofVec3f>* _vertices, vector<ofVec3f>* _normals, bool wireframe){
	if(wireframe)	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glPushMatrix();
	glMultMatrixf( transform.getPtr() );
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof((*_vertices)[0]), &(*_vertices)[0].x);
	
	if(_normals != NULL){
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer( GL_FLOAT, sizeof((*_normals)[0]), &(*_normals)[0].x);
	}
	
	glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	if(_normals != NULL)	glDisableClientState(GL_NORMAL_ARRAY);
	
	glPopMatrix();
	
	if(wireframe)	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void ofxMarchingCubes::drawGrid( bool drawGridPoints){
	
	glPushMatrix();
	glMultMatrixf( transform.getPtr() );
	
	if(drawGridPoints){
		if(gridVertices.size() == 0){
			float xStep = 1./float(resX);
			float yStep = 1./float(resY);
			float zStep = 1./float(resZ);
			ofVec3f halfSize = scale/2.;
			for(int x=0; x<=resX; x++){
				for(int y=0; y<=resY; y++){
					for(int z=0; z<=resZ; z++){
						gridVertices.push_back( ofVec3f(xStep * float(x), yStep * float(y), zStep * float(z)) - .5);

					}
				}
			}
		}
		
		glColor3f(.5,.5,.5);
		glPointSize(.5);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(gridVertices[0]), &gridVertices[0].x);
		glDrawArrays(GL_POINTS, 0, (int)gridVertices.size());
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	
	glColor3f(1,1,1);
	glBegin(GL_LINES);
	glVertex3f(-.5, -.5, -.5);
	glVertex3f(.5, -.5, -.5);
	glVertex3f(-.5, .5, -.5);
	glVertex3f(.5, .5, -.5);
	
	glVertex3f(-.5, -.5, .5);
	glVertex3f(.5, -.5, .5);
	glVertex3f(-.5, .5, .5);
	glVertex3f(.5, .5, .5);
	
	glVertex3f(-.5, -.5, .5);
	glVertex3f(-.5, -.5, -.5);
	glVertex3f(-.5, .5, .5);
	glVertex3f(-.5, .5, -.5);
	
	glVertex3f(.5, -.5, .5);
	glVertex3f(.5, -.5, -.5);
	glVertex3f(.5, .5, .5);
	glVertex3f(.5, .5, -.5);
	
	glVertex3f(-.5, .5, -.5);
	glVertex3f(-.5, -.5, -.5);
	glVertex3f(-.5, .5, .5);
	glVertex3f(-.5, -.5, .5);
	
	glVertex3f(.5, .5, -.5);
	glVertex3f(.5, -.5, -.5);
	glVertex3f(.5, .5, .5);
	glVertex3f(.5, -.5, .5);
	
	glEnd();
	
	glPopMatrix();
}

void ofxMarchingCubes::setIsoValue( int x, int y, int z, float value){
	if(x<resX && y<resY && z<resZ){
		isoVals[x][y][z] = value;
	}
}
float ofxMarchingCubes::getIsoValue(int x, int y, int z){
	if(x<resX && y<resY && z<resZ){
		return isoVals[x][y][z];
	}
	
	return 0;
}

void ofxMarchingCubes::setResolution( int _x, int _y, int _z ){
	
	gridVertices.clear();
	
	resX = _x;
	resY = _y;
	resZ = _z;
	resXm1 = resX-1;
	resYm1 = resY-1;
	resZm1 = resZ-1;
	
	isoVals.resize( resX );
	gridPoints.resize( resX );
	normalVals.resize( resX );
	voxelComputed.resize( resX );
	
	for(int x=0; x<isoVals.size(); x++){
		isoVals[x].resize( resY );
		gridPoints[x].resize( resY );
		normalVals[x].resize( resY );
		voxelComputed[x].resize( resY );
		
		for(int y=0; y<isoVals[x].size(); y++){
			isoVals[x][y].resize( resZ );
			gridPoints[x][y].resize( resZ );
			normalVals[x][y].resize( resZ );
			voxelComputed[x][y].resize( resZ );
		}
	}
	
	setSize( resX*10, resY*10, resZ*10 );
}

void ofxMarchingCubes::wipeIsoValues( float value){
	for(int x=0; x<isoVals.size(); x++){
		for(int y=0; y<isoVals[x].size(); y++){
			std::fill(isoVals[x][y].begin(), isoVals[x][y].end(), value);
			std::fill(voxelComputed[x][y].begin(), voxelComputed[x][y].end(), false);

//			std::fill(gridPoints[x][y].begin(), gridPoints[x][y].end(), ofVec3f());
//			std::fill(normalVals[x][y].begin(), normalVals[x][y].end(), ofVec3f());
		}
	}
}


void ofxMarchingCubes::clear(){
	for(int x=isoVals.size()-1; x>=0; x--){
		for(int y=isoVals[x].size()-1; y>=0; y--){
			isoVals[x][y].clear();
			gridPoints[x][y].clear();
			normalVals[x][y].clear();
		}
		isoVals[x].clear();
		gridPoints[x].clear();
		normalVals[x].clear();
	}
	isoVals.clear();
	gridPoints.clear();
	normalVals.clear();
}