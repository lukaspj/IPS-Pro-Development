// Copyright (C) 2013 Winterleaf Entertainment L,L,C.
// 
// THE SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND,
// INCLUDING WITHOUT LIMITATION THE WARRANTIES OF MERCHANT ABILITY, FITNESS
// FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT. THE ENTIRE RISK AS TO THE
// QUALITY AND PERFORMANCE OF THE SOFTWARE IS THE RESPONSIBILITY OF LICENSEE.
// SHOULD THE SOFTWARE PROVE DEFECTIVE IN ANY RESPECT, LICENSEE AND NOT LICEN-
// SOR OR ITS SUPPLIERS OR RESELLERS ASSUMES THE ENTIRE COST OF ANY SERVICE AND
// REPAIR. THIS DISCLAIMER OF WARRANTY CONSTITUTES AN ESSENTIAL PART OF THIS
// AGREEMENT. NO USE OF THE SOFTWARE IS AUTHORIZED HEREUNDER EXCEPT UNDER
// THIS DISCLAIMER.
// 
// The use of the WinterLeaf Entertainment LLC Improved Particle System Bundle (IPS Bundle)
// is governed by this license agreement.
// 
// RESTRICTIONS
// 
// (a) Licensee may not: (i) create any derivative works of IPS Bundle, including but not
// limited to translations, localizations, technology add-ons, or game making software
// other than Games without express permission from Winterleaf Entertainment; (ii) redistribute, 
// encumber , sell, rent, lease, sublicense, or otherwise
// transfer rights to IPS Bundle; or (iii) remove or alter any trademark, logo, copyright
// or other proprietary notices, legends, symbols or labels in IPS Bundle; or (iv) use
// the Software to develop or distribute any software that competes with the Software
// without WinterLeaf Entertainment's prior written consent; or (v) use the Software for
// any illegal purpose.
// (b) Licensee may not distribute the IPS Bundle in any manner.
// 
// LICENSEGRANT.
// This license allows companies of any size, government entities or individuals to cre-
// ate, sell, rent, lease, or otherwise profit commercially from, games using executables
// created from the source code of IPS Bundle
// 
// Please visit http://www.winterleafentertainment.com for more information about the project and latest updates.

#ifndef _IPSCore_H_
#define _IPSCore_H_
#include "gfx/gfxDrawUtil.h"

#include "T3D\shapeBase.h"
#include "T3D\tsStatic.h"
#include "console/engineAPI.h"  
#include "console/consoleTypes.h" 
#include <time.h>
#include <iostream>
#include <fstream>
/*
#ifdef _WIN32
	#include <windows.h>
#endif*/


#ifndef ParticleBehaviourCount
#define ParticleBehaviourCount (U8)8
#endif

class IPSCore{
public:
	struct intersection
	{
		int count;
		Point3F intersection1;
		Point3F intersection2;
		F32 intersection1Coeff;
		F32 intersection2Coeff;
	};

	struct face
	{
		S32 triStart;
		S32 meshIndex;
		bool skinMesh;
		F32 fullArea;
		F32 area;
		intersection sideAIntersect;
		intersection sideBIntersect;
		intersection sideCIntersect;
		bool v1Inside;
		bool v2Inside;
		bool v3Inside;
		int vertsInside;
		bool inboundSphere;
		F32 sphereProjp1CoeffX;
		F32 sphereProjp1CoeffY;
		F32 sphereSideACosFrom;
		F32 sphereSideACosTo;
		F32 sphereSideBCosFrom;
		F32 sphereSideBCosTo;
		F32 sphereSideCCosFrom;
		F32 sphereSideCCosTo;
		F32 sphereSideASinFrom;
		F32 sphereSideASinTo;
		F32 sphereSideBSinFrom;
		F32 sphereSideBSinTo;
		F32 sphereSideCSinFrom;
		F32 sphereSideCSinTo;

		F32 trapezoidArea1;
		F32 trapezoidArea2;
		F32 trapezoidCoef;

		F32 arcArea1;
		F32 arcArea1Coeff;
		F32 arcArea2;
		F32 arcArea2Coeff;
	};
	// Gets the height and normal of the terrain at the given x and y coordinates
	static bool GetTerrainInfoAtPosition(const F32 x, const F32 y, F32 &height, Point3F &normal);
	// Gets the height, normal and material of the terrain at the given x and y coordinates
	static bool	GetTerrainInfoAtPosition(const F32 &x, const F32 &y, F32 &height, StringTableEntry &mat, Point3F &normal);
	// Returns intersections between a sphere and a ray
	static IPSCore::intersection raySphereIntersection(Point3F center, F32 radius, Point3F from, Point3F to);
	// Returns intersections between a sphere and a line
	static IPSCore::intersection lineSphereIntersection(Point3F center, F32 radius, Point3F from, Point3F to);
	// Returns intersections between a triangle and a sphere
	static void triSphereIntersection(StringTableEntry object, face* tri, Point3F center, F32 radius, Point3F p1, Point3F p2, Point3F p3);
	// Calculates the area of a triangle using Herons formula
	static F32 HeronsF(VectorF a, VectorF b, VectorF c);
	// Calculates the angle between two vectors with a given normal
	static F32 AngleBetween(VectorF N, VectorF V1, VectorF V2);
	// Calculates the height of a chord
	static F32 ChordHeight(Point3F p1, Point3F p2, Point3F N, Point3F center, F32 radius);
	// Calculates the height of a chord with a given chord-coefficient
	static F32 ChordHeight(Point3F p1, Point3F p2, Point3F N, Point3F center, F32 radius, F32 coefficient);
	// Calculates the square of a float
	static float square( float f ) { return (f*f); };
	// Calculates the perpendicular vector to a vector with a given normal
	static Point3F perpendicularVector(Point3F V1, Point3F N);
	// Projects one vector onto another
	static Point3F project(Point3F V1, Point3F V2);
	// Calculates the area of a circle segment
	static F32 AreaOfCircularSegment(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd);
	// Determines whether a point is within a given arch
	static bool IsPointInArc(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd, Point3F point);
	// Determines whether two points is on the same side of a line
	static bool SameSideOfLine(Point3F lineStart, Point3F lineEnd, Point3F p1, Point3F p2);
	// Gets the top of a circular segment
	static Point3F TopOfCircularSegment(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd);
	// --- Internal functions ---
	static F32 getClockTime() { return ((F32)clock())/CLOCKS_PER_SEC; };

	static char* UpToLow(char* str) {
		for (int i=0;i<strlen(str);i++)
			if(isalpha(str[i]))
				str[i] = tolower(str[i]);
		return str;
	};
   /*
	#ifdef _WIN32
	static UINT64 get_time()
	{
		LARGE_INTEGER t;
		if( QueryPerformanceCounter(&t) == false )
			Con::printf("Query failed!");
		UINT64 Utick = t.QuadPart;
		return Utick;
	};

	static F64 get_time(UINT64 base)
	{
		LARGE_INTEGER t, f;
		UINT64 Utick, delta;
		if( QueryPerformanceCounter(&t) == false )
			Con::printf("Query failed!!");
		if( QueryPerformanceFrequency(&f) == false )
			Con::printf("Query failed!!!");
		Utick = t.QuadPart;
		delta = Utick - base;
		F64 ret = ((F64)delta)/((F64)f.QuadPart / 1000.0);
		return ret;
	};

	#endif*/
};

class IPSBenchmarker {
public:
	IPSBenchmarker() {};
    static IPSBenchmarker& Instance() {
        static IPSBenchmarker singleton;
        return singleton;
    };
	struct Benchmark{
		char* name;
		Vector<F32> CPS;
	};
	
	Vector<Benchmark> benches;
	void addBenchMark(F64 CPS, char* name) {
		bool found = false;
		for(int i = 0; i < benches.size(); i++)
		{
			if(strcmp(name, benches[i].name) == 0)
			{
				found = true;
				benches[i].CPS.push_back(CPS);
			}
		}
		if(found == false)
		{
			Benchmark b;
			b.name = name;
			b.CPS.push_back(CPS);
			benches.push_back(b);
		}
	};

	void printBenchMark(){
		std::ofstream myfile("IPSBenchmark.txt");
		if(myfile.is_open())
		{
			for(int i  = 0; i < benches.size(); i++) { 
				F64 average = 0; 
				for(int idx = 0; idx < benches[i].CPS.size(); idx++) 
					average += benches[i].CPS[idx];
				average /= benches[i].CPS.size();
				myfile << "Test: " << (benches[i].name) << ", " << average << "ms. Code was run: " << benches[i].CPS.size() << " times.\n";
			}
			myfile.close();
		}
	};
};

//#define Benchmark
#ifdef Benchmark
#	define IPSBenchmarkBegin UINT64 startTime = IPSCore::get_time();
#	define IPSBenchmarkEnd(name) IPSBenchmarker::Instance().addBenchMark(IPSCore::get_time(startTime), name);
#	define PrintBenchMarks IPSBenchmarker::Instance().printBenchMark();
#else
#	define IPSBenchmarkBegin
#	define IPSBenchmarkEnd
#	define PrintBenchMarks
#endif

__interface IParticleEmitter{
   bool isDead();
   const Box3F& getRenderWorldBox();
   Box3F getParentNodeBox();
   ColorF* getColors();
   F32* getSizes();
   MatrixF getBBObjToWorld();
   Point3F getLastPosition();
   Point3F getParentNodePos();
   const F32 getAgedSpinToRadians();
   SimObject* getPointer();
};

__interface IParticleEmitterData{
   bool isRenderingReflections();
   bool isHighResOnly();
   bool isOrientedOnVelocity();
   bool useEmitterColors();
   bool useEmitterSizes();
   bool useReversedOrder();
   bool useSortedParticles();
   bool useAlignedParticles();
   bool useOrientedParticles();
   U32 getPartListInitSize();
   S32 getBlendStyle();
   F32 getMediumResDistance();
   F32 getLowResDistance();
   F32 getSoftnessDistance();
   F32 getAmbientFactor();
   VectorF getAlignDirection();
   GFXTexHandle getTextureHandle();
   GFXPrimitiveBufferHandle getPrimBuff();
   GFXPrimitiveBufferHandle* getPrimBuffRef();
   void allocPrimBuffer( S32 overrideSize = -1 );
   SimObject* getPointer();
};

#endif