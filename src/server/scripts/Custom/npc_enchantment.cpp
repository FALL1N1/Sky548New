  enum Enchants
{
	ENCHANT_WEP_BLOODY_DANCING_STEEL	= 5125,
    ENCHANT_WEP_MAJOR_LAND				= 4099,
	ENCHANT_WEP_POWER 					= 4097,
	ENCHANT_WEP_AVALANCH				= 4067,
	ENCHANT_WEP_HEARTSON				= 4084,
	ENCHANT_WEP_MENDING	    			= 4066,
	ENCHANT_WEP_WINDWALK 				= 4098,
	ENCHANT_WEP_PYRIUM					= 4217,
	ENCHANT_WEP_HURRICANE				= 4083,
	ENCHANT_WEP_POTENCY  				= 3833,
	ENCHANT_WEP_AGILITY					= 4227,
	ENCHANT_WEP_ACCURACY     			= 3788,
	ENCHANT_WEP_MAJOR_AGILITY			= 2670,
	ENCHANT_WEP_LIFESTEALING			= 3870,
	ENCHANT_WEP_INTELLECT 				= 4091,
	ENCHANT_WEP_BLADE	 				= 3869,

	ENCHANT_SHIELD_ARMOR				= 4073,
	ENCHANT_SHIELD_RESILIENCE			= 3229,
	ENCHANT_SHIELD_MASTERY				= 4085,
	ENCHANT_SHIELD_STAM					= 1071,
	ENCHANT_SHIELD_INT					= 4091,


	ENCHANT_CLOAK_INTELLECT				= 4096,
	ENCHANT_CLOAK_STRIKE				= 4100,
	ENCHANT_CLOAK_STAMINA				= 4090,
	ENCHANT_CLOAK_SPELL_PEN	   		    = 4064,
	ENCHANT_CLOAK_HASTE					= 3831,
	ENCHANT_CLOAK_DEF					= 1951,
	ENCHANT_CLOAK_SPIRIT				= 3296,

	ENCHANT_GLOVES_SPELLPOWER			= 3246,
	ENCHANT_GLOVES_EXPERTISE			= 4082,
	ENCHANT_GLOVES_HIT					= 3234,
	ENCHANT_GLOVES_ATTACK				= 1603,
	ENCHANT_GLOVES_AGILITY				= 3222,
	ENCHANT_GLOVES_MASTERY				= 4107,
	ENCHANT_GLOVES_HASTE				= 4068,
	ENCHANT_GLOVES_STRENGTH				= 4106,

	ENCHANT_WRIST_STRENGTH				= 4256,
	ENCHANT_WRIST_SPELL_INTELLECT 		= 4257,
	ENCHANT_WRIST_SPIRIT				= 4093,
	ENCHANT_WRIST_AGILITY				= 4258,
	ENCHANT_WRIST_SPEED					= 4108,
	ENCHANT_WRIST_EXPERTISE				= 4095,
	ENCHANT_WRIST_PRECISION				= 4989,
	ENCHANT_WRIST_CRIT					= 4101,
	ENCHANT_WRIST_DODGE					= 4086,

	ENCHANT_CHEST_STATS					= 4102,
	ENCHANT_CHEST_RESILIENCE			= 4077,
	ENCHANT_CHEST_HEALTH				= 4103,
	ENCHANT_CHEST_SPIRIT				= 4088,

	ENCHANT_BOOTS_AGISPEED				= 4105,
	ENCHANT_BOOTS_MASPEED				= 4104,
	ENCHANT_BOOTS_STAMSPEED				= 4062,
	ENCHANT_BOOTS_MASTERY				= 4094,
	ENCHANT_BOOTS_HIT					= 4092,
	ENCHANT_BOOTS_AGI					= 4076,
	ENCHANT_BOOTS_HASTE					= 4069,
};

#include "ScriptPCH.h"
 
void Enchant(Player* player, Item* item, uint32 enchantid)
{
    if (!item)
    {
        player->GetSession()->SendNotification("You must first equip the item you are trying to enchant in order to enchant it!");
        return;
    }

    if (!enchantid)
    {
        player->GetSession()->SendNotification("Something went wrong in the code. It has been logged for developers and will be looked into, sorry for the inconvenience.");
        return;
    }
		
    player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, false);
       item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantid, 0, 0);
       player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, true);
    player->GetSession()->SendNotification("|cff800080%s |cffFF0000succesfully enchanted!", item->GetTemplate()->Name1.c_str());
}

void RemoveEnchant(Player* player, Item* item)
{
	if (!item)
	{
		player->GetSession()->SendNotification("You don't have the item equipped?");
		return;
	}

	item->ClearEnchantment(PERM_ENCHANTMENT_SLOT);
	player->GetSession()->SendNotification("|cff800080%s's |cffFF0000enchant has successfully been removed!", item->GetTemplate()->Name1.c_str());
}

class npc_enchantment : public CreatureScript
{
public:
    npc_enchantment() : CreatureScript("npc_enchantment") { }
 
        bool OnGossipHello(Player* player, Creature* creature)
        {
            player->ADD_GOSSIP_ITEM(1, "[Enchant Weapon]", GOSSIP_SENDER_MAIN, 1);
			player->ADD_GOSSIP_ITEM(1, "[Enchant Off-Hand]", GOSSIP_SENDER_MAIN, 2);
			player->ADD_GOSSIP_ITEM(1, "[Enchant Caster Off-Hand]", GOSSIP_SENDER_MAIN, 3);
			player->ADD_GOSSIP_ITEM(1, "[Enchant Shield]", GOSSIP_SENDER_MAIN, 4);
            player->ADD_GOSSIP_ITEM(1, "[Enchant Cloak]", GOSSIP_SENDER_MAIN, 6);
            player->ADD_GOSSIP_ITEM(1, "[Enchant Chest]", GOSSIP_SENDER_MAIN, 7);
            player->ADD_GOSSIP_ITEM(1, "[Enchant Bracers]", GOSSIP_SENDER_MAIN, 8);
            player->ADD_GOSSIP_ITEM(1, "[Enchant Gloves]", GOSSIP_SENDER_MAIN, 9);
            player->ADD_GOSSIP_ITEM(1, "[Enchant Feet]", GOSSIP_SENDER_MAIN, 11);



            player->PlayerTalkClass->SendGossipMenu(100001, creature->GetGUID());
			return true;
		}
 
        bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action)
        {
			player->PlayerTalkClass->ClearMenus();
			Item * item;

            switch (action)
            {
				case 15:
			{
				player->PlayerTalkClass->SendCloseGossip();
				break;
			}
                case 1: // Enchant Weapon
					player->ADD_GOSSIP_ITEM(5, "Bloody Dancing Steel", GOSSIP_SENDER_MAIN, 5000);
					player->ADD_GOSSIP_ITEM(5, "Power Torrent - 500 int Proc", GOSSIP_SENDER_MAIN, 100);
					player->ADD_GOSSIP_ITEM(5, "Hurricane - 450 Haste Proc", GOSSIP_SENDER_MAIN, 103);
					player->ADD_GOSSIP_ITEM(5, "Pyrium Weapon Chain 50% reduc on disarm/40 hit Proc", GOSSIP_SENDER_MAIN, 104);
					player->ADD_GOSSIP_ITEM(5, "Landslide - 1000 AP Proc", GOSSIP_SENDER_MAIN, 105);
					player->ADD_GOSSIP_ITEM(5, "Avalanch - 463 - 537 Nature Damage", GOSSIP_SENDER_MAIN, 106);
					player->ADD_GOSSIP_ITEM(5, "Heartsong 200 Spirit Proc", GOSSIP_SENDER_MAIN, 107);
					player->ADD_GOSSIP_ITEM(5, "Mending - Heals player upon melee", GOSSIP_SENDER_MAIN, 108);
					player->ADD_GOSSIP_ITEM(5, "Windwalk - 600 Dodge/15% speed Proc", GOSSIP_SENDER_MAIN, 109);
					player->ADD_GOSSIP_ITEM(5, "Blade Ward - +200 Parry 600-800 Dmg Parry Proc", GOSSIP_SENDER_MAIN, 914);
					player->ADD_GOSSIP_ITEM(1, "<-Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(100002, creature->GetGUID());
					break;
				case 2: // Enchant Off-Hand
					if (item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
					{
						if (ItemTemplate const* iInfo = item->GetTemplate())
						{
							if ((iInfo->SubClass == ITEM_SUBCLASS_WEAPON_AXE || iInfo->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER || ITEM_SUBCLASS_WEAPON_WAND ||
								iInfo->SubClass == ITEM_SUBCLASS_WEAPON_MACE || iInfo->SubClass == ITEM_SUBCLASS_WEAPON_SWORD) && iInfo->Class == ITEM_CLASS_WEAPON)
							{
								player->ADD_GOSSIP_ITEM(5, "Heartsong - 200 Spirit Proc", GOSSIP_SENDER_MAIN, 1341);
								player->ADD_GOSSIP_ITEM(5, "Mending Heals player upon melee", GOSSIP_SENDER_MAIN, 1084);
								player->ADD_GOSSIP_ITEM(5, "Windwalk - 600 Dodge/15% speed Proc", GOSSIP_SENDER_MAIN, 1094);
								player->ADD_GOSSIP_ITEM(5, "Pyrium Weapon Chain 50% reduc on disarm/40 hit Proc", GOSSIP_SENDER_MAIN, 1238);
								player->ADD_GOSSIP_ITEM(5, "Hurricane - 450 Haste Proc", GOSSIP_SENDER_MAIN, 1233);
								player->ADD_GOSSIP_ITEM(5, "Avalanch - 463 - 537 Nature Damage", GOSSIP_SENDER_MAIN, 1789);
								player->ADD_GOSSIP_ITEM(5, "Power Torrent - 500 int Proc", GOSSIP_SENDER_MAIN, 1743);
								player->ADD_GOSSIP_ITEM(5, "Landslide - 1000 AP Proc", GOSSIP_SENDER_MAIN, 1333);
								player->ADD_GOSSIP_ITEM(1, "<-Back", GOSSIP_SENDER_MAIN, 300);
								player->SEND_GOSSIP_MENU(100002, creature->GetGUID());
							}
							else
							{
								ChatHandler(player->GetSession()).PSendSysMessage("Your off-hand cannot be enchanted");
								OnGossipHello(player, creature);
							}
						}
					}
					else
					{
						ChatHandler(player->GetSession()).PSendSysMessage("You don't have an offhand!");
						OnGossipHello(player, creature);
					}
					break;


				case 3: // Enchant Caster Off-Hand
					player->ADD_GOSSIP_ITEM(5, "Intellect - +40", GOSSIP_SENDER_MAIN, 543);
					player->ADD_GOSSIP_ITEM(1, "<-Back", GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100003, creature->GetGUID());
					break;

                case 4: // Enchant Shield
					player->ADD_GOSSIP_ITEM(5, "+160 Armor", GOSSIP_SENDER_MAIN, 110);
					player->ADD_GOSSIP_ITEM(5, "+12 PvP Resilience", GOSSIP_SENDER_MAIN, 111);
					player->ADD_GOSSIP_ITEM(5, "Mastery - +50", GOSSIP_SENDER_MAIN, 915);
					player->ADD_GOSSIP_ITEM(5, "Stamina - +18", GOSSIP_SENDER_MAIN, 916);
					player->ADD_GOSSIP_ITEM(5, "Intellect - +40", GOSSIP_SENDER_MAIN, 946);
					player->ADD_GOSSIP_ITEM(1, "<-Back", GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100004, creature->GetGUID());
					break;

                case 6: // Enchant Cloak
					player->ADD_GOSSIP_ITEM(5, "Intellect +50", GOSSIP_SENDER_MAIN, 142);
					player->ADD_GOSSIP_ITEM(5, "+250 Armor", GOSSIP_SENDER_MAIN, 114);
					player->ADD_GOSSIP_ITEM(5, "+65 Critical Strike", GOSSIP_SENDER_MAIN, 115);
					player->ADD_GOSSIP_ITEM(5, "Haste - +23", GOSSIP_SENDER_MAIN, 136);
					player->ADD_GOSSIP_ITEM(5, "Spell Penetration - +70", GOSSIP_SENDER_MAIN, 139);
					player->ADD_GOSSIP_ITEM(5, "Defense Rating - +16", GOSSIP_SENDER_MAIN, 137);
					player->ADD_GOSSIP_ITEM(5, "Spirit - +10", GOSSIP_SENDER_MAIN, 138);
					player->ADD_GOSSIP_ITEM(1, "<-Back", GOSSIP_SENDER_MAIN, 300);
                    player->PlayerTalkClass->SendGossipMenu(100007, creature->GetGUID());
					break;

				case 7: //Enchant chest
					player->ADD_GOSSIP_ITEM(5, "+75 Stamina", GOSSIP_SENDER_MAIN, 116);
					player->ADD_GOSSIP_ITEM(5, "Resil +40", GOSSIP_SENDER_MAIN, 117);
					player->ADD_GOSSIP_ITEM(5, "All stats - +20", GOSSIP_SENDER_MAIN, 118);
					player->ADD_GOSSIP_ITEM(5, "Spirit - +40", GOSSIP_SENDER_MAIN, 451);
					player->ADD_GOSSIP_ITEM(1, "<-Back", GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100008, creature->GetGUID());
					break;

				case 8: //Enchant Bracers
					player->ADD_GOSSIP_ITEM(5, "Intellect +50", GOSSIP_SENDER_MAIN, 119);
					player->ADD_GOSSIP_ITEM(5, "Spirit - +50", GOSSIP_SENDER_MAIN, 120);
					player->ADD_GOSSIP_ITEM(5, "Strength - +50", GOSSIP_SENDER_MAIN, 121);
					player->ADD_GOSSIP_ITEM(5, "Agility - +50", GOSSIP_SENDER_MAIN, 122);
					player->ADD_GOSSIP_ITEM(5, "Haste - +65", GOSSIP_SENDER_MAIN, 900);
					player->ADD_GOSSIP_ITEM(5, "Expertise - +50", GOSSIP_SENDER_MAIN, 901);
					player->ADD_GOSSIP_ITEM(5, "Hit Rating - +50", GOSSIP_SENDER_MAIN, 902);
					player->ADD_GOSSIP_ITEM(5, "Crit - +65", GOSSIP_SENDER_MAIN, 903);
					player->ADD_GOSSIP_ITEM(5, "Dodge - +50", GOSSIP_SENDER_MAIN, 904);
					player->ADD_GOSSIP_ITEM(1, "<-Back", GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100009, creature->GetGUID());
					break;

				case 9: //Enchant Gloves
					player->ADD_GOSSIP_ITEM(5, "Major Agility - +20", GOSSIP_SENDER_MAIN, 123);
					player->ADD_GOSSIP_ITEM(5, "Attack Power - +44", GOSSIP_SENDER_MAIN, 124);
					player->ADD_GOSSIP_ITEM(5, "Hit Rating - +20", GOSSIP_SENDER_MAIN, 125);
					player->ADD_GOSSIP_ITEM(5, "Haste +50", GOSSIP_SENDER_MAIN, 126);
					player->ADD_GOSSIP_ITEM(5, "Spellpower - +28", GOSSIP_SENDER_MAIN, 127);
					player->ADD_GOSSIP_ITEM(5, "Expertise Rating - +50", GOSSIP_SENDER_MAIN, 128);
					player->ADD_GOSSIP_ITEM(5, "Mastery +65", GOSSIP_SENDER_MAIN, 129);
					player->ADD_GOSSIP_ITEM(5, "Strength +50", GOSSIP_SENDER_MAIN, 141);
					player->ADD_GOSSIP_ITEM(1, "<-Back", GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100010, creature->GetGUID());
					break;

				case 11: //Enchant feet
					player->ADD_GOSSIP_ITEM(5, "+25 Agility and Minor Movement Speed", GOSSIP_SENDER_MAIN, 130);
					player->ADD_GOSSIP_ITEM(5, "+50 Hit", GOSSIP_SENDER_MAIN, 131);
					player->ADD_GOSSIP_ITEM(5, "+35 Mastery and Minor Movement ", GOSSIP_SENDER_MAIN, 132);
					player->ADD_GOSSIP_ITEM(5, "+50 Mastery", GOSSIP_SENDER_MAIN, 133);
					player->ADD_GOSSIP_ITEM(5, "+30 Stamina and Minor Movement Speed", GOSSIP_SENDER_MAIN, 134);
					player->ADD_GOSSIP_ITEM(5, "Agility +35", GOSSIP_SENDER_MAIN, 917);
					player->ADD_GOSSIP_ITEM(5, "Haste +50", GOSSIP_SENDER_MAIN, 918);
					player->ADD_GOSSIP_ITEM(1, "<-Back", GOSSIP_SENDER_MAIN, 300);
					player->PlayerTalkClass->SendGossipMenu(100012, creature->GetGUID());
					break;

					case 100:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_POWER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;


					case 1743:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_POWER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
                case 102:
					item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
					if (!item)
						break;

					if (item->GetTemplate()->InventoryType == INVTYPE_2HWEAPON)
					{
						Enchant(player, item, ENCHANT_WEP_AGILITY);
						player->PlayerTalkClass->SendCloseGossip();
						break;

					}
					ChatHandler(player->GetSession()).PSendSysMessage("You do not have a 2H weapon equipped!");
					break;
					
                case 103:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_HURRICANE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;


				case 1233:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_HURRICANE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
                case 104:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_PYRIUM);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

			   case 1238:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_PYRIUM);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
                case 105:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_MAJOR_LAND);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;


				case 1333:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_MAJOR_LAND);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
                case 106:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_AVALANCH);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;


				case 1789:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_AVALANCH);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
                case 107:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_HEARTSON);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

			    case 1341:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_HEARTSON);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

                case 108:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_MENDING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;


				case 1084:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_MENDING);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;


				case 109:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_WINDWALK);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;


				case 1094:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_WINDWALK);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

					
                case 914:
                    Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BLADE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 119:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_WRIST_SPELL_INTELLECT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 120:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_WRIST_SPIRIT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 121:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_WRIST_STRENGTH);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 122:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_WRIST_AGILITY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 900:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_WRIST_SPEED);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 901:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_WRIST_EXPERTISE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 902:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_WRIST_PRECISION);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 903:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_WRIST_CRIT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

				case 904:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS), ENCHANT_WRIST_DODGE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 110:
					item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
					if (!item)
						break;

					if (item->GetTemplate()->InventoryType == INVTYPE_SHIELD)
					{
						Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_ARMOR);
						player->PlayerTalkClass->SendCloseGossip();
						break;
					}


				case 946:
					item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
					if (!item)
						break;

					if (item->GetTemplate()->InventoryType == INVTYPE_SHIELD)
					{
						Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_INT);
						player->PlayerTalkClass->SendCloseGossip();
						break;
					}

					
				case 111:
					item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
					if (!item)
						break;

					if (item->GetTemplate()->InventoryType == INVTYPE_SHIELD)
					{
						Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_RESILIENCE);
						player->PlayerTalkClass->SendCloseGossip();
						break;
					}
					
				case 915:
					item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
					if (!item)
						break;

					if (item->GetTemplate()->InventoryType == INVTYPE_SHIELD)
					{
						Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_MASTERY);
						player->PlayerTalkClass->SendCloseGossip();
						break;
					}



					case 543:
					item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
					if (!item)
						break;

					if (item->GetTemplate()->InventoryType == INVTYPE_HOLDABLE)
					{
						Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_WEP_INTELLECT);
						player->PlayerTalkClass->SendCloseGossip();
						break;
					}
					
				case 916:
					item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
					if (!item)
						break;

					if (item->GetTemplate()->InventoryType == INVTYPE_SHIELD)
					{
						Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND), ENCHANT_SHIELD_STAM);
						player->PlayerTalkClass->SendCloseGossip();
						break;
					}
				case 142:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_INTELLECT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 114:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_STAMINA);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 115:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_STRIKE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 136:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_HASTE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 139:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SPELL_PEN);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 137:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_DEF);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 138:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK), ENCHANT_CLOAK_SPIRIT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 116:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_HEALTH);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 117:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_RESILIENCE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 118:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_STATS);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 451:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST), ENCHANT_CHEST_SPIRIT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 130:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_AGISPEED);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 131:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_HIT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 132:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_MASPEED);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 133:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_MASTERY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 134:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_STAMSPEED);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 917:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_AGI);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 918:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET), ENCHANT_BOOTS_HASTE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 123:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_AGILITY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 124:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_ATTACK);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 125:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_HIT);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 126:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_HASTE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 127:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_SPELLPOWER);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 128:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_EXPERTISE);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 129:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_MASTERY);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
					
				case 141:
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS), ENCHANT_GLOVES_STRENGTH);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;
				case 300: //<-Back menu
					{
						player->ADD_GOSSIP_ITEM(1, "Enchant Weapon", GOSSIP_SENDER_MAIN, 1);
						player->ADD_GOSSIP_ITEM(1, "Enchant Off-Hand Weapon", GOSSIP_SENDER_MAIN, 13);
						player->ADD_GOSSIP_ITEM(1, "Enchant 2H Weapon", GOSSIP_SENDER_MAIN, 2);
						player->ADD_GOSSIP_ITEM(1, "Enchant Shield", GOSSIP_SENDER_MAIN, 3);
						player->ADD_GOSSIP_ITEM(1, "Enchant Head", GOSSIP_SENDER_MAIN, 4);
						player->ADD_GOSSIP_ITEM(1, "Enchant Shoulders", GOSSIP_SENDER_MAIN, 5);
						player->ADD_GOSSIP_ITEM(1, "Enchant Cloak", GOSSIP_SENDER_MAIN, 6);
						player->ADD_GOSSIP_ITEM(1, "Enchant Chest", GOSSIP_SENDER_MAIN, 7);
						player->ADD_GOSSIP_ITEM(1, "Enchant Bracers", GOSSIP_SENDER_MAIN, 8);
						player->ADD_GOSSIP_ITEM(1, "Enchant Gloves", GOSSIP_SENDER_MAIN, 9);
						player->ADD_GOSSIP_ITEM(1, "Enchant Legs", GOSSIP_SENDER_MAIN, 10);
						player->ADD_GOSSIP_ITEM(1, "Enchant Feet", GOSSIP_SENDER_MAIN, 11);

						if (player->HasSkill(SKILL_ENCHANTING) && player->GetSkillValue(SKILL_ENCHANTING) == 450)
						{
							player->ADD_GOSSIP_ITEM(1, "Enchant Rings", GOSSIP_SENDER_MAIN, 12);
						}


						player->PlayerTalkClass->SendGossipMenu(100001, creature->GetGUID());
						return true;
					}
						break;

				case 400: //Remove enchant for mainhand
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 401: //Remove enchant for offhand
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 402: //Remove enchant for head
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 403: //Remove enchant for shoulders
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 404: //remove enchant for cloak
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 405: //remove enchant for chest
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 406: //remove enchant for bracers
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 407: //remove enchant for gloves
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 408: //remove enchant for legs
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 409: //remove enchant for feet
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 410:
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER1));
					RemoveEnchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FINGER2));
					player->PlayerTalkClass->SendCloseGossip();
					break;

				case 5000: // Bloody Dancing Steel
					Enchant(player, player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND), ENCHANT_WEP_BLOODY_DANCING_STEEL);
                    player->PlayerTalkClass->SendCloseGossip();
                    break;

			}
			
			return true;
        }
	

};

void AddSC_npc_enchantment()
{
  new npc_enchantment();
}