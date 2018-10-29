#include "TimelineKey.h"
#include<glm\glm.hpp>
float _linear(float a, float b, float t)
{
	return ((b - a)*t) + a;
}


float angle_linear(float angleA, float angleB, int spin, float t)
{
	if (spin == 0)
	{
		return angleA;
	}
	if (spin>0)
	{
		if ((angleB - angleA)<0)
		{
			angleB += 360;
		}
	}
	else if (spin<0)
	{
		if ((angleB - angleA)>0)
		{
			angleB -= 360;
		}
	}

	return _linear(angleA, angleB, t);
}
SpatialInfo _linear(SpatialInfo infoA, SpatialInfo infoB, int spin, float t)
{
	SpatialInfo resultInfo;
	resultInfo.x = _linear(infoA.x, infoB.x, t);
	resultInfo.y = _linear(infoA.y, infoB.y, t);
	resultInfo.angle = angle_linear(infoA.angle, infoB.angle, spin, t);
	resultInfo.scaleX = _linear(infoA.scaleX, infoB.scaleX, t);
	resultInfo.scaleY = _linear(infoA.scaleY, infoB.scaleY, t);
	resultInfo.a = _linear(infoA.a, infoB.a, t);
	return resultInfo;
}

float quadratic(float a, float b, float c, float t)
{
	return _linear(_linear(a, b, t), _linear(b, c, t), t);
}

float cubic(float a, float b, float c, float d, float t)
{
	return _linear(quadratic(a, b, c, t), quadratic(b, c, d, t), t);
}






#include<iostream>
void TimelineKey::Log() {
	printf("\t\t\tkey %d %d %d %f %f\n", time, spin, curveType, c1, c2);
}
void BoneTimelineKey::Log() {
	TimelineKey::Log();
	printf("\t\t\tbone %f %f %f %f %f %f --- %d %d\n", info.x, info.y, info.angle, info.scaleX, info.scaleY,info.a, length, height);
}
void SpriteTimelineKey::Log() {
	TimelineKey::Log();
	printf("\t\t\tobject %f %f %f %f %f %f --- %d %d %f %f\n", info.x, info.y, info.angle, info.scaleX, info.scaleY, info.a, folder, file, pivot_x, pivot_y);
}



void TimelineKey::Interpolate(TimelineKey*movingKey, TimelineKey*nextKey, int nextKeyTime, float currentTime)
{
	float t = GetTWithNextKey(nextKey, nextKeyTime, currentTime);
	Linear(movingKey,nextKey, t);
}


float TimelineKey::GetTWithNextKey(TimelineKey*nextKey, int nextKeyTime, float currentTime)
{
	if (curveType == INSTANT || time == nextKey->time)
	{
		return 0;
	}

	float t = (currentTime - (float)time) / (float)(nextKey->time - time);
	if (curveType == LINEAR)
	{

		return t;
	}
	else if (curveType == QUADRATIC)
	{
		return(quadratic(0.0,c1,1.0,t));
	}
	else if (curveType == CUBIC)
	{
		return(cubic(0.0,c1,c2,1.0,t));
	}

	return 0; // Runtime should never reach here        
}









SpatialInfo SpatialInfo::unmapFromParent(SpatialInfo parentInfo)
{

	SpatialInfo unmappedObj = *this;
	unmappedObj.angle += parentInfo.angle;
	unmappedObj.scaleX *= parentInfo.scaleX;
	unmappedObj.scaleY *= parentInfo.scaleY;
	unmappedObj.a *= parentInfo.a;

	if (x != 0 || y != 0)
	{

		float preMultX = x*parentInfo.scaleX;
		float preMultY = y*parentInfo.scaleY;
		float s = sin(glm::radians(parentInfo.angle));
		float c = cos(glm::radians(parentInfo.angle));

		unmappedObj.x = (preMultX * c) - (preMultY * s);
		unmappedObj.y = (preMultX * s) + (preMultY * c);
		unmappedObj.x += parentInfo.x;
		unmappedObj.y += parentInfo.y;
	}
	else
	{
		// Mandatory optimization for future features           
		unmappedObj.x = parentInfo.x;
		unmappedObj.y = parentInfo.y;
	}

	return unmappedObj;
}


#include"ScmlObject.h"
#include"../../misc/Locator.h"
void SpriteTimelineKey::Paint(const std::vector<Folder*>&folders)
{
	int paintPivotX;
	int paintPivotY;
	File textureFile = folders.at(folder)->files[file];
	Texture*pTexture = textureFile.pTexture;
	if (pTexture == NULL) return;

	if (useDefaultPivot)
	{
		//paintPivotX = ScmlObject::activeCharacterMap[folder]->files[file].pivotX;
		//paintPivotY = ScmlObject::activeCharacterMap[folder]->files[file].pivotY;

		paintPivotX = textureFile.pivotX;
		paintPivotY = textureFile.pivotY;
	}
	else
	{
		paintPivotX = pivot_x;
		paintPivotY = pivot_y;
	}

	// paint image represented by
	// ScmlObject.activeCharacterMap[folder].files[file],fileReference 
	// at x,y,angle (counter-clockwise), offset by paintPivotX,paintPivotY

	float w = info.scaleX*((float)pTexture->GetWidth());
	float h = info.scaleY*((float)pTexture->GetHeight());

	//since pivot in spriter is at the top-left
	//thankfully we have faced this problem before, so we just give the pivot
	//position to the Draw() function as the center of rotating

	glm::vec2 pivot(w*(float)paintPivotX, h*(float)paintPivotY);

	Locator::GetTextureRenderer()->Draw(
		glm::vec4(info.x,info.y-h, w, h),
		glm::vec4(0.0, 0.0, 1.0, 1.0), 
		pTexture->GetId(), 
		0, glm::vec4(1.0f), glm::radians(info.angle),pivot);
}

void BoneTimelineKey::Paint(const std::vector<Folder*>&folders)
{
	if (paintDebugBones)
	{
		float drawLength = (float)length*info.scaleX;
		float drawHeight = (float)height*info.scaleY;
		// paint debug bone representation 
		// e.g. line starting at x,y,at angle, 
		// of length drawLength, and height drawHeight

		//printf("%f  ", info.angle);
		float s = glm::sin(glm::radians(info.angle));
		float c = glm::cos(glm::radians(info.angle));

		glm::vec2 p1(info.x, info.y);
		glm::vec2 p2 = p1 + glm::vec2(c*drawLength, s*drawLength);
		Locator::GetPrimitiveRenderer()->DrawLine(p1,p2);

	}
}


void BoneTimelineKey::Linear(TimelineKey*_movingKey,TimelineKey*_keyB, float t)
// keyB must be BoneTimelineKeys
{
	BoneTimelineKey*keyB = (BoneTimelineKey*)_keyB;
	BoneTimelineKey*movingKey = (BoneTimelineKey*)_movingKey;
	*movingKey = *this;

	movingKey->info = _linear(info, keyB->info, spin, t);
	if (paintDebugBones)
	{
		movingKey->length = _linear((float)length, (float)keyB->length, t);
		movingKey->height = _linear((float)height, (float)keyB->height, t);
	}

	//BoneTimelineKey*returnKey = new BoneTimelineKey(*this);
	//BoneTimelineKey*keyB = (BoneTimelineKey*)_keyB;
	//returnKey->info = _linear(info, keyB->info, spin, t);
	////printf("time t %d %f    ", spin,t);
	//if (paintDebugBones)
	//{
	//	returnKey->length = _linear((float)length, (float)keyB->length, t);
	//	returnKey->height = _linear((float)height, (float)keyB->height, t);
	//}

	//return returnKey;
}

void BoneTimelineKey::Clone(TimelineKey * target)
{
	*((BoneTimelineKey*)target) = *this;
}

void SpriteTimelineKey::Linear(TimelineKey*movingKey,TimelineKey*keyB, float t)
{
	SpriteTimelineKey*_keyB = ((SpriteTimelineKey*)keyB);
	SpriteTimelineKey*_movingKey = ((SpriteTimelineKey*)movingKey);
	*_movingKey = *this;
	_movingKey->info = _linear(info, _keyB->info, spin, t);
	if (!useDefaultPivot)
	{
		_movingKey->pivot_x = _linear(pivot_x, _keyB->pivot_x, t);
		_movingKey->pivot_y = _linear(pivot_y, _keyB->pivot_y, t);
	}
}

void SpriteTimelineKey::Clone(TimelineKey * target)
{
	*((SpriteTimelineKey*)target) = *this;
}


