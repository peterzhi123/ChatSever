aux_source_directory(. CLIENT_DIR)

add_executable(ChatClient ${CLIENT_DIR})

target_link_libraries(ChatClient pthread)