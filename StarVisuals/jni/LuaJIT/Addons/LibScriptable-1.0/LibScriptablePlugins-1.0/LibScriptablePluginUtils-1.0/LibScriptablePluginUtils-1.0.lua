--A lot of this comes from ckknight.

local MAJOR = "LibScriptablePluginUtils-1.0" 
local MINOR = 24
local PluginUtils = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginUtils then return end
local LibError = LibStub("LibScriptableUtilsError-1.0", false)
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local _G = _G

local tinsert = table.insert
local tremove = table.remove

local ScriptEnv = {}

--- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginUtils:New(environment)
	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end
	return environment
end


local L = setmetatable({}, {__index = function(self, key)
	str = rawget(self, key)
	if str then
		return str
	else
		return key
	end
end})

local DEBUG = false

--- Leave a function as-is or if a string is passed in, convert it to a namespace-method function call.
-- @param namespace a table with the method func_name on it
-- @param func_name a function (which would then just return) or a string, which is the name of the method.
-- @usage PluginUtils.ConvertMethodToFunction({}, function(value) return value end)("blah") == "blah"
-- @usage PluginUtils.ConvertMethodToFunction({ method = function(self, value) return value end }, "method")("blah") == "blah"
-- @return a function
function PluginUtils.ConvertMethodToFunction(namespace, func_name)
	if type(func_name) == "function" then
		return func_name
	end
	
	if DEBUG then
		expect(namespace[func_name], 'typeof', 'function')
	end
	
	return function(...)
		return namespace[func_name](namespace, ...)
	end
end
ScriptEnv.ConvertMethodToFunction = PluginUtils.ConvertMethodToFunction

local function deep_copy(data)
	local t = {}
	for k, v in pairs(data) do
		if type(v) == "table" then
			t[k] = deep_copy(v)
		else
			t[k] = v
		end
	end
	setmetatable(t,getmetatable(data))
	return t
end
PluginUtils.deep_copy = deep_copy

function PluginUtils.ResizeText(str, size)
	if strlen(str) < size then
		for i = strlen(str), size do
			str = str .. " "
		end
	end
	return str:sub(1, size)
end
ScriptEnv.ResizeText = PluginUtils.ResizeText

function PluginUtils.ReplaceText(text1, pos1, text2, pos2)
	local char = text2:sub(pos2, pos2)
	
	if char == "" then char = ' ' end
	
	local left = text1:sub(1, pos1 - 1)
	local right = text1:sub(pos1 + 1)
	
	return left .. char .. right
end
ScriptEnv.ReplaceText = PluginUtils.ReplaceText

function PluginUtils.ResizeList(tbl, size)
	if #tbl < size then
		for i = #tbl + 1, size do
			tinsert(tbl, " ")
		end
	end
end

function PluginUtils.ReplaceTable(tbl1, pos1, tbl2, pos2)
	
end

function PluginUtils.Memcopy(to, from, size)
	for i = 0, size - 1 do
		to[i] = from[i]
	end
end
ScriptEnv.Memcopy = PluginUtils.Memcopy

--- Retrieve the column and row coordinates for the provided index and pitch
-- @usage GetCoords(n, pitch)
-- @param n An index within a buffer
-- @param pitch Your surface's column width
-- @return The column and row representing the provided data
function PluginUtils.GetCoords(n, pitch)				
	local col = n % pitch
	local row = (n - col) / pitch
	return col, row
end					
ScriptEnv.GetCoords = PluginUtils.GetCoords

--- Split a string using the delimiter provided
-- @usage Split(str, delim)
-- @param str The string to split
-- @param delim The delimiter should be a single character
-- @return A new table populated with the split substrings
function PluginUtils.Split(str, delim)
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
ScriptEnv.Split = PluginUtils.Split
 
--- Determine if the number provided is a power of 2
-- @usage IsPowerOf2(n)
-- @param n A number value
-- @return A boolean indicating whether the value is a power of 2 or not
function PluginUtils.IsPowerOf2(n)
	local bits_found = false
	
	if (n < 1) then
		return false
	end

	repeat
		if bit.band(n, 1) ~= 0 then
			if (bits_found) then
				return FALSE;
			end

			bits_found = true;
		end

		n = bit.rshift(n, 1)

	until (n <= 0);

	return true
end
ScriptEnv.IsPowerOF2 = PluginUtils.IsPowerOf2

local NOCOORD = -1
--- Intersect - Find out if two frames intersect, adjusted by paddings.
-- usage Intersect(frame1, frame2, frame1xPad1, frame1yPad1, frame1xPad2, frame1yPad2, frame2xPad1, frame2yPad1, frame2xPad2, frame2yPad2)
-- @param frame1 The first frame to compare.
-- @param frame2 The second frame to compare.
-- @param frame1xPad1 Padding for left side of frame1
-- @param frame1yPad1 Padding for top side of frame1
-- @param frame1xPad2 Padding for right side of frame1
-- @param frame1yPad2 Padding for bottom of frame1
-- @param frame2xPad1 Padding for left side of frame2
-- @param frame2yPad1 Padding for top side of frame2
-- @param frame2xPad2 Padding for right side of frame2
-- @param frame2yPad2 Padding for bottom of frame2
-- @return True if the two frames intersect, false otherwise
function PluginUtils.Intersect(frame1, frame2, frame1xPad1, frame1yPad1, frame1xPad2, frame1yPad2, frame2xPad1, frame2yPad1, frame2xPad2, frame2yPad2)
	if type(frame1) ~= "table" or type(frame2) ~= "table" then LibError:Print(MAJOR .. " PluginUtils.Intersect received invalid frame parameter. Two frames are required."); return false end
	
	if frame1 == frame2 then return true end
		
	frame1xPad1 = frame1xPad1 or 0
	frame1yPad1 = frame1yPad1 or 0
	frame1xPad2 = frame1xPad2 or frame1xPad1
	frame1yPad2 = frame1yPad2 or frame1yPad1
	frame2xPad1 = frame2xPad1 or 0
	frame2yPad1 = frame2yPad1 or 0
	frame2xPad2 = frame2xPad2 or frame2xPad1
	frame2yPad2 = frame2yPad2 or frame2yPad1
		
	frame1.col = -1
	frame1.x1, frame1.y1, frame1.x2, frame1.y2 = -1, -1, -1, -1
	if frame1:GetCenter() then
		local frame = frame1:GetParent()
		local scale = frame1:GetScale()
		local x, y = frame1:GetCenter()
		local width = frame1:GetWidth()
		local height = frame1:GetHeight()
		x = x * scale
		y = y * scale
		width = width * scale
		height = height * scale
		while frame and frame ~= UIParent do
			scale = frame:GetScale()
			x = x * scale
			y = y * scale
			width = width * scale
			height = height * scale
			frame = frame:GetParent()
		end
		frame1.width = width
		frame1.height = height
		frame1.col = x - width / 2
		frame1.row = y - height / 2
		frame1.x1 = frame1.col
		frame1.y1 = frame1.row
		frame1.x2 = frame1.col + frame1.width
		frame1.y2 = frame1.row + frame1.height
	end
	
	frame2.col = -1
	frame2.x1, frame2.y1, frame2.x2, frame2.y2 = -1, -1, -1, -1
	if frame2:GetCenter() then
		local frame = frame2:GetParent()
		local scale = frame2:GetScale()
		local x, y = frame2:GetCenter()
		local width = frame2:GetWidth()
		local height = frame2:GetHeight()
		x = x * scale
		y = y * scale
		width = width * scale
		height = height * scale
		while frame and frame ~= UIParent do
			scale = frame:GetScale()
			x = x * scale
			y = y * scale
			width = width * scale
			height = height * scale
			frame = frame:GetParent()
		end
		frame2.width = width
		frame2.height = height
		frame2.col = x - width / 2
		frame2.row = y - height / 2
		
		frame2.x1 = frame2.col
		frame2.y1 = frame2.row
		frame2.x2 = frame2.col + width
		frame2.y2 = frame2.row + height
	end

    local x1w1, y1w1, x2w1, y2w1;	--/* 1st rectangle */
    local x1w2, y1w2, x2w2, y2w2;	--/* 2nd rectangle */

    if (frame1.x2 == NOCOORD or frame1.y2 == NOCOORD or frame2.x2 == NOCOORD or frame2.y2 == NOCOORD) then
		--/* w1 or w2 is no display widget: no intersection */
		return false;
    end
	
    x1w1 = min(frame1.x1 - frame1xPad1, frame1.x2 + frame1xPad2);
    x2w1 = max(frame1.x1 - frame1xPad1, frame1.x2 + frame1xPad2);
    y1w1 = min(frame1.y1 - frame1yPad1, frame1.y2 + frame1yPad2);
    y2w1 = max(frame1.y1 - frame1yPad1, frame1.y2 + frame1yPad2);
    x1w2 = min(frame2.x1 - frame2xPad1, frame2.x2 + frame2xPad2);
    x2w2 = max(frame2.x1 - frame2xPad1, frame2.x2 + frame2xPad2);
    y1w2 = min(frame2.y1 - frame2yPad1, frame2.y2 + frame2yPad2);
    y2w2 = max(frame2.y1 - frame2yPad1, frame2.y2 + frame2yPad2);
	
    if (x1w2 < x2w1 and x2w2 > x1w1 and y1w2 < y2w1 and y2w2 > y1w1) then
		--/* true: Intersection */
		return true;
    else
		return false;
    end

end

ScriptEnv.Intersect = PluginUtils.Intersect

