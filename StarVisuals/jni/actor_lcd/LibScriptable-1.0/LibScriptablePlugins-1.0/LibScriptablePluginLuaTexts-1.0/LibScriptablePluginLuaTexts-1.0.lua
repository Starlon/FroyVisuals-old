-- Huge appreciation goes out to ckknight for 99% of the following.
local MAJOR = "LibScriptablePluginLuaTexts-1.0"
local MINOR = 24

local PluginLuaTexts = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginLuaTexts then return end
local PluginUtils = LibStub("LibScriptablePluginUtils-1.0", true)
assert(PluginUtils, MAJOR .. " requires LibScriptablePluginUtils-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local PluginColor = LibStub("LibScriptablePluginColor-1.0")
assert(PluginColor, MAJOR .. " requires LibScriptablePluginColor-1.0")
local Locale = LibStub("LibScriptableLocale-1.0", true)
assert(Locale, MAJOR .. " requires LibScriptableLocale-1.0")
local L = Locale.L

local _G = _G
local ScriptEnv = {}

ScriptEnv.Colorize = PluginColor.Colorize

if not PluginLuaTexts.__index then
	PluginLuaTexts.__index = PluginLuaTexts
end

--- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment, and the plugin object as second return value
function PluginLuaTexts:New(environment)
	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end
	
	return environment
end

DAYS = "Day";
DAYS_P1 = "Days";
DAYS_ABBR = "Day";
DAYS_ABBR_P1 = "Days";
DAY_ONELETTER_ABBR = "%d d";
HOURS = "Hour";
HOURS_P1 = "Hours";
HOURS_ABBR = "Hr";
HOURS_ABBR_P1 = "Hrs";
HOUR_ONELETTER_ABBR = "%d h";
MINUTES = "Minute"; -- Minutes of time
MINUTES_P1 = "Minutes";
MINUTES_ABBR = "Min";
MINUTES_ABBR_P1 = "Mins";
MINUTE_ONELETTER_ABBR = "%d m";
SECONDS = "Second"; -- Seconds of time
SECONDS_P1 = "Seconds";
SECONDS_ABBR = "Sec";
SECONDS_ABBR_P1 = "Secs";
SECOND_ONELETTER_ABBR = "%d s";

local L_DAY_ONELETTER_ABBR    = DAY_ONELETTER_ABBR:gsub("%s*%%d%s*", "")

local L_HOUR_ONELETTER_ABBR   = HOUR_ONELETTER_ABBR:gsub("%s*%%d%s*", "")

local L_MINUTE_ONELETTER_ABBR = MINUTE_ONELETTER_ABBR:gsub("%s*%%d%s*", "")

local L_SECOND_ONELETTER_ABBR = SECOND_ONELETTER_ABBR:gsub("%s*%%d%s*", "")

local L_DAYS_ABBR = DAYS_ABBR:gsub("%s*%%d%s*","")

local L_HOURS_ABBR = HOURS_ABBR:gsub("%s*%%d%s*","")

local L_MINUTES_ABBR = MINUTES_ABBR:gsub("%s*%%d%s*","")

local L_SECONDS_ABBR = SECONDS_ABBR:gsub("%s*%%d%s*","")


local tremove = table.remove
local wipe = function(tbl) 
	for i = 1, #tbl do
		tremove(tbl)
	end
end
local t = {}
local function FormatDuration(number, format)
	local negative = ""
	if number < 0 then
		number = -number
		negative = "-"
	end

	if not format then
		format = 'c'
	else
		format = format:sub(1, 1):lower()
	end

	if format == "e" then
		if number == 1/0 then
			return negative .. "***"
		end

		t[#t+1] = negative

		number = math.floor(number + 0.5)

		local first = true

		if number >= 60*60*24 then
			local days = math.floor(number / (60*60*24))
			number = number % (60*60*24)
			t[#t+1] = ("%.0f"):format(days)
			t[#t+1] = " "
			t[#t+1] = _L_DAYS_ABBR
			first = false
		end

		if number >= 60*60 then
			local hours = math.floor(number / (60*60))
			number = number % (60*60)
			if not first then
				t[#t+1] = " "
			else
				first = false
			end
			t[#t+1] = hours
			t[#t+1] = " "
			t[#t+1] = L_HOURS_ABBR
		end

		if number >= 60 then
			local minutes = math.floor(number / 60)
			number = number % 60
			if not first then
				t[#t+1] = " "
			else
				first = false
			end
			t[#t+1] = minutes
			t[#t+1] = " "
			t[#t+1] = L_MINUTES_ABBR
		end

		if number >= 1 or first then
			local seconds = number
			if not first then
				t[#t+1] = " "
			else
				first = false
			end
			t[#t+1] = seconds
			t[#t+1] = " "
			t[#t+1] = L_SECONDS_ABBR
		end
		local s = table.concat(t)
		wipe(t)
		return s
	elseif format == "f" then
		if number == 1/0 then
			return negative .. "***"
		elseif number >= 60*60*24 then
			return ("%s%.0f%s %02d%s %02d%s %02d%s"):format(negative, math.floor(number/86400), L_DAY_ONELETTER_ABBR, number/3600 % 24, L_HOUR_ONELETTER_ABBR, number/60 % 60, L_MINUTE_ONELETTER_ABBR, number % 60, L_SECOND_ONELETTER_ABBR)
		elseif number >= 60*60 then
			return ("%s%d%s %02d%s %02d%s"):format(negative, number/3600, L_HOUR_ONELETTER_ABBR, number/60 % 60, L_MINUTE_ONELETTER_ABBR, number % 60, L_SECOND_ONELETTER_ABBR)
		elseif number >= 60 then
			return ("%s%d%s %02d%s"):format(negative, number/60, L_MINUTE_ONELETTER_ABBR, number % 60, L_SECOND_ONELETTER_ABBR)
		else
			return ("%s%d%s"):format(negative, number, L_SECOND_ONELETTER_ABBR)
		end
	elseif format == "s" then
		if number == 1/0 then
			return negative .. "***"
		elseif number >= 2*60*60*24 then
			return ("%s%.1f %s"):format(negative, number/86400, L_DAYS_ABBR)
		elseif number >= 2*60*60 then
			return ("%s%.1f %s"):format(negative, number/3600, L_HOURS_ABBR)
		elseif number >= 2*60 then
			return ("%s%.1f %s"):format(negative, number/60, L_MINUTES_ABBR)
		elseif number >= 3 then
			return ("%s%.0f %s"):format(negative, number, L_SECONDS_ABBR)
		else
			return ("%s%.1f %s"):format(negative, number, L_SECONDS_ABBR)
		end
	else
		if number == 1/0 then
			return ("%s**%d **:**:**"):format(negative, L_DAY_ONELETTER_ABBR)
		elseif number >= 60*60*24 then
			return ("%s%.0f%s %d:%02d:%02d"):format(negative, math.floor(number/86400), L_DAY_ONELETTER_ABBR, number/3600 % 24, number/60 % 60, number % 60)
		elseif number >= 60*60 then
			return ("%s%d:%02d:%02d"):format(negative, number/3600, number/60 % 60, number % 60)
		else
			return ("%s%d:%02d"):format(negative, number/60 % 60, number % 60)
		end
	end
end
ScriptEnv.FormatDuration = FormatDuration

-- Depends upon the local t = {} above FormatDuration
local function SeparateDigits(number, thousands, decimal)
	if not thousands then
		thousands = ','
	end
	if not decimal then
		decimal = '.'
	end
	local int = math.floor(number)
	local rest = number % 1
	if int == 0 then
		t[#t+1] = 0
	else
		local digits = math.log10(int)
		local segments = math.floor(digits / 3)
		t[#t+1] = math.floor(int / 1000^segments)
		for i = segments-1, 0, -1 do
			t[#t+1] = thousands
			t[#t+1] = ("%03d"):format(math.floor(int / 1000^i) % 1000)
		end
	end
	if rest ~= 0 then
		t[#t+1] = decimal
		rest = math.floor(rest * 10^6)
		while rest % 10 == 0 do
			rest = rest / 10
		end
		t[#t+1] = rest
	end
	local s = table.concat(t)
	wipe(t)
	return s
end
ScriptEnv.SeparateDigits = SeparateDigits

local function Angle(value)
	if value and value ~= '' then
		return '<' .. value .. '>'
	else
		return ''
	end
end
ScriptEnv.Angle = Angle

local function Paren(value)
	if value and value ~= '' then
		return '(' .. value .. ')'
	else
		return ''
	end
end
ScriptEnv.Paren = Paren

--[[
local function IsAFK(unit)
	return not not afk_times[UnitGUID(unit)]
end
ScriptEnv.IsAFK = IsAFK
]]

--[[
local function AFKDuration(unit)
	local afk = afk_times[UnitGUID(unit)]
	if afk then
		--UpdateIn(0.25)
		return GetTime() - afk
	end
end
ScriptEnv.AFKDuration = AFKDuration
]]

--[[
local function AFK(unit)
	local afk = AFKDuration(unit)
	if afk then
		return _G.AFK..' ('..FormatDuration(afk)..')'
	end
end
ScriptEnv.AFK = AFK

local function IsDND(unit)
	return not not dnd_times[UnitGUID(unit)]
end
ScriptEnv.IsDND = IsDND

local function DND(unit)
	if dnd[UnitGUID(unit)] then
		return _G.DND
	end
end
ScriptEnv.DND = DND

local function Level(unit)
	local level = UnitLevel(unit)
	if level <= 0 then
		level = '??'
	end
	return level
end
ScriptEnv.Level = Level


local function IsPet(unit)
	return not UnitIsPlayer(unit) and (UnitPlayerControlled(unit) or UnitPlayerOrPetInRaid(unit))
end
ScriptEnv.IsPet = IsPet

local function OfflineDuration(unit)
	local offline = offline_times[UnitGUID(unit)]
	if offline then
		--UpdateIn(0.25)
		return GetTime() - offline
	end
end
ScriptEnv.OfflineDuration = OfflineDuration

local function Offline(unit)
 	local offline = OfflineDuration(unit)
	if offline then
		return L["Offline"]..' ('..FormatDuration(offline)..')'
	end
end
ScriptEnv.Offline = Offline

local function IsOffline(unit)
	return not not offline_times[UnitGUID(unit)]
end
ScriptEnv.IsOffline = IsOffline

local function DeadDuration(unit)
	local dead_time = dead_times[UnitGUID(unit)]
	if dead_time then
		--UpdateIn(0.25)
		return GetTime() - dead_time
	end
end
ScriptEnv.DeadDuration = DeadDuration

local function Dead(unit)
	local dead_time = DeadDuration(unit)
	local dead_type = (UnitIsGhost(unit) and L["Ghost"]) or (UnitIsDead(unit) and L["Dead"])
	if dead_time and dead_type then
		return dead_type..' ('..FormatDuration(dead_time)..')'
	elseif dead_type then
		return dead_type
	end
end
ScriptEnv.Dead = Dead

local function HP(unit)
	local hp = UnitHealth(unit)
	if hp == 1 and UnitIsGhost(unit) then
		return 0
	end
	return hp
end
ScriptEnv.HP = HP
ScriptEnv.Health = HP

-- Just use the Blizzard API no change needed
-- only reason this is here is for symmetry,
-- it feels weird to have HP (which we need
-- to avoid the hp = 1 while dead crap), but
-- not have MaxHP
local MaxHP = UnitHealthMax
ScriptEnv.MaxHP = MaxHP
ScriptEnv.MaxHealth = MaxHP

local function Power(unit, power_type)
	local power = UnitPower(unit, power_type)

	-- Detect mana texts for player and pet units, cache the power
	-- and mark the font_strings for faster updating.  Allows
	-- smoothing updating of PowerBars.
	local guid = UnitGUID(unit)
	if power_type == nil or UnitPowerType(unit) == power_type then
		if guid == ScriptEnv.player_guid then
			ScriptEnv.player_power = power
		elseif guid == UnitGUID("pet") then
			ScriptEnv.pet_power = power
		end
	end

	return power
end
ScriptEnv.Power = Power

-- more symmetry
local MaxPower = UnitPowerMax
ScriptEnv.MaxPower = MaxPower
]]

local function Round(number, digits)
	if not digits then
		digits = 0
	end
	local mantissa = 10^digits
	local norm = number*mantissa + 0.5
	local norm_floor = math.floor(norm)
	if norm == norm_floor and (norm_floor % 2) == 1 then
		return (norm_floor-1) / mantissa
	else
		return norm_floor / mantissa
	end
end
ScriptEnv.Round = Round

local function Short(value,format)
	if type(value) == "number" then
		local fmt
		if value >= 10000000 or value <= -10000000 then
			fmt = "%.1fm"
			value = value / 1000000
		elseif value >= 1000000 or value <= -1000000 then
			fmt = "%.2fm"
			value = value / 1000000
		elseif value >= 100000 or value <= -100000 then
			fmt = "%.0fk"
			value = value / 1000
		elseif value >= 10000 or value <= -10000 then
			fmt = "%.1fk"
			value = value / 1000
		else
			fmt = "%d"
			value = math.floor(value+0.5)
		end
		if format then
			return fmt:format(value)
		else
			return fmt, value
		end
	else
		local fmt_a, fmt_b
		local a,b = value:match("^(%d+)/(%d+)$")
		if a then
			a, b = tonumber(a), tonumber(b)
			if a >= 10000000 or a <= -10000000 then
				fmt_a = "%.1fm"
				a = a / 1000000
			elseif a >= 1000000 or a <= -1000000 then
				fmt_a = "%.2fm"
				a = a / 1000000
			elseif a >= 100000 or a <= -100000 then
				fmt_a = "%.0fk"
				a = a / 1000
			elseif a >= 10000 or a <= -10000 then
				fmt_a = "%.1fk"
				a = a / 1000
			end
			if b >= 10000000 or b <= -10000000 then
				fmt_b = "%.1fm"
				b = b / 1000000
			elseif b >= 1000000 or b <= -1000000 then
				fmt_b = "%.2fm"
				b = b / 1000000
			elseif b >= 100000 or b <= -100000 then
				fmt_b = "%.0fk"
				b = b / 1000
			elseif b >= 10000 or b <= -10000 then
				fmt_b = "%.1fk"
				b = b / 1000
			end
			if format then
				return (fmt_a.."/"..fmt_b):format(a,b)
			else
				return fmt_a.."/"..fmt_b,a,b
			end
		else
			return value
		end
	end
end
ScriptEnv.Short = Short

local function VeryShort(value,format)
	if type(value) == "number" then
		local fmt
		if value >= 1000000 or value <= -1000000 then
			fmt = "%.0fm"
			value = value / 1000000
		elseif value >= 1000 or value <= -1000 then
			fmt = "%.0fk"
			value = value / 1000
		else
			fmt = "%.0f"
		end
		if format then
			return fmt:format(value)
		else
			return fmt, value
		end
	else
		local a,b = value:match("^(%d+)/(%d+)")
		if a then
			local fmt_a, fmt_b
			a, b = tonumber(a), tonumber(b)
			if b >= 1000000 or b <= -1000000 then
				fmt_b = "%.0fm"
				b = b / 1000000
			elseif b >= 1000 or b <= -1000 then
				fmt_b = "%.0fk"
				b = b / 1000
			end
			if a >= 1000000 or a <= -1000000 then
				fmt_a = "%.0fm"
				a = a / 1000000
			elseif a >= 1000 or a <= -1000 then
				fmt_a = "%.0fk"
				a = a / 1000
			end
			if format then
				return (fmt_a.."/"..fmt_b):format(a,b)
			else
				return fmt_a.."/"..fmt_b,a,b
			end
		else
			return value
		end
	end
end
ScriptEnv.VeryShort = VeryShort

local function Percent(x, y)
	if y ~= 0 then
		return Round(x / y * 100,1)
	else
		return 0
	end
end
ScriptEnv.Percent = Percent

--[[
local function XP(unit)
	if unit == "player" then
		return UnitXP(unit)
	elseif unit == "pet" or unit == "playerpet" then
		return GetPetExperience()
	else
		return 0
	end
end
ScriptEnv.XP = XP

local function MaxXP(unit)
	if unit == "player" then
		return UnitXPMax(unit)
	elseif unit == "pet" or unit == "playerpet" then
		local _, max = GetPetExperience()
		return max
	else
		return 0
	end
end
ScriptEnv.MaxXP = MaxXP

local function RestXP(unit)
	if unit == "player" then
		return GetXPExhaustion() or 0
	else
		return 0
	end
end
ScriptEnv.RestXP = RestXP

local function CastData(unit)
	return cast_data[UnitGUID(unit)]
end
ScriptEnv.CastData = CastData
]]

local function abbreviate(text)
	local b = text:byte(1)
	if b <= 127 then
		return text:sub(1, 1)
	elseif b <= 223 then
		return text:sub(1, 2)
	elseif b <= 239 then
		return text:sub(1, 3)
	else
		return text:sub(1, 4)
	end
end
local function Abbreviate(value)
    if value:find(" ") then
      return value:gsub(" *([^ ]+) *", abbreviate)
    else
      return value
    end
end
ScriptEnv.Abbreviate = Abbreviate

--[[
local function PVPDuration(unit)
	if unit and not UnitIsUnit(unit,"player") then return end
  if IsPVPTimerRunning() then
		--UpdateIn(0.25)
		return GetPVPTimer()/1000
	end
end
ScriptEnv.PVPDuration = PVPDuration

]]
-----------------End of ScriptEnv---------------------

for k, v in pairs(ScriptEnv) do
	PluginLuaTexts[k] = v
end

