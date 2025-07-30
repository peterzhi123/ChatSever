
aux_source_directory(. SRC_DIR)
aux_source_directory(./db DB_DIR)
aux_source_directory(./model MODEL_DIR)
aux_source_directory(./redis REDIS_DIR)

add_executable(ChatServer ${SRC_DIR} ${DB_DIR} ${MODEL_DIR} ${REDIS_DIR})

target_link_libraries(ChatServer muduo_net muduo_base pthread mysqlclient hiredis)