add_rules("mode.debug", "mode.release", "mode.releasedbg")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("groupmountain-repo https://github.com/GroupMountain/xmake-repo.git")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

-- Option 1: Use the latest version of LeviLamina released on GitHub.
add_requires("levilamina")
add_requires("gmlib 0.13.7")

target("Cleaner") -- Change this to your mod name.
    add_cxflags(
        "/EHa",
        "/utf-8"
    )
    add_files(
        "src/**.cpp"
    )
    add_includedirs(
        "src"
    )
    add_packages(
        "levilamina",
        "gmlib"
    )
    add_shflags(
        "/DELAYLOAD:bedrock_server.dll"
    )
    set_exceptions("none")
    set_kind("shared")
    set_languages("cxx23")

    after_build(function (target)
        local mod_packer = import("scripts.after_build")

        local mod_define = {
            modName = target:name(),
            modFile = path.filename(target:targetfile()),
        }
        
        mod_packer.pack_mod(target,mod_define)
    end)
