/*
 * Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2014 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "BattlegroundTOK.h"
#include "World.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "BattlegroundMgr.h"
#include "Creature.h"
#include "Language.h"
#include "Object.h"
#include "Player.h"
#include "Util.h"
#include "WorldSession.h"
#include "Battleground.h"

BattlegroundTOK::BattlegroundTOK()
{
    m_ReputationCapture = 0;
    m_HonorWinKills = 0;
    m_HonorEndKills = 0;

    m_EndTimer = 0;
    m_RemoveColTimer = BG_TOK_REMOVE_COL_TIME;
    m_UpdatePointsTimer = BG_TOK_POINTS_UPDATE_TIME;
    m_LastCapturedOrbTeam = PANDAREN_NEUTRAL;

    StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_TOK_START_TWO_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_TOK_START_ONE_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_TOK_START_HALF_MINUTE;
    StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_TOK_HAS_BEGUN;
}

BattlegroundTOK::~BattlegroundTOK()
{
}

void BattlegroundTOK::PostUpdateImpl(uint32 diff)
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        if (m_EndTimer <= diff)
        {
            uint32 allianceScore = GetTeamScore(ALLIANCE);
            uint32 hordeScore    = GetTeamScore(HORDE);

            if (allianceScore > hordeScore)
                EndBattleground(ALLIANCE);
            else if (allianceScore < hordeScore)
                EndBattleground(HORDE);
            else
            {
                // if 0 => tie
                EndBattleground(m_LastCapturedOrbTeam);
            }

            return;
        }
        else
        {
            uint32 minutesLeftPrev = GetRemainingTimeInMinutes();
            m_EndTimer -= diff;
            uint32 minutesLeft = GetRemainingTimeInMinutes();

            if (minutesLeft != minutesLeftPrev)
                UpdateWorldState(BG_TOK_TIME_REMAINING, minutesLeft);
        }

        if (m_RemoveColTimer <= diff) // remove door collision 2 seconds after the doors have been opened.. blizz-like
        {
            DelObject(BG_TOK_OBJECT_A_DOOR_COL);
            DelObject(BG_TOK_OBJECT_H_DOOR_COL);
        }
        else
            m_RemoveColTimer -= diff;

        if (m_UpdateScoreBoardTimer <= diff)
        {
            for (uint8 i = 0; i < MAX_ORBS; ++i)
                if (uint64 guid = m_OrbKeepers[i])
                    if (m_playersZone.find(guid) != m_playersZone.end())
                        if (Player* player = ObjectAccessor::FindPlayer(guid))
                            UpdatePlayerScore(player, SCORE_ORB_SCORE, 1);

            m_UpdateScoreBoardTimer = BG_TOK_SCOREBOARD_UPDATE_TIME;
        }
        else
            m_UpdateScoreBoardTimer -= diff;

        if (m_UpdatePointsTimer <= diff)
        {
            for (uint8 i = 0; i < MAX_ORBS; ++i)
                if (uint64 guid = m_OrbKeepers[i])
                    if (m_playersZone.find(guid) != m_playersZone.end())
                        if (Player* player = ObjectAccessor::FindPlayer(guid))
                            AccumulateScore(player->GetTeamId(), m_playersZone[guid]);

            m_UpdatePointsTimer = BG_TOK_POINTS_UPDATE_TIME;
        }
        else
            m_UpdatePointsTimer -= diff;
    }
}

void BattlegroundTOK::StartingEventCloseDoors()
{
    SpawnBGObject(BG_TOK_OBJECT_A_DOOR, RESPAWN_IMMEDIATELY);
    SpawnBGObject(BG_TOK_OBJECT_H_DOOR, RESPAWN_IMMEDIATELY);

    DoorClose(BG_TOK_OBJECT_A_DOOR);
    DoorClose(BG_TOK_OBJECT_H_DOOR);

    for (uint8 i = 0; i < 4; ++i)
        SpawnBGObject(BG_TOK_OBJECT_ORB_1 + i, RESPAWN_ONE_DAY);
}

void BattlegroundTOK::StartingEventOpenDoors()
{
    DoorOpen(BG_TOK_OBJECT_A_DOOR);
    DoorOpen(BG_TOK_OBJECT_H_DOOR);

    for (uint8 i = 0; i < 4; ++i)
    {
        SpawnBGObject(BG_TOK_OBJECT_ORB_1 + i, RESPAWN_IMMEDIATELY);
        if (Creature* aura = GetBGCreature(BG_TOK_CREATURE_ORB_AURA_1 + i))
            aura->AddAura(BG_TOK_ORBS_AURA[i], aura);
    }

    // Players that join battleground after start are not eligible to get achievement.
    StartTimedAchievement(ACHIEVEMENT_TIMED_TYPE_EVENT, BG_TOK_EVENT_START_BATTLE);
}

void BattlegroundTOK::AddPlayer(Player *plr)
{
    Battleground::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattlegroundTOKScore* sc = new BattlegroundTOKScore;

    PlayerScores[plr->GetGUID()] = sc;
    m_playersZone[plr->GetGUID()] = TOK_ZONE_OUT;
}

void BattlegroundTOK::EventPlayerClickedOnOrb(Player* source, GameObject* target_obj)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!source->IsWithinDistInMap(target_obj, 10))
        return;

    if (source->GetCurrentSpell(CURRENT_GENERIC_SPELL))
        return;

    uint32 index = target_obj->GetEntry() - BG_TOK_OBJECT_ORB_1_ENTRY;

    // If this orb is already keeped by a player, there is a problem
    if (index > MAX_ORBS || m_OrbKeepers[index] != 0)
        return;

    // Check if the player already have an orb
    for (uint8 i = 0; i < MAX_ORBS; ++i)
        if (m_OrbKeepers[i] == source->GetGUID())
            return;

    PlaySoundToAll(source->GetTeamId() == TEAM_ALLIANCE ? BG_TOK_SOUND_A_ORB_PICKED_UP: BG_TOK_SOUND_H_ORB_PICKED_UP);
    source->CastSpell(source, BG_TOK_ORBS_SPELLS[index], true);
    source->CastSpell(source, source->GetTeamId() == TEAM_ALLIANCE ? BG_TOK_ALLIANCE_INSIGNIA: BG_TOK_HORDE_INSIGNIA, true);

    UpdatePlayerScore(source, SCORE_ORB_HANDLES, 1);

    m_OrbKeepers[index] = source->GetGUID();
    UpdateWorldState(BG_TOK_ICON_A, 1);
    SpawnBGObject(BG_TOK_OBJECT_ORB_1 + index, RESPAWN_ONE_DAY);

    if (Creature* aura = GetBGCreature(BG_TOK_CREATURE_ORB_AURA_1 + index))
        aura->RemoveAllAuras();

    SendMessage2ToAll(LANG_BG_TOK_PICKEDUP, source->GetTeamId() == TEAM_ALLIANCE ? CHAT_MSG_BG_SYSTEM_ALLIANCE: CHAT_MSG_BG_SYSTEM_HORDE, source, GetOrbName[index]);
    source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
}

void BattlegroundTOK::EventPlayerDroppedOrb(Player* source)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    uint8 index = 0;

    for (; index <= MAX_ORBS; ++index)
    {
        if (index == MAX_ORBS)
            return;

        if (m_OrbKeepers[index] == source->GetGUID())
            break;
    }

    PlaySoundToAll(source->GetTeamId() == TEAM_ALLIANCE ? BG_TOK_SOUND_A_ORB_PICKED_UP: BG_TOK_SOUND_H_ORB_PICKED_UP);
    source->RemoveAurasDueToSpell(BG_TOK_ORBS_SPELLS[index]);
    source->RemoveAurasDueToSpell(BG_TOK_ALLIANCE_INSIGNIA);
    source->RemoveAurasDueToSpell(BG_TOK_HORDE_INSIGNIA);

    m_OrbKeepers[index] = 0;
    SpawnBGObject(BG_TOK_OBJECT_ORB_1 + index, RESPAWN_IMMEDIATELY);
    if (Creature* aura = GetBGCreature(BG_TOK_CREATURE_ORB_AURA_1 + index))
        aura->AddAura(BG_TOK_ORBS_AURA[index], aura);

    UpdateWorldState(BG_TOK_ICON_A, 0);
    SendWarningToAll(LANG_BG_TOK_O_PLACED, GetTrinityString(GetOrbName[index]));
    source->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
}

void BattlegroundTOK::RemovePlayer(Player* player, uint64 guid, uint32 /*team*/)
{
    EventPlayerDroppedOrb(player);
    m_playersZone.erase(guid);
}

void BattlegroundTOK::UpdateOrbState(Team team, uint32 value)
{
    if (team == ALLIANCE)
        UpdateWorldState(BG_TOK_ICON_A, value);
    else
        UpdateWorldState(BG_TOK_ICON_H, value);
}

void BattlegroundTOK::UpdateTeamScore(Team team)
{
    if (team == ALLIANCE)
        UpdateWorldState(BG_TOK_ORB_POINTS_A, GetTeamScore(team));
    else
        UpdateWorldState(BG_TOK_ORB_POINTS_H, GetTeamScore(team));
}

void BattlegroundTOK::HandleAreaTrigger(Player* source, uint32 trigger)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    uint64 sourceGuid = source->GetGUID();
    switch(trigger)
    {
        case 7734: // Out-In trigger
        case 7733:
        {
            if (m_playersZone.find(sourceGuid) == m_playersZone.end())
                return;

            if (m_playersZone[sourceGuid] == TOK_ZONE_OUT)
                m_playersZone[sourceGuid] = TOK_ZONE_IN;
            else
                m_playersZone[sourceGuid] = TOK_ZONE_OUT;
            break;
        }
        case 7735: // Middle-In trigger
        {
            if (m_playersZone.find(sourceGuid) == m_playersZone.end())
                return;

            if (m_playersZone[sourceGuid] == TOK_ZONE_IN)
                m_playersZone[sourceGuid] = TOK_ZONE_MIDDLE;
            else
                m_playersZone[sourceGuid] = TOK_ZONE_IN;
            break;
        }
        case 7843: // orb radius
        case 7842: // orb radius
        case 7845: // middle trigger - has 2 locations
        case 7841: // orb radius
        case 7840: // orb radius
        case 7844: // middle trigger - has 2 locations
        case 8378: // starting
        case 8377: // starting
            break;
        default:
            //source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", trigger);
            break;
    }
}

bool BattlegroundTOK::SetupBattleground()
{
    // Doors
    if (   !AddObject(BG_TOK_OBJECT_A_DOOR, BG_TOK_OBJECT_DOOR_ENTRY, BG_TOK_DoorPositions[0][0], BG_TOK_DoorPositions[0][1], BG_TOK_DoorPositions[0][2], BG_TOK_DoorPositions[0][3], 0, 0, sin(BG_TOK_DoorPositions[0][3]/2), cos(BG_TOK_DoorPositions[0][3]/2), RESPAWN_IMMEDIATELY)
        || !AddObject(BG_TOK_OBJECT_H_DOOR, BG_TOK_OBJECT_DOOR_ENTRY, BG_TOK_DoorPositions[1][0], BG_TOK_DoorPositions[1][1], BG_TOK_DoorPositions[1][2], BG_TOK_DoorPositions[1][3], 0, 0, sin(BG_TOK_DoorPositions[1][3]/2), cos(BG_TOK_DoorPositions[1][3]/2), RESPAWN_IMMEDIATELY)
        || !AddObject(BG_TOK_OBJECT_A_DOOR_COL, BG_TOK_OBJECT_DOOR_COL, BG_TOK_DoorColPositions[0][0], BG_TOK_DoorColPositions[0][1], BG_TOK_DoorColPositions[0][2], BG_TOK_DoorColPositions[0][3], 0, 0, sin(BG_TOK_DoorColPositions[0][3]/2), cos(BG_TOK_DoorColPositions[0][3]/2), RESPAWN_IMMEDIATELY)
        || !AddObject(BG_TOK_OBJECT_H_DOOR_COL, BG_TOK_OBJECT_DOOR_COL, BG_TOK_DoorColPositions[1][0], BG_TOK_DoorColPositions[1][1], BG_TOK_DoorColPositions[1][2], BG_TOK_DoorColPositions[1][3], 0, 0, sin(BG_TOK_DoorColPositions[1][3]/2), cos(BG_TOK_DoorColPositions[1][3]/2), RESPAWN_IMMEDIATELY))
        return false;

    if (   !AddSpiritGuide(BG_TOK_CREATURE_SPIRIT_1, BG_TOK_SpiritPositions[0][0], BG_TOK_SpiritPositions[0][1], BG_TOK_SpiritPositions[0][2], BG_TOK_SpiritPositions[0][3], ALLIANCE)
        || !AddSpiritGuide(BG_TOK_CREATURE_SPIRIT_2, BG_TOK_SpiritPositions[1][0], BG_TOK_SpiritPositions[1][1], BG_TOK_SpiritPositions[1][2], BG_TOK_SpiritPositions[1][3], HORDE))
        return false;

    // Orbs
    for (uint8 i = 0; i < MAX_ORBS; ++i)
    {
        if (!AddObject(BG_TOK_OBJECT_ORB_1 + i, BG_TOK_OBJECT_ORB_1_ENTRY + i, BG_TOK_OrbPositions[i][0], BG_TOK_OrbPositions[i][1], BG_TOK_OrbPositions[i][2], BG_TOK_OrbPositions[i][3], 0, 0, sin(BG_TOK_OrbPositions[i][3]/2), cos(BG_TOK_OrbPositions[i][3]/2), RESPAWN_ONE_DAY))
            return false;

        if (!AddCreature(WORLD_TRIGGER, BG_TOK_CREATURE_ORB_AURA_1 + i, PANDAREN_NEUTRAL, BG_TOK_OrbPositions[i][0], BG_TOK_OrbPositions[i][1], BG_TOK_OrbPositions[i][2], BG_TOK_OrbPositions[i][3], RESPAWN_IMMEDIATELY))
            return false;
    }

    return true;
}

void BattlegroundTOK::Reset()
{
    //call parent's class reset
    Battleground::Reset();
    BgObjects.resize(BG_TOK_OBJECT_MAX);
    BgCreatures.resize(BG_TOK_CREATURE_MAX);

    for (uint32 i = 0; i < MAX_ORBS; ++i)
        m_OrbKeepers[i] = 0;

    bool isBGWeekend = BattlegroundMgr::IsBGWeekend(GetTypeID());
    m_ReputationCapture = (isBGWeekend) ? 45 : 35;
    m_HonorWinKills = (isBGWeekend) ? 3 : 1;
    m_HonorEndKills = (isBGWeekend) ? 4 : 2;

    m_EndTimer = BG_TOK_TIME_LIMIT;
    m_LastCapturedOrbTeam = PANDAREN_NEUTRAL;
}

void BattlegroundTOK::EndBattleground(uint32 winner)
{
    //win reward
    if (winner == ALLIANCE)
        RewardHonorToTeam(GetBonusHonorFromKill(m_HonorWinKills), ALLIANCE);
    if (winner == HORDE)
        RewardHonorToTeam(GetBonusHonorFromKill(m_HonorWinKills), HORDE);
    //complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(m_HonorEndKills), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(m_HonorEndKills), HORDE);

    Battleground::EndBattleground(winner);
}

void BattlegroundTOK::HandleKillPlayer(Player *player, Player *killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    EventPlayerDroppedOrb(player);

    Battleground::HandleKillPlayer(player, killer);
}


void BattlegroundTOK::UpdatePlayerScore(Player* Source, uint32 type, uint32 value, bool doAddHonor)
{
    BattlegroundScoreMap::iterator itr = PlayerScores.find(Source->GetGUID());
    if (itr == PlayerScores.end())                         // player not found
        return;

    switch (type)
    {
        case SCORE_ORB_HANDLES:
            ((BattlegroundTOKScore*)itr->second)->OrbHandles += value;
            break;
        case SCORE_ORB_SCORE:
            ((BattlegroundTOKScore*)itr->second)->Score += value;
            break;
        default:
            Battleground::UpdatePlayerScore(Source, type, value, doAddHonor);
            break;
    }
}

WorldSafeLocsEntry const* BattlegroundTOK::GetClosestGraveYard(Player* player)
{
    if (player->GetTeam() == ALLIANCE)
    {
        if (GetStatus() == STATUS_IN_PROGRESS)
            return sWorldSafeLocsStore.LookupEntry(TOK_GRAVEYARD_RECTANGLEA1);
        else
            return sWorldSafeLocsStore.LookupEntry(TOK_GRAVEYARD_RECTANGLEA2);
    }
    else
    {
        if (GetStatus() == STATUS_IN_PROGRESS)
            return sWorldSafeLocsStore.LookupEntry(TOK_GRAVEYARD_RECTANGLEH1);
        else
            return sWorldSafeLocsStore.LookupEntry(TOK_GRAVEYARD_RECTANGLEH2);
    }
}

void BattlegroundTOK::AccumulateScore(uint32 team, BG_TOK_ZONE zone)
{
    if (zone > TOK_ZONE_MAX)
        return;

    if (team >= PANDAREN_NEUTRAL)
        return;

    m_TeamScores[team] += BG_TOK_TickPoints[zone];

    if (m_TeamScores[team] > BG_TOK_MAX_TEAM_SCORE)
        m_TeamScores[team] = BG_TOK_MAX_TEAM_SCORE;

    if (team == TEAM_ALLIANCE)
        UpdateWorldState(BG_TOK_ORB_POINTS_A, m_TeamScores[team]);
    if (team == TEAM_HORDE)
        UpdateWorldState(BG_TOK_ORB_POINTS_H, m_TeamScores[team]);

    // Test win condition
    if (m_TeamScores[TEAM_ALLIANCE] >= BG_TOK_MAX_TEAM_SCORE)
        EndBattleground(ALLIANCE);
    if (m_TeamScores[TEAM_HORDE] >= BG_TOK_MAX_TEAM_SCORE)
        EndBattleground(HORDE);
}

void BattlegroundTOK::FillInitialWorldStates(WorldStateBuilder& builder)
{
    builder.AppendState(BG_TOK_ORB_POINTS_A, GetTeamScore(ALLIANCE));
    builder.AppendState(BG_TOK_ORB_POINTS_H, GetTeamScore(HORDE));
    builder.AppendState(BG_TOK_ORB_POINTS_MAX, BG_TOK_MAX_TEAM_SCORE);
    builder.AppendState(BG_TOK_TIME_ENABLED, 1);
    builder.AppendState(BG_TOK_TIME_REMAINING, GetRemainingTimeInMinutes());
}