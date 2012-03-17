local MAJOR = "LibScriptableWidgetColor-1.0"
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub")
local WidgetColor = LibStub:NewLibrary(MAJOR, MINOR)
if not WidgetColor then return end
local LibWidget = LibStub("LibScriptableWidget-1.0", true)
assert(LibWidget, MAJOR .. " requires LibScriptableWidget-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local Evaluator = LibStub("LibScriptableUtilsEvaluator-1.0", true)
assert(Evaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
local PluginUtils = LibStub("LibScriptablePluginUtils-1.0", true)
assert(PluginUtils, MAJOR .. " requires LibScriptablePluginUtils-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L


if not WidgetColor.__index then WidgetColor.__index = WidgetColor end


local defaults = {
	update = 500,
	repeating = true
}
WidgetColor.defaults = defaults


local widgetType = {gesture=true}

local pool = setmetatable({}, {__mode = "v"})
local function new(...)
	local obj = table.remove(pool) or {}
	for i = 1, select("#", ...) do
		obj[i] = select(i, ...)
	end
	return obj
end

local function newDict(...)
	local obj = table.remove(pool) or {}
	for i = 1, select("#", ...), 2 do
		local key = select(i, ...)
		local val = select(i + 1, ...)
		obj[key] = val
	end
	return obj
end

local function del(obj)
	wipe(obj)
	tinsert(pool, obj)
end

local pool = setmetatable({}, {__mode = "k"})

local stopFunc

--- Create a new LibScriptableWidgetColor object
-- @usage WidgetColor:New(visitor, name, config, errorLevel)
-- @param visitor An LibScriptableCore-1.0 object, or provide your own
-- @param name A name for the timer widget
-- @param config This timeer's parameters
-- @param errorLevel The errorLevel for this object
-- @param callback An optional callback function to be executed when the gesture is performed.
-- @param timer An optional timer. This should have a :Start() and :Stop().
-- @return A new LibScriptableWidgetColor widget
function WidgetColor:New(visitor, name, config, errorLevel, callback, timer)
	assert(name, "WidgetColor requires a name.")
	assert(config, "Please provide the timer with a config")
	assert(config.expression, name .. ": Please provide the timer with an expression")

	local obj = next(pool)

	if obj then
		pool[obj] = nil
		obj.__index = nil
	else
		obj = {}
		obj.options = {}
	end

	setmetatable(obj, self)

	obj.widget = LibWidget:New(obj, visitor, MAJOR .. " " .. name, config, 0, 0, 0, widgetType, errorLevel)

	obj.expression = config.expression

	obj.update = config.update or defaults.update
	
	obj.draw = callback
	
	obj.repeating = config.repeating or defaults.repeating

	obj.timer = timer or LibTimer:New("WidgetColor.timer " .. obj.name, obj.update, obj.repeating, self.Update, obj, obj.errorLevel)

	return obj
end


--- Delete a LibScriptableWidgetColor object
-- @usage object:Del()
-- @return Nothing
function WidgetColor:Del()
	self:Stop()
	do return end
	self.widget:Del()
	self.timer:Del()
	pool[self] = true
end

--- Start a LibScriptableWidgetColor
-- @usage object:Start()
-- @return Nothing
function WidgetColor:Start()
	if self.update > 0 then
		self.timer:Start()
		self.active = true
	end
	self:Update()
end

--- Stop a LibScriptableWidgetColor
-- @usage object:Stop()
-- @return Nothing
function WidgetColor:Stop()
	self.timer:Stop()
	self.active = false
end

--- Update this widget
-- @usage object:Update()
-- @return Nothing
function WidgetColor:Update()

	self.environment.self = self
	local r, g, b, a = Evaluator.ExecuteCode(self.environment, MAJOR .. " Lua script", self.expression)
	--self.environment.self = nil

	if type(r) ~= "number" then return end

	if r and r > 1 then
		r = r / 255
	end

	if g and g > 1 then
		g = g / 255
	end

	if b and b > 1 then
		b = b / 255
	end

	if a and a > 1 then
		a = a / 255
	end

	if not g then
		if r > bit.bor(bit.bor(0xff, 0xff), 0xff) then
			self.r, self.g, self.b, self.a = PluginUtils.Color2RGBA(r, true)
		end
	else
		self.r, self.g, self.b, self.a = r, g, b, a or 1
	end
	
	if type(self.draw) == "function" then
		self.draw(self)
	end
end

--- Get an Ace3 option table. Plug this into a group type's args.
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table -- `name.args = options`.
function WidgetColor:GetOptions(db, callback, data)
	local defaults = WidgetColor.defaults
	local options = {
		enabled = {
			name = L["Enabled"],
			desc = L["Whether this timer is enabled or not"],
			type = "toggle",
			get = function() return db.enabled end,
			set = function(info, v)
				db.enabled = v
				db.enabledDirty = true
				if type(callback) == "function" then callback(data) end
			end,
			order = 5
		},
		update = {
			name = L["Update Rate"],
			desc = L["Enter the timer's refresh rate"],
			type = "input",
			pattern = "%d",
			get = function()
				return tostring(db.update or defaults.update)
			end,
			set = function(info, v)
				db.update = tonumber(v)
				db.updateDirty = true
				if type(callback) == "function" then callback(data) end
			end,
			order = 6
		},
		repeating = {
			name = L["Repeating Timer"],
			desc = L["Whether the timer associated with this widget repeats or not"],
			type = "toggle",
			get = function()
				return db.repeating
			end,
			set = function(info, val)
				db.repeating = val
				db.repeatingDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 7
		},
		expression = {
			name = L["Expression"],
			desc = L["This widget's Lua script"],
			type = "input",
			multiline = true,
			width = "full",
			get = function()
				return db.expression
			end,
			set = function(info, val)
				db.expression = val
				db.expressionDirty = val
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 7
		}
	}
	return options
end
