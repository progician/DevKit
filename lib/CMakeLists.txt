add_library(UnitTestMain OBJECT UnitTestMain.cpp)
target_link_libraries(UnitTestMain PUBLIC catch2::catch2 trompeloeil::trompeloeil)

add_library(TextModel STATIC
  TextModel/TextBuffer.cpp
)
target_include_directories(TextModel PUBLIC ${TOP_LEVEL_INCLUDE_DIR})

add_executable(TextModelUnit
  Generics/Tree.Test.cpp
  TextModel/TextBuffer.Test.cpp
)
target_link_libraries(TextModelUnit PRIVATE TextModel UnitTestMain)
add_test(TextModelUnitTests TextModelUnit)

add_custom_command(
  TARGET TextModelUnit
  POST_BUILD
  COMMENT "Running TextModel unit tests..."
  COMMAND TextModelUnit "[unit]"
)
