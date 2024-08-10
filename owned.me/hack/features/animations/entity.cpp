#include "../features.hpp"

void Build_Matrix( BaseEntity* pPlayer, matrix_t* aMatrix, bool bSafeMatrix )
{
	float_t flCurTime = interfaces::globals->cur_time;
	float_t flRealTime = interfaces::globals->realtime;
	float_t flFrameTime = interfaces::globals->frame_time;
	float_t flAbsFrameTime = interfaces::globals->absolute_frametime;
	int32_t iFrameCount = interfaces::globals->frame_count;
	int32_t iTickCount = interfaces::globals->tick_count;
	float_t flInterpolation = interfaces::globals->interpolation_amount;

	interfaces::globals->cur_time = pPlayer->SimulationTime( );
	interfaces::globals->realtime = pPlayer->SimulationTime( );
	interfaces::globals->frame_time = interfaces::globals->interval_per_tick;
	interfaces::globals->absolute_frametime = interfaces::globals->interval_per_tick;
	interfaces::globals->frame_count = INT_MAX;
	interfaces::globals->tick_count = utilities::TIME_TO_TICKS( pPlayer->SimulationTime( ) );
	interfaces::globals->interpolation_amount = 0.0f;

	int32_t nClientEffects = pPlayer->m_nClientEffects( );
	int32_t nLastSkipFramecount = pPlayer->m_nLastSkipFramecount( );
	int32_t nOcclusionMask = pPlayer->m_nOcclusionMask( );
	int32_t nOcclusionFrame = pPlayer->m_nOcclusionFrame( );
	int32_t iEffects = pPlayer->Effects( );
	bool bJiggleBones = pPlayer->m_bJiggleBones( );
	bool bMaintainSequenceTransition = pPlayer->m_bMaintainSequenceTransition( );
	vec3_t vecAbsOrigin = pPlayer->AbsOrigin( );

	int32_t iMask = BONE_USED_BY_ANYTHING;
	if ( bSafeMatrix )
		iMask = BONE_USED_BY_HITBOX;

	pPlayer->InvalidateBoneCache( );
	pPlayer->BoneAccessor( ).m_ReadableBones = NULL;
	pPlayer->BoneAccessor( ).m_WritableBones = NULL;

	pPlayer->m_nOcclusionFrame( ) = 0;
	pPlayer->m_nOcclusionMask( ) = 0;
	pPlayer->m_nLastSkipFramecount( ) = 0;

	pPlayer->SetAbsOrigin( pPlayer->Origin( ) );

	pPlayer->Effects( ) |= EF_NOINTERP;
	pPlayer->m_nClientEffects( ) |= 2;
	pPlayer->m_bJiggleBones( ) = false;

	pPlayer->AnimOverlays( )[ 12 ].flWeight = 0.0f;

	g::bUpdateBones = true;
	pPlayer->SetupBones( aMatrix, MAXSTUDIOBONES, iMask, pPlayer->SimulationTime( ) );
	g::bUpdateBones = false;

	pPlayer->m_nClientEffects( ) = nClientEffects;
	pPlayer->m_bJiggleBones( ) = bJiggleBones;
	pPlayer->Effects( ) = iEffects;
	pPlayer->m_nLastSkipFramecount( ) = nLastSkipFramecount;
	pPlayer->m_nOcclusionFrame( ) = nOcclusionFrame;
	pPlayer->m_nOcclusionMask( ) = nOcclusionMask;
	pPlayer->SetAbsOrigin( vecAbsOrigin );

	interfaces::globals->cur_time = flCurTime;
	interfaces::globals->realtime = flRealTime;
	interfaces::globals->frame_time = flFrameTime;
	interfaces::globals->absolute_frametime = flAbsFrameTime;
	interfaces::globals->frame_count = iFrameCount;
	interfaces::globals->tick_count = iTickCount;
	interfaces::globals->interpolation_amount = flInterpolation;
}

void C_AnimationSync::Instance( int Stage )
{
	if ( Stage != FRAME_NET_UPDATE_END )
		return;

	for ( int32_t iPlayerID = 1; iPlayerID <= interfaces::globals->max_clients; iPlayerID++ )
	{
		BaseEntity* pPlayer = reinterpret_cast< BaseEntity* >( interfaces::entity_list->get_client_entity( iPlayerID ) );
		if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->TeamNum( ) == g::pLocalPlayer->TeamNum( ) )
		{
			m_ResolverData.m_AnimResoled[ iPlayerID ] = false;
			m_ResolverData.m_MissedShots[ iPlayerID ] = 0;
			m_ResolverData.m_LastMissedShots[ iPlayerID ] = 0;

			continue;
		}

		bool bHasPreviousRecord = false;
		if ( pPlayer->OldSimulationTime( ) >= pPlayer->SimulationTime( ) )
		{
			if ( pPlayer->OldSimulationTime( ) > pPlayer->SimulationTime( ) )
				this->UnmarkAsDormant( iPlayerID );

			continue;
		}

		auto& LagRecords = g::m_CachedPlayerRecords[ iPlayerID ];
		if ( LagRecords.empty( ) )
			continue;

		C_LagRecord PreviousRecord = m_PreviousRecord[ iPlayerID ];
		if ( utilities::TIME_TO_TICKS( fabs( pPlayer->SimulationTime( ) - PreviousRecord.m_SimulationTime ) ) <= 17 )
			bHasPreviousRecord = true;

		C_LagRecord& LatestRecord = LagRecords.front( );
		if ( this->HasLeftOutOfDormancy( iPlayerID ) )
			bHasPreviousRecord = false;

		if ( LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flCycle == PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flCycle )
			continue;

		LatestRecord.m_UpdateDelay = utilities::TIME_TO_TICKS( pPlayer->SimulationTime( ) - pPlayer->OldSimulationTime( ) );
		if ( LatestRecord.m_UpdateDelay > 17 )
			LatestRecord.m_UpdateDelay = 1;

		player_info_t PlayerInfo;
		interfaces::engine->get_player_info( iPlayerID, &PlayerInfo );

		if ( PlayerInfo.fakeplayer || LatestRecord.m_UpdateDelay < 1 )
			LatestRecord.m_UpdateDelay = 1;

		vec3_t vecVelocity = LatestRecord.m_Velocity;
		if ( bHasPreviousRecord )
		{
			if ( LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flPlaybackRate == PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flPlaybackRate )
				LatestRecord.m_UpdateDelay = ( LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flCycle - PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flCycle ) / ( LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flPlaybackRate * interfaces::globals->interval_per_tick );

			if ( LatestRecord.m_UpdateDelay > 17 )
				LatestRecord.m_UpdateDelay = 1;

			if ( PlayerInfo.fakeplayer || LatestRecord.m_UpdateDelay < 1 )
				LatestRecord.m_UpdateDelay = 1;

			vecVelocity = ( LatestRecord.m_Origin - this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_Origin ) * ( 1.0f / utilities::TICKS_TO_TIME( LatestRecord.m_UpdateDelay ) );

			float_t flWeight = 1.0f - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flWeight;
			if ( flWeight > 0.0f && flWeight < 1.0f )
			{
				float_t flPreviousRate = this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flPlaybackRate;
				float_t flCurrentRate = LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).flPlaybackRate;

				if ( flPreviousRate == flCurrentRate )
				{
					int32_t iPreviousSequence = this->GetPreviousRecord( pPlayer->EntIndex( ) ).m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).nSequence;
					int32_t iCurrentSequence = LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 11 ).nSequence;

					if ( iPreviousSequence == iCurrentSequence )
					{
						float_t flSpeedNormalized = ( flWeight / 2.8571432f ) + 0.55f;
						if ( flSpeedNormalized > 0.0f )
						{
							float_t flSpeed = flSpeedNormalized * pPlayer->GetMaxPlayerSpeed( );
							if ( flSpeed > 0.0f )
								if ( vecVelocity.length_2d( ) > 0.0f )
									vecVelocity = ( vecVelocity / vecVelocity.length( ) ) * flSpeed;
						}
					}
				}
			}

			LatestRecord.m_Velocity = vecVelocity;
		}

		std::array < AnimationLayer, 13 > AnimationLayers;
		std::array < float_t, 24 > PoseParameters;
		CSGOPlayerAnimState AnimationState;

		std::memcpy( AnimationLayers.data( ), pPlayer->AnimOverlays( ), sizeof( AnimationLayer ) * 13 );
		std::memcpy( PoseParameters.data( ), pPlayer->PoseParameters( ).data( ), sizeof( float_t ) * 24 );
		std::memcpy( &AnimationState, pPlayer->AnimState( ), sizeof( AnimationState ) );

		for ( int32_t i = ROTATE_LEFT; i <= ROTATE_RIGHT; i++ )
		{
			this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, i );

			std::memcpy( LatestRecord.m_AnimationLayers.at( i ).data( ), pPlayer->AnimOverlays( ), sizeof( AnimationLayer ) * 13 );

			std::memcpy( pPlayer->AnimOverlays( ), AnimationLayers.data( ), sizeof( AnimationLayer ) * 13 );

			if ( i < ROTATE_LOW_LEFT )
				Build_Matrix( pPlayer, LatestRecord.m_Matricies[ i ].data( ), true );

			std::memcpy( pPlayer->PoseParameters( ).data( ), PoseParameters.data( ), sizeof( float_t ) * 24 );
			std::memcpy( pPlayer->AnimState( ), &AnimationState, sizeof( AnimationState ) );
		}

		if ( !LatestRecord.m_bIsShooting )
		{
			if ( LatestRecord.m_UpdateDelay > 1 && bHasPreviousRecord )
			{
				LatestRecord.m_RotationMode = m_ResolverData.m_LastBruteSide[ iPlayerID ];
				if ( LatestRecord.m_Velocity.length_2d( ) > 1.0f )
				{
					float_t flLeftDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_LEFT ).at( 6 ).flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flPlaybackRate );
					float_t flLowLeftDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_LOW_LEFT ).at( 6 ).flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flPlaybackRate );
					float_t flLowRightDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_LOW_RIGHT ).at( 6 ).flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flPlaybackRate );
					float_t flRightDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_RIGHT ).at( 6 ).flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flPlaybackRate );
					float_t flCenterDelta = fabsf( LatestRecord.m_AnimationLayers.at( ROTATE_CENTER ).at( 6 ).flPlaybackRate - LatestRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flPlaybackRate );

					LatestRecord.m_bAnimResolved = false;
					{
						float flLastDelta = 0.0f;
						if ( flLeftDelta > flCenterDelta )
						{
							LatestRecord.m_RotationMode = ROTATE_LEFT;
							flLastDelta = flLastDelta;
						}

						if ( flRightDelta > flLastDelta )
						{
							LatestRecord.m_RotationMode = ROTATE_RIGHT;
							flLastDelta = flRightDelta;
						}

						LatestRecord.m_bAnimResolved = true;
					}

					bool bIsValidResolved = true;
					if ( bHasPreviousRecord )
					{
						if ( fabs( ( LatestRecord.m_Velocity - PreviousRecord.m_Velocity ).length_2d( ) ) > 5.0f || PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).flWeight >= 1.0f )
							bIsValidResolved = false;
					}

					m_ResolverData.m_AnimResoled[ iPlayerID ] = LatestRecord.m_bAnimResolved;
					if ( LatestRecord.m_bAnimResolved && LatestRecord.m_RotationMode != ROTATE_SERVER )
						m_ResolverData.m_LastBruteSide[ iPlayerID ] = LatestRecord.m_RotationMode;

					if ( LatestRecord.m_RotationMode == ROTATE_SERVER )
						LatestRecord.m_RotationMode = m_ResolverData.m_LastBruteSide[ iPlayerID ];
				}
				else
				{
					if ( m_ResolverData.m_LastBruteSide[ iPlayerID ] < 0 )
					{
						float_t flFeetDelta = math::AngleNormalize( math::AngleDiff( math::AngleNormalize( pPlayer->LowerBodyYaw( ) ), math::AngleNormalize( pPlayer->EyeAngles( ).y ) ) );
						if ( flFeetDelta > 0.0f )
							LatestRecord.m_RotationMode = ROTATE_LEFT;
						else
							LatestRecord.m_RotationMode = ROTATE_RIGHT;

						m_ResolverData.m_LastBruteSide[ iPlayerID ] = LatestRecord.m_RotationMode;
					}
				}

				m_ResolverData.m_AnimResoled[ iPlayerID ] = LatestRecord.m_bAnimResolved;
				if ( LatestRecord.m_RotationMode == m_ResolverData.m_BruteSide[ iPlayerID ] )
				{
					if ( m_ResolverData.m_MissedShots[ iPlayerID ] > 0 )
					{
						int iNewRotation = 0;
						switch ( LatestRecord.m_RotationMode )
						{
							case ROTATE_LEFT: iNewRotation = ROTATE_RIGHT; break;
							case ROTATE_RIGHT: iNewRotation = ROTATE_LEFT; break;
							case ROTATE_LOW_RIGHT: iNewRotation = ROTATE_LOW_LEFT; break;
							case ROTATE_LOW_LEFT: iNewRotation = ROTATE_LOW_RIGHT; break;
						}

						LatestRecord.m_RotationMode = iNewRotation;
					}
				}

				this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, LatestRecord.m_RotationMode );
			}
			else
				this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, ROTATE_SERVER );
		}
		else
			this->UpdatePlayerAnimations( pPlayer, LatestRecord, PreviousRecord, bHasPreviousRecord, ROTATE_SERVER );

		std::memcpy( pPlayer->AnimOverlays( ), AnimationLayers.data( ), sizeof( AnimationLayer ) * 13 );
		std::memcpy( LatestRecord.m_PoseParameters.data( ), pPlayer->PoseParameters( ).data( ), sizeof( float_t ) * 24 );

		Build_Matrix( pPlayer, LatestRecord.m_Matricies[ ROTATE_SERVER ].data( ), false );

		for ( int i = 0; i < MAXSTUDIOBONES; i++ )
			m_BoneOrigins[ iPlayerID ][ i ] = pPlayer->Origin( ) - LatestRecord.m_Matricies[ ROTATE_SERVER ][ i ].get_origin( );

		std::memcpy( m_CachedMatrix[ iPlayerID ].data( ), LatestRecord.m_Matricies[ ROTATE_SERVER ].data( ), sizeof( matrix_t ) * MAXSTUDIOBONES );

		this->UnmarkAsDormant( iPlayerID );
	}
}

void C_AnimationSync::UpdatePlayerAnimations( BaseEntity* pPlayer, C_LagRecord& LagRecord, C_LagRecord PreviousRecord, bool bHasPreviousRecord, int32_t iRotationMode )
{
	float_t flCurTime = interfaces::globals->cur_time;
	int nFrameCount = interfaces::globals->frame_count;
	float_t flFrameTime = interfaces::globals->frame_time;
	float_t flInterpolationAmount = interfaces::globals->interpolation_amount;

	float_t flLowerBodyYaw = LagRecord.m_LowerBodyYaw;
	float_t flDuckAmount = LagRecord.m_DuckAmount;
	int32_t iFlags = LagRecord.m_Flags;
	int32_t iEFlags = pPlayer->EFlags( );

	if ( this->HasLeftOutOfDormancy( pPlayer->EntIndex( ) ) )
	{
		float_t flLastUpdateTime = LagRecord.m_SimulationTime - interfaces::globals->interval_per_tick;
		if ( pPlayer->Flags( ) & FL_ONGROUND )
		{
			pPlayer->AnimState( )->m_bLanding = false;
			pPlayer->AnimState( )->m_bOnGround = true;

			float_t flLandTime = 0.0f;
			if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).flCycle > 0.0f &&
				LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).flPlaybackRate > 0.0f )
			{
				int32_t iLandActivity = pPlayer->SequenceActivity( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).nSequence );
				if ( iLandActivity == ACT_CSGO_LAND_LIGHT || iLandActivity == ACT_CSGO_LAND_HEAVY )
				{
					flLandTime = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).flCycle / LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).flPlaybackRate;
					if ( flLandTime > 0.0f )
						flLastUpdateTime = LagRecord.m_SimulationTime - flLandTime - interfaces::globals->interval_per_tick;
				}
			}

			LagRecord.m_Velocity.z = 0.0f;
		}
		else
		{
			float_t flJumpTime = 0.0f;
			if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).flCycle > 0.0f &&
				LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).flPlaybackRate > 0.0f )
			{
				int32_t iJumpActivity = pPlayer->SequenceActivity( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).nSequence );
				if ( iJumpActivity == ACT_CSGO_JUMP )
				{
					flJumpTime = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).flCycle / LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).flPlaybackRate;
					if ( flJumpTime > 0.0f )
						flLastUpdateTime = LagRecord.m_SimulationTime - flJumpTime - interfaces::globals->interval_per_tick;
				}
			}

			pPlayer->AnimState( )->m_bOnGround = false;
		}

		float_t flWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flWeight;
		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flPlaybackRate < 0.00001f )
			LagRecord.m_Velocity.Zero( );

		pPlayer->AnimState( )->m_flLastUpdateTime = flLastUpdateTime;
	}

	if ( bHasPreviousRecord )
	{
		pPlayer->AnimState( )->m_flStrafeChangeCycle = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).flCycle;
		pPlayer->AnimState( )->m_flStrafeChangeWeight = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).flWeight;
		pPlayer->AnimState( )->m_nStrafeSequence = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).nSequence;
		pPlayer->AnimState( )->m_flPrimaryCycle = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flCycle;
		pPlayer->AnimState( )->m_flMoveWeight = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flWeight;
		pPlayer->AnimState( )->m_flAccelerationWeight = PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 12 ).flWeight;
		std::memcpy( pPlayer->AnimOverlays( ), PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( AnimationLayer ) * 13 );
	}
	else
	{
		pPlayer->AnimState( )->m_flStrafeChangeCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).flCycle;
		pPlayer->AnimState( )->m_flStrafeChangeWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).flWeight;
		pPlayer->AnimState( )->m_nStrafeSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 7 ).nSequence;
		pPlayer->AnimState( )->m_flPrimaryCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flCycle;
		pPlayer->AnimState( )->m_flMoveWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 6 ).flWeight;
		pPlayer->AnimState( )->m_flAccelerationWeight = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 12 ).flWeight;
		std::memcpy( pPlayer->AnimOverlays( ), LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).data( ), sizeof( AnimationLayer ) * 13 );
	}

	if ( LagRecord.m_UpdateDelay > 1 )
	{
		int32_t iActivityTick = 0;
		int32_t iActivityType = 0;

		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).flCycle > 0.0f && PreviousRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).flWeight <= 0.0f )
		{
			int32_t iLandSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).nSequence;

			int32_t iLandActivity = pPlayer->SequenceActivity( iLandSequence );
			if ( iLandActivity == ACT_CSGO_LAND_LIGHT || iLandActivity == ACT_CSGO_LAND_HEAVY )
			{
				float_t flCurrentCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).flCycle;
				float_t flCurrentRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).flPlaybackRate;

				if ( flCurrentCycle > 0.0f && flCurrentRate > 0.0f )
				{
					float_t flLandTime = ( flCurrentCycle / flCurrentRate );
					if ( flLandTime > 0.0f )
					{
						iActivityTick = utilities::TIME_TO_TICKS( LagRecord.m_SimulationTime - flLandTime ) - 1;
						iActivityType = ACTIVITY_LAND;
					}
				}
			}
		}

		if ( LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).flCycle > 0.0f && LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).flWeight <= 0.0f )
		{
			int32_t iJumpSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).nSequence;

			int32_t iJumpActivity = pPlayer->SequenceActivity( iJumpSequence );
			if ( iJumpActivity == ACT_CSGO_JUMP )
			{
				float_t flCurrentCycle = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).flCycle;
				float_t flCurrentRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).flPlaybackRate;

				if ( flCurrentCycle > 0.0f && flCurrentRate > 0.0f )
				{
					float_t flJumpTime = ( flCurrentCycle / flCurrentRate );
					if ( flJumpTime > 0.0f )
					{
						iActivityTick = utilities::TIME_TO_TICKS( LagRecord.m_SimulationTime - flJumpTime ) - 1;
						iActivityType = ACTIVITY_JUMP;
					}
				}
			}
		}

		pPlayer->EFlags( ) &= ~0x1000;
		for ( int32_t iSimulationTick = 1; iSimulationTick <= LagRecord.m_UpdateDelay; iSimulationTick++ )
		{
			float_t flSimulationTime = PreviousRecord.m_SimulationTime + utilities::TICKS_TO_TIME( iSimulationTick );
			interfaces::globals->cur_time = flSimulationTime;
			interfaces::globals->frame_count = utilities::TIME_TO_TICKS( flSimulationTime );
			interfaces::globals->frame_time = interfaces::globals->interval_per_tick;
			interfaces::globals->interpolation_amount = 0.0f;

			pPlayer->DuckAmount( ) = Interpolate( PreviousRecord.m_DuckAmount, LagRecord.m_DuckAmount, iSimulationTick, LagRecord.m_UpdateDelay );
			pPlayer->Velocity( ) = Interpolate( PreviousRecord.m_Velocity, LagRecord.m_Velocity, iSimulationTick, LagRecord.m_UpdateDelay );
			pPlayer->AbsVelocity( ) = pPlayer->Velocity( );

			if ( iSimulationTick < LagRecord.m_UpdateDelay )
			{
				int32_t iCurrentSimulationTick = utilities::TIME_TO_TICKS( flSimulationTime );
				if ( iActivityType > ACTIVITY_NONE )
				{
					bool bIsOnGround = pPlayer->Flags( ) & FL_ONGROUND;
					if ( iActivityType == ACTIVITY_JUMP )
					{
						if ( iCurrentSimulationTick == iActivityTick - 1 )
							bIsOnGround = true;
						else if ( iCurrentSimulationTick == iActivityTick )
						{
							// reset animation layer
							pPlayer->AnimOverlays( )[ 4 ].flCycle = 0.0f;
							pPlayer->AnimOverlays( )[ 4 ].nSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).nSequence;
							pPlayer->AnimOverlays( )[ 4 ].flPlaybackRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 4 ).flPlaybackRate;

							// reset player ground state
							bIsOnGround = false;
						}

					}
					else if ( iActivityType == ACTIVITY_LAND )
					{
						if ( iCurrentSimulationTick == iActivityTick - 1 )
							bIsOnGround = false;
						else if ( iCurrentSimulationTick == iActivityTick )
						{
							// reset animation layer
							pPlayer->AnimOverlays( )[ 5 ].flCycle = 0.0f;
							pPlayer->AnimOverlays( )[ 5 ].nSequence = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).nSequence;
							pPlayer->AnimOverlays( )[ 5 ].flPlaybackRate = LagRecord.m_AnimationLayers.at( ROTATE_SERVER ).at( 5 ).flPlaybackRate;

							// reset player ground state
							bIsOnGround = true;
						}
					}

					if ( bIsOnGround )
						pPlayer->Flags( ) |= FL_ONGROUND;
					else
						pPlayer->Flags( ) &= ~FL_ONGROUND;
				}

				if ( iRotationMode )
				{
					LagRecord.m_BruteYaw = math::NormalizeAnglee( LagRecord.m_BruteYaw );
					switch ( iRotationMode )
					{
						case ROTATE_LEFT: LagRecord.m_BruteYaw = math::NormalizeAnglee( LagRecord.m_BruteYaw - 60.0f ); break;
						case ROTATE_RIGHT: LagRecord.m_BruteYaw = math::NormalizeAnglee( LagRecord.m_BruteYaw + 60.0f ); break;
					}

					pPlayer->AnimState( )->m_flFootYaw = LagRecord.m_BruteYaw;
				}
			}
			else
			{
				pPlayer->Velocity( ) = LagRecord.m_Velocity;
				pPlayer->AbsVelocity( ) = LagRecord.m_Velocity;
				pPlayer->DuckAmount( ) = LagRecord.m_DuckAmount;
				pPlayer->Flags( ) = LagRecord.m_Flags;
			}

			pPlayer->AnimState( )->m_nLastUpdateFrame = 1488;

			bool bClientSideAnimation = pPlayer->ClientSideAnimation( );
			pPlayer->ClientSideAnimation( ) = true;

			for ( int32_t iLayer = NULL; iLayer < 13; iLayer++ )
				pPlayer->AnimOverlays( )[ iLayer ].pOwner = pPlayer;

			g::bUpdateAnims = true;
			pPlayer->UpdateClientSideAnimation( );
			g::bUpdateAnims = false;

			pPlayer->ClientSideAnimation( ) = bClientSideAnimation;
		}
	}
	else
	{
		interfaces::globals->cur_time = LagRecord.m_SimulationTime;
		interfaces::globals->frame_count = utilities::TIME_TO_TICKS( LagRecord.m_SimulationTime );
		interfaces::globals->frame_time = interfaces::globals->interval_per_tick;
		interfaces::globals->interpolation_amount = 0.0f;

		pPlayer->Velocity( ) = LagRecord.m_Velocity;
		pPlayer->AbsVelocity( ) = LagRecord.m_Velocity;

		if ( iRotationMode )
		{
			LagRecord.m_BruteYaw = math::NormalizeAnglee( LagRecord.m_BruteYaw );
			switch ( iRotationMode )
			{
				case ROTATE_LEFT: LagRecord.m_BruteYaw = math::NormalizeAnglee( LagRecord.m_BruteYaw - 60.0f ); break;
				case ROTATE_RIGHT: LagRecord.m_BruteYaw = math::NormalizeAnglee( LagRecord.m_BruteYaw + 60.0f ); break;
			}

			pPlayer->AnimState( )->m_flFootYaw = LagRecord.m_BruteYaw;
		}

		pPlayer->AnimState( )->m_nLastUpdateFrame = 1488;

		bool bClientSideAnimation = pPlayer->ClientSideAnimation( );
		pPlayer->ClientSideAnimation( ) = true;

		for ( int32_t iLayer = NULL; iLayer < 13; iLayer++ )
			pPlayer->AnimOverlays( )[ iLayer ].pOwner = pPlayer;

		g::bUpdateAnims = true;
		pPlayer->UpdateClientSideAnimation( );
		g::bUpdateAnims = false;

		pPlayer->ClientSideAnimation( ) = bClientSideAnimation;
	}

	pPlayer->LowerBodyYaw( ) = flLowerBodyYaw;
	pPlayer->DuckAmount( ) = flDuckAmount;
	pPlayer->EFlags( ) = iEFlags;
	pPlayer->Flags( ) = iFlags;

	interfaces::globals->cur_time = flCurTime;
	interfaces::globals->frame_count = nFrameCount;
	interfaces::globals->frame_time = flFrameTime;
	interfaces::globals->interpolation_amount = flInterpolationAmount;
}

bool C_AnimationSync::GetCachedMatrix( BaseEntity* pPlayer, matrix_t* aMatrix )
{
	std::memcpy( aMatrix, m_CachedMatrix[ pPlayer->EntIndex( ) ].data( ), sizeof( matrix_t ) * pPlayer->BoneCache( ).Count( ) );
	return true;
}

void C_AnimationSync::OnUpdateClientSideAnimation( BaseEntity* pPlayer )
{
	pPlayer->SetAbsOrigin( pPlayer->Origin( ) );
	for ( int i = 0; i < MAXSTUDIOBONES; i++ )
		m_CachedMatrix[ pPlayer->EntIndex( ) ][ i ].set_origin( pPlayer->Origin( ) - m_BoneOrigins[ pPlayer->EntIndex( ) ][ i ] );

	std::memcpy( pPlayer->BoneCache( ).Base( ), m_CachedMatrix[ pPlayer->EntIndex( ) ].data( ), sizeof( matrix_t ) * pPlayer->BoneCache( ).Count( ) );
	std::memcpy( pPlayer->BoneAccessor( ).m_pBones, m_CachedMatrix[ pPlayer->EntIndex( ) ].data( ), sizeof( matrix_t ) * pPlayer->BoneCache( ).Count( ) );

	return pPlayer->SetupBones_AttachemtnHelper( );
}