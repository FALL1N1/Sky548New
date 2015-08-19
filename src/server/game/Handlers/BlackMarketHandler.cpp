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

#include "BlackMarketMgr.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "Unit.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"

void WorldSession::HandleBlackMarketHelloOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BLACKMARKET_HELLO");

    ObjectGuid guid;

    recvData.ReadGuidMask(guid, 4, 5, 2, 7, 0, 1, 3, 6);

    recvData.ReadGuidBytes(guid, 3, 5, 0, 6, 4, 1, 7, 2);

    uint64 npcGuid = uint64(guid);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(npcGuid, UNIT_NPC_FLAG_BLACKMARKET);

    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: Received CMSG_BLACKMARKET_HELLO - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(npcGuid)));
        return;
    }

    if (GetPlayer()->HasUnitState(UNIT_STATE_DIED))
        GetPlayer()->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

    SendBlackMarketHello(npcGuid);
}

void WorldSession::SendBlackMarketHello(uint64 npcGuid)
{
    TC_LOG_DEBUG("network", "WORLD: Received SMSG_BLACK_MARKET_HELLO");

    WorldPacket data(SMSG_BLACK_MARKET_HELLO, 8);

    ObjectGuid guid = npcGuid;

    data.WriteGuidMask(guid, 2, 0, 4, 1, 3, 6, 5, 7);

    data.WriteBit(1); // Unknow

    data.WriteGuidBytes(guid, 6, 1, 2, 5, 0, 7, 4, 3);

    SendPacket(&data);
}

void WorldSession::HandleBlackMarketRequestItemOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BLACKMARKET_REQUEST_ITEMS");

    ObjectGuid guid;
    uint32 Timestamp;

    recvData >> Timestamp;

    recvData.ReadGuidMask(guid, 2, 6, 0, 3, 4, 5, 1, 7);

    recvData.ReadGuidBytes(guid, 6, 2, 3, 5, 7, 4, 1, 0);

    uint64 NpcGuid = uint64(guid);

    Creature* unit = GetPlayer()->GetNPCIfCanInteractWith(NpcGuid, UNIT_NPC_FLAG_BLACKMARKET);
    if (!unit)
    {
        TC_LOG_DEBUG("network", "WORLD: Received CMSG_BLACKMARKET_REQUEST_ITEMS - Unit (GUID: %u) not found or you can't interact with him.", uint32(GUID_LOPART(NpcGuid)));
        return;
    }

    SendBlackMarketRequestItemsResult();
}

void WorldSession::SendBlackMarketRequestItemsResult()
{
    TC_LOG_DEBUG("network", "WORLD: Received SMSG_BLACK_MARKET_REQUEST_ITEMS_RESULT");

    WorldPacket data(SMSG_BLACK_MARKET_REQUEST_ITEMS_RESULT, 9);
    sBlackMarketMgr->BuildBlackMarketAuctionsPacket(data, GetPlayer()->GetGUIDLow());
    SendPacket(&data);
}

void WorldSession::HandleBlackMarketBidOpcode(WorldPacket& recvData)
{
    TC_LOG_DEBUG("network", "WORLD: Received CMSG_BLACKMARKET_BID");

    ObjectGuid guid;
    uint32 itemid, id;
    uint64 price;

    recvData >> id >> itemid >> price;

    recvData.ReadGuidMask(guid, 0, 5, 4, 3, 7, 6, 1, 2);

    recvData.ReadGuidBytes(guid, 4, 3, 6, 5, 7, 1, 0, 2);

    uint64 npcGuid = uint64(guid);

    if (!price)
        return;

    Creature* creature = GetPlayer()->GetNPCIfCanInteractWith(npcGuid, UNIT_NPC_FLAG_BLACKMARKET);
    if (!creature)
        return;

    BMAuctionEntry *auction = sBlackMarketMgr->GetAuction(id);
    if (!auction)
        return;

    if (auction->bidder == GetPlayer()->GetGUIDLow()) // Trying to cheat
        return;

    if (auction->bid >= price && price != auction->bm_template->startBid) // Trying to cheat
        return;

    sBlackMarketMgr->UpdateAuction(auction, price, GetPlayer());

    SendBlackMarketBidResult();
    SendBlackMarketRequestItemsResult();
}

void WorldSession::SendBlackMarketBidResult()
{
    TC_LOG_DEBUG("network", "WORLD: Received SMSG_BLACK_MARKET_BID_RESULT");

    WorldPacket data(SMSG_BLACK_MARKET_BID_RESULT, 5);

    data << uint32(0); //unk
    data.WriteBits(0, 2);

    SendPacket(&data);
}