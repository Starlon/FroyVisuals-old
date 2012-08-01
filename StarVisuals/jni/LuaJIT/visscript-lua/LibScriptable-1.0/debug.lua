local debug = false
--[===[@debug@
debug = true
--@end-debug@]===]

local LibDriverCharacter = LibStub("LibScriptableLCDDriverCharacter-1.0")
local LibBuffer = LibStub("LibScriptableUtilsBuffer-1.0")
local AVSSuperScope = LibStub("LibScriptableImagesAVSSuperScope-1.0")
local PluginColor = LibStub("LibScriptablePluginColor-1.0")
local LibCore = LibStub("LibScriptableLCDCoreLite-1.0")
local LibTimer = LibStub("LibScriptableUtilsTimer-1.0")

local environment = {}
local core = LibCore:New(environment, "StarVisuals.AVS", errorLevel)
local context = UI.CreateContext("StarDebug")

table.insert(Command.Slash.Register("test"), {function (params)
	print(environment.cos(environment.PI))
end, "LibScriptable_1_0", "Test slash"})

table.insert(Command.Slash.Register("lcd4rift"), {function (commandLineParameters)
	--local display = LibDriverCharacter:New(environment, environment, "display_character", _G.LCD4Rift.config, 2)
	--display:Show()
end, "LibScriptable_1_0", "LCD4Rift slash"})

local avstimer
local images = {}
local presets
local visdata = LibBuffer:New("Superscope visdata", 576, 0)
local copy
copy = function(tbl)
	if type(tbl) ~= "table" then return tbl end
	local newtbl = {}
	for k, v in pairs(tbl) do
		newtbl[k] = copy(v)
	end
	return newtbl
end

local function createImages()
	
	for i, imagedb in ipairs(presets) do
		if imagedb.enabled then
			local image = AVSSuperScope:New(imagedb.name or "avs", copy(imagedb), draw)
			image.framebuffer = LibBuffer:New("framebuffer", image.width * image.height)
			local frame = UI.CreateFrame("Frame", "Image", context)
			frame:SetWidth(image.width * image.pixel)
			frame:SetHeight(image.height * image.pixel)
			local point = imagedb.points[1]
			frame:SetPoint(point[1], UIParent, point[3], point[4] or 0, point[5] or 0)
			frame:SetVisible(true)
			image.textures = {}
			for row = 0, image.height - 1 do
				for col = 0, image.width - 1 do
					local n = row * image.width + col
					image.textures[n] = UI.CreateFrame("Frame", "Texture" .. n, frame)
					image.textures[n]:SetHeight(image.pixel)
					image.textures[n]:SetWidth(image.pixel)
					image.textures[n]:SetPoint("TOPLEFT", frame, "TOPLEFT", col * image.pixel, row * image.pixel)
					image.textures[n]:SetVisible(true)
					image.textures[n]:SetBackgroundColor(math.random(), math.random(), math.random())
				end
			end
			image.canvas = frame
			images[imagedb] = image
		end
	end
end

local visdata = {}
local function update()
	for i, widget in pairs(images) do
		widget.buffer:Clear()
		local fbout = {}
		local total = 0
		widget.framebuffer = widget.framebuffer or LibBuffer:New("framebuffer", widget.width * widget.height)
		widget:Render(visdata, isBeat, widget.framebuffer, fbout, widget.width, widget.height)
		for row = 0, widget.height - 1 do
			for col = 0, widget.width - 1 do
				local n = row * widget.width + col
				local color = widget.buffer.buffer[n]
				local r, g, b = PluginColor.Color2RGBA(color)
				widget.textures[n]:SetBackgroundColor(r, g, b)
			end
		end
		
	end
end

local avstimer = LibTimer:New("StarVisuals.AVS", 90, true, update)

local toggle = false
table.insert(Command.Slash.Register("visuals"), {function (commandLineParameters)
	if not toggle then
		createImages()
		avstimer:Start()
	else
		avstimer:Stop()
	end
	toggle = not toggle
end, "LibScriptable_1_0", "Visuals slash"})

presets = {
	[1] = {
				name = "Spiral",
				init = [[
n=32
]],
				frame = [[
t=t-5
]],
				beat = [[
]],
				point = [[
d=i+v*0.2; 
r=t+i*PI*200; 
x=cos(r)*d*.8; 
y=sin(r)*d*.8
]],
				width = 92,
				height = 92,
				pixel = 2,
				drawLayer = "UIParent",
				points = {{"CENTER", "UIParent", "CENTER", 0, 255}},
				enabled = true,
				drawMode = 1,
				unit = "local"
				--next = 2
	},
	[2] = {
				name = "3d Fearn",
				init = [[
n=1000; zs=sqrt(2); izs=50.893/5; iys=10.385/5
zt = 100; yt = 200.5; xt = rand(100)

]],
				frame = [[
rx1=0; ry1=0; rz1=0; rx2=0; ry2=0; zt=zt+izt; yt=yt+iyt; xt=xt+ixt; cz=cos(zt); sz=sin(zt); cy=cos(yt); sy=sin(yt); cx=cos(xt); sx=sin(xt);
red = 15 / 255; green = 63 / 255; blue = 31 / 255
xt = xt + 20
]],
				beat = [[
local val = 2.5
izt=rand(100)/10-val; iyt=rand(100)/10-val; ixt=rand(100)/10-val;
]],
				point = [[
local random=rand(100);
t1=if2(equal(random,0),0,t1);
t1=if2(below(random,86)*above(random,0),1,t1);
t1=if2(below(random,93)*above(random,86),2,t1);
t1=if2(below(random,99)*above(random,92),3,t1);
rx2=rx1; ry2=ry1;
rx1=if2(equal(t1,0),0,rx1);
ry1=if2(equal(t1,0),ry1*.18,ry1);
rz1=if2(equal(t1,0),0,rz1);
rx2=rx1; ry2=ry1;
rx1=if2(equal(t1,1),rx1*.85,rx1);
ry1=if2(equal(t1,1),ry1*.85+rz1*0.1+1.6,ry1);
rz1=if2(equal(t1,1),ry2*-.1+rz1*0.85,rz1);
rx2=rx1; ry2=ry1;
rx1=if2(equal(t1,2),rx1*0.2+ry1*-0.2,rx1);
ry1=if2(equal(t1,2),rx2*0.2+ry1*0.2+0.8,ry1);
rz1=if2(equal(t1,2),rz1*0.3,rz1);
rx2=rx1; ry2=ry1;
rx1=if2(equal(t1,3),rx1*-0.2+ry1*0.2,rx1);
ry1=if2(equal(t1,3),rx2*0.2+ry1*0.2+0.8,ry1);
rz1=if2(equal(t1,3),rz1*0.3,rz1);
x1=rx1; y1=ry1; z1=rz1;
x2=x1*cz+y1*sz; y2=x1*sz-y1*cz;
x3=x2*cy+z1*sy; z2=x2*sy-z1*cy;
y3=y2*cx+z2*sx; z3=y2*sx-z2*cx+10;
x=x3/z3; y=y3/z3;
x = x / 1.5; y = y / 1.5;
]],
				width = 94,
				height = 94,
				pixel = 2,
				drawLayer = "UIParent",
				points = {{"CENTER", "UIParent", "CENTER", 0, 0}},
				enabled = false,
				drawMode = 0,
				line_blend_mode=2
				--next = 2
			},

}
