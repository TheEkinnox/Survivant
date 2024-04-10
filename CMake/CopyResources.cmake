function(copy_resources TARGET_NAME)
	set(CMAKE_FOLDER .utils)

	# remove the existing assets folder from the build directory (ensures removes are also replicated)
	# copy the assets folder in the target build directory
	# remove unwanted files
	add_custom_target(${TARGET_NAME}_Resources
		DEPENDS ${RESOURCES_STAMP}
		COMMAND ${CMAKE_COMMAND} -E rm -rf $<TARGET_FILE_DIR:${TARGET_NAME}>/assets
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${SURVIVANT_ASSETS_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/assets
		COMMAND ${CMAKE_COMMAND} -E rm -f $<TARGET_FILE_DIR:${TARGET_NAME}>/assets/.gitkeep $<TARGET_FILE_DIR:${TARGET_NAME}>/assets/CMakeLists.txt
	)

	add_dependencies(${TARGET_NAME} ${TARGET_NAME}_Resources)

	message(STATUS "Resources added to target ${TARGET_NAME}")
endfunction()