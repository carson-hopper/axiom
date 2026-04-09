if os.isfile("vendor/premake-vscode/vscode.lua") then
    require "vendor/premake-vscode/vscode"
end

workspace "Axiom"
    architecture "ARM64"
    startproject "Axiom"

    configurations {
        "Debug", "Release",
        "Debug_26.1", "Release_26.1"
    }

outputdir = "%{cfg.buildcfg}_%{cfg.architecture}"

include "Axiom"
