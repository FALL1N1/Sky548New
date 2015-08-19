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
 * Scriptnames of files in this file should be prefixed with "npc_pet_monk_".
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "Pet.h"

enum xuenSpells
{
    CRACKLING_TIGER_LIGHTNING   = 123996,
    PROVOKE                     = 130793,
    TIGER_LEAP                  = 124007,
    TIGER_LUST                  = 124009,
};

enum xuenEvents
{
    EVENT_LIGHTNING = 1,
    EVENT_PROVOKE   = 2,
    EVENT_LEAP      = 3,
};

class npc_pet_monk_xuen_the_white_tiger : public CreatureScript
{
    public:
        npc_pet_monk_xuen_the_white_tiger() : CreatureScript("npc_pet_monk_xuen_the_white_tiger") { }

        struct npc_pet_monk_xuen_the_white_tigerAI : public ScriptedAI
        {
            EventMap events;

            npc_pet_monk_xuen_the_white_tigerAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetReactState(REACT_DEFENSIVE);
            }

            void Reset()
            {
                events.Reset();

                events.ScheduleEvent(EVENT_LIGHTNING, 500);
                events.ScheduleEvent(EVENT_LEAP,      200);
                events.ScheduleEvent(EVENT_PROVOKE,   200);
            }

			void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                {
                    if (me->GetOwner())
                        if (Unit* victim = me->GetOwner()->getAttackerForHelper())
                            AttackStart(victim);

                    return;
                }

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                events.Update(diff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_LIGHTNING:
                        {
                            if (Unit* target = me->GetVictim())
                            {
                                me->CastSpell(target, CRACKLING_TIGER_LIGHTNING, false);
                            }
                            else if (me->GetOwner())
                            {
                                if (Unit* target = me->GetOwner()->getAttackerForHelper())
                                    me->CastSpell(target, CRACKLING_TIGER_LIGHTNING, false);
                            }

                            events.ScheduleEvent(EVENT_LIGHTNING, 6000);
                            break;
                        }
                        case EVENT_LEAP:
                        {
                            if (Unit* target = me->GetVictim())
                            {
                                me->CastSpell(target, TIGER_LEAP, false);
                            }
                            else if (me->GetOwner())
                            {
                                if (Unit* target = me->GetOwner()->getAttackerForHelper())
                                    me->CastSpell(target, TIGER_LEAP, false);
                            }

                            me->CastSpell(me, TIGER_LUST, false);
                            events.ScheduleEvent(EVENT_LEAP, 15000);

                            break;
                        }
                        case EVENT_PROVOKE:
                        {
                            Unit* m_owner = me->GetOwner();
                            if (!m_owner || m_owner->GetTypeId() != TYPEID_PLAYER)
                                break;

                            Player* p_owner = m_owner->ToPlayer();
                            if (p_owner->GetSpecializationId(p_owner->GetActiveSpec()) != CHAR_SPECIALIZATION_MONK_BREWMASTER)
                                break;

                            if (Unit* target = me->GetVictim())
                            {
                                if (target->GetTypeId() == TYPEID_UNIT)
                                    if (target->ToCreature()->IsDungeonBoss() || target->ToCreature()->isWorldBoss())
                                        break;

                                me->CastSpell(target, PROVOKE, false);
                            }
                            else if (me->GetOwner())
                            {
                                if (Unit* target = me->GetOwner()->getAttackerForHelper())
                                {
                                    if (target->GetTypeId() == TYPEID_UNIT)
                                        if (target->ToCreature()->IsDungeonBoss() || target->ToCreature()->isWorldBoss())
                                            break;

                                    me->CastSpell(target, PROVOKE, false);
                                }
                            }

                            events.ScheduleEvent(EVENT_PROVOKE, 15000);
                            break;
                        }
                        default:
                            break;
                    }
                }

                DoMeleeAttackIfReady();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pet_monk_xuen_the_white_tigerAI(creature);
        }
};

void AddSC_monk_pet_scripts()
{
    new npc_pet_monk_xuen_the_white_tiger();
}