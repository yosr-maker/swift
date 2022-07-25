// RUN: %empty-directory(%t)
// RUN: %target-swift-frontend %S/Inputs/ExportedImport/A.swift -module-name A -emit-module -emit-module-path %t/A.swiftmodule
// RUN: %target-swift-frontend %S/Inputs/ExportedImport/B.swift -module-name B -emit-module -emit-module-path %t/B.swiftmodule

// RUN: %target-swift-frontend %s -module-name NoDocExportedImport -emit-module -emit-module-path /dev/null -I %t -emit-symbol-graph -emit-symbol-graph-dir %t/
// RUN: %FileCheck %s --input-file %t/NoDocExportedImport.symbols.json --check-prefix PUBLIC
// RUN: ls %t | %FileCheck %s --check-prefix FILES

// RUN: %target-swift-frontend %s -module-name NoDocExportedImport -emit-module -emit-module-path /dev/null -I %t -emit-symbol-graph -emit-symbol-graph-dir %t/ -symbol-graph-minimum-access-level internal
// RUN: %FileCheck %s --input-file %t/NoDocExportedImport.symbols.json --check-prefix INTERNAL
// RUN: ls %t | %FileCheck %s --check-prefix FILES

@_nodoc @_exported import A
@_nodoc @_exported import struct B.StructOne

// PUBLIC-NOT: InternalSymbolFromA
// PUBLIC-NOT: StructTwo
// PUBLIC-NOT: "precise":"s:1A11SymbolFromAV"
// PUBLIC-NOT: "precise":"s:1B9StructOneV"

// INTERNAL-NOT: InternalSymbolFromA
// INTERNAL-NOT: StructTwo
// INTERNAL-DAG: "precise":"s:1A11SymbolFromAV"
// INTERNAL-DAG: "precise":"s:1B9StructOneV"

// FIXME: Symbols from `@_exported import` do not get emitted when using swift-symbolgraph-extract
// This is tracked by https://bugs.swift.org/browse/SR-15921.

// FILES-NOT: NoDocExportedImport@A.symbols.json

