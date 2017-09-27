cmake_minimum_required(VERSION 2.8.3)
project(dr_param)

find_package(catkin REQUIRED COMPONENTS
	dr_base
	dr_error
	roscpp
)

find_package(yaml-cpp REQUIRED)

catkin_package(
	INCLUDE_DIRS include
	LIBRARIES ${PROJECT_NAME}
	DEPENDS YAML_CPP
)

dr_include_directories(
	include/${PROJECT_NAME}
	${catkin_INCLUDE_DIRS}
	${YAML_CPP_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
	src/xmlrpc.cpp
	src/yaml.cpp
)

target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} ${YAML_CPP_LIBRARIES})

dr_add_gtest(xmlrpc test/xmlrpc.cpp)
target_link_libraries(${PROJECT_NAME}_test_xmlrpc ${PROJECT_NAME})
dr_add_gtest(yaml test/yaml.cpp)
target_link_libraries(${PROJECT_NAME}_test_yaml ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/${PROJECT_NAME}/
	DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
	FILES_MATCHING PATTERN "*.hpp"
)
