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

#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "RatedMgr.h"
#include "World.h"
#include "Group.h"
#include "RatedInfo.h"
#include "Player.h"
#include "WorldSession.h"
#include "Opcodes.h"

RatedInfo::RatedInfo(uint64 guid) : m_guid(guid), m_matchmakerRating(sWorld->getIntConfig(CONFIG_ARENA_START_MATCHMAKER_RATING))
{
    for (uint8 i = 0; i < MAX_RATED_SLOT; ++i)
    {
        RatedType type = GetRatedTypeBySlot(i);        
        StatsBySlot* statsBySlot = new StatsBySlot(type);

        statsBySlot->PersonalRating = sWorld->getIntConfig(CONFIG_ARENA_START_RATING);

        m_ratedStats[type] = statsBySlot;
    }
    
    // Only valid GUIDs are added automatically
    // BattlegroundInfos from default empty constructor must be handled manually

    if (guid)
        sRatedMgr->AddRatedInfo(this);
}

RatedInfo::~RatedInfo()
{ 
    for (uint8 i = 0; i < MAX_RATED_SLOT; ++i)
    {
        RatedType ratedType = GetRatedTypeBySlot(i);
        delete m_ratedStats[ratedType];
    }   
}

void RatedInfo::UpdateStats(RatedType ratedType, uint32 againstMatchmakerRating, int16 &ratingChange, int16 &matchmakerRatingChange, bool won, bool offline)
{
    StatsBySlot* stats = GetStatsBySlot(ratedType);

    // update rating
    ratingChange = stats->GetRatingMod(stats->PersonalRating, againstMatchmakerRating, !offline);
    stats->ModifyRating(ratingChange);

    // update matchmaker rating
    matchmakerRatingChange = stats->GetMatchmakerRatingMod(m_matchmakerRating, againstMatchmakerRating, won);
    ModifyMatchmakerRating(matchmakerRatingChange);

    // update game stats
    stats->SeasonGames += 1;
    stats->WeekGames += 1;

    if (won)
    {
        stats->SeasonWins += 1;
        stats->WeekWins += 1;
    }

    if (Player* player = ObjectAccessor::FindPlayer(GetGUID()))
    {
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING, stats->PersonalRating, ratedType);
    }

    SaveToDB(ratedType);
}

void RatedInfo::SaveToDB(RatedType ratedType)
{
    // Save team and member stats to db
    // Called after a match has ended or when calculating arena_points

    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_RATED_BATTLEGROUND_STATS);
    StatsBySlot const* stats = GetStatsBySlot(ratedType);
    stmt->setUInt32(0, GUID_LOPART(GetGUID()));
    stmt->setUInt8 (1, GetRatedSlotByType(ratedType));
    stmt->setUInt16(2, stats->WeekGames);
    stmt->setUInt16(3, stats->WeekWins);
    stmt->setUInt16(4, stats->WeekBest);
    stmt->setUInt16(5, stats->SeasonGames);
    stmt->setUInt16(6, stats->SeasonWins);
    stmt->setUInt16(7, stats->SeasonBest);
    stmt->setUInt16(8, stats->PersonalRating);
    trans->Append(stmt);

    PreparedStatement* stmt2 = CharacterDatabase.GetPreparedStatement(CHAR_REP_RATED_BATTLEGROUND_MATCHMAKER_RATING);
    stmt2->setUInt32(0, GUID_LOPART(GetGUID()));
    stmt2->setUInt16(1, GetMatchMakerRating());
    trans->Append(stmt2);

    CharacterDatabase.CommitTransaction(trans);
}

RatedType RatedInfo::GetRatedTypeBySlot(uint8 slot)
{
    switch (slot)
    {
        case 0: return RATED_TYPE_2v2;
        case 1: return RATED_TYPE_3v3;
        case 2: return RATED_TYPE_5v5;
        case 3: return RATED_TYPE_10v10;
        default: return RATED_TYPE_NOT_RATED;
    }
}

uint8 RatedInfo::GetRatedSlotByType(RatedType type)
{
    switch (type)
    {
        case RATED_TYPE_2v2:    return 0;
        case RATED_TYPE_3v3:    return 1;
        case RATED_TYPE_5v5:    return 2;
        case RATED_TYPE_10v10:  return 3;
        default:
            break;
    }
    return 0xFF;
}

void RatedInfo::FinishWeek()
{
    for (uint8 i = 0; i < MAX_RATED_SLOT; ++i)
    {
        RatedType ratedType = GetRatedTypeBySlot(i);
        StatsBySlot* stats = GetStatsBySlot(ratedType);

        stats->WeekGames = 0;
        stats->WeekWins = 0;
        stats->WeekBest = 0;
    }
}

void RatedInfo::ModifyMatchmakerRating(int32 mod)
{
    if (int32(m_matchmakerRating) + mod < 0)
        m_matchmakerRating = 0;
    else
        m_matchmakerRating += mod;
}

void StatsBySlot::ModifyRating(int32 mod)
{
    if (int32(PersonalRating) + mod < 0)
        PersonalRating = 0;
    else
        PersonalRating += mod;

    // Week´s Best
    if (PersonalRating > WeekBest)
        WeekBest = PersonalRating;

    // Season´s best
    if (PersonalRating > SeasonBest)
        SeasonBest = PersonalRating;
}

int32 StatsBySlot::GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won /*, float confidence_factor*/) const
{
    // 'Chance' calculation - to beat the opponent
    // This is a simulation. Not much info on how it really works
    float chance = GetChanceAgainst(ownRating, opponentRating);
    float won_mod = (won) ? 1.0f : 0.0f;

    // Calculate the rating modification
    float mod;

    /// @todo Replace this hack with using the confidence factor (limiting the factor to 2.0f)
    if (won && ownRating < 1300)
    {
        if (ownRating < 1000)
            mod = 96.0f * (won_mod - chance);
        else
            mod = (24.0f + (24.0f * (1300.0f - float(ownRating)) / 300.0f)) * (won_mod - chance);
    }
    else
        mod = 24.0f * (won_mod - chance);

    return (int32)ceil(mod);
}

int32 StatsBySlot::GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won) const
{
    // 'Chance' calculation - to beat the opponent
    // This is a simulation. Not much info on how it really works
    float chance = GetChanceAgainst(ownRating, opponentRating);
    float won_mod = (won) ? 1.0f : 0.0f;
    float mod = won_mod - chance;

    // Work in progress:
    /*
    // This is a simulation, as there is not much info on how it really works
    float confidence_mod = min(1.0f - fabs(mod), 0.5f);

    // Apply confidence factor to the mod:
    mod *= confidence_factor

    // And only after that update the new confidence factor
    confidence_factor -= ((confidence_factor - 1.0f) * confidence_mod) / confidence_factor;
    */

    // Real rating modification
    mod *= 24.0f;

    return (int32)ceil(mod);
}

float StatsBySlot::GetChanceAgainst(uint32 ownRating, uint32 opponentRating) const
{
    // Returns the chance to win against a team with the given rating, used in the rating adjustment calculation
    // ELO system
    return 1.0f / (1.0f + exp(log(10.0f) * (float)((float)opponentRating - (float)ownRating) / 650.0f));
}
