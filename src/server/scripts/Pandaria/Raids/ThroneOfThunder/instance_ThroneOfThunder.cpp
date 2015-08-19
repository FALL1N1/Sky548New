#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "ThroneOfThunder.h"

class instance_ThroneOfThunder : public InstanceMapScript
{
    public:
        instance_ThroneOfThunder() : InstanceMapScript("instance_ThroneOfThunder", 1098) { }

        struct instance_ThroneOfThunder_InstanceMapScript : public InstanceScript
        {
            instance_ThroneOfThunder_InstanceMapScript(Map* map) : InstanceScript(map)
            {
                //SetHeaders(DataHeader);
                SetBossNumber(EncounterCount);

            }

            void OnCreatureCreate(Creature* creature) override
            {
                switch (creature->GetEntry())
                {
                    case BOSS_Jinrokh:
                    case BOSS_Horridon:
                    case BOSS_Council:
                    case BOSS_Tortos:
                    case BOSS_Megaera:
                    case BOSS_JiKun:
                    case BOSS_Durumu:
                    case BOSS_Primordius:
                    case BOSS_DarkAnimus:
                    case BOSS_IronQon:
                    case BOSS_TwinConsorts:
                    case BOSS_LeiShen:
                    case BOSS_Raden:
                        break;
                    default:
                        break;
                }
            }

            bool SetBossState(uint32 type, EncounterState state) override
            {
                if (!InstanceScript::SetBossState(type, state))
                    return false;

                if (state != DONE)
                   return true;

                switch (type)
                {
                    case DATA_Jinrokh:
                    case DATA_Horridon:
                    case DATA_Council:
                    case DATA_Tortos:
                    case DATA_Megaera:
                    case DATA_JiKun:
                    case DATA_Durumu:
                    case DATA_Primordius:
                    case DATA_DarkAnimus:
                    case DATA_IronQon:
                    case DATA_TwinConsorts:
                    case DATA_LeiShen:
                    case DATA_Raden:
                        break;
                    default:
                        return true;
                }

                return true;
            }


        };

        InstanceScript* GetInstanceScript(InstanceMap* map) const override
        {
            return new instance_ThroneOfThunder_InstanceMapScript(map);
        }
};

void AddSC_instance_ThroneOfThunder()
{
    new instance_ThroneOfThunder();
}
