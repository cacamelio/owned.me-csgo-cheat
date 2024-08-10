#pragma once
#include "../../sdk/classes/c_usercmd.hpp"

#define MULTIPLAYER_BACKUP 150

class IInput {
public:
	std::byte            pad0[0xC];            // 0x0
	bool                m_fTrackIRAvailable;        // 0xC
	bool                m_fMouseInitialized;        // 0xD
	bool                m_fMouseActive;            // 0xE
	std::byte            pad1[0x9A];            // 0xF
	bool                m_bCameraInThirdPerson;    // 0xA9
	std::byte            pad2[0x2];            // 0xAA
	vec3_t                m_vecCameraOffset;        // 0xAC
	std::byte            pad3[0x38];            // 0xB8
	UserCmd* m_pCommands;
	VerifiedUserCmd* m_pVerifiedCommands;

	//UserCmd *get_pUser_cmd( int slot, int sequence_num ) {
	//	using fn = UserCmd * ( __thiscall * )( void *, int, int );
	//	return ( *( fn ** ) this )[ 8 ]( this, slot, sequence_num );
	//}

	UserCmd* get_pUser_cmd(int sequenceNum) {
		return &m_pCommands[sequenceNum % 150];
	}

	VerifiedUserCmd* get_pUser_verifCmd(int sequence){
		return &m_pVerifiedCommands[sequence % 150];
	}
};
