local MAJOR = "LibScriptablePluginSimpleMeter-1.0"
local MINOR = 24
local PluginSimpleMeter = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginSimpleMeter then return end

local UnitDPS = LibStub("LibScriptablePluginDPS-1.0"):New({}).UnitDPS

local NAME = "<SimpleMeter>"
local _G = _G

local ScriptEnv = {}

-- Populate an environment with this plugin's fields
-- @usage :New(environment)
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment, and the plugin object as second return
function PluginSimpleMeter:New(environment)

	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end	

	return environment
end


-- Credits go to Jor. This comes from SimpleMeter's BuildCopyText method for encounters.
-- Friendly and hostile checks are performed internally. 
-- Provide 'mode' and 'expand'. 
-- 'mode' is the report requested. DPS, Damage done, healing done, damage taken, heal taken, and dps otherwise. 
-- And 'expand' is the list. 
-- 'self' points to the current encounter.
-- 'top5' looks at the top 5 units.
-- 'all' will look at everything.
-- mode: dps, dmg, hps, heal, dtk, htk
-- expand: all, self, top5
local mode, expand = "dps", "top5"

-- Credits go to Jor. This comes from SimpleMeter's BuildCopyText method for encounters.
-- Friendly and hostile checks are performed internally. 
-- Provide 'mode' and 'expand'. 
-- 'mode' is the report requested. DPS, Damage done, healing done, damage taken, heal taken, and dps otherwise. 
-- And 'expand' is the list. 
-- 'self' points to the current encounter.
-- 'top5' looks at the top 5 units.
-- 'all' will look at everything.
-- mode: dps, dmg, hps, heal, dtk, htk
-- expand: all, self, top5
local mode, expand = "dps", "top5"

local function SimpleMeter(unit, mode, expand)

    local SimpleMeter = _G.SimpleMeter
    if SimpleMeter then
        local encounterIndex = SimpleMeter.state.encounterIndex
        local encounter = SimpleMeter.state.encounters[encounterIndex]
        local unitid = Inspect.Unit.Lookup(unit)
        local total, count = 0, 0
        local timeText,totalText, unitText, dpsText = "", "", "", ""
    
        local function grab(side, mode, expand)
            local list, copyFrom = {}, {}
            if side == "ally" then
                copyFrom = encounter.allies
            elseif side == "enemy" then
                copyFrom = encounter.enemies
            end
    
            if #copyFrom == 0 then return NAME end
    
            for _, v in pairs(copyFrom) do
                table.insert(list, v)
            end
    
            encounter:Sort(list, mode)
    
      	    local time = encounter:GetCombatTime()
            timeText = "Time: " .. SimpleMeter.Util.FormatTime(time)
    
            if side == "ally" then
                totalText = totalText .. " Ally"
            elseif side == "enemy" then
                totalText = totalText .. " Enemy"
            else
                return NAME
            end
    
            for _, id in pairs(list) do
                    if id == unitid then
			local unitspec = unit
			local unit = encounter.units[id]
                        local v = 0
            	        if mode == "dps" then
                            v = unit.damage / time
                        elseif mode == "dmg" then
                            v = unit.damage
                        elseif v == "heal" then
                            v =  unit.heal / time
    		        elseif v == "gtk" then
                            v = unit.damageTaken
                        elseif v == "htk" then
                            v = unit.healTaken
                        else
                            v = unit.damage / time
                        end
                        if (expand == "all" and v > 0)
                           or (expand == "top5" and count < 5)
                           or (expand == "self" and unitid == SimpleMeter.state.playerId) then
				dpsText =  "SM " .. SimpleMeter.Util.FormatNumber(v) .. " (Mouse " .. (UnitDPS(unitspec) or "0") .. ")"
                                count = count + 1
                        end
                        total = v
			break
                    end
            end
            if dpsText ~= "" then
                totalText = totalText .. " " .. SimpleMeter.Modes[mode].desc .. " " .. dpsText
            else 
                totalText = totalText .. " " .. SimpleMeter.Modes[mode].desc .. " 0 "
            end
    	end
    
        if encounter then
            local details = Inspect.Unit.Detail(unitid)
            if details and details.relation == "friendly" then
                grab("ally", mode, expand)
            elseif details and details.relation == "hostile" then
                grab("enemy", mode, expand)
            else
                grab("ally", mode, expand)
            end
    
            local text = timeText .. totalText
            if text ~= "0" then 
    	        return text
    	    end
            return NAME
        end
        return NAME
    end
end
ScriptEnv.SimpleMeter = SimpleMeter
