local MAJOR = "LibScriptableUtilsError-1.0"
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub")
local LibScriptableError = LibStub:NewLibrary(MAJOR, MINOR)
if not LibScriptableError then return end
local pool = setmetatable({}, {__mode = "k"})
LibScriptableError.defaultTexts = {"Quiet",  "Normal", "Verbose"}

local DEFAULT_LEVEL = 2
-- 3 verbose
-- 2 warning
-- 1 quiet

if not LibScriptableError.__index then
	LibScriptableError.__index = LibScriptableError
	LibScriptableError.name = MAJOR
	LibScriptableError.errorLevel = DEFAULT_LEVEL
end

--- Create a new LibScriptableError object
-- @usage :New(name, [handler])
-- @param name Give your error object a name
-- @param errorLevel Critical level for this object. This is optional. Default is 2.
-- @param handler An optional handler function, otherwise we use ChatFrame1 for printing
-- @return A new LibScriptableError object
function LibScriptableError:New(name, errorLevel, handler)
	assert(name, "LibScriptableError.New requires a name")
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
	end

	setmetatable(obj, self)

	obj.name = name
	obj.errorLevel = errorLevel or DEFAULT_LEVEL

	if type(handler) == "function" then
		obj.handler = handler
	end

	return obj
end

--- Delete a LibScriptableError object
-- @usage :Del()
-- @return Nothing
function LibScriptableError:Del(err)
	if not err then err = self end
	pool[err] = true
end

--- Print something
-- @usage LibError:Print(text, errorLevel)
-- @param text The text to print.
-- @param errorLevel The critical threshhold for this message
-- @return Nothing
function LibScriptableError:Print(text, errorLevel)
	assert(self, "LibScriptableError failed printing due to invalid 'self'")
	assert(self.name and self.name ~= "", "LibScriptableError object has no name")
	assert(text, "LibScriptableError failed printing due to nil text (" .. self.name .. ")")

	if type(errorLevel) ~= "number" then
		errorLevel = DEFAULT_LEVEL
	end

	if self.handler then
		self.handler(text, errorLevel)
		return
	end

	local color
	if errorLevel == 3 then
		color = "|cffff0000"
	elseif errorLevel == 2 then
		color = "|cffffff00"
	elseif errorLevel == 1 then
		color = "|cff00ff00"
	else
		color = "|cffffffff"
	end

	if self.errorLevel >= errorLevel then
		print(("(%s)"):format(text))
	end
end
