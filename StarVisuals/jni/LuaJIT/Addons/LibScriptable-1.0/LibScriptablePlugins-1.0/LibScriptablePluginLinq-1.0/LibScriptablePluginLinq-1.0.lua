local MAJOR = "LibScriptablePluginLinq-1.0" 
local MINOR = 24

local PluginLinq = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginLinq then return end
local LibLinq = LibStub("LibLinq-1.0")

-- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginLinq:New(environment)
	environment.Linq = LibLinq
	
	return environment
end
