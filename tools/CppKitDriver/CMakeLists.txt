add_executable(CppKitDriver main.cpp)
add_executable(CppKit main.mm)
target_link_libraries(CppKit
  PRIVATE
    "-framework Foundation"
    "-framework Cocoa"
)