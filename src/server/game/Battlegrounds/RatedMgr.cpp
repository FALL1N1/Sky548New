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

#include "Define.h"
#include "RatedMgr.h"
#include "RatedInfo.h"
#include "World.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "Language.h"
#include "ObjectAccessor.h"
#include "Player.h"

RatedMgr::RatedMgr() 
{ }

RatedMgr::~RatedMgr()
{
    for (RatedInfoContainer::iterator itr = m_ratedInfoStore.begin(); itr != m_ratedInfoStore.end(); ++itr)
        delete itr->second;
}

RatedInfo* RatedMgr::GetRatedInfo(uint64 guid)
{
    RatedInfoContainer::const_iterator itr = m_ratedInfoStore.find(guid);
    if (itr != m_ratedInfoStore.end())
        return itr->second;

    // No info found, lets create new info
    return new RatedInfo(guid);
}

void RatedMgr::AddRatedInfo(RatedInfo* info)
{
    RatedInfoContainer::iterator itr = m_ratedInfoStore.find(info->GetGUID());
    ASSERT(itr == m_ratedInfoStore.end() && "This RatedInfo with this guid is already included");
    m_ratedInfoStore[info->GetGUID()] = info;
}

void RatedMgr::RemoveRatedInfo(RatedInfo* info)
{
    // Save guid to keep the key
    uint64 guid = info->GetGUID();

    // Every RatedInfo object should be contained here
    RatedInfoContainer::iterator itr = m_ratedInfoStore.find(info->GetGUID());
    ASSERT(itr != m_ratedInfoStore.end());

    // now we can delete info itself
    delete itr->second;    

    // delete the key
    m_ratedInfoStore.erase(itr->first);
}

void RatedMgr::FinishWeek()
{
    // Resets all week stats
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_RATED_BATTLEGROUND_FINISH_WEEK);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    for (RatedInfoContainer::iterator itr = m_ratedInfoStore.begin(); itr != m_ratedInfoStore.end(); ++itr)
    {
        itr->second->FinishWeek();
    }
}

void RatedMgr::LoadRatedInfo()
{
    uint32 oldMSTime = getMSTime();

    // Delete all stats and mmr records with non existent characters
    CharacterDatabase.Execute("DELETE crs.* FROM character_rated_stats crs LEFT JOIN characters c ON crs.guid = c.guid WHERE c.guid IS NULL");
    CharacterDatabase.Execute("DELETE crmr.* FROM character_rated_matchmaker_rating crmr LEFT JOIN characters c ON crmr.guid = c.guid WHERE c.guid IS NULL");

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_RATED_BATTLEGROUND_MATCHMAKER_RATING);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    std::unordered_map<uint64, uint16> playersMMRCache;

    if (result)
    {
        do
        {
            uint8 index = 0;
            Field* fields = result->Fetch();
            uint64 guid = fields[index++].GetUInt32();
            uint16 matchmakerRating = fields[index++].GetUInt16();

            playersMMRCache[guid] = matchmakerRating;
        } 
        while (result->NextRow());
    }
    else
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 matchmaker ratings. DB table `character_rated_matchmaker_rating` is empty!");
    }

    auto getPlayersMMR = [playersMMRCache](uint64 guid) -> uint16
    {
        std::unordered_map<uint64, uint16>::const_iterator itr = playersMMRCache.find(guid);
        if (itr != playersMMRCache.end())
        {
            return itr->second;
        }
        else // no mmr record in db
        {
            return sWorld->getIntConfig(CONFIG_ARENA_START_RATING);
        }            
    };

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_RATED_BATTLEGROUND_STATS);
    result = CharacterDatabase.Query(stmt);

    if (!result)
    {
        TC_LOG_INFO("server.loading", ">> Loaded 0 character rated battleground stats. DB table `character_rated_stats` is empty!");
        return;
    }

    uint32 count = 0;
    uint64 lastGuid = 0;    

    // Container for even all available rated battleground slots (2v2, 3v3, 5v5 and 10v10) per 1 GUID
    RatedInfo* info = NULL;

    do
    {
        Field* fields = result->Fetch();

        uint8 index = 0;
        uint64 guid = fields[index++].GetUInt32();
        uint8 slot = fields[index++].GetUInt8();

        // Validity Check
        if (slot >= MAX_RATED_SLOT)
        {
            TC_LOG_ERROR("sql.sql", "Found invalid Rated battleground slot: %u for GUID: %u", slot, guid);
            continue;
        }

        if (!info || (lastGuid != guid))
        {
            lastGuid = guid;
            info = new RatedInfo(guid);
            info->SetMatchMakerRating(getPlayersMMR(guid));
        }

        RatedType ratedType = RatedInfo::GetRatedTypeBySlot(slot);

        // stats for all stats should be always initialized to default values
        StatsBySlot* stats = info->GetStatsBySlot(ratedType);
        ASSERT(stats);
        
        // everything is ok, load stats for current slot
        stats->WeekGames = fields[index++].GetUInt16();
        stats->WeekWins = fields[index++].GetUInt16();
        stats->WeekBest = fields[index++].GetUInt16();
        stats->SeasonGames = fields[index++].GetUInt16();
        stats->SeasonWins = fields[index++].GetUInt16();
        stats->SeasonBest = fields[index++].GetUInt16();
        stats->PersonalRating = fields[index++].GetUInt16();  

        ++count;
    }
    while (result->NextRow());

    TC_LOG_INFO("server.loading", ">> Loaded %u character rated stats in %u ms", count, GetMSTimeDiffToNow(oldMSTime));
}
