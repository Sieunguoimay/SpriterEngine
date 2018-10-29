#pragma once
#include<vector>
class Ref {
public:
	int parent = -1;
	int timeline;
	int key;
	//int z_index;
};
class MainlineKey {
public:
	int time = 0;
	std::vector<Ref> boneRefs;
	std::vector<Ref> objectRefs;
	void Log() {
		printf("\t\tmain_line key: %d\n", time);
		for (auto&ref : boneRefs)
			printf("\t\t\tbone_ref %d %d %d\n", ref.parent, ref.timeline, ref.key);
		for (auto&ref : objectRefs)
			printf("\t\t\tobject_ref %d %d %d\n", ref.parent, ref.timeline, ref.key);
	}
};
