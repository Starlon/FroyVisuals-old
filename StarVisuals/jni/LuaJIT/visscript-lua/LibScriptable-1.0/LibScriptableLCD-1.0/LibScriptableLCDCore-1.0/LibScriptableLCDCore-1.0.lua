local MAJOR = "LibScriptableLCDCore-1.0" 
local MINOR = 24

local LibCore = LibStub:NewLibrary(MAJOR, MINOR)
if not LibCore then return end
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0", true)
assert(LibTimer, MAJOR .. " requires LibScriptableUtilsTimer-1.0")
local LibError = LibStub("LibScriptableUtilsError-1.0", true)
assert(LibError, MAJOR .. " requires LibScriptableUtilsError-1.0")
local LibCFG = LibStub("LibScriptableUtilsCFG-1.0", true)
assert(LibCFG, MAJOR .. " requires LibScriptableUtilsCFG-1.0")


local WidgetText = LibStub("LibScriptableWidgetText-1.0", true)
assert(WidgetText, MAJOR .. " requires LibScriptableWidgetText-1.0")
--local WidgetBar = LibStub("LibScriptableWidgetBar-1.0", true)
--assert(WidgetBar, MAJOR .. " requires LibScriptableWidgetBar-1.0")
local WidgetIcon = LibStub("LibScriptableWidgetIcon-1.0", true)
assert(WidgetIcon, MAJOR .. " requires LibScriptableWidgetIcon-1.0")
--[[
local WidgetHistogram = LibStub("LibScriptableWidgetHistogram-1.0", true)
assert(WidgetHistogram, MAJOR .. " requires LibScriptableWidgetHistogram-1.0")
local WidgetBignums = LibStub("LibScriptableWidgetBignums-1.0", true)
--assert(WidgetBignums, MAJOR .. " requires LibScriptableWidgetBignums-1.0")
local WidgetKey = LibStub("LibScriptableWidgetKey-1.0", true)
assert(WidgetKey, MAJOR .. " requires LibScriptableWidgetKey-1.0")
local WidgetTimer = LibStub("LibScriptableWidgetTimer-1.0", true)
assert(WidgetTimer, MAJOR .. " requires LibScriptableWidgetTimer-1.0")
local WidgetGestures = LibStub("LibScriptableWidgetGestures-1.0", true)
assert(WidgetGestures, MAJOR .. "LibScriptableWidgetGestures-1.0")
local WidgetImage = LibStub("LibScriptableWidgetImage-1.0", true)
assert(WidgetImage, MAJOR .. "LibScriptableWidgetImage-1.0")
]]

--[[
local Evaluator = LibStub("LibScriptableUtilsEvaluator-1.0", true)
assert(Evaluator, MAJOR .. " requires LibScriptableUtilsEvaluator-1.0")
]]

local PluginUnit = LibStub("LibScriptablePluginUnit-1.0", true)
assert(PluginUnit, MAJOR .. " requires LibScriptablePluginUnit-1.0")
local PluginBit = LibStub("LibScriptablePluginBit-1.0", true)
assert(PluginBit, MAJOR .. " requires LibScriptablePluginBit-1.0")
local PluginLua = LibStub("LibScriptablePluginLua-1.0", true)
assert(PluginLua, MAJOR .. " requires LibScriptablePluginLua-1.0")
local PluginMath = LibStub("LibScriptablePluginMath-1.0", true)
assert(PluginMath, MAJOR .. " requires LibScriptablePluginMath-1.0")
local PluginString = LibStub("LibScriptablePluginString-1.0", true)
assert(PluginString, MAJOR .. " requires LibScriptablePluginString-1.0")
local PluginTable = LibStub("LibScriptablePluginTable-1.0", true)
assert(PluginTable, MAJOR .. " requires LibScriptablePluginTable-1.0")
local PluginColor = LibStub("LibScriptablePluginColor-1.0", true)
assert(PluginColor, MAJOR .. " requires LibScriptablePluginColor-1.0")
--local PluginLuaTexts = LibStub("LibScriptablePluginLuaTexts-1.0", true)
--assert(PluginLuaTexts, MAJOR .. " requires LibScriptablePluginLuaTexts-1.0")
--local PluginUtils = LibStub("LibScriptablePluginUtils-1.0", true)
--assert(PluginUtils, MAJOR .. " requires LibScriptablePluginUtils-1.0")
local PluginBlend = LibStub("LibScriptablePluginBlend-1.0")
assert(PluginBlend, MAJOR .. " requires LibScriptablePluginBlend-1.0")

local pool = setmetatable({}, {__mode = "k"})
local objects = {}
local objectDicts = {}

if not LibCore.__index then
	LibCore.__index = LibCore
end

LibCore.TRANSITION_RIGHT = 0
LibCore.TRANSITION_LEFT = 1
LibCore.TRANSITION_BOTH = 2
LibCore.TRANSITION_UP = 3
LibCore.TRANSITION_DOWN = 4
LibCore.TRANSITION_TENTACLE = 5
LibCore.TRANSITION_ALPHABLEND = 6
LibCore.TRANSITION_CHECKERBOARD = 7

local defaults = {rows = 4, cols = 20, transition_timeout = 0, transition_speed = 100, timeout = 100}

LibCore.defaults = defaults

-- @name LibScriptableCore:New
-- @description Note that you must provide the object an lcd object: core.lcd = myTextLCD
-- @usage LibScriptableCore:New(visitor, environment, name, config, typeOf, lcd, errorLevel)
-- @parma visitor App table
-- @return A new LibScriptableCore object
function LibCore:New(visitor, environment, name, config, typeOf, errorLevel)
	
	local obj = next(pool)

	if obj then
		pool[obj] = nil
	else
		obj = {}
	end

	if type(config) == "string" then
		config = Evaluator.ExecuteCode(environment, "LibCore:New", config)
	end

	setmetatable(obj, self)
	
	table.insert(objects, obj)
	
	assert(type(name) == "string" and config[name], name .. " not found in config.")

	obj.widget_templates = {}
	obj.widgets = {}
	obj.static_widgets = {}
	obj.layouts = {}

	obj.visitor = visitor
	obj.environment = environment
	obj.typeOf = typeOf
	obj.name = name
	obj.config = config
	obj.transition_timeout = config[name].transition_timeout or defaults.transition_timeout
	obj.transition_speed = config[name].transition_speed or defaults.transition_speed
	obj.is_transitioning = true
	obj.timer = LibTimer:New("LibCore.timer.timer " .. name, obj.transition_timeout, true, obj.ChangeLayout, obj, errorLevel)
	obj.transitionTimer = LibTimer:New("LibCore.transitionTimer " .. name, obj.transition_speed, true, obj.LayoutTransition, obj)
	obj.CFG = LibCFG:New(obj, config, environment, errorLevel)
	obj.error = LibError:New(MAJOR, errorLevel)
	obj.errorLevel = errorLevel
	
	environment.widgets = obj.widgets
	environment.visitor = obj
	environment._G = _G
	environment.environment = environment
	environment.null = {}
	
	-- Plugins
	PluginUnit:New(environment)
	PluginBit:New(environment)
	PluginLua:New(environment)
	PluginMath:New(environment)
	PluginString:New(environment)
	PluginTable:New(environment)
	PluginColor:New(environment)
	--PluginLuaTexts:New(environment)
	--PluginUtils:New(environment)
	PluginBlend:New(environment)
	

	environment.WidgetText = WidgetText
	environment.WidgetBar = WidgetBar
	environment.WidgetIcon = WidgetIcon
	environment.WidgetHistogram = WidgetHistogram
	environment.WidgetBignums = WidgetBignums
	environment.WidgetKey = WidgetKey
	environment.WidgetTimer = WidgetTimer
	environment.WidgetScript = WidgetScript
	environment.WidgetGestures = WidgetGestures
	environment.WidgetImage = WidgetImage
				
	return obj	
	
end

-- @name LibScriptableCore:Del
-- @usage LibScriptableCore:Del([core]) or object:Del()
-- @param core An optional core object
-- @return Nothing
function LibCore:Del(core)
	if not core then core = self end
	
	for i, v in ipairs(objects) do
		if v == self then
			table.remove(objects, i)
		end
	end
	
	pool[core] = true
	
	for i, v in ipairs(core.widgets) do
		v:Del()
	end
	
	table.wipe(core.widgets)
	table.wipe(core.widget_templates)
	table.wipe(core.layouts)
	core.currentIndex = nil
	
	core.error:Del()
	core.timer:Del()
	core.transitionTimer:Del()
end

function LibCore:Start()
	self.error:Print("Core Start : " .. self.name, 2)
	self.timer:Start()
	self:StartLayout()
end

function LibCore:Stop()
	self:StopLayout()
	self.timer:Stop()
end

function LibCore:AddWidget(widg)
	self.widgest[widget.name] = widg
end

function LibCore:CFGSetup()
	local i = 1
	local str = ""
	

	--app_->ProcessVariables(CFG_Get_Root(), (Evaluator *)this);

	local section = self.CFG:FetchRaw(self.CFG.root, self.name);
	if( not section) then
		self.error:Print(("Display <%s> doesn't exist in configuration."):format( self.name))
		return
	end

	self.transition_timeout = self.CFG:Fetch(section, "transition_timeout", 10000);

	self.transition_speed = self.CFG:Fetch(section, "transition_speed", 200);

		
	local i = 1
	
	local layouts = self.CFG:FetchRaw(section, "layouts")
	
	if layouts then
		local layout = self.CFG:FetchRaw(layouts, i);
		while layout do
			self.widget_templates[layout] = {}
			table.insert(self.layouts, layout)
			i = i + 1
			layout = self.CFG:FetchRaw(layouts, i)		
		end
	end
	
	local widgets = self.CFG:FetchRaw(section, "widgets");
	
	if widgets then
		for i, w in ipairs(widgets) do
			table.insert(self.static_widgets, w)
		end
	end
	
	local i = 1
	while i <= #self.layouts do
		layout = self.CFG:FetchRaw(self.CFG.root, self.layouts[i]);
		if not layout then
			self.error:Print(("Missing layout <%s>"):format( self.layouts[i]));
		end
		if(layout) then 
			--Json::Value *val = CFG_Fetch(layout, "keyless", new Json::Value(0));
			--if(val->asInt()) {
			--	keyless_layouts_[layouts_[i]] = true;
			--}
			local layer = 0
			while(layer <= self.lcd.LAYERS) do
				local cfg_layer = self.CFG:FetchRaw(layout, layer + 1);
				if(cfg_layer) then
					local row = 0
					while row <= self.lcd.LROWS do
						local cfg_row = self.CFG:FetchRaw(cfg_layer, row + 1)
						if cfg_row then
							local col = 0
							while(col <= self.lcd.LCOLS) do
								local cfg_col = self.CFG:FetchRaw(cfg_row, col + 1);

								if(cfg_col and type(cfg_col) == "string") then
									local w = {}
									w.key = cfg_col
									w.row = row
									w.col = col
									w.layer = layer;
									table.insert(self.widget_templates[self.layouts[i]], w)
								end
								col = col + 1
							end
						end
						row = row + 1
					end
				end
				layer = layer + 1
			end

			local row = 0
			while row < self.lcd.LROWS do
				local cfg_row = self.CFG:FetchRaw(layout, row + 1);
				if(cfg_row) then
					local col = 0
					while col < self.lcd.LCOLS do
						local cfg_col = self.CFG:FetchRaw(cfg_row, col + 1);

						if(cfg_col and type(cfg_col) == "string") then
							local w = {}
							w.key = cfg_col
							w.row = row;
							w.col = col;
							w.layer = 0;
							table.insert(self.widget_templates[self.layouts[i]], w)
						end
						col = col + 1
					end
				end
				row = row + 1
			end
		end
		i = i + 1
	end
	self.widget_templates[self.name] = {}
	for j, w in ipairs(self.static_widgets) do
		local ww = {}
		ww.key = w;
		ww.row = 0;
		ww.col = 0;
		table.insert(self.widget_templates[self.name], ww)
	end
	
end


function LibCore:BuildLayouts()
	local name
	for k, widgets in pairs(self.widget_templates) do
		for i = 1, #widgets do
			
			local widget_v = self.CFG:FetchRaw(self.CFG.root, widgets[i].key)

			if(not widget_v) then
				self.error:Print(("No widget named <%s>"):format(widgets[i].key))
			else
				local typeOf = self.CFG:FetchRaw(widget_v, "type");
				
				if(not typeOf) then
					self.error:Print(("Widget <%s> has no type!"):format( widgets[i].key))
				else
					local widget
					
					local name;
					local j = 0;
					name = k .. ":" .. widgets[i].key .. ":" .. j

					for k, v in pairs(self.widgets) do
						if v.name == name then
							j = j + 1
							name = k .. ":" .. widgets[i].key .. ":" .. j
						end
					end
						 
					if(typeOf == "text") then
						widget = WidgetText:New(self, name, widget_v, widgets[i].row, widgets[i].col, widgets[i].layer, self.errorLevel, self.lcd.TextDraw)
					elseif (typeOf == "bar") then
						--widget = WidgetBar:New(self, name, widget_v, widgets[i].row, widgets[i].col, widgets[i].layer, self.erroLevel, self.lcd.BarDraw)
					elseif (typeOf == "icon") then
						widget = WidgetIcon:New(self, name, widget_v, widgets[i].row, widgets[i].col, widgets[i].layer, self.errorLevel, self.lcd.IconDraw)
					elseif (typeOf == "histogram") then
						--widget = WidgetHistogram:New(self, name, widget_v, widgets[i].row, widgets[i].col, widgets[i].layer, self.errorLevel, self.lcd.HistogramDraw)
					elseif (typeOf == "bignums") then
						--widget = WidgetBignums:New(self, name, widget_v, widgets[i].row, widgets[i].col, widgets[i].layer, self.errorLevel, self.lcd.BignumsDraw)
					elseif (typeOf == "key") then
						--widget = WidgetKey:New(self, name, widget_v)
					elseif (typeOf == "timer") then
						--widget = WidgetTimer:New(self, name, widget_v, self.errorLevel) 
					elseif (typeOf == "script") then
						--widget = WidgetScript:New(self, name, widget_v)
					else
						 self.error("Unknown widget typeOf: %s", type(typeOf))
					end
					if(widget) then
						self.widgets[name] = widget;
					end
					
				end
			end
		end
	end
end

function LibCore:GetNextLayout(i)
	if i then
		self.currentIndex = i
	else
		self.currentIndex = (self.currentIndex or 0) + 1
	end
	if self.currentIndex > #self.layouts then
		self.currentIndex = 1
	end
	return self.layouts[self.currentIndex]
end
		
function LibCore:StartLayout(key)
    if type(key) ~= "string" then
		self.currentLayout = self:GetNextLayout()
    else
        self.currentLayout = key;
	end

	if not self.currentLayout then
		self.error:Print("No current layout", 2)
		return
	end
	
	self.layoutsDict = {}
	
	for i, v in ipairs(self.layouts) do
		self.layoutsDict[self.currentLayout] = v
	end

    --self.error:Print("StartLayout: " .. self.currentLayout)
	
    --emit static_cast<LCDEvents *>(wrapper_)->_LayoutChangeBefore();
	
	--self.lcd:Clear()
	
	local widgets = self.widgets
	for k, w in pairs(widgets) do
		
		
        if(self.currentLayout == w.widget.layout_base) then -- and w.widget.name ~= self.name) ) then

			if w.SetupChars then
				w:SetupChars();
			end
			w:Start();
		end
    end

	
	for k, w in pairs(widgets) do
		if w.type["timer"] or w.type["key"] then
			w:Start()
		end
    end
	
    --emit static_cast<LCDEvents *>(wrapper_)->_LayoutChangeAfter();

	local timeout = self.CFG:Fetch(self.visitor, self.currentLayout .. ".transition_timeout") or self.transition_timeout

    if(timeout > 0) then
        self.timer:Start(timeout);
	end


    self.is_transitioning = false;
end

function LibCore:StopLayout(layout) 
	if not layout then
		layout = self.currentLayout
	end
    for k, w in pairs(self.widgets) do
        if(layout == w.widget.layout_base ) then
			w:Stop();
		end
    end
end

function LibCore:ChangeLayout() 
	local t = self.CFG:FetchRaw(self.CFG.root, self.currentLayout .. ".transition")
    if(not t) then
        self:StopLayout(self.currentLayout);
        self:StartLayout();
    else 
        self:StartTransition(t);
	end
    
end

function LibCore:Transition(i) 
    if(self.is_transitioning) then
        return;
	end
	self.currentIndex = i
    self:ChangeLayout();
end

function LibCore:StartTransition(direction)
    if(not direction) then
        self.error:Print(("Transition not defined <%s>"):format( self.currentLayout));
        self:StartLayout();
        return;
    end
    self.is_transitioning = true;
	self.lastLayout = self.currentLayout
	self.currentLayout = self:GetNextLayout()
    self.lcd:SignalTransitionStart(self.currentLayout);
    self:StartLayout(self.currentLayout);
    self.direction = direction;
	self.transitionTimer:Start()
    self:LayoutTransition();
    self.error:Print(("Transition started -- speed: %d"):format(self.transition_speed));
	self.timer:Stop()
end

function LibCore:LayoutTransition()
	self.lcd:Transition()
end

function LibCore:TransitionFinished()
    self.transitionTimer:Stop();
	self.timer:Start()
    self:StopLayout(self.lastLayout);
    self.is_transitioning = false;
    self.lcd:SignalTransitionEnd();
end

--[[std::string LibCore::CFG_Key() {
    return key_;
}]]

--[[
function LibCore::KeypadEvent(key) 
    LCDError(("KeypadEvent(%d) %s"):format(key, currentLayout_.c_str()));

    std::map<std::string, bool>::iterator it = 
        keyless_layouts_.find(currentLayout_);

    if(it != keyless_layouts_.end())
        return;

    for(std::map<std::string, Widget *>::iterator w = widgets_.begin();
        w != widgets_.end(); w++) {
        if( w->second->GetType() & WIDGET_TYPE_KEYPAD )
            ((WidgetKey*)w->second)->KeyPressed(key);
    }
}
]]

function LibCore:ResizeLCD(rows, cols)
    self:StopLayout(self.currentLayout);
    local old_rows = lcd.LROWS;
    local old_cols = lcd.LCOLS;
    if(self:ResizeLCD(rows, cols) == 0) then
        --emit static_cast<LCDEvents *>(wrapper_)->_ResizeLCD(rows, cols, old_rows, old_cols);
    else
        LCDError("LibCore::ResizeLCD: Unable to resize LCD");
        return -1;
    end
    self:StartLayout(self.currentLayout);
    return 0;
end

function LibCore:SelectLayout(layout) 
    if(self.is_transitioning) then
        return;
	end
	local i = 0
	while i < #self.layouts do
		if self.layouts[i] == layout then
			self:StopLayout(self.currentLayout)
			self:StartLayout(layout)
		end
	end
end

function LibCore:RemoveWidget(name) 
    if(type(self.widgets[name]) ~= "nil") then
		self.widgets[name]:Del()
		self.widgets:remove(self.widgets[name])
        return 0;
    end
    return -1;
end

function LibCore:AddWidget(name, layout, row, col, layer, object) 
	local name = layout .. ":" .. name
	
    local i = 0;
    while(widgets[name]) do
		name = layout .. ":" .. name .. ":" .. i
		i = i + 1
    end
    
    local type = self.CFG:FetchRaw(object, "type");

    if(type) then
           local widget
           if(type == "text") then
               widget = WidgetText:New(self, name, root, row, col, layer);
           elseif (type == "bar") then
               widget = WidgetBar:New(self, name, root,row, col, layer);
           elseif (type == "icon") then
               widget = WidgetIcon:New(self, name, root, row, col, layer);
           elseif (type == "histogram") then
               widget = WidgetHistogram:New(self, name, root, row, col, layer);
           elseif (type == "bignums") then
               widget = WidgetBignums:New(self, name, root, row, col, layer);
           elseif (type == "key") then
               widget = WidgetKey:New(self, name, root);
           elseif (type == "timer") then
               widget = WidgetTimer:New(self, name, root);
           elseif (type == "script") then
               widget = WidgetScript:New(self, name, root);
           else
               LCDError("Unknown widget type: %s", type);
           end
           if(widget) then
               self.widgets[name] = widget;
               widget:Start();
           end
           return name;
    else
        LCDError(("Widget has no type <%s>"):format(object.c_str()));
    end
    return "";
end

--[[
int LibCore::MoveWidget(std::string widget, int row, int col) {
    return 0;
}
]]

function KeyEvent(_, modifier, up)
	for _, core in pairs(objects) do
		for k, v in pairs(core.widgets) do
			if v.type["key"] then
				v:KeyEvent(modifier, up)
			end
		end
	end
end

function LibCore:RebuildOpts(visitor, db)
	local options = {
--[[		name = {
			name = "Name",
			desc = "This display's name",
			type = "input",
			get = function() return db.name end,
			set = function(info, v)
				db.name = v
			end,
			order = 10
		},]]
		rows = {
			name = "Rows",
			desc = "This display's rows",
			type = "input",
			pattern = "%d",
			get = function() return tostring(db.rows or defaults.rows) end,
			set = function(info, v) db.rows = tonumber(v) end,
			order = 12
		},
		cols = {
			name = "Columns",
			desc = "This display's columns",
			type = "input",
			pattern = "%d",
			get = function() return tostring(db.cols or defaults.cols) end,
			set = function(info, v) db.cols = tonumber(v) end,
			order = 13
		},
		update = {
			name = "Update",
			desc = "This display's refresh rate",
			type = "input",
			pattern = "%d",
			get = function() return tostring(db.update or defaults.update) end,
			set = function(info, v) db.update = tonumber(v) end,
			order = 14
		},
		transition_timeout = {
			name = "Transition Timeout",
			desc = "This display's transition timeout",
			type = "input",
			pattern = "%d",
			get = function() return tostring(db.transition_timeout or defaults.transition_timeout) end,
			set = function(info, v) db.transition_timeout = tonumber(v) end,
			order = 16
		},
		transition_speed = {
			name = "Transition Speed",
			desc = "This display's transition speed",
			type = "input",
			pattern = "%d",
			get = function() return tostring(db.transition_speed or defaults.transition_speed) end,
			set = function(info, v) db.transition_speed = tonumber(v) end,
			order = 17
		},
		widgets = {
			name = "Widgets",
			type = "group",
			args = {
				add = {
					name = "Add a widget",
					desc = "Add a widget",
					type = "input",
					set = function(info, v)
						table.insert(db.widgets, "widget_" .. v)
						visitor:RebuildOpts(true)
					end,
					order = 1
				}
			},
			order = 18,
		},
		layouts = {
			name = "Layouts",
			type = "group",
			args = {
				add = {
					name = "Add a layout",
					desc = "Add a layout",
					type = "input",
					set = function(info, v)
						table.insert(db.layouts, "layout_" .. v)
						visitor:RebuildOpts(true)
					end,
					order = 1
				},
			},
			order = 19
		}
	}
	
	for i, v in ipairs(db.widgets or {}) do
		if v.type == "text" then
			options.widgets.args["widget" .. i] = {
				name = "Widget " .. i,
				type = "group",
				args = {
					name = {
						name = v,
						type = "input",
						get = function() return v end,
						set = function(info, v) 
							db.widgets[i] = v 
							visitor:RebuildOpts(true)
						end,
						order = 1
					},
					remove = {
						name = "Remove",
						type = "execute",
						func = function()
							table.remove(db.widgets, i)
							visitor:RebuildOpts(true)
						end,
						order = 2
					}
				}
			}
		end
	end
	
	for i, v in ipairs(db.layouts) do
		options.layouts.args["layout" .. i] = {
			name = "Layout " .. i,
			type = "group",
			args = {
				name = {
					name = v,
					type = "input",
					get = function() return v end,
					set = function(info, v) 
						db.layouts[i] = v 
					end,
					order = 1
				},
				up = {
					name = "Up",
					desc = "Move this layout up in the list",
					type = "execute",
					func = function()
						if i == 1 then return end
						db.layouts[i] = db.layouts[i - 1]
						db.layouts[i - 1] = v
						visitor:RebuildOpts(true)
					end,
					order = 2
				},
				down = {
					name = "Down",
					desc = "Move this layout down in the list",
					type = "execute",
					func = function()
						if i == #db.layouts then return end
						db.layouts[i] = db.layouts[i + 1]
						db.layouts[i + 1] = v
						visitor:RebuildOpts(true)
					end,
					order = 3
				},
				remove = {
					name = "Remove",
					desc = "Remove this layout from the display's list",
					type = "execute",
					func = function()
						table.remove(db.layouts, i)
						visitor:RebuildOpts()
					end,
					order = 4
				}
			}
		}
	end
	return options
end


