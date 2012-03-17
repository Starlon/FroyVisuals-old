local MAJOR = "LibScriptablePluginAchievement-1.0" 
local MINOR = 24

local PluginAchievement = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginAchievement then return end
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0")

if not PluginAchievement.__index then
	PluginAchievement.__index = PluginAchievement
end

local Detail = Inspect.Achievement.Detail
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
function PluginAchievement:New(environment)
	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end
	return environment
end


---category:	ID of the achievement's category.
local function AchievementCategory(achieve)
	local details = Detail(achieve)
	return details and details.category
end
ScriptEnv.AchievementCategory = AchievementCategory

---complete:	true is the achievement is completed by this character. If the achievement has been completed by another of the player's characters, gives that character's name.
local function AchievementComplete(achieve)
	local details = Detail(achieve)
	return details and details.complete
end
ScriptEnv.AchievementComplete = AchievementComplete

---description:	The achievement's description.
local function AchievementDescription(achieve)
	local details = Detail(achieve)
	return details and details.description
end
ScriptEnv.AchievementDescription = AchievementDescription

---faction:	The faction that this achievement requires, either "guardian", "defiant", or nil.
local function AchievementFaction(achieve)
	local details = Detail(achieve)
	return details and details.faction
end
ScriptEnv.AchievementFaction = AchievementFaction

---icon:	Internal name of the achievement's icon.
local function AchievementIcon(achieve)
	local details = Detail(achieve)
	return details and details.icon
end
ScriptEnv.AchievementIcon = AchievementIcon

---name:	The achievement's name.
local function AchievementName(achieve)
	local details = Detail(achieve)
	return details and details.name
end
ScriptEnv.AchievementName = AchievementName

---previous:	The ID of the achievement immediately previous to this in a chain.
local function AchievementPrevious(achieve)
	local details = Detail(achieve)
	return details and details.Previous
end
ScriptEnv.AchievementPrevious = AchievementPrevious

---requirement:	Table listing the requirements for this achievement. Each item may include multiple members.
---		type: The type of the requirement. Valid values include "achievement", "artifactset", "discover", "event", "quest", and "tradeskill".
---		name: The name of the requirement.
---		count: The count required for completion.
---		countDone: The count already completed.
---		complete: Signals that this requirement is complete.
---		id: The id of whatever this requires.
local function AchievementRequirement(achieve)
	local details = Detail(achieve)
	return details and details.requirement
end
ScriptEnv.AchievementRequirement = AchievementRequirement

---score:	The number of points this achievement awards for completion.
local function AchievementScore(achieve)
	local details = Detail(achieve)
	return details and details.score
end
ScriptEnv.AchievementScore = AchievementScore

---sort:	A number indicating the order that this achievement should be sorted in.
local function AchievementSort(achieve)
	local details = Detail(achieve)
	return details and details.sort
end
ScriptEnv.AchievementSort = AchievementSort

---title:	The ID of the title this achievement awards.
local function AchievementTitle(achieve)
	local details = Detail(achieve)
	return details and details.title
end
ScriptEnv.AchievementTitle = AchievementTitle


