--
-- Name:        vscode_workspace.lua
-- Purpose:     Generate a vscode file.
-- Author:      Ryan Pusztai
-- Modified by: Andrea Zanellato
--              Manu Evans
--              Yehonatan Ballas
-- Created:     2013/05/06
-- Copyright:   (c) 2008-2020 Jason Perkins and the Premake project
--

local p = premake
local project = p.project
local workspace = p.workspace
local tree = p.tree
local vscode = p.modules.vscode

vscode.workspace = {}
local m = vscode.workspace

--
-- Generate a vscode file
--
function m.generate(wks)
	p.utf8()
	p.w('{"folders": [')

	-- workspace should be first for clangd to use it as the working directory
	p.w('{')
	p.w('"path": "."')
	p.w('},')

	--
	-- Project list
	--
	local root_src_dirs = {}
	local tr = workspace.grouptree(wks)
	tree.traverse(tr, {
		onleaf = function(n)
			local prj = n.project

			if prj.workspace.location ~= prj.location then
				local prjpath = path.getrelative(prj.workspace.location, prj.location)
				p.w('{')
				p.w('"path": "%s"', prjpath)
				p.w('},')
			end

			-- add root source file directories
			local non_root_path = '' -- used to remove the non root part from the end of a leaf node's path
			local tr = project.getsourcetree(prj)
			tree.traverse(tr, {
				onbranchenter = function(node, depth)
					if depth ~= 0 then
						non_root_path = non_root_path .. '/' .. node.name
					end
				end,
				onbranchexit = function(node, depth)
					if depth ~= 0 then
						non_root_path = non_root_path:sub(1, non_root_path:len()-(node.name:len()+1))
					end
				end,
				onleaf = function(node, depth)
					if node.relpath == nil then
						return
					end
					non_root_path = non_root_path ..'/'.. node.name
					local rel_root_path = node.relpath:sub(1, node.relpath:len()-(non_root_path:len()))
					non_root_path = non_root_path:sub(1, non_root_path:len()-(node.name:len()+1))
					root_src_dirs[rel_root_path] = true
				end
			})
		end,
	})

	for src_dir_rel in pairs(root_src_dirs) do
		print(src_dir_rel)
		p.w('{')
		p.w('"path": "%s"', src_dir_rel)
		p.w('},')
	end

	-- for clangd to find compile_commands.json in the build dir
	p.w('],')
	p.w('"settings":{')
	p.w('"clangd.arguments":[')
	p.w('"--compile-commands-dir=.",')
	p.w('"--background-index",')
	p.w('"--header-insertion=never"')
	p.w(']}')

	p.w('}')

	--TODO wks.startproject
end

function m.generate_tasks(wks)
	p.utf8()
	_p('{')
	_p(1, '"version": "2.0.0",')
	_p(1, '"tasks": [')

	local first_task = true
	local tr = workspace.grouptree(wks)
	tree.traverse(tr, {
		onleaf = function(n)
			local prj = n.project
			for cfg in project.eachconfig(prj) do
				if not first_task then
					_p(1, ',{')
				else
					first_task = false
					_p(1, '{')
				end
				_p(2, '"type": "shell",')
				_p(2, '"label": "build %s (%s)",', prj.name, cfg.name)
				_p(2, '"command": "make config=%s -j8",', cfg.name:lower())
				_p(2, '"args": [],')
				_p(2, '"options": {')
					_p(3, '"cwd": "${workspaceFolder}/"')
				_p(2, '},')
				_p(2, '"problemMatcher": [')
					_p(3, '"$gcc"')
				_p(2, '],')
				if cfg.name == "Debug_26.1" then
					_p(2, '"group": { "kind": "build", "isDefault": true }')
				else
					_p(2, '"group": "build"')
				end
				_p(1, '}')
			end
		end,
	})
	_p(1, ']')
	_p('}')
end

function m.generate_launch(wks)
	p.utf8()
	_p('{')
	_p(1, '"configurations": [')

	local first_cfg = true
	local tr = workspace.grouptree(wks)
	tree.traverse(tr, {
		onleaf = function(n)
			local prj = n.project
			for cfg in project.eachconfig(prj) do
			if first_cfg then
				first_cfg = false
				_p(1, '{')
			else
				_p(1, ',{')
			end
				_p(2, '"name": "%s (%s)",', prj.name, cfg.name)
				_p(2, '"type": "lldb",')
				_p(2, '"request": "launch",')
				_p(2, '"program": "%s/%s",', cfg.buildtarget.directory, prj.name)
				_p(2, '"args": [],')
				_p(2, '"stopOnEntry": false,')
				_p(2, '"terminal": "external",')
				_p(2, '"cwd": "%s",', cfg.buildtarget.directory)
				_p(2, '"preLaunchTask": "build %s (%s)"', prj.name, cfg.name)
			_p(1, '}')
			end
		end,
	})

	_p(1, ']')
	_p('}')
end
