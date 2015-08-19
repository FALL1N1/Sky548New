#include "ScriptPCH.h"

enum AutoLearnSkills
{
    AUTO_LEARN_FIRST_AID,

    AUTO_LEARN_MAX
};

struct AutoLearnSkillInfo
{
    const uint32 spellId;
    const uint32 skillId;
    const uint32 trainerId;

    AutoLearnSkillInfo(const uint32 _spellId, const uint32 _skillId, const uint32 _trainerId) :
        spellId(_spellId), skillId(_skillId), trainerId(_trainerId) { }
};

const AutoLearnSkillInfo AutoLearnSkillList[AUTO_LEARN_MAX] =
{
    // First Aid
    AutoLearnSkillInfo(110406, SKILL_FIRST_AID, 59077)
};

class player_loginscript : public PlayerScript
{
private:
    std::set<uint32> m_glyphIds;

public:
    player_loginscript() : PlayerScript("player_loginscript")
    {
        // Extract all the glyphs from the ItemTemplate store.
        ExtractAllGlyphs();
    }

    void OnLogin(Player* player)
    {
        if (!player)
            return;

        // Silence all achievement announcements
        player->SilenceAchievementAnnounce(true);

        // Learn dual specialization
        LearnDualSpec(player);

        // Learn all glyphs
        LearnAllGlyphs(player);

        // Auto learn skills
        for (uint8 i = AUTO_LEARN_FIRST_AID; i < AUTO_LEARN_MAX; ++i)
        {
            AutoLearnSkillInfo const* info = &AutoLearnSkillList[i];
            LearnSkill(player, info->spellId, info->skillId, info->trainerId);
        }

        // Un-silence all achievement announcements
        player->SilenceAchievementAnnounce(false);
    }

    void LearnDualSpec(Player* player)
    {
        if (player->GetSpecsCount() == 1 &&
            player->getLevel() >= sWorld->getIntConfig(CONFIG_MIN_DUALSPEC_LEVEL))
        {
            player->CastSpell(player, 63680, true, NULL, NULL, player->GetGUID());
            player->CastSpell(player, 63624, true, NULL, NULL, player->GetGUID());
        }
    }

    void LearnAllGlyphs(Player* player)
    {
        for (std::set<uint32>::const_iterator itr = m_glyphIds.begin(); itr != m_glyphIds.end(); ++itr)
            if (ItemTemplate const* itemTemplate = sObjectMgr->GetItemTemplate(*itr))
                if (itemTemplate->AllowableClass & player->getClassMask())
                    for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                    {
                        uint32 spellId = itemTemplate->Spells[i].SpellId;
                        if (IsValidSpellForPlayer(player, spellId))
                            player->learnSpell(spellId, false);
                    }
    }

    bool IsValidSpellForPlayer(Player* player, uint32 spellId)
    {
        if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId))
            return SpellMgr::IsSpellValid(spellInfo, player, false) && !player->HasSpell(spellId);

        return false;
    }

    void ExtractAllGlyphs()
    {
        for (ItemTemplateContainer::const_iterator itr = sObjectMgr->GetItemTemplateStore()->begin();
             itr!=sObjectMgr->GetItemTemplateStore()->end(); ++itr)
            if (ItemTemplate const* itemTemplate = &itr->second)
                if (itemTemplate->Class == ITEM_CLASS_GLYPH)
                    m_glyphIds.insert(itemTemplate->ItemId);
    }

    void LearnSkill(Player* player, uint32 spellId, uint32 skillId, uint32 trainerId)
    {
        player->learnSpellHighRank(spellId);

        uint16 maxLevel = player->GetPureMaxSkillValue(skillId);
        player->SetSkill(skillId, player->GetSkillStep(skillId), maxLevel, maxLevel);

        //player->LearnProfessionRecipes(trainerId);
    }
};

void AddSC_player_loginscript()
{
    new player_loginscript;
}