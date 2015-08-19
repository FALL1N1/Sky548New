/*
* Copyright (C) 2008-2015 TrinityCore <http://www.trinitycore.org/>
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

#ifdef WIN32
#pragma warning(disable:4700) // uninitialized local variable
#pragma warning(disable:4101) // unreferenced local variable
#endif

#define REQ_LEVEL	80
class tele : public CreatureScript
{
public:

	tele()
		: CreatureScript("tele")
	{
	}
	bool OnGossipHello(Player* Plr, Creature* pCrea)
	{
		Plr->ADD_GOSSIP_ITEM(12, "Welcome to Celestial-WoW! Where would you like to go?", GOSSIP_SENDER_MAIN, 8888);
		// Main Menu for Alliance
		if (Plr->GetTeam() == ALLIANCE)
		{
			Plr->ADD_GOSSIP_ITEM(8, "Global Mall", GOSSIP_SENDER_MAIN, 100000);
			Plr->ADD_GOSSIP_ITEM(7, "Alliance Cities ->", GOSSIP_SENDER_MAIN, 1000);
			Plr->ADD_GOSSIP_ITEM(7, "Neutral Cities ->", GOSSIP_SENDER_MAIN, 3000);
			Plr->ADD_GOSSIP_ITEM(7, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
			Plr->ADD_GOSSIP_ITEM(9, "Raids ->", GOSSIP_SENDER_MAIN, 6);
			Plr->ADD_GOSSIP_ITEM(9, "World Bosses ->", GOSSIP_SENDER_MAIN, 90500);
			Plr->ADD_GOSSIP_ITEM(9, "|cffff0000Gurubashi Arena - PvP", GOSSIP_SENDER_MAIN, 3500);
		}
		else // Main Menu for Horde
		{
			Plr->ADD_GOSSIP_ITEM(8, "Global Mall", GOSSIP_SENDER_MAIN, 100000);
			Plr->ADD_GOSSIP_ITEM(7, "Horde Cities ->", GOSSIP_SENDER_MAIN, 2000);
			Plr->ADD_GOSSIP_ITEM(7, "Neutral Cities ->", GOSSIP_SENDER_MAIN, 3000);
			Plr->ADD_GOSSIP_ITEM(9, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
			Plr->ADD_GOSSIP_ITEM(9, "Raids ->", GOSSIP_SENDER_MAIN, 6);
			Plr->ADD_GOSSIP_ITEM(9, "World Bosses ->", GOSSIP_SENDER_MAIN, 90500);
			Plr->ADD_GOSSIP_ITEM(9, "|cffff0000Gurubashi Arena - PvP", GOSSIP_SENDER_MAIN, 3500);
		}

		Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* Plr, Creature* pCrea, uint32 /*uiSender*/, uint32 action)
	{

		// Not allow in combat
		if (Plr->IsInCombat())
		{
			Plr->CLOSE_GOSSIP_MENU();
			pCrea->MonsterSay("You are in combat!", LANG_UNIVERSAL, NULL);
			return false;
		}

		switch (action)

		{
		case 8888:
			Plr->PlayerTalkClass->ClearMenus();
			OnGossipHello(Plr, pCrea);
			break;

			// Global Mall
		case 100000:
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 3878.543213f, 2589.685547f, 757.201599f, 2.045576f);
			break;

			// Gurubashi
		case 3500:
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -13245.379883f, 194.428299f, 30.992887f, 1.098357f);
			break;

		case 1000: //Alliance Town
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Darnassus", GOSSIP_SENDER_MAIN, 1001);
			Plr->ADD_GOSSIP_ITEM(5, "Exodar", GOSSIP_SENDER_MAIN, 1005);
			Plr->ADD_GOSSIP_ITEM(5, "Ironforge", GOSSIP_SENDER_MAIN, 1010);
			Plr->ADD_GOSSIP_ITEM(5, "Stormwind", GOSSIP_SENDER_MAIN, 1015);
			Plr->ADD_GOSSIP_ITEM(5, "Shrine of Seven Stars", GOSSIP_SENDER_MAIN, 21000);
			Plr->ADD_GOSSIP_ITEM(7, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
			Plr->ADD_GOSSIP_ITEM(7, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 2000: //Horde Town
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Orgrimmar", GOSSIP_SENDER_MAIN, 2001);
			Plr->ADD_GOSSIP_ITEM(5, "Silvermoon", GOSSIP_SENDER_MAIN, 2005);
			Plr->ADD_GOSSIP_ITEM(5, "Thunder Bluff", GOSSIP_SENDER_MAIN, 2010);
			Plr->ADD_GOSSIP_ITEM(5, "Undercity", GOSSIP_SENDER_MAIN, 2015);
			Plr->ADD_GOSSIP_ITEM(5, "Shrine of Two Moons", GOSSIP_SENDER_MAIN, 22000);
			Plr->ADD_GOSSIP_ITEM(7, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
			Plr->ADD_GOSSIP_ITEM(7, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 3000: //Neutral Town
			Plr->PlayerTalkClass->ClearMenus();
			// Check if player is DK for "Ebon Hold"
			if (Plr->getClass() == 6)
			{
				Plr->ADD_GOSSIP_ITEM(5, "Dalaran", GOSSIP_SENDER_MAIN, 3010);
				Plr->ADD_GOSSIP_ITEM(5, "Shattrath City", GOSSIP_SENDER_MAIN, 3035);
				Plr->ADD_GOSSIP_ITEM(7, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);
			}
			else
			{
				Plr->ADD_GOSSIP_ITEM(5, "Dalaran", GOSSIP_SENDER_MAIN, 3010);
				Plr->ADD_GOSSIP_ITEM(5, "Shattrath City", GOSSIP_SENDER_MAIN, 3035);
				Plr->ADD_GOSSIP_ITEM(7, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);
			}
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 5000: //Dungeons
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(7, "Vanilla ->", GOSSIP_SENDER_MAIN, 5010);
			Plr->ADD_GOSSIP_ITEM(7, "Vanilla Page 2 ->", GOSSIP_SENDER_MAIN, 5020);
			Plr->ADD_GOSSIP_ITEM(7, "Outland ->", GOSSIP_SENDER_MAIN, 5025);
			Plr->ADD_GOSSIP_ITEM(7, "Northrend ->", GOSSIP_SENDER_MAIN, 5030);
			Plr->ADD_GOSSIP_ITEM(7, "Cataclysm ->", GOSSIP_SENDER_MAIN, 10000);
			Plr->ADD_GOSSIP_ITEM(7, "Pandaria ->", GOSSIP_SENDER_MAIN, 20000);
			Plr->ADD_GOSSIP_ITEM(7, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 5005: //Back To Main Menu
			Plr->PlayerTalkClass->ClearMenus();
			// Main Menu for Alliance
			Plr->ADD_GOSSIP_ITEM(12, "Welcome to Celestial-WoW! Where would you like to go?", GOSSIP_SENDER_MAIN, 8888);
			// Main Menu for Alliance
			if (Plr->GetTeam() == ALLIANCE)
			{
				Plr->ADD_GOSSIP_ITEM(8, "Global Mall", GOSSIP_SENDER_MAIN, 100000);
				Plr->ADD_GOSSIP_ITEM(7, "Alliance Cities ->", GOSSIP_SENDER_MAIN, 1000);
				Plr->ADD_GOSSIP_ITEM(7, "Neutral Cities ->", GOSSIP_SENDER_MAIN, 3000);
				Plr->ADD_GOSSIP_ITEM(7, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
				Plr->ADD_GOSSIP_ITEM(9, "Raids ->", GOSSIP_SENDER_MAIN, 6);
				Plr->ADD_GOSSIP_ITEM(9, "World Bosses ->", GOSSIP_SENDER_MAIN, 90500);
				Plr->ADD_GOSSIP_ITEM(9, "|cffff0000Gurubashi Arena - PvP", GOSSIP_SENDER_MAIN, 3500);
			}
			else // Main Menu for Horde
			{
				Plr->ADD_GOSSIP_ITEM(8, "Global Mall", GOSSIP_SENDER_MAIN, 100000);
				Plr->ADD_GOSSIP_ITEM(7, "Horde Cities ->", GOSSIP_SENDER_MAIN, 2000);
				Plr->ADD_GOSSIP_ITEM(7, "Neutral Cities ->", GOSSIP_SENDER_MAIN, 3000);
				Plr->ADD_GOSSIP_ITEM(9, "Dungeons ->", GOSSIP_SENDER_MAIN, 5000);
				Plr->ADD_GOSSIP_ITEM(9, "Raids ->", GOSSIP_SENDER_MAIN, 6);
				Plr->ADD_GOSSIP_ITEM(9, "World Bosses ->", GOSSIP_SENDER_MAIN, 90500);
				Plr->ADD_GOSSIP_ITEM(9, "|cffff0000Gurubashi Arena - PvP", GOSSIP_SENDER_MAIN, 3500);
			}
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 5010: // Vanilla Dungeons Page 1
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Blackfathom Deeps", GOSSIP_SENDER_MAIN, 6001);
			Plr->ADD_GOSSIP_ITEM(5, "Dire Maul", GOSSIP_SENDER_MAIN, 6010);
			Plr->ADD_GOSSIP_ITEM(5, "Maraudon", GOSSIP_SENDER_MAIN, 6015);
			Plr->ADD_GOSSIP_ITEM(5, "Ragefire Chasm", GOSSIP_SENDER_MAIN, 6025);
			Plr->ADD_GOSSIP_ITEM(5, "Razorfen Downs", GOSSIP_SENDER_MAIN, 6030);
			Plr->ADD_GOSSIP_ITEM(5, "Razorfen Kraul", GOSSIP_SENDER_MAIN, 6035);
			Plr->ADD_GOSSIP_ITEM(5, "Wailing Caverns", GOSSIP_SENDER_MAIN, 6050);
			Plr->ADD_GOSSIP_ITEM(5, "Blackrock Depths", GOSSIP_SENDER_MAIN, 7001);
			Plr->ADD_GOSSIP_ITEM(5, "Blackrock Spire", GOSSIP_SENDER_MAIN, 7005);
			Plr->ADD_GOSSIP_ITEM(5, "Gnomeregan", GOSSIP_SENDER_MAIN, 7020);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 5000);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 5020: // Vanilla Dungeons Page 2
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Scarlet Monastery", GOSSIP_SENDER_MAIN, 7040);
			Plr->ADD_GOSSIP_ITEM(5, "Stratholme", GOSSIP_SENDER_MAIN, 7055);
			Plr->ADD_GOSSIP_ITEM(5, "Sunken Temple", GOSSIP_SENDER_MAIN, 7060);
			Plr->ADD_GOSSIP_ITEM(5, "The Stockade", GOSSIP_SENDER_MAIN, 7065);
			Plr->ADD_GOSSIP_ITEM(5, "Uldaman", GOSSIP_SENDER_MAIN, 7070);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 5010);
			Plr->ADD_GOSSIP_ITEM(7, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());

			break;

		case 5025: //Outland	
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Auchindoun", GOSSIP_SENDER_MAIN, 8001);
			Plr->ADD_GOSSIP_ITEM(5, "Black Temple", GOSSIP_SENDER_MAIN, 8005);
			Plr->ADD_GOSSIP_ITEM(5, "Coilfang Reservoir", GOSSIP_SENDER_MAIN, 8010);
			Plr->ADD_GOSSIP_ITEM(5, "Gruul's Lair", GOSSIP_SENDER_MAIN, 8015);
			Plr->ADD_GOSSIP_ITEM(5, "Hellfire Citadel", GOSSIP_SENDER_MAIN, 8020);
			Plr->ADD_GOSSIP_ITEM(5, "Tempest Keep", GOSSIP_SENDER_MAIN, 8025);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 5000);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 5030: //Northrend	
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Ahn'Kahet", GOSSIP_SENDER_MAIN, 9001);
			Plr->ADD_GOSSIP_ITEM(5, "Azjol-Nerub", GOSSIP_SENDER_MAIN, 9005);
			Plr->ADD_GOSSIP_ITEM(5, "Drak'Tharon Keep", GOSSIP_SENDER_MAIN, 9010);
			Plr->ADD_GOSSIP_ITEM(5, "Gun'Drak", GOSSIP_SENDER_MAIN, 9015);
			Plr->ADD_GOSSIP_ITEM(5, "Icecrown Citadel", GOSSIP_SENDER_MAIN, 9020);
			Plr->ADD_GOSSIP_ITEM(5, "Naxxramas", GOSSIP_SENDER_MAIN, 9025);
			Plr->ADD_GOSSIP_ITEM(5, "The Nexus", GOSSIP_SENDER_MAIN, 9030);
			Plr->ADD_GOSSIP_ITEM(5, "The Obsidian Sanctum", GOSSIP_SENDER_MAIN, 9035);
			Plr->ADD_GOSSIP_ITEM(5, "Utgarde Keep", GOSSIP_SENDER_MAIN, 9040);
			Plr->ADD_GOSSIP_ITEM(5, "Utgarde Pinnacle", GOSSIP_SENDER_MAIN, 9045);
			Plr->ADD_GOSSIP_ITEM(5, "Ulduar", GOSSIP_SENDER_MAIN, 9050);
			Plr->ADD_GOSSIP_ITEM(5, "Violet Hold", GOSSIP_SENDER_MAIN, 9055);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 5000);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());

			break;

		case 10000: // Cataclysm Dungeons
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Blackrock Caverns", GOSSIP_SENDER_MAIN, 10050);
			Plr->ADD_GOSSIP_ITEM(5, "Deadmines", GOSSIP_SENDER_MAIN, 10100);
			Plr->ADD_GOSSIP_ITEM(5, "End Time", GOSSIP_SENDER_MAIN, 10150);
			Plr->ADD_GOSSIP_ITEM(5, "Grim Batol", GOSSIP_SENDER_MAIN, 10200);
			Plr->ADD_GOSSIP_ITEM(5, "Halls of Origination", GOSSIP_SENDER_MAIN, 10250);
			Plr->ADD_GOSSIP_ITEM(5, "Hour of Twilight", GOSSIP_SENDER_MAIN, 10300);
			Plr->ADD_GOSSIP_ITEM(5, "Lost City of the Tol'vir", GOSSIP_SENDER_MAIN, 10350);
			Plr->ADD_GOSSIP_ITEM(5, "Shadowfang Keep", GOSSIP_SENDER_MAIN, 10400);
			Plr->ADD_GOSSIP_ITEM(5, "The Stonecore", GOSSIP_SENDER_MAIN, 10450);
			Plr->ADD_GOSSIP_ITEM(5, "The Vortex Pinnacle", GOSSIP_SENDER_MAIN, 10500);
			Plr->ADD_GOSSIP_ITEM(5, "Throne of the Tides", GOSSIP_SENDER_MAIN, 10550);
			Plr->ADD_GOSSIP_ITEM(5, "Well of Eternity", GOSSIP_SENDER_MAIN, 10600);
			Plr->ADD_GOSSIP_ITEM(5, "Zul'Aman", GOSSIP_SENDER_MAIN, 10650);
			Plr->ADD_GOSSIP_ITEM(5, "Zul'Gurub", GOSSIP_SENDER_MAIN, 10700);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 5000);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());

			break;

		case 20000: // Pandaria Dungeons
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Gate of the Setting Sun", GOSSIP_SENDER_MAIN, 20050);
			Plr->ADD_GOSSIP_ITEM(5, "Mogu'shan Palace", GOSSIP_SENDER_MAIN, 20100);
			Plr->ADD_GOSSIP_ITEM(5, "Scarlet Halls", GOSSIP_SENDER_MAIN, 20150);
			Plr->ADD_GOSSIP_ITEM(5, "Scarlet Monastery", GOSSIP_SENDER_MAIN, 20200);
			Plr->ADD_GOSSIP_ITEM(5, "Scholomance", GOSSIP_SENDER_MAIN, 20250);
			Plr->ADD_GOSSIP_ITEM(5, "Shado-Pan Monastery", GOSSIP_SENDER_MAIN, 20300);
			Plr->ADD_GOSSIP_ITEM(5, "Siege of Niuzao Temple", GOSSIP_SENDER_MAIN, 20350);
			Plr->ADD_GOSSIP_ITEM(5, "Stormstout Brewery", GOSSIP_SENDER_MAIN, 20400);
			Plr->ADD_GOSSIP_ITEM(5, "Temple of the Jade Serpent", GOSSIP_SENDER_MAIN, 20450);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 5000);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());

			break;

		case 6: //Raids
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(7, "Vanilla ->", GOSSIP_SENDER_MAIN, 60000);
			Plr->ADD_GOSSIP_ITEM(7, "Outland ->", GOSSIP_SENDER_MAIN, 60050);
			Plr->ADD_GOSSIP_ITEM(7, "Northrend ->", GOSSIP_SENDER_MAIN, 60100);
			Plr->ADD_GOSSIP_ITEM(7, "Cataclysm ->", GOSSIP_SENDER_MAIN, 60150);
			Plr->ADD_GOSSIP_ITEM(7, "Pandaria ->", GOSSIP_SENDER_MAIN, 60200);
			Plr->ADD_GOSSIP_ITEM(7, "<- Main Menu", GOSSIP_SENDER_MAIN, 5005);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 60000: // Vanilla Raids
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Blackwing Lair", GOSSIP_SENDER_MAIN, 70100);
			Plr->ADD_GOSSIP_ITEM(5, "Ruins of Ahn'Qiraj", GOSSIP_SENDER_MAIN, 70150);
			Plr->ADD_GOSSIP_ITEM(5, "Temple of Ahn'Qiraj", GOSSIP_SENDER_MAIN, 70200);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 6);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 60050: // Outland Raids
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Karazhan", GOSSIP_SENDER_MAIN, 70250);
			Plr->ADD_GOSSIP_ITEM(5, "Gruul's Lair", GOSSIP_SENDER_MAIN, 70300);
			Plr->ADD_GOSSIP_ITEM(5, "Magtheridon's Lair", GOSSIP_SENDER_MAIN, 70350);
			Plr->ADD_GOSSIP_ITEM(5, "Serpentshrine Cavern", GOSSIP_SENDER_MAIN, 70400);
			Plr->ADD_GOSSIP_ITEM(5, "Tempest Keep: The Eye", GOSSIP_SENDER_MAIN, 70450);
			Plr->ADD_GOSSIP_ITEM(5, "Caverns of Time: Mount Hyjal", GOSSIP_SENDER_MAIN, 70500);
			Plr->ADD_GOSSIP_ITEM(5, "Black Temple", GOSSIP_SENDER_MAIN, 70550);
			Plr->ADD_GOSSIP_ITEM(5, "Sunwell Plateau", GOSSIP_SENDER_MAIN, 70600);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 6);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 60100: // Northrend Raids
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Naxxramas", GOSSIP_SENDER_MAIN, 80050);
			Plr->ADD_GOSSIP_ITEM(5, "Obsidian Sanctum", GOSSIP_SENDER_MAIN, 80100);
			Plr->ADD_GOSSIP_ITEM(5, "Vault of Archavon", GOSSIP_SENDER_MAIN, 80150);
			Plr->ADD_GOSSIP_ITEM(5, "The Eye of Eternity", GOSSIP_SENDER_MAIN, 80200);
			Plr->ADD_GOSSIP_ITEM(5, "Ulduar", GOSSIP_SENDER_MAIN, 80250);
			Plr->ADD_GOSSIP_ITEM(5, "Trial of the Crusader", GOSSIP_SENDER_MAIN, 80300);
			Plr->ADD_GOSSIP_ITEM(5, "Onyxia's Lair", GOSSIP_SENDER_MAIN, 80350);
			Plr->ADD_GOSSIP_ITEM(5, "Icecrown Citadel", GOSSIP_SENDER_MAIN, 80400);
			Plr->ADD_GOSSIP_ITEM(5, "Ruby Sanctum", GOSSIP_SENDER_MAIN, 80450);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 6);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 60150: // Catacylsm Raids
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Baradin Hold", GOSSIP_SENDER_MAIN, 80500);
			Plr->ADD_GOSSIP_ITEM(5, "Bastion of Twilight", GOSSIP_SENDER_MAIN, 80550);
			Plr->ADD_GOSSIP_ITEM(5, "Throne of the Four Winds", GOSSIP_SENDER_MAIN, 80600);
			Plr->ADD_GOSSIP_ITEM(5, "Blackwing Descent", GOSSIP_SENDER_MAIN, 80650);
			Plr->ADD_GOSSIP_ITEM(5, "Firelands", GOSSIP_SENDER_MAIN, 80700);
			Plr->ADD_GOSSIP_ITEM(5, "Dragon Soul", GOSSIP_SENDER_MAIN, 80750);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 6);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 60200: // Pandaria Raids
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Mogu'shan Vaults", GOSSIP_SENDER_MAIN, 90050);
			Plr->ADD_GOSSIP_ITEM(5, "Heart of Fear", GOSSIP_SENDER_MAIN, 90100);
			Plr->ADD_GOSSIP_ITEM(5, "Terrace of Endless Spring", GOSSIP_SENDER_MAIN, 90150);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 6);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

		case 90500: // World Bosses
			Plr->PlayerTalkClass->ClearMenus();
			Plr->ADD_GOSSIP_ITEM(5, "Sha of Anger", GOSSIP_SENDER_MAIN, 90550);
			Plr->ADD_GOSSIP_ITEM(5, "Galleon", GOSSIP_SENDER_MAIN, 90600);
			Plr->ADD_GOSSIP_ITEM(5, "Celestials (Timeless Isle)", GOSSIP_SENDER_MAIN, 98000);
			Plr->ADD_GOSSIP_ITEM(5, "Oondasta (Isle of Giants)", GOSSIP_SENDER_MAIN, 99000);
			Plr->ADD_GOSSIP_ITEM(5, "Nalak (Isle of Thunder) NYI", GOSSIP_SENDER_MAIN, 0);
			Plr->ADD_GOSSIP_ITEM(7, "<- Back", GOSSIP_SENDER_MAIN, 6);
			Plr->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCrea->GetGUID());
			break;

			// Alliance Locations

		case 1001: // Darnassus
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, 9948.961914f, 2478.533936f, 1316.130249f, 1.590730f);

			break;

		case 1005: // Exodar
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, -3979.73f, -11672.67f, -139.13f, 0.03f);

			break;

		case 1010: // Ironforge
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -4924.491699f, -951.436523f, 501.545135f, 5.400000f);

			break;

		case 1015: // Stormwind
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -8835.100586f, 620.213867f, 93.532265f, 0.777545f);

			break;

		case 21000: // Shrine of Seven Stars
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 821.502808f, 253.333267f, 503.919464f, 0.638051f);

			break;

			// Horde Locations

		case 2001: // Orgrimmar
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, 1569.302002f, -4398.538574f, 16.079157f, 0.414193f);

			break;

		case 2005: // Silvermoon
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, 9485.405273f, -7283.008789f, 14.283421f, 5.965403f);

			break;

		case 2010: // Thunder Bluff
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -1281.653198f, 130.327667f, 131.333115f, 5.195713f);

			break;

		case 2015: // Undercity
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, 1780.275635f, 239.056229f, 60.408932f, 3.136879f);

			break;

		case 22000: // Shrine of Two Moons
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 1678.535645f, 931.493713f, 471.425323f, 3.256570f);

			break;

			// Neutral Locations
		case 5555: // ???
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -378.718f, 1543.55f, 17.0085f, 2.99575f);

			break;

		case 3010: //Dalaran
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 5807.786621f, 588.372559f, 660.939026f, 1.680267f);

			break;

		case 3035:// Shattrath City
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, -1845.583740f, 5439.920898f, -12.427159f, 3.650046f);

			break;


			// Vanilla Dungeons Page 1

		case 6001:// Blackfathom Deeps
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, 4248.72f, 744.35f, -24.67f, 1.34f);

			break;

		case 6005:// Caverns of Time
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8173.66f, -4746.36f, 33.84f, 4.94f);

			break;

		case 6010:// Dire Maul
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -3960.95f, 1130.64f, 161.05f, 0.0f);

			break;

		case 6015:// Maraudon
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -1431.33f, 2962.34f, 98.23f, 4.74f);

			break;

		case 6020:// Onyxia's Lair
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -4707.44f, -3726.82f, 54.6723f, 3.8f);

			break;

		case 6025:// Ragefire Chasm
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, 1814.47f, -4419.46f, -18.78f, 5.28f);

			break;

		case 6030:// Razorfen Downs
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -4657.88f, -2525.59f, 81.4f, 4.16f);

			break;

		case 6035:// Razorfen Kraul
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -4463.6f, -1664.53f, 82.26f, 0.85f);

			break;

		case 6040:// Ruins of Ahn'Qiraj
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8413.33f, 1501.27f, 29.64f, 2.61f);

			break;

		case 6045:// Temple of Ahn'Qiraj
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8245.837891f, 1983.736206f, 129.071686f, 0.936195f);

			break;

		case 6050:// Wailing Caverns
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -722.53f, -2226.30f, 16.94f, 2.71f);

			break;

		case 6055:// Zul'Farrak
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -6801.9f, -2890.22f, 8.88f, 6.25f);

			break;

			// Vanilla Dungeons Page 2

		case 7001:// Blackrock Depths
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -7180.57f, -920.04f, 165.49f, 5.02f);

			break;

		case 7005:// Blackrock Spire
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -7526.77f, -1225.64f, 285.73f, 5.31f);

			break;

		case 7010:// Blackwing Lair
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(469, -7672.61f, -1107.21f, 396.65f, 3.75f);

			break;

		case 7015:// Deadmines
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -11208.2f, 1675.92f, 24.57f, 1.48f);

			break;

		case 7020:// Gnomeregan
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -5138.653809f, 890.234802f, 287.391541f, 2.275822f);

			break;

		case 7040:// Scarlet Monastery
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, 2843.89f, -693.74f, 139.32f, 5.11f);

			break;

		case 7045:// Scholomance
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, 1273.06f, -2574.01f, 92.66f, 2.06f);

			break;

		case 7050:// Shadowfang Keep
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -239.54f, 1550.8f, 76.89f, 1.18f);

			break;

		case 7055:// Stratholme
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, 3370.76f, -3343.63f, 142.26f, 5.23f);

			break;

		case 7060:// Sunken Temple
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -10346.92f, -3851.90f, -43.41f, 6.09f);

			break;

		case 7065:// The Stockade
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -8766.89f, 844.6f, 88.43f, 0.69f);

			break;

		case 7070:// Uldaman
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -6070.72f, -2955.33f, 209.78f, 0.05f);

			break;

			// Outland Dungeons

		case 8001:// Auchindoun
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, -3322.92f, 4931.02f, -100.56f, 1.86f);

			break;

		case 8005:// Black Temple
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, -3649.1f, 317.33f, 35.19f, 2.97f);

			break;

		case 8010:// Coilfang Reservoir
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, 721.08f, 6846.77f, -68.75f, 0.34f);

			break;

		case 8015:// Gruul's Lair
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, 3539.01f, 5082.36f, 1.69f, 0.0f);

			break;

		case 8020:// Hellfire Citadel
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, -292.71f, 3146.77f, 31.60f, 2.05f);

			break;

		case 8025:// Tempest Keep
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, 3087.62f, 1376.27f, 184.8f, 4.63f);

			break;

			// Northrend Dungeons

		case 9001:// Ahn'Kahet
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 3646.76f, 2045.17f, 1.79f, 4.37f);

			break;

		case 9005:// Azjol-Nerub
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 3675.43f, 2169.00f, 35.90f, 2.29f);

			break;

		case 9010:// Drak'Tharon Keep
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 4450.86f, -2045.25f, 162.83f, 0.00f);

			break;

		case 9015:// Gun'Drak
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 6937.54f, -4455.98f, 450.68f, 1.00f);

			break;

		case 9020:// Icecrown Citadel
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 6139.24f, 2233.72f, 510.79f, 3.56f);

			break;

		case 9025:// Naxxramas
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 3668.72f, -1262.46f, 243.622f, 4.785f);

			break;

		case 9030:// The Nexus
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 3789.95f, 6961.48f, 104.23f, 0.28f);

			break;

		case 9035:// The Obsidian Sanctum
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 3625.78f, 280.4f, -120.14f, 3.25f);

			break;

		case 9040:// Ulduar
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 1219.72f, -4865.28f, 41.25f, 0.31f);

			break;

		case 9045:// Utgarde Keep
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 1245.69f, -4856.59f, 216.86f, 3.45f);

			break;

		case 9050:// Utgarde Pinnacle
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 8976.24f, -1281.33f, 1059.01f, 0.58f);

			break;

		case 9055:// Violet Hold
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 5679.82f, 486.80f, 652.40f, 4.08f);

			break;

			// Cataclysm Dungeons

		case 10050:// Blackrock Caverns
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -7571.754883f, -1321.792236f, 245.536530f, 4.864721f);

			break;

		case 10100:// Deadmines
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -11208.277344f, 1673.817993f, 24.626690f, 1.512650f);

			break;

		case 10150:// End Time
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8308.328125f, -4465.562988f, -208.632889f, 0.432796f);

			break;

		case 10200:// Grim Batol
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -4061.051758f, -3450.575684f, 280.715546f, 0.315749f);

			break;

		case 10250:// Halls of Origination
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -10204.824219f, -1837.711914f, 20.127583f, 3.167534f);

			break;

		case 10300:// Hour of Twilight
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8292.702148f, -4566.308105f, -223.930969f, 4.752481f);

			break;

		case 10350:// Lost City of the Tol'vir
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -10679.065430f, -1307.216553f, 17.343424f, 3.381169f);

			break;

		case 10400:// Shadowfang Keep
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -234.675003f, 1561.630005f, 76.892097f, 1.231531f);

			break;

		case 10450:// The Stonecore
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(646, 1020.854065f, 660.956238f, 156.672287f, 4.927675f);

			break;

		case 10500:// The Vortex Pinnacle
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -11511.657227f, -2308.543701f, 608.392029f, 3.815558f);

			break;

		case 10550:// Throne of the Tides
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -5599.089844f, 5412.060059f, -1798.733643f, 5.637249f);

			break;

		case 10600:// Well of Eternity
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8598.068359f, -4010.312744f, -205.547150f, 1.372132f);

			break;

		case 10650:// Zul'Aman
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, 6851.129395f, -7996.834473f, 192.360992f, 4.700715f);

			break;

		case 10700:// Zul'Gurub
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -11916.082031f, -1212.084839f, 92.286949f, 4.712495f);

			break;

			// Pandaria Dungeons

		case 20050:// Gate of the Setting Sun
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 678.283936f, 2080.096680f, 371.699310f, 6.269781f);

			break;

		case 20100:// Mogu'shan Palace
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 1385.538574f, 444.627838f, 479.090363f, 5.460030f);

			break;

		case 20150:// Scarlet Halls
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, 2873.478271f, -818.857910f, 160.332413f, 3.560910f);

			break;

		case 20200:// Scarlet Monastery
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, 2909.784180f, -804.058228f, 160.332413f, 0.399680f);

			break;

		case 20250:// Scholomance
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, 1266.628906f, -2556.974121f, 94.126350f, 0.459366f);

			break;

		case 20300:// Shado-Pan Monastery
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 3628.976807f, 2538.049561f, 769.950195f, 0.405953f);

			break;

		case 20350:// Siege of Niuzao Temple
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 1432.531128f, 5085.062988f, 133.302139f, 0.402005f);

			break;

		case 20400:// Stormstout Brewery
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, -705.215027f, 1265.617188f, 136.024002f, 3.421074f);

			break;

		case 20450:// Temple of the Jade Serpent
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 961.371521f, -2463.018311f, 180.578964f, 4.420104f);

			break;

			// Vanilla Raids Locations

		case 70050:// Molten Core
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(409, 1090.916138f, -507.702026f, -104.040642f, 5.506685f);

			break;

		case 70100:// Blackwing Lair
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(469, -7664.856934f, -1102.056519f, 399.680450f, 0.602697f);

			break;

		case 70150:// Ruins of Ahn'Qiraj
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8412.460938f, 1502.792236f, 29.664701f, 2.690685f);

			break;

		case 70200:// Temple of Ahn'Qiraj
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8241.529297f, 1990.732056f, 129.071274f, 0.909280f);

			break;

			// Outland Raids Locations

		case 70250:// Karazhan
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -11119.538086f, -2012.186523f, 47.089775f, 0.709003f);

			break;

		case 70300:// Gruul's Lair
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, 3546.502930f, 5089.745605f, 3.327436f, 5.649148f);

			break;

		case 70350:// Magtheridon's Lair
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, -314.934448f, 3090.851563f, -116.462784f, 5.185768f);

			break;

		case 70400:// Serpentshrine Cavern
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, 812.467651f, 6865.915527f, -67.683540f, 6.281270f);

			break;

		case 70450:// Tempest Keep: The Eye
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, 3088.369873f, 1382.006470f, 184.854279f, 4.627746f);

			break;

		case 70500:// Caverns of Time: Mount Hyjal
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8180.515625f, -4192.102051f, -170.328613f, 1.203709f);

			break;

		case 70550:// Black Temple
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, -3644.682617f, 315.934906f, 35.221691f, 2.925086f);

			break;

		case 70600:// Sunwell Plateau
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(530, 12574.099609f, -6774.810059f, 15.090400f, 3.137880f);

			break;

			// Northrend Raids Locations

		case 80050:// Naxxramas
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 3671.189941f, -1264.585815f, 243.512070f, 5.038550f);

			break;

		case 80100:// Obsidian Sanctum
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 3457.100830f, 262.404388f, -113.814842f, 3.240710f);

			break;

		case 80150:// Vault of Archavon
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 5470.807617f, 2840.562256f, 418.674896f, 0.007854f);

			break;

		case 80200:// The Eye of Eternity
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 3861.765625f, 6988.832031f, 152.042572f, 5.796460f);

			break;

		case 80250:// Ulduar
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 9338.007813f, -1114.578369f, 1245.145996f, 6.270615f);

			break;

		case 80300:// Trial of the Crusader
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 8515.540039f, 729.356018f, 558.247986f, 1.604396f);

			break;

		case 80350:// Onyxia's Lair
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -4744.857910f, -3752.576172f, 50.351665f, 3.273367f);

			break;

		case 80400:// Icecrown Citadel
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 5792.206055f, 2072.966553f, 636.064209f, 3.587527f);

			break;

		case 80450:// Ruby Sanctum
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(571, 3599.983154f, 198.557877f, -113.736916f, 5.325771f);

			break;

			// Cataclysm Raids Locations

		case 80500:// Baradin Hold
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(732, -1266.979980f, 1049.989990f, 106.995003f, 3.153705f);

			break;

		case 80550:// Bastion of Twilight
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -4889.128418f, -4237.513184f, 827.763062f, 2.261736f);

			break;

		case 80600:// Throne of the Four Winds
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -11353.916992f, 57.921093f, 723.884399f, 1.989301f);

			break;

		case 80650:// Blackwing Descent
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(0, -7537.472168f, -1201.703857f, 477.723053f, 1.887442f);

			break;

		case 80700:// Firelands
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, 3993.966553f, -2952.521973f, 1002.546936f, 2.233010f);

			break;

		case 80750:// Dragon Soul
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(1, -8280.125000f, -4518.888672f, -218.677750f, 0.322133f);

			break;

			// Pandaria Raids Locations

		case 90050:// Mogu'shan Vaults
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 3988.593262f, 1104.330078f, 497.153564f, 2.268947f);

			break;

		case 90100:// Heart of Fear
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 170.030975f, 4044.145996f, 255.914291f, 1.780079f);

			break;

		case 90150:// Terrace of Endless Spring
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 953.235474f, -58.700344f, 509.947662f, 0.949086f);

			break;

			// World Bosses

		case 90550:// Sha of Anger
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 2053.445313f, 1259.858765f, 474.682861f, 4.072471f);

			break;

		case 90600:// Galleon
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, -633.415283f, -57.002029f, 155.540817f, 5.485884f);

			break;

		case 98000:// Timeless Celestials
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, -649.893f, -4900.282f, 2.35f, 4.733f);

			break;

		case 99000:// Oondasta
			Plr->CLOSE_GOSSIP_MENU();
			Plr->TeleportTo(870, 5935.182f, 1128.253f, 59.192f, 0.027f);

			break;
		default:
			break;

		}
		return true;
	}
};
void AddSC_tele()
{
	new tele();
}