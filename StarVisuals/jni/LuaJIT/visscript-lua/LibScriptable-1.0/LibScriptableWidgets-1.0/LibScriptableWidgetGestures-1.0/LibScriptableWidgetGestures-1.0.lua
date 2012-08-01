local MAJOR = "LibScriptableWidgetGestures-1.0" 
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub") 
local WidgetGestures = LibStub:NewLibrary(MAJOR, MINOR)
if not WidgetGestures then return end
local LibWidget = LibStub("LibScriptableWidget-1.0", true)
assert(LibWidget, MAJOR .. " requires LibScriptableWidget-1.0")
local Evaluator = LibStub("LibScriptableUtilsEvaluator-1.0", true)
assert(Evaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
local PluginUtils = LibStub("LibScriptablePluginUtils-1.0", true)
assert(PluginUtils, MAJOR .. " requires LibScriptablePluginUtils-1.0")
local LibMouseGestures = LibStub("LibMouseGestures-1.0", true)
assert(LibMouseGestures, MAJOR .. " requires LibMouseGestures-1.0")
local PluginDoodle = LibStub("LibScriptablePluginDoodlepad-1.0")
assert(PluginDoodle, MAJOR .. " requires LibScriptablePluginDoodlepad-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

PluginDoodle = PluginDoodle:New()

if not WidgetGestures.__index then WidgetGestures.__index = WidgetGestures end

local patternsList = {L["Left"], L["Right"], L["Up"], L["Down"], L["Diagonally left and up"], L["Diagonally right and up"], L["Diagonally right and down"], L["Diagonally left and down"], L["Diagonally left and up"], L["Clockwise"], L["Counter-clockwise"]}
WidgetGestures.patternsList = patternsList
local patternsDict = {["left"] = 1, ["right"] = 2, ["up"] = 3, ["down"] = 4, ["left-up"] = 5, ["right-up"] = 6, ["right-down"] = 7, ["left-down"] = 8, ["left-up"] = 9, ["clockwise"] = 10, ["counterclockwise"] = 11}
WidgetGestures.patternsDict = patternsDict
local patterns = {"left", "right", "up", "down", "left-up", "right-up", "right-down", "left-down", "left-up", "clockwise", "counterclockwise"}
WidgetGestures.patterns = patterns

local typesList = {L["Line"], L["Circle"]}
WidgetGestures.typesList = typesList
local typesDict = {line=1, circle=2}
WidgetGestures.typesDict = typesDict
local types = {"line", "circle"}
WidgetGestures.types = types

local defaults = {
	drawLayer = "UIParent",
	startButton = "Freehand",
	stopButton = "LeftButtonUp",
	nextButton = "",
	cancelButton = "",
	tooltip = "",
	maxGestures = 4,
	minGestures = 1,
	minLength = 300
}

WidgetGestures.defaults = defaults


function LibMouseGestures:Stop()
	self.drawLayer:SetScript("OnMouseDown", nil)
	self.drawLayer:SetScript("OnMouseUp", nil)
	self.drawLayer:SetScript("OnUpdate", nil)
	self.drawLayer:EnableMouse(false)
	self.drawLayer:ClearAllPoints()
	self.drawLayer:SetParent(nil)
	self.drawLayer:Hide()
end

local newCapture
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

local captureCache = setmetatable({}, {__mode="k"})
local function newCap()
	local cap = table.remove(captureCache)
	if not cap then cap = {} end
	return cap
end

local function delCap(cap)
	if not cap then return end
	captureCache[cap] = true
end

local function del(obj)
	wipe(obj)
	table.insert(pool, obj)
end

local cache = {}
local function newRec(drawLayer)
	local rec = table.remove(cache)
	return LibMouseGestures:New(drawLayer, rec);
end

local function delRec(rec)
	if not rec then return end
	table.insert(cache, rec)
end

local pool = setmetatable({}, {__mode = "k"})

local stopFunc, cancelFunc

--- Create a new LibScriptableWidgetGestures object
-- @usage WidgetGestures:New(visitor, name, config, errorLevel)
-- @param visitor An LibScriptableCore-1.0 object, or provide your own
-- @param name A name for the gesture widget
-- @param config This timeer's parameters
-- @param errorLevel The errorLevel for this object
-- @return A new LibScriptableWidgetGestures widget
function WidgetGestures:New(visitor, name, config, errorLevel) 
	assert(name, "WidgetGestures requires a name.")
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

	obj.widget = LibWidget:New(obj, visitor, MAJOR .. "." .. name, config, 0, 0, 0, widgetType, errorLevel)
	
	obj.expression = config.expression
	obj.repeating = config.repeating or WidgetGestures.defaults.repeating
	obj.update = config.update or WidgetGestures.defaults.update
	obj.disabled = config.disabled
	obj.gestures = config.gestures
	obj.minLength = config.minLength or WidgetGestures.defaults.minLength
	obj.callback = callback
	obj.error = LibError:New(MAJOR .. ": " .. name, errorLevel)
		
	obj.startButton = config.startButton or defaults.startButton
	obj.stopButton = config.stopButton or defaults.stopButton
	obj.nextButton = config.nextButton or defaults.nextButton
	obj.cancelButton = config.cancelButton or defaults.cancelButton
	-- We're just grabbing the runnable object without any actual execution.
	obj.startFunc = Evaluator.ExecuteCode(obj.environment, MAJOR .. " startFunc", config.startFunc, false, nil, true)
	obj.updateFunc = Evaluator.ExecuteCode(obj.environment, MAJOR .. " updateFunc", config.updateFunc, false, nil, true)
	obj.nextFunc = Evaluator.ExecuteCode(obj.environment, MAJOR .. " nextFunc", config.nextFunc, false, nil, true)
	obj.stopFunc = Evaluator.ExecuteCode(obj.environment, MAJOR .. " stopFunc", config.stopFunc, false, nil, true, true)
	obj.cancelFunc = Evaluator.ExecuteCode(obj.environment, MAJOR .. " cancelFunc", config.cancelFunc, false, nil, true)
	if obj.startFunc then obj.startFunc = obj.startFunc() end
	if obj.updateFunc then obj.updateFunc = obj.updateFunc() end
	if obj.nextFunc then obj.nextFunc = obj.nextFunc() end
	if obj.stopFunc then obj.stopFunc = obj.stopFunc() else obj.stopFunc = stopFunc end
	if obj.cancelFunc then obj.cancelFunc = obj.cancelFunc() else obj.cancelFunc = cancelFunc end

	obj.tooltip = config.tooltip
	obj.maxGestures = config.maxGestures or defaults.maxGestures
	obj.minGestures = config.minGestures or defaults.minGestures
	obj.showTrail = config.showTrail

	obj.drawLayer = _G[config.drawLayer or defaults.drawLayer]
	obj.gist = {}
	self.gestures = self.gestures

	obj:NewCapture()

	obj.rec = newRec(obj.drawLayer)
	obj.rec.widgetData = obj
	obj.rec.creator = obj.name
	obj.rec.cdoodle = {creator=obj.name}
	obj.rec.color = 0xffff0fff
	
--[[
	local capture = obj.drawLayer:GetScript("OnEnter") or function() return noop end
	local release = obj.drawLayer:GetScript("OnLeave") or function() return noop end

	local function CaptureFrame() 
		capture()
		obj:Start()
	end

	local function ReleaseFrame()
		release()
		obj:Stop()
	end

	if obj.drawLayer ~= UIParent then
		obj.drawLayer:SetScript("OnEnter", CaptureFrame)
		obj.drawLayer:SetScript("OnLeave", ReleaseFrame)
	else
		obj:Start()
	end

]]
	return obj	
end

function WidgetGestures:NewCapture()
	local obj = self
	delCap(obj.capture)
	local cap = newCap()
	cap["startButton"] = obj.startButton
	cap["stopButton"] = obj.stopButton
	cap["nextButton"] = obj.nextButton
	cap["cancelButton"] = obj.cancelButton
	cap["startFunc"] = WidgetGestures.StartFunc
	cap["updateFunc"] = WidgetGestures.UpdateFunc
	cap["nextFunc"] = WidgetGestures.NextFunc
	cap["stopFunc"] = WidgetGestures.StopFunc
	cap["cancelFunc"] = WidgetGestures.CancelFunc
	cap["tooltip"] = obj.tooltip
	cap["maxGestures"] = obj.maxGestures
	cap["showTrail"] = obj.showTrail
	obj.capture = cap
	return cap
end

local function DrawPoint(rec, pointa, pointb, col)
	if pointb then
		if not pointa[3] then
			local T = rec.drawLayer:CreateTexture()
			T:SetTexture([[Interface\AddOns\Doodlepad\line]])
			pointa[3] = T
			rec.texs = rec.texs or {}
			table.insert(rec.texs, T)
		end
		rec.w = rec.w or rec.widgetData.drawLayer:GetWidth()
		rec.h = rec.h or rec.widgetData.drawLayer:GetHeight()
		PluginDoodle.DrawLine(rec.drawLayer, pointa[3], pointb[1], pointb[2], pointa[1], pointa[2], rec.w, rec.h, col)
	end
end

function WidgetGestures:Draw()
	local rec = self.rec
	for n = 1, #rec.cdoodle do
		DrawPoint(rec, rec.cdoodle[n], rec.cdoodle[n-1], rec.color)
	end
end

--- Delete a LibScriptableWidgetGestures object
-- @usage :Del()
-- @return Nothing
function WidgetGestures:Del()
	self:Stop()
	self.widget:Del()
	self.error:Del()
	delRec(self.rec)
	pool[self] = true
end

--- Start a LibScriptableWidgetGestures
-- @usage object:Start()
-- @return Nothing
function WidgetGestures:Start()
	self.rec:StartCapture(self:NewCapture())
end

--- Stop a LibScriptableWidgetGestures
-- @usage object:Stop()
-- @return Nothing
function WidgetGestures:Stop()
	if type(self.rec) == "table" then
		self.rec:Stop()
	end
end

function stopFunc(rec)

	local self = rec.widgetData
	local g = rec:GetGist(self.gist);
			
	if #g < self.minGestures or #self.gestures < self.minGestures then self:Start() return end

	local current = 1
	local errors = 0
	if ( g ) then
		for n = 1, #g do
			if current > #self.gestures then break end
			local type = g[n][1]
			local pattern = g[n][2]
			if type == self.gestures[current].type and pattern == self.gestures[current].pattern then
				current = current + 1
			end
		end
	end
	if current - 1 == #self.gestures then
		local mx1, my1, mx2, my2 = rec:GetMovementBounds() 
		local length = math.sqrt( math.pow(mx2-mx1, 2) + math.pow(my2-my1, 2) )
		if length > self.minLength then
			self.visitor.environment.self = self
			Evaluator.ExecuteCode(self.visitor.environment, self.name, self.expression)
			if type(self.callback) == "function" then
				self:callback()
			end
			if not self.config.silent then
				self.error:Print("Execute", 2)
			end
		end
		
	end
			
	self:Start()
end

function cancelFunc(rec)
	local self = rec.widgetData
	self:Start()
end

function WidgetGestures.StartFunc(rec, ...)
	local self = rec.widgetData

	if self and type(self.startFunc) == "function" then
		self.startFunc(rec, ...)
	end
end

function WidgetGestures.UpdateFunc(rec, ...)
	local self = rec.widgetData
	if self and type(self.updateFunc) == "function" then
		self.updateFunc(rec, ...)
	end
end

function WidgetGestures.StopFunc(rec, ...)
	local self = rec.widgetData
	if self and type(self.stopFunc) == "function" then
		self.stopFunc(rec, ...)
	end
end

function WidgetGestures.NextFunc(rec, ...)
	local self = rec.widgetData
	if self and type(self.nextFunc) == "function" then
		self.nextFunc(rec, ...)
	end
end

function WidgetGestures.CancelFunc(rec, ...)
	local self = rec.widgetData
	if type(self.cancelFunc) == "function" then
		self.cancelFunc(rec, ...)
	end
end
--- Update widget
-- @usage :Update()
-- @return Nothing
--[[
function WidgetGestures:Update()
	if #self.gestures == 0 or not self.active then return end

	if self.rec then delRec(self.rec) end
	local rec = newRec(self.drawLayer)
	rec.widgetData = self
	self.rec = rec
	
	rec:StartCapture(self.capture)
	
	self:Stop()
end
]]

--- Get an Ace3 option table. Plug this into a group type's args.
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table: `name.args = options`.
function WidgetGestures:GetOptions(db, callback, data)
	local defaults = WidgetGestures.defaults
	local options = {
		enabled = {
			name = "Enabled",
			desc = "Whether this gesture is enabled or not.",
			type = "toggle",
			get = function() return db.enabled end,
			set = function(info, v) 
				db.enabled = v
				db.enabledDirty = true
				if type(callback) == "function" then callback(data) end
			end,
			order = 5
		},
		drawLayer = {
			name = L["Draw Layer"],
			type = "input",
			get = function()
				return db.drawLayer or defaults.drawLayer
			end,
			set = function(info, v)
				db.drawLayer = v
				db.drawLayerDirty = true
				if type(callback) == "function" then callback(data) end
			end,
			order = 9
		},
		startButton = {
			name = L["Start Button"],
			desc = L["A description of the mouse action that will trigger the start of the gesture recording. Possibly values are: LeftButtonUp, LeftButtonDown, RightButtonUp, RightButtonDown, MiddleButtonUp, MiddleButtonDown, Freehand"],
			type = "input",
			get = function()
				return db.startButton or defaults.startButton
			end,
			set = function(info, v)
				db.startButton = v
				db.startButtonDirty = true
				if type(callback) == "function" then callback(data) end
			end,
			order = 10
		},
		stopButton = {
			name = L["Stop Button"],
			desc = L["A description of the mouse action that will stop (finish) the gesture recording. Possible values are: LeftButtonUp, LeftButtonDown, RightButtonUp, RightButtonDown, MiddleButtonUp, MiddleButtonDown"],
			type = "input",
			get = function()
				return db.stopButton or defaults.stopButton
			end,
			set = function(info, v)
				db.stopButton = v
				db.stopButtonDirty = true
				if type(callback) == "function" then callback(data) end
			end,
			order = 11
		},
		nextButton = {
			name = L["Next Button"],
			desc = L["A description of the optional mouse action that will trigger nextFunc and start a new recording if defined. This can be used to simultaneously recording multiple gestures at once. Possible values are: LeftButtonUp, LeftButtonDown, RightButtonUp, RightButtonDown, MiddleButtonUp, MiddleButtonDown"],
			type = "input",
			get = function()
			
			end,
			set = function(info, v)
				db.nextButton = v
				db.nextButtonDirty = true
				if type(callback) == "function" then callback(data) end
			end,
			order = 12
		},
		cancelButton = {
			name = L["Cancel Button"],
			type = "input",
			desc = L["A description of the optional mouse action that will cancel the recording and call cancelFunc instead of stopFunc. Possible values are: LeftButtonUp, LeftButtonDown, RightButtonUp, RightButtonDown, MiddleButtonUp, MiddleButtonDown"],
			get = function()
				return db.cancelButton or defaults.cancelButton
			end,
			set = function(info, v)
				db.cancelButton = v
			end,
			order = 12
		},
		showTrail = {
			name = L["Show Trail"],
			type = "toggle",
			desc = L["If set to true, a cursor trail will be shown while the gesture takes place. (can be used for debugging and whatnot)"],
			get = function()
				return db.showTrail
			end,
			set = function(info, v)
				db.showTrail = v
				db.showTrailDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 13
		},
		maxGestures = {
			name = L["Max Gestures"],
			desc = L["If startButton is set to Freehand, maxGestures can be used to set a hard cap on the number of gestures the user can make before the recording stops. Please note, that this will only work on linear gestures. If you need to record circular gestures, leave out this field and the library will sort it for you instead."],
			type = "input",
			pattern = "%d",
			get = function()
				return tostring(db.maxGestures or defaults.maxGestures)
			end,
			set = function(info, v)
				db.maxGestures = tonumber(v)
				db.maxGesturesDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 14
		},
		minGestures = {
			name = L["Minimum Gestures"],
			desc = L["Minimum gestures before processing a full gesture."],
			type = "input",
			pattern = "%d",
			get = function()
				return db.minGestures or defaults.minGestures
			end,
			set = function(info, v)
				db.minGestures = v
				db.minGesturesDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 15
		},
--[[
		repeating = {
			name = L["Repeating"],
			desc = L["Whether to keep repeating the recording or not"],
			type = "toggle",
			get = function()
				return db.repeating or defaults.repeating
			end,
			set = function(info, v)
				db.repeating = v
				db.repeatingDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 15
		},
]]
		minLength = {
			name = L["Minimum Length"],
			desc = L["Minimum Length"],
			type = "input",
			pattern = "%d",
			get = function()
				return tostring(minLength or defaults.minLength)
			end,
			set = function(info, v)
				db.minLength = tonumber(v)
				db.minLengthDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 16
		},
--[[
		errorsAllowed = {
			name = L["Error Threshhold"],
			desc = L["The gesture will automatically fail if you make more than this many errors"],
			type = "input",
			pattern = "%d",
			get = function()
				return tostring(db.errorsAllowed or defaults.errorsAllowed)
			end,
			set = function(info, v)
				db.errorsAllowed = tonumber(v)
				db.errorsAllowedDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 17
		},
]]
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
				if type(callback) == "function" then callback(data) end
			end,
			order = 48
		},		
		startFunc = {
			name = L["Start Function"],
			type = "input",
			desc = L["An optional function that will be called on the first start of a gesture recording. This can be used to initializing values and so on. startFunc will only be called at the start of a capture, and not after a nextButton has fired. Callback values: recorder, x1, y1, x2, y2 : The recorder object as well as the initial coordinates where the mouse triggered startFunc and the current cursor coordinates (which should be the same in this case)"],
			width = "full",
			multiline = true,
			get = function()
				return db.startFunc			
			end,
			set = function(info, v)
				if v == "" then v = nil end
				db.startFunc = v
				db.startFuncDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 49
		},
		updateFunc = {
			name = L["Update Function"],
			desc = L["A function that will be called after each OnUpdate event on the recording frame. Callback values: recorder, x1, y1, x2, y2 : The recorder object as well as the initial coordinates where the mouse triggered startFunc (or the last called nextFunc) and the current cursor coordinates"],
			type = "input",
			width = "full",
			multiline = true,
			get = function()
				return db.updateFunc or defaults.updateFunc
			end,
			set = function(info, v)
				if v == "" then v = nil end
				db.updateFunc = v
				db.updateFuncDirty = true
				if type(callback) == "function" then
					callback(data)
				end			
			end,
			order = 50
		},
		stopFunc = {
			name = L["Stop Function"],
			desc = L["A function that will be called when stopButton has triggered the end of a recording. Callback values: recorder, x1, y1, x2, y2 : The recorder object as well as the initial coordinates where the mouse triggered startFunc (or the last called nextFunc) and the current cursor coordinates"],
			type = "input",
			width = "full",
			multiline = true,
			get = function()
				return db.stopFunc or defaults.stopFunc
			end,
			set = function(info, v)
				if v == "" then v = nil end
				db.stopFunc = v
				db.stopFuncDirty = true
				if type(callback) == "function" then
					callback(data)
				end			
			end,
			order = 51
		},
		
		nextFunc = {
			name = L["Next Function"],
			desc = L["A function that will be called when nextButton has triggered the stop of the current recording and immediate start of a new from the current position. Callback values: recorder, x1, y1, x2, y2 : The recorder object as well as the initial coordinates where the mouse triggered startFunc (or the last called nextFunc) and the current cursor coordinates."],
			type = "input",
			width = "full",
			multiline = true,
			get = function()
				return db.nextFunc
			end,
			set = function(info, v)
				if v == "" then v = nil end
				db.nextFunc = v
				db.nextFuncDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 52
		},
		cancelFunc = {
			name = L["Cancel Function"],
			desc = L["A function that will be called when cancelButton has triggered the termination of a recording. Callback values: recorder, x1, y1, x2, y2 : The recorder object as well as the initial coordinates where the mouse triggered startFunc (or the last called nextFunc) and the current cursor coordinates"],
			type = "input",
			width = "full",
			multiline = true,
			get = function()
				return db.cancelFunc or defaults.cancelFunc
			end,
			set = function(info, v)
				if v == "" then v = nil end
				db.cancelFunc = v
				db.cancelFuncDirty = true
				if type(callback) == "function" then
					callback(data)
				end			
			end,
			order = 53
		},
		tooltip	= {
			name = L["Tooltip"],
			desc = L["If set to either a string value or a table with multiple strings, a tooltip will be displayed next to the cursor when positioned over the recording frame. This can be used to provides tips on how to use the mouse gestures."],
			type = "input",
			width = "full",
			multiline = true,
			get = function()
				return db.tooltip or defaults.tooltip
			end,
			set = function(info, v)
				db.tooltip = v ~= "" and v or nil
				db.tooltipDirty = true
				if type(callback) == "function" then
					callback(data)
				end			
			end,
			order = 54
		},
	}
	options.gestures = {
		name = L["Gestures"],
		type = "group",
		args = {
			add = {
				name = L["Add Gesture"],
				type = "execute",
				func = function()
					local gesture = {
						type = defaults.type,
						pattern = defaults.pattern
					}
					db.gestures = db.gestures or {}
					table.insert(db.gestures, gesture)
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 1
			},
		}
	}
	
	for i, gesture in ipairs(db.gestures or {}) do
		options.gestures.args["Gesture" .. i] = {
			name = L["Gesture "] .. i,
			type = "group",
			order = i,
			args = {
				type = {
					name = L["Type"],
					type = "select",
					values = typesList,
					get = function()
						StarTip:Print(gesture.type)
						return gesture.type and typesDict[gesture.type] or typesDict[defaults.type]
					end,
					set = function(info, v)
						gesture.type = types[v]
						gesture.typeDirty = true
						if type(callback) == "function" then callback(data) end
					end,
					order = 7
				},
				pattern = {
					name = L["Pattern"],
					type = "select",
					values = patternsList,
					get = function()
						return gesture.pattern and patternsDict[gesture.pattern] or patternsDict[defaults.pattern]
					end,
					set = function(info, v)
						gesture.pattern = patterns[v]
						gesture.patternDirty = true
						if type(callback) == "function" then callback(data) end
					end,
					order = 8			
				},
				delete = {
					name = L["Delete"],
					type = "execute",
					func = function()
						tremove(db.gestures, i)
						if type(callback) == "function" then callback(data) end
					end,
					order = 9
				}
			}
		}	
	end
	return options
end
