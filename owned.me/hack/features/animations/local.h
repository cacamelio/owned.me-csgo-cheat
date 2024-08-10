#pragma once

class localAnims {
public:
	void work();
	void setupShotPos();
	void doAnimEvent(int type);
	bool getCachedMatrix(matrix_t* matrix);
	void onUpdateClientAnims();

	void resetData();

	std::array <matrix_t, MAXSTUDIOBONES> getDesyncMatrix();
	std::array <matrix_t, MAXSTUDIOBONES> getLagMatrix();
	std::array <AnimationLayer, 13> getAnimLayers();
	std::array <AnimationLayer, 13> getFakeAnimLayers();

	struct {
		std::array <AnimationLayer, 13> animLayers;
		std::array <AnimationLayer, 13> fakeLayers;

		std::array < float, 24 > poseParams;
		std::array < float, 24 > fakePoseParams;
		CSGOPlayerAnimState fakeAnimState;

		float spawnTime = 0.0f;
		float loweBodyYaw = 0.0f;
		float nextLBYupdate = 0.0f;

		std::array < int, 2 > moveType;
		std::array < int, 2 > iFlags;

		std::array< vec3_t, MAXSTUDIOBONES > vecBoneOrig;
		std::array< vec3_t, MAXSTUDIOBONES > fakeBoneOrig;

		vec3_t vecShootPos = vec3_t(0, 0, 0);

		bool didShotinCycle = false;
		vec3_t angShotChoked = vec3_t(0, 0, 0);

		float shotTime = 0.0f;
		vec3_t angForcedAngles = vec3_t(0, 0, 0);

		std::array <matrix_t, MAXSTUDIOBONES> mainBones;
		std::array <matrix_t, MAXSTUDIOBONES> desyncBones;
		std::array <matrix_t, MAXSTUDIOBONES> lagBones;
	}localData;
};

inline localAnims* g_localanims = new localAnims();