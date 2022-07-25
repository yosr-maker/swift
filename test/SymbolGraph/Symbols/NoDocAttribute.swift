// RUN: %empty-directory(%t)
// RUN: %target-build-swift %s -module-name NoDocAttribute -emit-module -emit-module-path %t/
// RUN: %target-swift-symbolgraph-extract -module-name NoDocAttribute -I %t -pretty-print -output-dir %t
// RUN: %FileCheck %s --input-file %t/NoDocAttribute.symbols.json --check-prefix PUBLIC

// RUN: %target-swift-symbolgraph-extract -module-name NoDocAttribute -I %t -pretty-print -output-dir %t -minimum-access-level internal
// RUN: %FileCheck %s --input-file %t/NoDocAttribute.symbols.json --check-prefix INTERNAL

// RUN: %target-swift-symbolgraph-extract -module-name NoDocAttribute -I %t -pretty-print -output-dir %t -minimum-access-level private
// RUN: %FileCheck %s --input-file %t/NoDocAttribute.symbols.json --check-prefix PRIVATE

// This test is a mirror of SkipsPublicUnderscore.swift, but using `@_nodoc`
// instead of underscored names.

public protocol PublicProtocol {}

// PUBLIC-NOT: ShouldntAppear
// INTERNAL-DAG: ShouldntAppear
// PRIVATE-DAG: ShouldntAppear

@_nodoc public struct ShouldntAppear: PublicProtocol {
    public struct InnerShouldntAppear {}
}

public class SomeClass {
    // PUBLIC-NOT: internalVar
    // INTERNAL-NOT: internalVar
    // PRIVATE-DAG: internalVar
    @_nodoc internal var internalVar: String = ""
}

@_nodoc public protocol ProtocolShouldntAppear {}

public struct PublicStruct: ProtocolShouldntAppear {
    @_nodoc public struct InnerShouldntAppear {}
}
