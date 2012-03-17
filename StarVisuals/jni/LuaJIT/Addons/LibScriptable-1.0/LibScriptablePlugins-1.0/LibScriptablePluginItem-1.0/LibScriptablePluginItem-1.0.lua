local MAJOR = "LibScriptablePluginItem-1.0" 
local MINOR = 24

local PluginItem = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginItem then return end
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0")

if not PluginItem.__index then
	PluginItem.__index = PluginItem
end

local Detail = Inspect.Item.Detail
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
function PluginItem:New(environment)
	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end
	return environment
end

---bind:	The item's binding type. May be "equip", "use", "pickup", or "account".
local function ItemBind(item)
	local details = Inspect.Item.Detail(item)
	return details and details.bind
end
ScriptEnv.ItemBind = ItemBind

---bound:	The item's bound flag.
local function ItemBound(item)
	local details = Inspect.Item.Detail(item)
	return details and details.bound

end
ScriptEnv.ItemBound = ItemBound

---category:	The item's type category.
local function ItemCategory(item)
	local details = Inspect.Item.Detail(item)
	return details and details.category

end
ScriptEnv.ItemCategory = ItemCategory

---cooldown:	The cooldown for using this item.
local function ItemCooldown(item)
	local details = Inspect.Item.Detail(item)
	return details and details.cooldown

end
ScriptEnv.ItemCooldown = ItemCooldown

---cooldownBegin:	The time the current cooldown started, in the context of Inspect.Time.Frame.
local function ItemCooldownBegin(item)
	local details = Inspect.Item.Detail(item)
	return details and details.cooldownBegin

end
ScriptEnv.ItemCooldownBegin = ItemCooldownBegin

---cooldownDuration:	Duration of the current cooldown the item is influenced by, in seconds.
local function ItemCooldownDuration(item)
	local details = Inspect.Item.Detail(item)
	return details and details.cooldownDuration

end
ScriptEnv.ItemCooldownDuration = ItemCooldownDuration

---cooldownExpired:	Number of seconds the current cooldown is past its expiration time. Generally indicates lag.
local function ItemCooldownExpired(item)
	local details = Inspect.Item.Detail(item)
	return details and details.cooldownExpired

end
ScriptEnv.ItemCooldownExpired = ItemCooldownExpired

---cooldownRemaining:	Time remaining in the item's current cooldown, in seconds.
local function ItemCooldownRemaining(item)
	local details = Inspect.Item.Detail(item)
	return details and details.cooldownRemaining

end
ScriptEnv.ItemCooldownRemaining = ItemCooldownRemaining

---crafter:	The name of the player who crafted this item.
local function ItemCrafter(item)
	local details = Inspect.Item.Detail(item)
	return details and details.crafter

end
ScriptEnv.ItemCrafter = ItemCrafter

---damageDelay:	If a weapon, the delay between autoattacks using this weapon.
local function ItemDamageDelay(item)
	local details = Inspect.Item.Detail(item)
	return details and details.damageDelay

end
ScriptEnv.ItemDamageDelay = ItemDamageDelay

---damageMax:	If a weapon, the maximum damage done by a single hit with this item.
local function ItemDamageMax(item)
	local details = Inspect.Item.Detail(item)
	return details and details.damageMax

end
ScriptEnv.ItemDamageMax = ItemDamageMax

---damageMin:	If a weapon, the minimum damage done by a single hit with this item.
local function ItemDamageMin(item)
	local details = Inspect.Item.Detail(item)
	return details and details.damageMin

end
ScriptEnv.ItemDamageMin = ItemDamageMin

---damageType:	If a weapon, the damage type done by autoattacks. Values include "life", "death", "air", "earth", "fire", and "water".
local function ItemDamageType(item)
	local details = Inspect.Item.Detail(item)
	return details and details.damageType

end
ScriptEnv.ItemDamageType = ItemDamageType

---description:	The description of this item.
local function ItemDescription(item)
	local details = Inspect.Item.Detail(item)
	return details and details.description

end
ScriptEnv.ItemDescription = ItemDescription

---flavor:	The flavor text for this item.
local function ItemFlavor(item)
	local details = Inspect.Item.Detail(item)
	return details and details.flavor

end
ScriptEnv.ItemFlavor = ItemFlavor

---icon:	Resource filename of the item's icon.
local function ItemIcon(item)
	local details = Inspect.Item.Detail(item)
	return details and details.icon

end
ScriptEnv.ItemIcon = ItemIcon

---lootable:	Indicates that the item contains loot.
local function ItemLootable(item)
	local details = Inspect.Item.Detail(item)
	return details and details.lootable

end
ScriptEnv.ItemLootable = ItemLootable

---name:	The item's name.
local function ItemName(item)
	local details = Inspect.Item.Detail(item)
	return details and details.name

end
ScriptEnv.ItemName = ItemName

---range:	If a ranged weapon, the maximum range of this item.
local function ItemRange(item)
	local details = Inspect.Item.Detail(item)
	return details and details.range

end
ScriptEnv.ItemRange = ItemRange

---rarity:	The item's rarity. Values include "trash", "uncommon", "rare", "epic", "relic", or "quest". Common items have a rarity of nil.
local function ItemRarity(item)
	local details = Inspect.Item.Detail(item)
	return details and details.rarity

end
ScriptEnv.ItemRarity = ItemRarity

---requiredCalling:	Space-delimited list of the required callings to use this item.
local function ItemRequiredCalling(item)
	local details = Inspect.Item.Detail(item)
	return details and details.requiredCalling

end
ScriptEnv.ItemRequiredCalling = ItemRequiredCalling

---requiredFaction:	The ID of the faction required to use this item.
local function ItemRequiredFaction(item)
	local details = Inspect.Item.Detail(item)
	return details and details.requiredFaction

end
ScriptEnv.ItemRequiredFaction = ItemRequiredFaction

---requiredFactionLevel:	The faction notoriety required to use this item.
local function ItemRequiredFactionLevel(item)
	local details = Inspect.Item.Detail(item)
	return details and details.requiredFactionLevel

end
ScriptEnv.ItemRequiredFactionLevel = ItemRequiredFactionLevel

---requiredLevel:	The level required to use this item.
local function ItemRequiredLevel(item)
	local details = Inspect.Item.Detail(item)
	return details and details.requiredLevel

end
ScriptEnv.ItemRequiredLevel = ItemRequiredLevel

---requiredPrestige:	The prestige rank required to use this item.
local function ItemRequiredPrestige(item)
	local details = Inspect.Item.Detail(item)
	return details and details.requiredPrestige

end
ScriptEnv.ItemRequiredPrestige = ItemRequiredPrestige

---requiredSkill:	The skill required to use this item.
local function ItemRequiredSkill(item)
	local details = Inspect.Item.Detail(item)
	return details and details.requiredSkill

end
ScriptEnv.ItemRequiredSkill = ItemRequiredSkill

---requiredSkillLevel:	The skill level required to use this item.
local function ItemRequiredSkillLevel(item)
	local details = Inspect.Item.Detail(item)
	return details and details.requiredSkillLevel

end
ScriptEnv.ItemRequiredSkillLevel = ItemRequiredSkillLevel

---sell:	The sell value of this item, in silver.
local function ItemSell(item)
	local details = Inspect.Item.Detail(item)
	return details and details.sell

end
ScriptEnv.ItemSell = ItemSell

---slots:	If a container, the number of slots that this item can contain.
local function ItemSlots(item) 
	local details = Inspect.Item.Detail(item)
	return details and details.slots

end
ScriptEnv.ItemSlots = ItemSlots

---stack:	The size of this item stack.
local function ItemStack(item)
	local details = Inspect.Item.Detail(item)
	return details and details.stack

end
ScriptEnv.ItemStack = ItemStack

---stackMax:	The maximum size of this item stack.
local function ItemStackMax(item)
	local details = Inspect.Item.Detail(item)
	return details and details.stackMax

end
ScriptEnv.ItemStackMax = ItemStackMax

---stats:	The base stats of this item. Members may include "block", "critAttack", "critSpell", "dexterity", "dodge", "endurance", "energyMax", "energyRegen", "focus", "hit", "intelligence", "manaMax", "manaRegen", "movement", "parry", "powerAttack", "powerMax", "powerRegen", "powerSpell", "resistAir", "resistDeath", "resistEarth", "resistFire", "resistLife", "resistWater", "stealth", "stealthDetect", "strength", "wisdom", and "xp".
local function ItemStats(item)
	local details = Inspect.Item.Detail(item)
	return details and details.stats

end
ScriptEnv.ItemStats = ItemStats

---statsRune:	The added rune stats of this item. May contain the same members as stats.
local function ItemStatsRune(item)
	local details = Inspect.Item.Detail(item)
	return details and details.statsRune

end
ScriptEnv.ItemStatsRune = ItemStatsRune

---statsRuneTemporary:	The added temporary rune stats of this item. May contain the same members as stats.
local function ItemStatsRuneTemporary(item)
	local details = Inspect.Item.Detail(item)
	return details and details.statsRuneTemporary

end
ScriptEnv.ItemStatsRuneTemporary = ItemStatsRuneTemporary

---type:	The item's type specifier.
local function ItemType(item)
	local details = Inspect.Item.Detail(item)
	return details and details.type

end
ScriptEnv.ItemType = ItemType



























