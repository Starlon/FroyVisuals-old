local MAJOR = "LibScriptableWidgetHistogram-1.0" 
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub") 
local WidgetHistogram = LibStub:NewLibrary(MAJOR, MINOR)
if not WidgetHistogram then return end

local LibProperty = LibStub("LibScriptableUtilsProperty-1.0", true)
assert(LibProperty, MAJOR .. " requires LibScriptableUtilsProperty-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local LibError = LibStub("LibScriptableUtilsError-1.0", true)
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local LibWidget = LibStub("LibScriptableWidget-1.0", true)
assert(LibWidget, MAJOR .. " requires LibScriptableWidget-1.0")
local Evaluator = LibStub("LibScriptableUtilsEvaluator-1.0")
assert(Evaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
local Utils = LibStub("LibScriptablePluginUtils-1.0", true)
assert(Utils, MAJOR .. " requires LibScriptablePluginUtils-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

local pool = setmetatable({}, {__mode = "k"})

if not WidgetHistogram.__index then
	WidgetHistogram.__index = WidgetHistogram
end

SCHARS = {
    {31, 31, 31, 31, 31, 31, 31, 31},
    {31, 16, 16, 16, 16, 16, 16, 31},
    {31,  0,  0,  0,  0,  0,  0, 31},
    {31,  1,  1,  1,  1,  1,  1, 31},
    {31, 31, 31, 31,  0,  0,  0,  0},
    { 0,  0,  0,  0, 31, 31, 31, 31}};
	
WidgetHistogram.DIR_EAST, WidgetHistogram.DIR_WEST = 1, 2

WidgetHistogram.directionList = {L["East"], L["West"]}
	
WidgetHistogram.defaults = {
	direction = WidgetHistogram.DIR_EAST, update = 500, width = 6, height = 50, char = "#", background = {0, 0, 0, 0}
}

local widgetType = {histogram=true, rc=true, special=true}

--- Create a new LibScriptableWidgetHistogram object
-- @usage WidgetHistogram:New(visitor, name, config, row, col, layer, errorLevel, callback, data)
-- @param visitor An LibScriptableCore-1.0 object, or provide your own
-- @param config This widget's settings
-- @param row This widget's row
-- @param col This widget's column
-- @param layer This widget's layer
-- @param errorLevel This widget's verboseness level
-- @param callback This widget's real draw function
-- @param data Data to pass back through the callback
-- @return A new LibScriptableWidgetHistogram object
function WidgetHistogram:New(visitor, name, config, row, col, layer, errorLevel, callback, data)

	assert(name, "Please provide the histogram with a name")
	assert(config, "Please provide the histogram with a config")
	assert(config.expression, name .. ": Please provide the histogram with an expression")
			
	local obj = next(pool)

	if obj then
		pool[obj] = nil
		obj.__index = nil
	else
		obj = {}
		obj.history = {}
	end

	setmetatable(obj, self)
	
	obj.error = visitor.error --LibError:New(MAJOR .. ": " .. name, errorLevel)
		
	obj.errorLevel = errorLevel

	obj.callback = callback
	obj.data = data

	obj.widget = LibWidget:New(obj, visitor, name, config, row, col, layer, widgetType, env, errorLevel)

	obj.IntersectUpdate = self.IntersectUpdate

    obj.callback = callback
	obj.data = data

    obj.min = 0
	obj.max = 0
    obj.string = ""

    obj.expression =  LibProperty:New(obj, visitor, name .. " expression", config.expression, "", config.unit, errorLevel)
    obj.expr_min = LibProperty:New(obj, visitor, name .. " min", config.min, "", errorLevel, config.unit);
    obj.expr_max = LibProperty:New(obj, visitor, name .. " max", config.max, "", errorLevel, config.unit);
	--obj.color = LibProperty:New(obj, visitor, name .. " color", config.color, "", errorLevel, config.unit);
	obj.color = config.color
	obj.width = config.width or WidgetHistogram.defaults.width
	obj.height = config.height or WidgetHistogram.defaults.height
	obj.reversed = config.reversed
	obj.char = config.char or WidgetHistogram.defaults.char
	obj.background = config.background or WidgetHistogram.defaults.background

	obj.direction = config.direction or WidgetHistogram.defaults.direction --visitor.CFG:FetchRaw(config, "direction", WidgetHistogram.defaults.direction)

	obj.update = config.update or WidgetHistogram.defaults.update -- visitor.CFG:FetchRaw(config, "update", WidgetHistogram.defaults.update)
	
	obj.timer = LibTimer:New("WidgetHistogram.timer " .. obj.widget.name, obj.update, true, obj.Update, obj)
	
    --QObject::connect(v->GetWrapper(), SIGNAL(_ResizeLCD(int, int, int, int)),
    --    this, SLOT(Resize(int, int, int, int)));

	table.wipe(obj.history)
	
	for i = 1, obj.width do
		tinsert(obj.history, {1, 1, 1, 1})
	end
		
	return obj	
end

--- Delete a LibScriptableWidgetHistogram object
-- @usage :Del()
-- @return Nothing
function WidgetHistogram:Del()
	local hist = self
	pool[hist] = true
	hist.error:Del()
	hist.widget:Del()
	hist.expression:Del()
	hist.expr_min:Del()
	hist.expr_max:Del()
end

--- Resize the widget
-- @param rows The new row size
-- @param cols The new column size
-- @param old_rows The old rows size
-- @param old_cols The old cols size
-- @return Nothing
function WidgetHistogram:Resize(rows, cols, old_rows, old_cols)
    local xres = self.visitor.lcd.XRES;
    local yres = self.visitor.lcd.YRES
    local y = self.rows * yres / old_rows;
    local x = self.cols * xres / old_cols;
    local r = self.row * yres / old_rows;
    local c = self.col * xres / old_cols;
    self.rows = round(self.visitor.lcd.LROWS * y / yres);
    self.cols = round(self.visitor.lcd.LCOLS * x / xres);
    self.row = round(self.visitor.lcd.LROWS * r / yres);
    self.col = round(self.visitor.lcd.LCOLS * c / xres);
    self:Update();
end

--- Update whether frames intersect with the mouse's focus
-- @usage IntersectUpdate(objects)
-- @param frame A specific frame. This is optional. Otherwise 'ChatFrame1' is used.
-- @return Nothing
function WidgetHistogram:IntersectUpdate(frame)
	local widget = self
	local frame = frame or _G["ChatFrame1"]
	for i, bar in ipairs(widget.bars or {}) do
		local strata = LibWidget.strata[frame:GetFrameStrata()]
		if type(frame) == "table" and frame.GetFrameStrata and frame:IsShown() and strata and strata > widget.minStrata then
			if widget.config.intersect then
				if not bar.hidden and Utils.Intersect(frame, bar, widget.config.intersectxPad1 or widget.config.itnersectPad or 0, widget.config.intersectyPad1 or widget.config.intersectPad or 0, widget.config.intersectxPad2 or widget.config.intersectPad or 0, widget.config.intersectyPad2 or widget.config.intersectPad or 0) then
					bar.hidden = frame
					bar:Hide()
				end
				if bar.hidden and not Utils.Intersect(bar.hidden, bar, widget.config.intersectxPad1 or widget.config.intersectPad or 0, widget.config.intersectyPad1 or widget.config.intersectPad or 0, widget.config.intersectxPad2 or widget.config.intersectPad or 0, widget.config.intersectyPad2 or widget.config.intersectPad or 0) then
					bar.hidden = false
					bar:Show()
				end
			end
		end
	end
end

--- Start the widget's timer
-- @usage object:Start()
-- @return Nothing
function WidgetHistogram:Start()
	self.unit = self.unitOverride or self.visitor.environment.unit
	self.timer:Start();
	self:Update();
end

--- Stop the widget's timer
-- @usage object:Stop()
-- @return Nothing
function WidgetHistogram:Stop()
	self.timer:Stop();
end

-- Draw the widget. This just calls the real draw function you provided at object creation.
-- @usage object:Draw()
-- @return Nothing
function WidgetHistogram:Draw()
	self:callback(self.data)
end

local cache = setmetatable({}, {__mode = "k"})
local function new(...)
	local obj = next(cache)
	if type(obj) == "table" then 
		cache[obj] = nil
	else
		obj = {} 
	end
	for i = 1, select("#", ...) do
		obj[i] = select(i, ...)
	end
	return obj
end

local function del(tbl)
	cache[tbl] = true
end

-- Update the widget
-- @usage object:Update()
-- @return Nothing
function WidgetHistogram:Update()
    self.expression:Eval();
    local val = self.expression:P2N();

	assert(type(val) == "number", format("%s: expression is not a number (%s)", self.name, type(val)))
	
	local r, g, b, a = Evaluator.ExecuteCode(self.environment, "WidgetHistogram.color", self.color)
	r = r or 1
	g = g or 1
	b = b or 1
	a = a or 1
	
	local max, min
    if( self.expr_min.is_valid ) then
        self.expr_min:Eval();
        min = self.expr_min:P2N();
		assert(type(min) == "number", format("%s: expr_min is not a number value (%s)", self.name, type(min)))
    else
        min = self.min;
        if( val < min ) then
            min = val;
		end
    end

    if( self.expr_max.is_valid ) then
        self.expr_max:Eval();
        max = self.expr_max:P2N();
		assert(type(max) == "number", format("%s: expr_max is not a number value (%s)", self.name, type(max)))
    else
        max = self.max;
        if( val > max ) then
            max = val;
		end
    end

    self.min = min;
    self.max = max;

    if( max > min ) then
        self.val = (val - min ) / (max - min);
    else
        self.val = 0.0;
    end

	if self.direction == WidgetHistogram.DIR_EAST then
		tinsert(self.history, 1, new(self.val, r, g, b, a))
		if #self.history > self.width then
			del(self.history[#self.history])
			tremove(self.history, #self.history)
		end
	else		
		tinsert(self.history, new(self.val, r, g, b, a))
		if #self.history > self.width then
			del(self.history[1])
			tremove(self.history, 1)
		end
	end
	
    self:Draw();
end

local function copy(tbl)
	if type(tbl) ~= "table" then return tbl end
	local newTbl = {}
	for k, v in pairs(tbl) do
		newTbl[k] = v
	end
	return newTbl
end

--- Get an Ace3 option table. Plug this into a group type's args.
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table -- `name.args = options`.
function WidgetHistogram:GetOptions(db, callback, data)
	local defaults = WidgetHistogram.defaults
	local options = {
		enabled = {
			name = L["Enabled"],
			desc = L["Whether the histogram's enabled or not"],
			type = "toggle",
			get = function() return db.enabled end,
			set = function(info, v) 
				db.enabled = v; 
				db["enabledDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 1
		},
		length = {
			name = L["Histogram length"],
			desc = L["Enter the histogram's length"],
			type = "input",
			pattern = "%d",
			get = function() return tostring(db.length or defaults.length) end,
			set = function(info, v) 
				db.length = tonumber(v); 
				db["lengthDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 2
		},
		height = {
			name = L["Histogram height"],
			desc = L["Enter the histogram's height"],
			type = "input",
			pattern = "%d",
			get = function() return tostring(db.height or defaults.height) end,
			set = function(info, v) 
				db.height = tonumber(v); 
				db["heightDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 3
		},
		update = {
			name = L["Histogram update rate"],
			desc = L["Enter the histogram's refresh rate"],
			type = "input",
			pattern = "%d",
			get = function() return tostring(db.update or defaults.update) end,
			set = function(info, v) 
				db.update = tonumber(v); 
				db["updateDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 4
		},
		direction = {
			name = L["Histogram direction"],
			type = "select",
			values = WidgetHistogram.directionList,
			get = function() return db.direction or defaults.direction end,
			set = function(info, v) 
				db.direction = v; 
				db["directionDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 5
		},
		alwaysShown = {
			name = L["Always Shown"],
			desc = L["Whether the frame should be shown always or not"],
			type = "toggle",
			get = function() return db.alwaysShown end,
			set = function(info, val) 
				db.shownAlways = val 
				db["shownAlwaysDirty"] = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 6
		},		
		background = {
			name = L["Backdrop Color"],
			type = "color",
			hasAlpha = true,
			get = function() 
				if type(db.background) ~= "table" then db.background = copy(WidgetHistogram.defaults.background) end
				return db.background[1], db.background[2], db.background[3], db.background[4] end,
			set = function(info, r, g, b, a)
				db.background[1] = r
				db.background[2] = g
				db.background[3] = b
				db.background[4] = a
				db.backgroundDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 7
		},
		expression = {
			name = L["Histogram expression"],
			desc = L["Enter this histogram's expression."],
			type = "input",
			multiline = true,
			width = "full",
			get = function() return db.expression end,
			set = function(info, v) 
				db.expression = v; 
				db["expressionDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 50
		},
		min = {
			name = L["Histogram min expression"],
			desc = L["Enter the histogram's minimum expression"],
			type = "input",
			multiline = true,
			width = "full",
			get = function() return db.min end,
			set = function(info, v) 
				db.min = v; 
				db["minDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 51
		
		},
		max = {
			name = L["Histogram max expression"],
			desc = L["Enter the histogram's maximum expression"],
			type = "input",
			multiline = true,
			width = "full",
			get = function() return db.max end,
			set = function(info, v) 
				db.max = v; 
				db["maxDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 52
		},
		color = {
			name = L["Histogram color expression"],
			desc = L["Enter the histogarm's color script"],
			type = "input",
			multiline = true,
			width = "full",
			get = function() return db.color end,
			set = function(info, v) 
				db.color = v; 
				db["colorDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 53
		}
	}
	options.widget = LibWidget:GetOptions(db, callback, data)
	return options
end
