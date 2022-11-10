################################################################################
#####
##### Tectiform TFCopy CMake Configuration File
##### Created by: Steve Wilson
#####
################################################################################

list(APPEND COPY_FILES
    base_panel.cpp
    base_panel.hpp
    copy_panel.cpp
    copy_panel.hpp
    data_model.cpp
    data_model.hpp
    main.cpp
    startup_panel.cpp
    startup_panel.hpp
    )

add_executable(tfcopy ${COPY_FILES})
target_compile_features(tfcopy PRIVATE cxx_std_20)
target_compile_options(tfcopy PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
     ${GCC_LIKE_COMPILER_FLAGS}>)
target_link_libraries(tfcopy PRIVATE
     TFFoundation::TFFoundation-static
     CONAN_PKG::ftxui
     )

