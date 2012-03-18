local MAJOR = "LibScriptablePluginResourceTools-1.0" 
local MINOR = 24

local PluginResourceTools = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginResourceTools then return end
local LibError = LibStub("LibScriptableUtilsError-1.0", true)
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")

if not PluginResourceTools.__index then
	PluginResourceTools.__index = PluginResourceTools
end

local scriptProfile = GetCVar("scriptProfile") == "1"
local oldtotalcpu, oldtotalmem
local lastcputime, lastmemtime = GetTime(), GetTime()

local oldmem = {}
local oldcpu = {}
local meminit
local cpuinit

local new, del
do
	local pool = setmetatable({}, {__mode = "k"})
	function new(...)
		local obj = next(pool)
		if obj then
			pool[obj] = nil
		else
			obj = {}
		end
		for i = 1, select("#", ...) do
			obj[i] = select(i, ...)
		end
		return obj
	end
	function del(tbl)
		pool[tbl] = true
	end
end

--- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @param environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginResourceTools:New(environment)
		
	environment.GetCPUUsage = self.GetCPUUsage -- returns cpu, perc, cpudiff, totalcpu, totaldiff
	environment.GetMemUsage = self.GetMemUsage -- returns mem, perc, memdiff, totalmem, totaldiff
	environment.GetLatency = self.GetLatency -- returns current latency
	environment.UpdateCPU = self.UpdateCPU -- Update cpu values
	environment.UpdateMem = self.UpdateMem -- Update memory values
	environment.Update = self.Update -- Call UpdateCPU and UpdateMem
	environment.GetFramerate = self.GetFramerate -- return the current frame rate
	environment.scriptProfile = scriptProfile -- Whether CVar("scriptProfile") is true or not
	environment.Debug = self.Debug
	environment.DebugMem = self.DebugMem
	environment.DebugCPU = self.DebugCPU
		
	return environment
end

local debug_mem = {}
--- Profile memory and display results
-- @usage DebugMem(addon)
-- @param addon The addon to profile
function PluginResourceTools.DebugMem(addon)
	debug_mem[addon] = debug_mem[addon] or 0
	local tag = "1)"
	if debug_mem[addon] % 2 == 0 then
		tag = "2)"
	end
	debug_mem[addon] = debug_mem[addon] + 1
	local _ = ResourceServer and ResourceServer.Update() or PluginResourceTools.Update()
	local mem, percent, memdiff, totalMem, totaldiff
	if ResourceServer then
		mem, percent, memdiff, totalMem, totaldiff = ResourceServer.GetMemUsage(addon)
	else
		mem, percent, memdiff, totalMem, totaldiff = PluginResourceTools.GetMemUsage(addon)
	end
	
	if not mem then return end
	LibError:Print(format("%s Mem: %d, Percent: %d, Memdiff: %d, Totalmem: %d, Totaldiff: %d", tag, mem,  percent, memdiff, totalMem, totaldiff))
end

local debug_cpu = {}

--- Profile CPU and display results
-- @usage DebugCPU(addon)
-- @param addon The addon to profile
function PluginResourceTools.DebugCPU(addon)
	debug_cpu[addon] = debug_cpu[addon] or 0
	local tag = "1)"
	if debug_cpu[addon] % 2 == 0 then
		tag = "2)"
	end
	debug_cpu[addon] = debug_cpu[addon] + 1
	local _ = ResourceServer and ResourceServer.Update() or PluginResourceTools.Update()
	local cpu, percent, cpudiff, totalCPU, totaldiff
	if ResourceServer then
		cpu, percent, cpudiff, totalCPU, totaldiff = ResourceServer.GetCPUUsage(addon)
	else
		cpu, percent, cpudiff, totalCPU, totaldiff = PluginResourceTools.GetCPUUsage(addon)
	end
	if not cpu then return end
	LibError:Print(format("%s CPU: %d, Percent: %d, CPUdiff: %d, TotalCPU: %d, Totaldiff: %d", tag, cpu,  percent, cpudiff, totalCPU, totaldiff))
end

--- Profile CPU and Memory and display results
-- @usage Debug(addon)
-- @param addon The addon to profile.
function PluginResourceTools.Debug(addon)
	PluginResourceTools.DebugMem(addon)
	PluginResourceTools.DebugCPU(addon)
end

--- Update profiling data
-- @usage Update()
function PluginResourceTools.Update()
	PluginResourceTools:UpdateMem()
	PluginResourceTools:UpdateCPU()
end

--- Update CPU data
-- @usage UpdateCPU()
function PluginResourceTools.UpdateCPU()
	cpuinit = true
	
	UpdateAddOnCPUUsage()
	
	local totalcpu = 0.001
	
	for i=1, GetNumAddOns() do
		totalcpu = totalcpu + (GetAddOnCPUUsage(i) )
	end	
	
	for i = 1, GetNumAddOns() do
	
		local addon = GetAddOnInfo(i) 
		
		if not oldcpu[addon] then 
			oldcpu[addon] = new(-1, -1, -1, -1, -1)
		end
		
		del(oldcpu[addon])
						
		local cpu = GetAddOnCPUUsage(addon)
		
		local cpuperc = cpu / (totalcpu <= 0 and .00001 or totalcpu) * 100
		local cpudiff = oldcpu[addon] and oldcpu[addon][1] and (cpu - oldcpu[addon][1]) or 0
		local totaldiff = oldtotalcpu and (totalcpu - oldtotalcpu) or 0

		oldcpu[addon] = new()
		oldcpu[addon][1] = cpu
		oldcpu[addon][2] = cpuperc
		oldcpu[addon][3] = cpudiff
		oldcpu[addon][4] = totalcpu
		oldcpu[addon][5] = totaldiff
	end
	oldtotalcpu = totalcpu	
end

local addons = {"LibScriptable-1.0", "StarTip", "LCD4WoW"}

local function getUserCPU(addon, usestarlibs)
	if addon == "StarTip" or addon == "LCD4WoW" or usestarlibs then
		addons[4] = addon
		local high = 0
		for i, v in ipairs(addons) do
			if oldcpu[v] and oldcpu[v][1] > high then
				high = oldcpu[v][1]
				addon = v
			end
		end
	end
	return addon
end

--- Retrieve resource profile for CPU usage
-- @usage GetCPUUsage(addon, update, usestarlibs)
-- @param addon We'll get the cpu usage for this addon.
-- @param update Whether to update data or not
-- @return cpu, percent, cpudiff, totalcpu, totaldiff, cpuperc
function PluginResourceTools.GetCPUUsage(addon, update, usestarlibs)
	if not scriptProfile then return 0, 0, 0, 0, 0, 0 end
	
	assert(type(addon) == "string", "GetCPUUsage: addon name is not a string")
	
	addon = getUserCPU(addon, usestarlibs)

	if update or not cpuinit then
		PluginResourceTools.UpdateCPU()
	end
	
	if not oldcpu[addon] then
		oldcpu[addon] = new(0, 0, 0, 0, 0)
	end
	
	local cpu = oldcpu[addon][1]
	local perc = oldcpu[addon][2]
	local cpudiff = oldcpu[addon][3]
	local totalcpu = oldcpu[addon][4]
	local totaldiff = oldcpu[addon][5]
	
	local cpuperc
	if totaldiff == 0 then
		totaldiff = 100
		cpudiff = 0
	end
	
	if totaldiff < 0 and cpudiff < 0 then
		local total = abs(totaldiff)
		local cpu = abs(cpudiff)
		cpuperc = cpu / total * 100
		cpuperc = cpuperc * -1
	elseif totaldiff >= 0 and cpudiff >= 0 then
		cpuperc = cpudiff / totaldiff * 100
	else
		cpuperc = 0
	end
	
	return cpu, perc, cpudiff, totalcpu, totaldiff, cpuperc
end

--- Update profile data for memory usage
-- @usage Update()
-- @return Nothing
function PluginResourceTools.UpdateMem() 
	UpdateAddOnMemoryUsage()
	
	meminit = true
	
	if not oldmem then oldmem = {} end
	
	local totalmem = 0.0001
	
	for i=1, GetNumAddOns() do
		totalmem = totalmem + (GetAddOnMemoryUsage(i) )
	end	

	for i = 1, GetNumAddOns() do	
		local addon = GetAddOnInfo(i) 
		
		if not oldmem[addon] then 
			oldmem[addon] = new(0, 0, 0, 0, 0)
		end
		
		del(oldmem[addon])

		local mem = GetAddOnMemoryUsage(addon)
		local memperc = mem / totalmem * 100
		local memdiff = oldmem[addon][1] and (mem - oldmem[addon][1]) or 0
		local totaldiff = oldtotalmem and (totalmem - oldtotalmem) or 0
				
		oldmem[addon] = new() 
		oldmem[addon][1] = mem
		oldmem[addon][2] = memperc
		oldmem[addon][3] = memdiff
		oldmem[addon][4] = totalmem
		oldmem[addon][5] = totaldiff
	end
	oldtotalmem = totalmem
end

local function getUserMem(addon, usestarlibs)
	if addon == "StarTip" or addon == "LCD4WoW" or usestarlibs then
		addons[4] = addon
		local high = 0
		for i, v in ipairs(addons) do
			if oldmem[v] and oldmem[v][1] > high then
				high = oldmem[v][1]
				addon = v
			end
		end
	end
	return addon
end

--- Retrieve profiling data for memory usage
-- @usage GetMemUsage(addon)
-- @param addon We'll get the mem usage for this addon
-- @param update Whether to update resource data or not
-- @return mem, percent, memdiff, totalmem, totaldiff, memperc
function PluginResourceTools.GetMemUsage(addon, update, usestarlibs)
	assert(type(addon) == "string", "GetCPUUsage: addon name is not a string")	

	if update or not meminit then 
		PluginResourceTools.UpdateMem() 
	end
	
	addon = getUserMem(addon, usestarlibs)

	if not oldmem[addon] then
		oldmem[addon] = new(0, 0, 0, 0, 0)
	end
	
	local mem = oldmem[addon][1]
	local perc = oldmem[addon][2]
	local memdiff = oldmem[addon][3]
	local totalmem = oldmem[addon][4]
	local totaldiff = oldmem[addon][5]

	local memperc
	if totaldiff == 0 then
		totaldiff = 100
		memdiff = 0
	end
	
	if totaldiff < 0 and memdiff < 0 then
		local total = abs(totaldiff)
		local mem = abs(memdiff)
		memperc = mem / total * 100
		memperc = memperc * -1
	elseif totaldiff >= 0 and memdiff >= 0 then
		memperc = memdiff / totaldiff * 100
	else
		memperc = 0
	end
	
	return mem, perc, memdiff, totalmem, totaldiff, memperc
end


local lastlat = 0
--- Retrieve latency info
-- @usage GetLatency()
-- @return Current latency information
function PluginResourceTools.GetLatency()
	local _, _, latency = GetNetStats()
	if type(latency) == "number" then lastlat = latency else latency = lastlat end
	return latency
end

--- Retrieve framerate info
-- @usage GetFramerate()
-- @return Current framerate
function PluginResourceTools.GetFramerate()
	return GetFramerate() or 0
end
