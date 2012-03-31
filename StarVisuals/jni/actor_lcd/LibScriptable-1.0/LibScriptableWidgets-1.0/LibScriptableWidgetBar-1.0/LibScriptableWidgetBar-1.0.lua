local MAJOR = "LibScriptableWidgetBar-1.0"  
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub") 
local WidgetBar = LibStub:NewLibrary(MAJOR, MINOR)
if not WidgetBar then return end

local LibProperty = LibStub("LibScriptableUtilsProperty-1.0", true)
assert(LibProperty, MAJOR .. " requires LibScriptableUtilsProperty-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local LibError = LibStub("LibScriptableUtilsError-1.0", true)
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local LibWidget = LibStub("LibScriptableWidget-1.0", true)
assert(LibWidget, MAJOR .. " requires LibScriptableWidget-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

local pool = setmetatable({}, {__mode = "k"})

if not WidgetBar.__index then
	WidgetBar.__index = WidgetBar
end

SCHARS = {
	{31, 31, 31, 31, 31, 31, 31, 31},
	{31, 16, 16, 16, 16, 16, 16, 31},
	{31,  0,  0,  0,  0,  0,  0, 31},
	{31,  1,  1,  1,  1,  1,  1, 31},
	{31, 31, 31, 31,  0,  0,  0,  0},
	{ 0,  0,  0,  0, 31, 31, 31, 31}};

WidgetBar.DIR_EAST, WidgetBar.DIR_WEST = 1, 2
WidgetBar.STYLE_NORMAL, WidgetBar.STYLE_HOLLOW = 1, 2
WidgetBar.ORIENTATION_HORIZONTAL = 1
WidgetBar.ORIENTATION_VERTICAL = 2

WidgetBar.directionList = {L["West"], L["East"]}
WidgetBar.styleList = {L["Normal"], L["Hollow"]}
WidgetBar.orientationList = {L["Horizontal"], L["Vertical"]}
	
WidgetBar.defaults = {
	length = 10, height = 1, direction = WidgetBar.DIR_EAST, update = 1000, style = WidgetBar.STYLE_NORMAL, orientation = 1
}

local widgetType = {bar = true}
local tremove = table.remove

--- Create a new LibScriptableWidgetBar object
-- @usage WidgetBar:New(visitor, name, config, row, col, layer, errorLevel, callback, data)
-- @param visitor An LibScriptableCore-1.0 object, or provide your own
-- @param config This widget's parameters
-- @param row This widget's row
-- @param col This widget's column
-- @param layer This widget's layer
-- @param errorLevel The errorLevel for this object
-- @param callback This widget's real draw function
-- @return A new LibScriptableWidgetBar object
function WidgetBar:New(visitor, name, config, row, col, layer, errorLevel, callback) 

	assert(name, "Please provide the bar with a name")
	assert(config, "Please provide the bar with a config")
	assert(config.expression, name .. ": Please provide the bar with an expression")
			
	local obj = tremove(pool)

	if not obj then
		obj = {}
	end
	
	setmetatable(obj, self)
	
	obj.error = visitor.error --LibError:New(MAJOR, errorLevel)
		
	obj.errorLevel = errorLevel or 3

	obj.callback = callback

	obj.widget = LibWidget:New(obj, visitor, name, config, row, col, layer, widgetType, errorLevel)


	obj.lcd_type = visitor.type

	obj.callback = callback

	obj.unitOverride = config.unitOverride

	obj.expression = LibProperty:New(obj, visitor, name .. " expression", config.expression, "", config.unit, errorLevel)
	obj.expression2 = LibProperty:New(obj, visitor, name .. "expression2", config.expression2, "", config.unit, errorLevel);
	obj.expr_min = LibProperty:New(obj, visitor, name .. " min", config.min, nil, config.unit, errorLevel)
	obj.expr_max = LibProperty:New(obj, visitor, name .. " max", config.max, nil, config.unit, errorLevel)

	obj.color1 = LibProperty:New(obj, visitor, name .. " color1", config.color1, "", config.unit, errorLevel)
	obj.color2 = LibProperty:New(obj, visitor, name .. " color2", config.color2 or config.color1, "", config.unit, errorLevel)
	
	obj.color_valid = {}
	
	--[[obj.color = {}
	obj.color_valid = {}
	obj.color_valid[0] = obj.widget:WidgetColor(section, "barcolor0", obj.color[0]);
	obj.color_valid[1] = obj.widget:WidgetColor(section, "barcolor1", obj.color[1]);

	obj.fg_color = {}
	obj.bg_color = {}
	obj.fg_valid = obj.widget:WidgetColor(section, "foreground", obj.fg_color);
	obj.bg_valid = obj.widget:WidgetColor(section, "background", obj.bg_color);
	]]
	
	obj.length = config.length or WidgetBar.defaults.length --visitor.CFG:FetchRaw(config, "length", WidgetBar.defaults.length)

	obj.height = config.height or WidgetBar.defaults.height --visitor.CFG:FetchRaw(config, "height", WidgetBar.defaults.height)

	obj.direction = config.direction or WidgetBar.defaults.direction --visitor.CFG:FetchRaw(config, "direction", WidgetBar.defaults.direction);
	
	obj.orientation = config.orientation or WidgetBar.defaults.orientation

	obj.update = config.update or WidgetBar.defaults.update --visitor.CFG:FetchRaw(config, "update", WidgetBar.defaults.update)

	obj.style = config.style or WidgetBar.defaults.style --visitor.CFG:FetchRaw(config, "style", WidgetBar.defaults.style);
	
	obj.texture = config.texture
	
	obj.val1 = 0;
	obj.val2 = 0;
	obj.min = 0;
	obj.max = 0;

	obj.timer = LibTimer:New("WidgetBar.timer " .. obj.widget.name, obj.update, true, self.Update, obj)
		
--[[
	if config.events then
		for event in pairs(config.events) do
			obj.widget:RegisterEvent(event)
			obj[event] = function(unit) 
				if unit == obj.unit then 
					obj:Update() 
				end
			end
		end
	end
]]
--	QObject::connect(visitor_->GetWrapper(), SIGNAL(_ResizeLCD(int, int, int, int)),
--		this, SLOT(Resize(int, int, int, int)));
		
	return obj	
end

--- Delete a LibScriptableWidgetBar object
-- @usage object:Del()
-- @return Nothing
function WidgetBar:Del()
	pool[self] = true
	self.timer:Del()
	self.widget:Del()
	self.error:Del()
	self:Stop()
	setmetatable(self, nil)
	--wipe(self)
end

--- Resize the widget. Note that this simply repositions this widget on your surface.
-- @param rows The new row size
-- @param cols The new column size
-- @param old_rows The old rows size
-- @param old_cols The old cols size
-- @return Nothing
function WidgetBar:Resize(rows, cols, old_rows, old_cols)
	local xres = self.visitor.lcd.XRES;
	local yres = self.visitor.lcd.YRES
	local y = rows_ * yres / old_rows;
	local x = cols_ * xres / old_cols;
	local r = row_ * yres / old_rows;
	local c = col_ * xres / old_cols;
	self.rows = round(self.visitor.lcd.LROWS * y / yres);
	self.cols = round(self.visitor.lcd.LCOLS * x / xres);
	self.row = round(self.visitor.lcd.LROWS * r / yres);
	self.col = round(self.visitor.lcd.LCOLS * c / xres);
	self:Update();
end

--- Check for frames intersecting the mouse's focus
-- @usage object:IntersectUpdate(bars)
-- @param frame An optional Frame object.
-- @return Nothing
--[[
function WidgetBar:IntersectUpdate(frame)
	local frame = frame or _G["ChatFrame1"]
	if type(frame) == "table" and frame.GetFrameStrata and frame:IsShown() and self.config and self.config.intersect then
		local strata = LibWidget.strata[frame:GetFrameStrata()]
		if strata and strata > self.minStrata then
			if not self.hidden and self.environment.Intersect(frame, self.bar, self.config.intersectxPad1 or self.config.intersectPad or 0, self.config.intersectyPad1 or self.config.intersectPad or 0, self.config.intersectxPad2 or self.config.intersectPad or 0, self.config.intersectyPad2 or self.config.intersectPad or 0) then
				self.hidden = frame
				self.bar:Hide()
			elseif self.hidden and not self.environment.Intersect(self.hidden, self.bar, self.config.intersectxPad1 or self.config.intersectPad or 0, self.config.intersectyPad1 or self.config.intersectPad or 0, self.config.intersectxPad2 or self.config.intersectPad or 0, self.config.intersectyPad2 or self.config.intersectPad or 0) then
				self.hidden = false
				self.bar:Show()
			end
		end
	end
end
]]

--- Start the widget's timer
-- @usage object:Start()
-- @return Nothing
function WidgetBar:Start(unit)
	self.timer:Start();
	self.unit = unit or self.unitOverride or self.visitor.environment.unit
	self:Update();
end

--- Stop the widget's timer
-- @usage object:Stop()
-- @return Nothing
function WidgetBar:Stop()
	self.timer:Stop();
end

--- Draw the widget. This just calls the real draw function you provided.
-- @name Draw()
-- @usage object:Draw()
-- @return Nothing
function WidgetBar:Draw()
	if type(self.callback) == "function" then
		self:callback(self.data)
	end
end

--- Update the widget
-- @usage object:Update()
-- @return Nothing
function WidgetBar:Update()
	if not self.expression.is_valid then return end
	
	self.expression:Eval();

	local val1, valMax = self.expression:P2N();
	
	self.color1:Eval()
	self.color2:Eval()
	
	if type(val1) ~= "number" then 
		val1 = 1
	end
	
	local val2 = val1;
	if( self.expression2.is_valid ) then
		self.expression2:Eval();
		val2 = self.expression2:P2N();
	end

	local max, min;
	if( self.expr_min.is_valid ) then
		self.expr_min:Eval();
		min = self.expr_min:P2N();
	else
		min = self.min;
		if( val1 < min ) then
			min = val1;
		end
		if( val2 < min ) then
			min = val2;
		end
	end

	if( self.expr_max.is_valid ) then
		self.expr_max:Eval();
		max = self.expr_max:P2N();
	else 
		max = self.max;
		if( val1 > max ) then
			max = val1;
		end
		if( val2 > max ) then
			max = val2;
		end
	end
	
	self.min = min;
	self.max = max;

	if( max > min ) then
		self.val1 = (val1 - min ) / (max - min);
		self.val2 = (val2 - min ) / (max - min);
	else
		self.val1 = 0.0;
		self.val2 = 0.0;
	end

	self:Draw();
end

--- Get an Ace3 option table. Plug this into a group type's args.
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table: `name.args = options`.
function WidgetBar:GetOptions(db, callback, data)
	local defaults = WidgetBar.defaults
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
			name = L["Bar length"],
			desc = L["Enter the bar's length"],
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
			name = L["Bar height"],
			desc = L["Enter the bar's height"],
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
			name = L["Bar update rate"],
			desc = L["Enter the bar's refresh rate"],
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
			name = L["Direction"],
			type = "select",
			values = WidgetBar.directionList,
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
		style = {
			name = L["Style"],
			type = "select",
			values = WidgetBar.styleList,
			get = function() return db.style or defaults.style end,
			set = function(info, v) 
				db.style = v; 
				db["styleDirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 6
		},
		orientation = {
			name = L["Orientation"],
			type = "select",
			values = WidgetBar.orientationList,
			get = function() return db.orientation or defaults.orientation end,
			set = function(info, v)
				db.orientation = v
				db.orientationDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 7
		},
		shownAlways = {
			name = L["Shown Always"],
			desc = L["Whether the frame should be shown always or not"],
			type = "toggle",
			get = function() return db.shownAlways end,
			set = function(info, val) 
				db.shownAlways = val 
				db["shownAlwaysDirty"] = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 8
		},
		min = {
			name = L["Minimum Expression"],
			desc = L["Enter the bar's minimum value expression"],
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
			order = 100
		
		},
		max = {
			name = L["Maximum Expression"],
			desc = L["Enter the bar's maximum value expression"],
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
			order = 101
		},
		expression = {
			name = L["Bar #1 Expression"],
			desc = L["This widget's first bar expression"],
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
			order = 102
		},
		color1 = {
			name = L["Bar #1 Color Script"],
			desc = L["Enter the bar's color script"],
			type = "input",
			multiline = true,
			width = "full",
			get = function() return db.color1 end,
			set = function(info, v) 
				db.color1 = v; 
				db["color1Dirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 103
		},
		expression2 = {
			name = L["Bar #2 Expression"],
			desc = L["This widget's second bar expression."],
			type = "input",
			multiline = true,
			width = "full",
			get = function() return db.expression2 end,
			set = function(info, v) 
				db.expression2 = v; 
				db["expression2Dirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 104
		},		
		color2 = {
			name = L["Bar #2 Color Script"],
			desc = L["Enter the bar's color script"],
			type = "input",
			multiline = true,
			width = "full",
			get = function() return db.color2 end,
			set = function(info, v) 
				db.color2 = v; 
				db["color2Dirty"] = true 
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 105
		}		
	}
	options.widget = LibWidget:GetOptions(db, callback, data)
	return options
end
