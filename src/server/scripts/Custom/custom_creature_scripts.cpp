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



enum AngryTurkeySpells
{
    ENRAGE                                                  = 48138,
    FRENZY                                                  = 48142,
    MORTAL_WOUND                                            = 59265,
    TURKEY_BITE                                             = 55266,
    CHICKEN_CALL                                             = 228,
    TURKEY_FEVER                                          = 54098,
    SUMMON_CHICKEN                                          = 659
};

// Angry Turkey yells
#define SAY_ANGRYTURKEY_AGGRO    "Gobble, gobble you no kill me!"
#define SAY_ANGRYTURKEY_SLAY     "Gobble, gobble, You taste yummy!"
#define SAY_ANGRYTURKEY_DEATH    "Noooooo, Me is turkey sammich!"
#define SAY_ANGRYTURKEY_MORPH    "Gobble gobble, Me stuff you!"

// Angry Turkey
class boss_angry_turkey : public CreatureScript
{
public:
	boss_angry_turkey() : CreatureScript("boss_angry_turkey") { }

    CreatureAI* GetAI(Creature* pCreature) const
    {
        return new boss_angry_turkeyAI (pCreature);
    }

    struct boss_angry_turkeyAI : public ScriptedAI
    {
        boss_angry_turkeyAI(Creature *pCreature) : ScriptedAI(pCreature){}

        uint32 EnrageTimer;
        uint32 WoundTimer;
        uint32 BiteTimer;
        uint32 FeverTimer;
        bool Enraged;
        bool Turkey;

        void Reset()
        {
            me->RestoreDisplayId();
            EnrageTimer = 15000;
            WoundTimer = 5000;
            BiteTimer = 45000;
            FeverTimer = 12000;
            Enraged = false;
            Turkey = false;
        }

        void EnterCombat(Unit* /*who*/)
        {
            me->MonsterYell(SAY_ANGRYTURKEY_AGGRO, LANG_UNIVERSAL, 0);
        }

        void KilledUnit(Unit* victim)
        {
            me->MonsterYell(SAY_ANGRYTURKEY_SLAY, LANG_UNIVERSAL, 0);

            if (victim->GetTypeId() == TYPEID_PLAYER)
                victim->SummonCreature(SUMMON_CHICKEN, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
        }

        void JustDied(Unit* /*killer*/)
        {
            me->MonsterYell(SAY_ANGRYTURKEY_DEATH, LANG_UNIVERSAL, 0);
            me->RestoreDisplayId();
        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;


            if (EnrageTimer <= uiDiff)
            {
                DoCast(me, ENRAGE);
                EnrageTimer = urand(20000,25000);
            } else EnrageTimer -= uiDiff;


            if (WoundTimer <= uiDiff)
            {
                DoCast(me->getVictim(), MORTAL_WOUND);
                WoundTimer = 5000;
            } else WoundTimer -= uiDiff;


            if (FeverTimer <= uiDiff)
            {
                DoCastAOE(TURKEY_FEVER);
                FeverTimer = 20000;
            } else FeverTimer -= uiDiff;

            if (BiteTimer <= uiDiff)
            {
                DoCast(me, TURKEY_BITE);
                if (Unit *pTarget = SelectTarget(SELECT_TARGET_RANDOM,1))
                    DoCast(pTarget, CHICKEN_CALL);
                BiteTimer = 45000;
            } else BiteTimer -= uiDiff;


            if (HealthBelowPct(80) && !Turkey)
            {
                me->SetDisplayId(21774);
                me->MonsterYell(SAY_ANGRYTURKEY_MORPH, LANG_UNIVERSAL, 0);
                DoCast(me, ENRAGE);
                EnrageTimer = urand(25000,35000);
                Turkey = true;
            }


            if (HealthBelowPct(15) && !Enraged)
            {
                DoCast(me, FRENZY);
                Enraged = true;
            }

            DoMeleeAttackIfReady();
        }
    };

};


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
	new boss_angry_turkey;
}

