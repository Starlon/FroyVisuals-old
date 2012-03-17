local MAJOR = "LibScriptableWidgetIcon-1.0"
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub")
local WidgetIcon = LibStub:NewLibrary(MAJOR, MINOR)
if not WidgetIcon then return end
local LibWidget = LibStub("LibScriptableWidget-1.0", true)
assert(LibWidget, MAJOR .. " requires LibScriptableWidget-1.0")
local LibProperty = LibStub("LibScriptableUtilsProperty-1.0", true)
assert(LibProperty, MAJOR .. " requires LibScriptableUtilsProperty-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local LibEvaluator = LibStub("LibScriptableUtilsEvaluator-1.0", true)
assert(LibEvaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

local pool = setmetatable({}, {__mode = "k"})

if not WidgetIcon.__index then
	WidgetIcon.__index = WidgetIcon
end

WidgetIcon.defaults = {
	speed = 500,
	repeating = true
}

local widgetType = {icon=true}

local function Split(str, delim)
	local start = 1
	local tbl = {}
	for i = 2, string.len(str) do
		if str:sub(i, i) == delim then
			table.insert(tbl, str:sub(start, i - 1))
			start = i + 1
		end
	end

	if start < string.len(str) then
		table.insert(tbl, str:sub(start, string.len(str)))
	end

	return tbl
end

--- Create a new LibScriptableWidgetIcon object
-- @usage WidgetIcon:New(visitor, name, config, row, col, layer, errorLevel, timer)
-- @param visitor An LibScriptableCore-1.0 object, or provide your own
-- @param name A name for this widget
-- @param config This widget's settings
-- @param row This widget's row
-- @param col This widget's column
-- @param layer This widget's layer
-- @param errorLevel The errorLevel for this widget
-- @param callback The draw function
-- @param timer An optional timer. This should have a :Start() and :Stop().
-- @return A new LibScriptableWidgetIcon widget
function WidgetIcon:New(visitor, name, config, row, col, layer, errorLevel, callback, timer)
	assert(name, "WidgetIcon requires a name.")
	assert(config, "Please provide the icon with a config")

	local obj = next(pool)

	if obj then
		pool[obj] = nil
		obj.__index = nil
	else
		obj = {}
		obj.options = {}
	end

	setmetatable(obj, self)

	obj.widget = LibWidget:New(obj, visitor, name, config, row, col, layer, widgetType, errorLevel)

	obj.config = config
	obj.callback = callback
	obj.data = data
	
	obj.error = LibError:New(MAJOR .. ": " .. name, errorLevel)

	obj:Init()

	return obj
end

function WidgetIcon:Init()

	self.cols = 1;
	self.rows = 1;

	self.speed = self.config.speed or self.defaults.speed

	local val = self.config.bitmap

	if( not val) then
		update_ = -1;
		return;
	end

	self.bitmap = val

	local ch = {}

	for i = 1, self.visitor.lcd.YRES do
		local row = val["row" .. i]
		if not row then break end
		local line = Split(row, "|")
		for j, segment in ipairs(line) do
			if j > #ch then
				table.insert(ch, {}) --SpecialChar:New(visitor.lcd.YRES))
				for i = 1, self.visitor.lcd.YRES do
					ch[j][i] = 0
				end
			end
			for c = 1, string.len(segment) do
				if segment:sub(c, c) == "*" then
					ch[j][i] = bit.bxor(ch[j][i], bit.lshift(1, c - 1))
				end
			end
		end
	end

	self.icon = ch

	self.index = 1
	self.started = false
	self.ch = -1
	self.repeating = self.config.repeating or self.defaults.repeating
	self.speed = self.config.speed or self.defaults.speed

	self.bitmap = {} --SpecialChar(self.data[0]:Size())

	self.timer = timer or LibTimer:New("WidgetText.timer " .. self.name, self.speed, self.repeating, self.Update, self, self.errorLevel)

	self.visible = LibProperty:New(self, self.visitor, self.name, self.config.visible, "return true", self.errorLevel);
	self.expr_max = LibProperty:New(self, self.visitor, self.name, self.config.max, "return 100", self.errorLevel)
	self.expr_min = LibProperty:New(self, self.visitor, self.name, self.config.min, "return 0", self.errorLevel)

	self.fg_color = self.config.fg_color
	self.bg_color = self.config.bg_color
	
	self.script = self.config.script

	self.min = 0
	self.max = 0
	
	--QObject::connect(visitor_->GetWrapper(), SIGNAL(_ResizeLCD(int, int, int, int)),
	--	this, SLOT(Resize(int, int, int, int)));

	return obj
end



--- Delete a LibScriptableWidgetIcon object
-- @usage :Del()
-- @return Nothing
function WidgetIcon:Del()
	local key = self
	key.timer:Del()
	key.visible:Del()
	key.fg_color:Del()
	key.bg_color:Del()
	key.widget:Del()
	key:Stop()
	pool[key] = true
end

--- Start a LibScriptableWidgetIcon
-- @usage object:Start()
-- @return Nothing
function WidgetIcon:Start()
	if(self.speed <= 0 or self.active) then
		return;
	end
	self.timer:Start();
	self.enabled = true;
	self.active = true
	self:Update();
end

--- Stop a LibScriptableWidgetIcon
-- @usage object:Stop()
-- @return Nothing
function WidgetIcon:Stop()
	self.timer:Stop()
	self.enabled = false
	self.active = false
end


--- Resize this icon. Note that this repositions this widget on the surface described.
-- @param rows The new row height.
-- @param cols The new column width.
-- @param old_rows The old row height.
-- @param old_cols The new column width.
-- @return Nothing
function WidgetIcon:Resize(rows, cols, old_rows, old_cols)
	local yres = self.visitor.YRES;
	local xres = self.visitor.XRES;
	local r = self.row * yres / old_rows;
	local c = self.col * xres / old_cols;
	self.row = round(rows * r / yres);
	self.col = round(cols * c / xres);
	self:Update();
end

--- Allocate space for this icon's special character
-- @usage :SetupChars()
-- @return Nothing
function WidgetIcon:SetupChars()
	--[[for i, widget in ipairs(self.visitor.widgets) do
		if widget.widget_base == self.widget_base then
			self.start = widget.start
			self.finish = widget.finish
			return
		end
	end]]
	self.start = #self.visitor.lcd.specialChars
	for i, v in ipairs(self.icon) do
		table.insert(self.visitor.lcd.specialChars, v)
	end
	self.finish = #self.visitor.lcd.specialChars
end

--- Draw the image. Note that this simply calls the callback provided in the object's creation.
-- @usage :Draw()
-- @return Nothing
function WidgetIcon:Draw()
	if type(self.callback) == "function" then
		self.callback(self)
	end
end

local function incrementIndex(self)
	self.index = self.index + 1
	if(self.index > #self.icon) then
		self.index = 1;
	end
end

--- Update icon data. This will usually just increment the icon face.
-- @usage :Update()
-- @return Nothing
function WidgetIcon:Update()
	if not self.active then return end
	self.visible:Eval()
	self.expr_max:Eval()
	self.expr_min:Eval()
	
	self.fg = self.fg or {}
	self.bg = self.bg or {}
	self.fg.r, self.fg.g, self.fg.b = LibEvaluator.ExecuteCode(self.environment, "WidgetIcon.fg_color", self.fg_color)
	self.bg.r, self.bg.g, self.bg.b = LibEvaluator.ExecuteCode(self.environment, "WidgetIcon.bg_color", self.bg_color)

	local val = LibEvaluator.ExecuteCode(self.environment, "WidgetIcon.script", self.script)
	
	self:Draw();
	
	if type(val) == "number" then

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
		
		self.index = floor(#self.icon * self.val + 0.5) + 1
		if self.index < 1 then
			self.index = 1
		elseif self.index > #self.icon then
			self.index = #self.icon
		end
	elseif type(val) == "boolean" then
		if val then
			incrementIndex(self)
		end
	else
		incrementIndex(self)
	end
	
end

--- Get an Ace3 option table. Plug this into a group type's args.
-- @usage :GetOptions(db, callback, data)
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table -- `name.args = options`.
function WidgetIcon:GetOptions(visitor, db)
	local defaults = WidgetIcon.defaults
	local options = {
		enabled = {
			name = "Enabled",
			desc = "Whether this icon is enabled or not",
			type = "toggle",
			get = function() return db.enabled end,
			set = function(info, v) db.enabled = v; db["enabledDirty"] = true end,
			order = 1
		}
	}
	options.widget = LibWidget:GetOptions(visitor, db)
	return options
end

