/*
 * Copyright (C) 2008-2011 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
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

/* ScriptData
SDName: Scrooge and the Ghost of Christmas Past, Present and Future
SD%Complete: 95
SDComment:
SDCategory: World
EndScriptData */

#include "ScriptPCH.h"
#include "Spell.h"
#include "WorldPacket.h"

#define SPELL_BERSERK               26662

struct boss_christmasAI : public ScriptedAI
{
	boss_christmasAI(Creature *pCreature) : ScriptedAI(pCreature){}

	uint32 GhostTimer;
	uint32 EnrageTimer;
	bool _checker;


	virtual void Reset() = 0;


	void ChristmasReset()
	{

		GhostTimer = 60000;
		me->ClearUnitState(UNIT_STATE_STUNNED);
		EnrageTimer = 15*60000;
		_checker = true;
	}


	void CheckEnrage(uint32 diff)
	{
		if (EnrageTimer <= diff)
		{
			if (!me->IsNonMeleeSpellCasted(true))
			{
				DoCast(me, SPELL_BERSERK);
				EnrageTimer = 60*60000;
			} else EnrageTimer = 0;
		} else EnrageTimer-=diff;
	}

	Creature* DeadGhost(uint32 diff)
	{
		std::list<Creature*> lUnitList;
		me->GetCreatureListWithEntryInGrid(lUnitList, 120003, 150.0f);
		me->GetCreatureListWithEntryInGrid(lUnitList, 120004, 150.0f);
		me->GetCreatureListWithEntryInGrid(lUnitList, 120005, 150.0f);



		if (!lUnitList.empty())
			for (std::list<Creature*>::const_iterator iter = lUnitList.begin(); iter != lUnitList.end(); ++iter)
			{
				if (Creature* c = *iter)
				{
					if (c->isDead())
					{
						if (GhostTimer <= diff)
						{
							c->Respawn();
							GhostTimer = 60000;
						}
						else
						{
							GhostTimer -= diff;

							if (GhostTimer == 0)
								GhostTimer=60000;

							return NULL;
						}
					}
				}
			}
		return NULL;
	}

	bool GhostAreAlive()
	{
		std::list<Creature*> lUnitList;
		me->GetCreatureListWithEntryInGrid(lUnitList, 120003, 150.0f);
		me->GetCreatureListWithEntryInGrid(lUnitList, 120004, 150.0f);
		me->GetCreatureListWithEntryInGrid(lUnitList, 120005, 150.0f);

		if (!lUnitList.empty())
			for (std::list<Creature*>::const_iterator iter = lUnitList.begin(); iter != lUnitList.end(); ++iter)
			{
				if (Creature* c = *iter)
				{
					if (c->isDead()) _checker = false;
					else return true;
				}
			}

		return _checker;
	}


	void HandleGhost(uint32 diff)
	{

		Creature* c = DeadGhost(diff);

	}
};



enum Scroogepells
{
	SPELL_SHIELD				= 71463,
	SPELL_UPPERCUT              = 26007,
	SPELL_UNBALANCING_STRIKE    = 26613,
	SPELL_SNOWBALL				= 25677,
	INVOCATION_VISUAL_ACTIVE    = 70934
};


// Scrooge yells
#define SAY_SCROOGE_AGGRO         "You may be an undigested bit of beef, a blot of mustard, a crumb of cheese, a fragment of underdone potato. There's more of gravy than of grave about you, whatever you are!"
#define SAY_SCROOGE_SLAY          "Darkness is cheap, and I like it."
#define SAY_SCROOGE_DEATH         "Well, then, I'll just swallow this and be tortured by a legion of hobgoblins, all of my own creation! It's all HUMBUG, I tell you, HUMBUG!"


class boss_scrooge : public CreatureScript
{
public:
	boss_scrooge() : CreatureScript("boss_scrooge") { }

	CreatureAI* GetAI(Creature* creature) const
	{
		return new boss_scroogeAI (creature);
	}

	struct boss_scroogeAI : public boss_christmasAI
	{
		boss_scroogeAI(Creature* c) : boss_christmasAI(c) {}

		uint32 UpperCut_Timer;
		uint32 UnbalancingStrike_Timer;
		uint32 Snowball_Timer;

		void Reset()
		{
			ChristmasReset();
			UpperCut_Timer = 14000 + rand()%15000;
			UnbalancingStrike_Timer = 8000 + rand()%10000;
			Snowball_Timer = 10000;
			me->RemoveAurasDueToSpell(SPELL_SHIELD);
			me->RemoveAurasDueToSpell(INVOCATION_VISUAL_ACTIVE);
		}

		void EnterCombat(Unit* /*who*/)
		{
			me->MonsterSay(SAY_SCROOGE_AGGRO, LANG_UNIVERSAL, 0);
			DoCast(me, INVOCATION_VISUAL_ACTIVE);
		}

		void KilledUnit(Unit* victim)
		{
			me->MonsterSay(SAY_SCROOGE_SLAY, LANG_UNIVERSAL, 0);
		}

		void JustDied(Unit* /*killer*/)
		{
			me->MonsterSay(SAY_SCROOGE_DEATH, LANG_UNIVERSAL, 0);
		}


		void UpdateAI(const uint32 diff)
		{
			//Return since we have no target
			if (!UpdateVictim())
				return;

			if (HealthBelowPct(20))
			{
				DoCast(me, SPELL_SHIELD);
				me->RemoveAurasDueToSpell(INVOCATION_VISUAL_ACTIVE);

				if(!GhostAreAlive())
				{
					DoCast(me, INVOCATION_VISUAL_ACTIVE);
					me->RemoveAurasDueToSpell(SPELL_SHIELD);
				}
			}

			//UnbalancingStrike_Timer
			if (UnbalancingStrike_Timer <= diff)
			{
				DoCast(me->getVictim(), SPELL_UNBALANCING_STRIKE);
				UnbalancingStrike_Timer = 8000+rand()%12000;
			} else UnbalancingStrike_Timer -= diff;

			if (UpperCut_Timer <= diff)
			{
				Unit* randomMelee = SelectTarget(SELECT_TARGET_RANDOM, 0, NOMINAL_MELEE_RANGE, true);
				if (randomMelee)
					DoCast(randomMelee, SPELL_UPPERCUT);
				UpperCut_Timer = 15000+rand()%15000;
			} else UpperCut_Timer -= diff;


			if (Snowball_Timer <= diff)
			{
				if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
					DoCast(pTarget, SPELL_SNOWBALL);
				Snowball_Timer = 10000;
			} else Snowball_Timer -= diff;

			HandleGhost(diff);
			CheckEnrage(diff);

			DoMeleeAttackIfReady();
		}
	};
};



enum Ghostpells
{
	EARTHQUAKE                                              = 46240,
	SHOCK_OF_SORROW                                         = 50760,
	ENTROPIC_AURA                                           = 36784,
	AURAL_SHOCK                                             = 14538
};

// Ghost yells

#define SAY_GHOST_PRESENT          "In life, my spirit never rose beyond the limits of our money-changing holes! Now I am doomed to wander without rest or peace, incessant torture and remorse!"
#define SAY_GHOST_PAST         "My time with you is at an end, Ebenezer Scrooge. Will you profit from what I've shown you of the good in most men's hearts?"
#define SAY_GHOST_FUTURE        "Before I draw nearer to that stone, tell me! Are these the shadows of things that must be, or are they the shadows of things that MIGHT be?"


/*#########
# Ghost of Christmas Past, Present and Future
#########*/

class npc_boss_ghostpast : public CreatureScript
{
public:
	npc_boss_ghostpast() : CreatureScript("npc_boss_ghostpast") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_boss_ghostpastAI (pCreature);
	}

	struct npc_boss_ghostpastAI : public boss_christmasAI
	{
		npc_boss_ghostpastAI(Creature *pCreature) : boss_christmasAI(pCreature){}

		uint32 EarthquakeTimer;


		void Reset()
		{
			ChristmasReset();
			EarthquakeTimer = 8000;
		}

		void EnterCombat(Unit* /*who*/)
		{
			DoCast(me, ENTROPIC_AURA);
		}


		void JustDied(Unit* /*killer*/)
		{
			me->MonsterSay(SAY_GHOST_PAST, LANG_UNIVERSAL, 0);
		}

		void UpdateAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
				return;

			if (EarthquakeTimer <= uiDiff)
			{
				DoCast(EARTHQUAKE);
				EarthquakeTimer = 20000;
			} else EarthquakeTimer -= uiDiff;


			DoMeleeAttackIfReady();
		}
	};

};


class npc_boss_ghostpresent : public CreatureScript
{
public:
	npc_boss_ghostpresent() : CreatureScript("npc_boss_ghostpresent") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_boss_ghostpresentAI (pCreature);
	}

	struct npc_boss_ghostpresentAI : public boss_christmasAI
	{
		npc_boss_ghostpresentAI(Creature *pCreature) : boss_christmasAI(pCreature){}


		uint32 ShockTimer;


		void Reset()
		{
			ChristmasReset();
			ShockTimer = 20000;
		}

		void EnterCombat(Unit* /*who*/)
		{
			DoCast(me, ENTROPIC_AURA);
		}


		void JustDied(Unit* /*killer*/)
		{
			me->MonsterSay(SAY_GHOST_PRESENT, LANG_UNIVERSAL, 0);
		}

		void UpdateAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
				return;

			if (ShockTimer <= uiDiff)
			{
				if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
					DoCast(pTarget, SHOCK_OF_SORROW);
				ShockTimer = 20000;
			} else ShockTimer -= uiDiff;

			DoMeleeAttackIfReady();
		}
	};

};


class npc_boss_ghostfuture : public CreatureScript
{
public:
	npc_boss_ghostfuture() : CreatureScript("npc_boss_ghostfuture") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new npc_boss_ghostfutureAI (pCreature);
	}

	struct npc_boss_ghostfutureAI : public boss_christmasAI
	{
		npc_boss_ghostfutureAI(Creature *pCreature) : boss_christmasAI(pCreature){}


		uint32 AuralShockTimer;

		void Reset()
		{
			ChristmasReset();

			AuralShockTimer = 50000;
		}

		void EnterCombat(Unit* /*who*/)
		{
			DoCast(me, ENTROPIC_AURA);
		}


		void JustDied(Unit* /*killer*/)
		{
			me->MonsterSay(SAY_GHOST_FUTURE, LANG_UNIVERSAL, 0);
		}

		void UpdateAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
				return;

			if (AuralShockTimer <= uiDiff)
			{
				DoCast(AURAL_SHOCK);
				AuralShockTimer = 20000;
			} else AuralShockTimer -= uiDiff;

			DoMeleeAttackIfReady();
		}
	};

};


void AddSC_custom_boss_scrooge()
{
	new boss_scrooge;
	new npc_boss_ghostpast;
	new npc_boss_ghostpresent;
	new npc_boss_ghostfuture;
}
