local MAJOR = "LibScriptablePluginBuff-1.0" 
local MINOR = 24

local PluginBuff = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginBuff then return end
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0")

if not PluginBuff.__index then
	PluginBuff.__index = PluginBuff
end

local Detail = Inspect.Buff.Detail
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
function PluginBuff:New(environment)
	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end
	return environment
end



---ability:	The ID of the ability that created this buff. Not guaranteed to exist.
local function BuffAbility(buff)
	local details = Detail(buff)
	return details and details.ability
end
ScriptEnv.BuffAbility = BuffAbility

---begin:	The time the buff started, in the context of Inspect.Time.Frame.
local function BuffBegin(buff)
	local details = Detail(buff)
	return details and details.begin
end
ScriptEnv.BuffBegin = BuffBegin

---caster:	Unit ID of the buff's caster.
local function BuffCaster(buff)
	local details = Detail(buff)
	return details and details.caster
end
ScriptEnv.BuffCaster = BuffCaster

---curse:	Signals that the buff is a curse.
local function BuffCurse(buff)
	local details = Detail(buff)
	return details and details.curse
end
ScriptEnv.BuffCurse = BuffCurse

---debuff:	Signals that the buff is a debuff. If this is missing, then it's an actual buff.
local function BuffDebuff(buff)
	local details = Detail(buff)
	return details and details.debuff
end
ScriptEnv.BuffDebuff = BuffDebuff

---description:	Description for the buff.
local function BuffDescription(buff)
	local details = Detail(buff)
	return details and details.description
end
ScriptEnv.BuffDescription = BuffDescription

---disease:	Signals that the buff is a disease.
local function BuffDisease(buff)
	local details = Detail(buff)
	return details and details.disease
end
ScriptEnv.BuffDisease = BuffDisease

---duration:	Duration of the buff in seconds.
local function BuffDuration(buff)
	local details = Detail(buff)
	return details and details.duration
end
ScriptEnv.BuffDuration = BuffDuration

---expired:	Number of seconds the buff is past its expiration time. Generally indicates lag.
local function BuffExpired(buff)
	local details = Detail(buff)
	return details and details.expired
end
ScriptEnv.BuffExpired = BuffExpired

---icon:	Resource filename of the buff's icon.
local function BuffIcon(buff)
	local details = Detail(buff)
	return details and details.icon
end
ScriptEnv.BuffIcon = BuffIcon

---name:	Name of the buff.
local function BuffName(buff)
	local details = Detail(buff)
	return details and details.name
end
ScriptEnv.BuffName = BuffName

---noncancelable:	Signals that the buff cannot be voluntarily canceled. Does not show up for debuffs.
local function BuffNoncancelable(buff)
	local details = Detail(buff)
	return details and details.noncancelable
end
ScriptEnv.BuffNoncancelable = BuffNoncancelable

---poison:	Signals that the buff is a poison.
local function BuffPoison(buff)
	local details = Detail(buff)
	return details and details.poison
end
ScriptEnv.BuffPoison = BuffPoison

---remaining:	Time remaining on the buff, in seconds.
local function BuffRemaining(buff)
	local details = Detail(buff)
	return details and details.remaining
end
ScriptEnv.BuffRemaining = BuffRemaining

---rune:	If this buff is created by a rune, the ID of the rune causing it.
local function BuffRune(buff)
	local details = Detail(buff)
	return details and details.rune
end
ScriptEnv.BuffRune = BuffRune

---stack:	Number of stacks on the buff.
local function BuffStack(buff)
	local details = Detail(buff)
	return details and details.stack
end
ScriptEnv.BuffStack = BuffStack


