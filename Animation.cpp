#include "Animation.h"
#include<glm\glm.hpp>
Animation::Animation(SpatialInfo * characterInfo)
	:characterInfo(characterInfo)
{
}
Animation::~Animation()
{
	for (auto&timeline : timelines)
		delete timeline;
	for (auto&mainlineKey : mainlineKeys)
		delete mainlineKey;
	printf("Deleted Animation\n");

}
void Animation::Log()
{
	printf("\tanimation %s %d %d\n", name.c_str(), length, loopType);

	for (auto&mainlineKey : mainlineKeys)
		mainlineKey->Log();
	for (auto&timeline : timelines)
		timeline->Log();
}

void Animation::SetCurrentTime(float newTime)
{
	if (loopType == NO_LOOPING)
	{
		newTime = glm::min(newTime, (float)length);
	}
	else if (loopType == LOOPING)
	{
		newTime = ((int)newTime)%length;
	}

	UpdateCharacter(MainlineKeyFromTime(newTime), newTime);
}

void Animation::Draw(const std::vector<Folder*>&folders)
{
	for (auto&currentKey : currentBoneKeysToDraw) 
		currentKey.Paint(folders);
	currentBoneKeysToDraw.clear();

	for (auto&currentKey : currentSpriteKeysToDraw)
		currentKey.Paint(folders);
	currentSpriteKeysToDraw.clear();
}

void Animation::UpdateCharacter(MainlineKey*mainKey, int newTime)
{
	std::vector<BoneTimelineKey> transformBoneKeys;
	int boneNum = mainKey->boneRefs.size();
	for (int b = 0; b < boneNum; b++)
	{
		SpatialInfo parentInfo;
		Ref currentRef = mainKey->boneRefs[b];
		if (currentRef.parent >= 0)
		{
			parentInfo = transformBoneKeys[currentRef.parent].info;
		}
		else
		{
			parentInfo = *characterInfo;
		}

		//handle the dynamic object
		//printf("%d \n", currentRef.key);

		BoneTimelineKey currentKey;
		KeyFromRef(&currentKey,currentRef, newTime);

		currentKey.info = currentKey.info.unmapFromParent(parentInfo);
		transformBoneKeys.push_back(currentKey);//copy
		currentBoneKeysToDraw.push_back(currentKey);

	}

	//std::vector<SpriteTimelineKey*> objectKeys;
	int objectNum = mainKey->objectRefs.size();
	for (int o = 0; o < objectNum; o++)
	{
		SpatialInfo parentInfo;
		Ref currentRef = mainKey->objectRefs[o];

		if (currentRef.parent >= 0)
		{
			parentInfo = transformBoneKeys[currentRef.parent].info;
		}
		else
		{
			parentInfo = *characterInfo;
		}

		SpriteTimelineKey currentKey;
		KeyFromRef(&currentKey,currentRef, newTime);
		currentKey.info = currentKey.info.unmapFromParent(parentInfo);
		//objectKeys.push_back(currentKey);
		currentSpriteKeysToDraw.push_back(currentKey);
	}

	// <expose objectKeys to api users to retrieve AND replace objectKeys>

	//for (auto&objectKey:objectKeys)
	//	objectKey->Paint();

}

MainlineKey * Animation::MainlineKeyFromTime(int currentTime)
{
	int currentMainKey = 0;
	for (int m = 0; m < mainlineKeys.size(); m++)
	{
		if (mainlineKeys[m]->time <= currentTime)
		{
			currentMainKey = m;
		}
		if (mainlineKeys[m]->time >= currentTime)
		{
			break;
		}
	}
	return mainlineKeys[currentMainKey];
}

void Animation::KeyFromRef(TimelineKey*pKey, Ref ref, int newTime)
{
	const Timeline*timeline = timelines.at(ref.timeline);
	TimelineKey*keyA = timeline->keys[ref.key];

	if (timeline->keys.size() == 1)
	{
		keyA->Clone(pKey);
		return;
	}

	int nextKeyIndex = ref.key + 1;

	if (nextKeyIndex >= timeline->keys.size())
	{
		if (loopType = LOOPING)
		{

			nextKeyIndex = 0;
		}
		else
		{
			keyA->Clone(pKey);
			return;
		}
	}

	TimelineKey*keyB = timeline->keys[nextKeyIndex];


	int keyBTime = keyB->time;

	if (keyBTime<keyA->time)
	{
		keyBTime = keyBTime + length;
	}

	keyA->Interpolate(pKey, keyB, keyBTime, newTime);
}
