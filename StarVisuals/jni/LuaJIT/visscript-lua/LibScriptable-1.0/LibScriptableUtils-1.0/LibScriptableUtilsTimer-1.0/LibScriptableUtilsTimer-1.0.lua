-- This file is Copyright (c) 2007-2010, Ace3 Development Team
-- All rights reserved.
local name, addon = ...
local MAJOR = "LibScriptableUtilsTimer-1.0" 
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub") 
local LibTimer = LibStub:NewLibrary(MAJOR, MINOR)
if not LibTimer then return end

local LibError = LibStub("LibScriptableUtilsError-1.0")
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local GetTime = Inspect.Time.Real
local pool = setmetatable({}, {__mode = "k"})

-- Ace timers stub.
local AceTimer = {}

local objects = {}
local cache = {}
local storage = {}
local update
local OnFinish
local tconcat = table.concat
local tinsert = table.insert

if not LibTimer.__index then
	LibTimer.__index = LibTimer
end

--[[
   xpcall safecall implementation from AceTimer-3.0
]]
local xpcall = xpcall

local function errorhandler(err)
	return geterrorhandler()(err)
end

local function CreateDispatcher(argCount)
	local code = [[
	local xpcall, eh = ...  -- our arguments are received as unnamed values in "..." since we don't have a proper function declaration
	local method, ARGS
	local function call() return method(ARGS) end

	local function dispatch(func, ...)
		method = func
		if not method then return end
		ARGS = ...
		return xpcall(call, eh)
	end

	return dispatch
	]]

	local ARGS = {}
	for i = 1, argCount do ARGS[i] = "arg"..i end
	code = code:gsub("ARGS", tconcat(ARGS, ", "))
	return assert(loadstring(code, "safecall Dispatcher["..argCount.."]"))(xpcall, errorhandler)
end

local Dispatchers = setmetatable({}, {
	__index=function(self, argCount)
		local dispatcher = CreateDispatcher(argCount)
		rawset(self, argCount, dispatcher)
		return dispatcher
	end
})
Dispatchers[0] = function(func)
	return xpcall(func, errorhandler)
end

local function safecall(func, ...)
	return Dispatchers[select('#', ...)](func, ...)
end


--- Create a new LibTimer object
-- @usage LibScriptableTimer:New(name, duration, repeating, callback, data, errorLevel, durationLimit)
-- @param name A name for this timer object
-- @param duration The timer's duration in milliseconds.
-- @param repeating Whether to repeat the timer or not
-- @param callback Function to call when the timer has expired
-- @param data Data to pass to the callback
-- @param errorLevel Error verbosity level
-- @return A new LibScriptableTimer object
function LibTimer:New(name, duration, repeating, callback, data, errorLevel)
	assert(type(name) == "string", "Timer objects require a name.")

	duration = type(duration) == "number" and duration or 0
	
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
	end

	setmetatable(obj, self)
	
	obj.error = LibError:New(MAJOR .. ": " .. name, errorLevel)
	
	obj.name = name
	obj.duration = duration / 1000
	
	obj.repeating = repeating
	obj.callback = callback or function() obj.error:Print("No callback", 2) end
	obj.data = data
	obj.errorLevel = errorLevel
	obj.startTime = 0
	
	tinsert(objects, obj)
	
	return obj	
	
end


--- Delete a LibTimer object
-- @usage object:Del()
-- @return Nothing
function LibTimer:Del()
	local timer = self
	pool[timer] = true
	timer:Stop()
	timer.error:Del()
end

--- Start a timer.
-- @usage object:Start([duration], [data])
-- @param duration The duration in milliseconds. This is optional. The timer's initialized duration will be replaced by this value.
-- @param data Replace the timer's data that will be sent through the callback with this value.
-- @param func Replace the timer's callback function
function LibTimer:Start(duration, data, func)
	if type(duration) == "number" then
		self.duration = duration / 1000
	end
		
	if self.duration == 0 then return end
	
	self.startTime = GetTime()
	self.active = true
		
	self.data = data or self.data
	if type(func) == "function" then self.callback = func end

	if self.timerID then
		AceTimer:CancelTimer(self.timerID, true)
	end
	
	if self.repeating then
		self.timerID = AceTimer:ScheduleRepeatingTimer(self.callback, self.duration, self.data)
	else
		self.timerID = AceTimer:ScheduleTimer(self.callback, self.duration, self.data)
	end
end

-- Set the timer's refresh rate. This also stops the timer.
-- @usage :Set(100)
-- @param duration The duration in milliseconds. If no duration is passed, then refresh rate is set to zero.
-- @return NOthing
function LibTimer:Set(duration)
	self.duration = (duration or 0) / 1000
	self:Stop()
end

--- Stop a timer
-- @usage object:Stop()
-- @return True if the timer was stopped
function LibTimer:Stop()
	self.active = false
	if self.timerID then
		AceTimer:CancelTimer(self.timerID, true)
		self.timerID = false
	end
end

--- Return how old the timer is since it was started.
-- @usage object:TimeElapsed()
-- @return The remaining duration
function LibTimer:TimeElapsed()
	if type(self.startTime) ~= "number" then return 0 end
	return GetTime() - self.startTime
end

local function update()
	local time = GetTime()
	for k, self in pairs(objects) do
		if time - self.startTime > self.duration and self.active then
			if self.safecall and false then 
				safecall(self.callback, type(self.data) == "table" and unpack(self.data) or self.data)
			else
				self.callback(self.data)
			end
			self.error:Print(("Elapsed - %f"):format(self:TimeElapsed()), 3)
			if not self.repeating then self:Stop() else self:Start() end
		end
	end
end
--tinsert(Event.System.Update.Begin, {update, "LibScriptableUtilsTimer_1_0", "refresh"})

-- For some reason AceTimer implementation is slow in Rift? o.O

--- **AceTimer-3.0** provides a central facility for registering timers.
-- AceTimer supports one-shot timers and repeating timers. All timers are stored in an efficient
-- data structure that allows easy dispatching and fast rescheduling. Timers can be registered, rescheduled
-- or canceled at any time, even from within a running timer, without conflict or large overhead.\\
-- AceTimer is currently limited to firing timers at a frequency of 0.1s. This constant may change
-- in the future, but for now it seemed like a good compromise in efficiency and accuracy.
--
-- All `:Schedule` functions will return a handle to the current timer, which you will need to store if you
-- need to cancel or reschedule the timer you just registered.
--
-- **AceTimer-3.0** can be embeded into your addon, either explicitly by calling AceTimer:Embed(MyAddon) or by 
-- specifying it as an embeded library in your AceAddon. All functions will be available on your addon object
-- and can be accessed directly, without having to explicitly call AceTimer itself.\\
-- It is recommended to embed AceTimer, otherwise you'll have to specify a custom `self` on all calls you
-- make into AceTimer.
-- @class file
-- @name AceTimer-3.0
-- @release $Id: AceTimer-3.0.lua 895 2009-12-06 16:28:55Z nevcairiel $

--[[
	Basic assumptions:
	* In a typical system, we do more re-scheduling per second than there are timer pulses per second
	* Regardless of timer implementation, we cannot guarantee timely delivery due to FPS restriction (may be as low as 10)

	This implementation:
		CON: The smallest timer interval is constrained by HZ (currently 1/10s).
		PRO: It will still correctly fire any timer slower than HZ over a length of time, e.g. 0.11s interval -> 90 times over 10 seconds
		PRO: In lag bursts, the system simly skips missed timer intervals to decrease load
		CON: Algorithms depending on a timer firing "N times per minute" will fail
		PRO: (Re-)scheduling is O(1) with a VERY small constant. It's a simple linked list insertion in a hash bucket.
		CAUTION: The BUCKETS constant constrains how many timers can be efficiently handled. With too many hash collisions, performance will decrease.
		
	Major assumptions upheld:
	- ALLOWS scheduling multiple timers with the same funcref/method
	- ALLOWS scheduling more timers during OnUpdate processing
	- ALLOWS unscheduling ANY timer (including the current running one) at any time, including during OnUpdate processing
]]


AceTimer.hash = AceTimer.hash or {}         -- Array of [0..BUCKET-1] = linked list of timers (using .next member)
                                            -- Linked list gets around ACE-88 and ACE-90.
AceTimer.selfs = AceTimer.selfs or {}       -- Array of [self]={[handle]=timerobj, [handle2]=timerobj2, ...}
--AceTimer.frame = AceTimer.frame or CreateFrame("Frame", "AceTimer30Frame")

-- Lua APIs
local assert, error, loadstring = assert, error, loadstring
local setmetatable, rawset, rawget = setmetatable, rawset, rawget
local select, pairs, type, next, tostring = select, pairs, type, next, tostring
local floor, max, min = math.floor, math.max, math.min
local tconcat = table.concat

-- Global vars/functions that we don't upvalue since they might get hooked, or upgraded
-- List them here for Mikk's FindGlobals script
-- GLOBALS: DEFAULT_CHAT_FRAME, geterrorhandler

-- Simple ONE-SHOT timer cache. Much more efficient than a full compost for our purposes.
local timerCache = nil

--[[
	Timers will not be fired more often than HZ-1 times per second. 
	Keep at intended speed PLUS ONE or we get bitten by floating point rounding errors (n.5 + 0.1 can be n.599999)
	If this is ever LOWERED, all existing timers need to be enforced to have a delay >= 1/HZ on lib upgrade.
	If this number is ever changed, all entries need to be rehashed on lib upgrade.
	]]
local HZ = 11

--[[
	Prime for good distribution
	If this number is ever changed, all entries need to be rehashed on lib upgrade.
]]
local BUCKETS = 131

local hash = AceTimer.hash
for i=1,BUCKETS do
	hash[i] = hash[i] or false	-- make it an integer-indexed array; it's faster than hashes
end


local lastint = floor(GetTime() * HZ)

-- --------------------------------------------------------------------
-- OnUpdate handler
--
-- traverse buckets, always chasing "now", and fire timers that have expired

local function OnUpdate()
	local now = GetTime()
	local nowint = floor(now * HZ)
	
	-- Have we passed into a new hash bucket?
	if nowint == lastint then return end
	
	local soon = now + 1 -- +1 is safe as long as 1 < HZ < BUCKETS/2
	
	-- Pass through each bucket at most once
	-- Happens on e.g. instance loads, but COULD happen on high local load situations also
	for curint = (max(lastint, nowint - BUCKETS) + 1), nowint do -- loop until we catch up with "now", usually only 1 iteration
		local curbucket = (curint % BUCKETS)+1
		-- Yank the list of timers out of the bucket and empty it. This allows reinsertion in the currently-processed bucket from callbacks.
		local nexttimer = hash[curbucket]
		hash[curbucket] = false -- false rather than nil to prevent the array from becoming a hash

		while nexttimer do
			local timer = nexttimer
			nexttimer = timer.next
			local when = timer.when
			
			if when < soon then
				-- Call the timer func, either as a method on given object, or a straight function ref
				local callback = timer.callback
				if type(callback) == "string" then
					safecall(timer.object[callback], timer.object, timer.arg)
				elseif callback then
					safecall(callback, timer.arg)
				else
					-- probably nilled out by CancelTimer
					timer.delay = nil -- don't reschedule it
				end

				local delay = timer.delay	-- NOW make a local copy, can't do it earlier in case the timer cancelled itself in the callback
				
				if not delay then
					-- single-shot timer (or cancelled)
					AceTimer.selfs[timer.object][tostring(timer)] = nil
					timerCache = timer
				else
					-- repeating timer
					local newtime = when + delay
					if newtime < now then -- Keep lag from making us firing a timer unnecessarily. (Note that this still won't catch too-short-delay timers though.)
						newtime = now + delay
					end
					timer.when = newtime
					
					-- add next timer execution to the correct bucket
					local bucket = (floor(newtime * HZ) % BUCKETS) + 1
					timer.next = hash[bucket]
					hash[bucket] = timer
				end
			else -- if when>=soon 
				-- reinsert (yeah, somewhat expensive, but shouldn't be happening too often either due to hash distribution)
				timer.next = hash[curbucket]
				hash[curbucket] = timer
			end -- if when<soon ... else
		end -- while nexttimer do
	end -- for curint=lastint,nowint
	
	lastint = nowint
end

-- Our update event. This triggers every single frame.
tinsert(Event.System.Update.Begin, {OnUpdate, "LibScriptableUtilsTimer_1_0", "refresh"})


-- ---------------------------------------------------------------------
-- Reg( callback, delay, arg, repeating )
--
-- callback( function or string ) - direct function ref or method name in our object for the callback
-- delay(int) - delay for the timer
-- arg(variant) - any argument to be passed to the callback function
-- repeating(boolean) - repeating timer, or oneshot
--
-- returns the handle of the timer for later processing (canceling etc)
local function Reg(self, callback, delay, arg, repeating)
	if type(callback) ~= "string" and type(callback) ~= "function" then 
		local error_origin = repeating and "ScheduleRepeatingTimer" or "ScheduleTimer"
		error(MAJOR..": " .. error_origin .. "(callback, delay, arg): 'callback' - function or method name expected.", 3)
	end
	if type(callback) == "string" then
		if type(self)~="table" then
			local error_origin = repeating and "ScheduleRepeatingTimer" or "ScheduleTimer"
			error(MAJOR..": " .. error_origin .. "(\"methodName\", delay, arg): 'self' - must be a table.", 3)
		end
		if type(self[callback]) ~= "function" then 
			local error_origin = repeating and "ScheduleRepeatingTimer" or "ScheduleTimer"
			error(MAJOR..": " .. error_origin .. "(\"methodName\", delay, arg): 'methodName' - method not found on target object.", 3)
		end
	end
	
	if delay < (1 / (HZ - 1)) then
		delay = 1 / (HZ - 1)
	end
	
	-- Create and stuff timer in the correct hash bucket
	local now = GetTime()
	
	local timer = timerCache or {}	-- Get new timer object (from cache if available)
	timerCache = nil
	
	timer.object = self
	timer.callback = callback
	timer.delay = (repeating and delay)
	timer.arg = arg
	timer.when = now + delay

	local bucket = (floor((now+delay)*HZ) % BUCKETS) + 1
	timer.next = hash[bucket]
	hash[bucket] = timer
	
	-- Insert timer in our self->handle->timer registry
	local handle = tostring(timer)
	
	local selftimers = AceTimer.selfs[self]
	if not selftimers then
		selftimers = {}
		AceTimer.selfs[self] = selftimers
	end
	selftimers[handle] = timer
	selftimers.__ops = (selftimers.__ops or 0) + 1
	
	return handle
end

--- Schedule a new one-shot timer.
-- The timer will fire once in `delay` seconds, unless canceled before.
-- @param callback Callback function for the timer pulse (funcref or method name).
-- @param delay Delay for the timer, in seconds.
-- @param arg An optional argument to be passed to the callback function.
-- @usage
-- MyAddon = LibStub("AceAddon-3.0"):NewAddon("TimerTest", "AceTimer-3.0")
-- 
-- function MyAddon:OnEnable()
--   self:ScheduleTimer("TimerFeedback", 5)
-- end
--
-- function MyAddon:TimerFeedback()
--   print("5 seconds passed")
-- end
function AceTimer:ScheduleTimer(callback, delay, arg)
	return Reg(self, callback, delay, arg)
end

--- Schedule a repeating timer.
-- The timer will fire every `delay` seconds, until canceled.
-- @param callback Callback function for the timer pulse (funcref or method name).
-- @param delay Delay for the timer, in seconds.
-- @param arg An optional argument to be passed to the callback function.
-- @usage
-- MyAddon = LibStub("AceAddon-3.0"):NewAddon("TimerTest", "AceTimer-3.0")
-- 
-- function MyAddon:OnEnable()
--   self.timerCount = 0
--   self.testTimer = self:ScheduleRepeatingTimer("TimerFeedback", 5)
-- end
--
-- function MyAddon:TimerFeedback()
--   self.timerCount = self.timerCount + 1
--   print(("%d seconds passed"):format(5 * self.timerCount))
--   -- run 30 seconds in total
--   if self.timerCount == 6 then
--     self:CancelTimer(self.testTimer)
--   end
-- end
function AceTimer:ScheduleRepeatingTimer(callback, delay, arg)
	return Reg(self, callback, delay, arg, true)
end

--- Cancels a timer with the given handle, registered by the same addon object as used for `:ScheduleTimer`
-- Both one-shot and repeating timers can be canceled with this function, as long as the `handle` is valid
-- and the timer has not fired yet or was canceled before.
-- @param handle The handle of the timer, as returned by `:ScheduleTimer` or `:ScheduleRepeatingTimer`
-- @param silent If true, no error is raised if the timer handle is invalid (expired or already canceled)
-- @return True if the timer was successfully cancelled.
function AceTimer:CancelTimer(handle, silent)
	if not handle then return end -- nil handle -> bail out without erroring
	if type(handle) ~= "string" then
		error(MAJOR..": CancelTimer(handle): 'handle' - expected a string", 2)	-- for now, anyway
	end
	local selftimers = AceTimer.selfs[self]
	local timer = selftimers and selftimers[handle]
	if silent then
		if timer then
			timer.callback = nil	-- don't run it again
			timer.delay = nil		-- if this is the currently-executing one: don't even reschedule 
			-- The timer object is removed in the OnUpdate loop
		end
		return not not timer	-- might return "true" even if we double-cancel. we'll live.
	else
		if not timer then
			geterrorhandler()(MAJOR..": CancelTimer(handle[, silent]): '"..tostring(handle).."' - no such timer registered")
			return false
		end
		if not timer.callback then 
			geterrorhandler()(MAJOR..": CancelTimer(handle[, silent]): '"..tostring(handle).."' - timer already cancelled or expired")
			return false
		end
		timer.callback = nil	-- don't run it again
		timer.delay = nil		-- if this is the currently-executing one: don't even reschedule 
		return true
	end
end

--- Cancels all timers registered to the current addon object ('self')
function AceTimer:CancelAllTimers()
	if not(type(self) == "string" or type(self) == "table") then
		error(MAJOR..": CancelAllTimers(): 'self' - must be a string or a table",2)
	end
	if self == AceTimer then
		error(MAJOR..": CancelAllTimers(): supply a meaningful 'self'", 2)
	end
	
	local selftimers = AceTimer.selfs[self]
	if selftimers then
		for handle,v in pairs(selftimers) do
			if type(v) == "table" then  -- avoid __ops, etc
				AceTimer.CancelTimer(self, handle, true)
			end
		end
	end
end

--- Returns the time left for a timer with the given handle, registered by the current addon object ('self').
-- This function will raise a warning when the handle is invalid, but not stop execution.
-- @param handle The handle of the timer, as returned by `:ScheduleTimer` or `:ScheduleRepeatingTimer`
-- @return The time left on the timer, or false if the handle is invalid.
function AceTimer:TimeLeft(handle)
	if not handle then return end
	if type(handle) ~= "string" then
		error(MAJOR..": TimeLeft(handle): 'handle' - expected a string", 2)    -- for now, anyway
	end
	local selftimers = AceTimer.selfs[self]
	local timer = selftimers and selftimers[handle]
	if not timer then
		geterrorhandler()(MAJOR..": TimeLeft(handle): '"..tostring(handle).."' - no such timer registered")
		return false
	end
	return timer.when - GetTime()
end


-- ---------------------------------------------------------------------
-- PLAYER_REGEN_ENABLED: Run through our .selfs[] array step by step
-- and clean it out - otherwise the table indices can grow indefinitely
-- if an addon starts and stops a lot of timers. AceBucket does this!
--
-- See ACE-94 and tests/AceTimer-3.0-ACE-94.lua

--[[
local lastCleaned = nil

local function OnEvent(this, event)
	if event~="PLAYER_REGEN_ENABLED" then
		return
	end
	
	-- Get the next 'self' to process
	local selfs = AceTimer.selfs
	local self = next(selfs, lastCleaned)
	if not self then
		self = next(selfs)
	end
	lastCleaned = self
	if not self then	-- should only happen if .selfs[] is empty
		return
	end
	
	-- Time to clean it out?
	local list = selfs[self]
	if (list.__ops or 0) < 250 then	-- 250 slosh indices = ~10KB wasted (max!). For one 'self'.
		return
	end
	
	-- Create a new table and copy all members over
	local newlist = {}
	local n=0
	for k,v in pairs(list) do
		newlist[k] = v
		n=n+1
	end
	newlist.__ops = 0	-- Reset operation count
	
	-- And since we now have a count of the number of live timers, check that it's reasonable. Emit a warning if not.
	if n>BUCKETS then
		print_raw(MAJOR..": Warning: The addon/module '"..tostring(self).."' has "..n.." live timers. Surely that's not intended?")
	end
	
	selfs[self] = newlist
end
]]

-- ---------------------------------------------------------------------
-- Embed handling

AceTimer.embeds = AceTimer.embeds or {}

local mixins = {
	"ScheduleTimer", "ScheduleRepeatingTimer", 
	"CancelTimer", "CancelAllTimers",
	"TimeLeft"
}

function AceTimer:Embed(target)
	AceTimer.embeds[target] = true
	for _,v in pairs(mixins) do
		target[v] = AceTimer[v]
	end
	return target
end

-- AceTimer:OnEmbedDisable( target )
-- target (object) - target object that AceTimer is embedded in.
--
-- cancel all timers registered for the object
function AceTimer:OnEmbedDisable( target )
	target:CancelAllTimers()
end


for addon in pairs(AceTimer.embeds) do
	AceTimer:Embed(addon)
end

-- ---------------------------------------------------------------------
-- Debug tools (expose copies of internals to test suites)
AceTimer.debug = AceTimer.debug or {}
AceTimer.debug.HZ = HZ
AceTimer.debug.BUCKETS = BUCKETS

-- ---------------------------------------------------------------------
-- Finishing touchups

--[[
AceTimer.frame:SetScript("OnUpdate", OnUpdate)
AceTimer.frame:SetScript("OnEvent", OnEvent)
AceTimer.frame:RegisterEvent("PLAYER_REGEN_ENABLED")
]]

-- In theory, we should hide&show the frame based on there being timers or not.
-- However, this job is fairly expensive, and the chance that there will 
-- actually be zero timers running is diminuitive to say the lest.
