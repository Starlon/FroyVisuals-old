local MAJOR = "LibScriptableWidgetText-1.0"
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub")
local WidgetText = LibStub:NewLibrary(MAJOR, MINOR)
if not WidgetText then return end

local WidgetPlugin = LibStub:NewLibrary("LibScriptableWidgetTextPlugin-1.0", 1)

local LibProperty = LibStub("LibScriptableUtilsProperty-1.0", true)
assert(LibProperty, MAJOR .. " requires LibScriptableUtilsProperty-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local LibError = LibStub("LibScriptableUtilsError-1.0", true)
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local LibWidget = LibStub("LibScriptableWidget-1.0", true)
assert(LibWidget, MAJOR .. " requires LibScriptableWidget-1.0")
local PluginUtils = LibStub("LibScriptablePluginUtils-1.0", true)
assert(PluginUtils, MAJOR .. " requires LibScriptablePluginUtils-1.0")
local LibBuffer = LibStub("LibScriptableUtilsBuffer-1.0", true)
assert(LibBuffer, MAJOR .. " requires LibScriptableUtilsBuffer-1.0")
local LibEvaluator = LibStub("LibScriptableUtilsEvaluator-1.0")
assert(LibEvaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
--local LibDogTag = LibStub("LibDogTag-3.0", true)
--assert(LibDogTag, MAJOR .. " requires LibDogTag-3.0", true)
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

local PINGPONGWAIT = 2

WidgetText.ALIGN_LEFT, WidgetText.ALIGN_CENTER, WidgetText.ALIGN_RIGHT, WidgetText.ALIGN_MARQUEE, WidgetText.ALIGN_AUTOMATIC, WidgetText.ALIGN_PINGPONG = 1, 2, 3, 4, 5, 6
local alignmentList = {L["Left"], L["Center"], L["Right"], L["Marquee"], L["Automatic"], L["Pingpong"]}
local alignmentDict = {[L["Left"]] = 1, [L["Center"]] = 2, [L["Right"]] = 3, [L["Marquee"]] = 4, [L["Automatic"]] = 5, [L["Pingpong"]] = 6}

WidgetText.alignmentList = alignmentList
WidgetText.alignmentDict = alignmentDict

WidgetText.SCROLL_RIGHT, WidgetText.SCROLL_LEFT = 1, 2
local directionList = {"Right", "Left"}
local directionDict = {Right = 1, Left = 2}

WidgetText.directionList = directionList
WidgetText.directionDict = directionDict

local map = {value = L["Value"], prefix = L["Prefix"], postfix = L["Postfix"], precision = L["Precision"], align = L["Alignment"], update = L["Update"], speed = L["Scroll Speed"], direction = L["Direction"], cols = L["Columns"]}

local widgetType = {text = true, rc = true}

local strlen = string.len

WidgetPlugin.New = function(environment)
	environment.ALIGN_LEFT, environment.ALIGN_CENTER, environment.ALIGN_RIGHT, environment.ALIGN_MARQUEE, environment.ALIGN_AUTOMATIC, environment.ALIGN_PINGPONG = 1, 2, 3, 4, 5, 6
	environment.SCROLL_RIGHT, environment.SCROLL_LEFT = 1, 2
end

local textUpdate, scrollUpdate
local pool = setmetatable({}, {__mode = "k"})

if not WidgetText.__index then
	WidgetText.__index = WidgetText
end

local new, del
do
	local pool = setmetatable({}, {__mode = "k"})
	function new()
		local tbl = next(pool)
		if tbl then
			pool[tbl] = nil
		else
			tbl = {}
		end

		return tbl
	end
	function del(tbl)
		for k, v in pairs(tbl) do
			if type(v) == "table" then
				del(v)
			end
		end
		pool[tbl] = true
	end
end

local safe = {}
local function copy(src)
	local dst = new()
	safe[src] = true
	if type(src) == "table" then
		for k, v in pairs(src) do
			if type(v) == "table" then
				v = copy(v)
			end
			dst[k] = v
		end
	end
	return dst
end

local kvargsPool = setmetatable({}, {__mode = "k"})
local function newKVargs(unit)
	local tbl = next(kvargsPool)

	if tbl then
		kvargsPool[tbl] = nil
	else
		tbl = {}
	end

	tbl.unit = unit

	return tbl
end

local function delKVargs(tbl)
	kvargsPool[tbl] = true
end

WidgetText.defaults = {
	value = '',
	prefix = '',
	postfix = '',
	precision = 0xbabe,
	align = WidgetText.ALIGN_LEFT,
	speed = 0,
	repeating = true,
	direction = WidgetText.SCROLL_RIGHT,
	update = 0,
	cols = 40,
	fontSize = 13,
	background = {0, 0, 0, 1},
	limited = false
}

--- Create a new LibScriptableWidgetText object
-- @usage WidgetText:New(visitor, name, config, row, col, layer, errorLevel, callback, timer)
-- @param visitor An LibScriptableCore-1.0 object. There's also a visitor pointer in the script environment provided by LibCore, so you can create new widgets at runtime.
-- @param config This widget's settings
-- @param row This widget's row
-- @param col This widget's column
-- @param layer This widget's layer
-- @param errorLevel The self.errorLevel for this object
-- @param callback Your draw function. The widget is passed as first parameter.
-- @return A new LibScriptableWidgetText object
function WidgetText:New(visitor, name, config, row, col, layer, errorLevel, callback)
	assert(name, "WidgetText requires a name.")
	assert(config, "Please provide the marquee with a config")
	assert(config.value, name .. ": Please provide the marquee with a script value")

	local obj = next(pool)

	if obj then
		pool[obj] = nil
		obj.__index = WidgetText
	else
		obj = {}
		obj.options = {}
	end

	setmetatable(obj, self)

	obj.widget = LibWidget:New(obj, visitor, name, config, row, col, layer, widgetType, errorLevel)
	WidgetPlugin:New(visitor.environment)

	obj.error = visitor.error --LibError:New(MAJOR .. " : " .. name, errorLevel)

	obj.config = config
	obj.callback = callback
	obj.timer = timer

	obj.widget = LibWidget:New(self, obj.visitor, obj.name, obj.config, obj.row, obj.col, self.obj, widgetType, self.errorLevel)

	obj.error = LibError:New(MAJOR .. " : "	.. self.name, self.errorLevel)

	if not timer then
		self.localTimer = true
	end
	
	if not textTimer then
		self.localTextTimer = true
	end

	obj:Init(config)

	return obj
end

--- Initialize this widget
-- @usage :Init()
-- @return Nothing
function WidgetText:Init(config)
	local name = self.name
	local visitor = self.visitor
	local obj = self
	local errorLevel = self.errorLevel
	local config = config or self.config
	self.config = config
	
	obj.visitor = visitor
	obj.errorLevel = errorLevel or 3

	obj.unitOverride = config.unitOverride

	--if obj.value then obj.value:Del() end
	obj.value = LibProperty:New(self, visitor, name .. " string", config.value, "", errorLevel) -- text of marquee
	
	--if obj.prefix then obj.prefix:Del() end
	obj.prefix = LibProperty:New(self, visitor, name .. " prefix", config.prefix, "", errorLevel) -- label on the left side
	
	--if obj.postfix then obj.postfix:Del() end
	obj.postfix = LibProperty:New(self, visitor, name .. " postfix", config.postfix, "", errorLevel) -- label on right side

	--if obj.color then obj.color:Del() end
	obj.color = LibProperty:New(self, visitor,	name .. " color", config.color, "", errorLevel) -- widget's color
	
	obj.precision = config.precision or self.defaults.precision -- number of digits after the decimal point
	obj.align = config.align or self.defaults.align -- alignment: left, center, right, marquee, automatic, pingpong
	obj.update = config.update or self.defaults.update -- update interval
	obj.repeating = config.repeating or self.defaults.repeating -- Whether to use repeating timers
	obj.speed = config.speed or self.defaults.speed -- marquee scrolling speed
	obj.direction = config.direction or self.defaults.direction -- marquee direction
	obj.cols = config.cols or self.defaults.cols -- number of colums in marquee
	obj.limited = config.limited or self.defaults.limited -- Whether to use strict length or not. Meaning that your string won't be cut short if its length is more than self.cols. Note that self.cols will be modified to fit the string should this field be false and strlen returns > cols.
	obj.bold = config.bold
	obj.offset = 0 -- increment by pixel
	obj.string = "" -- formatted value
	obj.dontRtrim = config.dontRtrim
	obj.background = config.background or copy(self.defaults.background)
	obj.frame = config.frame
	obj.dogtag = config.dogtag
	obj.fontSize = config.fontSize or self.defaults.fontSize
	
	if obj.direction == self.SCROLL_LEFT then
		obj.scroll = obj.cols -- marquee starting point
	else
		obj.scroll = 0
	end

	-- /* Init pingpong scroller. start scrolling left (wrong way) to get a delay */
	if (obj.align == self.ALIGN_PINGPONG) then
		obj.direction = self.SCROLL_LEFT;
		obj.delay = PINGPONGWAIT;
	end

	assert(type(obj.update) == "number", "You must provide a text widget with a refresh rate: update")

	obj.timer = LibTimer:New("WidgetText.timer " .. obj.widget.name, obj.update, obj.repeating, textUpdate, obj, obj.errorLevel)
	obj.textTimer = LibTimer:New("WidgetText.textTimer " .. obj.widget.name, obj.speed, true, textScroll, obj, obj.errorLevel)
	
	obj.buffer = nil
end

--- Delete a LibScriptableWidgetText object
-- @usage :Del()
-- @return Nothing
function WidgetText:Del()
	local marq = self
	marq:Stop()
	do return end
	if marq.widget then
		marq.widget:Del()
	end
	if marq.value then
		marq.value:Del()
	end
	if marq.prefix then
		marq.prefix:Del()
	end
	if marq.postfix then
		marq.postfix:Del()
	end
	if marq.localTimer then
		marq.timer:Del()
	end
	if marq.localTextTimer then
		marq.textTimer:Del()
	end
	if marq.error then
		marq.error:Del()
	end
	if marq.color then
		marq.color:Del()
	end
	pool[marq] = true
end

--[[
function WidgetText.IntersectUpdate(texts)
	local frame = GetMouseFocus()
	if frame and frame ~= UIParent and frame ~= WorldFrame then
		for k, widget in pairs(texts) do
			if widget.config.intersect then
				if environment.Intersect(frame, widget.frame, widget.config.intersectxPad1 or widget.config.intersectPad or 0, widget.config.intersectyPad1 or widget.config.intersectPad or 0, widget.config.intersectxPad2 or widget.config.intersectPad or 0, widget.config.intersectyPad2 or widget.config.intersectPad or 0) then
					widget.hidden = true
					widget.frame:Hide()
				elseif not environment.Intersect(frame, widget.frame, widget.config.intersectxPad1 or widget.config.intersectPad or 0, widget.config.intersectyPad1 or widget.config.intersectPad or 0, widget.config.intersectxPad2 or widget.config.intersectPad or 0, widget.config.intersectyPad2 or widget.config.intersectPad or 0) and widget.hidden then
					widget.hidden = false
					widget.frame:Show()
				end
			end
		end
	end
end
]]

WidgetText.IntersectUpdate = LibWidget.IntersectUpdate

--- Start a LibScriptableWidgetText object
-- @usage :Start()
-- @return Nothing
function WidgetText:Start(unit)
	self.unit = unit or self.unitOverride or self.visitor.environment.unit
	if self.update > 0 then
		self.timer:Start(self.update)
	end
	if self.speed > 0 then
		self.textTimer:Start(self.speed)
	end
	self.buffer = false
	self:Update()
end

--- Stop a LibScriptableWidgetText object
-- @usage :Stop()
-- @return Nothing
function WidgetText:Stop()
	if self.timer then
		self.timer:Stop()
	end
	if self.textTimer then
		self.textTimer:Stop()
	end
	self.oldBuffer = false
end

--- Update data. This will be called by this widget's timer, or else call it yourself.
-- @usage :Update()
-- @return Nothing
function WidgetText:Update()
	textUpdate(self)
end

--- Executes the widget's draw function -- the callback parameter
-- @name LibScriptableWidgetText.Draw
-- @param text The text to print.
-- @return Nothing
function WidgetText:Draw()
	if type(self.callback) == "function" then
		self:callback()
	end
end

local function strtrim(text)
	text = text:gsub("%s+$", "")
	text = text:gsub("^%s+", "")
	return text
end

local function rtrim(text)
	local pos = 0

	for i = 1, strlen(text) do
		if text:sub(i, i) ~= ' ' then
			pos = i
		end
	end

	if pos == 0 then
		return strtrim(text)
	else
		return text:sub(1, pos) .. strtrim(text:sub(pos + 1))
	end
end


function textScroll(self)

	self.count = (self.count or 0) + 1
	local pre = self.prefix:P2S()
	local post = self.postfix:P2S()

	local str = self.string .. " "

	local num, len, width, pad = 0, strlen(str), self.cols - strlen(pre) - strlen(post), 0

	local srcPtr, dstPtr = 0, 0

	local src = LibBuffer:New(MAJOR .. ": " .. self.widget.name .. "._src", 0, " ", self.errorLevel)
	local dst = LibBuffer:New(MAJOR .. ": " .. self.widget.name .. "._dst ", self.cols, " ", self.errorLevel)

	src:Resize(0)
	dst:Resize(self.cols)

	if width < 0 then
		width = 0
	end

	if self.align == self.ALIGN_LEFT then
		pad = 0
	elseif self.align == self.ALIGN_CENTER then
		pad = (width - len) / 2
		if pad < 0 then
			pad = 0
		end
	elseif self.align == self.ALIGN_RIGHT then
		pad = width - len
		if pad < 0 then
			pad = 0
		end
	elseif self.align == self.ALIGN_AUTOMATIC then
		if len <= width then
			pad = 0
		end
	elseif self.align == self.ALIGN_MARQUEE then
		pad = self.scroll
		if self.direction == self.SCROLL_LEFT then
			self.scroll = self.scroll - 1
			if self.scroll < 0 then
				self.scroll = self.cols
			end

		else
			self.scroll = self.scroll + 1
			if self.scroll > self.cols then
				self.scroll = 0
			end
		end
	elseif self.align == self.ALIGN_PINGPONG then
		if len <= width then
			pad = self.scroll
			if self.direction == self.SCROLL_RIGHT then
				self.scroll = self.scroll + 1
				if self.scroll >= width - len then
					self.direction = self.SCROLL_LEFT
				end
			else
				self.scroll = self.scroll - 1
				if self.scroll < 0 then
					self.direction = self.SCROLL_RIGHT
				end
			end
		else
			pad = 0
		end
	else
		pad = 0
		self.error:Print("No alignment specified")
	end
	
	dstPtr = 0;
	local num = 0;

	-- /* process prefix */
	src:FromString(pre)
	while (num < self.cols) do
		if (srcPtr >= src:Size()) then
			break
		end
		if dstPtr >= self.cols then
			break
		end
		dst:Replace(dstPtr, src.buffer[srcPtr]) --PluginUtils:replaceText(dst, dstPtr, src, srcPtr)
		dstPtr = dstPtr + 1
		srcPtr = srcPtr + 1
		num = num + 1
	end

	src:FromString(str)
	srcPtr = 0;

	local offset = pad

	if(offset < 0) then
		offset = 0;
	end

	-- pad beginning
	while pad > 0 and num <= self.cols do
		dst:Replace(dstPtr, " ")
		dstPtr = dstPtr + 1
		num = num + 1
		pad = pad - 1
	end

	--/* copy content */
	while (num < self.cols) do
		if dstPtr >= self.cols then
			break
		end
		if (srcPtr >= src:Size()) then
			break;
		end
		local offset = pad
		if offset < 0 then offset = 0 end
		dst:Replace(dstPtr, src.buffer[srcPtr])
		dstPtr = dstPtr + 1
		srcPtr = srcPtr + 1
		num = num + 1
	end

	len = src:Size()

	-- pad end
	while (num < self.cols - len) do
		if dstPtr >= self.cols then
			break
		end
		dst:Replace(dstPtr, " ")
		dstPtr = dstPtr + 1
		num = num + 1;
	end

	srcPtr = 0;
	src:FromString(post)

	--/* process postfix */
	while (num < self.cols) do
		if dstPtr >= self.cols then
			break
		end
		if (srcPtr >= src:Size()) then
			break;
		end
		dst:Replace(dstPtr, src.buffer[srcPtr])
		dstPtr = dstPtr + 1
		srcPtr = srcPtr + 1
		num = num + 1
	end

	if self.dontRtrim then
		self.buffer = dst:AsString()
	else
		self.buffer = rtrim(dst:AsString())
	end

	if self.dogtag then
		local unit = type(self.dogtagUnit) == "string" and self.dogtagUnit or self.unit
		local kvargs = newKVargs(unit)
		self.buffer = LibDogTag:Evaluate(self.buffer, "Unit", kvargs)
		delKVargs(kvargs)
	end

	if self.clearOverride then self.unitOverride = false; self.clearOverride = false end

	dst:Del()
	src:Del()

end


function textUpdate(self)
	assert(self.value, self.name .. ": WidgetText has no value")
	if not self.prefix then return self.error:Print("WidgetText needs a prefix", 2)end
	if not self.postfix then return self.error:Print("WidgetText needs a postfix", 2) end
	if not self.color then return self.error:Print("WidgetText needs a color", 2) end

	self._update = 1
	self._update = self._update + self.prefix:Eval()
	self._update = self._update + self.postfix:Eval()
	self.value:Eval()
	self.color:Eval()

	-- /* str or number? */
	if (self.precision == 0xBABE) then
		str = self.value:P2S();
	else
		local fmt = format("return format(\"%%.%df\", %f)", self.precision, self.value:P2N())
		str = LibEvaluator.ExecuteCode(self.visitor.environment, "precision", fmt)
	end

	if str == "" or str ~= self.string then
		self._update = self._update + 1;
		self.string = str;
	end

	if self._update > 0 then
		--[[
		/* if there's a marquee scroller active, it has its own */
		/* update callback timer, so we do nothing here; otherwise */
		/* we simply call this scroll callback directly */
		]]
		if (self.align ~= self.ALIGN_MARQUEE and self.align ~= self.ALIGN_AUTOMATIC and self.align ~= self.ALIGN_PINGPONG) then
			if(strlen(self.string) > self.cols and not self.limited) then
				self.cols = strlen(self.string)
			end
			textScroll(self)
		end
		self:Draw()
	end


	return true
end

local strataNameList = {
	"TOOLTIP", "FULLSCREEN_DIALOG", "FULLSCREEN", "DIALOG", "HIGH", "MEDIUM", "LOW", "BACKGROUND"
}

local strataLocaleList = {
	"Tooltip", "Fullscreen Dialog", "Fullscreen", "Dialog", "High", "Medium", "Low", "Background"
}

--- Get an Ace3 option table. Plug this into a group type's args.
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table -- `name.args = options`.
function WidgetText:GetOptions(db, callback, data)
		local defaults = WidgetText.defaults
		local options = {
			enable = {
				name = L["Enable"],
				desc = L["Enable text widget"],
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
			precision = {
				name = L["Precision"],
				type = "input",
				pattern = "%d",
				get = function()
					return tostring(db.precision or WidgetText.defaults.precision)
				end,
				set = function(info, v)
					db.precision = tonumber(v)
					db["precisionDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 2
			},
			align = {
				name = L["Alignment"],
				type = "select",
				values = alignmentList,
				get = function()
					return db.align or WidgetText.defaults.align
				end,
				set = function(info, v)
					db.align = v
					db["alignDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 3
			},
			update = {
				name = L["Update"],
				type = "input",
				pattern = "%d",
				get = function()
					return tostring(db.update or WidgetText.defaults.update)
				end,
				set = function(info, v)
					db.update = tonumber(v)
					db["updateDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 4
			},
			speed = {
				name = L["Scroll Speed"],
				type = "input",
				pattern = "%d",
				get = function()
					return tostring(db.speed or WidgetText.defaults.speed)
				end,
				set = function(info, v)
					db.speed = tonumber(v)
					db["speedDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 5
			},
			direction = {
				name = L["Direction"],
				type = "select",
				values = directionList,
				get = function()
					return db.direction or WidgetText.defaults.direction
				end,
				set = function(info, v)
					db.direction = v
					db["directionDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 6
			},
			cols = {
				name = L["Columns"],
				type = "input",
				pattern = "%d",
				get = function()
					return tostring(db.cols or WidgetText.defaults.cols)
				end,
				set = function(info, v)
					db.cols = tonumber(v)
					db["colsDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 7
			},
			bold = {
				name = L["Bold"],
				type = "toggle",
				get = function()
					return db.bold or WidgetText.defaults.bold
				end,
				set = function(info, val)
					db.bold = val
					db["boldDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 9
			},
			background = {
				name = L["Background Color"],
				desc = L["This will be the widget's backdrop color."],
				type = "color",
				hasAlpha = true,
				get = function() return unpack(db.background or WidgetText.defaults.background) end,
				set = function(info, r, g, b, a)
					if type(db.background) ~= "table" then db.background = {} return end
					db.background[1] = r
					db.background[2] = g
					db.background[3] = b
					db.background[4] = a
					db["backgroundDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 11
			},
			value = {
				name = L["Value"],
				desc = L["Enter this widget's Lua script"],
				type = "input",
				width = "full",
				multiline = true,
				get = function()
					return db.value
				end,
				set = function(info, v)
					db.value = v
					db["valueDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 11
			},
			prefix = {
				name = L["Prefix"],
				desc = L["Enter this widget's prefix script"],
				type = "input",
				width = "full",
				multiline = true,
				get = function()
					return db.prefix
				end,
				set = function(info, v)
					db.prefix = v
					db["prefixDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 12
			},
			postfix = {
				name = L["Postfix"],
				desc = L["Enter this widget's postfix script"],
				type = "input",
				width = "full",
				multiline = true,
				get = function()
					return db.postfix
				end,
				set = function(info, v)
					db.postfix = v
					db["postfixDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 13
			},
			color = {
				name = L["Color"],
				desc = L["Enter this widget's color script"],
				type = "input",
				width = "full",
				multiline = true,
				get = function()
					return db.color
				end,
				set = function(info, v)
					db.color = v
					db["colorDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 14
			},

		}
	options.widget = LibWidget:GetOptions(db, callback, data)
	return options
end
