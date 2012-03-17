local MAJOR = "LibScriptablePluginAbility-1.0" 
local MINOR = 24

local PluginAbility = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginAbility then return end
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0")

if not PluginAbility.__index then
	PluginAbility.__index = PluginAbility
end

local Detail = Inspect.Ability.Detail
local cache = setmetatable({}, {__mode="v"})
local ScriptEnv = {}
local GetTime = Inspect.Time.Real

local function capital(str)
	return str and string.upper(string.sub(str, 1, 1)) .. string.sub(str, 2, string.len(str))
end

local function split(str, sep)
        local sep, fields = sep or ":", {}
        local pattern = string.format("([^%s]+)", sep)
        str:gsub(pattern, function(c) fields[#fields+1] = c end)
        return fields
end

--- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginAbility:New(environment)
	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end
	return environment
end


---autoattack:	Autoattack mode of the ability.
local function AbilityAutoAttack(ability)
	local details = Detail(ability)
	return details and details.autoattack
end
ScriptEnv.AbilityAutoAttack = AbilityAutoAttack

---castingTime:	Casting time of the ability, in seconds.
local function AbilityCastingTime(ability)
	local details = Detail(ability)
	return details and details.castingTime
end
ScriptEnv.AbilityCastingTime = AbilityCastingTime

---channeled:	Signals that the ability is channeled.
local function AbilityChanneled(ability)
	local details = Detail(ability)
	return details and details.channeled
end
ScriptEnv.AbilityChanneled = AbilityChanneled

---continuous:	Signals that the ability is continuous.
local function AbilityContinuous(ability)
	local details = Detail(ability)
	return details and details.continuous
end
ScriptEnv.AbilityContinuous = AbilityContinuous

---cooldown:	Cooldown of the ability, in seconds.
local function AbilityCooldown(ability)
	local details = Detail(ability)
	return details and details.cooldown
end
ScriptEnv.AbilityCooldown = AbilityCooldown

---costCharge:	The amount of charge this ability consumes on use.
local function AbilityCostCharge(ability)
	local details = Detail(ability)
	return details and details.costCharge
end
ScriptEnv.AbilityCostCharge = AbilityCostCharge

---costEnergy:	The amount of energy this ability consumes on use.
local function AbilityCostEnergy(ability)
	local details = Detail(ability)
	return details and details.costEnergy
end
ScriptEnv.AbilityCostEnergy = AbilityCostEnergy

---costMana:	The amount of mana this ability consumes on use.
local function AbilityCostMana(ability)
	local details = Detail(ability)
	return details and details.costMana
end
ScriptEnv.AbilityCostMana = AbilityCostMana

---costPlanarCharge:	The amount of planar charges this ability consumes on use.
local function AbilityCostPlanarCharge(ability)
	local details = Detail(ability)
	return details and details.costPlanarCharge
end
ScriptEnv.AbilityCostPlanarCharge = AbilityCostPlanarCharge

---costPower:	The amount of power this ability consumes on use.
local function AbilityCostPower(ability)
	local details = Detail(ability)
	return details and details.costPower
end
ScriptEnv.AbilityCostPower = AbilityCostPower

---currentCooldownBegin:	The time the current cooldown started, in the context of Inspect.Time.Frame.
local function AbilityCooldownBegin(ability)
	local details = Detail(ability)
	return details and details.cooldownBegin
end
ScriptEnv.AbilityCooldownBegin = AbilityCooldownBegin

---currentCooldownDuration:	Duration of the current cooldown the ability is influenced by, in seconds.
local function AbilityCooldown(ability)
	local details = Detail(ability)
	return details and details.cooldown
end
ScriptEnv.AbilityCooldown = AbilityCooldown

---currentCooldownExpired:	Number of seconds the current cooldown is past its expiration time. Generally indicates lag.
local function AbilityCurrentCooldownExpired(ability)
	local details = Detail(ability)
	return details and details.currentCooldownExpired
end
ScriptEnv.AbilityCurrentCooldownExpired = AbilityCurrentCooldownExpired

---currentCooldownRemaining:	Time remaining in the ability's current cooldown, in seconds.
local function AbilityCurrentCooldownRemaining(ability)
	local details = Detail(ability)
	return details and details.currentCooldownRemaining
end
ScriptEnv.AbilityCurrentCooldownRemaining = AbilityCurrentCooldownRemaining

---description:	Description for the ability.
local function AbilityDescription(ability)
	local details = Detail(ability)
	return details and details.description
end
ScriptEnv.AbilityDescription = AbilityDescription

---gainCharge:	Amount of charge gained by using the ability.
local function AbilityGainCharge(ability)
	local details = Detail(ability)
	return details and details.gainCharge
end
ScriptEnv.AbilityGainCharge = AbilityGainCharge

---icon:	Resource filename of the ability's icon.
local function AbilityIcon(ability)
	local details = Detail(ability)
	return details and details.icon
end
ScriptEnv.AbilityIcon = AbilityIcon

---name:	Name of the ability.
local function AbilityName(ability)
	local details = Detail(ability)
	return details and details.name
end
ScriptEnv.AbilityName = AbilityName

---outOfRange:	Signals that the ability is out of range.
local function AbilityoutOfRange(ability)
	local details = Detail(ability)
	return details and details.outOfRange
end
ScriptEnv.AbilityOutOfRange = AbilityOutOfRange

---passive:	Signals that the ability is passive.
local function AbilityPassive(ability)
	local details = Detail(ability)
	return details and details.passive
end
ScriptEnv.AbilityPassive = AbilityPassive

---positioned:	Signals that the ability's effect is manually positioned by the user.
local function AbilityPositioned(ability)
	local details = Detail(ability)
	return details and details.positioned
end
ScriptEnv.AbilityPositioned = AbilityPositioned

---racial:	Signals that the ability is a racial ability.
local function AbilityRacial(ability)
	local details = Detail(ability)
	return details and details.racial
end
ScriptEnv.AbilityRacial = AbilityRacial

---rangeMax:	The maximum range of the ability.
local function AbilityRangeMax(ability)
	local details = Detail(ability)
	return details and details.rangeMax
end
ScriptEnv.AbilityRangeMax = AbilityRangeMax

---rangeMin:	The minimum range of the ability.
local function AbilityRangeMin(ability)
	local details = Detail(ability)
	return details and details.rangeMin
end
ScriptEnv.AbilityRangeMin = AbilityRangeMin

---stealthRequired:	Signals that the ability requires the user to be in stealth.
local function AbilityStealthRequired(ability)
	local details = Detail(ability)
	return details and details.stealthRequired
end
ScriptEnv.AbilityStealthRequired = AbilityStealthRequired

---target:	The Unit ID of the unit that this ability will be used on if triggered at this moment.
local function AbilityTarget(ability)
	local details = Detail(ability)
	return details and details.target
end
ScriptEnv.AbilityTarget = AbilityTarget

---unusable:	Signals that this ability is unusable.
local function AbilityUnusable(ability)
	local details = Detail(ability)
	return details and details.Unusuable
end
ScriptEnv.AbilityUnusuable = AbilityUnusuable

---weapon:	The required equipped weapon for this ability. May be "any", "melee", or "ranged".
local function AbilityWeapon(ability)
	local details = Detail(ability)
	return details and details.weapon
end
ScriptEnv.AbilityWeapon = AbilityWeapon


