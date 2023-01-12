# Usage:
# AutoBuild_use_package_Assimp(
#    # The 1st argument is required.  It is the name of the target you wish to link this dependency to.
#    my-program 
#
#    # The 2nd argument is required.  It impacts how dependencies are propagated through CMake.  You can treat
#    # this dependency as an INTERFACE, PUBLIC, or PRIVATE.  See the following URL for details:
#    # https://cmake.org/cmake/help/latest/command/target_include_directories.html
#    PUBLIC 
# )
#
macro(AutoBuild_use_package_Assimp YOUR_TARGET INTERFACE_PUBLIC_OR_PRIVATE)

    set(PACKAGE_NAME "Assimp")
    string(TOUPPER ${PACKAGE_NAME} PACKAGE_NAME_UPPER)

    AutoBuild_find_package_module_mode(${PACKAGE_NAME})

    if ("${AUTOBUILD_PACKAGE_${PACKAGE_NAME_UPPER}}")
        if ("${AUTOBUILD_EXECUTE_NOW}")

            message(STATUS "AutoBuild: Beginning download, build, install sequence.")

            AutoBuild_download_project( 
                ${PACKAGE_NAME}
                GIT_REPOSITORY https://github.com/assimp/assimp.git
            )

            set(ASSIMP_CONFIG_OPTIONS -DBUILD_SHARED_LIBS=OFF —DASSIMP_BUILD_TESTS=OFF —DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE} -DASSIMP_INSTALL_PDB=OFF -DASSIMP_BUILD_ZLIB=ON -DASSIMP_WARNINGS_AS_ERRORS=OFF
            )
			if (APPLE)
				list(APPEND ASSIMP_CONFIG_OPTIONS -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD="c++11"
				)
                message(status "Assimp config options: ${ASSIMP_CONFIG_OPTIONS}")
                #set(ASSIMP_CONFIG_OPTIONS ${ASSIMP_CONFIG_OPTIONS}
                #    -DCMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD="c++11" -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT} -DCMAKE_PREFIX_PATH=${STDLIB_SDK_PATH}/usr
                #)
			endif()

			
            if (MSVC)
                list(APPEND ASSIMP_CONFIG_OPTIONS -DUSE_MSVC_RUNTIME_LIBRARY_DLL=ON
                )
            endif()

            #STRING(REGEX REPLACE ";" " " ASSIMP_ESCAPED_CONFIG_OPTIONS "${ASSIMP_CONFIG_OPTIONS}" )

            AutoBuild_build_and_install_project(${PACKAGE_NAME} . ${ASSIMP_CONFIG_OPTIONS})

            AutoBuild_find_built_package_module_mode(${PACKAGE_NAME})

            set(${PACKAGE_NAME_UPPER}_AUTOBUILT "TRUE" CACHE BOOL "Confirms that package ${PACKAGE_NAME} was successfully built by the AutoBuild system.")
            mark_as_advanced(${PACKAGE_NAME}_AUTOBUILT)

        else()
            message(STATUS "AutoBuild: Scheduled to build ${PACKAGE_NAME} the next time CMake Configure is run.")
        endif()
    endif()


    # If we've found the package, either directly on the system or after autobuilding, then link with it
    if ("${${PACKAGE_NAME}_FOUND}" OR "${${PACKAGE_NAME_UPPER}_FOUND}")
        message(STATUS "Linking target ${YOUR_TARGET} with ${INTERFACE_PUBLIC_OR_PRIVATE} dependency ${PACKAGE_NAME}.")

        target_link_libraries(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${ASSIMP_LIBRARIES})
        target_include_directories(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} ${ASSIMP_INCLUDE_DIRS})

        target_compile_definitions(${YOUR_TARGET} ${INTERFACE_PUBLIC_OR_PRIVATE} -DUSE_${PACKAGE_NAME})		
    endif()

endmacro()
