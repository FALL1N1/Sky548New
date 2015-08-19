/*
+    © Emiliyan Kurtseliyanski (JunkyBulgaria)
+    Email: emiliyan9604bg@gmail.com
+    Site: www.kurtseliyanski.info
+
*/

#include "ScriptMgr.h"
#include "CreatureAI.h"
#include "Player.h"
#include "Unit.h"
#include "UnitAI.h"
#include "ScriptedCreature.h"
#include "ObjectMgr.h"
#include "timeless_isle.h"

enum Spells
{
	SPELL_AGILITY                   = 144631,
    SPELL_CHI_BARRAGE_TRIGGER       = 144643,
    SPELL_CHI_BARRAGE               = 144642,
    SPELL_CRACKLING_LIGHTNING       = 144635,
    SPELL_LEAP                      = 144640,
    SPELL_SPECTRAL_SWIPE_TRIGGER    = 144652,
    SPELL_SPECTRAL_SWIPE            = 144638,
};

enum Events
{
    EVENT_SPECTRAL_SWIPES           = 0,
    EVENT_CHI_BARRAGE_AOE           = 1,
    EVENT_CRACKLING_LIGHTNING       = 2,
    EVENT_AGILITY_SELF_BUFF         = 3,
    EVENT_LEAP                      = 4,
    EVENT_DEFEATED                  = 5,
};

enum Timers // 1000ms = 1s
{
    TIMER_SPECTRAL_SWIPES           = 1000,
    TIMER_CHI_BARRAGE_AOE           = 1000,
    TIMER_CRACKLING_LIGHTNING       = 1000,
    TIMER_AGILITY_SELF_BUFF         = 1000,
    TIMER_LEAP                      = 1000,
    TIMER_DEFEATED                  = 1000,
};

enum Actions
{
	ACTION_DEFEATED					= 0,
};

enum Says
{
    // unknown for now
};

enum Phases
{
	PHASE_INTRO						= 0,
	PHASE_COMBAT					= 1,
	PHASE_DEFEATED					= 2,
};

class boss_xuen : public CreatureScript
{
    public:
        boss_xuen() : CreatureScript("boss_xuen") { }

        struct boss_xuenAI : public BossAI
        {
            boss_xuenAI(Creature* creature) : BossAI(creature, DATA_CHI_JI) { }

            void CancelAllEvents()
            {
                events.CancelEvent(EVENT_SPECTRAL_SWIPES);
                events.CancelEvent(EVENT_CRACKLING_LIGHTNING);
                events.CancelEvent(EVENT_CHI_BARRAGE_AOE);
                events.CancelEvent(EVENT_LEAP);
            }

            void CancelAllCombatEventsAndReset()
            {
            	me->InterruptNonMeleeSpells(true);

            	events.SetPhase(PHASE_DEFEATED);

                CancelAllEvents();

                me->m_Events.KillAllEvents(true);
            }

            void Reset() override
            {
                _Reset();
                me->setFaction(35);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                events.SetPhase(PHASE_INTRO);
            }

            void EnterCombat(Unit* /*target*/) override
            {
                me->setActive(true);

                events.SetPhase(PHASE_COMBAT);
                events.ScheduleEvent(EVENT_LEAP, TIMER_LEAP, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_CRACKLING_LIGHTNING, TIMER_CRACKLING_LIGHTNING, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_CHI_BARRAGE_AOE, TIMER_CHI_BARRAGE_AOE, 0, PHASE_COMBAT);
                events.ScheduleEvent(EVENT_SPECTRAL_SWIPES, TIMER_SPECTRAL_SWIPES, 0, PHASE_COMBAT);
            }

            void EnterEvadeMode() override
            {
                BossAI::EnterEvadeMode();

                if (Defeated == true)
                {
                	CancelAllCombatEventsAndReset();

                	me->AI()->DoAction(ACTION_DEFEATED);
                }

                else if (Defeated == false)
                	me->Respawn(true);
            }

			void KilledUnit(Unit* victim) override
            {
                //if (victim->GetTypeId() == TYPEID_PLAYER && !me->IsInEvadeMode() && events.IsInPhase(PHASE_COMBAT))
                    //Talk(SAY_KILL);

                if (!me->GetVictim() && !me->IsInEvadeMode())
                {
                	Defeated = false;
                	EnterEvadeMode();
                }
            }

			void DoAction(int32 action) override
            {
                switch (action)
                {
                	case ACTION_DEFEATED:
                        // tekstove i gluposti kogato xuen e pobeden
                		//events.ScheduleEvent(EVENT_DEFEATED_1, 1000, 0, PHASE_DEFEATED);
                		//events.ScheduleEvent(EVENT_DEFEATED_2, 11000, 0, PHASE_DEFEATED);
                		//events.ScheduleEvent(EVENT_DEFEATED_3, 17000, 0, PHASE_DEFEATED);
                		break;
                	default:
                		break;
                }
            }

			void DamageTaken(Unit* /*attacker*/, uint32& damage) override
            {
                if (events.IsInPhase(PHASE_COMBAT) && !HealthAbovePct(3))
                {
                    damage = 0;
                	Defeated = true;

                	EnterEvadeMode();
                	return;
                }
            }

			void UpdateAI(uint32 diff) override
            {
                if (events.IsInPhase(PHASE_COMBAT))
                {
                    if (!UpdateVictim())
                        return;
                }

                events.Update(diff);

                if (me->HasUnitState(UNIT_STATE_CASTING))
                    return;

                if (events.IsInPhase(PHASE_INTRO))
                	return;

				// #todo
                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_SPECTRAL_SWIPES:
                            DoCast(SPELL_SPECTRAL_SWIPE, false); // v konus, single
                            events.ScheduleEvent(EVENT_SPECTRAL_SWIPES, TIMER_SPECTRAL_SWIPES, 0, PHASE_COMBAT); 
                            break;
                        case EVENT_AGILITY_SELF_BUFF:
                            DoCast(me, SPELL_AGILITY, false); // agility buff-a si go slaga na nego
                            events.ScheduleEvent(EVENT_AGILITY_SELF_BUFF, TIMER_AGILITY_SELF_BUFF, 0, PHASE_COMBAT); 
                            break;
                        case EVENT_LEAP:
                            if (Unit *random_target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(random_target, SPELL_LEAP, false); // da skacha (cast leap) na random target
                            events.ScheduleEvent(EVENT_LEAP, TIMER_LEAP, 0, PHASE_COMBAT); 
                            break;
                        case EVENT_CRACKLING_LIGHTNING:
                            if (Unit *random_target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                                DoCast(random_target, SPELL_CRACKLING_LIGHTNING, false); // da skacha (cast leap) na random target
                            events.ScheduleEvent(EVENT_CRACKLING_LIGHTNING, TIMER_CRACKLING_LIGHTNING, 0, PHASE_COMBAT); 
                            break;
                        case EVENT_CHI_BARRAGE_AOE:
                            DoCast(SPELL_CHI_BARRAGE, false); // v konus, single
                            events.ScheduleEvent(EVENT_CHI_BARRAGE_AOE, TIMER_CHI_BARRAGE_AOE, 0, PHASE_COMBAT); 
                        break;
                        case EVENT_DEFEATED:
                            me->Respawn();
                            me->setActive(false);
                            me->setFaction(35);
                            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE_TO_PC);
                            break;
                    	default:
                    		break;
                    }
                }

                DoMeleeAttackIfReady();
            }

        private:
        	bool Defeated;
        };

        CreatureAI* GetAI(Creature* creature) const override
        {
            return new boss_xuenAI(creature);
        }
};

void AddSC_worldboss_xuen()
{
    new boss_chi_ji_ti();
}