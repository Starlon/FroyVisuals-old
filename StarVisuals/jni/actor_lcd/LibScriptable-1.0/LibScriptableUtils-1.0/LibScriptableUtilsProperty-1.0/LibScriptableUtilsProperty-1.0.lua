
local MAJOR = "LibScriptableUtilsProperty-1.0" 
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub") 
local LibProperty = LibStub:NewLibrary(MAJOR, MINOR)
if not LibProperty then return end

local Evaluator = LibStub("LibScriptableUtilsEvaluator-1.0")
assert(Evaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")

local errorHandler
assert(LibStub("LibScriptableUtilsError-1.0"), MAJOR .. " requires LibScriptableUtilsError-1.0")

local pool = setmetatable({},{__mode='k'})

if not LibProperty or not Evaluator then return end

if not LibProperty.pool then
	LibProperty.__index = LibProperty
end

--- Create a new LibScriptableProperty object
-- @usage LibProperty:New(widget, visitor, name, expression, defval, errorLevel)
-- @param visitor A LibScriptableCore object
-- @param widget The parent widget
-- @param visitor A LibCore object, or provide your own.
-- @param name Give your property a name. 
-- @param expression The lua script for this property.
-- @param defval The default value for this property.
-- @return A new LibScriptableProperty object
function LibProperty:New(widget, visitor, name, expression, defval, errorLevel)
	assert(visitor.environment, "Unable to create new property. Requires a visitor.environment")
	assert(name, "Unable to create new property. Requires a name")
	
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
	end

	setmetatable(obj, self)

	obj.visitor = visitor
	obj.name = name
	obj.environment = visitor.environment
	obj.widget = widget
	obj.expression = expression
	obj.defval = defval
	obj.errorLevel = errorLevel

	obj.is_valid = type(expression) == "string" and true

	obj.error = LibStub("LibScriptableUtilsError-1.0"):New(MAJOR, errorLevel)
	
	return obj	
end

--- Delete a property
-- @usage LibProperty:Del()
-- @return Nothing
function LibProperty:Del()
	pool[self] = true
	self.error:Del()
end

local count = 0
--- Evaluate a property's code
-- @usage object:Eval()
-- @return Nothing
function LibProperty:Eval(unit)
	if not self.is_valid then return 0 end
	
	local update = 1

	self.environment.unit = self.widget.unitOverride or self.widget.unit or self.environment.unit

	local old = self.ret1

	self.environment.self = self.widget
	self.ret1, self.ret2, self.ret3, self.ret4, self.ret5 = Evaluator.ExecuteCode(self.environment, self.name, self.expression, false, self.defval)
	
	if old == self.ret1 then
		update = 0
	end
	
	return update
end

--- Return the property's value as a number
-- @usage object:P2N()
-- @return The property as a number value
function LibProperty:P2N()
	if not self.is_valid then return self.defval end
	if type(self.ret1) == "string" then
		return tonumber(self.ret1), tonumber(self.ret2), tonumber(self.ret3), tonumber(self.ret4), tonumber(self.ret5)
	elseif type(self.ret1) == "number" then
		return self.ret1, self.ret2, self.ret3, self.ret4, self.ret5
	end
end

--- Return the property's value as a string
-- @usage object:P2S()
-- @return The property as a string value
function LibProperty:P2S()
	if not self.is_valid then return self.defval end
	
	if type(self.ret1) ~= "number" and type(self.ret1) ~= "string" then
		return ""
	end
	
	
	if type(self.ret1) == "number" then
		return tostring(self.ret1) and tostring(self.ret1), self.ret2 and tostring(self.ret2), self.ret3 and tostring(self.ret3), self.ret4 and tostring(self.ret4), self.ret5 and tostring(self.ret5)
	elseif type(self.ret1) == "string" then
		return self.ret1, self.ret2, self.ret3, self.ret4, self.ret5
	end
end
