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

 /*
 * Scriptnames of files in this file should be prefixed with "npc_pet_pala_".
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Pet.h"

enum GuardianSpellsAndEntries
{
    NPC_PROTECTION_GUARDIAN         = 46490,
    NPC_HOLY_GUARDIAN               = 46499,
    NPC_RETRI_GUARDIAN              = 46506,
    SPELL_ANCIENT_GUARDIAN_VISUAL   = 86657,
    SPELL_ANCIENT_HEALER            = 86674
};

class npc_pet_pala_guardian_of_ancient_kings : public CreatureScript
{
    public:
        npc_pet_pala_guardian_of_ancient_kings() : CreatureScript("npc_pet_pala_guardian_of_ancient_kings") { }

        struct npc_pet_pala_guardian_of_ancient_kingsAI : public ScriptedAI
        {
            npc_pet_pala_guardian_of_ancient_kingsAI(Creature *creature) : ScriptedAI(creature) {}

            void Reset()
            {
                if (me->GetEntry() == NPC_RETRI_GUARDIAN || me->GetEntry() == NPC_HOLY_GUARDIAN)
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE);
                else if (me->GetEntry() == NPC_PROTECTION_GUARDIAN)
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_PC | UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE);

                if (me->GetEntry() == NPC_RETRI_GUARDIAN)
                    me->SetReactState(REACT_DEFENSIVE);
                else
                    me->SetReactState(REACT_PASSIVE);

                if (me->GetEntry() == NPC_PROTECTION_GUARDIAN)
                {
                    if (me->GetOwner())
                        DoCast(me->GetOwner(), SPELL_ANCIENT_GUARDIAN_VISUAL);
                }
                else if (me->GetEntry() == NPC_RETRI_GUARDIAN)
                {
                    if (me->GetOwner())
                    {
                        if (me->GetOwner()->GetVictim())
                            AttackStart(me->GetOwner()->GetVictim());

                        DoCast(me, 86703, true);
                    }
                }
                else if (me->GetEntry() == NPC_HOLY_GUARDIAN)
                    if (me->GetOwner())
                        me->GetOwner()->CastSpell(me->GetOwner(), SPELL_ANCIENT_HEALER, true);
            }

            void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (me->GetOwner())
                    if (Unit* newVictim = me->GetOwner()->GetVictim())
                        if (me->GetVictim() != newVictim)
                            AttackStart(newVictim);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pet_pala_guardian_of_ancient_kingsAI(creature);
        }
};

void AddSC_paladin_pet_scripts()
{
    new npc_pet_pala_guardian_of_ancient_kings();
}