local MAJOR = "LibScriptableLocale-1.0"
local MINOR = 21+1
assert(LibStub, MAJOR.." requires LibStub")

local LibLocale = LibStub:NewLibrary(MAJOR, MINOR)
if not LibLocale then return end

local function GetLocale() end

if GetLocale() == "deDE" then
	LibLocale.L = LibStub("LibScriptableLocale-deDE-1.0").L

elseif GetLocale() == "esES" then
	LibLocale.L = LibStub("LibScriptableLocale-esES-1.0").L

elseif GetLocale() == "esMX" then
	LibLocale.L = LibStub("LibScriptableLocale-esMX-1.0").L

elseif GetLocale() == "frFR" then
	LibLocale.L = LibStub("LibScriptableLocale-frFR-1.0").L

elseif GetLocale() == "koKR" then
	LibLocale.L = LibStub("LibScriptableLocale-koKR-1.0").L

elseif GetLocale() == "ruRU" then
	LibLocale.L = LibStub("LibScriptableLocale-ruRU-1.0").L

elseif GetLocale() == "zhCN" then
	LibLocale.L = LibStub("LibScriptableLocale-zhCN-1.0").L

elseif GetLocale() == "zhTW" then
	LibLocale.L = LibStub("LibScriptableLocale-zhTW-1.0").L

else
	LibLocale.L = LibStub("LibScriptableLocale-enUS-1.0").L
end

