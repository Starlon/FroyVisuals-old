--- This file borrows from Winamp's open source plugin known as AVS.
--- Copyright 2005 Nullsoft, Inc.
--- All rights reserved.

local MAJOR = "LibScriptablePluginBlend-1.0"
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub")
local LibPluginBlend = LibStub:NewLibrary(MAJOR, MINOR)
if not LibPluginBlend then return end

local bit = bit
local band = bit.band
local bor = bit.bor
local bxor = bit.bxor
local lshift = bit.lshift
local rshift = bit.rshift
local min = math.min
local max = math.max

local ScriptEnv = {}
local blendtable = {}

for j = 0, 255 do
	blendtable[j] = {}
	for i = 0, 255 do
		blendtable[j][i] = i / 255 * j
	end
end

-- Populate an environment with this plugin's fields
-- @usage :New(environment) 
-- @parma environment This will be the environment when setfenv is called.
-- @return A new plugin object, aka the environment
function LibPluginBlend:New(environment)
	for k, v in pairs(ScriptEnv) do
		environment[k] = v
	end
	return environment
end

--- Blend two colors
-- @usage BLEND(a, b)
-- @param a First color to blend
-- @param b Second color to blend
-- @return The new color
local function BLEND(a, b)
	local r=band(a,0xff)+band(b,0xff);
	local t=min(r,0xff);
	r=band(a,0xff00)+band(b,0xff00);
	t = bor(t, min(r,0xff00))
	r=band(a,0xff0000)+band(b,0xff0000);
	t = bor(t, min(r,0xff0000))
	r=band(a,0xff000000)+band(b,0xff000000);
	return bor(t,min(r,0xff000000))
end
ScriptEnv.BLEND = BLEND

--- Blend two colors
-- @usage BLEND_MAX(a, b)
-- @param a First color to blend
-- @param b Second color to blend
-- @return The new color
local function BLEND_MAX(a, b)
	local _a=band(a, 0xff)
	local _b=band(b, 0xff)
	local t=max(_a,_b);
	_a=band(a, 0xff00)
	_b=band(b, 0xff00)
	t = bor(t, max(_a,_b))
	_a = band(a, 0xff0000)
	_b = band(b, 0xff0000)
	t = bor(t, max(_a,_b))
	return t;

end
ScriptEnv.BLEND_MAX = BLEND_MAX

--- Blend two colors
-- @usage BLEND_MIN(a, b)
-- @param a First color to blend
-- @param b Second color to blend
-- @return The new color
local function BLEND_MIN(a, b)
	local _a=band(a, 0xff)
	local _b=band(b, 0xff)
	local t=min(_a,_b);
	_a=band(a, 0xff00)
	_b=band(b, 0xff00)
	t = bor(t, min(_a,_b))
	_a = band(a, 0xff0000)
	_b = band(b, 0xff0000)
	t = bor(t, min(_a,_b))
	return t;
end
ScriptEnv.BLEND_MIN = BLEND_MIN

--- Blend two colors
-- @usage BLEND(a, b)
-- @param a First color to blend
-- @param b Second color to blend
-- @return The new color
local function BLEND_AVG(a, b)
	a = rshift(a, 1)
	b = rshift(b, 1)
	local l1 = lshift(1, 7)
	local l2 = lshift(1, 15)
	local l3 = lshift(1, 23)
	return band(a, bnot(bor(l1, bor(l2, l3)))) + band(b, bnot(bor(l1, bor(l2, l3))))
end
ScriptEnv.BLEND_AVG = BLEND_AVG

--- Blend two colors
-- @usage BLEND_SUB(a, b)
-- @param a First color to blend
-- @param b Second color to blend
-- @return The new color
local function BLEND_SUB(a, b)
	local r=band(a, 0xff)-band(b, 0xff);
	local t = max(r, 0)
	r=band(a, 0xff00)-band(b, 0xff00);
	t = bor(t, max(r,0))
	r=band(a, 0xff0000)-band(b, 0xff0000);
	t = bor(t, max(r,0))
	r=band(a, 0xff000000)-band(b, 0xff000000);
	return bor(t, max(r,0))
end
ScriptEnv.BLEND_SUB = BLEND_SUB

--- Blend two colors
-- @usage BLEND_ADJ(a, b)
-- @param a First color to blend
-- @param b Second color to blend
-- @return The new color
local function  BLEND_ADJ(a, b, v)
	local t = blendtable[band(a, 0xff)][v]+blendtable[band(b, 0xff)][0xff-v]
	t = bor(t, lshift(blendtable[rshift(band(a, 0xff00), 8)][v]+blendtable[rshift(band(b, 0xff00), 8)][0xff-v], 8))
	t = bor(t, lshift(blendtable[rshift(band(a, 0xff00), 16)][v]+blendtable[rshift(band(b, 0xff0000), 16)][0xff-v], 16))
	return t;
end
ScriptEnv.BLEND_ADJ = BLEND_ADJ

--- Blend two colors
-- @usage BLEND_MUL(a, b)
-- @param a First color to blend
-- @param b Second color to blend
-- @return The new color
local function BLEND_MUL(a, b)
	local t = blendtable[band(a, 0xff)][band(b, 0xff)]
	t = bor(t, lshift(blendtable[rshift(band(a, 0xff00), 8)][rshift(band(b, 0xff00), 8)], 8))
	t = bor(t, lshift(blendtable[rshift(band(a, 0xff0000), 16)][rshift(band(b, 0xff0000), 16)], 16))
	return t;
end
ScriptEnv.BLEND_MUL = BLEND_MUL

--- Blend a cell within a buffer with a color
-- @usage BLEND_LINE(line_blend_mod, fb, n, color)
-- @param line_blend_mode Which mode to blend by
-- @param fb A table of color values
-- @param n The position within the table that'll be blended
-- @param color The color to blend
-- @return Nothing
local function BLEND_LINE(line_blend_mode, fb, n, color)
	if band(line_blend_mode, 0xff) == 2 then
		fb[n]=BLEND(fb[n],color);
	elseif band(line_blend_mode, 0xff) == 3 then
		fb[n]=BLEND_MAX(fb[n],color);
	elseif band(line_blend_mode, 0xff) == 4 then
		fb[n]=BLEND_AVG(fb[n],color);
	elseif band(line_blend_mode, 0xff) == 5 then
		fb[n]=BLEND_SUB(fb[n],color);
	elseif band(line_blend_mode, 0xff) == 6 then
		fb[n]=BLEND_SUB(color,fb[n]);
	elseif band(line_blend_mode, 0xff) == 7 then
		fb[n]=BLEND_MUL(fb[n],color);
	elseif band(line_blend_mode, 0xff) == 8 then
		fb[n]=BLEND_ADJ(fb[n], color, band(rshift(line_blend_mode, 8), 0xff))
	elseif band(line_blend_mode, 0xff) == 9 then
		fb[n]=bxor(fb[n], color)
	elseif band(line_blend_mode, 0xff) == 10 then
		fb[n]=BLEND_MIN(fb[n],color);
	else
		fb[n]=color;
	end
end
ScriptEnv.BLEND_LINE = BLEND_LINE

--[[
local function BLEND4(AvsGlobalProxy *obj, *p1, w, int xp, int yp)
  register int t;
  unsigned char a1,a2,a3,a4;
  a1=obj->blendtable[255-xp][255-yp];
  a2=obj->blendtable[xp][255-yp];
  a3=obj->blendtable[255-xp][yp];
  a4=obj->blendtable[xp][yp];
  t=obj->blendtable[p1[0]&0xff][a1]+obj->blendtable[p1[1]&0xff][a2]+obj->blendtable[p1[w]&0xff][a3]+obj->blendtable[p1[w+1]&0xff][a4];
  t|=(obj->blendtable[(p1[0]>>8)&0xff][a1]+obj->blendtable[(p1[1]>>8)&0xff][a2]+obj->blendtable[(p1[w]>>8)&0xff][a3]+obj->blendtable[(p1[w+1]>>8)&0xff][a4])<<8;
  t|=(obj->blendtable[(p1[0]>>16)&0xff][a1]+obj->blendtable[(p1[1]>>16)&0xff][a2]+obj->blendtable[(p1[w]>>16)&0xff][a3]+obj->blendtable[(p1[w+1]>>16)&0xff][a4])<<16;
  return t;
 end


local function BLEND4_16(AvsGlobalProxy *obj, *p1, w, int xp, int yp)
  register int t;
  unsigned char a1,a2,a3,a4;
  xp=(xp>>8)&0xff;
  yp=(yp>>8)&0xff;
  a1=obj->blendtable[255-xp][255-yp];
  a2=obj->blendtable[xp][255-yp];
  a3=obj->blendtable[255-xp][yp];
  a4=obj->blendtable[xp][yp];
  t=obj->blendtable[p1[0]&0xff][a1]+obj->blendtable[p1[1]&0xff][a2]+obj->blendtable[p1[w]&0xff][a3]+obj->blendtable[p1[w+1]&0xff][a4];
  t|=(obj->blendtable[(p1[0]>>8)&0xff][a1]+obj->blendtable[(p1[1]>>8)&0xff][a2]+obj->blendtable[(p1[w]>>8)&0xff][a3]+obj->blendtable[(p1[w+1]>>8)&0xff][a4])<<8;
  t|=(obj->blendtable[(p1[0]>>16)&0xff][a1]+obj->blendtable[(p1[1]>>16)&0xff][a2]+obj->blendtable[(p1[w]>>16)&0xff][a3]+obj->blendtable[(p1[w+1]>>16)&0xff][a4])<<16;
  return t;
end
]]



