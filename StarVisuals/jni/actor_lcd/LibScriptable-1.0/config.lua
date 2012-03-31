local addon, ns = ...

local TRANSITION_RIGHT = 0
local TRANSITION_LEFT = 1
local TRANSITION_BOTH = 2
local TRANSITION_UP = 3
local TRANSITION_DOWN = 4
local TRANSITION_TENTACLE = 5
local TRANSITION_ALPHABLEND = 6
local TRANSITION_CHECKERBOARD = 7

local ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT, ALIGN_MARQUEE, ALIGN_AUTOMATIC, ALIGN_PINGPONG = 1, 2, 3, 4, 5, 6
local SCROLL_RIGHT, SCROLL_LEFT = 1, 2

local stratas = {
	"BACKGROUND",
	"LOW",
	"MEDIUM",
	"HIGH",
	"DIALOG",
	"FULLSCREEN",
	"FULLSCREEN_DIALOG",
	"TOOLTIP"
}
	
local anchors = {
	"TOP",
	"TOPRIGHT",
	"TOPLEFT",
	"BOTTOM",
	"BOTTOMRIGHT",
	"BOTTOMLEFT",
	"RIGHT",
	"LEFT",
	"CENTER"
}

local foo = 100

_G.LCD4Rift = {}
_G.LCD4Rift.config = {
    ["display_startip"] = {
		["addon"] = "LCD4Rift",
		["enabled"] = false,
		["driver"] = "qtip",
		["layers"] = 2,
		["background"] = "d9ccf16f",
        ["row"] = 0,
        ["col"] = 0,
		["rows"] = 64,
		["cols"] = 256,
		["update"] = 0,
		["timeout"] = 2000,
		["transition_speed"] = 50,
		["widgets"] = {},
		["layouts"] = {"layout_LCD4Rift"},
		["font"] = {normal="Interface\\AddOns\\LCD4Rift\\Fonts\\ttf-bitstream-vera-1.10\\VeraMo.ttf", bold="Interface\\AddOns\\LCD4Rift\\Fonts\\ttf-bitstream-vera-1.10\\VeraMoBd.ttf", size=12},
		["points"] = {{"CENTER", "UIParent", "TOP", 0, -100}}
    },
    ["display_character"] = {
		["addon"] = "LCD4Rift",
		["enabled"] = true,
		["driver"] = "character",
		["layers"] = 1,
		["background"] = "d9ccf16f",
		["pixel"] = 2,
		["row"] = -50,
		["col"] = 0,
		["rows"] = 2,
		["cols"] = 20,
		["update"] = 0,
		--["widgets"] = {"widget_key_up", "widget_key_down", "widget_resources_timer"},
		["layouts"] = {"layout_LCD4Rift"},
		["points"] = {{"CENTER", "UIParent", "CENTER"}},
		["parent"] = "UIParent",
		["strata"] = 1
    },
	["display_icon"] = {
		["addon"] = "LCD4Rift",
		["enabled"] = false,
		["driver"] = "qtip",
		["layers"] = 1,
		["row"] = 0,
		["col"] = 500,
		["rows"] = 8,
		["cols"] = 6 * 3,
		["layouts"] = {"layout_icon"},
		["widgets"] = {},
		["font"] = {normal="Interface\\AddOns\\LCD4Rift\\Fonts\\ttf-bitstream-vera-1.10\\VeraMo.ttf", size=1},
		["points"] = {{"CENTER", "UIParent", "CENTER", 400, 0}},
		["parent"] = "UIParent"
	},
	["display_health"] = {
		["addon"] = "LCD4Rift",
		["enabled"] = false,
		["driver"] = "character",
		["pixel"] = 1,
		["layers"] = 1,
		["rows"] = 1,
		["cols"] = 12,
		["layouts"] = {"layout_health"},
		["widgets"] = {},
		["points"] = {{"CENTER", "UIParent", "CENTER", 0, -100}},
		["parent"] = "UIParent",
		["strata"] = #stratas,
	},
	["display_mana"] = {
		["addon"] = "LCD4Rift",
		["enabled"] = false,
		["driver"] = "character",
		["pixel"] = 1,
		["layers"] = 1,
		["rows"] = 1,
		["cols"] = 12,
		["layouts"] = {"layout_mana"},
		["widgets"] = {},
		["points"] = {{"TOPRIGHT", "GameTooltip", "BOTTOMRIGHT", 0, -130}},
		["parent"] = "GameTooltip",
		["strata"] = #stratas
	},
	["widget_resources_timer"] = {
        type = "timer",
		update = 1000,
		repeating = false,
		expression = [[
Update()
]]
	},
	["layout_health"] = {
		[1] = {
			[1] = {
				[1] = "widget_text_health",
			}
		}
	},
	["layout_mana"] = {
		[1] = {
			[1] = {
				[1] = "widget_text_mana",
			}
		}
	},
	["layout_icon"] = {
		[1] = {
			[1] = {
				[1] = "widget_icon_blob",
				[7] = "widget_icon_ekg",
				[13] = "widget_icon_timer",
			}
		}
	},
	["layout_tiny"] = {
		[1] = {
			[1] = {
				[1] = "widget_name"
			},
			[2] = {
				[1] = "widget_icon_blob",
				[2] = "widget_icon_ekg",
				[3] = "widget_icon_timer",
				[4] = "widget_icon_heartbeat",
				[5] = "widget_icon_karo",
				[6] = "widget_icon_rain",
				[7] = "widget_icon_squirrel",
				[8] = "widget_icon_heart",
				[9] = "widget_icon_wave",
				[10] = "widget_icon_blob",
				[11] = "widget_icon_ekg",
				[12] = "widget_icon_timer",
				[13] = "widget_icon_heartbeat",
				[14] = "widget_icon_karo",
				[15] = "widget_icon_rain",
				[16] = "widget_icon_squirrel",
				[17] = "widget_icon_heart",
				[18] = "widget_icon_wave",				
				[19] = "widget_icon_blob",
				[20] = "widget_icon_ekg",
			}
		}
	},
	["layout_blank"] = {
		["keyless"] = 1,
		["layout-timeout"] = 0
    },
	["layout_LCD4Rift"] = {
		[1] = { -- layer, defined in driver description.
			[1] = { -- row 
				[1] = "return 'Name:'", -- column
				[10] = "widget_name"
			},
            [2] = {
                [1] = "return 'Mana:'",
                [10] = "widget_mana",
            },
            [3] = {
                [1] = "return 'Health:'",
                [10 = "widget_health"
            },
			[4] = {
				[1] = "widget_icon_blob",
				[2] = "widget_icon_ekg",
				[3] = "widget_icon_timer",
				[4] = "widget_icon_heartbeat",
				[5] = "widget_icon_karo",
				[6] = "widget_icon_rain",
				[7] = "widget_icon_squirrel",
				[8] = "widget_icon_heart",
				[9] = "widget_icon_wave",
				[10] = "widget_icon_blob",
				[11] = "widget_icon_ekg",
				[12] = "widget_icon_timer",
				[13] = "widget_icon_heartbeat",
				[14] = "widget_icon_karo",
				[15] = "widget_icon_rain",
				[16] = "widget_icon_squirrel",
				[17] = "widget_icon_heart",
				[18] = "widget_icon_wave",				
				[19] = "widget_icon_blob",
				[20] = "widget_icon_ekg",
			}
		},
		["transition"] = TRANSITION_TENTACLE,
    },
--[[
	["layout_histogram_cpu"] = {
		[2] = {
			[1] = {
				[1] = "widget_cpu_histogram"
			},
		},
		[1] = {
			[3] = {
				[1] = "widget_cpu_perc"
			}
		},
		["transition"] = TRANSITION_BOTH,
		["timeout"] = 2000
	},
	["layout_histogram_mem"] = {
		[2] = {
			[1] = {
				[1] = "widget_mem_histogram"
			},
		},
		[1] = {
			[3] = {
				[1] = "widget_mem_perc"
			}
		},
		["transition"] = TRANSITION_CHECKERBOARD,
		["timeout"] = 2000
	},
--]]
	["widget_text_health"] = {
		type = "text",
		value = 'return "Health: " .. (UnitHealth("player") / UnitHealthMax("player") * 100) .. "%"',
		align = ALIGN_LEFT,
		cols = 12,
		update = 1000,
		dontRtrim = true
	},	
	["widget_text_mana"] = {
		type = "text",
		value = 'return "Power: " .. (UnitMana("player") / UnitManaMax("player") * 100) .. "%"',
		align = ALIGN_RIGHT,
		cols = 12,
		update = 1000,
		dontRtrim = true
	},
	["widget_name_label"] = {
		type = "text",
		value = 'return "Name:"',
		precision = 0xbabe,
		align = ALIGN_RIGHT,
		cols = 9,
		color = "return 0xffffffff"
	},
	["widget_name"] = {
		type = "text",
		value = "return '--' .. UnitName('player') .. '--'",
		cols = 20,
		align = ALIGN_PINGPONG,
		update = 50,
		speed = 50,
		direction = SCROLL_LEFT,
		dontRtrim = true
	},
	["widget_race_label"] = {
		type = "text",
		value = 'return "Race:"',
		cols = 9,
		align = ALIGN_RIGHT
	},
	["widget_race"] = {
		type = "text",
		value = "return UnitRace('player')",
		cols = 10
	},
	["widget_level_label"] = {
		type = "text",
		value = 'return "Level:"',
		cols = 9,
		align = ALIGN_RIGHT,
	},
	["widget_level"] = {
		type = "text",
		value = "return UnitLevel('player')",
		cols = 10
	},
	["widget_mem_label"] = {
		type = "text",
		value = "return 'Memory:'",
		cols = 9,
		align = ALIGN_RIGHT
	},
	["widget_mem"] = {
		type = "text",
		value = [[
mem = GetMemUsage("LCD4Rift")
--do return random(100) .. "%" end
if mem then
    return memshort(tonumber(format("%.2f", mem)))
end
]],
		cols = 10,
		update = 1000,
		dontRtrim = true
	},
	["widget_mem_perc"] = {
		type = "text",
		value = [[
--do return random(100) .. "%" end
mem, percent, memdiff, totalMem, totaldiff = GetMemUsage("LCD4Rift")

if mem then
    if totaldiff == 0 then totaldiff = 1 end
    return '-==MEM::' .. format("%.2f", memdiff / totaldiff * 100) .. "%" .. "::MEM==-"
end
]],
		align = ALIGN_PINGPONG,
		direction = SCROLL_RIGHT,
		cols = 30,
		update = 1000,
		speed = 100,
		dontRtrim = true
	},
	["widget_mem_bar"] = {
		type = "bar",
		expression = [[
--do return random(100) end
mem, percent, memdiff, totalMem, totaldiff = GetMemUsage("LCD4Rift")

if mem then
    if totaldiff == 0 then return 0 end
    return memdiff / totaldiff * 100
end
]],
		min = "return 0",
		max = "return 100",
		length = 10
	},
	["widget_mem_histogram"] = {
		type = "histogram",
		expression = [[
--do return random(100) end
mem, percent, memdiff, totalMem, totaldiff = GetMemUsage("LCD4Rift")

if mem then
    if totaldiff == 0 then totaldiff = 1 end
    return memdiff / totaldiff * 100
end
]],
		min = "return 0",
		max = "return 100",
		reversed = true,
		char = "0",
		width = 30,
		height = 6,
		layer = 1
	},
	["widget_cpu_label"] = {
		type = "text",
		value = "return 'CPU:'",
		cols = 9,
		align = ALIGN_RIGHT
	},
	["widget_cpu"] = {
		type = "text",
		value = [[
--do return timeshort(random(10000)) end
cpu = GetCPUUsage("LCD4Rift")

if cpu then
    return timeshort(cpu)
end
]],
		cols = 10,
		update = 1000,
		dontRtrim = true
	},
	["widget_cpu_bar"] = {
		type = "bar",
		expression = [[
--do return random(100) end
cpu, percent, cpudiff, totalCPU, totaldiff = GetCPUUsage("LCD4Rift")

if cpu then
    if totaldiff == 0 then return 0 end
    return cpudiff / totaldiff * 100
end
]],
		min = "return 0",
		max = "return 100",
		length = 10
	},
	["widget_cpu_histogram"] = {
		type = "histogram",
		expression = [[
if not scriptProfile then return random(100) end
cpu, percent, cpudiff, totalCPU, totaldiff = GetCPUUsage("LCD4Rift")

if cpu then
    if totaldiff == 0 then return 0 end
    return cpudiff / totaldiff * 100
end
]],
		min = "return 0",
		max = "return 100",
		width = 30,
		height = 6
	},
	["widget_cpu_perc"] = {
		type = "text",
		value = [[
--do return format("------%d%%-------", random(100)) end
cpu, percent, cpudiff, totalCPU, totaldiff = GetCPUUsage("LCD4Rift")

if cpu then
    if totaldiff == 0 then totaldiff = 1 end
    return '-==CPU::' .. format("%.2f", cpudiff / totaldiff * 100) .. "%" .. "::CPU==-"
end
]],
		align = ALIGN_PINGPONG,
		direction = SCROLL_RIGHT,
		cols = 30,
		update = 1000,
		speed = 100,
		dontRtrim = true
	},
	["widget_icon_blob"] = {
		["bitmap"] = {
    		["row1"] = ".....|.....|.....",
    		["row2"] = ".....|.....|.***.",
    		["row3"] = ".....|.***.|*...*",
    		["row4"] = "..*..|.*.*.|*...*",
    		["row5"] = ".....|.***.|*...*",
    		["row6"] = ".....|.....|.***.",
    		["row7"] = ".....|.....|.....",
    		["row8"] = ".....|.....|....."
        },
		["speed"] = foo,
		["type"] = "icon"
    },
	["widget_icon_ekg"] = {
		["bitmap"] = {
    		["row1"] = ".....|.....|.....|.....|.....|.....|.....|.....",
    		["row2"] = ".....|....*|...*.|..*..|.*...|*....|.....|.....",
    		["row3"] = ".....|....*|...*.|..*..|.*...|*....|.....|.....",
    		["row4"] = ".....|....*|...**|..**.|.**..|**...|*....|.....",
    		["row5"] = ".....|....*|...**|..**.|.**..|**...|*....|.....",
    		["row6"] = ".....|....*|...*.|..*.*|.*.*.|*.*..|.*...|*....",
    		["row7"] = "*****|*****|****.|***..|**..*|*..**|..***|.****",
    		["row8"] = ".....|.....|.....|.....|.....|.....|.....|....."
        },
		["speed"] = foo,
		["type"] = "icon"
    },
	["widget_icon_heart"] = {
		["bitmap"] = {
    		["row1"] = ".....|.....|.....|.....|.....|.....",
    		["row2"] = ".*.*.|.....|.*.*.|.....|.....|.....",
    		["row3"] = "*****|.*.*.|*****|.*.*.|.*.*.|.*.*.",
    		["row4"] = "*****|.***.|*****|.***.|.***.|.***.",
    		["row5"] = ".***.|.***.|.***.|.***.|.***.|.***.",
    		["row6"] = ".***.|..*..|.***.|..*..|..*..|..*..",
    		["row7"] = "..*..|.....|..*..|.....|.....|.....",
    		["row8"] = ".....|.....|.....|.....|.....|....."
        },
		["speed"] = foo,
		["type"] = "icon"
    },
	["widget_icon_heartbeat"] = {
		["bitmap"] = {
    		["row1"] = ".....|.....",
    		["row2"] = ".*.*.|.*.*.",
    		["row3"] = "*****|*.*.*",
    		["row4"] = "*****|*...*",
    		["row5"] = ".***.|.*.*.",
    		["row6"] = ".***.|.*.*.",
    		["row7"] = "..*..|..*..",
    		["row8"] = ".....|....."
        },
		["speed"] = foo,
		["type"] = "icon"
    },
	["widget_icon_karo"] = {
		["bitmap"] = {
    		["row1"] = ".....|.....|.....|.....|..*..|.....|.....|.....",
    		["row2"] = ".....|.....|.....|..*..|.*.*.|..*..|.....|.....",
    		["row3"] = ".....|.....|..*..|.*.*.|*...*|.*.*.|..*..|.....",
    		["row4"] = ".....|..*..|.*.*.|*...*|.....|*...*|.*.*.|..*..",
    		["row5"] = ".....|.....|..*..|.*.*.|*...*|.*.*.|..*..|.....",
    		["row6"] = ".....|.....|.....|..*..|.*.*.|..*..|.....|.....",
    		["row7"] = ".....|.....|.....|.....|..*..|.....|.....|.....",
    		["row8"] = ".....|.....|.....|.....|.....|.....|.....|....."
        },
		["speed"] = foo,
		["type"] = "icon"
    },
	["widget_icon_rain"] = {
		["bitmap"] = {
    		["row1"] = "...*.|.....|.....|.*...|....*|..*..|.....|*....",
    		["row2"] = "*....|...*.|.....|.....|.*...|....*|..*..|.....",
    		["row3"] = ".....|*....|...*.|.....|.....|.*...|....*|..*..",
    		["row4"] = "..*..|.....|*....|...*.|.....|.....|.*...|....*",
    		["row5"] = "....*|..*..|.....|*....|...*.|.....|.....|.*...",
    		["row6"] = ".*...|....*|..*..|.....|*....|...*.|.....|.....",
    		["row7"] = ".....|.*...|....*|..*..|.....|*....|...*.|.....",
    		["row8"] = ".....|.....|.*...|....*|..*..|.....|*....|...*."
        },
		["speed"] = foo,
		["type"] = "icon"
    },
	["widget_icon_squirrel"] = {
		["bitmap"] = {
    		["row1"] = ".....|.....|.....|.....|.....|.....",
    		["row2"] = ".....|.....|.....|.....|.....|.....",
    		["row3"] = ".....|.....|.....|.....|.....|.....",
    		["row4"] = "**...|.**..|..**.|...**|....*|.....",
    		["row5"] = "*****|*****|*****|*****|*****|*****",
    		["row6"] = "...**|..**.|.**..|**...|*....|.....",
    		["row7"] = ".....|.....|.....|.....|.....|.....",
    		["row8"] = ".....|.....|.....|.....|.....|....."
        },
		["speed"] = foo,
		["type"] = "icon"
    },
	["widget_icon_timer"] = {
		["bitmap"] = {
    		["row1"] = ".....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|",
    		["row2"] = ".***.|.*+*.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.+++.|.+*+.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|",
    		["row3"] = "*****|**+**|**++*|**+++|**++.|**++.|**+++|**+++|**+++|**+++|**+++|+++++|+++++|++*++|++**+|++***|++**.|++**.|++***|++***|++***|++***|++***|*****|",
    		["row4"] = "*****|**+**|**+**|**+**|**+++|**+++|**+++|**+++|**+++|**+++|+++++|+++++|+++++|++*++|++*++|++*++|++***|++***|++***|++***|++***|++***|*****|*****|",
    		["row5"] = "*****|*****|*****|*****|*****|***++|***++|**+++|*++++|+++++|+++++|+++++|+++++|+++++|+++++|+++++|+++++|+++**|+++**|++***|+****|*****|*****|*****|",
    		["row6"] = ".***.|.***.|.***.|.***.|.***.|.***.|.**+.|.*++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.++*.|.+**.|.***.|.***.|.***.|.***.|",
    		["row7"] = ".....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|",
    		["row8"] = ".....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|"
        },
		["speed"] = foo,
		["type"] = "icon"
    },
	["widget_icon_wave"] = {
		["bitmap"] = {
    		["row1"] = "..**.|.**..|**...|*....|.....|.....|.....|.....|....*|...**",
    		["row2"] = ".*..*|*..*.|..*..|.*...|*....|.....|.....|....*|...*.|..*..",
    		["row3"] = "*....|....*|...*.|..*..|.*...|*....|....*|...*.|..*..|.*...",
    		["row4"] = "*....|....*|...*.|..*..|.*...|*....|....*|...*.|..*..|.*...",
    		["row5"] = "*....|....*|...*.|..*..|.*...|*....|....*|...*.|..*..|.*...",
    		["row6"] = ".....|.....|....*|...*.|..*..|.*..*|*..*.|..*..|.*...|*....",
    		["row7"] = ".....|.....|.....|....*|...**|..**.|.**..|**...|*....|.....",
    		["row8"] = ".....|.....|.....|.....|.....|.....|.....|.....|.....|....."
        },
		["speed"] = foo,
		["type"] = "icon"
    },
	["widget_key_down"] = {
		["expression"] = "lcd.Transition(-1)",
		["key"] = 2,
		["type"] = "key"
    },
	["widget_key_up"] = {
		["expression"] = "lcd.Transition(1)",
		["key"] = 1,
		["type"] = "key"
    },
	["widget_percent"] = {
		["expression"] = "'%'",
		["type"] = "text"
    }
}
