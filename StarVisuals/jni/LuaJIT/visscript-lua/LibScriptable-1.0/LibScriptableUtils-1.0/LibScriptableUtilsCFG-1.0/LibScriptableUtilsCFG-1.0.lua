local MAJOR = "LibScriptableUtilsCFG-1.0" 
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub") 
LibError = LibStub("LibScriptableUtilsError-1.0")
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local LibCFG = LibStub:NewLibrary(MAJOR, MINOR)
if not LibCFG then return end
local evaluator = LibStub("LibScriptableUtilsEvaluator-1.0")
assert(evaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")

local pool = setmetatable({}, {__mode = "k"})

if not LibCFG.__index then
	LibCFG.__index = LibCFG
end

local TRANSITION_RIGHT = 0
local TRANSITION_LEFT = 1
local TRANSITION_BOTH = 2
local TRANSITION_UP = 3
local TRANSITION_DOWN = 4
local TRANSITION_TENTACLE = 5
local TRANSITION_ALPHABLEND = 6


-- @name LibScriptableCFG:New
-- @usage LibScriptableCFG:New(duration, repeating, callback, data)
-- @param visitor A LibScriptableCore object
-- @param config The configuration table
-- @param environment The execution environment
-- @return A new LibScriptableCFG object
function LibCFG:New(visitor, config, environment, errorLevel)
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
	end

	setmetatable(obj, self)
			
	obj.visitor = visitor
	obj.environment = environment
	obj.root = config
	obj.error = LibError:New("LibCFG", errorLevel) 
	obj.errorLevel = errorLevel
	
	return obj	
	
end

-- @name LibScriptableCFG:Del
-- @usage LibScriptableCFG:Del([core]) or object:Del()
-- @param core An optional core object
-- @return Nothing
function LibCFG:Del(core)
	if core and type(core) == "table" then
		pool[core] = true
	else
		pool[self] = true
	end
	obj.error:Del()
end

function LibCFG:FetchRaw(section, key, defval)

	if not section then self.error:Print("FetchRaw: No section"); return end
	if not key then self.error:Print("FetchRaw: No key"); return end
	
    local pos = type(key) == "string" and key:find("%.");
	
    if( pos and pos > 1 ) then
        local subSection = self.visitor.CFG:FetchRaw(section, key:sub(1, pos - 1));
        if(not subSection) then
            return defval;
		end
        return self:FetchRaw(subSection, key:sub(pos + 1), defval);
    end
	
	local val = section[key] or defval

    return val
end

function LibCFG:Fetch(section, key, defval)

	local val = self:FetchRaw(section, key, defval)

    if(not val) then
        return defval
	end;

    if( type(val) == "number") then
        return val;
    elseif ( type(val) == "string" ) then
		return LibEvaluator.ExecuteCode(self.environment, "fetch", val, false, defval)
    end
	
    return defval;
end
	