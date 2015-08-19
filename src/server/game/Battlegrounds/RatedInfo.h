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

#ifndef RATEDINFO_H
#define RATEDINFO_H

#include "Define.h"
#include <map>

#define MAX_ARENA_SLOT 3
#define MAX_RATED_SLOT 4

enum RatedType
{
    RATED_TYPE_NOT_RATED    = 0,
    RATED_TYPE_2v2          = 2,
    RATED_TYPE_3v3          = 3,
    RATED_TYPE_5v5          = 5,
    RATED_TYPE_10v10        = 10
};

struct GroupRatedStats
{
    uint32 averageMMR;
    uint32 averageRating;

    GroupRatedStats() : averageMMR(0), averageRating(0) { }
};

struct StatsBySlot
{
    StatsBySlot(RatedType type) : Type(type), WeekGames(0),
        WeekWins(0), SeasonGames(0), SeasonWins(0), WeekBest(0), 
        SeasonBest(0), PersonalRating(0) { }

    RatedType Type;
    uint16 WeekGames;
    uint16 WeekWins;
    uint16 SeasonGames;
    uint16 SeasonWins;
    uint16 WeekBest;
    uint16 SeasonBest;
    uint16 PersonalRating;

    void ModifyRating(int32 mod);
    float GetChanceAgainst(uint32 ownRating, uint32 opponentRating) const;
    int32 GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won) const;
    int32 GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won) const;
};

typedef std::map<RatedType, StatsBySlot*> RatedStats;

class RatedInfo
{
public: 
    RatedInfo(uint64 guid = 0);
    ~RatedInfo();

    void FinishWeek();
    void SaveToDB(RatedType ratedType);
    uint64 GetGUID() const { return m_guid; }
    StatsBySlot* GetStatsBySlot(RatedType ratedType) { return m_ratedStats[ratedType]; }
    void UpdateStats(RatedType ratedType, uint32 againstMatchmakerRating, int16 &ratingChange, int16 &matchmakerRatingChange, bool won, bool offline);
    uint16 GetMatchMakerRating() const { return m_matchmakerRating; }
    void SetMatchMakerRating(uint16 mmr) { m_matchmakerRating = mmr; }
    void ModifyMatchmakerRating(int32 mod);

    // Static methods
    static bool IsArenaSlot(uint8 slot) { return (slot < 3); }
    static bool IsRatedBattlegroundSlot(uint8 slot) { return (slot == 3); }
    static RatedType GetRatedTypeBySlot(uint8 slot);
    static uint8 GetRatedSlotByType(RatedType type);    

private:
    RatedStats m_ratedStats;
    uint16 m_matchmakerRating;
    uint64 m_guid;
};

#endif

