######
## Version number in win32 pe executable
include(generate_product_version)

generate_product_version(
    ProductVersionFiles
    NAME ${PROJECT_NAME}
    VERSION_MAJOR ${PROJECT_VERSION_MAJOR}
    VERSION_MINOR ${PROJECT_VERSION_MINOR}
    VERSION_PATCH ${PROJECT_VERSION_PATCH}
    VERSION_REVISION ${PROJECT_VERSION_TWEAK}
    ICON ${${PROJECT_NAME}_APP_ICON}
    COMPANY_NAME ${${PROJECT_NAME}_COMPANY_NAME}
    COMPANY_COPYRIGHT ${${PROJECT_NAME}_COMPANY_COPYRIGHT}
)
add_definitions(-DFILE_VERSION_STR="${PROJECT_VERSION}")
message(${DSTATUS} "Project NAME: ${PROJECT_NAME}")
message(${DSTATUS} "Project Version: ${PROJECT_VERSION}")
target_sources(${PROJECT_NAME} PRIVATE ${ProductVersionFiles})
