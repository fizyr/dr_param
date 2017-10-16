cmake_minimum_required(VERSION 3.8)
project(dr_param)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-Wall -Wextra -Wpedantic)

find_package(catkin REQUIRED COMPONENTS
	dr_error
	dr_util
	roscpp
)

find_package(yaml-cpp REQUIRED)
find_package(fmt REQUIRED)

catkin_package(
	INCLUDE_DIRS include
	LIBRARIES ${PROJECT_NAME}
	DEPENDS YAML_CPP
	CATKIN_DEPENDS dr_error
)

include_directories(include/${PROJECT_NAME})
include_directories(SYSTEM
	${catkin_INCLUDE_DIRS}
	${YAML_CPP_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
	src/xmlrpc.cpp
	src/yaml.cpp
	src/yaml_preprocess.cpp
)

target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES} fmt::fmt-header-only)

add_definitions("-DTEST_DATA=${CMAKE_CURRENT_SOURCE_DIR}/test/data")

catkin_add_gtest(${PROJECT_NAME}_test_xmlrpc test/xmlrpc.cpp)
target_link_libraries(${PROJECT_NAME}_test_xmlrpc ${PROJECT_NAME})
catkin_add_gtest(${PROJECT_NAME}_test_yaml test/yaml.cpp)
target_link_libraries(${PROJECT_NAME}_test_yaml ${PROJECT_NAME})
catkin_add_gtest(${PROJECT_NAME}_test_yaml_preprocess test/yaml_preprocess.cpp)
target_link_libraries(${PROJECT_NAME}_test_yaml_preprocess ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
	DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
	FILES_MATCHING PATTERN "*.hpp"
)
