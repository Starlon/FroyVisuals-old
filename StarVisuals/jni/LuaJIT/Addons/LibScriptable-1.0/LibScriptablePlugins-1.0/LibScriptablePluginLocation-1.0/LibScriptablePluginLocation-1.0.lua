local MAJOR = "LibScriptablePluginLocation-1.0" 
local MINOR = 24

local PluginLocation = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginLocation then return end

local _G = _G

if not PluginLocation.__index then
	PluginLocation.__index = PluginLocation
end

-- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginLocation:New(environment)
		
    environment.GetMinimapZoneText = _G.GetMinimapZoneText -- GetMinimapZoneText() - Returns the zone text, that is displayed over the minimap. 
    environment.GetRealZoneText = _G.GetRealZoneText -- GetRealZoneText() - Returns either instance name or zone name 
    environment.GetSubZoneText = _G.GetSubZoneTExt -- GetSubZoneText() - Returns the subzone text (e.g. "The Canals"). 
    environment.GetZonePVPInfo = _G.GetZonePVPInfo -- GetZonePVPInfo() - Returns PVP info for the current zone. 
    environment.GetZoneText = _G.GetZoneText -- GetZoneText() - Returns the zone text (e.g. "Stormwind City"). 
	
	return environment
end
