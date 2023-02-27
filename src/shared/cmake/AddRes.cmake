#[[
    ck_add_res(
        <target>
        FILES           <files>
        DIRS            <dirs>
        [TARGET         <target2>]
        [RELATIVE_PATH  <rel_path>]
        [ABSOLUTE_PATH  <abs_path>]
    )

    usage:
        copy files and dirs to destination relative to target2(default to ${_target}) after build
]] #
function(ck_add_res _target)
    set(options)
    set(oneValueArgs RELATIVE_PATH ABSOLUTE_PATH TARGET)
    set(multiValueArgs FILES DIRS)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(FUNC_TARGET)
        set(_path_target ${FUNC_TARGET})
    else()
        set(_path_target ${_target})
    endif()

    if(FUNC_ABSOLUTE_PATH)
        set(_dest ${FUNC_ABSOLUTE_PATH})
    else()
        if(FUNC_RELATIVE_PATH)
            set(_dest $<TARGET_FILE_DIR:${_path_target}>/${FUNC_RELATIVE_PATH})
        else()
            set(_dest $<TARGET_FILE_DIR:${_path_target}>)
        endif()
    endif()

    # Generate uuid
    string(RANDOM LENGTH 8 _rand)
    set(_res_update_target ${_target}_res_update_${_rand})

    add_custom_target(${_res_update_target})

    # Copy files
    if(FUNC_FILES)
        file(GLOB _files ${FUNC_FILES})

        foreach(_file ${_files})
            add_custom_command(
                TARGET ${_target}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${_dest}
                COMMAND ${CMAKE_COMMAND} -E copy ${_file} ${_dest}
            )
            add_custom_command(
                TARGET ${_res_update_target}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${_dest}
                COMMAND ${CMAKE_COMMAND} -E copy ${_file} ${_dest}
            )

            if(FUNC_RELATIVE_PATH)
                get_filename_component(_name ${_file} NAME)
                get_target_property(_temp ${_target} TC_RES_FILES)

                if(_temp STREQUAL _temp-NOTFOUND)
                    set(_temp)
                endif()

                list(APPEND _temp ${_dest}/${_name})
                set_target_properties(${_target} PROPERTIES TC_RES_FILES "${_temp}")
            endif()
        endforeach()
    endif()

    # Copy dirs
    if(FUNC_DIRS)
        foreach(_dir ${FUNC_DIRS})
            file(GLOB_RECURSE _files ${_dir}/*)

            foreach(_file ${_files})
                file(RELATIVE_PATH _rel_path ${_dir} ${_file})
                add_custom_command(
                    TARGET ${_target}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${_dest}/${_rel_path}
                    COMMAND ${CMAKE_COMMAND} -E copy ${_file} ${_dest}/${_rel_path}
                )
                add_custom_command(
                    TARGET ${_res_update_target}
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E make_directory ${_dest}/${_rel_path}
                    COMMAND ${CMAKE_COMMAND} -E copy ${_file} ${_dest}/${_rel_path}
                )

                if(FUNC_RELATIVE_PATH)
                    get_filename_component(_name ${_file} NAME)
                    get_target_property(_temp ${_target} TC_RES_FILES)

                    if(_temp STREQUAL _temp-NOTFOUND)
                        set(_temp)
                    endif()

                    list(APPEND _temp ${_dest}/${_name})
                    set_target_properties(${_target} PROPERTIES TC_RES_FILES "${_temp}")
                endif()
            endforeach()
        endforeach()
    endif()
endfunction()