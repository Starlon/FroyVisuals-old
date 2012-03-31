local MAJOR = "LibScriptablePluginBit-1.0" 
local MINOR = 24

local PluginBit = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginBit then return end

local _G = _G

if not PluginBit.__index then
	PluginBit.__index = PluginBit
end

-- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginBit:New(environment)

	environment.arshift = bit.arshift
	environment.band = bit.band
	environment.bnot = bit.bnot
	environment.bor = bit.bor
	environment.bswap = bit.bswap
	environment.bxor = bit.bxor
	environment.lshift = bit.lshift
	environment.rol = bit.rol
	environment.ror = bit.ror
	environment.rshift = bit.rshift
	environment.tobit = bit.tobit
	environment.tohex = bit.tohex
	
	return environment
	
end
