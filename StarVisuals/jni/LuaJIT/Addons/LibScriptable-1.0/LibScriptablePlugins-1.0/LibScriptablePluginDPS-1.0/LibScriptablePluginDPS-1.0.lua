local MAJOR = "LibScriptablePluginDPS-1.0"
local MINOR = 24
local PluginDPS = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginDPS then return end

local _G = _G
local data = {}
local update
local MAXRECORDS = 10

local ScriptEnv = {}

-- Populate an environment with this plugin's fields
-- @usage :New(environment)
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment, and the plugin object as second return
function PluginDPS:New(environment)

	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end

	return environment
end

local GetTime = Inspect.Time.Real

-- Return the unit's DPS score.
-- @usage UnitDPS(unit)
-- @param unit The said unit, i.e mouseover, raid1
-- @return The unit's DPS
function PluginDPS.UnitDPS(unit)
	local id = Inspect.Unit.Lookup(unit) or "environment"
	if not data[id] then return end
	local dps = data[id].damage / (GetTime() - data[id].startTime)
	return string.format("%.1f", dps), dps
end
ScriptEnv.UnitDPS = PluginDPS.UnitDPS

-- Clear all DPS data for a unit
-- @usage ResetDPS(unit)
-- @param unit The said unit, i.e. mouseover, raid1
function PluginDPS.ResetDPS(unit)
	local id = Inspect.Unit.Lookup(unit) or "environment"
	if not data[id] then return end
	data[id] = nil
end
ScriptEnv.ResetDPS = PluginDPS.ResetDPS

-- Wipe all DPS data
function PluginDPS.WipeDPS()
end
ScriptEnv.WipeDPS = PluginDPS.WipeDPS

-- Start DPS data collection
-- @usage StartDPS()
function PluginDPS.StartDPS()
end
ScriptEnv.StartDPS = PluginDPS.StartDPS

-- Stop DPS data collection
-- @usage StopDPS()
function PluginDPS.StopDPS()
end
ScriptEnv.StopDPS = PluginDPS.StopDPS

function PluginDPS.GetData()
	return data
end
ScriptEnv.GetData = PluginDPS.GetData

table.insert(Event.Combat.Damage, {function(info)
	info.caster = info.caster or "environment"
	data[info.caster] = data[info.caster] or {startTime=GetTime()}
	data[info.caster].damage = (data[info.caster].damage or 0) + (info.damage or 0)
	data[info.caster].lastUpdate = GetTime()
	
end, "LibScriptablePluginDPS_1_0", "refresh"})