local MAJOR = "LibScriptableWidget-1.0" 
local MINOR = 24

assert(LibStub, MAJOR.." requires LibStub") 
local LibWidget = LibStub:NewLibrary(MAJOR, MINOR)
if not LibWidget then return end
local LibProperty = LibStub("LibScriptableUtilsProperty-1.0", true)
assert(LibProperty, MAJOR .. " requires LibScriptableUtilsProperty-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local LibError = LibStub("LibScriptableUtilsError-1.0", true)
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local Utils = LibStub("LibScriptablePluginUtils-1.0")
assert(Utils, MAJOR .. " requires LibScriptablePluginUtils-1.0")
local LibEvaluator = LibStub("LibScriptableUtilsEvaluator-1.0")
assert(LibEvaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

local pool = setmetatable({}, {__mode = "k"})
local objects = {}

if not LibWidget.__index then
	LibWidget.__index = LibWidget
end

local strlen = string.len

local function rfind(str, char)
	local i = strlen(str)
	while i > 0 do
		if str:sub(i, i) == char then
			return i
		end
		i = i - 1
	end
end

local SCRIPT_SHOW, SCRIPT_HIDE, SCRIPT_SHOWN, SCRIPT_HIDDEN = 1, 2, 3, 4

local function runScript(widget, runnable)
	return widget.runnable[runnable]()	
end

LibWidget.defaults = {
	frameName = "GameTooltip",
	intersectFrameName = "ChatFrame1",
	scriptShow = "self.frame:Show()",
	scriptHide = "self.frame:Hide()",
	scriptShown = "return self.frame:IsShown()",
	scriptHidden = "return not self.frame:IsShown()",
	strata = 1,
	level = 1,
	alwaysShown = false,
	intersect = false,
	minStrata = 4,
	intersectPad = 0,
	intersectxPad1 = 0,
	intersectyPad1 = 0,
	intersectxPad2 = 0,
	intersectyPad2 = 0
}
local defaults = LibWidget.defaults

--[[
local forEvents = {}
local function OnEvent(frame, event, ...)
	if forEvents[event] then
		for k, v in pairs(forEvents[event]) do
			if v[event] then v[event](v, ...) end
		end
	end
end

local eventFrame = CreateFrame("Frame")
eventFrame:SetScript("OnEvent", OnEvent)
]]

--- Create a new LibScriptableWidget object
-- @usage WidgetText:New(child, visitor, name, config, row, col, layer, typeOf, errorLevel)
-- @param visitor An LibScriptableCore-1.0 object, or provide your own
-- @parma name This widget's name
-- @param config This widget's parameters
-- @param row This widget's row
-- @param col This widget's column
-- @param layer This widget's layer
-- @param type Dict of widget types.
-- @param errorLevel The error level for this object.
-- @return A new LibScriptableWidgetText object
function LibWidget:New(child, visitor, name, config, row, col, layer, typeOf, errorLevel) 
	
	assert(type(child) == "table", "No child")
	assert(type(visitor) == "table", "No visitor")
	assert(type(name) == "string", "No name")
	assert(type(config) == "table", "No config")
	assert(type(row) == "number", "No row")
	assert(type(col) == "number", "No col")
	assert(type(typeOf) == "table", "No type")
	
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
	end

	
	obj.frameName = config.frameName or defaults.frameName
	obj.intersectFrameName = config.intersectFrameName or defaults.intersectFrameName
	obj.strata = config.strata or defaults.strata
	obj.level = config.level or defaults.level
	obj.alwaysShown = config.alwaysShown or defaults.alwaysShown
	obj.intersect = config.intersect or defaults.intersect
	obj.intersectxPad1 = config.intersectxPad1 or defaults.intersectxPad1
	obj.intersectyPad1 = config.intersectyPad1 or defaults.intersectyPad1
	obj.intersectxPad2 = config.intersectxPad2 or defaults.intersectxPad2
	obj.intersectyPad2 = config.intersectyPad2 or defaults.intersectyPad2
	obj.intersectPad = config.intersectPad or defaults.intersectPad
	obj.minStrata = config.minStrata or defaults.minStrata
	obj.template = config.template or defaults.template
	obj.frameType = config.frameType or defaults.frameType
	obj.scriptShow = config.scriptShow or defaults.scriptShow
	obj.scriptHide = config.scriptHide or defaults.scriptHide
	obj.scriptShown = config.scriptShown or defaults.scriptShown
	obj.scriptHidden = config.scriptHidden or defaults.scriptHidden

	--obj.frame = _G[obj.frameName]
	--[[obj.internalFrame = CreateFrame("Frame")
	obj.intersectFrame = _G[obj.intersectFrameName]
	obj.intersectFrame:HookScript("OnShow", self.OnShow)
	obj.intersectFrame:HookScript("OnHide", self.OnHide)
	obj.internalFrame:SetParent(obj.intersectFrame)
	obj.internalFrame.obj = obj
	]]
	
	obj.runnable = {}
	obj.runnable[SCRIPT_SHOW] = LibEvaluator.GetRunnable(visitor.environment, "SCRIPT_SHOW", obj.scriptShow)
	obj.runnable[SCRIPT_HIDE] = LibEvaluator.GetRunnable(visitor.environment, "SCRIPT_HIDE", obj.scriptHide)
	obj.runnable[SCRIPT_SHOWN] = LibEvaluator.GetRunnable(visitor.environment, "SCRIPT_SHOWN", obj.scriptShown)
	obj.runnable[SCRIPT_HIDDEN] = LibEvaluator.GetRunnable(visitor.environment, "SCRIPT_HIDDEN", obj.scriptHidden)
	obj.child = child
	obj.visitor = visitor
	obj.environment = visitor.environment
	obj.name = name
	obj.config = config
	obj.row = row
	obj.col = col
	obj.layer = layer
	obj.type = typeOf
	obj.errorLevel = errorLevel
	obj.lcd = visitor.lcd

	if type(visitor.widgets) == "table" then
		visitor.widgets[name] = child
	end
	
	local pos1 = name:find(":")
	local pos2 = rfind(name, ":")
	if pos1 then
		obj.layout_base = name:sub(0, pos1 - 1)
		obj.widget_base = name:sub(pos1 + 1, pos2 - 1)	
	end
	obj.started = false
	obj.errorLevel = errorLevel or 3
	
	for k, v in pairs(obj) do
		child[k] = v
	end

	setmetatable(obj, self)

	if config.events then
		for event in pairs(config.events) do
			obj:RegisterEvent(event)
			obj.child[event] = function(self, unit) 
				if unit == self.unit and self.Update then 
					self:Update() 
				end
			end
		end
	end

	obj.dummyTimer = LibTimer:New(obj.name) -- Just in case someone needs it.

	objects[obj] = true

	obj.deleted = false
	
	return obj	
end

function LibWidget:RegisterEvent(event)
	forEvents[event] = forEvents[event] or {}
	forEvents[event][self.name] = self.child
	eventFrame:RegisterEvent(event)
end

function LibWidget:UnregisterEvent(event)

end

--- Delete this widget
-- @usage object:Del()
-- @return Nothing
function LibWidget:Del()
	pool[self] = true
	objects[self] = nil
	self.deleted = true
	self.dummyTimer:Del()
end
    
function LibWidget:OnShow()
	--self.obj.hidden = false
end
	
function LibWidget:OnHide()
end

function LibWidget:FadeOut()
end

--- Check for intersecting frames.
-- @usage IntersectUpdate(objects)
-- @param frame An optional Frame object.
-- @return Nothing
function LibWidget:IntersectUpdate(frame, intersectFrame)
	local frame = frame or self.frame
	local intersectFrame = intersectFrame or self.intersectFrame
	self.environment.self = self
	if frame and intersectFrame then
			if self.intersect then
				if runScript(self, SCRIPT_SHOWN) and Utils.Intersect(frame, intersectFrame, self.intersectxPad1 or self.intersectPad or 0, self.intersectyPad1 or self.intersectPad or 0, self.intersectxPad2 or self.intersectPad or 0, self.intersectyPad2 or self.intersectPad or 0) then
					runScript(self, SCRIPT_HIDE)
				elseif runScript(self, SCRIPT_HIDDEN) and not Utils.Intersect(frame, intersectFrame, self.intersectxPad1 or self.intersectPad or 0, self.intersectyPad1 or self.intersectPad or 0, self.intersectxPad2 or self.intersectPad or 0, self.intersectyPad2 or self.intersectPad or 0) then
					runScript(self, SCRIPT_SHOW)
				end
			end
	end
end
	
local anchors = {
	"TOP",
	"TOPRIGHT",
	"TOPLEFT",
	"BOTTOM",
	"BOTTOMRIGHT",
	"BOTTOMLEFT",
	"RIGHT",
	"LEFT",
	"CENTER"
}

local anchorsDict = {}

for i, v in ipairs(anchors) do
	anchorsDict[v] = i
end
	
local strataNameList = {
	"TOOLTIP", "FULLSCREEN_DIALOG", "FULLSCREEN", "DIALOG", "HIGH", "MEDIUM", "LOW", "BACKGROUND"
}

LibWidget.strata = {}

for i, v in ipairs(strataNameList) do
	LibWidget.strata[v] = i
end

local strataLocaleList = {
	L["Tooltip"], L["Fullscreen Dialog"], L["Fullscreen"], L["Dialog"], L["High"], L["Medium"], L["Low"], L["Background"]
}
LibWidget.anchors = anchors
LibWidget.anchorsDict = anchorsDict
LibWidget.strataNameList = strataNameList
LibWidget.strataLocaleList = strataLocaleList

	
--- Get an Ace3 option table. Plug this into a group type's args.
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table: `name.args = options`.	
function LibWidget:GetOptions(db, callback, data)
	if type(db) ~= "table" then return end
	local options = {
		name = "Frame Details",
		type = "group",
		args = {
			add = {
				name = L["Add Point"],
				desc = L["Add a new point"],
				type = "input",
				set = function(info, v)
					tinsert(db.points, {"Center", "GameTooltip", "Center", 0, 0})
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 1
			},
			frameName = {
				name = L["Frame Name"],
				desc = L["This is the frame's global name. Follow all rules for naming in Lua."],
				type = "input",
				get = function() return db.frameName or defaults.frameName end,
				set = function(info, val) 
					db.frameName = val
					db.frameNameDirty = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 2
			},
			intersectFrameName = {
				name = L["Intersect Frame"],
				type = "input",
				get = function() return db.intersectFrameName or defaults.intersectFrameName end,
				set = function(info, val) 
					db.intersectFrameName = val; 
					db.intersectFrameNameDirty = true 
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 5
			},
			strata = {
				name = L["Frame Strata"],
				type = "select",
				values = strataLocaleList,
				get = function()
					return db.strata or 1
				end,
				set = function(info, val)
					db.strata = val
					db["strataDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 6
			},
			level = {
				name = L["Frame Level"],
				type = "input",
				pattern = "%d",
				get = function()
					return tostring(db.level or 1)
				end,
				set = function(info, val)
					db.level = tonumber(val)
					db["levelDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 7
			},
			alwaysShown = {
				name = L["Always Shown"],
				type = "toggle",
				desc = "Toggle whether to always show the widget or not",
				get = function() return db.alwaysShown end,
				set = function(info, val)
					db.alwaysShown = val
				end,
				order = 8
			},
			intersect = {
				name = L["Intersect Frames"],
				desc = "Whether to check for intersecting frames or not",
				type = "toggle",
				get = function() return db.intersect end,
				set = function(info, v) 
					db.intersect = v 
					db["intersectDirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 9
			},
			intersectxPad1 = {
				name = L["Intersect X Pad #1"],
				type = "input",
				pattern = "%d",
				get = function() return tostring(db.intersectxPad1 or 0) end,
				set = function(info, v) 
					db.intersectxPad1 = tonumber(v)
					db["intersectxPad1Dirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
					db.interesectPad = nil
				end,
				order = 10
			},
			intersectyPad1 = {
				name = L["Intersect Y Pad #1"],
				type = "input",
				pattern = "%d",
				get = function() return tostring(db.intersectyPad1 or 0) end,
				set = function(info, v) 
					db.intersectyPad1 = tonumber(v)
					db["intersectyPad1Dirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
					db.intersectPad = nil
				end,
				order = 11
			},
			intersectxPad2 = {
				name = L["Intersect X Pad #2"],
				type = "input",
				pattern = "%d",
				get = function() return tostring(db.intersectxPad2 or 0) end,
				set = function(info, v) 
					db.intersectxPad2 = tonumber(v) 
					db["intersectxPad2Dirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
					db.interesectPad = nil
				end,
				order = 12
			},
			intersectyPad2 = {
				name = L["Intersect Y Pad #2"],
				type = "input",
				pattern = "%d",
				get = function() return tostring(db.intersectyPad2 or 0) end,
				set = function(info, v) 
					db.intersectyPad2 = tonumber(v) 
					db["intersectyPad2Dirty"] = true
					if type(callback) == "function" then
						callback(data)
					end
					db.interesectPad = nil
				end,
				order = 11
			},
			intersectPad = {
				name = L["Intersect Padding"],
				desc = "Use this to specify a universal padding",
				type = "input",
				pattern = "%d",
				get = function() return tostring(db.intersectPad or 0) end,
				set = function(info, v)
					db.intersectPad = tonumber(v)
					db.intersectPadDirty = true
					db.intersectxPad1 = nil
					db.intersectyPad1 = nil
					db.intersectxPad2 = nil
					db.intersectyPad2 = nil
					if type(callback) == "function" then
						callback(data)
					end

				end,
				order = 12
			},
			minStrata = {
				name = L["Minimum Intersect Strata"],
				type = "select",
				values = LibWidget.strata,
				get = function() return db.minStrata or defaults.minStrata end,
				set = function(info, v) 
					db.minStrata = v 
					db.minStrataDirty = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 13
			},
			showScript = {
				name = L["SCRIPT_SHOW"],
				type = "input",
				multiline = true,
				width = "full",
				get = function() return db.showScript or defaults.showScript end,
				set = function(info, v) 
					db.showScript = v
					db.shownScriptDirty = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 14

			},
			hideScript = {
				name = L["SCRIPT_HIDE"],
				type = "input",
				multiline = true,
				width = "full",
				get = function() return db.hideScript or defaults.hideScript end,
				set = function(info, v) 
					db.hideScript = v
					db.hideScriptDirty = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 15

			},
			shownScript = {
				name = L["SCRIPT_SHOWN"],
				type = "input",
				multiline = true,
				width = "full",
				get = function() return db.shownScript or defaults.shownScript end,
				set = function(info, v) 
					db.shownScript = v
					db.shownScriptDirty = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 16

			},
			hiddenScript = {
				name = L["SCRIPT_HIDDEN"],
				type = "input",
				multiline = true,
				width = "full",
				get = function() return db.hiddenScript or defaults.hiddenScript end,
				set = function(info, v) 
					db.hiddenScript = v
					db.hiddenScriptDirty = true
					if type(callback) == "function" then
						callback(data)
					end
				end,
				order = 17
			}
		}
	}
	for i, point in ipairs(db.points or {}) do
			options.args["point" .. i] = {
				name = L["Point #"] .. i,
				type = "group",
				args = {
					point = {
						name = L["Text anchor"],
						type = "select",
						values = anchors,
						get = function() return anchorsDict[point[1] or 1] end,
						set = function(info, v) 
							point[1] = anchors[v]; 
							db["pointsDirty"] = true 
							if type(callback) == "function" then
								callback(data)
							end							
						end,
						order = 1
					},
					relativeFrame = {
						name = L["Relative Frame"],
						type = "input",
						get = function() return point[2] end,
						set = function(info, v) 
							point[2] = v; 
							db["pointsDirty"] = true 
							if type(callback) == "function" then
								callback(data)
							end							
						end,
						order = 2
					},
					relativePoint = {
						name = L["Relative Point"],
						type = "select",
						values = anchors,
						get = function() return anchorsDict[point[3] or 1] end,
						set = function(info, v) 
							point[3] = anchors[v]; 
							db["pointsDirty"] = true 
							if type(callback) == "function" then
								callback(data)
							end
						end,
						order = 3
					},
					xOfs = {
						name = L["X Offset"],
						type = "input",
						pattern = "%d",
						get = function() return tostring(point[4] or 0) end,
						set = function(info, v) 
							point[4] = tonumber(v); 
							db["pointsDirty"] = true 
							if type(callback) == "function" then
								callback(data)
							end
						end,
						order = 4
					},
					yOfs = {
						name = L["Y Offset"],
						type = "input",
						pattern = "%d",
						get = function() return tostring(point[5] or 0) end,
						set = function(info, v) 
							point[5] = tonumber(v);
							db["pointsDirty"] = true 
							if type(callback) == "function" then
								callback(data)
							end
						end,
						order = 5					
					},
					delete = {
						name = L["Delete"],
						type = "execute",
						func = function()
							tremove(db.points, i)
							if type(callback) == "function" then
								callback(data)
							end
						end,
						order = 6
					}					
				},
				order = i
			}
	end
	return options
end
