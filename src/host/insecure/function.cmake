
set(TEE_EXECUTABLE_TARGETS)

function(tee_add_executable target_name)
    add_executable(${target_name} ${ARGN})
    list(APPEND TEE_EXECUTABLE_TARGETS ${target_name})
    set(TEE_EXECUTABLE_TARGETS ${TEE_EXECUTABLE_TARGETS} PARENT_SCOPE)
endfunction()
