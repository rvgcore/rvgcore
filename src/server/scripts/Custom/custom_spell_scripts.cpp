/* ScriptData
SDName: Pilgrim Bounty Fixes
SDAuthor: Zyrain
SDComment: 
SDCategory: Spells
EndScriptData */

#include "ScriptPCH.h"
#include "SpellAuraEffects.h"
#include "SkillDiscovery.h"
#include "GridNotifiers.h"
#include "Vehicle.h"
#include "Group.h"
#include "LFGMgr.h"

enum FriendOrFowl
{
    SPELL_TURKEY_VENGEANCE  = 25285,
};

class spell_gen_turkey_marker : public SpellScriptLoader
{
    public:
        spell_gen_turkey_marker() : SpellScriptLoader("spell_gen_turkey_marker") { }

        class spell_gen_turkey_marker_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_gen_turkey_marker_AuraScript);

            void OnApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                // store stack apply times, so we can pop them while they expire
                _applyTimes.push_back(getMSTime());

                // on stack 15 cast the achievement crediting spell
                if (GetStackAmount() >= 15)
                    GetTarget()->CastSpell(GetTarget(), SPELL_TURKEY_VENGEANCE, true, NULL, aurEff, GetCasterGUID());
            }

            void OnPeriodic(AuraEffect const* /*aurEff*/)
            {
                if (_applyTimes.empty())
                    return;

                // pop stack if it expired for us
                if (_applyTimes.front() + GetMaxDuration() < getMSTime())
                    ModStackAmount(-1, AURA_REMOVE_BY_EXPIRE);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_gen_turkey_marker_AuraScript::OnApply, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_gen_turkey_marker_AuraScript::OnPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
            }

            std::list<uint32> _applyTimes;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_gen_turkey_marker_AuraScript();
        }
};


enum TheTurkinator
{
    SPELL_KILL_COUNTER_VISUAL       = 62015,
    SPELL_KILL_COUNTER_VISUAL_MAX   = 62021,
};

#define THE_THUKINATOR_10           "Turkey Hunter!"
#define THE_THUKINATOR_20           "Turkey Domination!"
#define THE_THUKINATOR_30           "Turkey Slaughter!"
#define THE_THUKINATOR_40           "TURKEY TRIUMPH!"

class spell_gen_turkey_tracker : public SpellScriptLoader
{
    public:
        spell_gen_turkey_tracker() : SpellScriptLoader("spell_gen_turkey_tracker") {}

        class spell_gen_turkey_tracker_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_turkey_tracker_SpellScript);

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_KILL_COUNTER_VISUAL_MAX))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex /*effIndex*/)
            {
                if (GetCaster()->GetAura(SPELL_KILL_COUNTER_VISUAL_MAX))
                    return;

                Player* target = GetHitPlayer();
                if (!target)
                    return;

                if (Aura const* aura = GetCaster()->ToPlayer()->GetAura(GetSpellInfo()->Id))
                {
                    switch (aura->GetStackAmount())
                    {
                        case 10:
                            target->MonsterTextEmote(THE_THUKINATOR_10, 0, true);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                            break;
                        case 20:
                            target->MonsterTextEmote(THE_THUKINATOR_20, 0, true);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                            break;
                        case 30:
                            target->MonsterTextEmote(THE_THUKINATOR_30, 0, true);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                            break;
                        case 40:
                            target->MonsterTextEmote(THE_THUKINATOR_40, 0, true);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL);
                            GetCaster()->CastSpell(target, SPELL_KILL_COUNTER_VISUAL_MAX); // Achievement Credit
                            break;
                        default:
                            break;
                    }
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_turkey_tracker_SpellScript::HandleScript, EFFECT_1, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_turkey_tracker_SpellScript();
        }
};

class spell_gen_feast_on : public SpellScriptLoader
{
    public:
        spell_gen_feast_on() : SpellScriptLoader("spell_gen_feast_on") { }

        class spell_gen_feast_on_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_feast_on_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                int32 bp0 = GetSpellInfo()->Effects[EFFECT_0].CalcValue();

                Unit* caster = GetCaster();
                if (caster->IsVehicle())
                    if (Unit* player = caster->GetVehicleKit()->GetPassenger(0))
                        caster->CastSpell(player, bp0, true, NULL, NULL, player->ToPlayer()->GetGUID());
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_feast_on_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_feast_on_SpellScript();
        }
};

enum WellFedPilgrimsBount
{
    // Feast On
    SPELL_A_SERVING_OF_TURKEY           = 61807,
    SPELL_A_SERVING_OF_CRANBERRIES      = 61804,
    SPELL_A_SERVING_OF_STUFFING         = 61806,
    SPELL_A_SERVING_OF_SWEET_POTATOES   = 61808,
    SPELL_A_SERVING_OF_PIE              = 61805,

    // Well Fed
    SPELL_WELL_FED_AP                   = 65414,
    SPELL_WELL_FED_ZM                   = 65412,
    SPELL_WELL_FED_HIT                  = 65416,
    SPELL_WELL_FED_HASTE                = 65410,
    SPELL_WELL_FED_SPIRIT               = 65415,

    // Pilgrim's Paunch
    SPELL_THE_SPIRIT_OF_SHARING         = 61849,
};

class spell_gen_well_fed_pilgrims_bount : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_well_fed_pilgrims_bount(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_well_fed_pilgrims_bount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_well_fed_pilgrims_bount_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_well_fed_pilgrims_bount_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleScript(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Player* target = GetHitPlayer();
                if (!target)
                    return;

                Aura const* Turkey = target->GetAura(SPELL_A_SERVING_OF_TURKEY);
                Aura const* Cranberies = target->GetAura(SPELL_A_SERVING_OF_CRANBERRIES);
                Aura const* Stuffing = target->GetAura(SPELL_A_SERVING_OF_STUFFING);
                Aura const* SweetPotatoes = target->GetAura(SPELL_A_SERVING_OF_SWEET_POTATOES);
                Aura const* Pie = target->GetAura(SPELL_A_SERVING_OF_PIE);

                // A Serving of Turkey - Attack power and Stamina
                if (Aura const* aura = target->GetAura(_triggeredSpellId1))
                {
                    if (aura->GetStackAmount() == 5)
                        target->CastSpell(target, _triggeredSpellId2, true);
                }

                // The Spirit of Sharing - Achievement Credit
                if (!target->GetAura(SPELL_THE_SPIRIT_OF_SHARING))
                {
                    if ((Turkey && Turkey->GetStackAmount() == 5) && (Cranberies && Cranberies->GetStackAmount() == 5) && (Stuffing && Stuffing->GetStackAmount() == 5) &&
                        (SweetPotatoes && SweetPotatoes->GetStackAmount() == 5) && (Pie && Pie->GetStackAmount() == 5))
                        target->CastSpell(target, SPELL_THE_SPIRIT_OF_SHARING, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_well_fed_pilgrims_bount_SpellScript::HandleScript, EFFECT_0, SPELL_EFFECT_APPLY_AURA);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_well_fed_pilgrims_bount_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum OnPlatePilgrimsBount
{
    // "FOOD FIGHT!" - Achivement Credit
    SPELL_ON_PLATE_TURKEY           = 61928,
    SPELL_ON_PLATE_CRANBERRIES      = 61925,
    SPELL_ON_PLATE_STUFFING         = 61927,
    SPELL_ON_PLATE_SWEET_POTATOES   = 61929,
    SPELL_ON_PLATE_PIE              = 61926,

    // Sharing is Caring - Achivement Credit
    SPELL_PASS_THE_TURKEY           = 66373,
    SPELL_PASS_THE_CRANBERRIES      = 66372,
    SPELL_PASS_THE_STUFFING         = 66375,
    SPELL_PASS_THE_SWEET_POTATOES   = 66376,
    SPELL_PASS_THE_PIE              = 66374,
};

class spell_gen_on_plate_pilgrims_bount : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId1;
        uint32 _triggeredSpellId2;

    public:
        spell_gen_on_plate_pilgrims_bount(const char* name, uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScriptLoader(name),
            _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

        class spell_gen_on_plate_pilgrims_bount_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_on_plate_pilgrims_bount_SpellScript)
        private:
            uint32 _triggeredSpellId1;
            uint32 _triggeredSpellId2;

        public:
            spell_gen_on_plate_pilgrims_bount_SpellScript(uint32 triggeredSpellId1, uint32 triggeredSpellId2) : SpellScript(),
                _triggeredSpellId1(triggeredSpellId1), _triggeredSpellId2(triggeredSpellId2) { }

            bool Validate(SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId1))
                    return false;
                if (!sSpellMgr->GetSpellInfo(_triggeredSpellId2))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (caster->IsVehicle())
                {
                    Unit* player = caster->GetVehicleKit()->GetPassenger(0);
                    if (!player)
                        return;

                    player->CastSpell(GetHitUnit(), _triggeredSpellId1, true, NULL, NULL, player->ToPlayer()->GetGUID());
                    player->CastSpell(player, _triggeredSpellId2, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_gen_on_plate_pilgrims_bount_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_on_plate_pilgrims_bount_SpellScript(_triggeredSpellId1, _triggeredSpellId2);
        }
};

enum BountifulFeast
{
    // Bountiful Feast
    SPELL_BOUNTIFUL_FEAST_DRINK          = 66041,
    SPELL_BOUNTIFUL_FEAST_FOOD           = 66478,
    SPELL_BOUNTIFUL_FEAST_REFRESHMENT    = 66622,
};

class spell_gen_bountiful_feast : public SpellScriptLoader
{
    public:
        spell_gen_bountiful_feast() : SpellScriptLoader("spell_gen_bountiful_feast") { }

        class spell_gen_bountiful_feast_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_gen_bountiful_feast_SpellScript);

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_DRINK, true);
                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_FOOD, true);
                caster->CastSpell(caster, SPELL_BOUNTIFUL_FEAST_REFRESHMENT, true);
            }

            void Register()
            {
                OnEffectHit += SpellEffectFn(spell_gen_bountiful_feast_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_gen_bountiful_feast_SpellScript();
        }
};

enum PilgrimsBountyBuffFood
{
    // Pilgrims Bounty Buff Food
    SPELL_WELL_FED_AP_TRIGGER       = 65414,
    SPELL_WELL_FED_ZM_TRIGGER       = 65412,
    SPELL_WELL_FED_HIT_TRIGGER      = 65416,
    SPELL_WELL_FED_HASTE_TRIGGER    = 65410,
    SPELL_WELL_FED_SPIRIT_TRIGGER   = 65415,
};

class spell_pilgrims_bount_buff_food : public SpellScriptLoader
{
    private:
        uint32 _triggeredSpellId;
    public:
        spell_pilgrims_bount_buff_food(const char* name, uint32 triggeredSpellId) : SpellScriptLoader(name), _triggeredSpellId(triggeredSpellId) { }

        class spell_pilgrims_bount_buff_food_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pilgrims_bount_buff_food_AuraScript)
        private:
            uint32 _triggeredSpellId;

        public:
            spell_pilgrims_bount_buff_food_AuraScript(uint32 triggeredSpellId) : AuraScript(), _triggeredSpellId(triggeredSpellId) { }

            bool Load()
            {
                _handled = false;
                return true;
            }

            void HandleTriggerSpell(AuraEffect const* /*aurEff*/)
            {
                if (_handled)
                    return;

                Unit* caster = GetCaster();
                if (!caster)
                    return;

                _handled = true;
                caster->CastSpell(caster, _triggeredSpellId, true);
            }

            void Register()
            {
                OnEffectPeriodic += AuraEffectPeriodicFn(spell_pilgrims_bount_buff_food_AuraScript::HandleTriggerSpell, EFFECT_2, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
            }

            bool _handled;
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pilgrims_bount_buff_food_AuraScript(_triggeredSpellId);
        }
};


void AddSC_custom_spell_scripts()
{

    new spell_gen_turkey_tracker();
    new spell_gen_feast_on();
    new spell_gen_well_fed_pilgrims_bount("spell_gen_well_fed_pilgrims_bount_ap", SPELL_A_SERVING_OF_TURKEY, SPELL_WELL_FED_AP);
    new spell_gen_well_fed_pilgrims_bount("spell_gen_well_fed_pilgrims_bount_zm", SPELL_A_SERVING_OF_CRANBERRIES, SPELL_WELL_FED_ZM);
    new spell_gen_well_fed_pilgrims_bount("spell_gen_well_fed_pilgrims_bount_hit", SPELL_A_SERVING_OF_STUFFING, SPELL_WELL_FED_HIT);
    new spell_gen_well_fed_pilgrims_bount("spell_gen_well_fed_pilgrims_bount_haste", SPELL_A_SERVING_OF_SWEET_POTATOES, SPELL_WELL_FED_HASTE);
    new spell_gen_well_fed_pilgrims_bount("spell_gen_well_fed_pilgrims_bount_spirit", SPELL_A_SERVING_OF_PIE, SPELL_WELL_FED_SPIRIT);
    new spell_gen_on_plate_pilgrims_bount("spell_gen_on_plate_pilgrims_bount_turkey", SPELL_ON_PLATE_TURKEY, SPELL_PASS_THE_TURKEY);
    new spell_gen_on_plate_pilgrims_bount("spell_gen_on_plate_pilgrims_bount_cranberries", SPELL_ON_PLATE_CRANBERRIES, SPELL_PASS_THE_CRANBERRIES);
    new spell_gen_on_plate_pilgrims_bount("spell_gen_on_plate_pilgrims_bount_stuffing", SPELL_ON_PLATE_STUFFING, SPELL_PASS_THE_STUFFING);
    new spell_gen_on_plate_pilgrims_bount("spell_gen_on_plate_pilgrims_bount_sweet_potatoes", SPELL_ON_PLATE_SWEET_POTATOES, SPELL_PASS_THE_SWEET_POTATOES);
    new spell_gen_on_plate_pilgrims_bount("spell_gen_on_plate_pilgrims_bount_pie", SPELL_ON_PLATE_PIE, SPELL_PASS_THE_PIE);
    new spell_gen_bountiful_feast();
    new spell_pilgrims_bount_buff_food("spell_gen_slow_roasted_turkey", SPELL_WELL_FED_AP_TRIGGER);
    new spell_pilgrims_bount_buff_food("spell_gen_cranberry_chutney", SPELL_WELL_FED_ZM_TRIGGER);
    new spell_pilgrims_bount_buff_food("spell_gen_spice_bread_stuffing", SPELL_WELL_FED_HIT_TRIGGER);
    new spell_pilgrims_bount_buff_food("spell_gen_pumpkin_pie", SPELL_WELL_FED_SPIRIT_TRIGGER);
    new spell_pilgrims_bount_buff_food("spell_gen_candied_sweet_potato", SPELL_WELL_FED_HASTE_TRIGGER);
}
