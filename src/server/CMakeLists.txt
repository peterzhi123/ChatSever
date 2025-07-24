
aux_source_directory(. SRC_DIR)

add_executable(ChatServer ${SRC_DIR})

target_link_libraries(ChatServer muduo_net muduo_base pthread)