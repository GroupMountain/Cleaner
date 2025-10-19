add_rules("mode.debug", "mode.release", "mode.releasedbg")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("groupmountain-repo https://github.com/GroupMountain/xmake-repo.git")
add_repositories("miracleforest https://github.com/MiracleForest/xmake-repo")

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

-- Option 1: Use the latest version of LeviLamina released on GitHub.
add_requires("levilamina 1.5.2")
add_requires("levibuildscript")
add_requires("gmlib 1.5.2")
add_requires("ilistenattentively 0.8.0")

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
        "gmlib",
        "ilistenattentively"
    )
    add_defines(
        "NOMINMAX", 
        "UNICODE",
        "_HAS_CXX23=1"
    )
    add_rules("@levibuildscript/linkrule")
    set_exceptions("none")
    set_kind("shared")
    set_languages("cxx20")
    set_symbols("debug")

    after_build(function (target)
        local mod_packer = import("scripts.after_build")

        local mod_define = {
            modName = target:name(),
            modFile = path.filename(target:targetfile()),
        }
        
        mod_packer.pack_mod(target,mod_define)
    end)
