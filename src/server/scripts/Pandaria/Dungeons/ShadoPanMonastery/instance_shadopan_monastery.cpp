#include "shadopan_monastery.h"
#include "InstanceScript.h"

Position snowdriftCenterPos = { 3659.08f, 3015.38f, 804.74f };


enum eSpells
{
	SPELL_STEALTH_COSMETIC = 91194,
	SPELL_CRISE = 128248,
	SPELL_ICE_ARROW = 126114,
	SPELL_EXPLOSION_DAMAGE = 106966,
	SPELL_PURIFICATION_RITUAL = 111690,
	SPELL_APPARITIONS_AURA = 112060,

	// Residual Hatred
	SPELL_CURSE_OF_AGONY = 112999,
	SPELL_RING_OF_MALICE = 112932,
	SPELL_SHADOW_BOLT = 112998,

	// Vestige of Hatred
	SPELL_BLACK_CLEAVE = 113020,
	SPELL_DEATH_GRIP = 113021,
	SPELL_ICE_TRAP = 135382,
	SPELL_TOUCH_OF_WEAKNESS = 113022,

	// Fragment of Hatred
	//SPELL_ICE_TRAP              = 135382,
	SPELL_SINISTER_STRIKE = 112931,
	SPELL_VOLLEY_OF_HATRED = 112911,
};


enum eEvents
{
	// Residual Hatred
	EVENT_CURSE_OF_AGONY = 1,
	EVENT_RING_OF_MALICE = 2,
	EVENT_SHADOW_BOLT = 3,

	// Vestige of Hatred
	EVENT_BLACK_CLEAVE = 4,
	EVENT_DEATH_GRIP = 5,
	EVENT_ICE_TRAP = 6,
	EVENT_TOUCH_OF_WEAKNESS = 7,

	// Fragment of Hatred
	//EVENT_ICE_TRAP              = 8,
	EVENT_SINISTER_STRIKE = 9,
	EVENT_VOLLEY_OF_HATRED = 10,
};

std::list<uint64> minibossPositionsGuid;
std::list<uint64> minibossPositionsGuidSave;

std::list<uint64> firstDefeatedNovicePositionsGuid;
std::list<uint64> firstDefeatedNovicePositionsGuidSave;

std::list<uint64> secondDefeatedNovicePositionsGuid;
std::list<uint64> secondDefeatedNovicePositionsGuidSave;

std::list<uint64> firstArcherySet;
std::list<uint64> secondArcherySet;
std::list<uint64> archeryTargetGuids;

class instance_shadopan_monastery : public InstanceMapScript
{
public:
	instance_shadopan_monastery() : InstanceMapScript("instance_shadopan_monastery", 959) { }

	InstanceScript* GetInstanceScript(InstanceMap* map) const
	{
		return new instance_shadopan_monastery_InstanceMapScript(map);
	}

	struct instance_shadopan_monastery_InstanceMapScript : public InstanceScript
	{
		uint8 aliveNoviceCount;
		uint8 aliveMinibossCount;

		uint64 guCloudstikeGuid;
		uint64 masterSnowdriftGuid;
		uint64 shaViolenceGuid;
		uint64 taranZhuGuid;

		uint64 azureSerpentGuid;

		uint64 cloudstikeEntranceGuid;
		uint64 cloudstikeExitGuid;
		uint64 snowdriftEntranceGuid;
		uint64 snowdriftPossessionsGuid;
		uint64 snowdriftFirewallGuid;
		uint64 snowdriftDojoDoorGuid;
		uint64 snowdriftExitGuid;

		uint64 shaEntranceGuid;
		uint64 shaExitGuid;

		

		uint32 dataStorage[MAX_DATA];

		instance_shadopan_monastery_InstanceMapScript(Map* map) : InstanceScript(map)
		{}

		void Initialize()
		{
			SetBossNumber(EncounterCount);

			aliveNoviceCount = MAX_NOVICE;
			aliveMinibossCount = 2;

			guCloudstikeGuid = 0;
			masterSnowdriftGuid = 0;
			shaViolenceGuid = 0;
			taranZhuGuid = 0;

			azureSerpentGuid = 0;

			cloudstikeEntranceGuid = 0;
			cloudstikeExitGuid = 0;
			snowdriftEntranceGuid = 0;
			snowdriftEntranceGuid = 0;
			snowdriftPossessionsGuid = 0;
			snowdriftFirewallGuid = 0;
			snowdriftDojoDoorGuid = 0;
			snowdriftExitGuid = 0;

			shaEntranceGuid = 0;
			shaExitGuid = 0;

			firstArcherySet.clear();
			secondArcherySet.clear();

			memset(dataStorage, 0, MAX_DATA * sizeof(uint32));
		}

		void OnCreatureCreate(Creature* creature)
		{
			switch (creature->GetEntry())
			{
			case NPC_GU_CLOUDSTRIKE:    guCloudstikeGuid = creature->GetGUID();          return;
			case NPC_MASTER_SNOWDRIFT:  masterSnowdriftGuid = creature->GetGUID();          return;
			case NPC_SHA_VIOLENCE:      shaViolenceGuid = creature->GetGUID();          return;
			case NPC_TARAN_ZHU:         taranZhuGuid = creature->GetGUID();          return;
			case NPC_AZURE_SERPENT:     azureSerpentGuid = creature->GetGUID();          return;
			case NPC_ARCHERY_TARGET:    archeryTargetGuids.push_back(creature->GetGUID());  return;
			case NPC_SNOWDRIFT_POSITION:
			{
				uint32 guid = creature->GetDBTableGUIDLow();

				if (creature->GetDistance(snowdriftCenterPos) > 5.0f && creature->GetDistance(snowdriftCenterPos) < 15.0f)
				{
					minibossPositionsGuid.push_back(creature->GetGUID());
					minibossPositionsGuidSave.push_back(creature->GetGUID());
				}
				else if (creature->GetDistance(snowdriftCenterPos) > 15.0f  && creature->GetDistance(snowdriftCenterPos) < 17.5f)
				{
					firstDefeatedNovicePositionsGuid.push_back(creature->GetGUID());
					firstDefeatedNovicePositionsGuidSave.push_back(creature->GetGUID());
				}
				else if (creature->GetDistance(snowdriftCenterPos) > 17.5f && creature->GetDistance(snowdriftCenterPos) < 25.0f)
				{
					secondDefeatedNovicePositionsGuid.push_back(creature->GetGUID());
					secondDefeatedNovicePositionsGuidSave.push_back(creature->GetGUID());
				}
				break;
			}
			}
		}

		void OnGameObjectCreate(GameObject* go)
		{
			switch (go->GetEntry())
			{
			case GO_CLOUDSTRIKE_ENTRANCE:
				cloudstikeEntranceGuid = go->GetGUID();
				HandleGameObject(0, true, go);
				break;
			case GO_CLOUDSTRIKE_EXIT:
				cloudstikeExitGuid = go->GetGUID();
				break;
			case GO_SNOWDRIFT_ENTRANCE:
				snowdriftEntranceGuid = go->GetGUID();
				HandleGameObject(0, true, go);
				break;
			case GO_SNOWDRIFT_POSSESSIONS:
				go->SetPhaseMask(2, true);
				snowdriftPossessionsGuid = go->GetGUID();
				break;
			case GO_SNOWDRIFT_FIRE_WALL:
				snowdriftFirewallGuid = go->GetGUID();
				break;
			case GO_SNOWDRIFT_DOJO_DOOR:
				snowdriftDojoDoorGuid = go->GetGUID();
				break;
			case GO_SNOWDRIFT_EXIT:
				snowdriftExitGuid = go->GetGUID();
				break;
			case GO_SHA_ENTRANCE:
				shaEntranceGuid = go->GetGUID();
				HandleGameObject(0, true, go);
				break;
			case GO_SHA_EXIT:
				shaExitGuid = go->GetGUID();
				break;
			default:
				return;
			}
		}

		bool SetBossState(uint32 id, EncounterState state)
		{
			if (!InstanceScript::SetBossState(id, state))
				return false;

			switch (id)
			{
			case DATA_GU_CLOUDSTRIKE:
			{
				switch (state)
				{
				case NOT_STARTED:
				case FAIL:
				{
					HandleGameObject(cloudstikeEntranceGuid, true);
					HandleGameObject(cloudstikeExitGuid, false);
					break;
				}
				case IN_PROGRESS:
				{
					HandleGameObject(cloudstikeEntranceGuid, false);
					HandleGameObject(cloudstikeExitGuid, false);
					break;
				}
				case DONE:
				{
					HandleGameObject(cloudstikeEntranceGuid, true);
					HandleGameObject(cloudstikeExitGuid, true);
					HandleGameObject(snowdriftEntranceGuid, true);
					break;
				}
				}
				break;
			}
			case DATA_MASTER_SNOWDRIFT:
			{
				switch (state)
				{
				case NOT_STARTED:
				case FAIL:
					aliveNoviceCount = MAX_NOVICE;
					aliveMinibossCount = 2;
					minibossPositionsGuid = minibossPositionsGuidSave;
					firstDefeatedNovicePositionsGuid = firstDefeatedNovicePositionsGuidSave;
					secondDefeatedNovicePositionsGuid = secondDefeatedNovicePositionsGuidSave;

					HandleGameObject(snowdriftEntranceGuid, true);
					HandleGameObject(snowdriftFirewallGuid, false);
					HandleGameObject(snowdriftDojoDoorGuid, false);
					HandleGameObject(snowdriftExitGuid, false);
					break;
				case IN_PROGRESS:
					HandleGameObject(snowdriftEntranceGuid, false);
					HandleGameObject(snowdriftDojoDoorGuid, false);
					break;
				case DONE:
					if (GameObject* possessions = instance->GetGameObject(snowdriftPossessionsGuid))
						possessions->SetPhaseMask(1, true);

					HandleGameObject(snowdriftEntranceGuid, true);
					HandleGameObject(snowdriftFirewallGuid, true);
					HandleGameObject(snowdriftDojoDoorGuid, true);
					HandleGameObject(snowdriftExitGuid, true);
					HandleGameObject(shaEntranceGuid, true);
					break;
				}
				break;
			}
			case DATA_SHA_VIOLENCE:
			{
				switch (state)
				{
				case NOT_STARTED:
				case FAIL:
					HandleGameObject(shaEntranceGuid, true);
					HandleGameObject(shaExitGuid, false);
					break;
				case IN_PROGRESS:
					HandleGameObject(shaEntranceGuid, false);
				case DONE:
					HandleGameObject(shaEntranceGuid, true);
					HandleGameObject(shaExitGuid, true);
					break;
				}
				break;
			}
			case DATA_TARAN_ZHU:
			{
				if (state == IN_PROGRESS)
					(SPELL_HATE);
				else
				{
					Map::PlayerList const &PlayerList = instance->GetPlayers();

					if (!PlayerList.isEmpty())
						for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
							if (Player* player = i->GetSource())
							{
								player->RemoveAurasDueToSpell(SPELL_HATE);
								player->RemoveAurasDueToSpell(SPELL_HAZE_OF_HATE);
								player->RemoveAurasDueToSpell(SPELL_HAZE_OF_HATE_VISUAL);
							}
				}
				break;
			}
			default:
				break;
			}

			return true;
		}

		void SetData(uint32 type, uint32 data)
		{
			switch (type)
			{
			case DATA_DEFEATED_NOVICE:
				if (!--aliveNoviceCount)
					if (Creature* snowdrift = instance->GetCreature(masterSnowdriftGuid))
						if (snowdrift->IsAIEnabled)
							snowdrift->AI()->DoAction(ACTION_NOVICE_DONE);
				break;
			case DATA_DEFEATED_MINIBOSS:
				if (!--aliveMinibossCount)
				{
					if (Creature* snowdrift = instance->GetCreature(masterSnowdriftGuid))
						if (snowdrift->IsAIEnabled)
							snowdrift->AI()->DoAction(ACTION_MINIBOSS_DONE);

					HandleGameObject(snowdriftFirewallGuid, true);
					HandleGameObject(snowdriftDojoDoorGuid, true);
				}
				break;
			default:
				if (type < MAX_DATA)
					dataStorage[type] = data;
				break;
			}
		}

		uint32 GetData(uint32 type) const
		{
			if (type < MAX_DATA)
				return dataStorage[type];

			return 0;
		}

		uint64 GetData64(uint32 type) const
		{
			switch (type)
			{
			case NPC_GU_CLOUDSTRIKE:        return guCloudstikeGuid;
			case NPC_MASTER_SNOWDRIFT:      return masterSnowdriftGuid;
			case NPC_SHA_VIOLENCE:          return shaViolenceGuid;
			case NPC_TARAN_ZHU:             return taranZhuGuid;
			case NPC_AZURE_SERPENT:         return azureSerpentGuid;
			case NPC_ARCHERY_TARGET:        return Trinity::Containers::SelectRandomContainerElement(archeryTargetGuids);
			case DATA_RANDOM_FIRST_POS:
			{
				if (firstDefeatedNovicePositionsGuid.empty())
					return 0;
				const uint64 guid = Trinity::Containers::SelectRandomContainerElement(firstDefeatedNovicePositionsGuid);
				firstDefeatedNovicePositionsGuid.remove(guid);

				return guid;
			}
			case DATA_RANDOM_SECOND_POS:
			{
				if (secondDefeatedNovicePositionsGuid.empty())
					return 0;

				uint64 guid = Trinity::Containers::SelectRandomContainerElement(secondDefeatedNovicePositionsGuid);
				secondDefeatedNovicePositionsGuid.remove(guid);
				return guid;
			}
			case DATA_RANDOM_MINIBOSS_POS:
			{
				if (minibossPositionsGuid.empty())
					return 0;

				uint64 guid = Trinity::Containers::SelectRandomContainerElement(minibossPositionsGuid);
				minibossPositionsGuid.remove(guid);
				return guid;
			}
			}

			return 0;
		}
	};

};

class npc_shadopan_ambusher : public CreatureScript
{
public:
	npc_shadopan_ambusher() : CreatureScript("npc_shadopan_ambusher") { }

	struct npc_shadopan_ambusherAI : public ScriptedAI
	{
		npc_shadopan_ambusherAI(Creature* creature) : ScriptedAI(creature) {}

		uint32 criseTimer;
		bool inFight;

		void Reset()
		{
			criseTimer = 5000;
			inFight = false;
			me->AddAura(SPELL_STEALTH_COSMETIC, me);
		}

		void MoveInLineOfSight(Unit* who)
		{
			if (!inFight && me->GetDistance(who) < 35.0f)
			{
				inFight = true;
				me->RemoveAurasDueToSpell(SPELL_STEALTH_COSMETIC);
				me->GetMotionMaster()->MoveJump(who->GetPositionX() + frand(-2.0f, 2.0f), who->GetPositionY() + frand(-2.0f, 2.0f), who->GetPositionZ(), 20.0f, 20.0f, 1);
			}
		}

		void MovementInform(uint32 uiType, uint32 uiId)
		{
			if (uiType != EFFECT_MOTION_TYPE)
				return;

			if (uiId == 1)
			{
				DoZoneInCombat();
				if (Unit* target = SelectTarget(SELECT_TARGET_NEAREST))
					AttackStart(target);
			}
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			if (criseTimer <= diff)
			{
				DoZoneInCombat();
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0, 0, true))
				{
					me->getThreatManager().addThreat(target, 1000000.0f);
					me->CastSpell(me, SPELL_CRISE, true);
				}

				criseTimer = urand(3000, 5000);
			}
			else criseTimer -= diff;

			DoMeleeAttackIfReady();
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadopan_ambusherAI(creature);
	}
};

class npc_shadopan_archery : public CreatureScript
{
public:
	npc_shadopan_archery() : CreatureScript("npc_shadopan_archery") { }

	struct npc_shadopan_archeryAI : public ScriptedAI
	{
		npc_shadopan_archeryAI(Creature* creature) : ScriptedAI(creature)
		{
			pInstance = creature->GetInstanceScript();
		}

		uint16 fireTimer;
		InstanceScript* pInstance;

		void Reset()
		{
			fireTimer = urand(2000, 4000);
			me->setActive(true);
		}

		void UpdateAI(const uint32 diff)
		{
			if (!pInstance || !pInstance->GetData(DATA_ARCHERY))
				return;

			if (fireTimer <= diff)
			{
				uint64 targetGuid = 0;

				if (pInstance->GetData(DATA_ARCHERY) == 1 && me->GetEntry() == NPC_ARCHERY_FIRST)
				{
					targetGuid = pInstance->GetData64(NPC_ARCHERY_TARGET);
					fireTimer = urand(2000, 4000);
				}
				else if (pInstance->GetData(DATA_ARCHERY) == 2 && me->GetEntry() == NPC_ARCHERY_SECOND)
				{
					Map::PlayerList const& playerList = pInstance->instance->GetPlayers();

					if (!playerList.isEmpty())
					{
						Map::PlayerList::const_iterator Itr = playerList.begin();

						uint8 advance = urand(0, playerList.getSize() - 1);
						for (uint8 i = 0; i < advance; ++i, ++Itr);

						if (Player* player = Itr->GetSource())
							targetGuid = player->GetGUID();
					}

					fireTimer = urand(5000, 10000);
				}
				else
					fireTimer = 5000;

				if (Unit* target = ObjectAccessor::FindUnit(targetGuid))
					me->CastSpell(target, SPELL_ICE_ARROW, false);
			}
			else fireTimer -= diff;
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadopan_archeryAI(creature);
	}
};

class npc_shadopan_hatred : public CreatureScript
{
public:
	npc_shadopan_hatred() : CreatureScript("npc_shadopan_hatred") { }

	struct npc_shadopan_hatredAI : public ScriptedAI
	{
		npc_shadopan_hatredAI(Creature* creature) : ScriptedAI(creature) {}

		EventMap events;

		void Reset()
		{
			if (me->GetEntry() == NPC_RESIDUAL_OF_HATRED)
			{
				events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(5000, 1000));
				events.ScheduleEvent(EVENT_RING_OF_MALICE, urand(12500, 15000));
				events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2500, 10000));
			}
			else if (me->GetEntry() == NPC_VESTIGE_OF_HATRED)
			{
				events.ScheduleEvent(EVENT_BLACK_CLEAVE, 15000);
				events.ScheduleEvent(EVENT_DEATH_GRIP, urand(7500, 12500));
				events.ScheduleEvent(EVENT_ICE_TRAP, urand(17500, 22500));
				events.ScheduleEvent(EVENT_TOUCH_OF_WEAKNESS, urand(20000, 25000));
			}
			else
			{
				events.ScheduleEvent(EVENT_ICE_TRAP, urand(17500, 22500));
				events.ScheduleEvent(EVENT_SINISTER_STRIKE, urand(2500, 10000));
				events.ScheduleEvent(EVENT_VOLLEY_OF_HATRED, urand(7500, 12500));
			}
		}

		void EnterCombat(Unit* /*victim*/)
		{
			DoZoneInCombat();
		}

		void DamageTaken(Unit* attacker, uint32& damage)
		{
			if (me->HasAura(SPELL_APPARITIONS_AURA))
				damage = 0;
		}

		void UpdateAI(const uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			switch (events.ExecuteEvent())
			{
				// Residual Hatred
			case EVENT_CURSE_OF_AGONY:
			{
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
					me->CastSpell(target, SPELL_CURSE_OF_AGONY, false);

				events.ScheduleEvent(EVENT_CURSE_OF_AGONY, urand(5000, 1000));
				break;
			}
			case EVENT_RING_OF_MALICE:
			{
				me->CastSpell(me, SPELL_RING_OF_MALICE, false);
				events.ScheduleEvent(EVENT_RING_OF_MALICE, urand(12500, 15000));
				break;
			}
			case EVENT_SHADOW_BOLT:
			{
				if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
					me->CastSpell(target, SPELL_SHADOW_BOLT, false);

				events.ScheduleEvent(EVENT_SHADOW_BOLT, urand(2500, 10000));
				break;
			}
			// Vestige of Hatred
			case EVENT_BLACK_CLEAVE:
			{
				if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
					me->CastSpell(target, SPELL_BLACK_CLEAVE, false);

				events.ScheduleEvent(EVENT_BLACK_CLEAVE, 15000);
				break;
			}
			case EVENT_DEATH_GRIP:
			{
				if (Unit* target = SelectTarget(SELECT_TARGET_FARTHEST))
					me->CastSpell(target, SPELL_DEATH_GRIP, false);

				events.ScheduleEvent(EVENT_DEATH_GRIP, urand(7500, 12500));
				break;
			}
			case EVENT_TOUCH_OF_WEAKNESS:
			{
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
					me->CastSpell(target, SPELL_TOUCH_OF_WEAKNESS, false);

				events.ScheduleEvent(EVENT_TOUCH_OF_WEAKNESS, urand(20000, 25000));
				break;
			}
			// Vestige of Hatred
			case EVENT_SINISTER_STRIKE:
			{
				if (Unit* target = SelectTarget(SELECT_TARGET_TOPAGGRO))
					me->CastSpell(target, SPELL_SINISTER_STRIKE, false);

				events.ScheduleEvent(EVENT_SINISTER_STRIKE, urand(2500, 10000));
				break;
			}
			case EVENT_VOLLEY_OF_HATRED:
			{
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
					me->CastSpell(target, SPELL_TOUCH_OF_WEAKNESS, false);

				events.ScheduleEvent(EVENT_VOLLEY_OF_HATRED, urand(7500, 12500));
				break;
			}
			// Common
			case EVENT_ICE_TRAP:
			{
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM))
					me->CastSpell(target, SPELL_ICE_TRAP, false);

				events.ScheduleEvent(EVENT_ICE_TRAP, urand(17500, 22500));
				break;
			}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_shadopan_hatredAI(creature);
	}
};

class spell_shadopan_explosion : public SpellScriptLoader
{
public:
	spell_shadopan_explosion() : SpellScriptLoader("spell_shadopan_explosion") { }

	class spell_shadopan_explosion_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_shadopan_explosion_AuraScript);

		void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
		{
			if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_DEATH)
				if (Unit* caster = GetCaster())
					caster->CastSpell(caster, SPELL_EXPLOSION_DAMAGE, true);
		}

		void Register()
		{
			OnEffectRemove += AuraEffectRemoveFn(spell_shadopan_explosion_AuraScript::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_shadopan_explosion_AuraScript();
	}
};

class spell_shadopan_apparitions : public SpellScriptLoader
{
public:
	spell_shadopan_apparitions() : SpellScriptLoader("spell_shadopan_apparitions") { }

	class spell_shadopan_apparitions_AuraScript : public AuraScript
	{
		PrepareAuraScript(spell_shadopan_apparitions_AuraScript);

		void OnPeriodic(AuraEffect const* aurEff)
		{
			PreventDefaultAction();

			if (Unit* caster = GetCaster())
			{
				if (caster->HasAura(SPELL_PURIFICATION_RITUAL))
				{
					GetAura()->Remove();
					return;
				}

				std::list<Creature*> hatredList;

				caster->GetCreatureListWithEntryInGrid(hatredList, NPC_RESIDUAL_OF_HATRED, 20.0f);
				caster->GetCreatureListWithEntryInGrid(hatredList, NPC_VESTIGE_OF_HATRED, 20.0f);
				caster->GetCreatureListWithEntryInGrid(hatredList, NPC_FRAGMENT_OF_HATRED, 20.0f);

				for (auto hatred : hatredList)
					if (hatred->IsAlive())
						hatred->CastSpell(hatred, GetSpellInfo()->Effects[EFFECT_0].TriggerSpell, true);
			}
		}

		void Register()
		{
			OnEffectPeriodic += AuraEffectPeriodicFn(spell_shadopan_apparitions_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
		}
	};

	AuraScript* GetAuraScript() const
	{
		return new spell_shadopan_apparitions_AuraScript();
	}
};

enum areaTrigger
{
	AREATRIGGER_ARCHERY_FIRST_BEGIN = 8271,
	AREATRIGGER_ARCHERY_FIRST_END = 8272,
	AREATRIGGER_ARCHERY_SECOND_FIRST = 7121,
	AREATRIGGER_ARCHERY_SECOND_END = 7126,
};

class areatrigger_at_shadopan_archery : public AreaTriggerScript
{
public:

	areatrigger_at_shadopan_archery() : AreaTriggerScript("areatrigger_at_shadopan_archery") {}

	bool OnTrigger(Player* player, AreaTriggerEntry const* trigger)
	{
		InstanceScript* pInstance = player->GetInstanceScript();

		if (!pInstance)
			return false;

		switch (trigger->id)
		{
		case AREATRIGGER_ARCHERY_FIRST_BEGIN:
			pInstance->SetData(DATA_ARCHERY, 1);
			break;
		case AREATRIGGER_ARCHERY_FIRST_END:
			pInstance->SetData(DATA_ARCHERY, 0);
			break;
		case AREATRIGGER_ARCHERY_SECOND_FIRST:
			pInstance->SetData(DATA_ARCHERY, 2);
			break;
		case AREATRIGGER_ARCHERY_SECOND_END:
			pInstance->SetData(DATA_ARCHERY, 0);
			break;
		}

		return false;
	}
};

void AddSC_instance_shadopan_monastery()
{
	new instance_shadopan_monastery();
	new npc_shadopan_ambusher();
	new npc_shadopan_archery();
	new npc_shadopan_hatred();
	new spell_shadopan_explosion();
	new spell_shadopan_apparitions();
	new areatrigger_at_shadopan_archery();
}
