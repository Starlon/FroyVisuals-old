local MAJOR = "LibScriptableWidgetTimer-1.0" 
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub") 
local WidgetTimer = LibStub:NewLibrary(MAJOR, MINOR)
if not WidgetTimer then return end
local LibWidget = LibStub("LibScriptableWidget-1.0", true)
assert(LibWidget, MAJOR .. " requires LibScriptableWidget-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local Evaluator = LibStub("LibScriptableUtilsEvaluator-1.0", true)
assert(Evaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

local pool = setmetatable({}, {__mode = "k"})

if not WidgetTimer.__index then WidgetTimer.__index = WidgetTimer end

WidgetTimer.defaults = {
	expression = 'return',
	repeating = false,
	update = 0,
}

local widgetType = {timer=true}

--- Create a new LibScriptableWidgetTimer object
-- @usage WidgetTimer:New(visitor, name, config, errorLevel)
-- @param visitor An LibScriptableCore-1.0 object, or provide your own
-- @param name A name for the timer widget
-- @param config This timeer's parameters
-- @param errorLevel The errorLevel for this object
-- @param An optional timer. This should have a :Start() and :Stop().
-- @return A new LibScriptableWidgetTimer widget
function WidgetTimer:New(visitor, name, config, errorLevel, timer) 
	assert(name, "WidgetTimer requires a name.")
	assert(config, "Please provide the timer with a config")
	assert(config.expression, name .. ": Please provide the timer with an expression")
	
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
		obj.options = {}
	end
		
	setmetatable(obj, self)

	obj.widget = LibWidget:New(obj, visitor, name, config, 0, 0, 0, widgetType, errorLevel)
	
	obj.expression = config.expression
	obj.repeating = config.repeating
	obj.update = config.update or WidgetTimer.defaults.update
	obj.enabled = config.enabled
		
	obj.error = LibError:New(MAJOR .. ": " .. name, errorLevel)
	
	obj.timer = timer or LibTimer:New("WidgetTimer.timer " .. obj.name, obj.update, obj.repeating, self.Update, obj, obj.errorLevel)
	
	return obj	
end


--- Delete a LibScriptableWidgetTimer object
-- @usage :Del()
-- @return Nothing
function WidgetTimer:Del()
	local timer = self
	timer:Stop()
	timer.widget:Del()
	timer.timer:Del()
	pool[timer] = true
end

--- Start a LibScriptableWidgetTimer
-- @usage :Start()
-- @return Nothing
function WidgetTimer:Start()
	if self.enabled and not self.active then
		self.timer:Start()
		self.active = true
		self:Update()
	end
end

--- Stop a LibScriptableWidgetTimer
-- @usage :Stop()
-- @return Nothing
function WidgetTimer:Stop()
	self.timer:Stop()
	self.active = false
end

--- Update data. This is called by the timer, or else call it yourself.
-- @usage :Update()
-- @return Nothing
function WidgetTimer:Update()
	if not self.active then return end
	self.visitor.environment.self = self
	Evaluator.ExecuteCode(self.visitor.environment, self.name, self.expression)
end

--- Retrieve an Ace3 options table for the given widget
-- @usage :GetOptions(db, callback, data)
-- @return A new Ace3 options table populated with a widget's data
function WidgetTimer:GetOptions(db, callback, data)
		local defaults = WidgetTimer.defaults
		local options = {
			enabled = {
				name = L["Enabled"],
				desc = L["Whether this timer is enabled or not"],
				type = "toggle",
				get = function() return db.enabled end,
				set = function(info, v) 
					db.enabled = v
					if type(callback) == "function" then
						callback(data)
					end
					db["enabledDirty"] = true
				end,
				order = 2
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
					if type(callback) == "function" then
						callback(data)
					end
					db["updateDirty"] = true
				end,
				order = 3
			},
			repeating = {
				name = L["Repeating"],
				desc = L["Toggle whether to repeat this timer"],
				type = "toggle",
				get = function()
					return db.repeating or defaults.repeating
				end,
				set = function(info, v)
					db.repeating = v
					if type(callback) == "function" then
						callback(data)
					end
					db["repeatingDirty"] = true
				end,
				order = 4
			},
			expression = {
				name = L["Expression"],
				desc = L["Enter this widget's expression"],
				type = "input",
				width = "full",
				multiline = true,
				get = function()
					return db.expression or defaults.expression
				end,
				set = function(info, v)
					db.expression = v
					if type(callback) == "function" then
						callback(data)
					end
					db["expressionDirty"] = true
				end,
				order = 5
			},
			
		}
	return options
end
