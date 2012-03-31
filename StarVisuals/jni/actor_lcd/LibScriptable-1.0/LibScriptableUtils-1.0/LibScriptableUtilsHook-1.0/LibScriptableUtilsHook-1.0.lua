local MAJOR = "LibScriptableUtilsHook-1.0"
local MINOR = 24

local LibHook = LibStub:NewLibrary(MAJOR, MINOR)
if not LibHook then return end

local hooks = {}

--- Create a hook
-- @param obj The object to hook
-- @param method The object's method to hook
-- @param target This is your handler function
-- @return Nothing
function LibHook:CreateHook(obj, method, target)
	assert(type(obj) == "table", "CreateHook requires an object to hook")
	assert(type(method) == "string", "CreateHook requires a string representation of the method to hook")
	assert(type(target) == "table", "CreateHook requires a target to hook")
	assert(type(target[method]) == "function", "Target method is not a function")

	hooks[obj] = hooks[obj] or {}
	
	hooks[obj][method] = target
	obj[method] = function(...) 
		return hooks[obj][method](...) 
	end
end
