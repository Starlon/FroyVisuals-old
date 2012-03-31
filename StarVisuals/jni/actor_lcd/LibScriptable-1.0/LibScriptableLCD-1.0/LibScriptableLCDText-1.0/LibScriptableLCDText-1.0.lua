local MAJOR = "LibScriptableLCDText-1.0"
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub")
local LCDText = LibStub:NewLibrary(MAJOR, MINOR)
if not LCDText then return end
local LibError = LibStub("LibScriptableUtilsError-1.0")
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local LibBuffer = LibStub("LibScriptableUtilsBuffer-1.0", true)
assert(LibBuffer, MAJOR .. " requires LibScriptableUtilsBuffer-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")

local pool = setmetatable({}, {__mode = "k"})

local objects = {}
local objectsDict = {}
local update

local tinsert = table.insert
local tremove = table.remove

local DEFAULT_LIMIT = 50

if not LCDText.__index then
	LCDText.__index = LCDText
end

-- @name LibScriptableLCDText:New
-- @usage LibScriptableLCDText:New(visitor, rows, cols, yres, xres, layers)
-- @param visitor A LibScriptableCore object
-- @param rows The number of rows
-- @param cols The number of cols
-- @param yres The y resolution for characters
-- @param xres The x resolution for characters
-- @param layers The number of layers
-- @param errorLevel Error verbosity level
-- @param realBlit Your driver's blit function
-- @param data Some data to pass back to the realBlit function
-- @param update Refresh interval. Set to 0 or nil to update in real time.
-- @return A new LibScriptableLCDText object
function LCDText:New(visitor, rows, cols, yres, xres, layers, errorLevel, realBlit, data, update)

	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
		obj.LayoutFB = {}
		obj.TransitionFB = {}
		obj.BoldFB = {}
		obj.specialChars = {}
	end

	setmetatable(obj, self)

	tinsert(objects, obj)
	objectsDict[obj] = obj

	obj.error = LibError:New(MAJOR, errorLevel)
	obj.visitor = visitor
	obj.LROWS = rows
	obj.LCOLS = cols
	obj.DROWS = rows
	obj.DCOLS = cols
	obj.YRES = yres
	obj.XRES = xres
	obj.LAYERS = layers
	obj.errorLevel = errorLevel
	obj.RealBlit = realBlit
	obj.data = data
	obj.RealDefChar = realDefChar
	obj.transition_tick = 0

	local n = rows * cols

	for i = 0, layers - 1 do
		obj.LayoutFB[i] = LibBuffer:New(MAJOR .. ".LayoutFB", n, " ", self.errorLevel)
		obj.TransitionFB[i] = LibBuffer:New(MAJOR .. ".TransitionFB", n, " ", self.errorLevel)
		obj.BoldFB[i] = LibBuffer:New(MAJOR .. ".BoldFB", n, 0, self.errorLevel)
	end
	obj.DisplayFB = LibBuffer:New(MAJOR .. ".DisplayFB", n, "", self.errorLevel)
	
	if type(update) == "number" and update > 0 then
		obj.timer = LibTimer:New(MAJOR .. ": " .. visitor.name, update, true, obj.Update, obj, obj.errorLevel)
		obj:Start()
	end

	return obj

end

-- Delete a LCDText object
-- @usage StarLCDText:Del([lcd]) or object:Del()
-- @param lcd An optional lcd object
-- @return Nothing
function LCDText:Del(lcd)
	if not lcd then lcd = self end

	if type(lcd) == "table" then
		pool[lcd] = true
		lcd.error:Del()
		for i = 0, self.LAYERS do
			obj.LayoutFB[i]:Del()
			obj.TransitionFB[i]:Del()
		end
		lcd.DisplayFB:Del()
	end
end

-- Clear the frame buffers
-- @usage :Clear()
function LCDText:Clear()
	for i = 0, self.LROWS * self.LCOLS - 1 do
		for l = 0, self.LAYERS - 1 do
			self.LayoutFB[l]:Clear()
			self.TransitionFB[l]:Clear()
			self.BoldFB[l]:Clear()
		end
	end
	self:TextBlit(0, 0, self.LROWS, self.LCOLS)
end

-- Start the LCD's internal timer
-- @usage :Stop()
function LCDText:Start()
	if self.timer then
		self.timer:Start()
	end
end

-- Stop the LCD's internal timer
-- @usage :Stop()
function LCDText:Stop()
	if self.timer then
		self.timer:Stop()
	end
end

function LCDText:Update()
	if not self.transitioning then
		self:TextBlit(0, 0, self.LROWS, self.LCOLS)
	end
end

function LCDText:TextBlit(row, col, height, width)
	--local lr, lc
	--local dr, dc
	--local p1, p2
	local fb = LibBuffer:New(MAJOR .. "._fb", self.LROWS * self.LCOLS, " ", self.errorLevel)
	local bold = LibBuffer:New(MAJOR .. "._bold", self.LROWS * self.LCOLS, 0, self.errorLevel)

	local r = row

	while r < self.LROWS and r < row + height do
		local c = col
		while c < self.LCOLS and c < col + width do
			local n = r * self.LCOLS + c
			local l = self.LAYERS - 1
			while l >= 0 do
				self.BoldFB[l]:Clear()
				if self.LayoutFB[l].buffer[n] ~= ' ' and self.LayoutFB[l].buffer[n] ~= false then
					if self.BoldFB[l].buffer[n] == 1 then
						bold.buffer[n] = 1
					else
						bold.buffer[n] = 0
					end
					assert(self.LayoutFB[l].buffer[n], string.format("1 %d %d", n, self.LROWS * self.LCOLS))
					fb:Replace(n, self.LayoutFB[l].buffer[n] or " ")
				end
				l = l - 1
			end
			c = c + 1
		end
		r = r + 1
	end

	--fb:Copy(self.LayoutFB[0])
	for i = 0, fb:Size() - 1 do
		assert(fb.buffer[i], string.format("2 %d %d", i, fb:Size()))
	end

	--/* loop over layout rows */
	local lr = row

	while lr < self.LROWS and lr < row + height do
		--/* transform layout to display row */
		local dr = lr;
		--/* sanity check */
		if dr >= 0 and dr < self.DROWS then
			--/* loop over layout cols */
			local lc = col
			while lc < self.LCOLS and lc < col + width do
				--/* transform layout to display column */
				local dc = lc;
				--/* sanity check */
				if dc >= 0 and dc < self.DCOLS then
					--/* find start of difference */
					if self.DisplayFB.buffer[dr * self.DCOLS + dc] ~= fb.buffer[lr * self.LCOLS + lc] then
					--if string.sub(self.DisplayFB, dr * self.DCOLS + dc, 1) ~= string.sub(fb, lr * self.LCOLS + lc) then
						--/* find end of difference */
						local p1 = dc
						local p2 = p1
						while lc < self.LCOLS and lc < col + width do
							--/* transform layout to display column */
							dc = lc;
							--/* sanity check */
							if dc >= 0 and dc < self.DCOLS then
								if self.DisplayFB.buffer[dr * self.DCOLS + dc] ~= fb.buffer[lr * self.LCOLS + lc] then
									p2 = dc;
								end
							end
							lc  = lc + 1
						end

						--/* send to display */
						local from = fb:Sub(lr * self.LCOLS + p1)
						self.DisplayFB:Memcopy(dr * self.DCOLS + p1, from, 0, p2 - p1 + 1)
						--self.DisplayFB:Merge(from, dr * self.DCOLS + p1, p2 - p1 + 1)
						if (self.RealBlit) then
							local n = dr * self.DCOLS + p1
							local tmp = self.DisplayFB:Sub(n, n + p2 - p1)
							local tmpbold = bold:Sub(n, n + p2 - p1)
							self:RealBlit(self.data, dr, p1, tmp.buffer, p2 - p1 + 1, tmpbold)							
							tmp:Del()
							tmpbold:Del()
						end
						from:Del()
					end
				end
				lc = lc + 1
			end
		end
		lr = lr + 1
	end
	fb:Del()
	bold:Del()
end

function LCDText:SetSpecialChars()
	for i = 1, #self.specialChars do
		self:SpecialCharChanged(i);
	end
	--emit static_cast<LCDEvents *>(wrapper_)->_TextSpecialCharsSet();
end

function LCDText:SpecialCharChanged(ch)
	assert(type(ch) == "number")
	if( ch > #self.specialChars ) then
		return;
	end

	if(self.RealDefChar) then
		self.RealDefChar(self.data, ch, self.specialChars[ch]);
	else
		self.error:Print("No RealDefChar", 1)
	end
end

function LCDText.TextDraw(widget, data)
	local lcd = widget.visitor.lcd
	local n  = widget.widget.row * lcd.LCOLS + widget.widget.col
	widget.buffer = widget.buffer or "error"
	local len = string.len(widget.buffer)
	local layer = widget.widget.layer
	local fb;
	if lcd.transitioning and widget.widget.layout_base == lcd.transitionLayout then
		fb = lcd.TransitionFB[layer];
	else
		fb = lcd.LayoutFB[layer];
	end

	for i = n, n + len do
		if i - widget.widget.row * lcd.LCOLS >= lcd.LCOLS then
			break
		end
		fb:Replace(i, widget.buffer:sub(i - n + 1, i - n + 1))
		if widget.bold then
			lcd.BoldFB[layer].buffer[n] = 1
		end
	end

	if not lcd.transitioning and not lcd.timer then
		lcd:TextBlit(widget.widget.row, widget.widget.col, 1, len)
	end

end

function LCDText.BarDraw(widget)
	local lcdText = widget.widget.visitor.lcd
	local length = widget.length
	local row = widget.widget.row
	local col = widget.widget.col
	local layer = widget.widget.layer

	local val = math.floor((widget.val1 * length) + 0.5 );

	if(val < 0) then val = 0 end

	local n = row * lcdText.LCOLS + col;

	local fb;
	if lcdText.transitioning and widget.widget.layout_base == lcdText.transitionLayout then
		fb = lcdText.TransitionFB[layer];
	else
		fb = lcdText.LayoutFB[layer];
	end

	for x = n, n + length do
		fb.buffer[x] = ' ';
	end

	if( widget.direction == widget.DIR_EAST ) then
		for i = n, n + val - 1 do
			if mod(i, lcdText.LCOLS) >= lcdText.LCOLS then
				break
			end
			fb.buffer[i] = "#"
		end
	elseif( widget.direction == widget.DIR_WEST ) then
		for i = n, n - val + 1, -1 do
			if i / lcdText.LROWS < row then
				break
			end
			fb.buffer[i] = "#"
		end
	end
	if not lcdText.transitioning and not lcdText.timer then
		lcdText:TextBlit(row, col, 1, length)
	end
end

function LCDText.HistogramDraw(widget)
	local lcdText = widget.widget.visitor.lcd

	local row = widget.widget.row
	local col = widget.widget.col
	local width = widget.width
	local height = widget.height
	local layer = widget.widget.layer

	local fb;
	if(lcdText.transitioning and widget.widget.layout_base == lcdText.transitionLayout) then
		fb = lcdText.TransitionFB[layer]
	else
		fb = lcdText.LayoutFB[layer]
	end

	if not fb then
		widget.error:Print("Bad layer " .. layer)
		return
	end

	for c = col, col + width - 1 do
		for r = row, row + height - 1 do
			fb.buffer[r * lcdText.LCOLS + c] = " "
		end
	end

	for c = col, col + width - 1 do
		local val
		if c - col >= #widget.history then
			val = 0
		else
			val = floor(widget.history[c - col + 1][1] * height + 0.5)
		end
		if not widget.reversed then
			for r = row, row + val - 1 do
				fb.buffer[(row + r) * lcdText.LCOLS + c] = widget.char or "#"
			end
		else

			for r = row + height, row + height - val, -1 do
				fb.buffer[(row + r) * lcdText.LCOLS + c] = widget.char or "#"
			end
		end
	end

	if not lcdText.transitioning and not lcdText.timer then
		lcdText:TextBlit(row, col, height, width);
	end
end

function LCDText.IconDraw(widget)
	local lcdText = widget.visitor.lcd

	local row = widget.row
	local col = widget.col
	local layer = widget.layer
	local n = row * lcdText.LCOLS + col

	local fb;
	if(lcdText.transitioning and widget.layout_base == lcdText.transitionLayout) then
		fb = lcdText.TransitionFB[layer]
	else
		fb = lcdText.LayoutFB[layer]
	end

	fb.buffer[n] = widget.index + widget.start
	
	if not lcdText.transitioning and not lcdText.timer then
		lcdText:TextBlit(row, col, 1, 1)
	end
end

function LCDText:SignalTransitionStart(layout)
	self.transitioning = true;
	self.transitionLayout = layout
end

function LCDText:SignalTransitionEnd()
	self.transitioning = false
end

function LCDText:Transition()
	if self.visitor.direction == self.visitor.TRANSITION_LEFT or
		self.visitor.direction == self.visitor.TRANSITION_RIGHT or
		self.visitor.direction == self.visitor.TRANSITION_BOTH then
		self:TransitionLeftRight()
	elseif self.visitor.direction == self.visitor.TRANSITION_UP or
		self.visitor.direction == self.visitor.TRANSITION_DOWN then
		self:TransitionUpDown()
	elseif self.visitor.direction == self.visitor.TRANSITION_TENTACLE then
		self:TransitionTentacle()
	elseif self.visitor.direction == self.visitor.TRANSITION_CHECKERBOARD then
		self:TransitionCheckerBoard()
	else
		self:TransitionLeftRight()
	end
end

function LCDText:TransitionLeftRight()
	local direction = self.visitor.direction
	local col
	local left, right
	local layout = LibBuffer:New("tmplayout", self.LROWS * self.LCOLS, " ", self.errorLevel)
	local transition = LibBuffer:New("tmptransition", self.LROWS * self.LCOLS, " ", self.errorLevel)

	for l = self.LAYERS - 1, 0, -1 do
		for n = 0, self.LROWS * self.LCOLS - 1 do
			if self.LayoutFB[l].buffer[n] ~= ' ' then
				layout.buffer[n] = self.LayoutFB[l].buffer[n]
			end
			if self.TransitionFB[l].buffer[n] ~= ' ' then
				transition.buffer[n] = self.TransitionFB[l].buffer[n]
			end
		end
	end

	for row = 0, self.LROWS - 1 do
		local n = row * self.LCOLS


		if( direction == self.visitor.TRANSITION_LEFT or
			(direction == self.visitor.TRANSITION_BOTH and mod(row, 2) == 0)) then
			col = self.LCOLS - self.transition_tick - 1;
			left = layout;
			right = transition;
		elseif( direction == self.visitor.TRANSITION_RIGHT or direction == self.visitor.TRANSITION_BOTH) then
			col = self.transition_tick;
			left = transition;
			right = layout;
		else
			col = self.transition_tick;
			left = transition;
			right = layout;
			self.error:Print("LCDText: Bad transition direction: " .. direction);
		end
		--self.DisplayFB:Memset(n, ' ', self.LCOLS)
		self.DisplayFB:Memcopy(n, left, n + (self.LCOLS - col), col)
		self.DisplayFB:Memcopy(n + col + 1, right, n, self.LCOLS - col - 1)
		--memset(DisplayFB + n, ' ', LCOLS);
		--memcpy(DisplayFB + n, left + n + (LCOLS - col), col);
		--memcpy(DisplayFB + n + col + 1, right + n, LCOLS - col - 1);
		if(direction ~= self.visitor.TRANSITION_BOTH) then
			self.DisplayFB.buffer[n + col] = ':';
		end
		if(self.RealBlit) then
			local fb = self.DisplayFB:Sub(n)
			self:RealBlit(self.data, row, 0, fb.buffer, self.LCOLS);
			fb:Del()
		end

	end

	self.transition_tick = self.transition_tick + 1
	if( self.transition_tick >= self.LCOLS ) then
		self.transitioning = false;
		self.transition_tick = 0;
		--emit static_cast<LCDEvents *>(
		--	visitor_->GetWrapper())->_TransitionFinished();
		self.visitor:TransitionFinished()
		for l = 0, self.LAYERS - 1 do
			self.LayoutFB[l]:Copy(self.TransitionFB[l])
			--self.LayoutFB[l]:Memcopy(0, self.TransitionFB[l], 0, self.LCOLS * self.LROWS)
			--memcpy(LayoutFB[l], TransitionFB[l], LCOLS * LROWS);
			self.TransitionFB[l]:Clear()
			--self.TransitionFB[l]:Memset(0, ' ', self.LCOLS * self.LROWS)
			--memset(TransitionFB[l], ' ', LCOLS * LROWS);
		end
		self:TextBlit(0, 0, self.LROWS, self.LCOLS);
	end

	layout:Del()
	transition:Del()
end

function LCDText:TransitionUpDown()
	local direction = self.visitor.direction
	local row;
	local top, bottom;
	local layout = LibBuffer:New("tmplayout", self.LROWS * self.LCOLS, " ", self.errorLevel)
	local transition = LibBuffer:New("tmptransition", self.LROWS * self.LCOLS, " ", self.errorLevel)

	for l = self.LAYERS - 1, 0, -1 do
		for n = 0, self.LROWS * self.LCOLS - 1 do
			if(self.LayoutFB[l].buffer[n] ~= ' ') then
				layout.buffer[n] = self.LayoutFB[l].buffer[n];
			end
			if(self.TransitionFB[l].buffer[n] ~= ' ') then
				transition.buffer[n] = self.TransitionFB[l].buffer[n];
			end
		end
	end

	if(direction == self.visitor.TRANSITION_UP) then
		top = layout;
		bottom = transition;
		row = self.LROWS - self.transition_tick
	else
		top = transition;
		bottom = layout;
		row = self.transition_tick
	end

	for r = 0, self.LROWS - 1 do
		local n = r * self.LCOLS;
		for l = 0, self.LAYERS do
			self.DisplayFB:Memset(n, ' ', self.LCOLS)
			if(r < row) then
				self.DisplayFB:Memcopy(n, top, n, self.LCOLS)
			else
				self.DisplayFB:Memcopy(n, bottom, n, self.LCOLS)
			end
		end
		if(self.RealBlit) then
			local fb = self.DisplayFB:Sub(n)
			self:RealBlit(self.data, r, 0, fb.buffer, self.LCOLS);
			fb:Del()
		end
	end

	self.transition_tick = self.transition_tick + 1
	if( self.transition_tick >= self.LROWS ) then
		self.transitioning = false;
		self.transition_tick = 0;
		self.visitor:TransitionFinished()
		for l = 0, self.LAYERS - 1 do
			self.LayoutFB[l]:Memcopy(0, self.TransitionFB[l], 0, self.LCOLS * self.LROWS)
			self.TransitionFB[l]:Memset(0, ' ', self.LCOLS * self.LROWS)
		end
		self:TextBlit(0, 0, self.LROWS, self.LCOLS);
	end

	layout:Del()
	transition:Del()
end

function LCDText:SaneCoords(x, y1, y2)
	if(x >= self.LCOLS) then
		x = self.LCOLS - 1;
	elseif(x < 0) then
		x = 0;
	end

	if(y1 >= self.LROWS) then
		y1 = self.LROWS - 1;
	elseif(y1 < 0) then
		y1 = 0;
	end

	if(y2 >= self.LROWS) then
		y2 = self.LROWS - 1;
	elseif(y2 < 0) then
		y2 = 0;
	end
	return floor(x + 0.5), floor(y1 + 0.5), floor(y2 + 0.5)
end

function LCDText:VlineFromBuffer(dst, src, x, y1, y2)
	x, y1, y2 = self:SaneCoords(x, y1, y2);

	for i = y1, y2 do
		dst.buffer[i * self.LCOLS + x] = src.buffer[i * self.LCOLS + x];
	end
end


function LCDText:TransitionTentacle()
	local height1;
	local height2;
	local add1;
	local add2;

	local sinrate = (self.tentacle_move or 0);
	local multiplier = 0;
	local multiadd = 7.000 / self.LCOLS;
	local rate = (self.LCOLS - self.transition_tick) / self.LCOLS;

	self.DisplayFB:Memset(0, " ", self.LCOLS * self.LROWS)

	local layout = LibBuffer:New("layout tmp", self.LCOLS * self.LROWS, " ", self.errorLevel)

	for l = self.LAYERS - 1, 0, -1 do
		for n = 0, self.LROWS * self.LCOLS - 1 do
			if self.LayoutFB[l].buffer[n] ~= ' ' then
				layout.buffer[n] = self.LayoutFB[l].buffer[n]
			end
			if self.TransitionFB[l].buffer[n] ~= ' ' then
				self.DisplayFB.buffer[n] = self.TransitionFB[l].buffer[n]
			end
		end
	end

	-- Taken from LibVisual
	for i = 0, self.LCOLS - 1 do
		add1 = (self.LROWS / 2) - ((self.LROWS / 2) * (rate * 1.5));
		add2 = (self.LROWS / 2) + ((self.LROWS / 2) * (rate * 1.5));

		height1 = (sin(sinrate) * ((self.LROWS / 8) * multiplier)) + add1;
		height2 = (sin(sinrate) * ((self.LROWS / 8) * multiplier)) + add2;
		multiplier = multiplier + multiadd

		self:VlineFromBuffer(self.DisplayFB, layout, i, height1, height2);
		sinrate = sinrate + 0.2
		self.tentacle_move = (self.tentacle_move or 0) + 0.05
	end

	layout:Del()

	self.transition_tick = self.transition_tick + 1
	if( self.transition_tick >= self.LCOLS) then
		self.transition_tick = 0;
		self.visitor:TransitionFinished()
		self.DisplayFB:Memset(0, "*", self.LCOLS * self.LROWS)
		for l = 0, self.LAYERS - 1 do
			self.LayoutFB[l]:Memcopy(0, self.TransitionFB[l], 0, self.LCOLS * self.LROWS)
			self.TransitionFB[l]:Memset(0, " ", self.LCOLS * self.LROWS)
		end
		self.transitioning = false;
		self:TextBlit(0, 0, self.LROWS, self.LCOLS);
		return;
	end
	if(self.RealBlit) then
		for row = 0, self.LROWS - 1 do
			local fb = self.DisplayFB:Sub(row * self.LCOLS)
			self:RealBlit(self.data, row, 0, fb.buffer, self.LCOLS);
			fb:Del()
		end
	end
end

function LCDText:TransitionCheckerBoard()
	local layout = LibBuffer:New("tmplayout", self.LROWS * self.LCOLS, " ", self.errorLevel)
	local transition = LibBuffer:New("tmptransition", self.LROWS * self.LCOLS, " ", self.errorLevel)

	for l = self.LAYERS - 1, 0, -1 do
		for n = 0, self.LROWS * self.LCOLS - 1 do
			if(self.LayoutFB[l].buffer[n] ~= ' ') then
				layout.buffer[n] = self.LayoutFB[l].buffer[n];
			end
			if(self.TransitionFB[l].buffer[n] ~= ' ') then
				transition.buffer[n] = self.TransitionFB[l].buffer[n];
			end
		end
	end

	self.DisplayFB:Memset(0, " ", self.LCOLS * self.LROWS)

	for row = 0, self.LROWS - 1 do
		for col = 0, self.LCOLS - 1 do
			local n = row * self.LCOLS + col
			if mod(self.transition_tick, 2) == 0 then
				if mod(col, 2) == 0 then
					if mod(row, 2) == 0 then
						self.DisplayFB:Replace(n, transition.buffer[n])
					else
						self.DisplayFB:Replace(n, layout.buffer[n])
					end
				else
					if mod(row, 2) == 0 then
						self.DisplayFB:Replace(n, layout.buffer[n])
					else
						self.DisplayFB:Replace(n, transition.buffer[n])
					end
				end
			else
				if mod(col, 2) == 0 then
					if mod(row, 2) == 0 then
						self.DisplayFB:Replace(n, layout.buffer[n])
					else
						self.DisplayFB:Replace(n, transition.buffer[n])
					end
				else
					if mod(row, 2) == 0 then
						self.DisplayFB:Replace(n, transition.buffer[n])
					else
						self.DisplayFB:Replace(n, layout.buffer[n])
					end
				end
			end
		end
	end

	if(self.RealBlit) then
		for row = 0, self.LROWS - 1 do
			local fb = self.DisplayFB:Sub(row * self.LCOLS)
			self:RealBlit(self.data, row, 0, fb.buffer, self.LCOLS);
			fb:Del()
		end
	end

	self.transition_tick = self.transition_tick + 1
	if( self.transition_tick >= self.LROWS ) then
		self.transitioning = false;
		self.transition_tick = 0;
		self.visitor:TransitionFinished()
		for l = 0, self.LAYERS - 1 do
			self.LayoutFB[l]:Memcopy(0, self.TransitionFB[l], 0, self.LCOLS * self.LROWS)
			self.DisplayFB:Memset(0, "*", self.LCOLS * self.LROWS)
			self.TransitionFB[l]:Memset(0, ' ', self.LCOLS * self.LROWS)
		end
		self:TextBlit(0, 0, self.LROWS, self.LCOLS);
	end

	layout:Del()
	transition:Del()
end
