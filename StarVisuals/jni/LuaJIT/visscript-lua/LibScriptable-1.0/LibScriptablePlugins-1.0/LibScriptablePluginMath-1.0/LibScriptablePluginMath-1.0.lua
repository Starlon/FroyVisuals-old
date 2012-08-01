local MAJOR = "LibScriptablePluginMath-1.0" 
local MINOR = 19

local PluginMath = LibStub:NewLibrary(MAJOR, MINOR)
if not PluginMath then return end

local math = _G.math

if not PluginMath.__index then
	PluginMath.__index = PluginMath
end

local ScriptEnv = {}

-- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function PluginMath:New(environment)
	
	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end
		
	environment.abs = math.abs
	environment.acos = math.acos
	environment.asin = math.asin
	environment.atan = math.atan
	environment.atan2 = math.atan2
	environment.ceil = math.ceil
	environment.cos = math.cos
	environment.cosh = math.cosh
	environment.deg = math.deg
	environment.exp = math.exp
	environment.floor = math.floor
	environment.fmod = math.fmod
	environment.frexp = math.frexp
	environment.huge = math.huge
	environment.ldexp = math.ldexp
	environment.log = math.log
	environment.log10 = math.log10
	environment.max = math.max
	environment.min = math.min
	environment.mod = math.mod
	environment.modf = math.modf
	environment.pi = math.pi
	environment.pow = math.pow
	environment.rad = math.rad
	environment.random = math.random
	environment.randomseed = math.randomseed
	environment.sin = math.sin
	environment.sinh = math.sinh
	environment.sqrt = math.sqrt
	environment.tan = math.tan
	environment.tanh = math.tanh

	environment.PI = 3.14159265358979323846
	environment.E = 2.71828
	environment.PHI = 1.61033

	return environment
end

local function isnonzero(x)
	return (abs(x) > 0.00001) and 1 or 0
end
ScriptEnv.isnonzero = isnonzero

local function sqr(x)
	return x*x
end
ScriptEnv.sqr = sqr

local function sigmoid(a, b)
	local t = 1+exp(-a * b);

	local val
	if isnonzero(t) then
		val = 1.0/t
	else
		val = 0
	end
	
	return val
end
ScriptEnv.sigmoid = sigmoid

local function above(val1, val2)
	return val1 > val2 and 1 or 0
end
ScriptEnv.above = above

local function below(val1, val2)
	return val1 < val2 and 1 or 0
end
ScriptEnv.below = below

local function if2(bool, a, b)
	return bool == 0 and b or a
end
ScriptEnv.if2 = if2

local function sign(v)
	if v < 0 then
		return -1
	elseif v > 0 then
		return 1
	end
	return 0
end
ScriptEnv.sign = sign

local function equal(a, b)
	return a == b and 1 or 0
end
ScriptEnv.equal = equal

local random = math.random
local function rand(val)
	val = val or 100
	return random() * val
end
ScriptEnv.rand = rand