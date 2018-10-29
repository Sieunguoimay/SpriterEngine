#pragma once
#include"CharacterMap.h"
#include"Animation.h"
class ScmlEntity {
public:
	~ScmlEntity() {
		for (auto&characterMap : characterMaps)
			delete characterMap;
		for (auto&animation : animations)
			delete animation;
	}
	void Log() {
		printf("entity %s\n", name.c_str());
		for (auto&characterMap : characterMaps)
			characterMap->Log();
		for (auto&animation : animations)
			animation->Log();
	}
	std::string name;
	std::vector<CharacterMap*> characterMaps;
	std::vector<Animation*> animations;
};
