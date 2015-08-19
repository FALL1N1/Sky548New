#include "ScriptPCH.h"
#include "Chat.h"
#include "InstanceSaveMgr.h"

class world_chat : public CommandScript
{
public:
	world_chat() : CommandScript("world_chat"){}

	ChatCommand * GetCommands() const
	{	
		static ChatCommand WorldCommandTable[] = 
		{
			{"mall",           rbac::RBAC_PERM_COMMAND_MALL,	   false, &HandleMallCommand,			"", NULL},
			{"chat",           rbac::RBAC_PERM_COMMAND_WORLD_CHAT, false, &HandleWorldChatCommand,      "", NULL},
			{NULL, 0, false, NULL, "", NULL}
		};

	return WorldCommandTable;
	}

	static std::string GetTimeString(uint64 time)
	{
		uint64 days = time / DAY, hours = (time % DAY) / HOUR, minute = (time % HOUR) / MINUTE;
		std::ostringstream ss;

		if (days)
			ss << days << "d ";

		if (hours)
			ss << hours << "h ";
		ss << minute << 'm';
		return ss.str();
	}

	static bool HandleMallCommand(ChatHandler * handler, const char * args)
	{
		Player * plr = handler->GetSession()->GetPlayer();
		if (plr->IsInCombat())
		{
			ChatHandler(plr->GetSession()).PSendSysMessage("|cffFF0000[ERROR] You Are In Combat!");
			return true;
		}
		else
		{
			plr->TeleportTo(870, 3878.543213f, 2589.685547f, 757.201599f, 2.045576f);
			return true;
		}
	}
	
	static bool HandleWorldChatCommand(ChatHandler * handler, const char * args)
	{
		if (!handler->GetSession()->GetPlayer()->CanSpeak())
			return false;

		std::string temp = args;

		if (!args || temp.find_first_not_of(' ') == std::string::npos)
			return false;

		std::string msg = "";

		Player * player = handler->GetSession()->GetPlayer();

		switch(player->GetSession()->GetSecurity())
		{
		case SEC_PLAYER: // PLAYER
			if (player->GetTeam() == ALLIANCE)
			{
				msg += "|cff57c606[World]|r|cffe8e4e0|r";
				msg += handler->GetNameLink(player);
				msg += "|cffe8e4e0|r|cff14c7c2:";
			}
			else
			{
				msg += "|cff57c606[World]|r|cffe8e4e0|r";
				msg += handler->GetNameLink(player);
				msg += "|cffe8e4e0|r|cffea66a6:";
			}
			break;
		case SEC_MODERATOR: // MODERATOR
			if(player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GM))
			{
				if (player->GetTeam() == ALLIANCE)
				{
					msg += "|cff57c606[World]|r|TInterface\\ChatFrame\\UI-ChatIcon-Blizz.blp:0:2:0:-3|t|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cff14c7c2:";
				}
				else
				{
					msg += "|cff57c606[World]|r|TInterface\\ChatFrame\\UI-ChatIcon-Blizz.blp:0:2:0:-3|t|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cffea66a6:";
				}
			}
			else
			{
				if (player->GetTeam() == ALLIANCE)
				{
					msg += "|cff57c606[World]|r|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cff14c7c2:";
				}
				else
				{
					msg += "|cff57c606[World]|r|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cffea66a6:";
				}
			}
			break;
		case SEC_GAMEMASTER: // GAMEMASTER
			if(player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GM))
			{
				if (player->GetTeam() == ALLIANCE)
				{
					msg += "|cff57c606[World]|r|TInterface\\ChatFrame\\UI-ChatIcon-Blizz.blp:0:2:0:-3|t|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cff14c7c2:";
				}
				else
				{
					msg += "|cff57c606[World]|r|TInterface\\ChatFrame\\UI-ChatIcon-Blizz.blp:0:2:0:-3|t|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cffea66a6:";
				}
			}
			else
			{
				if (player->GetTeam() == ALLIANCE)
				{
					msg += "|cff57c606[World]|r|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cff14c7c2:";
				}
				else
				{
					msg += "|cff57c606[World]|r|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cffea66a6:";
				}
			}
			break;
		case SEC_ADMINISTRATOR: // ADMINISTARDOR
			if(player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GM))
			{
				if (player->GetTeam() == ALLIANCE)
				{
					msg += "|cff57c606[World]|r|TInterface\\ChatFrame\\UI-ChatIcon-Blizz.blp:0:2:0:-3|t|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cff14c7c2:";
				}
				else
				{
					msg += "|cff57c606[World]|r|TInterface\\ChatFrame\\UI-ChatIcon-Blizz.blp:0:2:0:-3|t|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cffea66a6:";
				}
			}
			else
			{
				if (player->GetTeam() == ALLIANCE)
				{
					msg += "|cff57c606[World]|r|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cff14c7c2:";
				}
				else
				{
					msg += "|cff57c606[World]|r|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cffea66a6:";
				}
			}
			break;
		case SEC_CONSOLE: // Console
			if(player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_GM))
			{
				if (player->GetTeam() == ALLIANCE)
				{
					msg += "|cff57c606[World]|r|TInterface\\ChatFrame\\UI-ChatIcon-Blizz.blp:0:2:0:-3|t|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cff14c7c2:";
				}
				else
				{
					msg += "|cff57c606[World]|r|TInterface\\ChatFrame\\UI-ChatIcon-Blizz.blp:0:2:0:-3|t|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cffea66a6:";
				}
			}
			else
			{
				if (player->GetTeam() == ALLIANCE)
				{
					msg += "|cff57c606[World]|r|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cff14c7c2:";
				}
				else
				{
					msg += "|cff57c606[World]|r|cffe8e4e0|r";
					msg += handler->GetNameLink(player);
					msg += "|cffe8e4e0|r|cffea66a6:";
				}
			}
			break;
		}

		msg += args;
		sWorld->SendServerMessage(SERVER_MSG_STRING, msg.c_str(), 0);

		PreparedStatement * mt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_MUTE_TIME);
		int64 muteTime = time(NULL) + 3;
		player->GetSession()->m_muteTime = muteTime;
		mt->setInt64(0, muteTime);
		return true;
	}
};

void AddSC_world_chat()
{
	new world_chat();
}