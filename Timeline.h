#pragma once 
#include<vector>
#include<string>
#include"TimelineKey.h"
enum TimelineObjectType {SPRITE, BONE, BOX, POINT, SOUND, ENTITY, VARIABLE};
class Timeline {
public:
	std::string name;
	int objectType;
	std::vector<TimelineKey*> keys;

	~Timeline() {
		for (auto&timelineKey : keys)
			delete timelineKey;
	}
	void Log() {
		printf("\t\ttimeline %s %d\n", name.c_str(), objectType);
		for (auto&timelineKey : keys)
			timelineKey->Log();
	}
};
