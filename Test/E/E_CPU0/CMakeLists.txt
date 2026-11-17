# Please see README.md for build instructions

cmake_minimum_required(VERSION 3.16.4)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_SYSTEM_PROCESSOR arm)

# Use add_library() with the STATIC option to name the source file in the generated project.
# This avoids running the linker and is intended for use with cross-compiling toolchains that
# cannot link without custom flags or linker scripts
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo MinSizeRel)

if(BUILD_KIT)
	message(NOTICE "Build Kit is defined as ${BUILD_KIT}")
endif()

message(NOTICE "CMAKE_CURRENT_BINARY_DIR is defined as ${CMAKE_CURRENT_BINARY_DIR}")

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(isMultiConfig)
	message(NOTICE "Multi-config generator(${CMAKE_GENERATOR}) is active")
else()
	message(NOTICE "Single-config generator(${CMAKE_GENERATOR}) is active")
	if(DEFINED CMAKE_BUILD_TYPE)
		message(NOTICE "CMAKE_BUILD_TYPE is defined as ${CMAKE_BUILD_TYPE}")
	elseif(DEFINED BUILD_TYPE)
		set(CMAKE_BUILD_TYPE ${BUILD_TYPE})
		message(NOTICE "CMAKE_BUILD_TYPE is set to BUILD_TYPE ENV ${CMAKE_BUILD_TYPE}")
	else()
		message(FATAL_ERROR "Please specify the CMAKE_BUILD_TYPE with -DCMAKE_BUILD_TYPE=")
	endif()
	#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
	message(NOTICE "CMAKE_RUNTIME_OUTPUT_DIRECTORY is defined as ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
endif()

# Project configuration
project(E_CPU0
		VERSION 1.0.0
		LANGUAGES C CXX ASM)

set(CMAKE_CROSSCOMPILING "TRUE")
set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE")

# ============================================================
# 【重要】C++ 标准设置必须在 project() 之后，且在 include 之前
# ============================================================
# 设置 C++ 标准为 C++14（支持 std::make_unique, std::conditional_t 等）
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 禁用 GNU 扩展，使用标准 C++14（但某些嵌入式代码可能需要 gnu++14）
# set(CMAKE_CXX_EXTENSIONS OFF)

# ============================================================
# 在包含任何子 CMake 文件之前，先保存我们的 C++ 标准设置
# ============================================================
set(FORCE_CXX_STANDARD "14" CACHE INTERNAL "Forced C++ standard")

include(${CMAKE_CURRENT_LIST_DIR}/Config.cmake)

# GeneratedCfg.cmake - 只包含一次！
include(${CMAKE_CURRENT_LIST_DIR}/cmake/GeneratedCfg.cmake)

# ============================================================
# 【重要】在 GeneratedCfg.cmake 之后，强制覆盖 C++ 标准
# 因为 GeneratedCfg.cmake 可能添加了 -std=c++11
# ============================================================
# 方法1: 清理 CMAKE_CXX_FLAGS 中所有 -std=* 标志
string(REGEX REPLACE "-std=[^ ]*" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# 方法2: 使用目标编译选项代替全局标志（更推荐，但需要 target 存在）
# 由于此时 target 尚未定义，我们通过全局变量 + target 方式处理

# 设置全局的编译选项，在 target 创建时应用
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

# 保存一个变量，供后续 target 使用
set(CMAKE_CXX_FLAGS_SAVED "${CMAKE_CXX_FLAGS}" CACHE INTERNAL "Saved CXX flags")

message(NOTICE "CMAKE_CXX_STANDARD set to: ${CMAKE_CXX_STANDARD}")
message(NOTICE "CMAKE_CXX_FLAGS after cleanup: ${CMAKE_CXX_FLAGS}")

# GeneratedSrc.cmake - 只包含一次！
include(${CMAKE_CURRENT_LIST_DIR}/cmake/GeneratedSrc.cmake)

# ============================================================
# 添加项目特定的头文件路径
# ============================================================
include_directories(
		${CMAKE_CURRENT_LIST_DIR}/zf_common
		${CMAKE_CURRENT_LIST_DIR}/zf_device
)

# ============================================================
# 以下是为特定源文件设置的编译选项（只设置一次）
# ============================================================

# 针对 arm_atan2_q31.c 编译器内部错误的临时解决方案
set_source_files_properties(
		${CMAKE_CURRENT_LIST_DIR}/ra/arm/CMSIS-DSP/Source/FastMathFunctions/arm_atan2_q31.c
		PROPERTIES COMPILE_OPTIONS "-O0"
)

# 针对 arm_biquad_cascade_df1_q15.c 向量类型转换错误的解决方案
set_source_files_properties(
		${CMAKE_CURRENT_LIST_DIR}/ra/arm/CMSIS-DSP/Source/FilteringFunctions/arm_biquad_cascade_df1_q15.c
		PROPERTIES COMPILE_OPTIONS "-flax-vector-conversions"
)

# 为所有 CMSIS-DSP 源文件添加 -flax-vector-conversions 选项
# 以解决向量类型转换错误（批量处理）
file(GLOB_RECURSE CMSIS_DSP_SOURCES
		${CMAKE_CURRENT_LIST_DIR}/ra/arm/CMSIS-DSP/Source/*.c
)

foreach(SRC ${CMSIS_DSP_SOURCES})
	# 跳过已经特殊处理的文件（arm_atan2_q31.c 已经用 -O0）
	if(NOT SRC MATCHES "arm_atan2_q31\\.c$")
		set_source_files_properties(${SRC} PROPERTIES COMPILE_OPTIONS "-flax-vector-conversions")
	endif()
endforeach()

# ============================================================
# Ethos-U Vela 生成的代码（src/mera）里把 int32_t* 传给 int* 形参。
# GCC 14 起 -Wincompatible-pointer-types 由警告变成错误，而代码是自动生成的、
# 重新生成还会出现，所以这里只把这一条降回警告。
# ============================================================
file(GLOB_RECURSE MERA_GENERATED_SOURCES
		${CMAKE_CURRENT_LIST_DIR}/src/mera/*.c
)
foreach(SRC ${MERA_GENERATED_SOURCES})
	set_source_files_properties(${SRC} PROPERTIES
			COMPILE_OPTIONS "-Wno-error=incompatible-pointer-types")
endforeach()

# ============================================================
# 如果 GeneratedSrc.cmake 定义了 E_CPU0.elf 目标，需要在此之后添加
# 覆盖编译选项，确保所有 C++ 文件使用 C++14
# ============================================================
# 检查目标是否存在，如果存在则覆盖其编译选项
if(TARGET E_CPU0.elf)
	# 移除所有 -std=* 标志
	target_compile_options(E_CPU0.elf PRIVATE
			$<$<COMPILE_LANG_AND_ID:CXX,ARMClang>:SHELL:--std=c++14>
			$<$<COMPILE_LANG_AND_ID:CXX,GNU>:SHELL:-std=c++14>
			$<$<COMPILE_LANG_AND_ID:CXX,Clang>:SHELL:-std=c++14>
	)

	# 方法2: 对于 GNU 工具链，使用 gnu++14 可能更兼容嵌入式代码
	# target_compile_options(E_CPU0.elf PRIVATE -std=gnu++14)

	message(NOTICE "Overridden C++ standard for target E_CPU0.elf")
else()
	message(WARNING "Target E_CPU0.elf not defined yet - C++ standard will be applied via global flags")
endif()
# 链接 zf_device_config 库
target_link_libraries(E_CPU0.elf PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/zf_device/libzf_device_config.a
)