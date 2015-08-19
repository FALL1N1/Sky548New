/*
* Copyright (C) 2015 Celestial <http://www.celestial-wow.com/>
* Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
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

#include "ScriptPCH.h"

class npc_global_trainer : public CreatureScript
{
public:
	npc_global_trainer() : CreatureScript("npc_global_trainer"){}

	bool OnGossipHello(Player * pPlayer, Creature * pCreature)
	{
		pPlayer->ADD_GOSSIP_ITEM(4, "Reset my Talents", GOSSIP_SENDER_MAIN, 0);
		pPlayer->ADD_GOSSIP_ITEM(4, "Reset my Spec", GOSSIP_SENDER_MAIN, 1);
		pPlayer->ADD_GOSSIP_ITEM(4, "Learn Dual Specialization", GOSSIP_SENDER_MAIN, 2);
		pPlayer->PlayerTalkClass->SendGossipMenu(9425, pCreature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player * Player, Creature * Creature, uint32 /*uiSender*/, uint32 uiAction)
	{
		if (!Player)
			return true;

		switch (uiAction)
		{

		case 0:
		{
			Player->ResetTalents(true);
			Player->SendTalentsInfoData();
			Player->GetSession()->SendNotification("Your Talents have been reset!");
			Player->PlayerTalkClass->SendCloseGossip();
		}
			break;

		case 1:
		{
			Player->ResetSpec();
			Player->GetSession()->SendNotification("Your Specialization has been reset!");
			Player->PlayerTalkClass->SendCloseGossip();
		}
			break;

		case 2:
		{
			Player->CastSpell(Player, 63680, true, NULL, NULL, Player->GetGUID());
			Player->CastSpell(Player, 63624, true, NULL, NULL, Player->GetGUID());
			Player->GetSession()->SendNotification("You have learned Dual Specialization!");
			Player->PlayerTalkClass->SendCloseGossip();
		}
			break;

		}
		return true;
	}

};

void AddSC_npc_global_trainer()
{
	new npc_global_trainer();
}

