#pragma once
#include"Timeline.h"
#include"MainlineKey.h"
#include"Folder.h"
enum AnimationLoopType{ NO_LOOPING, LOOPING };
class Animation {
public:
	std::string name;
	int length;
	int loopType = LOOPING;
	std::vector<MainlineKey*>mainlineKeys;
	std::vector<Timeline*>timelines;

	SpatialInfo*characterInfo;

	std::vector<BoneTimelineKey>currentBoneKeysToDraw;
	std::vector<SpriteTimelineKey>currentSpriteKeysToDraw;

	Animation(SpatialInfo*characterInfo);
	~Animation();
	void Log();

	void SetCurrentTime(float newTime);
	void Draw(const std::vector<Folder*>&folders);
	void UpdateCharacter(MainlineKey*mainKey, int newTime);
	MainlineKey* MainlineKeyFromTime(int time);

	void KeyFromRef(TimelineKey*pKey, Ref ref, int newTime);

	//TimelineKey* KeyFromRef(Ref ref, int newTime);

};
