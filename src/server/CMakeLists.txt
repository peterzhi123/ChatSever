
aux_source_directory(. SRC_DIR)
aux_source_directory(./db DB_DIR)

add_executable(ChatServer ${SRC_DIR} ${DB_DIR})

target_link_libraries(ChatServer muduo_net muduo_base pthread mysqlclient)