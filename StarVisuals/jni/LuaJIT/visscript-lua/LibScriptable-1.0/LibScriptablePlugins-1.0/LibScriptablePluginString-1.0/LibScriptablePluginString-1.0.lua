local MAJOR = "LibScriptablePluginString-1.0" 
local MINOR = 24

local PluginString = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginString then return end

local _G = _G

if not PluginString.__index then
	PluginString.__index = PluginString
end

--- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @param environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginString:New(environment)
	
	environment.string = _G.string
	environment.byte = string.byte
	environment.char = string.char
	environment.find = string.find
	environment.format = string.format
	environment.gfind = string.gfind
	environment.gmatch = string.gmatch
	environment.gsub = string.gsub
	environment.len = string.len
	environment.lower = string.lower
	environment.match = string.match
	environment.rep = string.rep
	environment.reverse = string.reverse
	environment.sub = string.sub
	environment.upper = string.upper
	
	environment.short = function(value)
		assert(type(value) == "number", MAJOR .. ".short requires a number")
		if value >= 10000000 or value <= -10000000 then
			value = ("%.1fm"):format(value / 1000000)
		elseif value >= 1000000 or value <= -1000000 then
			value = ("%.2fm"):format(value / 1000000)
		elseif value >= 100000 or value <= -100000 then
			value = ("%.0fk"):format(value / 1000)
		elseif value >= 1000 or value <= -1000 then
			value = ("%.1fk"):format(value / 1000)
		else
			value = tostring(floor(value+0.5))
		end
		return value
	end
	
	environment.memshort = function(value)
		assert(type(value) == "number", MAJOR .. ".memshort requires a number")
		if value <= 1024 then
			value = ("%.3fKb"):format(value)
		elseif value <= 1024 * 1024 then
			value = ("%.3fMb"):format(value / 1024)
		elseif value <= 1024 * 1024 * 1024 then
			value = ("%.3fGb"):format(value / ( 1024 * 1024) )
		elseif value <= 1024 * 1024 * 1024 * 1024 then
			value = ("%.3fTb"):format(value / (1024 * 1024 * 1024))
		end
		return value
	end
	
	environment.timeshort = function(value)
		assert(type(value) == "number", MAJOR .. ".timeshort requires a number")
		if value < 1000 then
			value = ("%.3fms"):format(value)
		elseif value / 1000 < 60 then
			value = ("%.3fs"):format(value / 1000)
		elseif value / 1000 / 60 < 60 then
			value = ("%.3fm"):format(value / 1000 / 60)
		elseif value / 1000 / 60 / 60 < 60 then
			value = ("%.3fh"):format(value / 1000 / 60 / 60)
		end
		return value
	end

    environment.split = function(txt, delim)
        local tbl = {}
	for i = 1, string.len(txt) do
		local remaining = string.sub(txt, i) 
                if i == string.len(txt) then
                    buf = remaining
                else
                    buf = string.sub(txt, 1, -string.len(remaining))
                end
                table.insert(tbl, buf)
        end
        return tbl
    end


	
	return environment
end

