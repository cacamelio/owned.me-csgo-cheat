#pragma once

enum weapon_type_legit {
	autos = 0,
	scout,
	awp,
	deagle,
	revolver,
	pistols,
	rifles,
	shotguns,
	smgs
};

namespace legitbot
{
	void work(UserCmd* cmd);

	void run_aim(UserCmd* cmd);

    void run_trigger(UserCmd* pCmd);

    void setup_weapon();

	inline triggerbot_t trigger_settings = triggerbot_t();
	inline legitbot_t legit_settings = legitbot_t();

	inline int killDelay = 0;
}

struct sequence_object_t {
	sequence_object_t(int iInReliableState, int iOutReliableState, int iSequenceNr, float flCurrentTime)
		: iInReliableState(iInReliableState), iOutReliableState(iOutReliableState), iSequenceNr(iSequenceNr), flCurrentTime(flCurrentTime) { }

	int iInReliableState;
	int iOutReliableState;
	int iSequenceNr;
	float flCurrentTime;
};

namespace legit_records
{
	void update(int stage);
	void run(UserCmd* cmd);

	void updateSequences(INetChannel* netChannel);
	void clearSequences();
	
	void addLatency(INetChannel* netChannel, float latency);

	inline std::deque<sequence_object_t> sequencesVectors = { };

	inline int lastSequences = 0;

	struct storedRecords {
		vec3_t hitbox;
		vec3_t origin;
		float simTime;
		matrix_t matrix[128];
	};

	inline std::deque<storedRecords> records[65];

	constexpr auto lerp_time() {
		auto ratio = std::clamp(convars::interp_ratio->GetFloat(), convars::min_interp_ratio->GetFloat(), convars::max_interp_ratio->GetFloat());

		return std::max(convars::interp->GetFloat(), (ratio / ((convars::max_update_rate) ? convars::max_update_rate->GetFloat() : convars::update_rate->GetFloat())));
	}

	inline auto valid(float simtime) {
		auto network = interfaces::engine->get_net_channel_info();
		if (!network)
			return false;

		float server_time = g::pLocalPlayer->GetTickBase() * interfaces::globals->interval_per_tick;

		auto delta = std::clamp(network->get_latency(FLOW_OUTGOING) + network->get_latency(FLOW_INCOMING) + lerp_time(), 0.f, convars::max_unlag->GetFloat()) - (server_time - simtime);
		return std::fabsf(delta) <= 0.2f;
	}

	constexpr auto time_to_ticks(float time) {
		return static_cast<int>(0.5f + time / interfaces::globals->interval_per_tick);
	}

	static void init() {
		records->clear();
	}
}