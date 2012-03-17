local MAJOR = "LibScriptablePluginLua-1.0" 
local MINOR = 24

local PluginLua = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginLua then return end

local _G = _G

if not PluginLua.__index then
	PluginLua.__index = PluginLua
end

-- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginLua:New(environment)
	
	-- Lua functions
	environment._G = _G
	environment.assert = assert
	environment.collectgarbage = collectgarbage
	environment.coroutine = coroutine
	environment.debug = debug
	environment.dump = dump
	environment.error = error
	environment.gcinfo = gcinfo
	environment.getfenv = getfenv
	environment.getmetatable = getmetatable
	environment.ipairs = ipairs
	environment.load = load
	environment.loadstring = loadstring
	environment.newproxy = newproxy
	environment.next = next
	environment.pairs = pairs
	environment.pcall = pcall
	environment.print = print
	environment.print_raw = print_raw
	environment.rawequal = rawequal
	environment.rawget = rawget
	environment.rawset = rawset
	environment.select = select
	environment.setfenv = setfenv
	environment.setmetatable = setmetatable
	environment.tonumber = tonumber
	environment.tostring = tostring
	environment.type = type
	environment.unpack = unpack
	environment.xpcall = xpcall
	
	return environment
end
