local MAJOR = "LibScriptableImagesPNM-1.0"
local MINOR = 24

local PNM = LibStub:NewLibrary(MAJOR, MINOR)
if not PNM then return end
PluginUtils = LibStub("LibScriptablePluginUtils-1.0", true)
assert(PluginUtils, MAJOR .. " requires LibScriptablePluginUtils-1.0")
PluginColor = LibStub("LibScriptablePluginColor-1.0", true)
assert(PluginColor, MAJOR .. " requires LibScriptablePluginColor-1.0")
PluginColor = PluginColor:New({})
LibWidget = LibStub("LibScriptableWidget-1.0")
assert(LibWidget, MAJOR .. " requires LibScriptableWidget-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L


local defaults = {pixel = 1}
PNM.defaults = defaults

--- Parse a PNM image and return the data
-- @usage :New(name, config, errorLevel)
-- @param name A name for the image
-- @param config This image's parameters
-- @errorLevel Error verboseness
-- @return A new PNM object. This will have one of these three fields: bitmap, grayimage, colorimage. It will also have w, h, and n.
function PNM:New(name, config, errorLevel)	
	local obj = {}

	local preImage = PluginUtils.Split(config.pnm or "", "\n")
	local image = {}
	local bitmap
	local grayimage
	local colorimage
	local endComments = 0
	image[1] = preImage[1]
	while preImage[2 + endComments]:sub(1, 1) == "#" do
		endComments = endComments + 1
	end
	image[2] = preImage[2 + endComments]
	image[3] = preImage[3 + endComments]
	
	for i = 4, #preImage - endComments do
		local line = preImage[i + endComments]
		local segments = PluginUtils.Split(line, " ")
		if #segments > 0 then
			for j, segment in ipairs(segments) do
				tinsert(image, segment)
			end
		else
			tinsert(image, line)
		end
	end
	
	if image[1] == "P1" then
		bitmap = {type=image[1]}
		local wh = PluginUtils.Split(image[2], " ")
		local w = tonumber(wh[1])
		local h = tonumber(wh[2])
		obj.w = w
		obj.h = h
		for x = 0 , w - 1 do
			for y = 0, h - 1 do
				if image[x * h + y + 2] == "1" then
					bitmap[y * w + x] = true
				end
			end
		end
	elseif image[1] == "P2" then
		grayimage = {type=image[1]}
		
		local wh = PluginUtils.Split(image[2], " ")
		w = tonumber(wh[1])
		h = tonumber(wh[2])
		n = tonumber(image[3])
		obj.w = w
		obj.h = h
		obj.n = n
		for x = 0, w - 1 do
			for y = 0, h - 1 do
				grayimage[y * w + x] = tonumber(image[y * w + x + 3])
			end
		end
	elseif image[1] == "P3" then
		colorimage = {type=image[1]}
		local wh = PluginUtils.Split(image[2], " ")
		w = tonumber(string.trim(wh[1]))
		h = tonumber(string.trim(wh[2]))
		n = tonumber(string.trim(image[3]))
		obj.w = w
		obj.h = h
		obj.n = n
		for x = 0, w - 1 do
			for y = 0, h - 1 do
				colorimage[y * w + x] = {}
				colorimage[y * w + x].b = tonumber(string.trim(image[(y * w + x) * 3 + 3]))
				colorimage[y * w + x].r = tonumber(string.trim(image[(y * w + x) * 3 + 3 + 1]))
				colorimage[y * w + x].g = tonumber(string.trim(image[(y * w + x) * 3 + 3 + 2]))
			end
		end
	else
		return nil
	end
	obj.bitmap = bitmap
	obj.grayimage = grayimage
	obj.colorimage = colorimage
	
	obj.pixel = config.pixel or defaults.pixel
	obj.config = config
	
	return obj

end

--- Get an Ace3 option table. Plug this into a group type's args.
-- @param db The database table
-- @param callback Provide this if you want to execute the callback once an option is changed
-- @param data Some data to pass when executing the callback
-- @return An Ace3 options table -- `name.args = options`.
function PNM:GetOptions(db, callback, data)
	local options = {
		enabled = {
			name = "Enabled",
			desc = "Whether the histogram's enabled or not",
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
		pixel = {
			name = "Pixel Size",
			type = "input",
			pattern = "%d",
			get = function() return tostring(db.pixel or defaults.pixel) end,
			set = function(info, v)
				db.pixel = tonumber(v)
				db.pixelDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 2
		},
		pnm = {
			name = "PNM Data",
			type = "input",
			multiline=true,
			width="full",
			get = function() return db.pnm end,
			set = function(info, v)
				db.pnm = v
				db.pnmDirty = true
				if type(callback) == "function" then
					callback(data)
				end
			end,
			order = 3
		}
	}
	options.widget = LibWidget:GetOptions(db, callback, data)
	return options
end
