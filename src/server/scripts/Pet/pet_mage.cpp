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
 * Scriptnames of files in this file should be prefixed with "npc_pet_mag_".
 */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "CombatAI.h"
#include "Pet.h"

enum MageSpells
{
    SPELL_MAGE_CLONE_ME                 = 45204,
    SPELL_MAGE_MASTERS_THREAT_LIST      = 58838,
    SPELL_MAGE_FROZEN_ORB_VISUAL        = 123605,
    SPELL_MAGE_FROZEN_ORB_PERIODIC_AURA = 84717,
    SPELL_MAGE_FROZEN_ORB_VISUAL_DMG    = 113162,
    SPELL_MAGE_FROZEN_ORB_DMG           = 84721,
    SPELL_MAGE_FROZEN_ORB_SELF_SNARE    = 82736,
    SPELL_MAGE_FINGERS_OF_FROST         = 44544
};

class npc_pet_mage_mirror_image : public CreatureScript
{
public:
    npc_pet_mage_mirror_image() : CreatureScript("npc_pet_mage_mirror_image") { }

    struct npc_pet_mage_mirror_imageAI : CasterAI
    {
        npc_pet_mage_mirror_imageAI(Creature* creature) : CasterAI(creature) { }

        void InitializeAI()
        {
            CasterAI::InitializeAI();
            Unit* owner = me->GetOwner();
            if (!owner)
                return;
            // Inherit Master's Threat List (not yet implemented)
            owner->CastSpell((Unit*) NULL, SPELL_MAGE_MASTERS_THREAT_LIST, true);
            // here mirror image casts on summoner spell (not present in client dbc) 49866
            // here should be auras (not present in client dbc): 35657, 35658, 35659, 35660 selfcasted by mirror images (stats related?)
            // Clone Me!
            owner->CastSpell(me, SPELL_MAGE_CLONE_ME, false);
        }

        // Do not reload Creature templates on evade mode enter - prevent visual lost
        void EnterEvadeMode()
        {
            if (me->IsInEvadeMode() || !me->IsAlive())
                return;

            Unit* owner = me->GetCharmerOrOwner();

            me->CombatStop(true);
            if (owner && !me->HasUnitState(UNIT_STATE_FOLLOW))
            {
                me->GetMotionMaster()->Clear(false);
                me->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, me->GetFollowAngle(), MOTION_SLOT_ACTIVE);
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pet_mage_mirror_imageAI(creature);
    }
};

class npc_pet_mage_ring_of_frost : public CreatureScript
{
    public:
        npc_pet_mage_ring_of_frost() : CreatureScript("npc_pet_mage_ring_of_frost") { }

        struct npc_pet_mage_ring_of_frostAI : public ScriptedAI
        {
            npc_pet_mage_ring_of_frostAI(Creature *c) : ScriptedAI(c)
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void Reset()
            {
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }

            void InitializeAI()
            {
                ScriptedAI::InitializeAI();
                Unit * owner = me->GetOwner();
                if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
                    return;

                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

                std::list<Creature*> templist;
                me->GetCreatureListWithEntryInGrid(templist, me->GetEntry(), 200.0f);
                if (!templist.empty())
                    for (std::list<Creature*>::const_iterator itr = templist.begin(); itr != templist.end(); ++itr)
                        if ((*itr)->GetOwner() == me->GetOwner() && *itr != me)
                            (*itr)->DisappearAndDie();
            }

            void CheckIfMoveInRing(Unit *who)
            {
                if (who->IsAlive() && me->IsInRange(who, 2.0f, 4.7f) && me->IsWithinLOSInMap(who))
                {
                    if (!who->HasAura(82691))
                    {
                        if (!who->HasAura(91264))
                        {
                            me->CastSpell(who, 82691, true);
                            me->CastSpell(who, 91264, true);
                        }
                    }
                    else me->CastSpell(who, 91264, true);
                }
            }

			void UpdateAI(uint32 diff)
            {
                std::list<Unit*> targets;
                Trinity::AnyUnfriendlyUnitInObjectRangeCheck u_check(me, me, 5.0f);
                Trinity::UnitListSearcher<Trinity::AnyUnfriendlyUnitInObjectRangeCheck> searcher(me, targets, u_check);
                me->VisitNearbyObject(5.0f, searcher);
                for (std::list<Unit*>::const_iterator iter = targets.begin(); iter != targets.end(); ++iter)
                    if (!(*iter)->IsTotem())
                        CheckIfMoveInRing(*iter);
            }
        };

        CreatureAI* GetAI(Creature* pCreature) const
        {
            return new npc_pet_mage_ring_of_frostAI(pCreature);
        }
};

enum frozenOrbSpells
{
    SPELL_FINGERS_OF_FROST_VISUAL   = 44544,
    SPELL_SELF_SNARE_90             = 82736,
    SPELL_SNARE_DAMAGE              = 84721,
    SPELL_FINGERS_OF_FROST          = 126084
};

class npc_pet_mage_frozen_orb : public CreatureScript
{
    public:
        npc_pet_mage_frozen_orb() : CreatureScript("npc_pet_mage_frozen_orb") { }

        struct npc_pet_mage_frozen_orbAI : public ScriptedAI
        {
            npc_pet_mage_frozen_orbAI(Creature* creature) : ScriptedAI(creature)
            {
                //x = me->GetPositionX();
                //y = me->GetPositionY();
                //z = me->GetOwner()->GetPositionZ()+2;
                //o = me->GetOrientation();
                //dist = 30;
                //me->NearTeleportTo(x, y, z, o, true);
                //angle = me->GetOwner()->GetAngle(me);
                //newx = me->GetPositionX() + dist * cos(angle);
                //newy = me->GetPositionY() + dist * sin(angle);
                //CombatCheck = false;
                frozenOrbTimer = 0;
            }


            //float x,y,z,o,newx,newy,angle;
            //bool CombatCheck;
            uint32 DespawnTimer;
            //uint32 DespawnCheckTimer;
            uint32 frozenOrbTimer;
            //uint32 dist;

            void IsSummonedBy(Unit* owner)
            {
                if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                {
                    owner->CastSpell(me, SPELL_SNARE_DAMAGE, true);
                    owner->CastSpell(owner, SPELL_FINGERS_OF_FROST_VISUAL, true);
                    owner->CastSpell(owner, SPELL_FINGERS_OF_FROST, true);
                    me->AddAura(SPELL_SELF_SNARE_90, me);

                    frozenOrbTimer = 1000;
                }
                else
                    me->DespawnOrUnsummon();
            }

            /*void EnterCombat(Unit* /*target/)
            {
                if (me->IsWithinDist2d(newx,newy, 30.0f))
                {
                me->GetMotionMaster()->MoveCharge(newx, newy, z, 0.2f);
                }
                else dist = 15;
                DespawnTimer = 10 * IN_MILLISECONDS;

            }*/

            void Reset()
            {
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE|UNIT_FLAG_NON_ATTACKABLE);
                me->AddUnitMovementFlag(MOVEMENTFLAG_FLYING);
                me->SetReactState(REACT_PASSIVE);
                DespawnTimer = 10 * IN_MILLISECONDS;
                //me->GetMotionMaster()->MovePoint(0, newx, newy, z);
            }

			void UpdateAI(uint32 diff)
            {
                Unit* owner = me->GetOwner();

                if (!owner)
                    return;

                if (frozenOrbTimer <= diff)
                {
                    if (owner && owner->ToPlayer())
                        if (owner->ToPlayer()->HasSpellCooldown(SPELL_SNARE_DAMAGE))
                            owner->ToPlayer()->RemoveSpellCooldown(SPELL_SNARE_DAMAGE);

                    owner->CastSpell(me, SPELL_SNARE_DAMAGE, true);
                    frozenOrbTimer = 1000;
                }
                else
                    frozenOrbTimer -= diff;
            }
        };

        CreatureAI* GetAI(Creature* creature) const
        {
            return new npc_pet_mage_frozen_orbAI(creature);
        }
};

void AddSC_mage_pet_scripts()
{
    new npc_pet_mage_mirror_image();
    new npc_pet_mage_ring_of_frost();
    new npc_pet_mage_frozen_orb();
}