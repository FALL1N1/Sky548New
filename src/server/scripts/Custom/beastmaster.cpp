#include "Pet.h"

class Npc_Beastmaster : public CreatureScript
{
public:
	Npc_Beastmaster() : CreatureScript("Npc_Beastmaster") { }

	void CreatePet(Player *player, Creature * m_creature, uint32 entry) {

		if(player->getClass() != CLASS_HUNTER)
		{
			player->GetSession()->SendNotification("You are not a Hunter!");
				return;
		}

		if(player->GetPet())
		{
			player->GetSession()->SendNotification("Dismiss your current pet first!");
			player->PlayerTalkClass->SendCloseGossip();
			return;
		}

		Creature *creatureTarget = m_creature->SummonCreature(entry, player->GetPositionX(), player->GetPositionY()+2, player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 500);
		if(!creatureTarget) return;

		Pet* pet = player->CreateTamedPetFrom(creatureTarget, 0);
		if(!pet) return;

		creatureTarget->setDeathState(JUST_DIED);
		creatureTarget->RemoveCorpse();
		creatureTarget->SetHealth(0);                       // just for nice GM-mode view

		pet->SetUInt64Value(UNIT_FIELD_CREATED_BY, player->GetGUID());
		pet->SetUInt32Value(UNIT_FIELD_FACTION_TEMPLATE, player->getFaction());

		pet->SetUInt32Value(UNIT_FIELD_LEVEL, player->getLevel() - 1);
		pet->GetMap()->AddToMap(pet->ToCreature());

		pet->SetUInt32Value(UNIT_FIELD_LEVEL, player->getLevel());

		pet->GetCharmInfo()->SetPetNumber(sObjectMgr->GeneratePetNumber(), true);

		if(!pet->InitStatsForLevel(player->getLevel()))
			TC_LOG_ERROR("entities.pet", "Fail: no init stats for entry %u", entry);

		pet->UpdateAllStats();

		// Old code, new one below this, testing something
		//player->SetMinion(pet, true, PET_SLOT_UNK_SLOT);
		//pet->SavePetToDB(player->ToPlayer()->getSlotForNewPet());
		//player->PetSpellInitialize();

		player->SetMinion(pet, true);

		pet->SavePetToDB(PET_SLOT_ACTUAL_PET_SLOT);
		player->PetSpellInitialize();
		player->GetSession()->SendStablePet(0);

		player->PlayerTalkClass->SendCloseGossip();
		player->GetSession()->SendNotification("Pet successfully added!");
	}


	bool OnGossipHello(Player *player, Creature * m_creature)
	{

		if(player->getClass() != CLASS_HUNTER)
		{
			player->GetSession()->SendNotification("You are not a Hunter!");
			return true;
		}
		player->ADD_GOSSIP_ITEM(10, "I want a Normal Pet", GOSSIP_SENDER_MAIN, 30);
		if (player->CanTameExoticPets())
		{
			player->ADD_GOSSIP_ITEM(10, "I want an Exotic Pet", GOSSIP_SENDER_MAIN, 50);
		}
			player->ADD_GOSSIP_ITEM(10, "Exit", GOSSIP_SENDER_MAIN, 150);
		player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player *player, Creature * m_creature, uint32 sender, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		switch (action)
		{

		case 100:
			player->ADD_GOSSIP_ITEM(10, "I want a Normal Pet", GOSSIP_SENDER_MAIN, 30);
			if (player->CanTameExoticPets())
			{
				player->ADD_GOSSIP_ITEM(10, "I want an Exotic Pet", GOSSIP_SENDER_MAIN, 50);
			}
			player->ADD_GOSSIP_ITEM(10, "Exit", GOSSIP_SENDER_MAIN, 150);
			player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
			break;

		case 150:
			player->CLOSE_GOSSIP_MENU();
			break;

		case 30:
			player->ADD_GOSSIP_ITEM(10, "<- Main Menu", GOSSIP_SENDER_MAIN, 100);
			player->ADD_GOSSIP_ITEM(10, "Next Page ->", GOSSIP_SENDER_MAIN, 31);
			player->ADD_GOSSIP_ITEM(10, "Bat", GOSSIP_SENDER_MAIN, 18);
			player->ADD_GOSSIP_ITEM(10, "Bear", GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(10, "Boar", GOSSIP_SENDER_MAIN, 2);
			player->ADD_GOSSIP_ITEM(10, "Cat", GOSSIP_SENDER_MAIN, 4);
			player->ADD_GOSSIP_ITEM(10, "Carrion Bird", GOSSIP_SENDER_MAIN, 5);
			player->ADD_GOSSIP_ITEM(10, "Crab", GOSSIP_SENDER_MAIN, 6);
			player->ADD_GOSSIP_ITEM(10, "Crocolisk", GOSSIP_SENDER_MAIN, 7);
			player->ADD_GOSSIP_ITEM(10, "Dragonhawk", GOSSIP_SENDER_MAIN, 17);
			player->ADD_GOSSIP_ITEM(10, "Gorilla", GOSSIP_SENDER_MAIN, 8);
			player->ADD_GOSSIP_ITEM(10, "Hound", GOSSIP_SENDER_MAIN, 9);
			player->ADD_GOSSIP_ITEM(10, "Hyena", GOSSIP_SENDER_MAIN, 10);
			player->ADD_GOSSIP_ITEM(10, "Moth", GOSSIP_SENDER_MAIN, 11);
			player->ADD_GOSSIP_ITEM(10, "Owl", GOSSIP_SENDER_MAIN, 12);
			player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
			break;

		case 31:  
			player->ADD_GOSSIP_ITEM(10, "<- Main Menu", GOSSIP_SENDER_MAIN, 30);
			player->ADD_GOSSIP_ITEM(10, "Next Page ->", GOSSIP_SENDER_MAIN, 32);
			player->ADD_GOSSIP_ITEM(10, "Raptor", GOSSIP_SENDER_MAIN, 20);
			player->ADD_GOSSIP_ITEM(10, "Ravager", GOSSIP_SENDER_MAIN, 19);
			player->ADD_GOSSIP_ITEM(10, "Strider", GOSSIP_SENDER_MAIN, 13);
			player->ADD_GOSSIP_ITEM(10, "Scorpid", GOSSIP_SENDER_MAIN, 414);
			player->ADD_GOSSIP_ITEM(10, "Spider", GOSSIP_SENDER_MAIN, 16);
			player->ADD_GOSSIP_ITEM(10, "Serpent", GOSSIP_SENDER_MAIN, 21);  
			player->ADD_GOSSIP_ITEM(10, "Turtle", GOSSIP_SENDER_MAIN, 15);
			player->ADD_GOSSIP_ITEM(10, "Wasp", GOSSIP_SENDER_MAIN, 93);
			player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
			break;

		case 32:  
			player->ADD_GOSSIP_ITEM(10, "<- Main Menu", GOSSIP_SENDER_MAIN, 30);
			player->ADD_GOSSIP_ITEM(10, "<- Previous Page", GOSSIP_SENDER_MAIN, 31);
			player->ADD_GOSSIP_ITEM(10, "Monkey", GOSSIP_SENDER_MAIN, 300);
			player->ADD_GOSSIP_ITEM(10, "Frost Dog", GOSSIP_SENDER_MAIN, 301);
			player->ADD_GOSSIP_ITEM(10, "Porcupine", GOSSIP_SENDER_MAIN, 302);
			player->ADD_GOSSIP_ITEM(10, "Crane", GOSSIP_SENDER_MAIN, 303);
			player->ADD_GOSSIP_ITEM(10, "Scarab", GOSSIP_SENDER_MAIN, 304);
			player->ADD_GOSSIP_ITEM(10, "Quilen", GOSSIP_SENDER_MAIN, 305);  
			player->ADD_GOSSIP_ITEM(10, "Dreadfang", GOSSIP_SENDER_MAIN, 306);
			player->ADD_GOSSIP_ITEM(10, "Deep Spider", GOSSIP_SENDER_MAIN, 307);
			player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
			break;

		case 50:  
			player->ADD_GOSSIP_ITEM(10, "<- Main Menu", GOSSIP_SENDER_MAIN, 100);
			player->ADD_GOSSIP_ITEM(10, "Next Page ->", GOSSIP_SENDER_MAIN, 51);
			player->ADD_GOSSIP_ITEM(10, "Chimaera", GOSSIP_SENDER_MAIN, 600);
			player->ADD_GOSSIP_ITEM(10, "Core Hound", GOSSIP_SENDER_MAIN, 52);
			player->ADD_GOSSIP_ITEM(10, "Devilsaur", GOSSIP_SENDER_MAIN, 53);
			player->ADD_GOSSIP_ITEM(10, "Rhino", GOSSIP_SENDER_MAIN, 54);
			player->ADD_GOSSIP_ITEM(10, "Silithid", GOSSIP_SENDER_MAIN, 55);
			player->ADD_GOSSIP_ITEM(10, "Worm", GOSSIP_SENDER_MAIN, 56);  
			player->ADD_GOSSIP_ITEM(10, "Loque'nahak", GOSSIP_SENDER_MAIN, 57);
			player->ADD_GOSSIP_ITEM(10, "Skoll", GOSSIP_SENDER_MAIN, 58);
			player->ADD_GOSSIP_ITEM(10, "Gondria", GOSSIP_SENDER_MAIN, 59);
			player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
			break;

		case 51:  
			player->ADD_GOSSIP_ITEM(10, "<- Main Menu", GOSSIP_SENDER_MAIN, 100);
			player->ADD_GOSSIP_ITEM(10, "<- Previous Page", GOSSIP_SENDER_MAIN, 50);
			player->ADD_GOSSIP_ITEM(10, "Green Ghost Owl", GOSSIP_SENDER_MAIN, 310);
			player->ADD_GOSSIP_ITEM(10, "Spectral Crab", GOSSIP_SENDER_MAIN, 311);
			player->ADD_GOSSIP_ITEM(10, "Ghost Wolf", GOSSIP_SENDER_MAIN, 312);
			player->ADD_GOSSIP_ITEM(10, "Green Spectral Saber Tiger", GOSSIP_SENDER_MAIN, 313);
			player->ADD_GOSSIP_ITEM(10, "Spectral Bear", GOSSIP_SENDER_MAIN, 314);
			player->SEND_GOSSIP_MENU(1, m_creature->GetGUID());
			break;

		case GOSSIP_OPTION_VENDOR:
			player->GetSession()->SendListInventory(m_creature->GetGUID());
			break;
		case 600: //chimera
			CreatePet(player, m_creature, 21879);
			break;
		case 52: //core hound
			CreatePet(player, m_creature, 21108);
			break;
		case 53: //devilsaur
			CreatePet(player, m_creature, 20931);
			break;
		case 54: //rhino
			CreatePet(player, m_creature, 30445);
			break;
		case 55: //silithid
			CreatePet(player, m_creature, 5460);
			break;
		case 56: //worm
			CreatePet(player, m_creature, 30148);
			break;
		case 57: //loque'nahak
			CreatePet(player, m_creature, 32517);
			break;
		case 58: //skoll
			CreatePet(player, m_creature, 35189);
			break;
		case 59: //gondria
			CreatePet(player, m_creature, 33776);
			break;
		case 16: //spider
			CreatePet(player, m_creature, 2349);
			break;
		case 17: //dragonhawk
			CreatePet(player, m_creature, 27946);
			break;
		case 18: //bat
			CreatePet(player, m_creature, 28233);
			break;
		case 19: //ravager
			CreatePet(player, m_creature, 17199);
			break;
		case 20: //raptor
			CreatePet(player, m_creature, 14821);
			break;
		case 21: //serpent
			CreatePet(player, m_creature, 28358);
			break;
		case 1: //bear
			CreatePet(player, m_creature, 29319);
			break;
		case 2: //boar
			CreatePet(player, m_creature, 29996);
			break;
		case 93: //bug
			CreatePet(player, m_creature, 28085);
			break;
		case 4: //cat
			CreatePet(player, m_creature, 28097);
			break;
		case 5: //carrion
			CreatePet(player, m_creature, 26838);
			break;
		case 6: //crab
			CreatePet(player, m_creature, 24478);
			break;   
		case 7: //crocolisk
			CreatePet(player, m_creature, 1417);
			break;  
		case 8: //gorila
			CreatePet(player, m_creature, 28213);
			break;
		case 9: //hound
			CreatePet(player, m_creature, 29452);
			break;
		case 10: //hyena
			CreatePet(player, m_creature, 13036);
			break;
		case 11: //moth
			CreatePet(player, m_creature, 27421);
			break;
		case 12: //owl
			CreatePet(player, m_creature, 23136);
			break;
		case 13: //strider
			CreatePet(player, m_creature, 22807);
			break;
		case 414: //scorpid
			CreatePet(player, m_creature, 14476);
			break;
		case 15: //turtle
			CreatePet(player, m_creature, 25482);
			break;
		case 300: //monkey
			CreatePet(player, m_creature, 52441);
			break;
		case 301: //frost dog
			CreatePet(player, m_creature, 26730);
			break;
		case 302: //Porcupine
			CreatePet(player, m_creature, 64745);
			break;
		case 303: //Crane
			CreatePet(player, m_creature, 59763);
			break;
		case 304: //Scarab
			CreatePet(player, m_creature, 47079);
			break; 
		case 305: //Yao
			CreatePet(player, m_creature, 61306);
			break; 
		case 306: //Dreadfang
			CreatePet(player, m_creature, 5056);
			break; 
		case 307: //Deep Spider
			CreatePet(player, m_creature, 49815);
			break; 
		case 310: //Green Ghost Owl
			CreatePet(player, m_creature, 54320);
			break; 
		case 311: //Spectral Crab
			CreatePet(player, m_creature, 50051);
			break; 
		case 312: //Ghost Wolf
			CreatePet(player, m_creature, 50138);
			break; 
		case 313: //Green Spectral Saber Tiger
			CreatePet(player, m_creature, 54318);
			break; 
		case 314: //Arctus
			CreatePet(player, m_creature, 38453);
			break; 
		}
		return true;
	}
};

void AddSC_Npc_Beastmaster()
{
	new Npc_Beastmaster();
}