#pragma once
#include<iostream>
#include"Folder.h"

enum  CurveType { INSTANT, LINEAR, QUADRATIC, CUBIC };
class TimelineKey
{
public:
	int time = 0;
	int spin = 1;
	int curveType = LINEAR; // enum : INSTANT,LINEAR,QUADRATIC,CUBIC
	float c1;
	float c2;

	virtual ~TimelineKey() {}
	virtual void Log();

	//TimelineKey*Interpolate(TimelineKey*nextKey, int nextKeyTime, float currentTime);
	void Interpolate(TimelineKey*movingKey,TimelineKey*nextKey, int nextKeyTime, float currentTime);
	float GetTWithNextKey(TimelineKey*nextKey, int nextKeyTime, float currentTime);

	// overridden in inherited types  return linear(this,keyB,t);
	virtual void Linear(TimelineKey*movingKey,TimelineKey*keyB, float t) {  }
	virtual void Clone(TimelineKey*target) {};
};

class SpatialInfo
{
public:
	float x = 0;
	float y = 0;
	float angle = 0;
	float scaleX = 1;
	float scaleY = 1;
	float a = 1;

	SpatialInfo unmapFromParent(SpatialInfo parentInfo);
};



class SpatialTimelineKey : public TimelineKey
{
public:
	SpatialInfo info;
	virtual void Paint(const std::vector<Folder*>&folders) = 0;
};


class BoneTimelineKey: public SpatialTimelineKey
{
public:
	// unimplemented in Spriter
	int length = 50;
	int height = 5;
	bool paintDebugBones = true;

	~BoneTimelineKey() {}
	void Log()override;

	// override paint if you want debug visuals for bones
	void Paint(const std::vector<Folder*>&folders);
	void Linear(TimelineKey*movingKey, TimelineKey*keyB, float t);
	void Clone(TimelineKey*target) override;
};



class SpriteTimelineKey: public SpatialTimelineKey
{
public:
	int folder; // index of the folder within the ScmlObject
	int file;
	bool useDefaultPivot; // true if missing pivot_x and pivot_y in object tag
	float pivot_x = 0;
	float pivot_y = 1;

	~SpriteTimelineKey() {}
	void Log()override;

	void Paint(const std::vector<Folder*>&folders);
	void Linear(TimelineKey*movingKey, TimelineKey*keyB, float t);
	void Clone(TimelineKey*target) override;
};

extern float linear(float a, float b, float t);
extern SpatialInfo linear(SpatialInfo infoA, SpatialInfo infoB, int spin, float t);
extern float angleLinear(float angleA, float angleB, int spin, float t);
extern float quadratic(float a, float b, float c, float t);
extern float cubic(float a, float b, float c, float d, float t);