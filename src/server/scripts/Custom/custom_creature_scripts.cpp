/* ScriptData
SDName: Hallow's End Custom Boss: Lys Ladimore
SDAuthor: Zyrain
SDComment: 
SDCategory: Hallow's End
EndScriptData */

#include "ScriptPCH.h"


enum LysLadimoreSpells
{
	SPELL_SHADOW_BOLT_VOLLEY                                 = 36736,
	SPELL_SHADOW_BOLT                                        = 69068,
	SPELL_ARMY_OF_THE_DEAD                                   = 67874,
	SPELL_MIND_CONTROL                                       = 67229,
	SPELL_PIERCING_SHADOW 									 = 36698

};

// Lys Ladimore yells
#define SAY_LYS_LADIMORE_AGGRO      "I'm sorry, I cannot control myself!"
#define SAY_LYS_LADIMORE_DEATH       "Thank You, I am now set free at last. If only my sister could forgive me..."
#define SAY_LYS_LADIMORE_SLAY				 "Welcome to the undead!"
#define SAY_LYS_LADIMORE_MIND_CONTROL        "Come join me in the underworld!"


// Lys Ladimore

class boss_lys_ladimore : public CreatureScript
{
public:
	boss_lys_ladimore() : CreatureScript("boss_lys_ladimore") { }

	CreatureAI* GetAI(Creature* pCreature) const
	{
		return new boss_lys_ladimoreAI (pCreature);
	}

	struct boss_lys_ladimoreAI : public ScriptedAI
	{
		boss_lys_ladimoreAI(Creature *pCreature) : ScriptedAI(pCreature){}



		uint32 BoltTimer;
		uint32 ControlTimer;
		uint32 VolleyTimer;
		uint32 PiercingTimer;
		uint32 ArmyTimer;


		void Reset()
		{

			ControlTimer = 30000;
			BoltTimer = 10000;
			VolleyTimer = 15000;
			PiercingTimer = 20000;
			ArmyTimer =25000;
		}



		void EnterCombat(Unit* /*who*/)
		{
			me->MonsterYell(SAY_LYS_LADIMORE_AGGRO, LANG_UNIVERSAL, 0);
		}

		void KilledUnit(Unit* victim)
		{
			me->MonsterYell(SAY_LYS_LADIMORE_SLAY, LANG_UNIVERSAL, 0);
		}

		void JustDied(Unit* /*killer*/)
		{
			me->MonsterYell(SAY_LYS_LADIMORE_DEATH, LANG_UNIVERSAL, 0);
		}

		void UpdateAI(const uint32 uiDiff)
		{
			if (!UpdateVictim())
				return;

			// Army of the dead
			if (ArmyTimer <= uiDiff)
			{
				me->InterruptNonMeleeSpells(false);
				if (Unit *pTarget = SelectTarget(SELECT_TARGET_TOPAGGRO,0))
					DoCast(pTarget, SPELL_ARMY_OF_THE_DEAD);
				ArmyTimer = 45000;
			} else ArmyTimer -= uiDiff;


			// Mind Control
			if (ControlTimer <= uiDiff)
			{
				me->InterruptNonMeleeSpells(false);
				if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,0))
					DoCast(pTarget, SPELL_MIND_CONTROL);
				me->MonsterYell(SAY_LYS_LADIMORE_MIND_CONTROL, LANG_UNIVERSAL, 0);
				ControlTimer = 30000;
			} else ControlTimer -= uiDiff;

			// Piercing shadow
			if (PiercingTimer <= uiDiff)
			{
				me->InterruptNonMeleeSpells(false);
				if (Unit *pTarget = SelectTarget(SELECT_TARGET_TOPAGGRO,0))
					DoCast(pTarget, SPELL_PIERCING_SHADOW);
				PiercingTimer = 10000;
			} else PiercingTimer -= uiDiff;

			// AOE Shadow Bolt Volley
			if (VolleyTimer <= uiDiff)
			{
				DoCastAOE(SPELL_SHADOW_BOLT_VOLLEY);
				VolleyTimer = 15000;
			} else VolleyTimer -= uiDiff;


			DoSpellAttackIfReady(SPELL_SHADOW_BOLT);
		}
	};

};


void AddSC_custom_creature_scripts()
{
	new boss_lys_ladimore;
}

