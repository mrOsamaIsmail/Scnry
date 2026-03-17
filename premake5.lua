workspace "Scnry"
    configurations {"Debug","Release"}
    architecture "x86_64"
    project "ScnryTest"
        language "C++"
        cppdialect "C++17"
        kind "ConsoleApp"
        files {
           
            "./Test/**.h",
            "./Test/**.c",
            "./Test/**.cpp"
        }
        includedirs
        {
            "./include"
        }
        filter "configurations:Debug"
        libdirs
        {
            --"./Dependencies/yaml-cpp/bin/Debug"
            "./libScnry/bin/Debug"
        }
        links"LibScnry.lib"
        targetdir "TestOut/bin/Debug"
        objdir "TestOut/obj/Debug"

        filter "configurations:Release"
        libdirs
        {
            --"./Dependencies/yaml-cpp/bin/Release/"
            "./libScnry/bin/Release"
        }
        links"LibScnry.lib"
        
        targetdir "TestOut/bin/Release"
        objdir "TestOut/obj/Release"

        filter{}

    project "LibScnry"
        language "C++"
        cppdialect "C++17"
        kind "staticLib"
        includedirs
        {
            "./include",
             "./Dependencies/yaml-cpp/include/"

        }
        files {
            "./include/**.h",
            "./include/**.hpp",
            "./Dependencies/yaml-cpp/include/**.h",
            "./Dependencies/yaml-cpp/include/**.hpp",
            "./Dependencies/yaml-cpp/src/**.h",
            "./Dependencies/yaml-cpp/src/**.cpp",
            "./Scnry.cpp",--,
            "./include/ProjectTypes.h",
            "./ProjectSerializer.cpp"--,
            --"./MainTest.cpp"
        }
       
        
    filter "configurations:Debug"
        targetdir "libScnry/bin/Debug"
        objdir "libScnry/obj/Debug"
        
    filter "configurations:Release"
        targetdir "libScnry/bin/Release"
        objdir "libScnry/obj/Release"

    filter {}
-- Clean action
newaction {
    trigger = "clean",
    description = "Clean build files",
    execute = function()
        os.rmdir("bin")
        os.rmdir("obj")
        os.rmdir(".vs")
        
        -- Remove project files
        if os.host() == "windows" then
            os.remove("*.sln")
            os.remove("*.vcxproj")
            os.remove("*.vcxproj.filters")
        elseif os.host() == "linux" or os.host() == "macosx" then
            os.remove("Makefile")
        end
        
        print("Project cleaned")
    end
}