set_project("Midnight Engine 2D")

-- Enable standard build modes (debug, release, releasedbg, etc.)
add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

-- Set C++ standard
set_languages("cxx17")

-- Dependencies managed automatically by Xmake
add_requires("sfml >=3.0.0", {
    system = false,
    configs = {
        graphics = true,
        window   = true,
        audio    = true,
        network  = false,   -- set to true if you need it
        shared   = false    -- static is usually easier on Windows
    }
})

add_requires("imgui-sfml", {system = false})

target("game_engine")
set_kind("binary")
set_targetdir("build/bin")
set_rundir("build/bin")

add_files("src/main.cpp")
add_files("src/**/*.cpp")
add_headerfiles("src/**/*.h")

add_includedirs("src", {public = true})
add_packages("sfml", "imgui-sfml")

-- helpful on Windows with static SFML
if is_plat("windows") then
    add_syslinks("opengl32", "gdi32", "user32", "advapi32", "winmm", "ws2_32")
end

-- Copy assets post-build
after_build(function (target)
    local src_assets = path.join(target:scriptdir(), "src/example_game/game_data")
    local dst_assets = path.join(target:targetdir(), "game")
    os.mkdir(dst_assets)
    os.cp(path.join(src_assets, "*"), dst_assets)
end)