local MAJOR = "LibScriptableLCDDriver-1.0"
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub") 

local LibDriver = LibStub:NewLibrary(MAJOR, MINOR)
if not LibDriver then return end

local LibError = LibStub("LibScriptableUtilsError-1.0", true)
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local LibCore = LibStub("LibScriptableLCDCore-1.0", true)
assert(LibCore, MAJOR .. " requires LibScriptableLCDCore-1.0")
local LCDText = LibStub("LibScriptableLCDText-1.0", true)
assert(LCDText, MAJOR .. " requires LibScriptableLCDText-1.0")

local PluginString = LibStub("LibScriptablePluginString-1.0", true)
assert(PluginString, MAJOR .. " requires LibScriptablePluginString-1.0")
PluginString = PluginString:New(PluginString)

local pool = setmetatable({}, {__mode = "k"})

local options

local context = UI.CreateContext("LSLCDDriver")
local frame = UI.CreateFrame("Frame", "Frame", context)

local objects = {}

local DEFAULT_LIMIT = 50

local tinsert, tremove = table.insert, table.remove

if not LibDriver.__index then
	LibDriver.__index = LibDriver
end

-- @name LibScriptableDriver:New
-- @usage LibScriptableDriver:New(visitor, rows, cols, yres, xres, layers)
-- @param visitor App
-- @param environment Execution environment
-- @param name A name for this LCD
-- @param config The configuration
-- @param errorLevel Error verbosity
-- @return A new LibScriptableDriver object

function LibDriver:New(visitor, environment, name, config, errorLevel)
	
	assert(type(name) == "string", MAJOR .. ": Invalid name")
	assert(type(config[name]) == "table", MAJOR .. ": " .. name .. ": Invalid config")
	assert(type(config[name].addon) == "string", string.format("%s : %s: Missing %s.addon", MAJOR, name, name))
	
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
		obj.buffer = {}
	end

	setmetatable(obj, self)
	
	obj.environment = environment
	obj.name = name
	obj.core = LibCore:New(obj, obj.environment, name, config, "text", errorLevel)
	obj.error = LibError:New(MAJOR, errorLevel)
	--local rows, cols = config[name].rows, config[name].cols
	obj.rows, obj.cols, obj.yres, obj.xres, obj.layers = config[name].rows or 4, config[name].cols or 20, config[name].yres or 8, config[name].xres or 6, config[name].layers or 3
	obj.lcd = LCDText:New(obj.core, obj.rows, obj.cols, obj.yres, obj.xres, obj.layers, errorLevel, obj.Blit, obj, config[name].update)
	obj.core.lcd = obj.lcd -- You must provide a LibCore object with an LCD object.	
	obj.row = config[name].row or 0
	obj.col = config[name].col or 0
	
	obj.tooltip = UI.CreateFrame("Frame", "Frame", context)
	obj.tooltip:SetPoint("CENTER", UIParent, "CENTER", obj.col, obj.row)

	obj.points = {}
	
	for i, point in ipairs(config[name].points or {}) do
		obj.points[i] = point
	end
	
	-- New font looking like GameTooltipText but red with height 15
	--[[
	local font = CreateFont("LibDriver-normal-"..name)
	local fontbold = CreateFont("LibDriver-bold-"..name)
	]]
	
	obj.font = {}
	obj.font.normal = font
	obj.font.bold = fontbold
	obj.font.size = config[name].font.size
	
	--[[
	if config[name].font then
		font:SetFont(config[name].font.normal, config[name].font.size or 12, config[name].font.style)
		fontbold:SetFont(config[name].font.bold or config[name].font.normal, config[name].font.size or 12, config[name].font.style)
		self.font.size = config[name].font.size
		self.font.style = config[name].font.style		
	else
		font:SetFont(GameTooltipText:GetFont())
		fontbold:SetFont(GameTooltipText:GetFont())
		self.font.size = 12
	end
	
	font:SetTextColor(1,1,1)
	fontbold:SetTextColor(1,1,1)
	
	obj.tooltip:SetFont(font)
	]]
	
	obj.parent = config[name].parent or "UIParent"
		
	obj.background = config.background

	obj.tooltip.cells = {}
	
	for row = 1, obj.core.lcd.LROWS do
		for col = 1, obj.core.lcd.LCOLS do
			local fs = UI.CreateFrame("Text", "Text", obj.tooltip)
			fs:SetWidth(12)
			fs:SetHeight(12)
			obj.tooltip.cells[string.format("%dx%d", col, row)] = fs
			fs:SetPoint("TOPLEFT", UIParent, "TOPLEFT", col * 12, row * 12)--col * obj.font.size, row * obj.font.size)
			fs:SetText(" ")
		end
	end	
	
	obj.tooltip.SetCell = function(tooltip, col, row, text)
		--print(tooltip, col, row, text, tooltip.cells[string.format("%dx%d", col, row)])
		tooltip.cells[string.format("%dx%d", col, row)]:SetText(text)	
			local cell = obj.tooltip.cells[string.format("%dx%d", col, row)]
			if not cell then print("wtf", col, row) end
	end
	--obj:Move("CENTER", obj.col, obj.row)	
	obj.core:CFGSetup()
	obj.core:BuildLayouts()
	
	tinsert(objects, obj)
	
	return obj
end

-- Delete an object
-- @name LibScriptableDriver:Del
-- @usage LibScriptableDriver:Del([lcd]) or object:Del()
-- @param lcd An optional lcd object
-- @return Nothing
function LibDriver:Del(lcd)
	if not lcd then
		lcd = self
	end
	pool[lcd] = true
	lcd.tooltip:SetVisible(false)
	lcd.core:Del()
	lcd.error:Del()
end

-- Show the tooltip
-- @name LibScriptableDriver:Show
-- @usage LibScriptableDriver:Show()
-- @return Nothing
function LibDriver:Show()
	self.core:Start()
	self.tooltip:SetVisible(true)
end

-- Hide the tooltip
-- @name :Hide
-- @usage :Hide()
-- @return NOthing
function LibDriver:Hide()
	self.core:Stop()
	self.tooltip:SetVisible(false)
end

-- Move the tooltip
-- @name LibScriptableDriver:Move
-- @usage LibScriptableDriver(anchor, x, y)
-- @param anchor Frame anchor, i.e: CENTER.
-- @param x X position
-- @param y Y position
-- @return Nothing
function LibDriver:Move(arg1, arg2, arg3, arg4, arg5)
	self.tooltip:ClearAllPoints()
	self.tooltip:SetPoint(arg1, arg2, arg3, arg4, arg5)
end

function LibDriver:Blit(obj, r, c, buffer, len)
	assert(type(buffer) == "table", type(buffer))
	for i = c, c + len - 1 do
		if type(buffer[i - c]) == "number" then
			local chr = obj.lcd.specialChars[buffer[i - c]]
			for y = 0 , obj.lcd.YRES - 1 do
				local mask = bit.lshift(1, obj.lcd.XRES)
				for x = 0, obj.lcd.XRES - 1 do
					mask = bit.rshift(mask, 1)
					if bit.band(chr[y + 1], mask) == 0 then
						local row = r + y + 1
						local col = i + x + 1
						obj.tooltip:SetCell(row, col, " ")
					else
						obj.tooltip:SetCell(row, col, "#")
					end
				end
			end
			
		else
			obj.tooltip:SetCell(i + 1, r + 1, (buffer[i - c] or ' '):sub(1, 1))
		end
	end
end

--[[
function LibDriver:RebuildOpts(visitor, db)
	return LibCore:RebuildOpts(visitor, db)
end

function LibDriver:KeyEvent(modifier, up)
	--self.core:KeyEvent(modifier, up)
end
]]