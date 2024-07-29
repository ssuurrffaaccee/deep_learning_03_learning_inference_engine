cmake_minimum_required(VERSION 3.26)
project(
  learn_blaze
  VERSION 1.0.0
  LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(ONNX)
find_package(protobuf)
find_package(OpenBLAS)


set(LINK_TARGET onnx::onnx protobuf::protobuf OpenBLAS::OpenBLAS)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
endif()
include_directories("${PROJECT_SOURCE_DIR}/src")


function(compile_proto_file proto_files_dir  out_generated_cc_file)
  file(GLOB SRC_FILES_PROTO CONFIGURE_DEPENDS "${proto_files_dir}/*.proto")
  message("----->${SRC_FILES_PROTO}")
  set(generated_cc_file "")
  foreach(PROTO_FILE ${SRC_FILES_PROTO})    
    get_filename_component(PROTO_NAME "${PROTO_FILE}" NAME_WE)  
    set(GEN_CC_FILE "${proto_files_dir}/${PROTO_NAME}.pb.cc")  
    set(GEN_HDR_FILE "${proto_files_dir}/${PROTO_NAME}.pb.h")  
    add_custom_command(  
        OUTPUT ${GEN_CC_FILE} ${GEN_HDR_FILE}  
        COMMAND ${PROTOBUF_PROTOC_EXECUTABLE}  
        ARGS --cpp_out=${proto_files_dir} -I${proto_files_dir} ${PROTO_FILE}  
        COMMENT "Generating C++ source from ${PROTO_FILE}"  
        VERBATIM  
    )
    list(APPEND generated_cc_file ${GEN_CC_FILE}) 
  endforeach()  
  set(${${out_generated_cc_file}} ${generated_cc_file} PARENT_SCOPE)
endfunction()

# add_compile_options(-mavx -mavx2 -march=native -mavx512f -mavx512bf16)
# add_compile_options(-mavx -mavx2 -march=native )
function(learn_step step_name)  
    set(GENERATED_CC_FILE "")
    set(OUT_GENERATED_CC_FILE "GENERATED_CC_FILE")
    compile_proto_file("${PROJECT_SOURCE_DIR}/src/${step_name}/blaze/proto/" OUT_GENERATED_CC_FILE)
    file(GLOB_RECURSE SRC_FILES_CPP CONFIGURE_DEPENDS "src/${step_name}/*.cc")
    # file(GLOB_RECURSE SRC_FILES_CU CONFIGURE_DEPENDS "src/${step_name}/*.cu")
    set(SRC_FILES ${SRC_FILES_CPP} ${SRC_FILES_CU})
    add_executable(${step_name} ${SRC_FILES} ${GENERATED_CC_FILE})
    target_compile_options(${step_name} PRIVATE -mavx -mavx2 -march=native) 
    target_link_libraries(${step_name} ${LINK_TARGET})
    target_include_directories(${step_name} PRIVATE "${PROJECT_SOURCE_DIR}/src/${step_name}/")
    install(TARGETS ${step_name} RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/${step_name})
endfunction()

learn_step(00-load_onnx_model)
learn_step(01-simple_cpu_op)



