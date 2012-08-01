local MAJOR = "LibScriptableWidgetKey-1.0" 
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub") 
local WidgetKey = LibStub:NewLibrary(MAJOR, MINOR)
if not WidgetKey then return end
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local Evaluator = LibStub("LibScriptableUtilsEvaluator-1.0", true)
assert(Evaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

local pool = setmetatable({}, {__mode = "k"})
local objects = {}

if not WidgetKey.__index then
	WidgetKey.__index = WidgetKey
end

WidgetKey.defaults = {
	expression = 'print("Execute Key:" .. (self.name or "' .. MAJOR ..'"))',
	test = "return true",
	up = false,
	modifier = 1
}
local defaults = WidgetKey.defaults

local widgetType = {key=true}

WidgetKey.modifiers = {"Ctrl", "Alt"}
WidgetKey.modifierTags = {"LCTRL", "RCTRL", "LALT", "RALT", "LSHIFT", "RSHIFT"}

--- Create a new LibScriptableWidgetKey object
-- @usage WidgetKey:New(visitor, name, config, errorLevel)
-- @param visitor An LibScriptableCore-1.0 object
-- @param name A name for your key widget
-- @param config This widget's parameters.
-- @param errorLevel The errorLevel for this object
-- @return A new LibScriptableWidgetKey object
function WidgetKey:New(visitor, name, config, errorLevel) 
	assert(name, "WidgetKey requires a name.")
	assert(config, "Please provide the timer with a config")
	assert(config.expression, name .. ": Please provide the marquee with a string")
	
	local obj = next(pool)

	if obj then
		pool[obj] = nil
		obj.__index = nil
	else
		obj = {}
		obj.options = {}
	end
		
	objects[obj] = true

	setmetatable(obj, self)

	obj.widget = LibWidget:New(obj, visitor, name, config, 0, 0, 0, widgetType, errorLevel)
	
	obj.config = config
	obj.expression = config.expression or defaults.expression
	obj.test = config.test or defaults.test
	obj.up = (config.up or defaults.up) and 1 or 0
	obj.modifier = config.modifier or defaults.modifier
	obj.key = {}
	obj.key[WidgetKey.modifierTags[obj.modifier] .. obj.up] = true
	
	obj.error = LibError:New(MAJOR .. ": " .. name, errorLevel)
	
	return obj	
end


--- Delete a LibScriptableWidgetKey object
-- @usage :Del()
-- @return Nothing
function WidgetKey:Del()
	local key = self
	key.widget:Del()
	key.widget = nil
	key:Stop()
	pool[key] = true
	objects[self] = nil
end

--- Start a LibScriptableWidgetKey
-- @usage :Start()
-- @return Nothing
function WidgetKey:Start()
	self.enabled = true
end

--- Stop a LibScriptableWidgetKey
-- @usage :Stop()
-- @return Nothing
function WidgetKey:Stop()
	self.enabled = false
end

--- A key event fired, now execute this widget's code
-- @usagae :KeyEvent(modifier, up)
-- @param modifier Which key, i.e. LCTRL, RALT. Note that RALT and RALT both fire the same event.
-- @param up Whether this is the button release or not
-- @return Nothing
function WidgetKey:KeyEvent(modifier, up)
	if not self.enabled then return end
	if self.key[modifier .. up] then
		if Evaluator.ExecuteCode(self.visitor.environment, self.name, self.test) then
			Evaluator.ExecuteCode(self.visitor.environment, self.name, self.expression)
		end
	end
end

--- Get an Ace3 option table. Plug this into a group type's args.
-- @usage :GetOptions(db, callback, data)
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table -- `name.args = options`.
function WidgetKey:GetOptions(db, callback, data)
		local options = {
			enabled = {
				name = L["Enabled"],
				desc = L["Whether this timer is enabled or not"],
				type = "toggle",
				get = function() return db.enabled end,
				set = function(info, v) 
					db.enabled = v; 
					db.enabledDirty = true 
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 1
			},
			modifier = {
				name = L["Modifier"],
				desc = L["Which modifier"],
				type = "select",
				values = WidgetKey.modifierTags,
				get = function() return db.modifier or defaults.modifier end,
				set = function(info, v)
					db.modifier = v
					db.modifierDirty = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 2
			},
			up = {
				name = L["UP?"],
				desc = L["Execute this widget on the up stroke?"],
				type = "toggle",
				get = function() return db.up or defaults.up end,
				set = function(info, v) 
					db.up = v; 
					db.upDirty = true 
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 3
			},
			test = {
				name = L["Test Case"],
				desc = L["This will be the test case deciding if the expression should execute or not."],
				type = "input",
				width = "full",
				multiline = true,
				get = function()
					return db.test or defaults.test
				end,
				set = function(info, v)
					db.test = v
					db.testDirty = true
					if type(callback) == "function" then
						callback(data)
					end
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
					db.expressionDirty = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 5
			},
			
		}
	return options
end
