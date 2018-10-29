#include "ScmlObject.h"
#include"../../misc/tinyXML2/tinyxml2.h"
#include"../../misc/Assistances.h"
#include"../../misc/Locator.h"

ScmlObject::ScmlObject()
	:currentEntity(0)
	,currentAnimation(0)
{
}

ScmlObject::~ScmlObject()
{
	for (auto&entity : entities)
		delete entity;
	for (auto&folder : folders)
		delete folder;
}

void ScmlObject::LoadFile(const char * filepath)
{
	//<spriter_data> ----> scmlObject
	tinyxml2::XMLDocument doc;
	doc.LoadFile(filepath);

	tinyxml2::XMLElement*pRoot = doc.FirstChildElement("spriter_data");
	if (pRoot) {
		//<folder>
		tinyxml2::XMLElement*pFolder = pRoot->FirstChildElement("folder");
		while (pFolder) {

			Folder*folder = new Folder();
			folder->name = _atos(pFolder->Attribute("name"));
			tinyxml2::XMLElement*pFile = pFolder->FirstChildElement("file");
			//<file>
			while (pFile) {
				File file;
				const char* name = pFile->Attribute("name");
				if (name) {
					file.name = _atos(name);

					file.pTexture = Locator::GetAssets()->GetTexture(file.name);
				}
				file.pivotX = _atof(pFile->Attribute("pivot_x"));
				file.pivotY = _atof(pFile->Attribute("pivot_y"));

				folder->files.push_back(file);
				pFile = pFile->NextSiblingElement("file");
			}
			this->folders.push_back(folder);
			pFolder = pFolder->NextSiblingElement("folder");
		}
		//<entity>		
		tinyxml2::XMLElement*pEntity = pRoot->FirstChildElement("entity");
		while (pEntity) {
			ScmlEntity*entity = new ScmlEntity();
			entity->name = _atos(pEntity->Attribute("name"));


			//<character_map>
			tinyxml2::XMLElement*pCharacterMap = pEntity->FirstChildElement("character_map");
			while (pCharacterMap)
			{
				CharacterMap*characterMap = new CharacterMap();
				characterMap->name = pCharacterMap->Attribute("name");

				//<map>
				tinyxml2::XMLElement*pMap = pCharacterMap->FirstChildElement("map");
				while (pMap) {
					MapInstruction map;
					map.folder = _atoi(pMap->Attribute("folder"));
					map.file = _atoi(pMap->Attribute("file"));

					const char*targetFolder = pMap->Attribute("target_folder");
					if (targetFolder) map.tarFolder = _atoi(targetFolder);

					const char*targetFile = pMap->Attribute("target_file");
					if (targetFile) map.tarFile = _atoi(targetFile);

					characterMap->maps.push_back(map);
					pMap = pMap->NextSiblingElement("map");
				}


				entity->characterMaps.push_back(characterMap);
				pCharacterMap = pCharacterMap->NextSiblingElement("character_map");
			}


			//<animation>
			tinyxml2::XMLElement*pAnimation = pEntity->FirstChildElement("animation");
			while (pAnimation) {
				Animation *animation = new Animation(&this->characterInfo);

				animation->name = _atos(pAnimation->Attribute("name"));
				animation->length = _atoi(pAnimation->Attribute("length"));
				const char*looping = pAnimation->Attribute("looping");
				if (looping) animation->loopType = _atoi(looping);

				//<mainline>
				tinyxml2::XMLElement*pMainline = pAnimation->FirstChildElement("mainline");
				if (pMainline) {
					//<key>
					tinyxml2::XMLElement*pKey = pMainline->FirstChildElement("key");
					while (pKey) {
						MainlineKey*mainlineKey = new MainlineKey();
						mainlineKey->time = _atoi(pKey->Attribute("time"));
						//<bone_ref>
						tinyxml2::XMLElement*pBoneRef = pKey->FirstChildElement("bone_ref");
						while (pBoneRef) {
							Ref ref;

							const char*parent = pBoneRef->Attribute("parent");
							if (parent) ref.parent = _atoi(parent);

							ref.timeline = _atoi(pBoneRef->Attribute("timeline"));
							ref.key = _atoi(pBoneRef->Attribute("key"));
							//ref.z_index = _atoi(pBoneRef->Attribute("z_index"));

							mainlineKey->boneRefs.push_back(ref);
							pBoneRef = pBoneRef->NextSiblingElement("bone_ref");
						}

						//<object_ref>
						tinyxml2::XMLElement*pObjectRef = pKey->FirstChildElement("object_ref");
						while (pObjectRef) {
							Ref ref;
							const char*parent = pObjectRef->Attribute("parent");
							if (parent) ref.parent = _atoi(parent);

							ref.timeline = _atoi(pObjectRef->Attribute("timeline"));
							ref.key = _atoi(pObjectRef->Attribute("key"));

							mainlineKey->objectRefs.push_back(ref);
							pObjectRef = pObjectRef->NextSiblingElement("object_ref");
						}

						animation->mainlineKeys.push_back(mainlineKey);
						pKey = pKey->NextSiblingElement("key");
					}
				}

				//<timeline>
				tinyxml2::XMLElement*pTimeline = pAnimation->FirstChildElement("timeline");
				while (pTimeline) {

					Timeline*timeline = new Timeline;
					timeline->name = _atos(pTimeline->Attribute("name"));

					if (pTimeline->Attribute("object_type", "bone"))
						timeline->objectType = TimelineObjectType::BONE;



					if (timeline->objectType == TimelineObjectType::BONE) {
						//<key>
						tinyxml2::XMLElement*pKey = pTimeline->FirstChildElement("key");
						while (pKey) {
							BoneTimelineKey*m_boneTimelineKey = new BoneTimelineKey();
							const char*time = pKey->Attribute("time");
							if (time)m_boneTimelineKey->time = _atoi(time);

							const char*spin = pKey->Attribute("spin");
							if (spin)m_boneTimelineKey->spin = _atoi(spin);

							const char*curveType = pKey->Attribute("curve_type");
							if (curveType)m_boneTimelineKey->curveType = _atoi(curveType);

							m_boneTimelineKey->c1 = _atof(pKey->Attribute("c1"));
							m_boneTimelineKey->c2 = _atof(pKey->Attribute("c2"));


							//<bone>
							tinyxml2::XMLElement*pBone = pKey->FirstChildElement("bone");
							if (pBone) {
								const char*x = pBone->Attribute("x");
								if (x) m_boneTimelineKey->info.x = _atof(x);

								const char*y = pBone->Attribute("y");
								if (y) m_boneTimelineKey->info.y = _atof(y);

								const char*angle = pBone->Attribute("angle");
								if (angle) m_boneTimelineKey->info.angle = _atof(angle);

								const char*scaleX = pBone->Attribute("scale_x");
								if (scaleX) m_boneTimelineKey->info.scaleX = _atof(scaleX);

								const char*scaleY = pBone->Attribute("scale_x");
								if (scaleY)m_boneTimelineKey->info.scaleY = _atof(scaleY);

								const char*a = pBone->Attribute("a");
								if (a) m_boneTimelineKey->info.a = _atof(a);

								timeline->keys.push_back(m_boneTimelineKey);
							}
							pKey = pKey->NextSiblingElement("key");
						}
					}


					if (timeline->objectType == TimelineObjectType::SPRITE) {
						//<key>
						tinyxml2::XMLElement*pKey = pTimeline->FirstChildElement("key");
						while (pKey) {

							SpriteTimelineKey*m_spriteTimelineKey = new SpriteTimelineKey();

							const char*time = pKey->Attribute("time");
							if (time) m_spriteTimelineKey->time = _atoi(time);

							const char*spin = pKey->Attribute("spin");
							if (spin)m_spriteTimelineKey->spin = _atoi(spin);

							const char*curveType = pKey->Attribute("curve_type");
							if (curveType) m_spriteTimelineKey->curveType = _atoi(curveType);

							m_spriteTimelineKey->c1 = _atof(pKey->Attribute("c1"));
							m_spriteTimelineKey->c2 = _atof(pKey->Attribute("c2"));


							//<object>
							tinyxml2::XMLElement*pObject = pKey->FirstChildElement("object");
							if (pObject) {

								const char*x = pObject->Attribute("x");
								if (x) m_spriteTimelineKey->info.x = _atof(x);

								const char*y = pObject->Attribute("y");
								if (y) m_spriteTimelineKey->info.y = _atof(y);

								const char*angle = pObject->Attribute("angle");
								if (angle) m_spriteTimelineKey->info.angle = _atof(angle);

								const char*scaleX = pObject->Attribute("scale_x");
								if (scaleX) m_spriteTimelineKey->info.scaleX = _atof(scaleX);

								const char*scaleY = pObject->Attribute("scale_x");
								if (scaleY) m_spriteTimelineKey->info.scaleY = _atof(scaleY);

								const char*a = pObject->Attribute("a");
								if (a)m_spriteTimelineKey->info.a = _atof(a);

								const char*folder = pObject->Attribute("folder");
								if (folder)m_spriteTimelineKey->folder = _atoi(folder);

								const char*file = pObject->Attribute("file");
								if (file) m_spriteTimelineKey->file = _atoi(file);

								const char*pivot_x = pObject->Attribute("pivot_x");
								if (pivot_x) m_spriteTimelineKey->pivot_x = _atof(pivot_x);

								const char *pivot_y = pObject->Attribute("pivot_y");
								if (pivot_y) m_spriteTimelineKey->pivot_y = _atof(pivot_y);

								m_spriteTimelineKey->useDefaultPivot = !(pivot_x&&pivot_y);

								timeline->keys.push_back(m_spriteTimelineKey);
							}

							pKey = pKey->NextSiblingElement("key");
						}
					}
					animation->timelines.push_back(timeline);
					pTimeline = pTimeline->NextSiblingElement("timeline");
				}
				entity->animations.push_back(animation);
				pAnimation = pAnimation->NextSiblingElement("animation");
			}
			pEntity = pEntity->NextSiblingElement("entity");
			this->entities.push_back(entity);
		}
	}
	this->Log();
}

void ScmlObject::Log()
{
	printf("spriter_data\n\n");
	for (auto&folder : folders)
		folder->Log();
	for (auto&entity : entities)
		entity->Log();

}

//SpatialInfo ScmlObject::characterInfo()
//{
//	// Fill a SpatialInfo class with the 
//	// x,y,angle,etc of this character in game
//
//	// To avoid distortion the character keep 
//	// scaleX and scaleY values equal
//
//	// Make scaleX or scaleY negative to flip on that axis
//
//	// Examples (scaleX,scaleY)
//	// (1,1) Normal size
//	// (-2.5,2.5) 2.5x the normal size, and flipped on the x axis
//	return SpatialInfo();
//}

void ScmlObject::SetCurrentTime(float newTime)
{
	if (entities.empty())return;
	ScmlEntity* currentEnt = entities[currentEntity];
	Animation*currentAnim = currentEnt->animations[currentAnimation];
	currentAnim->SetCurrentTime(newTime);
}

void ScmlObject::Draw()
{
	if (entities.empty())return;
	ScmlEntity* currentEnt = entities[currentEntity];
	Animation*currentAnim = currentEnt->animations[currentAnimation];
	currentAnim->Draw(folders);
}

void ScmlObject::ApplyCharacterMap(CharacterMap * charMap, bool reset)
{
	if (reset)
	{
		for(auto&folder:folders)
			activeCharacterMap.push_back(folder);
	}
	for (auto&currentMap:charMap->maps)
	{
		if (currentMap.tarFolder>-1 && currentMap.tarFile>-1)
		{
			Folder*targetFolder = activeCharacterMap[currentMap.tarFolder];
			File targetFile = targetFolder->files[currentMap.tarFile];
			activeCharacterMap[currentMap.folder]->files[currentMap.file] = targetFile;
		}
	}
}
