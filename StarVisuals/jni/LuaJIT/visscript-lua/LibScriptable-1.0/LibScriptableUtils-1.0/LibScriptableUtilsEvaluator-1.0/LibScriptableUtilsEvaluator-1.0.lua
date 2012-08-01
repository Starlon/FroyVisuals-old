
local MAJOR = "LibScriptableUtilsEvaluator-1.0" 
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub") 
local LibEvaluator = LibStub:NewLibrary(MAJOR, MINOR)
if not LibEvaluator then return end
local LibError = LibStub("LibScriptableUtilsError-1.0")
assert(LibError, "LibEvaluator requires LibScriptableUtilsError-1.0")
local error = LibError:New("Evaluator")

local pool = setmetatable({}, {__mode = "k"})
	
if not LibEvaluator.__index then
	LibEvaluator.__index = LibEvaluator
end

do 
	local function errorhandler(str)
		print("ouch")
		error:Print(str)
	end
	
	local cache = {} --setmetatable({},{__mode='v'})	
	
	LibEvaluator.Evaluate = function(self, tag, code, unit)
		self.unit = unit
		return LibEvaluator.ExecuteCode(self, tag, code)
	end
	
	--- Execute some code
	-- @usage LibEvaluator.ExecuteCode(self, tag, code, dontSandbox, defval, forRunnable, test)
	-- @param self The script environment.
	-- @param tag A name for your runnable
	-- @param dontSandbox Whether to sandbox the execution or not
	-- @param defval The default value if any
	-- @param forRunnable Boolean indicating whether to return the actual function object or not
	-- @param test Whether to test the runnable before returning it. This is useful if you don't want your script to execute twice.
	-- @return ret1, ret2, ret3, ret4 -- return values from your code. You can pass 4 return values back from your script.
	LibEvaluator.ExecuteCode = function(self, tag, code, dontSandbox, defval, forRunnable, test)
		
		assert(self, "LibEvaluator.ExecuteCode requires an environment")
		assert(tag, "LibEvaluator.ExecuteCode requires a tag")
				
		code = code or "return nil"
		
		code = "return function() " .. code .. "\n end"
				
		local runnable = cache[code]
		local err
		
		local first
				
		if not runnable then
			runnable, err = loadstring(code, tag)
	
			if type(runnable) ~= "function" then 
				error:Print("Error in runnable: " .. err)
				return nil
			end
									
			if not dontSandbox then
				setfenv(runnable, self)
			end
			
			cache[code] = runnable(xpcall, errorhandler)
			
			runnable = cache[code]	
			
			first = true
		end

		if type(self.unit) ~= "string" then 
			self.unit = "mouseover" 
		end
		if forRunnable and test then
			return runnable() and runnable
		elseif forRunnable then
			return runnable
		end
		
		local ret1, ret2, ret3, ret4, ret5 = runnable()
								
		if not ret1 then ret1 = defval end

		return ret1, ret2, ret3, ret4, ret5
	end
	
	--- Retrieve a piece of code's runnable object
	--
	LibEvaluator.GetRunnable = function(self, tag, code, dontSandbox, defval)
		return LibEvaluator.ExecuteCode(self, tag, code, dontSandbox, defval, true, false)
	end
end
