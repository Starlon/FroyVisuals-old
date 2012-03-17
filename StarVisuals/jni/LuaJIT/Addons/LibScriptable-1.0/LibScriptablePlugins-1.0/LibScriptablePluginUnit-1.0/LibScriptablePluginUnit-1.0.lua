local MAJOR = "LibScriptablePluginUnit-1.0" 
local MINOR = 24

local PluginUnit = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginUnit then return end
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0")

if not PluginUnit.__index then
	PluginUnit.__index = PluginUnit
end

local Detail = Inspect.Unit.Detail
local cache = setmetatable({}, {__mode="v"})
local ScriptEnv = {}
local GetTime = Inspect.Time.Real

local function capital(str)
	return str and string.upper(string.sub(str, 1, 1)) .. string.sub(str, 2, string.len(str))
end

local function split(str, sep)
        local sep, fields = sep or ":", {}
        local pattern = string.format("([^%s]+)", sep)
        str:gsub(pattern, function(c) fields[#fields+1] = c end)
        return fields
end

--- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginUnit:New(environment)
	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end
	return environment
end

--- afk:	Signals that the unit is AFK. Provided only for the player and the player's groupmembers.
local function UnitAFK(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.afk and cached or Detail(unit)
	cached[id] = details
	return details and details.afk
end
ScriptEnv.UnitAFK = UnitAFK

local afk_times = {}
local function UnitAFKTime(unit)
	if UnitAFK(unit) then
		local id = Inspect.Unit.Lookup(unit)
		local start_time = afk_times[id]
		return start_time and GetTime() - start_time
	end
end
ScriptEnv.UnitAFKTime = UnitAFKTime

table.insert(Event.Unit.Detail.Afk, 
{function(units)
	for id, unit in pairs(units) do
		if UnitAFK(id) then
			afk_times[id] = GetTime()
		else
			afk_times[id] = false
		end
	end
end, "LibScriptablePluginUnit_1_0", "afk" })

LibTimer:New(MAJOR .. ".afk", 1000, false, 
function()
	local list = Inspect.Unit.List()
	for id, v in pairs(list) do
		local detail = Detail(id)
		if not afk_times[id] and detail and detail.afk then
			afk_times[id] = GetTime()
		end
	end
end, nil, 2):Start()

local callings = {mage = "Mage", rogue = "Rogue", cleric = "Cleric", warrior = "Warrior"}
--- calling:	The unit's calling. May be "mage", "rogue", "cleric", or "warrior".
local function UnitCalling(unit) 
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.calling and cached or Detail(unit)
	if details then return callings[details.calling] end
end
ScriptEnv.UnitCalling = UnitCalling
ScriptEnv.UnitClass = UnitCalling

--- charge:	The unit's charge. Provided only for the player.
local function UnitCharge(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.charge and cached or Detail(unit)
	cached[id] = details
	if details then return details.charge end
end
ScriptEnv.UnitCharge = UnitCharge

--- chargeMax:
local function UnitChargeMax(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.chargeMax and cached or Detail(unit)
	cached[id] = details
	if details then return details.chargeMax end
end
ScriptEnv.UnitChargeMax = UnitChargeMax

--- combo:	The unit's combo points. Provided only for the player.
local function UnitCombo(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.combo and cached or Detail(unit)
	cached[id] = details
	if details then return details.combo end
end
ScriptEnv.UnitCombo = UnitCombo

--- comboUnit:	The Unit ID of the unit that the combo points have been placed on. Provided only for the player.
local function UnitComboUnit(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.comboUnit and cached or Detail(unit)
	cached[id] = details
	if details then return details.comboUnit end
end
ScriptEnv.UnitComboUnit = UnitComboUnit

--- energy:	The unit's energy.
local function UnitEnergy(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.energy and cached or Detail(unit)
	cached[id] = details
	if details then return details.energy end
end
ScriptEnv.UnitEnergy = UnitEnergy

local function UnitEnergyMax(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.energyMax and cached or Detail(unit)
	cached[id] = details
	if details then return details.energyMax end
end
ScriptEnv.UnitEnergyMax = UnitEnergyMax

--- factionName: The unit's faction name.
local function UnitFaction(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.faction and cached or Detail(unit)
	cached[id] = details
	return details and details.faction and details.factionName
end
ScriptEnv.UnitFaction = UnitFaction

--- guaranteedLoot:	Signals that this unit guarantees loot on death. Shown in the user interface as a diamond above the portrait.
local function UnitGuaranteedLoot(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.guaranteedLoot and cached or Detail(unit)
	cached[id] = details
	if details then return details.guaranteedLoot end
end
ScriptEnv.UnitGuaranteedLoot = UnitGuaranteedLoot

--- guild:	The unit's guild.
local function UnitGuild(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.guild and cached or Detail(unit)
	cached[id] = details
	if details then return details.guild end
end
ScriptEnv.UnitGuild = UnitGuild

--- health:	The unit's health.
local function UnitHealth(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.health and cached or Detail(unit)
	cached[id] = details
	if details then return details.health end
end
ScriptEnv.UnitHealth = UnitHealth

--- healthCap:	The unit's capped maximum health.
local function UnitHealthCap(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.healthCap and cached or Detail(unit)
	cached[id] = details
	if details then return details.healthCap end
end
ScriptEnv.UnitHealthCap = UnitHealthCap

--- healthMax:	The unit's maximum health.
local function UnitHealthMax(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.healthMax and cached or Detail(unit)
	cached[id] = details
	if details then return details.healthMax end
end
ScriptEnv.UnitHealthMax = UnitHealthMax

--- relation: The unit's relation to you. May be "hostile" or "friendly." Neutral targets will not have this member.
local function UnitRelation(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.relation and cached or Detail(unit)
	cached[id] = details
	return detail and detail.relation
end
ScriptEnv.UnitRelation = UnitRelation

--- level:	The unit's level. May be "??" if the unit is hostile and very high-level.
local function UnitLevel(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.details and cached or Detail(unit)
	cached[id] = details
	if details then return details.level end
end
ScriptEnv.UnitLevel = UnitLevel

--- locationName: The name of the unit's location. Provided only for friendly players.
local function UnitLocation(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.locationName and cached or Detail(unit)
	cached[id] = details
	return detail and details.locationName
end
ScriptEnv.UnitLocation = UnitLocation

--- loot:	The Unit ID that has looting rights to this corpse.
local function UnitLoot(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.loot and cached or Detail(unit)
	cached[id] = details
	if details then return details.loot end
end
ScriptEnv.UnitLoot = UnitLoot

--- mana:	The unit's mana.
local function UnitMana(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.mana and cached or Detail(unit)
	cached[id] = details
	if details then return details.mana end
end
ScriptEnv.UnitMana = UnitMana

--- manaMax:	The unit's maximum mana.
local function UnitManaMax(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.manaMax and cached or Detail(unit)
	cached[id] = details
	if details then return details.manaMax end
end
ScriptEnv.UnitManaMax = UnitManaMax

--- mark:	The mark on this unit.
local function UnitMark(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.mark and cached or Detail(unit)
	cached[id] = details
	if details then return details.mark end
end
ScriptEnv.UnitMark = UnitMark

--- name:	The unit's name.
local function UnitName(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.name and cached or Detail(unit)
	cached[id] = details
	if details then return details.name end
end
ScriptEnv.UnitName = UnitName

--- nameSecondary:	The unit's secondary name.
local function UnitNameSecondary(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.secondary and cached or Detail(unit)
	cached[id] = details
	if details then return details.nameSecondary end
end
ScriptEnv.UnitNameSecondary = UnitNameSecondary

--- offline:	Signals that the unit is offline. Provided only for the player's groupmembers.
local function UnitOffline(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.offline and cached or Detail(unit)
	cached[id] = details
	if details then return details.offline end
end
ScriptEnv.UnitOffline = UnitOffline

local offline_times = {}
local function UnitOfflineTime(unit)
	if UnitOffline(unit) then
		local id = Inspect.Unit.Lookup(unit)
		local start_time = offline_times[id]
		return start_time and GetTime() - start_time
	end
end
ScriptEnv.UnitOfflineTime = UnitOfflineTime

table.insert(Event.Unit.Detail.Offline, {function(units)
	for id, status in pairs(units) do
		if status then
			offline_times[id] = GetTime()
		else
			offline_times[id] = false
		end
	end
end, "LibScriptablePluginUnit_1_0", "offline" })

LibTimer:New(MAJOR .. ".offline", 1000, false,
function()
	local list = Inspect.Unit.List()
	for id, v in pairs(list) do
		local detail = Detail(id)
		if not offline_times[id] and detail and detail.offline then
			offline_times[id] = GetTime()
		end
	end
end, nil, 2):Start()

--- planar:	The unit's available planar charges. Provided only for the player.
local function UnitPlanar(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.planar and cached or Detail(unit)
	cached[id] = details
	if details then return details.planar end
end
ScriptEnv.UnitPlanar = UnitPlanar

--- player:	Signals that the unit is a player, not an NPC.
local function UnitPlayer(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.player and cached or Detail(unit)
	cached[id] = details
	if details then return details.player end
end
ScriptEnv.UnitPlayer = UnitPlayer

--- power:	The unit's power.
local function UnitPower(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.power and cached or Detail(unit)
	cached[id] = details
	if details then return details.power end
end
ScriptEnv.UnitPower = UnitPower

--- publicSize: The unit's current public group size. nil if the group is not public. Provided only for friendly players.
local function UnitPublicSize(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.publicSize and cached or Detail(unit)
	cached[id] = details
	return details and details.publicSize
end
ScriptEnv.UnitPublicSize = UnitPublicSize

--- pvp:	The unit's PvP flag.
local function UnitPVP(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.pvp and cached or Detail(unit)
	--cached[id] = details
	return details and details.pvp
end
ScriptEnv.UnitPVP = UnitPVP

--- ready:	The unit's race.
local function UnitRace(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.race and cached or Detail(unit)
	cached[id] = details
	return details and capital(details.race)	
end
ScriptEnv.UnitRace = UnitRace

--- ready:	The unit's readycheck status.
local function UnitReady(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.ready and cached or Detail(unit)
	cached[id] = details
	return details and details.ready
end
ScriptEnv.UnitReady = UnitReady

local relations = {hostile = "Hostile", friendly = "Friendly"}
--- relation:	The unit's relation to you. May be "hostile" or "friendly". Neutral targets will not have this member.
local function UnitRelation(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.relation and cached or Detail(unit)
	cached[id] = details
	if details then return relations[details.relation] end
end
ScriptEnv.UnitRelation = UnitRelation

local function UnitHostile(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.hostile and cached or Detail(unit)
	cached[id] = details
	if details then return details.relation == "hostile" end
end
ScriptEnv.UnitHostile = UnitHostile

local pc = {r = 123 / 255, g = 171 / 255, b =252 / 255}
local function UnitRelationColor(unit)
	if UnitPlayer(unit) then
		return pc.r, pc.g, pc.b
	else
		if UnitHostile(unit) then
			return 1, 0, 0
		else
			if not UnitRelation(unit) then
				return 1, 1, 0
			else
				return 0, 1, 0
			end
		end
	end
end
ScriptEnv.UnitRelationColor = UnitRelationColor

local roles = {tank = "Tank", heal = "Heals", dps = "DPS", support = "Support"}
--- role:	The unit's role. May be "tank", "heal", "dps", "support", or nil. Provided only for the player and the player's groupmembers.
local function UnitRole(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.role and cached or Detail(unit)
	cached[id] = details
	if details then return roles[details.role] end
end
ScriptEnv.UnitRole = UnitRole

local function UnitTagged(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.tagged and cached or Detail(unit)
	cached[id] = details
	return detail and detail.tagged
end
ScriptEnv.UnitTagged = UnitTagged

--- tier:
local function UnitTier(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.tier and cached or Detail(unit)
	cached[id] = details
	return details and details.tier
end
ScriptEnv.UnitTier = UnitTier

--- titlePrefix:	The unit's title prefix.
local function UnitTitlePrefix(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.titlePrefixName and cached or Detail(unit)
	cached[id] = details
	if details then return details.titlePrefixName end
end
ScriptEnv.UnitTitlePrefix = UnitTitlePrefix

--- titleSuffix:	The unit's title suffix.
local function UnitTitleSuffix(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.titleSuffixName and cached or Detail(unit)
	cached[id] = details
	if details then return details.titleSuffixName end
end
ScriptEnv.UnitTitleSuffix = UnitTitleSuffix

--- tagName: The unit's tags, localized.
local function UnitTag(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.tag and cached or Detail(unit)
	if details and details.tag then
		cached[id] = details
		return split(details.tag, " ")
	end
end
ScriptEnv.UnitTag = UnitTag


--- tagName: The unit's tags, localized.
local function UnitTagText(unit)
	local tags = UnitTag(unit)
	if tags then
		local txt = ""
		for i, v in ipairs(tags) do
			txt = txt .. "<" .. capital(v) .. ">"
		end
		return txt
	end
end
ScriptEnv.UnitTagText = UnitTagText

--- vitality:	The unit's vitality. Provided only for the player.
local function UnitVitality(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.vitality and cached or Detail(unit)
	cached[id] = details
	if details then return details.vitality end
end
ScriptEnv.UnitVitality = UnitVitality

--- warfront:	Signals that the unit has temporarily left the group to join a warfront. Provided only for groupmembers.
local function UnitWarfront(unit)
	local id = Inspect.Unit.Lookup(unit)
	if not id then return end
	local cached = cache[id] or {}
	local details = cached.warfront and cached or Detail(unit)
	cached[id] = details
	if details then return details.warfront end
end
ScriptEnv.UnitWarfront = UnitWarfront

local function UnitCastPercent(unit)
	local info = Inspect.Unit.Castbar(unit)
	if info then
		return (info.remaining or 0) / (info.duration or .00001)
	end
end
ScriptEnv.UnitCastPercent = UnitCastPercent

local function UnitCastName(unit)
	local info = Inspect.Unit.Castbar(unit)
	if info then
		return info.abilityName
	end
end
ScriptEnv.UnitCastName = UnitCastName

local function UnitCastChanneled(unit)
	local info = Inspect.Unit.Castbar(unit)
	if info then
		return info.channeled
	end
end
ScriptEnv.UnitCastChanneled = UnitCastChanneled

table.insert(Event.Unit.Detail.Afk, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].afk = v
end
end, "LibScriptablePluginUnit_1_0", "Defailt.Afk"})

table.insert(Event.Unit.Detail.Charge, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].charge = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Charge"})

table.insert(Event.Unit.Detail.Combat, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].combat = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Combat"})

table.insert(Event.Unit.Detail.Combo, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].combo = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Combo"})

table.insert(Event.Unit.Detail.ComboUnit, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].comboUnit = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.ComboUnit"})

table.insert(Event.Unit.Detail.Energy, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].energy = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Energy"})

table.insert(Event.Unit.Detail.EnergyMax, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].energyMax = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.EnergyMax"})

table.insert(Event.Unit.Detail.Guild, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].guild = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Guild"})

table.insert(Event.Unit.Detail.Health, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].health = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Health"})

table.insert(Event.Unit.Detail.HealthCap, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].healthCap = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.HealthCap"})

table.insert(Event.Unit.Detail.HealthMax, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].healthMax = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.HealthMax"})

table.insert(Event.Unit.Detail.Level, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].level = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Level"})

table.insert(Event.Unit.Detail.LocationName, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].locationName = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.LocationName"})

table.insert(Event.Unit.Detail.Mana, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].mana = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Mana"})

table.insert(Event.Unit.Detail.ManaMax, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].manaMax = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Mark"})

table.insert(Event.Unit.Detail.Mark, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].mark = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Mark"})

table.insert(Event.Unit.Detail.Name, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].name = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Name"})

table.insert(Event.Unit.Detail.Offline, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].offline = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Offline"})

table.insert(Event.Unit.Detail.Planar, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].planar = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Planar"})

table.insert(Event.Unit.Detail.PlanarMax, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].planarMax = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.PlanarMax"})

table.insert(Event.Unit.Detail.Power, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].power = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Power"})

table.insert(Event.Unit.Detail.PublicSize, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].publicSize = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.PublicSize"})

table.insert(Event.Unit.Detail.Pvp, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].pvp = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Pvp"})

table.insert(Event.Unit.Detail.Ready, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].Ready = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Ready"})

table.insert(Event.Unit.Detail.Role, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].Role = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Role"})

table.insert(Event.Unit.Detail.Tagged, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].tagged = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Tagged"})

table.insert(Event.Unit.Detail.TitlePrefix, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].titlePrefix = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.TitlePrefix"})

table.insert(Event.Unit.Detail.TitlePrefixId, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].titlePrefixId = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.TitlePrefixId"})

table.insert(Event.Unit.Detail.TitlePrefixName, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].titlePrefixName = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.PrefixName"})

table.insert(Event.Unit.Detail.TitleSuffix, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].titleSuffix = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.TitleSuffix"})

table.insert(Event.Unit.Detail.TitleSuffixId, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].titleSuffixId = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.TitleSuffixId"})

table.insert(Event.Unit.Detail.TitleSuffixName, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].titleSufixName = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.TitleSuffixName"})

table.insert(Event.Unit.Detail.Vitality, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].vitality = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Vitality"})

table.insert(Event.Unit.Detail.Warfront, {
function(tbl)
for k, v in pairs(tbl) do
	cache[k] = cache[k] or {}
	cache[k].warfront = v
end

end, "LibScriptablePluginUnit_1_0", "Defailt.Warfront"})

