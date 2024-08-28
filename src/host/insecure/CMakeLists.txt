include(./function.cmake)
set(CHAT_SOURCE_FILES
# NOTE: you can add your insecure source files here
  chat.cpp file_stub.cpp
)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
set(COMPUTE_NODE_FILES
# NOTE: you can add your insecure source files here
  compute_node.cpp file_stub.cpp
)

# add_executable(server ${SERVER_SOURCE_FILES})
# add_executable(client ${CLIENT_SOURCE_FILES})
add_executable(client ${CHAT_SOURCE_FILES})
add_executable(compute_node ${COMPUTE_NODE_FILES})

# target_link_libraries(chat secure)
find_package(foonathan_memory REQUIRED)
# target_link_libraries(server secure distributed_tee fastrtps fastcdr foonathan_memory rt)
# target_link_libraries(client secure distributed_tee fastrtps fastcdr foonathan_memory rt)
target_link_libraries(client secure distributed_tee fastrtps fastcdr foonathan_memory rt)
target_link_libraries(compute_node secure distributed_tee fastrtps fastcdr foonathan_memory rt)

find_package(foonathan_memory REQUIRED)
foreach(EXE IN LISTS ${TEE_EXECUTABLE_TARGETS})
    target_link_libraries(${EXE} distributed_tee fastrtps fastcdr foonathan_memory rt)
endforeach()
