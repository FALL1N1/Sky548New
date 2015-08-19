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
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "npc_pet_pri_".
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "PassiveAI.h"
#include "PetAI.h"

enum PriestSpells
{
    SPELL_PRIEST_GLYPH_OF_SHADOWFIEND       = 58228,
    SPELL_PRIEST_GLYPH_OF_SHADOWFIEND_MANA  = 58227,
    SPELL_PRIEST_LIGHTWELL_CHARGES          = 59907
};

class npc_pet_pri_lightwell : public CreatureScript
{
    public:
        npc_pet_pri_lightwell() : CreatureScript("npc_pet_pri_lightwell") { }

        struct npc_pet_pri_lightwellAI : public PassiveAI
        {
            npc_pet_pri_lightwellAI(Creature* creature) : PassiveAI(creature)
            {
                DoCast(me, SPELL_PRIEST_LIGHTWELL_CHARGES, false);
            }

            void EnterEvadeMode()
            {
                if (!me->IsAlive())
                    return;

                me->DeleteThreatList();
                me->CombatStop(true);
                me->ResetPlayerDamageReq();
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pet_pri_lightwellAI(creature);
        }
};

class npc_pet_pri_shadowfiend : public CreatureScript
{
    public:
        npc_pet_pri_shadowfiend() : CreatureScript("npc_pet_pri_shadowfiend") { }

        struct npc_pet_pri_shadowfiendAI : public PetAI
        {
            npc_pet_pri_shadowfiendAI(Creature* creature) : PetAI(creature) { }

            void JustDied(Unit* /*killer*/)
            {
                if (me->IsSummon())
                    if (Unit* owner = me->ToTempSummon()->GetSummoner())
                        if (owner->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOWFIEND))
                            owner->CastSpell(owner, SPELL_PRIEST_GLYPH_OF_SHADOWFIEND_MANA, true);
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pet_pri_shadowfiendAI(creature);
        }
};

enum PsyfiendSpells
{
    SPELL_PSYCHIC_TERROR    = 113792,
    SPELL_ROOT_FOR_EVER     = 31366
};

//class npc_pet_pri_psyfiend : public CreatureScript
//{
//    public:
//        npc_pet_pri_psyfiend() : CreatureScript("npc_pet_pri_psyfiend") { }
//
//        struct npc_pet_pri_psyfiendAI : public ScriptedAI
//        {
//            npc_pet_pri_psyfiendAI(Creature* c) : ScriptedAI(c)
//            {
//                me->SetReactState(REACT_DEFENSIVE);
//                targetGUID = 0;
//            }
//
//            uint64 targetGUID;
//            uint32 psychicHorrorTimer;
//
//            void Reset()
//            {
//                if (!me->HasAura(SPELL_ROOT_FOR_EVER))
//                    me->AddAura(SPELL_ROOT_FOR_EVER, me);
//
//                psychicHorrorTimer = 1500;
//            }
//
//            void SetGUID(uint64 guid, int32)
//            {
//                targetGUID = guid;
//            }
//
//            void IsSummonedBy(Unit* owner)
//            {
//                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
//                {
//                    me->SetLevel(owner->getLevel());
//                    me->SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, 0.0f);
//                    me->SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, 0.0f);
//                    me->AddAura(SPELL_ROOT_FOR_EVER, me);
//                }
//                else
//                    me->DespawnOrUnsummon();
//            }
//
//			void UpdateAI(uint32 diff)
//            {
//                if (psychicHorrorTimer)
//                {
//                    if (psychicHorrorTimer <= diff)
//                    {
//                            std::list<Unit*> targets;
//                            Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(me, me, 20);
//                            Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(me, targets, u_check);
//                            me->VisitNearbyObject(20, searcher);
//                            for (std::list<Unit*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
//                            {
//                                if (!me->HasAuraWithMechanic(MECHANIC_FEAR) &&
//                                !me->HasAuraWithMechanic(MECHANIC_STUN) &&
//                                !me->HasAuraWithMechanic(MECHANIC_SILENCE) &&
//                                !(*iter)->IsTotem() &&
//                                !(*iter)->HasAura(SPELL_PSYCHIC_TERROR) &&
//                                !(*iter)->HasAuraType(SPELL_AURA_MOD_STEALTH) &&
//                                !(*iter)->HasAura(76577) &&
//                                (*iter)->IsWithinLOSInMap(me))
//                                me->CastSpell((*iter), SPELL_PSYCHIC_TERROR, true);
//                                me->Attack((*iter), false);
//                            }
//
//                        psychicHorrorTimer = 1500;
//                    }
//                    else
//                        psychicHorrorTimer -= diff;
//                }
//            }
//        };
//
//        CreatureAI* GetAI(Creature *creature) const
//        {
//            return new npc_pet_pri_psyfiendAI(creature);
//        }
//};

class npc_pet_pri_shadowy_apparition : public CreatureScript
{
    public:
        npc_pet_pri_shadowy_apparition() : CreatureScript("npc_pet_pri_shadowy_apparition") { }

        CreatureAI *GetAI(Creature* pCreature) const
        {
            return new npc_pet_pri_shadowy_apparitionAI(pCreature);
        }

        struct npc_pet_pri_shadowy_apparitionAI : public ScriptedAI
        {
            npc_pet_pri_shadowy_apparitionAI(Creature* pCreature) : ScriptedAI(pCreature)
            {
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
                me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
                me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
                me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
                me->SetSpeed(MOVE_RUN, 0.3f);
            }

            bool bCast;

            void Reset()
            {
                bCast = false;
            }

			void UpdateAI(uint32 diff)
            {
                if (!UpdateVictim())
                    return;

                if (!me->GetVictim() || me->GetVictim()->isDead())
                    me->DespawnOrUnsummon();

                if (!bCast && me->GetDistance(me->GetVictim()) < 1.0f)
                {
                    bCast = true;
                    me->CastSpell(me->GetVictim(), 87532, true, NULL, NULL, (me->GetOwner() ? me->GetOwner()->GetGUID() : 0));
                    me->DespawnOrUnsummon();
                }
            }
        };
};

class npc_pet_pri_divine_star_effect : public CreatureScript
{
    public:
        npc_pet_pri_divine_star_effect() : CreatureScript("npc_pet_pri_divine_star_effect") { }

        struct npc_pet_pri_divine_star_effectAI : public ScriptedAI
        {
            npc_pet_pri_divine_star_effectAI(Creature* creature) : ScriptedAI(creature)
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                effectTimer = 0;
                effectCount = 0;
                secondcycle = false;
                shadow = false;
            }

            uint32 effectTimer;
            uint32 effectCount;
            bool secondcycle;
            bool shadow;

            void IsSummonedBy(Unit* owner)
            {
                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                {
                    if (owner->HasAura(15473))
                        shadow = true;

                    me->SetReactState(REACT_PASSIVE);
                    me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                    me->SetSpeed(MOVE_RUN, 3.25f);

                    Position pos;
                    me->GetPosition(&pos);
                    float dist = 25.0f;
                    pos.m_positionX = pos.GetPositionX() + (dist * cos(pos.GetOrientation()));
                    pos.m_positionY = pos.GetPositionY() + (dist * sin(pos.GetOrientation()));
                    
                    effectTimer = 250;
                    me->GetMotionMaster()->MovePoint(0, pos.GetPositionX(), pos.GetPositionY(), pos.GetPositionZ());
                }
                else
                    me->DespawnOrUnsummon();
            }

			void UpdateAI(uint32 diff)
            {
                Unit* owner = me->GetOwner();

                if (!owner)
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (effectCount >= 4 && secondcycle) // this is used to give each cycle exactly max 4 procs
                {
                    me->DespawnOrUnsummon();
                    return;
                }

                if (!me->isMoving() && !secondcycle)
                {
                    me->SetInFront(owner);
                    me->CastSpell(me, shadow ? 122127 : 58880, true);
                    me->GetMotionMaster()->MovePoint(0, owner->GetPositionX(), owner->GetPositionY(), owner->GetPositionZ());
                    effectCount = 0;
                    secondcycle = true;
                    effectTimer = 250;
                    return;
                }

                if (effectTimer <= diff && effectCount < 4)
                {
					me->GetOwner()->CastSpell(me, shadow ? 122128 : 110745, true, NULL, NULL, me->GetOwnerGUID());
                    effectTimer = 250;
                    me->CastSpell(me, 124556, true);
                    ++effectCount;
                }
                effectTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pet_pri_divine_star_effectAI(creature);
        }
};

void AddSC_priest_pet_scripts()
{
    new npc_pet_pri_lightwell();
    new npc_pet_pri_shadowfiend();
	//new npc_pet_pri_psyfiend();
	new npc_pet_pri_shadowy_apparition();
    new npc_pet_pri_divine_star_effect();
}
