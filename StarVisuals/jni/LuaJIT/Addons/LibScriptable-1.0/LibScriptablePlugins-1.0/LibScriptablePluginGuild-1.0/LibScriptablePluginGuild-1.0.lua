local MAJOR = "LibScriptablePluginGuild-1.0" 
local MINOR = 24

local PluginGuild = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginGuild then return end
local LibAlts = LibStub("LibAlts-1.0", true)
assert(LibAlts, MAJOR .. " requires LibAlts-1.0")


-- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginGuild:New(environment)

	environment.LibAlts = LibAlts
	environment.AcceptGuild = _G.AcceptGuild -- () - The player accepts the invitation to join a guild. 
	environment.BuyGuildCharter = _G.BuyGuildCharter -- ("guildName") - Purchases a guild charter for guildName. 
	environment.CanEditGuildEvent = _G.CanEditGuildEvent -- () - Returns true if you are allowed to edit guild events  -- (in the calendar), 
	environment.CanEditGuildInfo = _G.CanEditGuildInfo -- () - Returns true if you are allowed to edit the guild info 
	environment.CanEditMOTD = _G.CanEditMOTD -- () - Returns true if you are allowed to edit the guild motd. 
	environment.CanEditOfficerNote = _G.CanEditOfficerNote -- () - Returns true if you are allowed to edit a guild member's officer note. 
	environment.CanEditPublicNote = _G.CanEditPublicNote -- () - Returns true if you are allowed to edit a guild member's public note. 
	environment.CanGuildDemote = _G.CanGuildDemote -- () - Returns true if you are allowed to demote a guild member. 
	environment.CanGuildInvite = _G.CanGuildInvite -- () - Returns true if you are allowed to invite a new member to the guild. 
	environment.CanGuildPromote = _G.CanGuildPromote -- () - Returns true if you are allowed to demote a guild member. 
	environment.CanGuildRemove = _G.CanGuildRemove -- () - Returns true if you are allowed to remove a guild member. 
	environment.CanViewOfficerNote = _G.CanViewOfficerNote -- () - Returns true if you are allowed to view a Officer Note. 
	environment.CloseGuildRegistrar = _G.CloseGuildRegistrar -- () - ?. 
	environment.CloseGuildRoster = _G.CloseGuildRoster -- () - ?. 
	environment.CloseTabardCreation = _G.CloseTabardCreation -- () - ?. 
	environment.DeclineGuild = _G.DeclineGuild -- () - The player declines the invitation to join a guild. 
	environment.GetGuildCharterCost = _G.GetGuildCharterCost -- () - Returns the cost of purchasing a guild charter. 
	environment.GetGuildEventInfo = _G.GetGuildEventInfo -- (index) - Returns the event information.  -- (Added in 2.3) 
	environment.GetGuildInfo = _G.GetGuildInfo -- ("unit") - This function returns the name of the guild unit belongs to. 
	environment.GetGuildInfoText = _G.GetGuildInfoText -- () - Returns the persistant Guild Information data.  -- (new in 1.9) 
	environment.GetGuildRosterInfo = _G.GetGuildRosterInfo -- (index) - This function is used to get info on members in the guild. 
	environment.GetGuildRosterLastOnline = _G.GetGuildRosterLastOnline -- (index) - Returns time since last online for indexth member in current sort order. 
	environment.GetGuildRosterMOTD = _G.GetGuildRosterMOTD -- () - Returns guild's MOTD. 
	environment.GetGuildRosterSelection = _G.GetGuildRosterSelection -- () - Returns the index of the current selected guild member. 
	environment.GetGuildRosterShowOffline = _G.GetGuildRosterShowOffline -- () - Returns true if showing offline members of the guild. 
	environment.GetNumGuildEvents = _G.GetNumGuildEvents -- () - Returns the number of guild events.  -- (Added in 2.3) 
	environment.GetNumGuildMembers = _G.GetNumGuildMembers -- (offline) - Returns the number of guild members total. 
	environment.GetTabardCreationCost = _G.GetTabardCreationCost -- () - Returns cost in coppers. 
	environment.GetTabardInfo = _G.GetTabardInfo -- () -?. 
	environment.GuildControlAddRank = _G.GuildControlAddRank -- ("name") - Add another rank called "name". Only Guildmaster. 
	environment.GuildControlDelRank = _G.GuildControlDelRank -- ("name") - Delete rank "name". Only Guildmaster. 
	environment.GuildControlGetNumRanks = _G.GuildControlGetNumRanks -- () - Returns number of ranks after guild frame open. Any guild member can use this. 
	environment.GuildControlGetRankFlags = _G.GuildControlGetRankFlags -- () - Returns list of values for each permission for a select rank  -- (default rank 1). 
	environment.GuildControlGetRankName = _G.GuildControlGetRankName -- (index) - Returns name of the rank at index. Any guild member can use this. 
	environment.GuildControlSaveRank = _G.GuildControlSaveRank -- ("name") - Saves the permissions for rank "name". Only Guildmaster. 
	environment.GuildControlSetRank = _G.GuildControlSetRank -- (rank) - Sets the currently selected rank to view. 
	environment.GuildControlSetRankFlag = _G.GuildControlSetRankFlag -- (index, enabled) - Enable/disable permission for an action at index. Only Guildmaster. 
	environment.GuildDemote = _G.GuildDemote -- ("name") - Demotes a player "name". 
	environment.GuildDisband = _G.GuildDisband -- () - Disbands at once your guild. You must be the guild's leader to do so. Be careful, no warning is given prior disbanding. 
	environment.GuildInfo = _G.GuildInfo -- () - Displays information about the guild you are a member of. 
	environment.GuildInvite = _G.GuildInvite -- ("name") - Invites a player to your guild. 
	environment.GuildLeave = _G.GuildLeave -- () - Removes you from your current guild. 
	environment.GuildPromote = _G.GuildPromote -- ("name") - Promotes a player "name". 
	environment.GuildRoster = _G.GuildRoster -- () - Fetches the guild list and fires a GUILD_ROSTER_UPDATE event. 
	environment.GuildRosterSetOfficerNote = _G.GuildRosterSetOfficerNote -- (index, "note") - Sets the officer note at index to "note". 
	environment.GuildRosterSetPublicNote = _G.GuildRosterSetPublicNote -- (index, "note") - Sets the public note at index to "note". 
	environment.GuildSetMOTD = _G.GuildSetMOTD -- ("note") - Set Guild Message of the Day to "note". 
	environment.GuildSetLeader = _G.GuildSetLeader -- ("name") - Transfers guild leadership to another character. 
	environment.GuildUninvite = _G.GuildUninvite -- ("name") - Removes the member "name". 
	environment.IsGuildLeader = _G.IsGuildLeader -- ("name") - Determine if player "name" is a guild master. 
	environment.IsInGuild = _G.IsInGuild -- () - Lets you know whether you are in a guild. 
	environment.QueryGuildEventLog = _G.QueryGuildEventLog -- () - Fetches the guild event list and fires a GUILD_EVENT_LOG_UPDATE event.  -- (Added in 2.3) 
	environment.SetGuildInfoText = _G.SetGuildInfoText -- () - Sets the persistant Guild Information data. Limit is 500 letters (GuildInfoEditBox is limited to this number). Longer texts are possible, but will be reseted during the day.  -- (new in 1.9) 
	environment.SetGuildRosterSelection = _G.SetGuildRosterSelection -- (index) - Selects/deselects a guild member according current sorting order. 
	environment.SetGuildRosterShowOffline = _G.SetGuildRosterShowOffline -- (enabled) - Sets/Resets the show offline members flag. 
	environment.SortGuildRoster = _G.SortGuildRoster -- ("sort") - Sorts guildroster according "sort". Any unknown values sort on "name". 
	environment.UnitGuildInfo = self.UnitGuildInfo
	
	return environment
end

function PluginGuild.UnitGuildInfo(unit)
	if select(2, UnitName(unit)) ~= select(2, UnitName("player")) then return end
	local name1 = UnitName(unit)
	for i = 1, GetNumGuildMembers() do
		--local skillID, isCollapsed, iconTexture, headerName, numOnline, numPlayers, playerName, class, online, zone, skill, classFileName = GetGuildTradeSkillInfo(i)
		local name2, rank, rankIndex, level, class, zone, note, officernote, online, status, classFileName, achievementPoints, achievementRank, isMobile = GetGuildRosterInfo(i);
		if name1 == name2 then
			return name2, rank, rankIndex, level, class, zone, note, officernote, online, status, classFileName, achievementPoints, achievementRank, isMobile
		end
	end
end

