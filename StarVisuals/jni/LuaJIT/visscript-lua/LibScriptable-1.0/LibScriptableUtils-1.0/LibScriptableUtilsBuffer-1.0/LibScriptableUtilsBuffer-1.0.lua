local MAJOR = "LibScriptableUtilsBuffer-1.0"
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub")
local LibBuffer = LibStub:NewLibrary(MAJOR, MINOR)
if not LibBuffer then return end
local LibError = LibStub("LibScriptableUtilsError-1.0")
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")

local strlen = string.len

local pool = setmetatable({}, {__mode = "k"})

if not LibBuffer.__index then
	LibBuffer.__index = LibBuffer
end

table.wipe = function(tbl) 
	if type(tbl) ~= "table" then return end
	for i = 1, #tbl do
		table.remove(tbl, 1)
	end
end

local new, del
do
	local pool = setmetatable({}, {__mode = "k"})
	function new(size)
		local obj = next(pool)
		if obj then
			local smallest = obj
			for tbl in pairs(pool) do
				if #tbl >= size and #tbl < #smallest then
					smallest = tbl
				end
			end
			obj = smallest
			pool[obj] = nil
		else
			obj = setmetatable({}, {__index=function(this, i)
				if type(i) ~= "number" then return nil end
				if rawget(this, i) == true then return nil end
				return rawget(this, i)
			end})
		end
		return obj
	end
	function del(obj)
		pool[obj] = true
	end
end

--- Create a new LibScriptableBuffer object
-- @usage LibBuffer:New(name, size, defval, errorLevel)
-- @param name A name for the buffer
-- @param size The buffer's size
-- @param defval The buffer's default value. The alpha channel.
-- @param errorLevel The verbocity level
-- @return A new LibScriptableBuffer object
function LibBuffer:New(name, size, defval, errorLevel)
	assert(type(name) ~= "nil", "LibBuffer requires a name")
	assert(type(size) ~= "nil", "Buffer size is required")
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
	end

	obj.buffer = new(size)

	setmetatable(obj, self)

	obj.name = name
	obj.defval = defval or " "
	obj.error = LibError:New(name, errorLevel)
	obj:Resize(size)

	return obj

end

--- Delete this buffer
-- @usage buffer:Del()
function LibBuffer:Del(buff)
	pool[self] = true
	self.error:Del()
	for i = 0, self:Size() do
		self.buffer[i] = false
	end
	self.size = 0
	del(self.buffer)
end

--- Make a copy of this buffer
-- @usage Clone([name])
-- @param name An optional name for the buffer, else this buffer's name is used
-- @return A new LibBuffer populated with this buffer's data
function LibBuffer:Clone(name)
	if not name then name = self.name end
	local buf = LibBuffer:New(name, self.size, self.defval, self.errorLevel)
	for i = 0, self.size - 1 do
		if type(buf.buffer[i]) ~= "boolean" then
			buf.buffer[i] = self.buffer[i]
		end
	end
	return buf
end

--- Wipe a buffer, 
-- @usage :Wipe()
-- @return Nothing
function LibBuffer:Wipe()
	for i = 0, self:Size() - 1 do
		self.buffer[i] = self.defval
	end
end

--- Fill a buffer with a value
-- @usage :Fill(val)
-- @param val The value to fill the buffer with
-- @return Nothing
function LibBuffer:Fill(val)
	for i = 0, self:Size() - 1 do
		self.buffer[i] = val
	end
end

--- Resize a buffer
-- @usage :Resize(size)
-- @param size The buffer's new size
-- @return Nothing
function LibBuffer:Resize(size)
	assert(self.defval ~= nil)
	assert(type(size) == "number")
	if self:Size() < size then
		for i = self:Size(), size - 1 do
			self.buffer[i] = self.defval
		end
	end

	if self:Size() > size then
		for i = size - 1, self:Size() do
			self.buffer[i] = false
		end
	end

	self.size = size

	if size > 0 then
		for i = 0, size - 1 do
			assert(self.buffer[i] ~= nil, string.format("resize %d %d", i, size))
		end
	end
end

--- Replace a buffer position with a new value.
-- @usage :Replace(pos, val)
-- @param pos The position in the buffer to replace
-- @param val The new value
-- @return Nothing
function LibBuffer:Replace(pos, val)
	assert(type(pos) == "number", "Position sent to Replace isn't a number")
	assert(val ~= nil, "Val sent to Replace is nil")
	if pos > self.size then self.size = pos end
	self.buffer[pos] = val
end

--- Insert a new value in the buffer at the indicated index
-- @usage :Insert(i, val)
-- @param i The position in the buffer
-- @param val The value to insert
-- @return Nothing
function LibBuffer:Insert(i, val)
	local sub = self:Sub(i + 1)
	self.buffer[0] = val
	self:Merge(sub, i, sub:Size() - 1)
	sub:Del()
end

--- Clear this buffer
-- @usage :Clear()
-- @return Nothing
function LibBuffer:Clear()
	for i = 0, self:Size() - 1 do
		self.buffer[i] = self.defval
	end
end

local pool = setmetatable({}, {__mode="k"})
local function new()
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
	end

	table.wipe(obj)
	return obj
end

local function del(obj)
	pool[obj] = true
end

--- Return the buffer as a concatenated string.
-- @usage :AsString()
-- @return All cells in the buffer concatenated as a string
function LibBuffer:AsString()
	assert(type(self.defval) == "string")
	local tbl = new()
	table.wipe(tbl)
	for i = 0, self:Size() - 1 do
		if type(self.buffer[i]) == "string" then
			tbl[i + 1] = self.buffer[i]
		else
			tbl[i + 1] = ""
		end
	end
	local str = table.concat(tbl)
	table.wipe(tbl)
	del(tbl)
	return str
end

--- Return the buffer as a list
-- @usage :AsString()
-- @return All cells in the buffer populating a list
function LibBuffer:AsList(list)
	list = list or {}
	
	for i = 0, self:Size() - 1 do
		tinsert(list, self.buffer[i])
	end
	
	return list
end

--- Populate each cell in the buffer with characters from the provided string
-- @usage :FromString(str)
-- @param str The string which to populate the buffer with
-- @return Nothing
function LibBuffer:FromString(str)
	if type(str) ~= "string" then return end
	for i = 0, strlen(str) - 1 do
		self.buffer[i] = str:sub(i + 1, i + 1)
	end
	if strlen(str) < self:Size() then
		for i = strlen(str) - 1, self:Size() do
			self.buffer[i] = false
		end
	end
	self.size = strlen(str)
end

--- Merge a string into the buffer
-- @usage :MergeString(pos1, str, pos2, length)
-- @param pos1 The index in this buffer to start merging from
-- @param str The string to merge
-- @param pos2 The position in the string from which to start
-- @param length This is best expressed with a Lua expression :) str:sub(pos2, length) 
-- @return Nothing
function LibBuffer:MergeString(pos1, str, pos2, length)
	for n = 0, length - 1 do
		self.buffer[pos1 + n] = str:sub(pos2, pos2 + length)
	end
end

--- Populate this buffer with the elements within the provided table
-- @usage :FromList(list)
-- @param list The table from which we'll copy into this buffer
-- @return Nothing
function LibBuffer:FromList(list)
	if type(list) ~= "table" then return end
	for i, v in ipairs(list) do
		self.buffer[i - 1] = v
	end
	self.size = #list
end

--- Return the size of this buffer
-- @usage :Size()
-- @return The size of this buffer
function LibBuffer:Size()
	return self.size or 0
end

--- Extract a subsection from this buffer
-- @usage :Sub(num1, num2)
-- @param num1 The starting position in the buffer from which to copy
-- @param num2 THe ending position in the buffer from which to copy
-- @param t An optional LibBuffer object
-- @return A new LibBuffer populated with this table subsection between num1 and num2
function LibBuffer:Sub(num1, num2, buf)

	if not num2 then num2 = self:Size() - 1 end
	if num1 > num2 or num2 > self:Size() - 1 then return end

	local t = buf or LibBuffer:New(self.name .. ":" , num2 - num1 + 1, self.defval, self.errorLevel)
	
	for i = num1, num2 do
		t.buffer[i - num1] = self.buffer[i] or self.defval
	end
	return t
end

--- Copy this buffer into another
-- @usage :Copy(other)
-- @param other The other LibBuffer object to copy into
-- @return Nothing
function LibBuffer:Copy(other)
	table.wipe(self.buffer)
	for k, v in pairs(other.buffer) do
		self.buffer[k] = other.buffer[k]
	end
end

--- Merge one buffer into this one
-- @usage :Merge(from, pos, len)
-- @param from The buffer from which we'll copy
-- @param pos The position in the other buffer that we'll start copying from
-- @param len The length of the content to merge
-- @return Nothing
function LibBuffer:Merge(from, pos, len)
	local i = 0
	while i < len - (self.size - len) do
		assert(from.buffer[i] ~= nil)
		self.buffer[i + pos] = from.buffer[i]
		i = i + 1
	end
end

--- Set a subsection of this buffer to the specified value
-- @usage :Memset(pos, val, length)
-- @param pos The starting position within this buffer to start assigning the specified value
-- @param val The value to assign to this buffer's subsection described by this method's other parameters
-- @param length The length of the subsection which is being assigned the specified value
-- @return Nothing
function LibBuffer:Memset(pos, val, length)
	for n = pos, pos + length - 1 do
		self.buffer[n] = val
	end
end

--- Copy another buffer into this one
-- @usage :Memcopy(pos1, buffer, pos2, length)
-- @param pos1 The starting position in this buffer to copy
-- @param buffer The other buffer to copy
-- @param pos2 The starting position in the other buffer where we'll begin copying
-- @param length The length of the subsection being copied
-- @return Nothing
function LibBuffer:Memcopy(pos1, buffer, pos2, length)
	for n = 0, length - 1 do
		self.buffer[pos1 + n] = buffer.buffer[pos2 + n] or self.defval
	end
end

local function randomize(a, b)
	if random(2) == 1 then
		return a < b
	else
		return a > b
	end
end

local function copy(tbl)
	if type(tbl) ~= "table" then return tbl end
	local new = new(#tbl)
	for k, v in pairs(tbl) do
		new[k + 1] = copy(v)
	end
	return new
end

--- Randomize the buffer
-- @usage :Randomize()
function LibBuffer:Randomize()
	local c = copy(self.buffer)
	local len = #c
	for i = 0, self:Size() - 1 do
		local i = random(#c)
		self.buffer[i] = c[i]
		tremove(c, i)
	end
	for i = 0, len - 1 do
		c[i] = true
	end
	del(c)
	return self
end

--- Retrieve the moving averages for this buffer of number values
-- @usage :MovingAverageExp(alpha, epsilon, buf)
-- @return A new buffer populated with this buffer's moving averages
function LibBuffer:MovingAverageExp(alpha, epsilon, buf)
	if type(epsilon) ~= "number" then epsilon = 0 end

	if not (0 < alpha and alpha < 1) then
		return
	end

	if not (0 <= epsilon and epsilon < alpha) then
		return
	end

	result = buf or LibBuffer:New("result", self:Size(), 0, self.errorLevel)

	for i = 0, result:Size() - 1 do
		local currentWeight = 1.0
		local numerator = 0
		local denominator = 0
		local sub = self:Sub(i)
		for j = 0, sub:Size() - 1 do
			local value = sub.buffer[j]
			numerator = numerator + value * currentWeight
			denominator = denominator + currentWeight

			currentWeight = currentWeight * alpha
			if currentWeight < epsilon then break end
		end
		result:Replace(i, numerator / denominator)
		sub:Del()
	end

	return result
end

--- The various line algorithms were found on the internet at various places. 
-- Line(), Line2(), Line3(), Line4(), Line5()
-- Some of these were found at the website of associate professor Leonard McMillan: 
-- http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html
-- Some of these don't work. Line5() is the most efficient algorithm.
-- The others should be regarded as deprecated as they will likely be removed.
-- @param x0 First X position
-- @param y0 First Y position
-- @param x1 Second X position
-- @param y1 Second Y position
-- @param color The line will be filled with this value
-- @param pitch The buffer's width.
--[[
function LibBuffer:Line(x0, y0, x1, y1, color, pitch)
	local dx = x1 - x0;
	local dy = y1 - y0;

	self.buffer[x0 + y0 * pitch] = color

	if (dx ~= 0) then
		local m = dy / dx;
		local b = y0 - m*x0;
		if x1 > x0 then
			dx = 1
		else
			dx = -1
		end
		while x0 ~= x1 do
			x0 = x0 + dx
			y0 = math.floor(m*x0 + b + 0.5);
			self.buffer[x0 + y0 * pitch] = color
		end

    end
end

function LibBuffer:Line2(x0, y0, x1, y1, color, pitch)
	local dx = x1 - x0;
	local dy = y1 - y0;

	self.buffer[x0 + y0 * pitch] = color
	if math.abs(dx) > math.abs(dy) then -- slope < 1
		local m = dy / dx -- compute slope
		local b = y0 - m*x0
		if dx < 0 then
			dx = -1
		else
			dx = 1
		end
		while x0 ~= x1 do
			x0 = x0 + dx
			local n = floor(m*x0 + b + 0.5) * pitch + x0
			self.buffer[n] = color
		end

	elseif dy ~= 0 then -- slope >= 1
		local m = dx / dy -- compute slope
		local b = x0 - m*y0
		if dx < 0 then
			dx = -1
		else
			dx = 1
		end
		while y0 ~= y1 do
			y0 = y0 + dy
			local n = y0 * pitch + floor(m*y0 + b + 0.5)
			self.buffer[n] = color
		end
	end
end

function LibBuffer:Line3(x0, y0, x1, y1, color, pitch)
	local dx = x1 - x0;
	local dy = y1 - y0;
	local t = 0.5

	self.buffer[x0 + y0 * pitch] = color
	if math.abs(dx) > math.abs(dy) then -- slope < 1
		local m = dy / dx -- compute slope
		t = t + y0
		--local b = y0 - m*x0
		if dx < 0 then
			dx = -1
		else
			dx = 1
		end
		m = m * dx
		while x0 ~= x1 do
			x0 = x0 + dx
			t = t + m
			self.buffer[ t * pitch + x0] = color
		end

	elseif dy ~= 0 then -- slope >= 1
		local m = dx / dy -- compute slope
		t = t + x0
		if dy < 0 then
			dy = -1
		else
			dy = 1
		end
		m = m * dy
		while y0 ~= y1 do
			y0 = y0 + dy
			t = t + m
			self.buffer[y0 * pitch + x0] = color
		end
	end
end
]]

function LibBuffer:Line4(x0, y0, x1, y1, color, pitch)
	local dx = x1 - x0;
	local dy = y1 - y0;
	local stepx, stepy

	if dy < 0 then
		dy = -dy
		stepy = -1
	else
		stepy = 1
	end

	if dx < 0 then
		dx = -dx
		stepx = -1
	else
		stepx = 1
	end

	self.buffer[x0 + y0 * pitch] = color
	if dx > dy then
		local fraction = dy - rshift(dx, 1)
		while x0 ~= x1 do
			if fraction >= 0 then
				y0 = y0 + stepy
				fraction = fraction - dx
			end
			x0 = x0 + stepx
			fraction = fraction + dy
			self.buffer[floor(y0) * pitch + floor(x0)] = color
		end
	else
		local fraction = dx - rshift(dy, 1)
		while y0 ~= y1 do
			if fraction >= 0 then
				x0 = x0 + stepx
				fraction = fraction - dy
			end
			y0 = y0 + stepy
			fraction = fraction + dx
			self.buffer[floor(y0) * pitch + floor(x0)] = color
		end
	end
end

local lshift = bit.lshift
local rshift = bit.rshift
function LibBuffer:Line5(x0, y0, x1, y1, color, pitch)

        local dy = y1 - y0;
        local dx = x1 - x0;
        local stepx, stepy;

        if (dy < 0) then dy = -dy;  stepy = -pitch; else stepy = pitch; end
        if (dx < 0) then dx = -dx;  stepx = -1; else stepx = 1; end

	lshift(dy, 1)
	lshift(dx, 1)

	y0 = y0 * pitch
	y1 = y1 * pitch
	self.buffer[x0+y0] = color
        if (dx > dy) then
		local fraction = dy - rshift(dx, 1)
		while (x0 ~= x1) do
			if (fraction >= 0) then
				y0 = y0 + stepy
				fraction = fraction - dx
	                end
				x0 = x0 + stepx
				fraction = fraction + dy
				self.buffer[x0+y0] = color
		end
	else
		local fraction = dx - rshift(dy, 1)
		while (y0 ~= y1) do
			if (fraction >= 0) then
				x0 = x0 + stepx
				fraction = fraction - dy
			end
			y0 = y0 + stepy
			fraction = fraction + dx
			self.buffer[x0+y0] = color
		end
	end
end
