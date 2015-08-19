#include "ScriptMgr.h"

class player_scripts : public PlayerScript
{
public:
    player_scripts() : PlayerScript("player_scripts") {}

    void QuestAddingCheck(Player* player, uint32 id, bool forcecomplete)
    {
        Quest const* quest = sObjectMgr->GetQuestTemplate(id);
        if (player->GetQuestStatus(id) == QUEST_STATUS_NONE && player->CanTakeQuest(quest, false))
        {
            player->AddQuest(quest, NULL);

            if (forcecomplete)
                player->CompleteQuest(id);
        }
    }

    void OnLogin(Player* player)
    {
        // Pandarens
        if (player->getRace() == RACE_PANDAREN_NEUTRAL && player->FindNearestCreature(53566, 10.0f))
        {
            switch (player->getClass())
            {
                case CLASS_MONK:
                    QuestAddingCheck(player, 30039, true);
                    break;
                case CLASS_MAGE:
                    QuestAddingCheck(player, 30040, true);
                    break;
                case CLASS_HUNTER:
                    QuestAddingCheck(player, 30041, true);
                    break;
                case CLASS_PRIEST:
                    QuestAddingCheck(player, 30042, true);
                    break;
                case CLASS_ROGUE:
                    QuestAddingCheck(player, 30043, true);
                    break;
                case CLASS_SHAMAN:
                    QuestAddingCheck(player, 30044, true);
                    break;
                case CLASS_WARRIOR:
                    QuestAddingCheck(player, 30045, true);
                    break;
                default:
                    break;
            }
        }
    }
};

void AddSC_player_scripts()
{
    new player_scripts();
}