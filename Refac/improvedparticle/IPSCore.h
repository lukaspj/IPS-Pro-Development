//-----------------------------------------------------------------------------
// IPS Pro
// Copyright Lukas Jørgensen 2012 - FuzzyVoidStudio
// License:
// 'the product' refers to the IPS Pro.
// If you have not paid for the product you are not allowed to use it
//  - personally nor commercially.
// Assuming you have paid for the product you are allowed to
//  - include this in any commercial product as long as the IPS Pro
//  - source code is never released along with the commercial product.
// You may not resell or redistribute the product.
// You'll need one license for each man on your team, unless you fall under
//  - the indie license.
// You may not use the code to create new products in the same category.
//  - e.g. you may not use this code to construct a mathEmitter and sell that
//  - as a new product. Not if it is released for another GameEngine either.
// The indie license: As long as your products using IPS Pro has a revenue of
//  - less than 40.000$ annually you fall under this license.
// Email me at LukasPJ@FuzzyVoidStudio.com 
//  - if you have further questions regarding license
// http://fuzzyvoidstudio.com
//-----------------------------------------------------------------------------

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
#ifdef _WIN32
	#include <windows.h>
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

	static IPSCore::intersection raySphereIntersection(Point3F center, F32 radius, Point3F from, Point3F to);
	static IPSCore::intersection lineSphereIntersection(Point3F center, F32 radius, Point3F from, Point3F to);
	static void triSphereIntersection(StringTableEntry object, face* tri, Point3F center, F32 radius, Point3F p1, Point3F p2, Point3F p3);
	static F32 HeronsF(Point3F a, Point3F b, Point3F c);
	static F32 AngleBetween(Point3F N, Point3F V1, Point3F V2);
	static F32 ChordHeight(Point3F p1, Point3F p2, Point3F N, Point3F center, F32 radius);
	static F32 ChordHeight(Point3F p1, Point3F p2, Point3F N, Point3F center, F32 radius, F32 coefficient);
	static float square( float f ) { return (f*f); };
	static Point3F perpendicularVector(Point3F V1, Point3F N);
	static Point3F project(Point3F V1, Point3F V2);
	static F32 AreaOfCircularSegment(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd);
	static bool IsPointInArc(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd, Point3F point);
	static bool SameSideOfLine(Point3F lineStart, Point3F lineEnd, Point3F p1, Point3F p2);
	static Point3F TopOfCircularSegment(Point3F center, F32 radius, Point3F chordStart, Point3F chordEnd);
	static F32 getClockTime() { return ((F32)clock())/CLOCKS_PER_SEC; };

	static char* UpToLow(char* str) {
		for (int i=0;i<strlen(str);i++) 
			str[i] = tolower(str[i]);
		return str;
	}

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
		UINT64 Utick, delta, Ufreq;
		if( QueryPerformanceCounter(&t) == false )
			Con::printf("Query failed!!");
		if( QueryPerformanceFrequency(&f) == false )
			Con::printf("Query failed!!!");
		Utick = t.QuadPart;
		delta = Utick - base;
		F64 ret = ((F64)delta)/((F64)f.QuadPart / 1000.0);
		return ret;
	};
	/*
	#else

	#include <sys/time.h>
	#include <sys/resource.h>

	double get_time()
	{
		struct timeval t;
		struct timezone tzp;
		gettimeofday(&t, &tzp);
		return t.tv_sec + t.tv_usec*1e-6;
	}*/

	#endif
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

#define Benchmark
#ifdef Benchmark
#	define IPSBenchmarkBegin UINT64 startTime = IPSCore::get_time();
#	define IPSBenchmarkEnd(name) IPSBenchmarker::Instance().addBenchMark(IPSCore::get_time(startTime), name);
#	define PrintBenchMarks IPSBenchmarker::Instance().printBenchMark();
#else
#	define IPSBenchmarkBegin
#	define IPSBenchmarkEnd
#	define PrintBenchMarks
#endif


#endif