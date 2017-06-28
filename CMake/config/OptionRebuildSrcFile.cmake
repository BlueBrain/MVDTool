# Rebuild source?
option(${OPT_REBUILD_OPTION_NAME} "Rebuild ${OPT_REBUILD_SOURCE_FILE} from sources" OFF)

if(EXISTS ${OPT_REBUILD_SOURCE_FILE})
    if(${OPT_REBUILD_OPTION_NAME})
        # Force build
        file(REMOVE ${OPT_REBUILD_SOURCE_FILE})
    else()
        # Refresh cpp to avoid build
        message("Reusing ${OPT_REBUILD_SOURCE_FILE}...")
        execute_process(COMMAND touch ${OPT_REBUILD_SOURCE_FILE})
    endif()
else()
    # Must create
    set(${OPT_REBUILD_OPTION_NAME} ON)
endif()

# Checking requirements
if(${OPT_REBUILD_OPTION_NAME})
    message("Will rebuild ${OPT_REBUILD_SOURCE_FILE} from sources")
endif()
