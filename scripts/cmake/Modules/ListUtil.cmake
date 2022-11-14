# Use this micro to add prefix to each item and save to list
macro(list_append_with_prefix _list _prefix)
    foreach(_item ${ARGN})
        list(APPEND ${_list} ${_prefix}${_item})
    endforeach()
endmacro()


# Use this micro to remove all items in _list1 which are in _list2
macro(list_remove_all _list1 _list2)
    foreach(_item ${${_list2}})
        list(REMOVE_ITEM ${_list1} ${_item})
    endforeach()
endmacro()


macro(list_add_flatly _list)
    set(_temp_list)
    file(GLOB _temp_list ${ARGN})
    list(APPEND ${_list} ${_temp_list})
    unset(_temp_list)
endmacro()


macro(list_add_recursively _list)
    set(_temp_list)
    file(GLOB_RECURSE _temp_list ${ARGN})
    list(APPEND ${_list} ${_temp_list})
    unset(_temp_list)
endmacro()


macro(add_files _src)
    set(options CLEAR CURRENT CURRENT_RECURSE)
    set(oneValueArgs)
    set(multiValueArgs DIRECTORIES PATTERNS)
    cmake_parse_arguments(FUNC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(_temp_src)

    if(FUNC_CLEAR)
        set(${_src})
    endif()

    # Add current dir
    if(FUNC_CURRENT_RECURSE)
        foreach(_pat ${FUNC_PATTERNS})
        list_add_recursively(_temp_src ${CMAKE_CURRENT_SOURCE_DIR}/${_pat})
        endforeach()
    elseif(FUNC_CURRENT)
        foreach(_pat ${FUNC_PATTERNS})
            list_add_flatly(_temp_src ${CMAKE_CURRENT_SOURCE_DIR}/${_pat})
        endforeach()
    endif()

    # Add other dirs recursively
    foreach(_dir ${FUNC_DIRECTORIES})
        foreach(_pat ${FUNC_PATTERNS})
            list_add_recursively(_temp_src ${_dir}/${_pat})
        endforeach()
    endforeach()

    list(APPEND ${_src} ${_temp_src})

    unset(_temp_src)
endmacro()
