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
#ifndef TRINITYCORE_TEMPSUMMON_H
#define TRINITYCORE_TEMPSUMMON_H

#include "Creature.h"

enum SummonerType
{
    SUMMONER_TYPE_CREATURE      = 0,
    SUMMONER_TYPE_GAMEOBJECT    = 1,
    SUMMONER_TYPE_MAP           = 2
};

/// Stores data for temp summons
struct TempSummonData
{
    uint32 entry;        ///< Entry of summoned creature
    Position pos;        ///< Position, where should be creature spawned
    TempSummonType type; ///< Summon type, see TempSummonType for available types
    uint32 time;         ///< Despawn time, usable only with certain temp summon types
};

// Creature Pet entries
enum EPetEntries
{
    // Warlock
    PET_ENTRY_INFERNAL                              = 89,
    PET_ENTRY_IMP                                   = 416,
    PET_ENTRY_VOIDWALKER                            = 1860,
    PET_ENTRY_SUCCUBUS                              = 1863,
    PET_ENTRY_FELHUNTER                             = 417,
    PET_ENTRY_FELGUARD                              = 17252,
    PET_ENTRY_FEL_IMP                               = 58959,
    PET_ENTRY_VOIDLORD                              = 58960,
    PET_ENTRY_SHIVARRA                              = 58963,
    PET_ENTRY_OBSERVER                              = 58964,
    PET_ENTRY_WRATHGUARD                            = 58965,
    PET_ENTRY_WILD_IMP                              = 55659,
    PET_ENTRY_ABYSSAL                               = 58997,
    PET_ENTRY_TERRORGUARD                           = 59000,
    PET_ENTRY_DOOMGUARD                             = 11859,

    // Mage
    PET_ENTRY_WATER_ELEMENTAL                       = 510,
    PET_ENTRY_MIRROR_IMAGE                          = 31216,

    // Druid
    PET_ENTRY_TREANT_GUARDIAN                       = 54985,
    PET_ENTRY_TREANT_FERAL                          = 54984,
    PET_ENTRY_TREANT_RESTO                          = 54983,
    PET_ENTRY_TREANT_BALANCE                        = 1964,

    // Shaman
    PET_ENTRY_FIRE_ELEMENTAL                        = 15438,
    PET_ENTRY_FERAL_SPIRIT                          = 29264,
    PET_ENTRY_FERAL_SPIRIT_SYMBIOSIS                = 58488,
    PET_ENTRY_EARTH_ELEMENTAL                       = 15352,
    PET_ENTRY_HEALING_TIDE_TOTEM                    = 59764,

    // Death Knight
    PET_ENTRY_GHOUL                                 = 26125,
    PET_ENTRY_BLOODWORM                             = 28017,
    PET_ENTRY_GARGOYLE                              = 27829,
    PET_ENTRY_DEAD_ARMY                             = 24207,

    // Priest 
    PET_ENTRY_SHADOWFIEND                           = 19668,
    PET_ENTRY_MINDBENDER                            = 62982,
    PET_ENTRY_MINDBENDER_SHA                        = 67236,
    PET_ENTRY_SHADOWY_APPARITION                    = 61966,
    PET_ENTRY_PSYFIEND                              = 59190,

    // Hunter
    PET_ENTRY_VENOMOUS_SNAKE                        = 19833,
    PET_ENTRY_VIPER                                 = 19921,
    PET_ENTRY_MURDER_OF_CROWS                       = 61994,
    PET_ENTRY_DIRE_BEAST_DUNGEONS                   = 62005,
    PET_ENTRY_DIRE_BEAST_VALLEY_OF_THE_FW           = 62210,
    PET_ENTRY_DIRE_BEAST_KALIMDOR                   = 62855,
    PET_ENTRY_DIRE_BEAST_ESTERN_KINGDOMS            = 62856,
    PET_ENTRY_DIRE_BEAST_OUTLAND                    = 62857,
    PET_ENTRY_DIRE_BEAST_NORTHREND                  = 62858,
    PET_ENTRY_DIRE_BEAST_KRASARANG_WILDS            = 62860,
    PET_ENTRY_DIRE_BEAST_JADE_FOREST                = 62865,
    PET_ENTRY_DIRE_BEAST_VALE_OF_ETERNAL_BLOSSOMS   = 64617,
    PET_ENTRY_DIRE_BEAST_KUNLAI_SUMMIT              = 64618,
    PET_ENTRY_DIRE_BEAST_TOWNLONG_STEPPES           = 64619,
    PET_ENTRY_DIRE_BEAST_DREAD_WASTES               = 64620,

    // Rogue
    PET_ENTRY_DECOY                                 = 62261,

    // Paladin
    PET_ENTRY_GUARDIAN_OF_ANCIENT_KINGS             = 46506,

    // Monk
    PET_ENTRY_XUEN_THE_WHITE_TIGER                  = 63508
};

class TempSummon : public Creature
{
    public:
        explicit TempSummon(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        virtual ~TempSummon() { }
        void Update(uint32 time);
        virtual void InitStats(uint32 lifetime);
        virtual void InitSummon();
        virtual void UnSummon(uint32 msTime = 0);
        void RemoveFromWorld();
        void SetTempSummonType(TempSummonType type);
        void SaveToDB(uint32 /*mapid*/, uint32 /*spawnMask*/, uint32 /*phaseMask*/) { }
        Unit* GetSummoner() const;
        Creature* GetSummonerCreatureBase() const;
        uint64 GetSummonerGUID() const { return m_summonerGUID; }
        TempSummonType const& GetSummonType() { return m_type; }
        uint32 GetTimer() { return m_timer; }

        const SummonPropertiesEntry* const m_Properties;
    private:
        TempSummonType m_type;
        uint32 m_timer;
        uint32 m_lifetime;
        uint64 m_summonerGUID;
};

class Minion : public TempSummon
{
public:
    Minion(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
    void InitStats(uint32 duration);
    void RemoveFromWorld();
    Unit* GetOwner() const { return m_owner; }
    float GetFollowAngle() const { return m_followAngle; }
    void SetFollowAngle(float angle) { m_followAngle = angle; }

    bool IsTreant() const;
    bool IsPetGhoul() const { return GetEntry() == PET_ENTRY_GHOUL; }
    bool IsPetGargoyle() const { return GetEntry() == PET_ENTRY_GARGOYLE; }
    bool IsSpiritWolf() const { return GetEntry() == PET_ENTRY_FERAL_SPIRIT; }
    bool isWrathGuard() const { return GetEntry() == PET_ENTRY_WRATHGUARD; }
    bool IsWaterElemental() const { return GetEntry() == PET_ENTRY_WATER_ELEMENTAL; }
    bool IsGuardianPet() const;
    bool IsWarlockPet() const;
protected:
    Unit* const m_owner;
    float m_followAngle;
};

class Guardian : public Minion
{
    public:
        Guardian(SummonPropertiesEntry const* properties, Unit* owner, bool isWorldObject);
        void InitStats(uint32 duration);
        bool InitStatsForLevel(uint8 level);
        void InitSummon();

        bool UpdateStats(Stats stat);
        bool UpdateAllStats();
        void UpdateResistances(uint32 school);
        void UpdateArmor();
        void UpdateMaxHealth();
        void UpdateMaxPower(Powers power);
        void UpdateAttackPowerAndDamage(bool ranged = false);
        void UpdateDamagePhysical(WeaponAttackType attType);

        int32 GetBonusDamage() const { return m_bonusSpellDamage; }
        void SetBonusDamage(int32 damage);
    protected:
        int32   m_bonusSpellDamage;
        float   m_statFromOwner[MAX_STATS];
};

class Puppet : public Minion
{
    public:
        Puppet(SummonPropertiesEntry const* properties, Unit* owner);
        void InitStats(uint32 duration);
        void InitSummon();
        void Update(uint32 time);
        void RemoveFromWorld();
};

class ForcedUnsummonDelayEvent : public BasicEvent
{
public:
    ForcedUnsummonDelayEvent(TempSummon& owner) : BasicEvent(), m_owner(owner) { }
    bool Execute(uint64 e_time, uint32 p_time);

private:
    TempSummon& m_owner;
};

inline
bool Minion::IsTreant() const
{
    switch (GetEntry())
    {
        case PET_ENTRY_TREANT_GUARDIAN:
        case PET_ENTRY_TREANT_FERAL:
        case PET_ENTRY_TREANT_BALANCE:
        case PET_ENTRY_TREANT_RESTO:
            return true;
        default:
            return false;
    }
}

inline
bool Minion::IsGuardianPet() const
{
    return IsPet() || (m_Properties && m_Properties->Category == SUMMON_CATEGORY_PET);
}

inline
bool Minion::IsWarlockPet() const
{
    auto checkEntry = [this] ()
    {
        switch (GetEntry())
        {
            case PET_ENTRY_INFERNAL:
            case PET_ENTRY_IMP:
            case PET_ENTRY_VOIDWALKER:
            case PET_ENTRY_SUCCUBUS:
            case PET_ENTRY_FELHUNTER:
            case PET_ENTRY_FELGUARD:
            case PET_ENTRY_OBSERVER:
            case PET_ENTRY_SHIVARRA:
            case PET_ENTRY_VOIDLORD:
            case PET_ENTRY_FEL_IMP:
                return true;
            default:
                return false;
        }
    };

    return IsPet() && checkEntry() || isWrathGuard();
}

#endif
