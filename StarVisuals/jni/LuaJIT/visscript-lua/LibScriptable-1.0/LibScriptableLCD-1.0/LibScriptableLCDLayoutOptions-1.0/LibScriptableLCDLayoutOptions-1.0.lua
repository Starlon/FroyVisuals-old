
local MAJOR = "LibScriptableLCDLayoutOptions-1.0" 
local MINOR = 24
assert(LibStub, MAJOR.." requires LibStub") 
local LayoutOptions = LibStub:NewLibrary(MAJOR, MINOR)
if not LayoutOptions then return end


function LayoutOptions:RebuildOpts(visitor, db, name) 
	local options = {
		name = {
			name = name,
			type = "input",
			get = function() return db.name end,
			set = function(info, v) db.name = v end,
			order = 1
		},
		layers = {
			name = "Layers",
			type = "group",
			args = {
				add = {
					name = "Add Layer",
					type = "input",
					set = function(info, v)
						db[v] = {}
					end,
					order = 1
				}
			},
			order = 2
		}
	}
	for layer, v in ipairs(db) do
		options.layers.args["layer" .. layer] = {
			name = "Layer " .. layer,
			type = "group",
			args = {
				rows = {
					name = "Rows",
					type = "group",
					args = {}
				}
			}
		}

		for row, v in ipairs(db[layer]) do
			options.layers.args["layer" .. layer].args.rows.args["row" .. row] = {
				name = "Row " .. row,
				type = "group",
				args = {
					add = {
						name = "Add Column",
						desc = "Add a column",
						type = "input",
						pattern = "%d",
						set = function(info, v)
							db[layer][row][v] = "<placeholder>"
						end,
						order = 1
					},
				},
				order = row + 1
			}
			options.layers.args["layer" .. layer].args.rows.args["row" .. row].args.cols = {
				name = "Columns",
				type = "group",
				args = {}
			}
			for col, vv in ipairs(v) do
				options.layers.args["layer" .. layer].args.rows.args["row" .. row].args.cols.args["col" .. col] = {
					name = "Column" .. col,
					--desc = "Input a widget name",
					type = "input",
					get = function() return vv end,
					set = function(info, val) 
						db[layer][row][col] = val 
						visitor:RebuildOpts()
					end,
					order = col + 1
				}
			end
		end
	end
	
	return options
end
