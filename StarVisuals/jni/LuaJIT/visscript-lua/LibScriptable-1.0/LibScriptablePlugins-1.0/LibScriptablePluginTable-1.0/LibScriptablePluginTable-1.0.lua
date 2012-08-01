local MAJOR = "LibScriptablePluginTable-1.0" 
local MINOR = 24

local PluginTable = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginTable then return end

local _G = _G

if not PluginTable.__index then
	PluginTable.__index = PluginTable
end

--- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @param environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginTable:New(environment)
	
	environment.table = _G.table
	environment.setmetatable = _G.setmetatable
	environment.foreach = _G.foreach
	environment.foreachi = _G.foreachi
	environment.getn = _G.getn
	environment.ipairs = _G.ipairs
	environment.pairs = _G.pairs
	environment.sort = _G.table.sort
	environment.tContains = _G.tContains
	environment.tinsert = _G.table.insert
	environment.tremove = _G.table.remove
	environment.wipe = _G.table.wipe
	
	return environment
end
