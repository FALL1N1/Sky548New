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
#include "DBCStores.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "SpellMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Spell.h"
#include "Totem.h"
#include "TemporarySummon.h"
#include "SpellAuras.h"
#include "CreatureAI.h"
#include "ScriptMgr.h"
#include "GameObjectAI.h"
#include "SpellAuraEffects.h"
#include "Player.h"
#include "DBCStores.h"

void WorldSession::HandleClientCastFlags(WorldPacket& recvPacket, uint8 castFlags, SpellCastTargets& targets)
{
    // some spell cast packet including more data (for projectiles?)
    if (castFlags & 0x02)
    {
        // not sure about these two
        float elevation, speed;
        recvPacket >> elevation;
        recvPacket >> speed;

        targets.SetElevation(elevation);
        targets.SetSpeed(speed);

        uint8 hasMovementData;
        recvPacket >> hasMovementData;
        if (hasMovementData)
            HandleMovementOpcodes(recvPacket);
    }
}

void WorldSession::HandleUseItemOpcode(WorldPacket& recvPacket)
{
    /// @todo add targets.read() check
    Player* pUser = _player;
    Unit* mover = _player->m_mover;

    // ignore for remote control state
    if (mover != pUser)
        return;

    uint8 bagIndex, slot;
    uint8 castCount = 0;
    uint8 castFlags = 0;
    uint32 spellId = 0;
    uint32 glyphIndex = 0;
    uint32 targetMask = 0;
    uint32 targetStringLength = 0;
    float elevation = 0.0f;
    float missileSpeed = 0.0f;
    ObjectGuid itemGuid = 0;
    ObjectGuid targetGuid = 0;
    ObjectGuid itemTargetGuid = 0;
    ObjectGuid destTransportGuid = 0;
    ObjectGuid srcTransportGuid = 0;
    Position srcPos;
    Position destPos;
    std::string targetString;

    // Movement data
    MovementInfo movementInfo;
    ObjectGuid movementTransportGuid = 0;
    ObjectGuid movementGuid = 0;
    bool hasTransport = false;
    bool hasTransportTime2 = false;
    bool hasTransportVehicleId = false;
    bool hasFallData = false;
    bool hasFallDirection = false;
    bool hasTimestamp = false;
    bool hasSplineElevation = false;
    bool hasPitch = false;
    bool hasOrientation = false;
    bool hasUnkMovementField = false;
    uint32 unkMovementLoopCounter = 0;
    Unit* caster = mover;

    recvPacket >> slot >> bagIndex;

    bool hasElevation = !recvPacket.ReadBit();
    itemGuid[6] = recvPacket.ReadBit();
    bool hasTargetString = !recvPacket.ReadBit();
    itemGuid[1] = recvPacket.ReadBit();
    bool hasCastFlags = !recvPacket.ReadBit();
    bool hasDestLocation = recvPacket.ReadBit();
    recvPacket.ReadGuidMask(itemGuid, 2, 7, 0);
    bool hasTargetMask = !recvPacket.ReadBit();
    bool hasMissileSpeed = !recvPacket.ReadBit();
    bool hasMovement = recvPacket.ReadBit();
    bool hasCastCount = !recvPacket.ReadBit();
    bool hasSpellId = !recvPacket.ReadBit();
    recvPacket.ReadBit();
    bool hasGlyphIndex = !recvPacket.ReadBit();
    recvPacket.ReadBit();
    itemGuid[4] = recvPacket.ReadBit();
    bool hasSrcLocation = recvPacket.ReadBit();
    recvPacket.ReadGuidMask(itemGuid, 3, 5);
    uint8 researchDataCount = recvPacket.ReadBits(2);

    for (uint8 i = 0; i < researchDataCount; ++i)
        recvPacket.ReadBits(2);

    if (hasMovement)
    {
        hasPitch = !recvPacket.ReadBit();
        hasTransport = recvPacket.ReadBit();
        recvPacket.ReadBit();

        if (hasTransport)
        {
            recvPacket.ReadGuidMask(movementTransportGuid, 7, 2, 4, 5, 6, 0, 1);
            hasTransportVehicleId = recvPacket.ReadBit();
            movementTransportGuid[4] = recvPacket.ReadBit();
            hasTransportTime2 = recvPacket.ReadBit();
        }

        recvPacket.ReadGuidMask(movementGuid, 6, 2, 1);
        unkMovementLoopCounter = recvPacket.ReadBits(22);
        recvPacket.ReadBit();
        bool hasMovementFlags2 = !recvPacket.ReadBit();
        hasFallData = recvPacket.ReadBit();
        movementGuid[5] = recvPacket.ReadBit();
        hasSplineElevation = !recvPacket.ReadBit();
        recvPacket.ReadBit();
        recvPacket.ReadGuidMask(movementGuid, 7, 0);

        if (hasFallData)
            hasFallDirection = recvPacket.ReadBit();

        hasOrientation = !recvPacket.ReadBit();
        recvPacket.ReadGuidMask(movementGuid, 4, 3);
        hasTimestamp = !recvPacket.ReadBit();
        hasUnkMovementField = !recvPacket.ReadBit();
        bool hasMovementFlags = !recvPacket.ReadBit();

        if (hasMovementFlags2)
            movementInfo.flags2 = recvPacket.ReadBits(13);

        if (hasMovement)
            movementInfo.flags = recvPacket.ReadBits(30);
    }

    if (hasSrcLocation)
        recvPacket.ReadGuidMask(srcTransportGuid, 3, 1, 7, 4, 2, 0, 6, 5);

    if (hasDestLocation)
        recvPacket.ReadGuidMask(destTransportGuid, 2, 4, 1, 7, 6, 0, 3, 5);

    if (hasTargetString)
        targetStringLength = recvPacket.ReadBits(7);

    recvPacket.ReadGuidMask(itemTargetGuid, 1, 0, 5, 3, 6, 4, 7, 2);

    recvPacket.ReadGuidMask(targetGuid, 4, 5, 0, 1, 3, 7, 6, 2);

    if (hasCastFlags)
        castFlags = recvPacket.ReadBits(5);

    if (hasTargetMask)
        targetMask = recvPacket.ReadBits(20);

    recvPacket.ReadGuidBytes(itemGuid, 0, 5, 6, 3, 4, 2, 1);

    for (uint8 i = 0; i < researchDataCount; ++i)
    {
        recvPacket.read_skip<uint32>();
        recvPacket.read_skip<uint32>();
    }

    recvPacket.ReadByteSeq(itemGuid[7]);

    if (hasMovement)
    {
        for (uint8 i = 0; i != unkMovementLoopCounter; i++)
            recvPacket.read_skip<uint32>();

        if (hasTransport)
        {
            recvPacket >> movementInfo.transport.pos.m_positionY;
            recvPacket >> movementInfo.transport.pos.m_positionZ;
            recvPacket.ReadByteSeq(movementTransportGuid[1]);

            if (hasTransportVehicleId)
                recvPacket >> movementInfo.transport.vehicleId;

            recvPacket.ReadGuidBytes(movementTransportGuid, 7, 5, 2, 4);
            recvPacket >> movementInfo.transport.pos.m_positionX;
            movementInfo.transport.pos.SetOrientation(recvPacket.read<float>());
            recvPacket.ReadByteSeq(movementTransportGuid[0]);
            recvPacket >> movementInfo.transport.seat;
            recvPacket >> movementInfo.transport.time;
            recvPacket.ReadGuidBytes(movementTransportGuid, 6, 3);

            if (hasTransportTime2)
                recvPacket >> movementInfo.transport.time2;
        }

        if (hasFallData)
        {
            recvPacket >> movementInfo.jump.zspeed;

            if (hasFallDirection)
            {
                recvPacket >> movementInfo.jump.sinAngle;
                recvPacket >> movementInfo.jump.cosAngle;
                recvPacket >> movementInfo.jump.xyspeed;
            }

            recvPacket >> movementInfo.jump.fallTime;
        }

        recvPacket.ReadGuidBytes(movementTransportGuid, 3, 7, 6, 1);
        recvPacket >> movementInfo.pos.m_positionY;

        if (hasSplineElevation)
            recvPacket >> movementInfo.splineElevation;

        if (hasUnkMovementField)
            recvPacket.read_skip<uint32>();

        if (hasOrientation)
            movementInfo.pos.SetOrientation(recvPacket.read<float>());

        recvPacket.ReadByteSeq(movementTransportGuid[2]);
        recvPacket >> movementInfo.pos.m_positionZ;

        if (hasTimestamp)
            recvPacket >> movementInfo.time;

        recvPacket >> movementInfo.pos.m_positionX;
        recvPacket.ReadGuidBytes(movementTransportGuid, 5, 0);

        if (hasPitch)
            movementInfo.pitch = G3D::wrap(recvPacket.read<float>(), float(-M_PI), float(M_PI));

        recvPacket.ReadByteSeq(movementTransportGuid[4]);
    }

    if (hasDestLocation)
    {
        float x, y, z;

        recvPacket.ReadByteSeq(destTransportGuid[7]);
        recvPacket >> x;
        recvPacket.ReadGuidBytes(destTransportGuid, 0, 6, 1, 3);
        recvPacket >> y;
        recvPacket.ReadByteSeq(destTransportGuid[5]);
        recvPacket >> z;
        recvPacket.ReadGuidBytes(destTransportGuid, 4, 2);

        destPos.Relocate(x, y, z);
    }
    else
    {
        destTransportGuid = caster->GetTransGUID();

        if (destTransportGuid)
            destPos.Relocate(caster->GetTransOffsetX(), caster->GetTransOffsetY(), caster->GetTransOffsetZ(), caster->GetTransOffsetO());
        else
            destPos.Relocate(caster);
    }

    recvPacket.ReadGuidBytes(targetGuid, 6, 7, 2, 0, 3, 4, 1, 5);

    if (hasSrcLocation)
    {
        float x, y, z;

        recvPacket.ReadByteSeq(srcTransportGuid[7]);
        recvPacket >> x;
        recvPacket.ReadGuidBytes(srcTransportGuid, 1, 5, 4);
        recvPacket >> z;
        recvPacket.ReadGuidBytes(srcTransportGuid, 6, 0, 3);
        recvPacket >> y;
        recvPacket.ReadByteSeq(srcTransportGuid[2]);

        srcPos.Relocate(x, y, z);
    }
    else
    {
        srcTransportGuid = caster->GetTransGUID();
        if (srcTransportGuid)
            srcPos.Relocate(caster->GetTransOffsetX(), caster->GetTransOffsetY(), caster->GetTransOffsetZ(), caster->GetTransOffsetO());
        else
            srcPos.Relocate(caster);
    }

    if (hasSpellId)
        recvPacket >> spellId;

    recvPacket.ReadGuidBytes(itemTargetGuid, 1, 4, 3, 6, 2, 0, 7, 5);

    if (hasTargetString)
        targetString = recvPacket.ReadString(targetStringLength);

    if (hasElevation)
        recvPacket >> elevation;

    if (hasGlyphIndex)
        recvPacket >> glyphIndex;

    if (hasMissileSpeed)
        recvPacket >> missileSpeed;

    if (hasCastCount)
        recvPacket >> castCount;

    if (glyphIndex >= MAX_GLYPH_SLOT_INDEX)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    Item* pItem = pUser->GetUseableItemByPos(bagIndex, slot);
    if (!pItem)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    if (pItem->GetGUID() != itemGuid)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    TC_LOG_DEBUG("network", "WORLD: CMSG_USE_ITEM packet, bagIndex: %u, slot: %u, castCount: %u, spellId: %u, Item: %u, glyphIndex: %u, data length = %i", bagIndex, slot, castCount, spellId, pItem->GetEntry(), glyphIndex, (uint32)recvPacket.size());

    ItemTemplate const* proto = pItem->GetTemplate();
    if (!proto)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, pItem, NULL);
        return;
    }

    // some item classes can be used only in equipped state
    if (proto->InventoryType != INVTYPE_NON_EQUIP && !pItem->IsEquipped())
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, pItem, NULL);
        return;
    }

    InventoryResult msg = pUser->CanUseItem(pItem);
    if (msg != EQUIP_ERR_OK)
    {
        pUser->SendEquipError(msg, pItem, NULL);
        return;
    }

    // only allow conjured consumable, bandage, poisons (all should have the 2^21 item flag set in DB)
    if (proto->Class == ITEM_CLASS_CONSUMABLE && !(proto->Flags & ITEM_PROTO_FLAG_USEABLE_IN_ARENA) && pUser->InArena())
    {
        pUser->SendEquipError(EQUIP_ERR_NOT_DURING_ARENA_MATCH, pItem, NULL);
        return;
    }

    // don't allow items banned in arena
    if (proto->Flags & ITEM_PROTO_FLAG_NOT_USEABLE_IN_ARENA && pUser->InArena())
    {
        pUser->SendEquipError(EQUIP_ERR_NOT_DURING_ARENA_MATCH, pItem, NULL);
        return;
    }

    if (pUser->IsInCombat())
    {
        for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(proto->Spells[i].SpellId))
            {
                if (!spellInfo->CanBeUsedInCombat())
                {
                    pUser->SendEquipError(EQUIP_ERR_NOT_IN_COMBAT, pItem, NULL);
                    return;
                }
            }
        }
    }

    // check also  BIND_WHEN_PICKED_UP and BIND_QUEST_ITEM for .additem or .additemset case by GM (not binded at adding to inventory)
    if (pItem->GetTemplate()->Bonding == BIND_WHEN_USE || pItem->GetTemplate()->Bonding == BIND_WHEN_PICKED_UP || pItem->GetTemplate()->Bonding == BIND_QUEST_ITEM)
    {
        if (!pItem->IsSoulBound())
        {
            pItem->SetState(ITEM_CHANGED, pUser);
            pItem->SetBinding(true);
        }
    }

    SpellCastTargets targets(caster, targetMask, targetGuid, itemTargetGuid, srcTransportGuid, destTransportGuid, srcPos, destPos, elevation, missileSpeed, targetString);

    // Note: If script stop casting it must send appropriate data to client to prevent stuck item in gray state.
    if (!sScriptMgr->OnItemUse(pUser, pItem, targets))
    {
        // TODO: this method does not work well, this provokes unfunctional item's spells
        pUser->CastItemUseSpell(pItem, targets, castCount, glyphIndex);
    }
	
}

void WorldSession::HandleOpenItemOpcode(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_OPEN_ITEM packet, data length = %i", (uint32)recvPacket.size());

    Player* pUser = _player;

    // ignore for remote control state
    if (pUser->m_mover != pUser)
        return;

    uint8 bagIndex, slot;

    recvPacket >> bagIndex >> slot;

    TC_LOG_DEBUG("network", "bagIndex: %u, slot: %u", bagIndex, slot);

    Item* item = pUser->GetItemByPos(bagIndex, slot);
    if (!item)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
        return;
    }

    ItemTemplate const* proto = item->GetTemplate();
    if (!proto)
    {
        pUser->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, item, NULL);
        return;
    }

    // Verify that the bag is an actual bag or wrapped item that can be used "normally"
    if (!(proto->Flags & ITEM_PROTO_FLAG_OPENABLE) && !item->HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_WRAPPED))
    {
        pUser->SendEquipError(EQUIP_ERR_CLIENT_LOCKED_OUT, item, NULL);
        TC_LOG_ERROR("network", "Possible hacking attempt: Player %s [guid: %u] tried to open item [guid: %u, entry: %u] which is not openable!",
                pUser->GetName().c_str(), pUser->GetGUIDLow(), item->GetGUIDLow(), proto->ItemId);
        return;
    }

    // locked item
    uint32 lockId = proto->LockID;
    if (lockId)
    {
        LockEntry const* lockInfo = sLockStore.LookupEntry(lockId);

        if (!lockInfo)
        {
            pUser->SendEquipError(EQUIP_ERR_ITEM_LOCKED, item, NULL);
            TC_LOG_ERROR("network", "WORLD::OpenItem: item [guid = %u] has an unknown lockId: %u!", item->GetGUIDLow(), lockId);
            return;
        }

        // was not unlocked yet
        if (item->IsLocked())
        {
            pUser->SendEquipError(EQUIP_ERR_ITEM_LOCKED, item, NULL);
            return;
        }
    }

    if (item->HasFlag(ITEM_FIELD_DYNAMIC_FLAGS, ITEM_FLAG_WRAPPED))// wrapped?
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_GIFT_BY_ITEM);

        stmt->setUInt32(0, item->GetGUIDLow());

        PreparedQueryResult result = CharacterDatabase.Query(stmt);

        if (result)
        {
            Field* fields = result->Fetch();
            uint32 entry = fields[0].GetUInt32();
            uint32 flags = fields[1].GetUInt32();

            item->SetUInt64Value(ITEM_FIELD_GIFT_CREATOR, 0);
            item->SetEntry(entry);
            item->SetUInt32Value(ITEM_FIELD_DYNAMIC_FLAGS, flags);
            item->SetState(ITEM_CHANGED, pUser);
        }
        else
        {
            TC_LOG_ERROR("network", "Wrapped item %u don't have record in character_gifts table and will deleted", item->GetGUIDLow());
            pUser->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
            return;
        }

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GIFT);

        stmt->setUInt32(0, item->GetGUIDLow());

        CharacterDatabase.Execute(stmt);
    }
    else
        pUser->SendLoot(item->GetGUID(), LOOT_CORPSE);
}

void WorldSession::HandleGameObjectUseOpcode(WorldPacket& recvData)
{
    ObjectGuid guid;

    recvData.ReadGuidMask(guid, 6, 1, 3, 4, 0, 5, 7, 2);

    recvData.ReadGuidBytes(guid, 0, 1, 6, 2, 3, 4, 5, 7);

    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_GAMEOBJ_USE Message [guid=%u]", GUID_LOPART(guid));

    if (GameObject* obj = GetPlayer()->GetMap()->GetGameObject(guid))
    {
        // ignore for remote control state
        if (_player->m_mover != _player)
            if (!(_player->IsOnVehicle(_player->m_mover) || _player->IsMounted()) && !obj->GetGOInfo()->IsUsableMounted())
                return;

        obj->Use(_player);
    }
}

void WorldSession::HandleGameobjectReportUse(WorldPacket& recvPacket)
{
    ObjectGuid guid;

    recvPacket.ReadGuidMask(guid, 4, 7, 5, 3, 6, 1, 2, 0);

    recvPacket.ReadGuidBytes(guid, 7, 1, 6, 5, 0, 3, 2, 4);

    TC_LOG_DEBUG("network", "WORLD: Recvd CMSG_GAMEOBJ_REPORT_USE Message [in game guid: %u]", GUID_LOPART(guid));

    // ignore for remote control state
    if (_player->m_mover != _player)
        return;

    GameObject* go = GetPlayer()->GetMap()->GetGameObject(guid);
    if (!go)
        return;

    if (!go->IsWithinDistInMap(_player, INTERACTION_DISTANCE))
        return;

    if (go->AI()->GossipHello(_player))
        return;

    _player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_GAMEOBJECT, go->GetEntry());
}

void WorldSession::HandleCastSpellOpcode(WorldPacket& recvPacket)
{
    // ignore for remote control state (for player case)
    Unit* mover = _player->m_mover;
    if (mover != _player && mover->GetTypeId() == TYPEID_PLAYER)
    {
        recvPacket.rfinish(); // prevent spam at ignore packet
        return;
    }

    uint8 castCount = 0;
    uint8 castFlags = 0;
    uint32 spellId = 0;
    uint32 glyphIndex = 0;
    uint32 targetMask = 0;
    uint32 targetStringLength = 0;
    float elevation = 0.0f;
    float missileSpeed = 0.0f;
    ObjectGuid targetGuid = 0;
    ObjectGuid itemTargetGuid = 0;
    ObjectGuid destTransportGuid = 0;
    ObjectGuid srcTransportGuid = 0;
    Position srcPos;
    Position destPos;
    std::string targetString;

    // Movement data
    MovementInfo movementInfo;
    ObjectGuid movementTransportGuid = 0;
    ObjectGuid movementGuid = 0;
    bool hasTransport = false;
    bool hasTransportTime2 = false;
    bool hasTransportVehicleId = false;
    bool hasFallData = false;
    bool hasFallDirection = false;
    bool hasTimestamp = false;
    bool hasSplineElevation = false;
    bool hasPitch = false;
    bool hasOrientation = false;
    bool hasUnkMovementField = false;
    uint32 unkMovementLoopCounter = 0;
    Unit* caster = mover;

    recvPacket.ReadBit(); // Fake bit
    bool hasTargetString = !recvPacket.ReadBit();
    recvPacket.ReadBit(); // Fake Bit
    bool hasCastCount = !recvPacket.ReadBit();
    bool hasSrcLocation = recvPacket.ReadBit();
    bool hasDestLocation = recvPacket.ReadBit();
    bool hasSpellId = !recvPacket.ReadBit();
    uint8 researchDataCount = recvPacket.ReadBits(2);
    bool hasTargetMask = !recvPacket.ReadBit();
    bool hasMissileSpeed = !recvPacket.ReadBit();

    for (uint8 i = 0; i < researchDataCount; ++i)
        recvPacket.ReadBits(2);

    bool hasGlyphIndex = !recvPacket.ReadBit();
    bool hasMovement = recvPacket.ReadBit();
    bool hasElevation = !recvPacket.ReadBit();
    bool hasCastFlags = !recvPacket.ReadBit();

    recvPacket.ReadGuidMask(targetGuid, 5, 4, 2, 7, 1, 6, 3, 0);

    if (hasDestLocation)
    {
        recvPacket.ReadGuidMask(destTransportGuid, 1, 3, 5, 0, 2, 6, 7, 4);
    }


    if (hasMovement)
    {
        unkMovementLoopCounter = recvPacket.ReadBits(22);
        recvPacket.ReadBit();
        movementGuid[4] = recvPacket.ReadBit();
        hasTransport = recvPacket.ReadBit();

        if (hasTransport)
        {
            hasTransportTime2 = recvPacket.ReadBit();
            recvPacket.ReadGuidMask(movementTransportGuid, 7, 4, 1, 0, 6, 3, 5);
            hasTransportVehicleId = recvPacket.ReadBit();
            movementTransportGuid[2] = recvPacket.ReadBit();
        }

        recvPacket.ReadBit();
        movementGuid[7] = recvPacket.ReadBit();
        hasOrientation = !recvPacket.ReadBit();
        movementGuid[6] = recvPacket.ReadBit();
        hasSplineElevation = !recvPacket.ReadBit();
        hasPitch = !recvPacket.ReadBit();
        movementGuid[0] = recvPacket.ReadBit();
        recvPacket.ReadBit();
        bool hasMovementFlags = !recvPacket.ReadBit();
        hasTimestamp = !recvPacket.ReadBit();
        hasUnkMovementField = !recvPacket.ReadBit();

        if (hasMovementFlags)
            movementInfo.flags = recvPacket.ReadBits(30);

        recvPacket.ReadGuidMask(movementGuid, 1, 3, 2, 5);
        hasFallData = recvPacket.ReadBit();

        if (hasFallData)
            hasFallDirection = recvPacket.ReadBit();

        bool hasMovementFlags2 = !recvPacket.ReadBit();

        if (hasMovementFlags2)
            movementInfo.flags2 = recvPacket.ReadBits(13);
    }

    recvPacket.ReadGuidMask(itemTargetGuid, 1, 0, 7, 4, 6, 5, 3, 2);

    if (hasSrcLocation)
    {
        recvPacket.ReadGuidMask(srcTransportGuid, 4, 5, 3, 0, 7, 1, 6, 2);
    }

    if (hasTargetMask)
        targetMask = recvPacket.ReadBits(20);

    if (hasCastFlags)
        castFlags = recvPacket.ReadBits(5);

    if (hasTargetString)
        targetStringLength = recvPacket.ReadBits(7);

    for (uint8 i = 0; i < researchDataCount; ++i)
    {
        recvPacket.read_skip<uint32>();
        recvPacket.read_skip<uint32>();
    }

    if (hasMovement)
    {
        recvPacket >> movementInfo.pos.m_positionX;
        recvPacket.ReadByteSeq(movementGuid[0]);

        if (hasTransport)
        {
            recvPacket.ReadByteSeq(movementTransportGuid[2]);
            recvPacket >> movementInfo.transport.seat;
            recvPacket.ReadGuidBytes(movementTransportGuid, 3, 7);
            recvPacket >> movementInfo.transport.pos.m_positionX;
            recvPacket.ReadByteSeq(movementTransportGuid[5]);

            if (hasTransportVehicleId)
                recvPacket >> movementInfo.transport.vehicleId;

            recvPacket >> movementInfo.transport.pos.m_positionZ;
            recvPacket >> movementInfo.transport.pos.m_positionY;

            recvPacket.ReadGuidBytes(movementTransportGuid, 6, 1);
            movementInfo.transport.pos.SetOrientation(recvPacket.read<float>());

            recvPacket.ReadByteSeq(movementTransportGuid[4]);

            if (hasTransportTime2)
                recvPacket >> movementInfo.transport.time2;

            recvPacket.ReadByteSeq(movementTransportGuid[0]);
            recvPacket >> movementInfo.transport.time;

        }

        recvPacket.ReadByteSeq(movementGuid[5]);

        if (hasFallData)
        {
            recvPacket >> movementInfo.jump.fallTime;
            recvPacket >> movementInfo.jump.zspeed;

            if (hasFallDirection)
            {
                recvPacket >> movementInfo.jump.sinAngle;
                recvPacket >> movementInfo.jump.xyspeed;
                recvPacket >> movementInfo.jump.cosAngle;
            }
        }

        if (hasSplineElevation)
            recvPacket >> movementInfo.splineElevation;

        recvPacket.ReadByteSeq(movementGuid[6]);

        if (hasUnkMovementField)
            recvPacket.read_skip<uint32>();

        recvPacket.ReadByteSeq(movementGuid[4]);

        if (hasOrientation)
            movementInfo.pos.SetOrientation(recvPacket.read<float>());

        if (hasTimestamp)
            recvPacket >> movementInfo.time;

        recvPacket.ReadByteSeq(movementGuid[1]);

        if (hasPitch)
            movementInfo.pitch = G3D::wrap(recvPacket.read<float>(), float(-M_PI), float(M_PI));

        recvPacket.ReadByteSeq(movementGuid[3]);

        for (uint8 i = 0; i != unkMovementLoopCounter; i++)
            recvPacket.read_skip<uint32>();

        recvPacket >> movementInfo.pos.m_positionY;
        recvPacket.ReadByteSeq(movementGuid[7]);
        recvPacket >> movementInfo.pos.m_positionZ;
        recvPacket.ReadByteSeq(movementGuid[2]);
    }

    recvPacket.ReadGuidBytes(itemTargetGuid, 4, 2, 1, 5, 7, 3, 6, 0);

    if (hasDestLocation)
    {
        float x, y, z;
        recvPacket.ReadByteSeq(destTransportGuid[2]);
        recvPacket >> x;
        recvPacket.ReadGuidBytes(destTransportGuid, 4, 1, 0, 3);
        recvPacket >> y;
        recvPacket.ReadByteSeq(destTransportGuid[7]);
        recvPacket >> z;
        recvPacket.ReadGuidBytes(destTransportGuid, 5, 6);
        destPos.Relocate(x, y, z);
    }
    else
    {
        destTransportGuid = caster->GetTransGUID();
        if (destTransportGuid)
            destPos.Relocate(caster->GetTransOffsetX(), caster->GetTransOffsetY(), caster->GetTransOffsetZ(), caster->GetTransOffsetO());
        else
            destPos.Relocate(caster);
    }

    recvPacket.ReadGuidBytes(targetGuid, 3, 4, 7, 6, 2, 0, 1, 5);

    if (hasSrcLocation)
    {
        float x, y, z;
        recvPacket >> y;
        recvPacket.ReadGuidBytes(srcTransportGuid, 5, 1, 7, 6);
        recvPacket >> x;
        recvPacket.ReadGuidBytes(srcTransportGuid, 3, 2, 0, 4);
        recvPacket >> z;
        srcPos.Relocate(x, y, z);
    }
    else
    {
        srcTransportGuid = caster->GetTransGUID();
        if (srcTransportGuid)
            srcPos.Relocate(caster->GetTransOffsetX(), caster->GetTransOffsetY(), caster->GetTransOffsetZ(), caster->GetTransOffsetO());
        else
            srcPos.Relocate(caster);
    }

    if (hasTargetString)
        targetString = recvPacket.ReadString(targetStringLength);

    if (hasMissileSpeed)
        recvPacket >> missileSpeed;

    if (hasElevation)
        recvPacket >> elevation;

    if (hasCastCount)
        recvPacket >> castCount;

    if (hasSpellId)
        recvPacket >> spellId;

    if (hasGlyphIndex)
        recvPacket >> glyphIndex;

    TC_LOG_DEBUG("network", "WORLD: got cast spell packet, castCount: %u, spellId: %u, castFlags: %u, data length = %u", castCount, spellId, castFlags, (uint32)recvPacket.size());

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("network", "WORLD: unknown spell id %u", spellId);
        recvPacket.rfinish(); // prevent spam at ignore packet
        return;
    }

    if (spellInfo->IsPassive())
    {
        recvPacket.rfinish(); // prevent spam at ignore packet
        return;
    }

    if (caster->GetTypeId() == TYPEID_UNIT && !caster->ToCreature()->HasSpell(spellId))
    {
        // If the vehicle creature does not have the spell but it allows the passenger to cast own spells
        // change caster to player and let him cast
        if (!_player->IsOnVehicle(caster) || spellInfo->CheckVehicle(_player) != SPELL_CAST_OK)
        {
            recvPacket.rfinish(); // prevent spam at ignore packet
            return;
        }

        caster = _player;
    }

    auto IsExcludedSpell = [spellInfo, castFlags] () -> bool
    {
        if (spellInfo->AttributesEx8 & SPELL_ATTR8_RAID_MARKER)
            return true;

        if (castFlags & 0x8)
            return true;

        switch (spellInfo->Id)
        {
            case 67869: // Knocking
                return true;
            default:
                return false;
        }
    };
        
    if (caster->GetTypeId() == TYPEID_PLAYER && !caster->ToPlayer()->HasActiveSpell(spellId) && !IsExcludedSpell())
    {       
        // not have spell in spellbook
        recvPacket.rfinish(); // prevent spam at ignore packet
        return;
    }

    Unit::AuraEffectList swaps = mover->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
    Unit::AuraEffectList const& swaps2 = mover->GetAuraEffectsByType(SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2);
    if (!swaps2.empty())
        swaps.insert(swaps.end(), swaps2.begin(), swaps2.end());

    if (!swaps.empty())
    {
        for (Unit::AuraEffectList::const_iterator itr = swaps.begin(); itr != swaps.end(); ++itr)
        {
            if ((*itr)->IsAffectingSpell(spellInfo))
            {
                if (SpellInfo const* newInfo = sSpellMgr->GetSpellInfo((*itr)->GetAmount()))
                {
                    spellInfo = newInfo;
                    spellId = newInfo->Id;
                }
                break;
            }
        }
    }

    // Client is resending autoshot cast opcode when other spell is casted during shoot rotation
    // Skip it to prevent "interrupt" message
    if (spellInfo->IsAutoRepeatRangedSpell() && caster->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL)
        && caster->GetCurrentSpell(CURRENT_AUTOREPEAT_SPELL)->m_spellInfo == spellInfo)
    {
        recvPacket.rfinish();
        return;
    }

    // can't use our own spells when we're in possession of another unit,
    if (_player->isPossessing())
    {
        recvPacket.rfinish(); // prevent spam at ignore packet
        return;
    }

    // client provided targets
    SpellCastTargets targets(caster, targetMask, targetGuid, itemTargetGuid, srcTransportGuid, destTransportGuid, srcPos, destPos, elevation, missileSpeed, targetString);

    // auto-selection buff level base at target level (in spellInfo)
    if (targets.GetUnitTarget())
    {
        SpellInfo const* actualSpellInfo = spellInfo->GetAuraRankForLevel(targets.GetUnitTarget()->getLevel());

        // if rank not found then function return NULL but in explicit cast case original spell can be casted and later failed with appropriate error message
        if (actualSpellInfo)
            spellInfo = actualSpellInfo;
    }

    // Override spell Id, client send base spell and not the overrided id
    if (!spellInfo->OverrideSpellList.empty())
    {
        for (auto itr : spellInfo->OverrideSpellList)
        {
            if (_player->HasSpell(itr))
            {
                SpellInfo const* overrideSpellInfo = sSpellMgr->GetSpellInfo(itr);
                if (overrideSpellInfo)
                {
                    spellInfo = overrideSpellInfo;
                    spellId = itr;
                }
                break;
            }
        }
    }
	
	// Custom MoP Script
	// Power Word : Solace - 129250 and Power Word : Insanity - 129249
	if (spellInfo->Id == 129250 && _player->GetShapeshiftForm() == FORM_SHADOW)
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(129249);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Alter Time - 108978 and Alter Time (overrided) - 127140
	else if (spellInfo->Id == 108978 && _player->HasAura(110909))
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(127140);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Fix Dark Soul for Destruction warlocks
	else if (spellInfo->Id == 113860 && _player->GetSpecializationId(_player->GetActiveSpec()) == CHAR_SPECIALIZATION_WARLOCK_DESTRUCTION)
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(113858);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Halo - 120517 and Halo - 120644 (shadow form)
	else if (spellInfo->Id == 120517 && _player->HasAura(15473))
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(120644);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Consecration - 116467 and Consecration - 26573
	else if (spellInfo->Id == 116467)
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(26573);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Cascade (shadow) - 127632 and Cascade - 121135
	else if (spellInfo->Id == 121135 && _player->HasAura(15473))
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(127632);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Zen Pilgrimage - 126892 and Zen Pilgrimage : Return - 126895
	else if (spellInfo->Id == 126892 && _player->HasAura(126896))
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(126895);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Soul Swap - 86121 and Soul Swap : Exhale - 86213
	else if (spellInfo->Id == 86121 && _player->HasAura(86211))
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(86213);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
			_player->RemoveAura(86211);
		}
	}
	// Mage Bomb - 125430 and  Living Bomb - 44457
	else if (spellInfo->Id == 125430 && _player->HasSpell(44457))
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(44457);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Mage Bomb - 125430 and Frost Bomb - 112948
	else if (spellInfo->Id == 125430 && _player->HasSpell(112948))
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(112948);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Mage Bomb - 125430 and  Nether Tempest - 114923
	else if (spellInfo->Id == 125430 && _player->HasSpell(114923))
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(114923);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Evocation - 12051 and  Rune of Power - 116011
	else if (spellInfo->Id == 12051 && _player->HasSpell(116011))
	{
		SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(116011);
		if (newSpellInfo)
		{
			spellInfo = newSpellInfo;
			spellId = newSpellInfo->Id;
		}
	}
	// Frostbolt - 116 and Frostbolt - 126201 (heal for water elemental)
	else if (spellInfo->Id == 116 && targets.GetUnitTarget())
	{
		if (targets.GetUnitTarget()->GetOwner() && targets.GetUnitTarget()->GetOwner()->GetTypeId() == TYPEID_PLAYER && targets.GetUnitTarget()->GetOwner()->GetGUID() == _player->GetGUID())
		{
			SpellInfo const* newSpellInfo = sSpellMgr->GetSpellInfo(126201);
			if (newSpellInfo)
			{
				spellInfo = newSpellInfo;
				spellId = newSpellInfo->Id;
			}
		}
	}
	// Surging Mist - 116694 and Surging Mist - 116995
	// Surging Mist is instantly casted if player is channeling Soothing Mist
	else if (spellInfo->Id == 116694 && _player->GetCurrentSpell(CURRENT_CHANNELED_SPELL) && _player->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->GetSpellInfo()->Id == 115175)
	{
		recvPacket.rfinish();
		_player->CastSpell(targets.GetUnitTarget(), 116995, true);
		_player->EnergizeBySpell(_player, 116995, 1, POWER_CHI);
		int32 powerCost = spellInfo->CalcPowerCost(_player, spellInfo->GetSchoolMask());
		_player->ModifyPower(POWER_MANA, -powerCost);
		return;
	}
	// Enveloping Mist - 124682 and Enveloping Mist - 132120
	// Enveloping Mist is instantly casted if player is channeling Soothing Mist
	else if (spellInfo->Id == 124682 && _player->GetCurrentSpell(CURRENT_CHANNELED_SPELL) && _player->GetCurrentSpell(CURRENT_CHANNELED_SPELL)->GetSpellInfo()->Id == 115175)
	{
		recvPacket.rfinish();
		_player->CastSpell(targets.GetUnitTarget(), 132120, true);
		int32 powerCost = spellInfo->CalcPowerCost(_player, spellInfo->GetSchoolMask());
		_player->ModifyPower(POWER_CHI, -powerCost);
		return;
	}

    Spell* spell = new Spell(caster, spellInfo, TRIGGERED_NONE, 0, false);
    spell->m_cast_count = castCount;   // set count of casts
    spell->m_glyphIndex = glyphIndex;

    if (targetMask & TARGET_FLAG_GAMEOBJECT)
        if (WorldObject* const obj = ObjectAccessor::GetWorldObject(*caster, targetGuid))
            targets.SetGOTarget(obj->ToGameObject());

    if (castFlags & 0x8)   // Archaeology
    {
        SpellResearchData* researchData = new SpellResearchData();
        memset(researchData, 0, sizeof(SpellResearchData*));
        uint32 count;
        uint8 type;
        recvPacket >> count;
        for (uint32 i = 0; i < count; ++i)
        {
            recvPacket >> type;
            switch (type)
            {
            case 2: // Keystones
                recvPacket >> researchData->keystoneItemId;       // Item id
                recvPacket >> researchData->keystoneCount;        // Item count
                break;
            case 1: // Fragments
                recvPacket >> researchData->fragmentCurrencyId;   // Currency id
                recvPacket >> researchData->fragmentCount;        // Currency count
                break;
            }
        }

        recvPacket.ReadGuidBytes(itemTargetGuid, 4, 2, 1, 5, 7, 3, 6, 0);

        // Set m_itemTargetGUID = itemGUID taken from this packet (CMSG_CAST_SPELL)
        // It will be used by Spell::CheckItems
        if (Item* pItem = _player->GetItemByGuid(itemTargetGuid))
        {
            targets.SetItemTargetGUID(itemTargetGuid);
            targets.SetItemTarget(pItem);
        }

        spell->m_researchData = researchData;
    }

    spell->prepare(&targets);
}

void WorldSession::HandleCancelCastOpcode(WorldPacket& recvPacket)
{
    uint32 spellId = 0;
    uint8 counter = 0;

    bool hasCounter = !recvPacket.ReadBit();
    bool hasSpellId = !recvPacket.ReadBit();

    recvPacket.FlushBits();

    if (hasSpellId)
        recvPacket >> spellId;

    if (hasCounter)
        recvPacket >> counter;

    if (_player->IsNonMeleeSpellCasted(false))
        _player->InterruptNonMeleeSpells(false, spellId, false);
}

void WorldSession::HandleCancelAuraOpcode(WorldPacket& recvPacket)
{
    uint32 spellId;
    recvPacket >> spellId;

    ObjectGuid guid;

    recvPacket.ReadBit(); // Fake Bit
    recvPacket.ReadGuidMask(guid, 6, 5, 1, 0, 4, 3, 2, 7);

    recvPacket.FlushBits();

    recvPacket.ReadGuidBytes(guid, 3, 2, 1, 0, 4, 7, 5, 6);

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
        return;

    // not allow remove spells with attr SPELL_ATTR0_CANT_CANCEL
    if (spellInfo->Attributes & SPELL_ATTR0_CANT_CANCEL)
        return;

    // channeled spell case (it currently casted then)
    if (spellInfo->IsChanneled())
    {
        if (Spell* curSpell = _player->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
            if (curSpell->m_spellInfo->Id == spellId)
                _player->InterruptSpell(CURRENT_CHANNELED_SPELL);
        return;
    }

    // non channeled case:
    // don't allow remove non positive spells
    // don't allow cancelling passive auras (some of them are visible)
    if (!spellInfo->IsPositive() || spellInfo->IsPassive())
        return;

    // maybe should only remove one buff when there are multiple?
    _player->RemoveOwnedAura(spellId, 0, 0, AURA_REMOVE_BY_CANCEL);
}

void WorldSession::HandlePetCancelAuraOpcode(WorldPacket& recvPacket)
{
    uint64 guid;
    uint32 spellId;

    recvPacket >> guid;
    recvPacket >> spellId;

    SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
    if (!spellInfo)
    {
        TC_LOG_ERROR("network", "WORLD: unknown PET spell id %u", spellId);
        return;
    }

    Creature* pet=ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, guid);

    if (!pet)
    {
        TC_LOG_ERROR("network", "HandlePetCancelAura: Attempt to cancel an aura for non-existant pet %u by player '%s'", uint32(GUID_LOPART(guid)), GetPlayer()->GetName().c_str());
        return;
    }

    if (pet != GetPlayer()->GetGuardianPet() && pet != GetPlayer()->GetCharm())
    {
        TC_LOG_ERROR("network", "HandlePetCancelAura: Pet %u is not a pet of player '%s'", uint32(GUID_LOPART(guid)), GetPlayer()->GetName().c_str());
        return;
    }

    if (!pet->IsAlive())
    {
        pet->SendPetActionFeedback(FEEDBACK_PET_DEAD);
        return;
    }

    pet->RemoveOwnedAura(spellId, 0, 0, AURA_REMOVE_BY_CANCEL);

    pet->AddCreatureSpellCooldown(spellId);
}

void WorldSession::HandleCancelGrowthAuraOpcode(WorldPacket& /*recvPacket*/) { }

void WorldSession::HandleCancelAutoRepeatSpellOpcode(WorldPacket& /*recvPacket*/)
{
    // may be better send SMSG_CANCEL_AUTO_REPEAT?
    // cancel and prepare for deleting
    _player->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
}

void WorldSession::HandleCancelChanneling(WorldPacket& recvData)
{
    recvData.read_skip<uint32>();                          // spellid, not used

    // ignore for remote control state (for player case)
    Unit* mover = _player->m_mover;
    if (mover != _player && mover->GetTypeId() == TYPEID_PLAYER)
        return;

    mover->InterruptSpell(CURRENT_CHANNELED_SPELL);
}

void WorldSession::HandleTotemDestroyed(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_TOTEM_DESTROYED");

    // ignore for remote control state
    if (_player->m_mover != _player)
        return;
    
    uint8 slotId;
    ObjectGuid guid;

    recvPacket >> slotId;

    uint8 bitOrder[8] = {4, 2, 1, 3, 0, 6, 7, 5};
    recvPacket.ReadBitInOrder(guid, bitOrder);

    uint8 byteOrder[8] = {6, 2, 4, 1, 5, 0, 3, 7};
    recvPacket.ReadBytesSeq(guid, byteOrder);

    ++slotId;
    if (slotId >= MAX_TOTEM_SLOT)
        return;

    if (!_player->m_SummonSlot[slotId])
        return;

    Creature* totem = GetPlayer()->GetMap()->GetCreature(_player->m_SummonSlot[slotId]);
    if (totem && totem->IsTotem() && totem->GetGUID() == guid)
        totem->ToTotem()->UnSummon();
}

void WorldSession::HandleSelfResOpcode(WorldPacket& /*recvData*/)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_SELF_RES");                  // empty opcode

    if (_player->HasAuraType(SPELL_AURA_PREVENT_RESURRECTION))
        return; // silent return, client should display error by itself and not send this opcode

    if (_player->GetUInt32Value(PLAYER_FIELD_SELF_RES_SPELL))
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(_player->GetUInt32Value(PLAYER_FIELD_SELF_RES_SPELL));
        if (spellInfo)
            _player->CastSpell(_player, spellInfo, false, 0);

        _player->SetUInt32Value(PLAYER_FIELD_SELF_RES_SPELL, 0);
    }
}

void WorldSession::HandleSpellClick(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_SPELLCLICK");

    ObjectGuid guid;

    recvData.ReadGuidMask(guid, 7, 4, 0, 3, 6, 5);

    uint8 unk = recvData.ReadBit();

    recvData.ReadGuidMask(guid, 1, 2);

    recvData.ReadGuidBytes(guid, 6, 1, 5, 4, 7, 2, 3, 0);

    // this will get something not in world. crash
    Creature* unit = ObjectAccessor::GetCreatureOrPetOrVehicle(*_player, guid);

    if (!unit)
        return;

    /// @todo Unit::SetCharmedBy: 28782 is not in world but 0 is trying to charm it! -> crash
    if (!unit->IsInWorld())
        return;

    unit->HandleSpellClick(_player);
}

void WorldSession::HandleMirrorImageDataRequest(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_GET_MIRRORIMAGE_DATA");
    uint64 guid;
    recvData >> guid;
    recvData.read_skip<uint32>(); // DisplayId ?

    // Get unit for which data is needed by client
    Unit* unit = ObjectAccessor::GetObjectInWorld(guid, (Unit*)NULL);
    if (!unit)
        return;

    if (!unit->HasAuraType(SPELL_AURA_CLONE_CASTER))
        return;

    // Get creator of the unit (SPELL_AURA_CLONE_CASTER does not stack)
    Unit* creator = unit->GetAuraEffectsByType(SPELL_AURA_CLONE_CASTER).front()->GetCaster();
    if (!creator)
        return;

    WorldPacket data(SMSG_MIRRORIMAGE_DATA, 68);
    data << uint64(guid);
    data << uint32(creator->GetDisplayId());
    data << uint8(creator->getRace());
    data << uint8(creator->getGender());
    data << uint8(creator->getClass());

    if (creator->GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = creator->ToPlayer();
        Guild* guild = NULL;

        if (uint32 guildId = player->GetGuildId())
            guild = sGuildMgr->GetGuildById(guildId);

        data << uint8(player->GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 0));   // skin
        data << uint8(player->GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 1));   // face
        data << uint8(player->GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 2));   // hair
        data << uint8(player->GetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, 3));   // haircolor
        data << uint8(player->GetByteValue(PLAYER_FIELD_REST_STATE, 0)); // facialhair
        data << uint64(guild ? guild->GetGUID() : 0);

        static EquipmentSlots const itemSlots[] =
        {
            EQUIPMENT_SLOT_HEAD,
            EQUIPMENT_SLOT_SHOULDERS,
            EQUIPMENT_SLOT_BODY,
            EQUIPMENT_SLOT_CHEST,
            EQUIPMENT_SLOT_WAIST,
            EQUIPMENT_SLOT_LEGS,
            EQUIPMENT_SLOT_FEET,
            EQUIPMENT_SLOT_WRISTS,
            EQUIPMENT_SLOT_HANDS,
            EQUIPMENT_SLOT_BACK,
            EQUIPMENT_SLOT_TABARD,
            EQUIPMENT_SLOT_END
        };

        // Display items in visible slots
        for (EquipmentSlots const* itr = &itemSlots[0]; *itr != EQUIPMENT_SLOT_END; ++itr)
        {
            if (*itr == EQUIPMENT_SLOT_HEAD && player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_HIDE_HELM))
                data << uint32(0);
            else if (*itr == EQUIPMENT_SLOT_BACK && player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_HIDE_CLOAK))
                data << uint32(0);
            else if (Item const* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, *itr))
                data << uint32(item->GetTemplate()->DisplayInfoID);
            else
                data << uint32(0);
        }
    }
    else
    {
        // Skip player data for creatures
        data << uint8(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
        data << uint32(0);
    }

    SendPacket(&data);
}

void WorldSession::HandleUpdateProjectilePosition(WorldPacket& recvPacket)
{
    TC_LOG_DEBUG("network", "WORLD: CMSG_UPDATE_PROJECTILE_POSITION");

    uint64 casterGuid;
    uint32 spellId;
    uint8 castCount;
    float x, y, z;    // Position of missile hit

    recvPacket >> casterGuid;
    recvPacket >> spellId;
    recvPacket >> castCount;
    recvPacket >> x;
    recvPacket >> y;
    recvPacket >> z;

    Unit* caster = ObjectAccessor::GetUnit(*_player, casterGuid);
    if (!caster)
        return;

    Spell* spell = caster->FindCurrentSpellBySpellId(spellId);
    if (!spell || !spell->m_targets.HasDst())
        return;

    Position pos = *spell->m_targets.GetDstPos();
    pos.Relocate(x, y, z);
    spell->m_targets.ModDst(pos);

    WorldPacket data(SMSG_SET_PROJECTILE_POSITION, 21);
    data << uint64(casterGuid);
    data << uint8(castCount);
    data << float(x);
    data << float(y);
    data << float(z);
    caster->SendMessageToSet(&data, true);
}

void WorldSession::HandleRequestCategoryCooldowns(WorldPacket& /*recvPacket*/)
{
    std::map<uint32, int32> categoryMods;
    Unit::AuraEffectList const& categoryCooldownAuras = _player->GetAuraEffectsByType(SPELL_AURA_MOD_SPELL_CATEGORY_COOLDOWN);
    for (Unit::AuraEffectList::const_iterator itr = categoryCooldownAuras.begin(); itr != categoryCooldownAuras.end(); ++itr)
    {
        std::map<uint32, int32>::iterator cItr = categoryMods.find((*itr)->GetMiscValue());
        if (cItr == categoryMods.end())
            categoryMods[(*itr)->GetMiscValue()] = (*itr)->GetAmount();
        else
            cItr->second += (*itr)->GetAmount();
    }

    WorldPacket data(SMSG_SPELL_CATEGORY_COOLDOWN, 11);
    data.WriteBits(categoryMods.size(), 21);
    data.FlushBits();

    for (std::map<uint32, int32>::const_iterator itr = categoryMods.begin(); itr != categoryMods.end(); ++itr)
    {
        data << uint32(itr->first);
        data << int32(-itr->second);
    }

    SendPacket(&data);
}

void WorldSession::SendTotemCreated(ObjectGuid TotemGUID, uint32 Duration, uint32 SpellID, uint8 Slot)
{
    WorldPacket data(SMSG_TOTEM_CREATED, 17);
    data.WriteBit(TotemGUID[6]);
    data.WriteBit(TotemGUID[1]);
    data.WriteBit(TotemGUID[2]);
    data.WriteBit(TotemGUID[5]);
    data.WriteBit(TotemGUID[3]);
    data.WriteBit(TotemGUID[4]);
    data.WriteBit(TotemGUID[7]);
    data.WriteBit(TotemGUID[0]);

    data << uint32(Duration);
    data << uint32(SpellID);

    data.WriteByteSeq(TotemGUID[3]);
    data.WriteByteSeq(TotemGUID[4]);
    data.WriteByteSeq(TotemGUID[5]);
    data.WriteByteSeq(TotemGUID[6]);
    data.WriteByteSeq(TotemGUID[0]);
    data.WriteByteSeq(TotemGUID[2]);

    data << uint8(Slot);

    data.WriteByteSeq(TotemGUID[1]);
    data.WriteByteSeq(TotemGUID[7]);
    _player->SendDirectMessage(&data);
}