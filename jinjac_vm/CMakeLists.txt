
set(JINJAC_EXE_SRCS jinjac_vm.c loader.c vm.c stack.c j_object.c j_array.c j_value.c convert.c buildin.c param.c str_obj.c)

add_executable(jinjac_vm ${JINJAC_EXE_SRCS})
