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

#include "Common.h"
#include "Language.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Guild.h"
#include "GossipDef.h"
#include "SocialMgr.h"

#define CHARTER_DISPLAY_ID 16161

// Charters ID in item_template
enum CharterItemIDs
{
    GUILD_CHARTER                                 = 5863,
};

enum CharterCosts
{
    GUILD_CHARTER_COST                            = 1000,
};

void WorldSession::HandlePetitionBuyOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode CMSG_PETITION_BUY");

    ObjectGuid guid;

    recvData.ReadGuidMask(guid, 5, 2, 3);

    uint8 nameLength = recvData.ReadBits(7);

    recvData.ReadGuidMask(guid, 4, 1, 7, 0, 6);

    std::string name = recvData.ReadString(nameLength);

    recvData.ReadGuidBytes(guid, 1, 7, 4, 6, 0, 5, 2, 3);
    TC_LOG_DEBUG("network", "Petitioner with GUID %u tried sell petition: name %s", GUID_LOPART(guid), name.c_str());

    // prevent cheating
    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_PETITIONER);
    if (!creature)
    {
        TC_LOG_DEBUG("network", "WORLD: HandlePetitionBuyOpcode - Unit (GUID: %u) not found or you can't interact with him.", GUID_LOPART(guid));
        return;
    }

    // remove fake death
    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    uint32 charterid = 0;
    uint32 cost = 0;
    uint32 type = 0;

    // if tabard designer, then trying to buy a guild charter.
    // do not let if already in guild.
    if (_player->GetGuildId())
        return;

    charterid = GUILD_CHARTER;
    cost = GUILD_CHARTER_COST;
    type = GUILD_CHARTER_TYPE;

    if (sGuildMgr->GetGuildByName(name))
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_CREATE, ERR_GUILD_NAME_EXISTS_S, name);
        return;
    }

    if (sObjectMgr->IsReservedName(name) || !ObjectMgr::IsValidCharterName(name))
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_CREATE, ERR_GUILD_NAME_INVALID, name);
        return;
    }

    ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(charterid);
    if (!pProto)
    {
        _player->SendBuyError(BUY_ERR_CANT_FIND_ITEM, NULL, charterid, 0);
        return;
    }

    if (!_player->HasEnoughMoney(uint64(cost)))
    {                                                       //player hasn't got enough money
        _player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, creature, charterid, 0);
        return;
    }

    ItemPosCountVec dest;
    InventoryResult msg = _player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, charterid, pProto->BuyCount);
    if (msg != EQUIP_ERR_OK)
    {
        _player->SendEquipError(msg, NULL, NULL, charterid);
        return;
    }

    _player->ModifyMoney(-(int32)cost);
    Item* charter = _player->StoreNewItem(dest, charterid, true);
    if (!charter)
        return;

    //I think this has changed
    charter->SetUInt32Value(ITEM_FIELD_ENCHANTMENT, charter->GetGUIDLow());
    // ITEM_FIELD_ENCHANTMENT is guild/arenateam id
    // ITEM_FIELD_ENCHANTMENT+1 is current signatures count (showed on item)
    charter->SetState(ITEM_CHANGED, _player);
    _player->SendNewItem(charter, 1, true, false);

    // a petition is invalid, if both the owner and the type matches
    // we checked above, if this player is in an arenateam, so this must be
    // datacorruption
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_BY_OWNER);
    stmt->setUInt32(0, _player->GetGUIDLow());
    stmt->setUInt8(1, type);
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    std::ostringstream ssInvalidPetitionGUIDs;

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            ssInvalidPetitionGUIDs << '\'' << fields[0].GetUInt32() << "', ";
        } while (result->NextRow());
    }

    // delete petitions with the same guid as this one
    ssInvalidPetitionGUIDs << '\'' << charter->GetGUIDLow() << '\'';

    //Probably DB Charter Petitions has changed too
    TC_LOG_DEBUG("network", "Invalid petition GUIDs: %s", ssInvalidPetitionGUIDs.str().c_str());
    CharacterDatabase.EscapeString(name);
    SQLTransaction trans = CharacterDatabase.BeginTransaction();
    trans->PAppend("DELETE FROM petition WHERE petitionguid IN (%s)",  ssInvalidPetitionGUIDs.str().c_str());
    trans->PAppend("DELETE FROM petition_sign WHERE petitionguid IN (%s)", ssInvalidPetitionGUIDs.str().c_str());

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PETITION);
    stmt->setUInt32(0, _player->GetGUIDLow());
    stmt->setUInt32(1, charter->GetGUIDLow());
    stmt->setString(2, name);
    stmt->setUInt8(3, uint8(type)); //Type no needed anymore
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
}

void WorldSession::HandlePetitionShowSignOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode CMSG_PETITION_SHOW_SIGNATURES");

    uint8 playerCount = 0;
    ObjectGuid petitionGuid;

    recvData.ReadGuidMask(petitionGuid, 3, 7, 2, 4, 5, 6, 0, 1);

    recvData.ReadGuidBytes(petitionGuid, 2, 4, 5, 7, 1, 0, 3, 6);

    // solve (possible) some strange compile problems with explicit use GUID_LOPART(petitionguid) at some GCC versions (wrong code optimization in compiler?)
    uint32 petitionGuidLow = GUID_LOPART(petitionGuid);

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_TYPE);

    stmt->setUInt32(0, petitionGuidLow);

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
    {
        TC_LOG_DEBUG("entities.player.items", "Petition %u is not found for player %u %s", GUID_LOPART(petitionGuid), GetPlayer()->GetGUIDLow(), GetPlayer()->GetName().c_str());
        return;
    }

    Field* fields = result->Fetch();
    uint32 type = fields[0].GetUInt8();

    // if guild petition and has guild => error, return;
    if (type == GUILD_CHARTER_TYPE && _player->GetGuildId())
        return;

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURE);

    stmt->setUInt32(0, petitionGuidLow);

    result = CharacterDatabase.Query(stmt);

    // result == NULL also correct in case no sign yet
    if (result)
        playerCount = uint8(result->GetRowCount());

    TC_LOG_DEBUG("network", "CMSG_PETITION_SHOW_SIGNATURES petition entry: '%u'", petitionGuidLow);

    ObjectGuid playerGuid = _player->GetGUID();
    ObjectGuid* playerGuids = new ObjectGuid[playerCount];

    for (uint8 i = 0; i < playerCount; ++i)
    {
        Field* fields2 = result->Fetch();
        uint32 lowGuid = fields2[0].GetUInt32();
        playerGuids[i] = MAKE_NEW_GUID(lowGuid, 0, HIGHGUID_PLAYER);
        result->NextRow();
    }

    WorldPacket data(SMSG_PETITION_SHOW_SIGNATURES, (9 + 9 + 3 + 4 + playerCount * (9 + 4)));
    data.WriteBit(playerGuid[1]);
    data.WriteBit(petitionGuid[3]);
    data.WriteBit(playerGuid[3]);
    data.WriteGuidMask(petitionGuid, 4, 0);
    data.WriteGuidMask(playerGuid, 7, 5);
    data.WriteGuidMask(petitionGuid, 1, 5, 7);
    data.WriteGuidMask(playerGuid, 0, 6);
    data.WriteBit(petitionGuid[6]);
    data.WriteGuidMask(playerGuid, 2, 4);
    data.WriteBits(playerCount, 21);

    for (int i = 0; i < playerCount; i++)
    {
        data.WriteGuidMask(playerGuids[i], 2, 0, 4, 7, 5, 1, 6, 3);
    }

    data.WriteBit(petitionGuid[2]);
    data.FlushBits();

    for (int i = 0; i < playerCount; i++)
    {
        data.WriteGuidBytes(playerGuids[i], 6, 0, 1, 3, 2, 5, 7, 4);
        data << uint32(1); // Choice ??? Blizzard also stores declined players ???
    }

    data.WriteGuidBytes(petitionGuid, 6, 5, 4);
    data.WriteByteSeq(playerGuid[4]);
    data.WriteByteSeq(petitionGuid[1]);
    data << uint32(petitionGuidLow); // guildID
    data.WriteGuidBytes(petitionGuid, 2, 3, 7);
    data.WriteGuidBytes(playerGuid, 5, 6, 3, 7, 1, 0);
    data.WriteByteSeq(petitionGuid[0]);
    data.WriteByteSeq(playerGuid[2]);

    delete[] playerGuids;
    SendPacket(&data);
}

void WorldSession::HandlePetitionQueryOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode CMSG_PETITION_QUERY");   // ok

    uint32 guildguid;
    ObjectGuid petitionGuid;

    recvData >> guildguid;                                 // in Trinity always same as GUID_LOPART(petitionguid)

    recvData.ReadGuidMask(petitionGuid, 2, 3, 1, 0, 4, 7, 6, 5);

    recvData.ReadGuidBytes(petitionGuid, 0, 4, 7, 5, 1, 6, 3, 2);

    TC_LOG_DEBUG("network", "CMSG_PETITION_QUERY Petition GUID %u Guild GUID %u", GUID_LOPART(petitionGuid), guildguid);

    SendPetitionQueryOpcode(petitionGuid);
}

void WorldSession::SendPetitionQueryOpcode(uint64 petitionGuid)
{
    ObjectGuid ownerGuid = 0;
    uint32 type;
    std::string name = "NO_NAME_FOR_GUID";

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION);

    stmt->setUInt32(0, GUID_LOPART(petitionGuid));

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        Field* fields = result->Fetch();
        ownerGuid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);
        name      = fields[1].GetString();
        type      = fields[2].GetUInt8();
    }
    else
    {
        TC_LOG_DEBUG("network", "CMSG_PETITION_QUERY failed for petition (GUID: %u)", GUID_LOPART(petitionGuid));
        return;
    }

    WorldPacket data(SMSG_PETITION_QUERY_RESPONSE, (4+8+name.size()+1+1+4*12+2+10));
    data << uint32(GUID_LOPART(petitionGuid));              // guild/team guid (in Trinity always same as GUID_LOPART(petition guid)
    data.WriteBit(1); // hasData;

    for (int i = 0; i < 10; i++)
        data.WriteBits(0, 6); // Unk strings;

    data.WriteGuidMask(ownerGuid, 2, 4);
    data.WriteBits(0, 12);
    data.WriteGuidMask(ownerGuid, 0, 7, 3, 6, 5);
    data.WriteBits(name.size(), 7); //NameLen
    data.WriteBit(ownerGuid[1]);
    data.FlushBits();

    data.WriteByteSeq(ownerGuid[5]);
    data << uint32(0);
    data.WriteString(name);
    data << uint32(0);
    data.WriteByteSeq(ownerGuid[4]);
    data << uint32(type); // Type - 4 guild
    data.WriteByteSeq(ownerGuid[6]);
    data << uint32(0);
    data << uint32(sWorld->getIntConfig(CONFIG_MIN_PETITION_SIGNS)); // Required sign count
    data.WriteGuidBytes(ownerGuid, 1, 7, 0);
    data << uint32(0);
    data << uint32(0);
    data.WriteByteSeq(ownerGuid[2]);
    data << uint32(time(NULL) + YEAR); // Deadline
    data << uint16(0);
    data << uint32(0);
    data.WriteByteSeq(ownerGuid[3]);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);
    data << uint32(0);

    SendPacket(&data);
}

void WorldSession::HandlePetitionRenameOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode CMSG_PETITION_RENAME");

    ObjectGuid petitionGuid;
    uint32 type;
    uint8 nameLen;
    std::string newName;

    nameLen = recvData.ReadBits(7);
    recvData.ReadGuidMask(petitionGuid, 7, 4, 6, 2, 0, 5, 3, 1);

    recvData.ReadGuidBytes(petitionGuid, 4, 1, 7);
    newName = recvData.ReadString(nameLen);
    recvData.ReadGuidBytes(petitionGuid, 0, 3, 2, 6, 5);

    Item* item = _player->GetItemByGuid(petitionGuid);
    if (!item)
        return;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_TYPE);

    stmt->setUInt32(0, GUID_LOPART(petitionGuid));

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        Field* fields = result->Fetch();
        type = fields[0].GetUInt8();
    }
    else
    {
        TC_LOG_DEBUG("network", "CMSG_PETITION_QUERY failed for petition (GUID: %u)", GUID_LOPART(petitionGuid));
        return;
    }

    if (sGuildMgr->GetGuildByName(newName))
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_CREATE, ERR_GUILD_NAME_EXISTS_S, newName);
        return;
    }
    if (sObjectMgr->IsReservedName(newName) || !ObjectMgr::IsValidCharterName(newName))
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_CREATE, ERR_GUILD_NAME_INVALID, newName);
        return;
    }

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_PETITION_NAME);

    stmt->setString(0, newName);
    stmt->setUInt32(1, GUID_LOPART(petitionGuid));

    CharacterDatabase.Execute(stmt);

    TC_LOG_DEBUG("network", "Petition (GUID: %u) renamed to '%s'", GUID_LOPART(petitionGuid), newName.c_str());

    WorldPacket data(SMSG_PETITION_RENAME_RESULT, (9 + 1 + newName.size()));
    data.WriteBits(newName.length(), 7);
    data.WriteGuidMask(petitionGuid, 0, 3, 4, 2, 6, 5, 7, 1);

    data.WriteGuidBytes(petitionGuid, 4, 3, 6, 0, 5, 2, 1, 7);
    data.WriteString(newName);
    SendPacket(&data);
}

void WorldSession::HandlePetitionSignOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode CMSG_PETITION_SIGN");    // ok

    Field* fields;
    ObjectGuid petitionGuid;

    recvData.read_skip<uint8>();
    recvData.ReadGuidMask(petitionGuid, 4, 2, 0, 1, 5, 3, 6, 7);

    recvData.ReadGuidBytes(petitionGuid, 6, 1, 7, 2, 5, 3, 0, 4);

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURES);

    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    stmt->setUInt32(1, GUID_LOPART(petitionGuid));

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
    {
        TC_LOG_ERROR("network", "Petition %u is not found for player %u %s", GUID_LOPART(petitionGuid), GetPlayer()->GetGUIDLow(), GetPlayer()->GetName().c_str());
        return;
    }

    fields = result->Fetch();
    uint64 ownerGuid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);
    uint64 signs = fields[1].GetUInt64();
    uint8 type = fields[2].GetUInt8();

    uint32 playerGuid = _player->GetGUIDLow();
    if (GUID_LOPART(ownerGuid) == playerGuid)
        return;

    // not let enemies sign guild charter
    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD) && GetPlayer()->GetTeam() != sObjectMgr->GetPlayerTeamByGUID(ownerGuid))
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_CREATE, ERR_GUILD_NOT_ALLIED);
        return;
    }

    if (_player->GetGuildId())
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_INVITE, ERR_ALREADY_IN_GUILD_S, _player->GetName());
        return;
    }
    if (_player->GetGuildIdInvited())
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_INVITE, ERR_ALREADY_INVITED_TO_GUILD_S, _player->GetName());
        return;
    }

    if (++signs > type)                                        // client signs maximum
        return;

    // Client doesn't allow to sign petition two times by one character, but not check sign by another character from same account
    // not allow sign another player from already sign player account
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIG_BY_ACCOUNT);

    stmt->setUInt32(0, GetAccountId());
    stmt->setUInt32(1, GUID_LOPART(petitionGuid));

    result = CharacterDatabase.Query(stmt);

    if (result)
    {
        // close at signer side
        SendPetitionSignResults(petitionGuid, _player->GetGUID(), PETITION_SIGN_ALREADY_SIGNED);
        return;
    }

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_PETITION_SIGNATURE);

    stmt->setUInt32(0, GUID_LOPART(ownerGuid));
    stmt->setUInt32(1, GUID_LOPART(petitionGuid));
    stmt->setUInt32(2, playerGuid);
    stmt->setUInt32(3, GetAccountId());

    CharacterDatabase.Execute(stmt);

    TC_LOG_DEBUG("network", "PETITION SIGN: GUID %u by player: %s (GUID: %u Account: %u)", GUID_LOPART(petitionGuid), _player->GetName().c_str(), playerGuid, GetAccountId());

    // close at signer side
    SendPetitionSignResults(petitionGuid, _player->GetGUID(), PETITION_SIGN_OK);

    // update signs count on charter, required testing...
    //Item* item = _player->GetItemByGuid(petitionguid));
    //if (item)
    //    item->SetUInt32Value(ITEM_FIELD_ENCHANTMENT+1, signs);

    // update for owner if online
    if (Player* owner = ObjectAccessor::FindPlayer(ownerGuid))
        owner->GetSession()->SendPetitionSignResults(petitionGuid, _player->GetGUID(), PETITION_SIGN_OK);
}

void WorldSession::HandlePetitionDeclineOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode MSG_PETITION_DECLINE");  // ok

    ObjectGuid petitionGuid;
    uint64 ownerGuid;

    recvData.ReadGuidMask(petitionGuid, 5, 6, 4, 3, 1, 7, 0, 2);

    recvData.ReadGuidBytes(petitionGuid, 6, 2, 1, 5, 0, 7, 4, 3);

    TC_LOG_DEBUG("network", "Petition %u declined by %u", GUID_LOPART(petitionGuid), _player->GetGUIDLow());

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_OWNER_BY_GUID);

    stmt->setUInt32(0, GUID_LOPART(petitionGuid));

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
        return;

    Field* fields = result->Fetch();
    ownerGuid = MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER);

    Player* owner = ObjectAccessor::FindPlayer(ownerGuid);
    if (owner)                                               // petition owner online
        owner->GetSession()->SendPetitionSignResults(petitionGuid, _player->GetGUID(), PETITION_SIGN_OK);
}

void WorldSession::HandleOfferPetitionOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode CMSG_OFFER_PETITION");   // ok

    uint8 playerCount = 0;
    ObjectGuid petitionGuid, playerGuid;
    uint32 type;
    Player* player;

    recvData.read_skip<uint32>();

    recvData.ReadGuidMask(playerGuid, 4, 1);
    petitionGuid[2] = recvData.ReadBit();
    playerGuid[6] = recvData.ReadBit();
    petitionGuid[1] = recvData.ReadBit();
    playerGuid[2] = recvData.ReadBit();
    petitionGuid[4] = recvData.ReadBit();
    recvData.ReadGuidMask(playerGuid, 3, 7);
    recvData.ReadGuidMask(petitionGuid, 0, 6);
    recvData.ReadGuidMask(playerGuid, 5, 0);
    recvData.ReadGuidMask(petitionGuid, 3, 5, 7);

    recvData.ReadByteSeq(playerGuid[7]);
    recvData.ReadGuidBytes(petitionGuid, 1, 4, 2);
    recvData.ReadByteSeq(playerGuid[6]);
    recvData.ReadGuidBytes(petitionGuid, 3, 0, 5);
    recvData.ReadGuidBytes(playerGuid, 0, 2, 5, 3, 4);
    recvData.ReadByteSeq(petitionGuid[7]);
    recvData.ReadByteSeq(playerGuid[1]);
    recvData.ReadByteSeq(petitionGuid[6]);

    player = ObjectAccessor::FindPlayer(playerGuid);
    if (!player)
        return;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_TYPE);

    stmt->setUInt32(0, GUID_LOPART(petitionGuid));

    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (!result)
        return;

    Field* fields = result->Fetch();
    type = fields[0].GetUInt8();

    TC_LOG_DEBUG("network", "OFFER PETITION: type %u, GUID1 %u, to player id: %u", type, GUID_LOPART(petitionGuid), GUID_LOPART(playerGuid));

    if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD) && GetPlayer()->GetTeam() != player->GetTeam())
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_CREATE, ERR_GUILD_NOT_ALLIED);
        return;
    }

    if (player->GetGuildId())
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_INVITE, ERR_ALREADY_IN_GUILD_S, _player->GetName());
        return;
    }

    if (player->GetGuildIdInvited())
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_INVITE, ERR_ALREADY_INVITED_TO_GUILD_S, _player->GetName());
        return;
    }

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURE);

    stmt->setUInt32(0, GUID_LOPART(petitionGuid));

    result = CharacterDatabase.Query(stmt);

    // result == NULL also correct charter without signs
    if (result)
        playerCount = uint8(result->GetRowCount());

    ObjectGuid* playerGuids = new ObjectGuid[playerCount];

    for (uint8 i = 0; i < playerCount; ++i)
    {
        Field* fields2 = result->Fetch();
        uint32 lowGuid = fields2[0].GetUInt32();
        playerGuids[i] = MAKE_NEW_GUID(lowGuid, 0, HIGHGUID_PLAYER);
        result->NextRow();
    }

    WorldPacket data(SMSG_PETITION_SHOW_SIGNATURES, (9 + 9 + 3 + 4 + playerCount * (9 + 4)));
    data.WriteBit(playerGuid[1]);
    data.WriteBit(petitionGuid[3]);
    data.WriteBit(playerGuid[3]);
    data.WriteGuidMask(petitionGuid, 4, 0);
    data.WriteGuidMask(playerGuid, 7, 5);
    data.WriteGuidMask(petitionGuid, 1, 5, 7);
    data.WriteGuidMask(playerGuid, 0, 6);
    data.WriteBit(petitionGuid[6]);
    data.WriteGuidMask(playerGuid, 2, 4);
    data.WriteBits(playerCount, 21);

    for (int i = 0; i < playerCount; i++)
    {
        data.WriteGuidMask(playerGuids[i], 2, 0, 4, 7, 5, 1, 6, 3);
    }

    data.WriteBit(petitionGuid[2]);
    data.FlushBits();

    for (int i = 0; i < playerCount; i++)
    {
        data.WriteGuidBytes(playerGuids[i], 6, 0, 1, 3, 2, 5, 7, 4);
        data << uint32(1); // Choice ??? Blizzard also stores declined players ???
    }

    data.WriteGuidBytes(petitionGuid, 6, 5, 4);
    data.WriteByteSeq(playerGuid[4]);
    data.WriteByteSeq(petitionGuid[1]);
    data << uint32(GUID_LOPART(petitionGuid)); // guildID
    data.WriteGuidBytes(petitionGuid, 2, 3, 7);
    data.WriteGuidBytes(playerGuid, 5, 6, 3, 7, 1, 0);
    data.WriteByteSeq(petitionGuid[0]);
    data.WriteByteSeq(playerGuid[2]);

    delete[] playerGuids;
    player->GetSession()->SendPacket(&data);
}

void WorldSession::HandleTurnInPetitionOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received opcode CMSG_TURN_IN_PETITION");

    // Get petition guid from packet
    WorldPacket data;
    ObjectGuid petitionGuid;

    recvData.ReadGuidMask(petitionGuid, 1, 2, 3, 0, 5, 7, 4, 6);

    recvData.ReadGuidBytes(petitionGuid, 2, 1, 4, 6, 0, 7, 5, 3);

    // Check if player really has the required petition charter
    Item* item = _player->GetItemByGuid(petitionGuid);
    if (!item)
        return;

    TC_LOG_DEBUG("network", "Petition %u turned in by %u", GUID_LOPART(petitionGuid), _player->GetGUIDLow());

    // Get petition data from db
    uint32 ownerguidlo;
    uint32 type;
    std::string name;

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION);
    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    PreparedQueryResult result = CharacterDatabase.Query(stmt);

    if (result)
    {
        Field* fields = result->Fetch();
        ownerguidlo = fields[0].GetUInt32();
        name = fields[1].GetString();
        type = fields[2].GetUInt8();
    }
    else
    {
        TC_LOG_ERROR("network", "Player %s (guid: %u) tried to turn in petition (guid: %u) that is not present in the database", _player->GetName().c_str(), _player->GetGUIDLow(), GUID_LOPART(petitionGuid));
        return;
    }

    // Only the petition owner can turn in the petition
    if (_player->GetGUIDLow() != ownerguidlo)
        return;

    // Petition type (guild/arena) specific checks
    // Check if player is already in a guild
    if (_player->GetGuildId())
    {
        data.Initialize(SMSG_TURN_IN_PETITION_RESULTS, 1);
        data.WriteBits(PETITION_TURN_ALREADY_IN_GUILD, 4);
        data.FlushBits();
        _player->GetSession()->SendPacket(&data);
        return;
    }

    // Check if guild name is already taken
    if (sGuildMgr->GetGuildByName(name))
    {
        Guild::SendCommandResult(this, GUILD_COMMAND_CREATE, ERR_GUILD_NAME_EXISTS_S, name);
        return;
    }

    // Get petition signatures from db
    uint8 signatures;

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PETITION_SIGNATURE);
    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    result = CharacterDatabase.Query(stmt);

    if (result)
        signatures = uint8(result->GetRowCount());
    else
        signatures = 0;

    uint32 requiredSignatures;
    if (type == GUILD_CHARTER_TYPE)
        requiredSignatures = sWorld->getIntConfig(CONFIG_MIN_PETITION_SIGNS);
    else
        requiredSignatures = type-1;

    // Notify player if signatures are missing
    if (signatures < requiredSignatures)
    {
        data.Initialize(SMSG_TURN_IN_PETITION_RESULTS, 1);
        data.WriteBits(PETITION_TURN_NEED_MORE_SIGNATURES, 4);
        data.FlushBits();
        SendPacket(&data);
        return;
    }

    // Proceed with guild/arena team creation

    // Delete charter item
    _player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);

    // Create guild
    Guild* guild = new Guild;

    if (!guild->Create(_player, name))
    {
        delete guild;
        return;
    }

    // Register guild and add guild master
    sGuildMgr->AddGuild(guild);

    Guild::SendCommandResult(this, GUILD_COMMAND_CREATE, ERR_GUILD_COMMAND_SUCCESS, name);

    // Add members from signatures
    for (uint8 i = 0; i < signatures; ++i)
    {
        Field* fields = result->Fetch();
        guild->AddMember(MAKE_NEW_GUID(fields[0].GetUInt32(), 0, HIGHGUID_PLAYER));
        result->NextRow();
    }

    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PETITION_BY_GUID);
    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PETITION_SIGNATURE_BY_GUID);
    stmt->setUInt32(0, GUID_LOPART(petitionGuid));
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);

    // created
    TC_LOG_DEBUG("network", "TURN IN PETITION GUID %u", GUID_LOPART(petitionGuid));

    data.Initialize(SMSG_TURN_IN_PETITION_RESULTS, 1);
    data.WriteBits(PETITION_TURN_OK, 4);
    data.FlushBits();
    SendPacket(&data);
}

void WorldSession::HandlePetitionShowListOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "Received CMSG_PETITION_SHOWLIST");

    ObjectGuid guid;

    recvData.ReadGuidMask(guid, 1, 7, 2, 5, 4, 0, 3, 6);

    recvData.ReadGuidBytes(guid, 6, 3, 2, 4, 1, 7, 5, 0);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_PETITIONER);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: HandlePetitionerShowListOpcode - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(guid)));
        return;
    }

    SendPetitionShowList(guid);
}

void WorldSession::SendPetitionShowList(uint64 guid)
{
    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(guid, UNIT_NPC_FLAG_PETITIONER);
    if (!creature)
    {
        TC_LOG_DEBUG("network", "WORLD: HandlePetitionShowListOpcode - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(guid)));
        return;
    }

    ObjectGuid npcGuid = guid;

    WorldPacket data(SMSG_PETITION_SHOWLIST, 9 + 4);
    data.WriteGuidMask(npcGuid, 3, 5, 7, 6, 1, 0, 2, 4);

    data.WriteGuidBytes(npcGuid, 6, 0, 1);
    data << uint32(GUILD_CHARTER_COST); // Petition Cost
    data.WriteGuidBytes(npcGuid, 4, 3, 5, 2, 7);

    SendPacket(&data);
    TC_LOG_DEBUG("network", "Sent SMSG_PETITION_SHOWLIST");
}

void WorldSession::SendPetitionSignResults(ObjectGuid petitionGuid, ObjectGuid playerGuid, uint8 result)
{
    WorldPacket data(SMSG_PETITION_SIGN_RESULTS, 9 + 9 +1);
    data.WriteGuidMask(playerGuid, 2, 0);
    data.WriteBit(petitionGuid[0]);
    data.WriteBit(playerGuid[1]);
    data.WriteGuidMask(petitionGuid, 5, 2, 4);
    data.WriteBit(playerGuid[6]);
    data.WriteGuidMask(petitionGuid, 1, 6);
    data.WriteBit(playerGuid[4]);
    data.WriteBit(petitionGuid[3]);
    data.WriteGuidMask(playerGuid, 5, 3);
    data.WriteBit(petitionGuid[7]);
    data.WriteBit(playerGuid[7]);
    data.WriteBits(result, 4);
    data.FlushBits();

    data.WriteGuidBytes(petitionGuid, 0, 5);
    data.WriteGuidBytes(playerGuid, 3, 0);
    data.WriteByteSeq(petitionGuid[3]);
    data.WriteGuidBytes(playerGuid, 2, 6, 4, 1);
    data.WriteGuidBytes(petitionGuid, 6, 7);
    data.WriteByteSeq(playerGuid[7]);
    data.WriteGuidBytes(petitionGuid, 2, 1);
    data.WriteByteSeq(playerGuid[5]);
    data.WriteByteSeq(petitionGuid[4]);
    SendPacket(&data);
}