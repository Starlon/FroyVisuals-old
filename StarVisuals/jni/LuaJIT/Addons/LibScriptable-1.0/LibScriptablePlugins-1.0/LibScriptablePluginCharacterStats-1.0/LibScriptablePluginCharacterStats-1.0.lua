local MAJOR = "LibScriptablePluginCharacterStats-1.0" 
local MINOR = 24

local PluginCharacterStats = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginCharacterStats then return end

local _G = _G

if not PluginCharacterStats.__index then
	PluginCharacterStats.__index = PluginCharacterStats
end

-- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginCharacterStats:New(environment)

	environment.GetArmorPenetration = _G.GetArmorPenetration -- GetArmorPenetration() - Returns percent of armor ignored by your physical attacks. 
	environment.GetAttackPowerForStat = _G.GetAttackPowerForStat --GetAttackPowerForStat(stat, value) - Returns the amount of attack power contributed by a specified amount of a particular stat. 
	environment.GetBlockChance = _G.GetBlockChange -- GetBlockChance() - Returns the player's percentage block chance. 
	environment.GetCombatRating = _G.GetCombatRating -- GetCombatRating(ratingID) - Returns the player's combat rating for a particular combat rating. (2.0) 
	environment.GetCombatRatingBonus = _G.GetCombatRatingBonus -- GetCombatRatingBonus(ratingID) - Returns the player's combat rating bonus for a particular combat rating. (2.0) 
	environment.GetCritChance = _G.GetCritChance -- GetCritChance() - Returns the player's melee critical hit chance 
	environment.GetCritChanceFromAgility = _G.GetCritChanceFromAgility -- GetCritChanceFromAgility("unit") - Returns the amount of your critical hit chance contributed by Agility. 
	environment.GetDodgeChance = _G.GetDodgeChance -- GetDodgeChance() - Returns the player's percentage dodge chance. 
	environment.GetExpertise = _G.GetExpertise -- GetExpertise() - Returns main hand and offhand expertise value. 
	environment.GetExpertisePercent = _G.GetExpertisePercent -- GetExpertisePercent() - Returns reduction in dodge and parry chances for main hand and offhand attacks, in percent. 
	environment.GetManaRegen = _G.GetManaRegen -- GetManaRegen() - Returns the player's mana regeneration rates. 
	environment.GetMaxCombatRatingBonus = _G.GetMaxCombatRatingBonus -- GetMaxCombatRatingBonus(lowestRating) 
	environment.GetParryChance = _G.GetParryChance -- GetParryChance() - Returns the player's percentage parry chance. 
	environment.GetPetSpellBonusDamage = _G.GetPetSpellBonusDamage -- GetPetSpellBonusDamage 
	environment.GetSpellBonusDamage = _G.GetSpellBonusDamage -- GetSpellBonusDamage(spellTreeID) - Returns the raw spell damage of the player for a given spell tree. 
	environment.GetRangedCritChance = _G.GetRangedCritChance -- GetRangedCritChance() 
	environment.GetSpellBonusHealing = _G.GetSpellBonusHealing -- GetSpellBonusHealing() - Returns the raw bonus healing of the player. 
	environment.GetSpellCritChance = _G.GetSpellCritChance -- GetSpellCritChance(school) - returns the players critical hit chance with a particular spell school. 
	environment.GetShieldBlock = _G.GetShieldBlock -- GetShieldBlock() 
	environment.GetSpellCritChanceFromIntellect = _G.GetSpellCritChanceFromIntellect -- GetSpellCritChanceFromIntellect("unit") 
	environment.GetSpellPenetration = _G.GetSpellPenetration -- GetSpellPenetration() 
	
	return environment
	
end