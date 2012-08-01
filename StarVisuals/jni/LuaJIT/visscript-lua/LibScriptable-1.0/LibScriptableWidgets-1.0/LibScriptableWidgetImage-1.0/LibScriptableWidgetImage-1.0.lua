local MAJOR = "LibScriptableWidgetImage-1.0"
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub")
local WidgetImage = LibStub:NewLibrary(MAJOR, MINOR)
if not WidgetImage then return end
local LibWidget = LibStub("LibScriptableWidget-1.0", true)
assert(LibWidget, MAJOR .. " requires LibScriptableWidget-1.0")
local LibProperty = LibStub("LibScriptableUtilsProperty-1.0", true)
assert(LibProperty, MAJOR .. " requires LibScriptableUtilsProperty-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local LibEvaluator = LibStub("LibScriptableUtilsEvaluator-1.0", true)
assert(LibEvaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
local LibBuffer = LibStub("LibScriptableUtilsBuffer-1.0", true)
assert(LibBuffer, MAJOR .. " requires LibScriptableUtilsBuffer-1.0")
local PluginColor = LibStub("LibScriptablePluginColor-1.0", true)
assert(PluginColor, MAJOR .. " requires LibScriptablePluginColor-1.0")
PluginColor = PluginColor:New({})
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

local pool = setmetatable({}, {__mode = "k"})

if not WidgetImage.__index then
	WidgetImage.__index = WidgetImage
end

WidgetImage.defaults = {
	update = 500,
	repeating = true,
	layers = 1,
	pixel = 3,
	foreground = 0xffffffff,
	background = 0xff000000,
	width = 64,
	height = 64
}

local defaults = WidgetImage.defaults

local widgetType = {icon=true}

local function Split(str, delim)
	local start = 1
	local tbl = {}
	for i = 2, strlen(str) do
		if str:sub(i, i) == delim then
			tinsert(tbl, str:sub(start, i - 1))
			start = i + 1
		end
	end

	if start < strlen(str) then
		tinsert(tbl, str:sub(start, strlen(str)))
	end

	return tbl
end

--- Create a new LibScriptableWidgetImage object
-- @usage WidgetImage:New(visitor, name, config, row, col, layer, errorLevel, timer)
-- @param visitor An LibScriptableCore-1.0 object, or provide your own
-- @param name A name for this widget
-- @param config This widget's settings
-- @param row This widget's row
-- @param col This widget's column
-- @param layer This widget's layer
-- @param errorLevel The errorLevel for this widget
-- @param callback The draw function
-- @param timer An optional timer. This should have a :Start() and :Stop().
-- @return A new LibScriptableWidgetImage widget
function WidgetImage:New(visitor, name, config, row, col, layer, errorLevel, callback, timer)
	assert(name, "WidgetImage requires a name.")
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
	obj.Draw = callback
	obj.timer = timer

	obj.error = LibError:New(MAJOR .. ": " .. name, errorLevel)

	obj:Init()

	return obj
end

function WidgetImage:Init()

	self.width = self.config.width
	self.height = self.config.height

	self.fg_color = self.config.foreground or defaults.foreground
	self.bg_color = self.config.background or defaults.background

	self.index = 1
	self.repeating = self.config.repeating or defaults.repeating
	self.update = self.config.update or defaults.update

	self.timer = self.timer or LibTimer:New("WidgetImage.timer " .. self.name, self.update, self.repeating, self.Update, self, self.errorLevel)

	self.visible = LibProperty:New(self, self.visitor, self.name, self.config.visible, "return true", self.errorLevel);
	self.expr_max = LibProperty:New(self, self.visitor, self.name, self.config.expr_max, "return 0", self.errorLevel)
	self.expr_min = LibProperty:New(self, self.visitor, self.name, self.config.expr_min, "return 0", self.errorLevel)

	self.prescript = self.config.prescript
	self.script = self.config.script

	self.min = 0
	self.max = 0

	self.layers = self.config.layers or defaults.layers
	self.pixel = self.config.pixel or defaults.pixel

	self.image = {}
	for i = 1, self.layers do
		tinsert(self.image, LibBuffer:New(self.name, self.width * self.height, self.bg_color, errorLevel))
	end

	--QObject::connect(visitor_->GetWrapper(), SIGNAL(_ResizeLCD(int, int, int, int)),
	--	this, SLOT(Resize(int, int, int, int)));

	return obj
end



--- Delete a LibScriptableWidgetImage object
-- @name LibScriptableWidgetImage.Del
-- @usage :Del()
-- @return Nothing
function WidgetImage:Del()
	local key = self
	key.timer:Del()
	key.visible:Del()
	key.widget:Del()
	key.buffer:Del()
	key:Stop()
	pool[key] = true
end

--- Start a LibScriptableWidgetImage
-- @name LibScriptableWidgetImage.Start
-- @usage object:Start()
-- @return Nothing
function WidgetImage:Start()
	if(self.config.update <= 0 or self.active) then
		return;
	end
	self:Clear()
	self.environment.self = self
	local val = LibEvaluator.ExecuteCode(self.environment, "WidgetImage.prescript", self.prescript)
	self.environment.self = nil
	if not val and val ~= false or val then
		self.timer:Start();
		self.active = true
		self:Update();
	end
end

--- Stop a LibScriptableWidgetImage
-- @name LibScriptableWidgetImage.Stop
-- @usage object:Stop()
-- @return Nothing
function WidgetImage:Stop()
	self.timer:Stop()
	self.active = false
end

--- Resize this widget. Note that this just repositions the widget at its scaled position. It also calls :Update().
-- @usage :Resize(rows, cols, old_rows, old_cols)
-- @param rows We'll resize to this many rows
-- @param cols We'll resize to this many columns
-- @param old_rows We're resizing from this many rows
-- @param old_cols We're resizing from this many columns
-- @return Nothing
function WidgetImage:Resize(rows, cols, old_rows, old_cols)
	local yres = self.visitor.YRES;
	local xres = self.visitor.XRES;
	local r = self.row * yres / old_rows;
	local c = self.col * xres / old_cols;
	self.row = round(rows * r / yres);
	self.col = round(cols * c / xres);
	self:Update();
end

--[[
function WidgetImage:SetupChars()
	self.start = #self.visitor.lcd.specialChars
	for i, v in ipairs(self.icon) do
		tinsert(self.visitor.lcd.specialChars, v)
	end
	self.finish = #self.visitor.lcd.specialChars
end
]]

--[[
function WidgetImage:DrawVLine(x1, x2, y, color, layer)
	layer = layer or self.index
	local image = self.image[layer]
	local n = floor(y + x1)
	for i = 0, floor(x2 - x1) do
		image.buffer[n + i] = color
	end
end
]]

function WidgetImage:DrawVLine(x, y1, y2, color, l)
	local y
	local p
	l = l or 1
	local image = self.image[self.index]

	color = color or self.fg_color

	x = floor(x)
	y1 = floor(y1)
	y2 = floor(y2)
	
	if(y1 < y2) then
		p = (y1+1)+x+1
		for y = y1, y2 do
			image.buffer[p] = color
			p = p + 1
		end
	elseif(y2 < y1) then
		p = (y2+1)+x+1
		for y = y2, y1 do
			image.buffer[p] = color
			p = p + 1
		end
	else
		image.buffer[(y1+1)+x+1] = color
	end
end

function WidgetImage:Clear()
	for l = 1, self.layers do
		for n = 0, self.image[self.index]:Size() - 1 do
			self.image[l].buffer[n] = self.bg_color
		end
	end
end

function WidgetImage:Fill(color)
	for l = 1, self.layers do
		for n = 0, self.image[self.index]:Size() - 1 do
			self.image[l].buffer[n] = color or self.fg_color
		end
	end
end

function WidgetImage:FG_COL()
	return 0x00, 0x00, 0x00, 0xff
end

function WidgetImage:BG_COL()
	return 0xff, 0xff, 0xff, 0xff
end

function WidgetImage:BL_COL()
	return 0xff, 0xff, 0xff, 0x00
end

function WidgetImage:NO_COL()
	return 0x00, 0x00, 0x00, 0x00
end

function WidgetImage:Blend(row, col)
	if row >= self.height or col >= self.width then return end
	local r, g, b, a = self:BL_COL()
	local o = self.layers
	for l = 1, self.layers do
		local p = self.image[l].buffer[row * lcd.LCOLS + col]
		if p.a == 255 then
			o = l
		end
	end
	for l = o, 1, -1 do
		local p = self.image[l].buffer[row * lcd.LCOLS + col]
		if p.a == 255 then
			r = p.r
			g = p.g
			b = p.b
		elseif p.a > 0 then
			r = (p.r * p.a + r * (255 - p.a)) / 255
			g = (p.g * p.a + g * (255 - p.a)) / 255
			b = (p.b * p.a + b * (255 - p.a)) / 255
		end
	end
	return r, g, b, 0xff
end

local function incrementIndex(self)
	self.index = self.index + 1
	if(self.index > #self.image) then
		self.index = 1;
	end
end

--- Update image. This will execute the image's script and process any layer index information returned by the script.
-- @usage :Update()
-- @return Nothing
function WidgetImage:Update()
	if not self.active then return end
	self.visible:Eval()
	self.expr_max:Eval()
	self.expr_min:Eval()

	local size = 64
	
	self.noise = self.noise or LibBuffer:New("noise", size, 0)
	
	for i = 0, size - 1 do
		self.noise.buffer[i] = random(100) / 100
	end
	
	self.environment.noise = self.environment.noise or {}
	
	local noise = self.noise:MovingAverageExp(0.2)
	
	for i = 0, noise:Size() - 1 do
		self.environment.noise[i] = noise.buffer[i]
	end
			
	noise:Del()
		
	self.environment.self = self
	local val = LibEvaluator.ExecuteCode(self.environment, "WidgetImage.script", self.script)
	self.environment.self = nil

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

		self.index = floor(#self.image * self.val + 0.5) + 1
		if self.index < 1 then
			self.index = 1
		elseif self.index > #self.icon then
			self.index = #self.icon
		end
	elseif type(val) == "boolean" then
		if val then
			incrementIndex(self)
		end
	end

	self:Draw(val);
end

--- Get an Ace3 option table. Plug this into a group type's args.
-- @usage :GetOptions(db, callback, data)
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table -- `name.args = options`.
function WidgetImage:GetOptions(db, callback, data)
	local defaults = WidgetImage.defaults
	local options = {
		enabled = {
			name = L["Enabled"],
			desc = L["Whether this icon is enabled or not"],
			type = "toggle",
			get = function() return db.enabled end,
			set = function(info, v) db.enabled = v; db["enabledDirty"] = true end,
			order = 1
		},
		update = {
			name = L["Update"],
			desc = L["This widget's refresh rate"],
			type = "input",
			pattern = "%d",
			get = function() return db.update or defaults.update end,
			set = function(info, v)
				db.update = v
				db.updateDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 2
		},
		width = {
			name = L["Width"],
			desc = L["This widget's width"],
			type = "input",
			pattern = "%d",
			get = function() return db.width or defaults.width end,
			set = function(info, v)
				db.width = v
				db.widthDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 3
		
		},
		height = {
			name = L["Height"],
			desc = L["This widget's height"],
			type = "input",
			pattern = "%d",
			get = function() return db.height or defaults.height end,
			set = function(info, v)
				db.height = v
				db.heightDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 4
		
		},
		pixel = {
			name = L["Pixel Size"],
			desc = L["This widget's pixel size"],
			type = "input",
			pattern = "%d",
			get = function() return db.pixel or defaults.pixel end,
			set = function(info, v)
				db.pixel = v
				db.pixelDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 5
		
		},
		foreground = {
			name = L["Foreground"],
			desc = L["This widget's foreground color"],
			type = "color",
			get = function() return PluginColor.Color2RGBA(db.foreground or defaults.foreground) end,
			set = function(info, r, g, b)
				db.foreground = PluginColor.RGBA2Color(r, g, b)
				db.foregroundDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 6
		},
		background = {
			name = L["Foreground"],
			desc = L["This widget's background color"],
			type = "color",
			get = function() return PluginColor.Color2RGBA(db.background or defaults.background) end,
			set = function(info, r, g, b)
				db.background = PluginColor.RGBA2Color(r, g, b)
				db.backgroundDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 7		
		},
		prescript = {
			name = L["Prescript"],
			desc = L["This widget's prescript"],
			type = "input",
			multiline = true,
			width = "full",
			get = function() return db.prescript end,
			set = function(info, v)
				db.prescript = v
				db.prescriptDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 8
		
		},
		script = {
			name = L["Script"],
			desc = L["This widget's script"],
			type = "input",
			multiline = true,
			width = "full",
			get = function() return db.script end,
			set = function(info, v)
				db.script = v
				db.scriptDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 9
		
		}
	}
	options.widget = LibWidget:GetOptions(db, callback, data)
	return options
end

