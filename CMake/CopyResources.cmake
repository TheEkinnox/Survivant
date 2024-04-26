function(copy_resources TARGET_NAME)
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E create_symlink ${SURVIVANT_ASSETS_DIR} $<TARGET_FILE_DIR:${TARGET_NAME}>/assets
	)

	message(STATUS "Resources added to target ${TARGET_NAME}")
endfunction()