workspace "Axiom"
    architecture "ARM64"
    startproject "Axiom"

    configurations {
        "Debug", "Release",
        "Debug_26.1", "Release_26.1"
    }

outputdir = "%{cfg.buildcfg}_%{cfg.architecture}"

include "Axiom"
