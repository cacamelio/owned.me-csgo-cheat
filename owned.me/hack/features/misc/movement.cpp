#include "../features.hpp"

void movement::AutoJump()
{
    if (!config.bunny_hop)
        return;

    if (g::pLocalPlayer->Flags() & FL_ONGROUND)
        return;

    g::pCmd->buttons &= ~IN_JUMP;
}

float get_move_rotation(unsigned int buttons)
{
    float yaw_add = 0.f;
    bool back = buttons & IN_BACK;
    bool forward = buttons & IN_FORWARD;
    bool left = buttons & IN_MOVERIGHT;
    bool right = buttons & IN_MOVELEFT;

    if (back)
    {
        yaw_add = -180.f;

        if (right)
            yaw_add -= 45.f;
        else if (left)
            yaw_add += 45.f;
    }
    else if (right)
    {
        yaw_add = 90.f;

        if (back)
            yaw_add += 45.f;
        else if (forward)
            yaw_add -= 45.f;
    }
    else if (left)
    {
        yaw_add = -90.f;

        if (back)
            yaw_add -= 45.f;
        else if (forward)
            yaw_add += 45.f;
    }
    else
        yaw_add = 0.f;

    return yaw_add;
}

void movement::AutoStrafe()
{
    if (!config.movement_strafe)
        return;

    if (g::pCmd->buttons & IN_SPEED)
        return;

    if (g::pLocalPlayer->Flags() & FL_ONGROUND)
        return;

    vec3_t velocity = g::pLocalPlayer->Velocity();
    float velocity_angle = RAD2DEG(std::atan2(velocity.y, velocity.x));

    g::vStrafeAngles.y += get_move_rotation(g::pCmd->buttons);

    g::pCmd->forwardmove = g::pCmd->sidemove = 0.f;

    float delta = math::NormalizeYaw(g::vStrafeAngles.y - velocity_angle);
    g::pCmd->sidemove = delta > 0.f ? -450.f : 450.f;
    g::vStrafeAngles.y -= delta;
}

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/258323-autostop.html
void movement::FastStop()
{
    if (!config.faststop)
        return;

    auto cmd = g::pCmd;
    if (!(g::pLocalPlayer->Flags() & FL_ONGROUND))
        return;

    auto pressed_move_key = cmd->buttons & IN_FORWARD || cmd->buttons & IN_MOVELEFT || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVERIGHT || cmd->buttons & IN_JUMP;

    if (pressed_move_key)
        return;
    auto velocity = g::pLocalPlayer->Velocity();

    if (velocity.length_2d() > 20.0f)
    {
        vec3_t direction;
        vec3_t real_view;

        math::vector_angles(velocity, direction);
        interfaces::engine->get_view_angles(real_view);

        direction.y = real_view.y - direction.y;

        vec3_t forward;
        math::angle_vectors(direction, forward);

        static auto cl_forwardspeed = interfaces::console->FindVar("cl_forwardspeed");
        static auto cl_sidespeed = interfaces::console->FindVar("cl_sidespeed");

        auto negative_forward_speed = -cl_forwardspeed->GetFloat();
        auto negative_side_speed = -cl_sidespeed->GetFloat();

        auto negative_forward_direction = forward * negative_forward_speed;
        auto negative_side_direction = forward * negative_side_speed;

        cmd->forwardmove = negative_forward_direction.x;
        cmd->sidemove = negative_side_direction.y;
    }
}

void ChangeSpeed(float max_speed)
{
    float sidemove = g::pCmd->sidemove;
    float forwardmove = g::pCmd->forwardmove;

    float move_speed = std::sqrt(std::pow(sidemove, 2) + std::pow(forwardmove, 2));
    if (move_speed > max_speed)
    {
        bool invalid_speed = max_speed + 1.f < g::pLocalPlayer->Velocity().length();

        g::pCmd->sidemove = invalid_speed ? 0.f : (sidemove / move_speed) * max_speed;
        g::pCmd->forwardmove = invalid_speed ? 0.f : (forwardmove / move_speed) * max_speed;
    }
}

void movement::SlowWalk()
{
    if (!config.slowwalk)
        return;

    if (!g_keyhandler.CheckKey(config.slowwalk_key, config.slowwalk_key_style))
        return;

    if (!(g::pLocalPlayer->Flags() & FL_ONGROUND))
        return;

    ChangeSpeed(max_speed / 3.f);
}

void movement::InfiniteDuck()
{
    if (!config.infinite_duck)
        return;

    g::pCmd->buttons |= IN_BULLRUSH;
}

void movement::EdgeJump(int iFlags)
{
    static bool bToggled = false;
    if (!config.edgejump ||
        !g_keyhandler.CheckKey(config.edgejumpkey, config.edgejumpkey_style))
        return;

    if (!(g::pLocalPlayer->Flags() & FL_ONGROUND) &&
        (iFlags & FL_ONGROUND))
        g::pCmd->buttons |= IN_JUMP;
}

void movement::JumpBug(int unFlags, int preFlags)
{
    if (config.jumpbug && !(unFlags & (1 << 0)) && preFlags & (1 << 0) && g_keyhandler.CheckKey(config.jumpbugkey, config.jumpbugkey_style))
    {
        g::pCmd->buttons |= (IN_DUCK);
    }

    if (config.jumpbug)
    {
        if (g_keyhandler.CheckKey(config.jumpbugkey, config.jumpbugkey_style))
        {
            if (preFlags & FL_ONGROUND) 
            {
                g::pCmd->buttons &= ~IN_JUMP;
                m_bIsJumpbugging = true;
            }
            else
            {
                m_bIsJumpbugging = false;
            }
        }
    }
}

void movement::AutoPeek()
{
    if (m_bRetreated)
        m_vecStartPosition.Zero();
    else if (g_keyhandler.CheckKey(config.autopeek_key, config.autopeek_key_style) && config.autopeek)
    {
        if (!m_bTurnedOn)
        {
            m_bTurnedOn = true;
            m_bWaitAnimationProgress = true;
            m_bNegativeSide = false;

            m_flAnimationTime = interfaces::globals->realtime + .1f;

            m_vecStartPosition = g::pLocalPlayer->AbsOrigin();
        }
    }
    else if (m_bTurnedOn)
    {
        m_bTurnedOn = false;
        m_bWaitAnimationProgress = true;
        m_bNegativeSide = true;

        m_flAnimationTime = interfaces::globals->realtime + .1f;
    }
    auto pressing_movement =
        ((g::pCmd->buttons & IN_LEFT) ||
            (g::pCmd->buttons & IN_FORWARD) ||
            (g::pCmd->buttons & IN_BACK) ||
            (g::pCmd->buttons & IN_RIGHT) ||
            (g::pCmd->buttons & IN_MOVELEFT) ||
            (g::pCmd->buttons & IN_MOVERIGHT));

    auto did_shot = g::pCmd->buttons & IN_ATTACK;

    if (!m_bRetreat) {
        if (did_shot || !pressing_movement)
            m_bRetreat = true;
        else
            m_bRetreat = false;
    }


    if (!m_bRetreat || !m_bTurnedOn)
        return;

    auto vecDifference = g::pLocalPlayer->AbsOrigin() - m_vecStartPosition;
    if (vecDifference.length_2d() <= 5.0f)
    {
        m_bRetreat = false;
        return;
    }

    vec3_t angWishAngles;
    interfaces::engine->get_view_angles(angWishAngles);

    float_t flVelocityX = vecDifference.x * cos(angWishAngles.y / 180.0f * M_PI) + vecDifference.y * sin(angWishAngles.y / 180.0f * M_PI);
    float_t flVelocityY = vecDifference.y * cos(angWishAngles.y / 180.0f * M_PI) - vecDifference.x * sin(angWishAngles.y / 180.0f * M_PI);

    g::pCmd->forwardmove = -flVelocityX * 20.0f;
    g::pCmd->sidemove = flVelocityY * 20.0f;
}

void movement::AutoPeekIndicator()
{
    auto weapon = g::pLocalWeapon;
    if (!weapon)
        return;

    static auto position = vec3_t(0, 0, 0);

    if (!m_vecStartPosition.is_zero())
        position = m_vecStartPosition;

    if (position.is_zero())
        return;

    static auto alpha = 4.0f;

    if (g_keyhandler.CheckKey(config.autopeek_key, config.autopeek_key_style) || alpha)
    {
        if (g_keyhandler.CheckKey(config.autopeek_key, config.autopeek_key_style))
            alpha += 9.0f * interfaces::globals->frame_time;
        else
            alpha -= 9.0f * interfaces::globals->frame_time;

        alpha = std::clamp(alpha, 0.0f, 1.0f);
        render::filled_circle3d(position, alpha * 20.f, m_bRetreat ? Color(40, 220, 5, (int)(alpha * 135.0f)) : Color(200, 200, 200, (int)(alpha * 125.0f)));
    }
}

void movement::FixMove(UserCmd* cmd, vec3_t& ang)
{
    if (!cmd)
        cmd = g::pCmd;

    if (!ang.valid())
        ang = g::vStrafeAngles;

    vec3_t move = vec3_t(cmd->forwardmove, cmd->sidemove, 0.f);
    vec3_t dir = {};

    float delta, len;
    vec3_t move_angle = {};

    len = move.NormalizedFloat();

    if (!len)
        return;

    math::vector_angles(move, move_angle);

    delta = (cmd->viewangles.y - ang.y);

    move_angle.y += delta;

    math::angle_to_vectors(move_angle, &dir);

    dir *= len;

    if (g::pLocalPlayer->MoveType() == MOVETYPE_LADDER)
    {
        if (cmd->viewangles.x >= 45 && ang.x < 45 && std::abs(delta) <= 65)
            dir.x = -dir.x;

        cmd->forwardmove = dir.x;
        cmd->sidemove = dir.y;

        if (cmd->forwardmove > 200)
            cmd->buttons |= IN_FORWARD;

        else if (cmd->forwardmove < -200)
            cmd->buttons |= IN_BACK;

        if (cmd->sidemove > 200)
            cmd->buttons |= IN_MOVERIGHT;

        else if (cmd->sidemove < -200)
            cmd->buttons |= IN_MOVELEFT;
    }
    else
    {
        if (cmd->viewangles.x < -90 || cmd->viewangles.x > 90)
            dir.x = -dir.x;

        cmd->forwardmove = dir.x;
        cmd->sidemove = dir.y;
    }

    cmd->forwardmove = std::clamp(cmd->forwardmove, -450.f, 450.f);
    cmd->sidemove = std::clamp(cmd->sidemove, -450.f, 450.f);
    cmd->upmove = std::clamp(cmd->upmove, -320.f, 320.f);
}

void movement::run()
{
    if (!interfaces::engine->is_in_game())
        return;

    if (!g::pLocalPlayer->IsAlive())
        return;

    if (g::pLocalPlayer->MoveType() == MOVETYPE_LADDER
        || g::pLocalPlayer->MoveType() == MOVETYPE_NOCLIP)
        return;

    const auto weapon = g::pLocalWeapon;
    if (!weapon)
        return;

    const auto info = g::pLocalWeaponData;
    if (!info)
        return;

    max_speed = g::pLocalPlayer->IsScoped() ? info->weapon_max_speed_alt : info->weapon_max_speed;

    int preFlags = g::pLocalPlayer->Flags();
    int unFlags = g::pLocalPlayer->Flags();

    AutoJump();
    AutoStrafe();
    FastStop();
    SlowWalk();
    InfiniteDuck();
    EdgeJump(unFlags);
    JumpBug(unFlags, preFlags);
    AutoPeek();
}