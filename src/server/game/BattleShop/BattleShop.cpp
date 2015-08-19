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

#include "BattleShop.h"
#include "WorldSession.h"

CharacterBooster::CharacterBooster(WorldSession* session) : m_session(session), m_timer(0), m_boosting(false), m_sendPacket(false) { }

SlotEquipmentMap const* CharacterBooster::_GetCharBoostItems(std::vector<std::pair<uint32, uint32> >& itemsToMail) const
{
    for (uint8 i = 0; i < 4; i++) // BagCount
        itemsToMail.push_back(std::make_pair(EMBERSILK_BAG_ID, 1));

    itemsToMail.push_back(std::make_pair(LEMON_FLAVOUR_PUDING_ID, LEMON_FLAVOUR_PUDING_COUNT));

    switch (m_charBoostInfo.specialization)
    {
        case CHAR_SPECIALIZATION_DEATH_KNIGHT_BLOOD:
            return &deathKnightEquipmentBlood;
        case CHAR_SPECIALIZATION_DEATH_KNIGHT_FROST:
            return &deathKnightEquipmentFrost;
        case CHAR_SPECIALIZATION_DEATH_KNIGHT_UNHOLY:
            return &deathKnightEquipmentUnholy;
        case CHAR_SPECIALIZATION_DRUID_BALANCE:
            return &druidEquipmentBalance;
        case CHAR_SPECIALIZATION_DRUID_FERAL:
        case CHAR_SPECIALIZATION_DRUID_GUARDIAN:
            return &druidEquipmentFeral;
        case CHAR_SPECIALIZATION_DRUID_RESTORATION:
            return &druidEquipmentRestoration;
        case CHAR_SPECIALIZATION_HUNTER_BEAST_MASTERY:
        case CHAR_SPECIALIZATION_HUNTER_MARKSMANSHIP:
        case CHAR_SPECIALIZATION_HUNTER_SURVIVAL:
            return &hunterEquipment;
        case CHAR_SPECIALIZATION_MAGE_ARCANE:
        case CHAR_SPECIALIZATION_MAGE_FIRE:
        case CHAR_SPECIALIZATION_MAGE_FROST:
            itemsToMail.push_back(std::make_pair(101081, 1));
            return &mageEquipment;
        case CHAR_SPECIALIZATION_MONK_BREWMASTER:
            return &monkEquipmentBrewmaster;
        case CHAR_SPECIALIZATION_MONK_MISTWEAVER:
            return &monkEquipmentMistweaver;
        case CHAR_SPECIALIZATION_MONK_WINDWALKER:
            return &monkEquipmentWindwalker;
        case CHAR_SPECIALIZATION_PALADIN_HOLY:
            return &paladinEquipmentHoly;
        case CHAR_SPECIALIZATION_PALADIN_PROTECTION:
            return &paladinEquipmentProtection;
        case CHAR_SPECIALIZATION_PALADIN_RETRIBUTION:
            return &paladinEquipmentRetribution;
        case CHAR_SPECIALIZATION_PRIEST_DISCIPLINE:
        case CHAR_SPECIALIZATION_PRIEST_HOLY:
            itemsToMail.push_back(std::make_pair(101172, 1));
            return &priestEquipmentDisciplineAndHoly;
        case CHAR_SPECIALIZATION_PRIEST_SHADOW:
            itemsToMail.push_back(std::make_pair(101172, 1));
            return &priestEquipmentShadow;
        case CHAR_SPECIALIZATION_ROGUE_ASSASSINATION:
            return &rogueEquipmentAssassionation;
        case CHAR_SPECIALIZATION_ROGUE_COMBAT:
            return &rogueEquipmentCombat;
        case CHAR_SPECIALIZATION_ROGUE_SUBTLETY:
            return &rogueEquipmentCombat;
        case CHAR_SPECIALIZATION_SHAMAN_ELEMENTAL:
            return &shamanEquipmentElemental;
        case CHAR_SPECIALIZATION_SHAMAN_ENHANCEMENT:
            return &shamanEquipmentEnhancement;
        case CHAR_SPECIALIZATION_SHAMAN_RESTORATION:
            return &shamanEquipmentRestoration;
        case CHAR_SPECIALIZATION_WARLOCK_AFFLICTION:
        case CHAR_SPECIALIZATION_WARLOCK_DEMONOLOGY:
        case CHAR_SPECIALIZATION_WARLOCK_DESTRUCTION:
            itemsToMail.push_back(std::make_pair(101275, 1));
            return &warlockEquipment;
        case CHAR_SPECIALIZATION_WARRIOR_ARMS:
            return &warriorEquipmentArms;
        case CHAR_SPECIALIZATION_WARRIOR_FURY:
            return &warriorEquipmentFury;
        case CHAR_SPECIALIZATION_WARRIOR_PROTECTION:
            return &warriorEquipmentProtection;
        default:
            return NULL;
    }
}

void CharacterBooster::_SendCharBoostPacket(SlotEquipmentMap const* items) const
{
    ObjectGuid guid = m_charBoostInfo.charGuid;
    WorldPacket data(SMSG_BATTLE_CHAR_BOOST_ITEMS, 8 + 3 + items->size());

    data.WriteGuidMask(guid, 2, 0, 7, 5, 3, 4, 1);
    data.WriteBits(items->size(), 22);
    data.WriteBit(guid[6]);

    data.FlushBits();

    data.WriteGuidBytes(guid, 7, 2, 6, 5);

    for (SlotEquipmentMap::const_iterator itr = items->begin(); itr != items->end(); itr++)
        data << (uint32)itr->second;

    data.WriteGuidBytes(guid, 0, 1, 3, 4);

    m_session->SendPacket(&data);
}

uint32 CharacterBooster::_PrepareMail(SQLTransaction& trans, std::string const& subject, std::string const& body) const
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL);
    uint32 mailId = sObjectMgr->GenerateMailID();

    stmt->setUInt32(0,  mailId);
    stmt->setUInt8(1,   MAIL_NORMAL);
    stmt->setInt8(2,    MAIL_STATIONERY_DEFAULT);
    stmt->setUInt16(3,  0);
    stmt->setUInt32(4,  m_charBoostInfo.lowGuid);
    stmt->setUInt32(5,  m_charBoostInfo.lowGuid);
    stmt->setString(6,  subject);
    stmt->setString(7,  body);
    stmt->setBool(8,    true);
    stmt->setUInt64(9,  time(NULL) + 180 * DAY);
    stmt->setUInt64(10, time(NULL));
    stmt->setUInt32(11, 0);
    stmt->setUInt32(12, 0);
    stmt->setUInt8(13,  0);

    trans->Append(stmt);

    return mailId;
}

void CharacterBooster::_SendMail(SQLTransaction& trans, std::vector<std::pair<uint32, uint32> > const& items) const
{
    if (items.empty())
        return;

    uint32 mailId = _PrepareMail(trans, CHARRACTER_BOOST_EQUIPED_ITEMS_MAIL_SUBJECT, CHARRACTER_BOOST_EQUIPED_ITEMS_MAIL_BODY);
    PreparedStatement* stmt = NULL;

    for (uint8 i = 0; i < items.size(); i++)
    {
        if (Item* item = Item::CreateItem(items[i].first, items[i].second, m_charBoostInfo.charGuid))
        {
            item->SetBinding(true);
            item->SaveToDB(trans);
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL_ITEM);

            stmt->setUInt32(0, mailId);
            stmt->setUInt32(1, item->GetGUIDLow());
            stmt->setUInt32(2, m_charBoostInfo.lowGuid);

            trans->Append(stmt);
        }
    }
}

void CharacterBooster::_MailEquipedItems(SQLTransaction& trans) const
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_INVENTORY_ITEM_GUID_UNTIL_BAG_SLOT);
    stmt->setUInt32(0, 0);
    stmt->setUInt8(1, EQUIPMENT_SLOT_END);
    stmt->setUInt32(2, m_charBoostInfo.lowGuid);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
        return;

    uint32 mailId = _PrepareMail(trans, CHARRACTER_BOOST_EQUIPED_ITEMS_MAIL_SUBJECT, CHARRACTER_BOOST_EQUIPED_ITEMS_MAIL_BODY);
    uint32 itemCount = 0;

    do
    {
        if (itemCount++ > 10)
        {
            itemCount = 0;
            mailId = _PrepareMail(trans, CHARRACTER_BOOST_EQUIPED_ITEMS_MAIL_SUBJECT, CHARRACTER_BOOST_EQUIPED_ITEMS_MAIL_BODY);
        }

        uint32 itemGuid = (*result)[0].GetUInt32();
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_MAIL_ITEM);

        stmt->setUInt32(0, mailId);
        stmt->setUInt32(1, itemGuid);
        stmt->setUInt32(2, m_charBoostInfo.lowGuid);

        trans->Append(stmt);
    }
    while (result->NextRow());

    // unEquip after sending
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_INVENTORY_UNTIL_BAG_SLOT);

    stmt->setUInt32(0, 0);
    stmt->setUInt8(1,  EQUIPMENT_SLOT_END);
    stmt->setUInt32(2, m_charBoostInfo.lowGuid);

    trans->Append(stmt);
}

std::string CharacterBooster::_SetSpecialization(SQLTransaction& trans, uint8 const& classId) const
{
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_TALENT);
    stmt->setUInt32(0, m_charBoostInfo.lowGuid);
    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
    {
        do
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SPELL_BY_SPELL);
            stmt->setUInt32(0, (*result)[0].GetUInt32());
            stmt->setUInt32(1, m_charBoostInfo.lowGuid);
            trans->Append(stmt);
        }
        while (result->NextRow());
    }

    if (uint32 const* specs = GetClassSpecializations(classId))
    {
        for (uint8 i = 0; i < MAX_SPECIALIZATIONS; i++)
        {
            if (std::vector<uint32> const* spells = GetSpecializationSpells(specs[i]))
            {
                for (std::vector<uint32>::const_iterator iter = spells->begin(); iter != spells->end(); iter++)
                {
                    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SPELL_BY_SPELL);
                    stmt->setUInt32(0, *iter);
                    stmt->setUInt32(1, m_charBoostInfo.lowGuid);
                    trans->Append(stmt);
                }
            }
        }
    }

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_TALENT);
    stmt->setUInt32(0, m_charBoostInfo.lowGuid);
    trans->Append(stmt);

    std::ostringstream talentTree;
    talentTree << m_charBoostInfo.specialization << " 0 ";
    return talentTree.str();
}

void CharacterBooster::_LearnSpells(SQLTransaction& trans, uint8 const& raceId, uint8 const& classId) const
{
    PreparedStatement* stmt;
    uint32 const* languageSpells = NULL;

    if (raceId == RACE_PANDAREN_HORDE)
        languageSpells = pandarenLanguageSpellsHorde;
    else if (raceId == RACE_PANDAREN_ALLIANCE)
        languageSpells = pandarenLanguageSpellsAlliance;

    if (languageSpells)
    {
        for (uint8 i = 0; i < PANDAREN_FACTION_LANGUAGE_COUNT; i++)
        {
            stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SPELL);

            stmt->setUInt32(0, m_charBoostInfo.lowGuid);
            stmt->setUInt32(1, languageSpells[i]);
            stmt->setBool(2,   true);
            stmt->setBool(3,   false);

            trans->Append(stmt);
        }
    }

    std::vector<uint32> spellsToLearn(CharBoostSpells);
    spellsToLearn.push_back(m_charBoostInfo.allianceFaction ? SWIFT_PURPLE_GRYPGON_SPELL : SWIFT_PURPLE_WIND_RIDER_SPELL);

    switch (classId)
    {
        case CLASS_WARRIOR:
        case CLASS_PALADIN:
            spellsToLearn.push_back(PLATE_MAIL_ARMOR_SPELL);
            break;
        default:
            break;
    }

    for (uint8 i = 0; i < spellsToLearn.size(); i++)
    {
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SPELL);

        stmt->setUInt32(0, m_charBoostInfo.lowGuid);
        stmt->setUInt32(1, spellsToLearn[i]);
        stmt->setBool(2,   true);
        stmt->setBool(3,   false);

        trans->Append(stmt);
    }
}

uint8 CharacterBooster::_GetRace() const
{
    uint8 charRace = 0;
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_RACE);
    stmt->setUInt32(0, m_charBoostInfo.lowGuid);

    if (PreparedQueryResult result = CharacterDatabase.Query(stmt))
    {
        charRace = (*result)[0].GetUInt8();
        if (charRace == RACE_PANDAREN_NEUTRAL)
            charRace = m_charBoostInfo.allianceFaction ? RACE_PANDAREN_ALLIANCE : RACE_PANDAREN_HORDE;
    }

    return charRace;
}

std::string CharacterBooster::_EquipItems(SQLTransaction& trans, SlotEquipmentMap const* itemsToEquip) const
{
    SlotEquipmentMap::const_iterator itr;
    std::ostringstream items;
    PreparedStatement* stmt;
    for (uint8 i = 0; i < INVENTORY_SLOT_BAG_END; i++)
    {
        itr = itemsToEquip->find(i);
        if (itr != itemsToEquip->end())
        {
            if (Item* item = Item::CreateItem(itr->second, 1, m_charBoostInfo.charGuid))
            {
                item->SetBinding(true);
                item->SaveToDB(trans);

                stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_INVENTORY);

                stmt->setUInt32(0, m_charBoostInfo.lowGuid);
                stmt->setUInt32(1, 0);
                stmt->setUInt8(2,  itr->first);
                stmt->setUInt32(3, item->GetGUIDLow());

                trans->Append(stmt);

                items << (itr->second) << " 0 ";
            }
            else
                items << "0 0 ";
        }
        else
            items << "0 0 ";
    }

    return items.str();
}

void CharacterBooster::_SaveBoostedChar(SQLTransaction& trans, std::string const& items, uint8 const& raceId, uint8 const& classId) const
{
    float const* position = m_charBoostInfo.allianceFaction ? startPosition[1] : startPosition[0];
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHARACTER_FOR_BOOST);

    stmt->setUInt8(0,  raceId);
    stmt->setFloat(1,  position[0]);
    stmt->setFloat(2,  position[1]);
    stmt->setFloat(3,  position[2]);
    stmt->setFloat(4,  position[3]);
    stmt->setUInt16(5, VALE_OF_ETERNAL_BLOSSOMS_MAP_ID);
    stmt->setString(6, _SetSpecialization(trans, classId));
    stmt->setUInt16(7, AT_LOGIN_FIRST);
    stmt->setString(8, items);
    stmt->setUInt32(9, m_charBoostInfo.lowGuid);

    trans->Append(stmt);
}

void CharacterBooster::_HandleCharacterBoost() const
{
    uint8 classId = 0;

    if (ChrSpecializationEntry const* specEntry = sChrSpecializationStore.LookupEntry(m_charBoostInfo.specialization))
        classId = specEntry->classId;

    if (!classId)
        return;

    uint8 raceId = _GetRace();
    if (!raceId)
        return;

    std::vector<std::pair<uint32, uint32> > itemsToMail;
    SlotEquipmentMap const* itemsToEquip = _GetCharBoostItems(itemsToMail);
    if (!itemsToEquip)
        return;

    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    _MailEquipedItems(trans);
    _SendMail(trans, itemsToMail);
    _LearnSpells(trans, raceId, classId);
    _SaveBoostedChar(trans, _EquipItems(trans, itemsToEquip), raceId, classId);
    CharacterDatabase.CommitTransaction(trans);
    m_session->SetBoosting(false);
    _SendCharBoostPacket(itemsToEquip);
}

void CharacterBooster::HandleCharacterBoost()
{
    if (!m_charBoostInfo.charGuid)
        return;

    switch (m_charBoostInfo.action)
    {
        case CHARACTER_BOOST_ITEMS:
            m_session->SendBattlePayDistributionUpdate(m_charBoostInfo.charGuid, CHARACTER_BOOST, m_charBoostInfo.action,
                                                       CHARACTER_BOOST_TEXT_ID, CHARACTER_BOOST_BONUS_TEXT, CHARACTER_BOOST_BONUS_TEXT2);
            m_charBoostInfo.action = CHARACTER_BOOST_APPLIED;
            m_timer = 500;
            m_sendPacket = true;
            break;
        case CHARACTER_BOOST_APPLIED:
            m_session->SendBattlePayDistributionUpdate(m_charBoostInfo.charGuid, CHARACTER_BOOST, m_charBoostInfo.action,
                                                       CHARACTER_BOOST_TEXT_ID, CHARACTER_BOOST_BONUS_TEXT, CHARACTER_BOOST_BONUS_TEXT2);
            m_charBoostInfo = CharacterBoostData();
            break;
        default:
            break;
    }
}

void CharacterBooster::SetBoostedCharInfo(uint64 guid, uint32 action, uint32 specialization, bool allianceFaction)
{
    m_boosting                      = true;
    m_charBoostInfo.charGuid        = guid;
    m_charBoostInfo.lowGuid         = GUID_LOPART(guid);
    m_charBoostInfo.action          = action;
    m_charBoostInfo.specialization  = specialization;
    m_charBoostInfo.allianceFaction = allianceFaction;
}

void CharacterBooster::Update(uint32 diff)
{
    if (m_sendPacket)
    {
        if (m_timer <= diff)
        {
            m_sendPacket = false;
            _HandleCharacterBoost();
        }
        else
            m_timer -= diff;
    }
}