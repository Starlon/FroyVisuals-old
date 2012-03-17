local MAJOR = "LibScriptablePluginRift-1.0"
local MINOR = 24
local PluginRift = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginRift then return end

local _G = _G

-- Populate an environment with this plugin's fields
-- @usage :New(environment)
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment, and the plugin object as second return
function PluginRift:New(environment)

	environment.Inspect = _G.Inspect
	environment.Event = _G.Event
	environment.Command = _G.Command
	environment.Utility = _G.Utility
	environment.UI = _G.UI
	
	return environment
end

