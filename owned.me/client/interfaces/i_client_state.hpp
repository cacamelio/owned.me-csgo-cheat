#pragma once

#pragma pack(push, 1)

#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

class INetChannel_info {
public:
	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,	// all other entity bytes
		SOUNDS,		// game sounds
		EVENTS,		// event messages
		pUserMESSAGES,	// pUser messages
		ENTMESSAGES,	// entity messages
		VOICE,		// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,		// client move cmds
		STRINGCMD,	// string command
		SIGNON,		// various signondata
		TOTAL,		// must be last and is not a real group
	};

	virtual const char* 	get_name(void) const = 0;	// get channel name
	virtual const char* 	get_address(void) const = 0; // get channel IP address as string
	virtual float		get_time(void) const = 0;	// current net time
	virtual float		get_time_connected(void) const = 0;	// get connection time in seconds
	virtual int		get_buffer_size(void) const = 0;	// netchannel packet history size
	virtual int		get_data_rate(void) const = 0; // send data rate in byte/sec

	virtual bool		is_loop_back(void) const = 0;	// true if loopback channel
	virtual bool		is_timing_out(void) const = 0;	// true if timing out
	virtual bool		is_play_back(void) const = 0;	// true if demo playback

	virtual float		get_latency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		get_average_latency(int flow) const = 0; // average packet latency in seconds
	virtual float		get_average_loss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		get_average_choke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		get_average_data(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		get_average_packets(int flow) const = 0; // avg packets/sec
	virtual int		get_total_data(int flow) const = 0;	 // total flow in/out in bytes
	virtual int		get_sequence_number(int flow) const = 0;	// last send seq number
	virtual bool		is_valid_packet(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		get_packet_time(int flow, int frame_number) const = 0; // time when packet was send
	virtual int		get_packet_bytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		get_stream_progress(int flow, int* received, int* total) const = 0;  // TCP progress if transmitting
	virtual float		get_since_last_time_recieved(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		get_command_interpolation_ammount(int flow, int frame_number) const = 0;
	virtual void		get_packet_response_latency(int flow, int frame_number, int* latency_msecs, int* choke) const = 0;
	virtual void		get_remote_framerate(float* frame_time, float* frame_time_std_deviation) const = 0;

	virtual float		get_timeout_seconds() const = 0;
};

class IClientState {
public:

	void full_update() {
		m_delta_tick = -1;
	}

	std::byte pad0[0x9C]; //0x0000
	INetChannel* net_channel; //0x009C
	int m_challenge_nr; //0x00A0
	std::byte pad1[0x64]; //0x00A4
	int m_signon_state;  //0x0108
	std::byte pad2[0x8]; //0x010C
	float m_next_cmd_time; //0x0114
	int m_server_count; //0x0118
	int m_current_seqeunce; //0x011C
	char _0x0120[8];

	struct {
		float        clock_offsets[16];
		int            cur_clock_offset;
		int            m_server_tick;
		int            m_client_tick;
	} m_clock_drift_mgr; //0x0124 

	int m_delta_tick; //0x0174
	bool m_paused; //0x0178
	std::byte pad4[0x7]; //0x0179
	int m_view_entity; //0x0180
	int m_player_slot; //0x0184
	char m_level_name[260]; //0x0188
	char m_level_name_short[80]; //0x028C
	char m_group_name[80]; //0x02DC
	std::byte pad5[0x5C]; //0x032C
	int m_max_clients;  //0x0388
	std::byte pad6[0x4984]; //0x038C
	float m_last_server_tick_time; //0x4D10
	bool m_in_simulation; //0x4D14
	std::byte pad7[0xB]; //0x4D15
	int m_old_tickcount; //0x4D18
	float m_tick_remainder; //0x4D1C
	float m_frametime; //0x4D20
	int m_last_outgoing_command; //0x4D38
	int choked_commands; //0x4D30
	int m_last_command_ack; //0x4D2C
	int m_command_ack; //0x4D30
	int m_sound_sequence; //0x4D34
	std::byte pad8[0x50]; //0x4D38
	vec3_t m_viewangles; //0x4D88
	std::byte pad9[204]; //0x4D9A
	event_t* events;
};

#pragma pack(pop)
