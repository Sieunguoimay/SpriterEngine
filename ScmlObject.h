#pragma once
#include"Folder.h"
#include"ScmlEntity.h"
class ScmlObject {
public:

	SpatialInfo characterInfo;
	int currentEntity;
	int currentAnimation;
	float currentTime;

	std::vector<Folder*>folders;
	std::vector<ScmlEntity*>entities;

	std::vector<Folder*>activeCharacterMap;




	ScmlObject();
	~ScmlObject();
	void Log();
	void Draw();
	void LoadFile(const char*string);
	void ApplyCharacterMap(CharacterMap*charMap,bool reset);
	void SetCurrentTime(float newTime);
};
