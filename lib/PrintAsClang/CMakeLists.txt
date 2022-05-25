
add_swift_host_library(swiftPrintAsClang STATIC
  ClangSyntaxPrinter.cpp
  DeclAndTypePrinter.cpp
  ModuleContentsWriter.cpp
  PrimitiveTypeMapping.cpp
  PrintAsClang.cpp
  PrintClangFunction.cpp
  PrintClangValueType.cpp
  PrintSwiftToClangCoreScaffold.cpp
  SwiftToClangInteropContext.cpp)
target_link_libraries(swiftPrintAsClang PRIVATE
  swiftAST
  swiftClangImporter
  swiftFrontend
  swiftIDE
  swiftIRGen)

set_swift_llvm_is_available(swiftPrintAsClang)

# Add target to install the list of symbols added to compatibility headers
set(symbols_file "${CMAKE_CURRENT_SOURCE_DIR}/compatibility_symbols")

add_custom_target(swift-compatibility-symbols DEPENDS ${symbols_file})

add_dependencies(swiftPrintAsClang swift-compatibility-symbols)

swift_install_in_component(
  FILES
    ${symbols_file}
  DESTINATION
    "share/swift"
  COMPONENT
    compiler
)
