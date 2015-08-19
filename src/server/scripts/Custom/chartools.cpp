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
     
class npc_changer : public CreatureScript
{
        public:
                npc_changer() : CreatureScript("npc_changer"){}
     
                bool OnGossipHello(Player * pPlayer, Creature * pCreature)
                {
                    pPlayer->ADD_GOSSIP_ITEM(4, "Race Change", GOSSIP_SENDER_MAIN, 0);
                    pPlayer->ADD_GOSSIP_ITEM(4, "Faction Change", GOSSIP_SENDER_MAIN, 1);
					pPlayer->ADD_GOSSIP_ITEM(4, "Character Customization", GOSSIP_SENDER_MAIN, 2);
                    pPlayer->PlayerTalkClass->SendGossipMenu(9425, pCreature->GetGUID());
                        return true;
                }
     
                bool OnGossipSelect(Player * Player, Creature * Creature, uint32 /*uiSender*/, uint32 uiAction)
                {
                        if(!Player)
							return true;
     
				switch(uiAction)
				{
					case 0:
					{
						//Player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
						Player->GetSession()->SendNotification("Race change isn't yet implemented, check back later!");
						Player->PlayerTalkClass->SendCloseGossip();
					}

					{
						Player->PlayerTalkClass->SendCloseGossip();
					}
					break;

					case 1:
					{
						//Player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
						Player->GetSession()->SendNotification("Faction change isn't yet implemented, check back later!");
						Player->PlayerTalkClass->SendCloseGossip();
					}

					{
						Player->PlayerTalkClass->SendCloseGossip();
					}
						break;

					case 2:
					{
						Player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
						Player->GetSession()->SendNotification("Please relog to customize your character!");
						Player->PlayerTalkClass->SendCloseGossip();
					}

					{
						Player->PlayerTalkClass->SendCloseGossip();
					}
						break;
				}
					
		return true;
    }
};
     
void AddSC_npc_changer()
{
	new npc_changer();
}
