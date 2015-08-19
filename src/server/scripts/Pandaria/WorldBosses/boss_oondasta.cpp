/*
    © Emiliyan Kurtseliyanski (JunkyBulgaria)
    Email: emiliyan9604bg@gmail.com
    Site: www.kurtseliyanski.info

	TACTIC:

1. Tank 1 attacks and gains  Alpha Male. 

2. Tank 1 positions boss to face AWAY from the raid. While Oondasta is NOT TAUNTABLE, whichever tank has the  
Alpha Male buff has control of where the boss is facing, 
including NPC tanking pets. Oondasta should be placed with ample room around his hind side to allow all melee and ranged space to spread out.

3. Non-tanks, melee, and ranged must be behind the boss spread out 10 yards apart to avoid  Spiritfire Beam. This chain-lightning 
on crack will destroy you swiftly.

4. All members must stop casting when they see the boss casting  Piercing Roar, otherwise they will be silenced.

5. Anything within the boss' frontal 180 degree cone can and will be hit with  Frill Blast. This includes anyone and anything directly 
to the boss' sides. Stay behind the boss at all times.

6. Tank 1 gets  Crush debuff, rendering Tank 1 ineffective. Tank 1 stops all attacks to shed threat. 

7. Tank 2 continues attacking and ramps up all threat possible to gain  Alpha Male. Tank 2 now has control of where Oondasta faces.

8. Tank 2 will eventually get the  Crush debuff and must enact step 6, swapping the boss back to Tank 1.

Rinse and repeat.
*/
#include "SpellScript.h"
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"

/*
    todo: Correct timings??
*/

enum Bosove
{
    BOSS_OONDASTA,
};

enum Magii
{
    SPELL_CRUSH                 = 137504,
    SPELL_FRILL_BLAST           = 137505,
    SPELL_GROWING_FURY          = 137502,
    SPELL_PIERCING_ROAR         = 137458,
    SPELL_SPIRITFIRE_BEAM_1     = 137511,
    SPELL_SPIRITFIRE_BEAM_2     = 137508,
    SPELL_ALPHA_MALE            = 138931,
};

enum Eventi
{
    EVENT_CRUSH                 = 0,
    EVENT_ALPHA_MALE            = 1,
    EVENT_FRILL_BLAST           = 2,
    EVENT_GROWING_FURY          = 3,
    EVENT_PIERCING_ROAR         = 4,
    EVENT_SPIRITFIRE_BEAM       = 5,
};

enum Taimeri // v milisekundi (1000milisekundi = 1sekunda)
{
	TIMER_ALPHA_MALE = 1000, // moje da ne e blizzlike (prez 1s proverqva dali main tanka ima aurata), sluji za tank swap fazata
	TIMER_CRUSH = 15000, // ???
	TIMER_FRILL_BLAST = 19000, // ???
	TIMER_GROWING_FURY = 30000, // 30 sek
	TIMER_PIERCING_ROAR = 30000, // ???
	TIMER_SPIRITFIRE_BEAM = 45000, // ??? 
};

class boss_oondasta : public CreatureScript
{
public:
    boss_oondasta() : CreatureScript("boss_oondasta") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_oondasta_AI(creature);
    }

    struct boss_oondasta_AI : public BossAI
    {
        boss_oondasta_AI(Creature* creature) : BossAI(creature, BOSS_OONDASTA) { }

        void Reset()
        {
            events.Reset();
            _Reset();
        }

        void KilledUnit(Unit* u) { /* za sega nqma da trqbva */ }

        void EnterCombat(Unit* unit)
        {
        	events.ScheduleEvent(EVENT_ALPHA_MALE,       TIMER_ALPHA_MALE); // da proverqva vsqka sekunda dali tanka ima aurata i ako q nqma da q sloji
            events.ScheduleEvent(EVENT_CRUSH,            TIMER_CRUSH); // nikakva ideq kolko e
            events.ScheduleEvent(EVENT_FRILL_BLAST,      TIMER_FRILL_BLAST); // nikakva ideq kolko e
            events.ScheduleEvent(EVENT_PIERCING_ROAR,    TIMER_PIERCING_ROAR); // nikakva ideq kolko e
            events.ScheduleEvent(EVENT_GROWING_FURY, 	 TIMER_GROWING_FURY); // 30 sekundi (ot sniffera i klipove)
        }

        void EligibleForAlphaMale()
        {
           if(Unit* tanka = me->GetVictim())
           {
                if(!tanka->HasAura(SPELL_ALPHA_MALE)) // tui kato swap-vat aggroto moje da izgubqt aurata (explit)
        			DoCastVictim(SPELL_ALPHA_MALE); // castni alpha male v-u main tanka

        		if(tanka->HasAura(SPELL_CRUSH) && tanka->HasAura(SPELL_ALPHA_MALE)) // tank switch, mahane na aura tui kato ima crush-a i e "slab", ne e "alfa mujkara"
        			tanka->RemoveAura(SPELL_ALPHA_MALE);
        	} else return;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
                switch (eventId)
                {
                	case EVENT_ALPHA_MALE:
                	{
                		// tva moje bi da trqbva da se prenapishe
                		EligibleForAlphaMale();
        				events.ScheduleEvent(EVENT_ALPHA_MALE, TIMER_ALPHA_MALE);
        				break;
                	}
                    case EVENT_CRUSH:
                    {
                        me->CastSpell(me, SPELL_CRUSH, true);
                        events.ScheduleEvent(EVENT_CRUSH, TIMER_CRUSH);
                        break;
                    }
                    case EVENT_FRILL_BLAST:
                    {
                        me->CastSpell(me, SPELL_FRILL_BLAST, true);
                        events.ScheduleEvent(EVENT_PIERCING_ROAR, TIMER_FRILL_BLAST);
                        break;
                    }
                    case EVENT_PIERCING_ROAR:
                    {
                        me->CastSpell(me, SPELL_PIERCING_ROAR, true);
                        events.ScheduleEvent(EVENT_PIERCING_ROAR, TIMER_PIERCING_ROAR);
                        break;
                    }
                    case EVENT_GROWING_FURY: // castva go prez 30 sek, 10% damage buff
                    {
                        me->CastSpell(me, SPELL_GROWING_FURY, false);
						events.ScheduleEvent(EVENT_GROWING_FURY, TIMER_GROWING_FURY);
                        break;
                    }
                }
            DoMeleeAttackIfReady();
        }
    };
};

void AddSC_boss_oondasta()
{
	new boss_oondasta();
}
