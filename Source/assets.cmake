FUNCTION(target_copy_assets)
	set(oneValueArgs TARGET ASSETS_DIRECTORY)
	set(multiValueArgs ASSETS)
	cmake_parse_arguments(PARSED_ARG "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})


	get_target_property(TARGET_DIR ${PARSED_ARG_TARGET} RUNTIME_OUTPUT_DIRECTORY)
	set(RESULT_FOLDER "${TARGET_DIR}/${PARSED_ARG_ASSETS_DIRECTORY}")
	file(MAKE_DIRECTORY ${RESULT_FOLDER})

	message(STATUS "Copying assets for ${PARSED_ARG_TARGET} into ${RESULT_FOLDER}")

	foreach(AssetPath ${PARSED_ARG_ASSETS})
		add_custom_command(
			TARGET ${PARSED_ARG_TARGET}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy "${AssetPath}" "${RESULT_FOLDER}/${AssetPath}"
			COMMENT "Copying ${AssetPath}..."
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
	endforeach()
ENDFUNCTION()