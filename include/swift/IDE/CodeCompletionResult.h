//===--- CodeCompletionResult.h -------------------------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2022 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_IDE_CODECOMPLETION_RESULT_H
#define SWIFT_IDE_CODECOMPLETION_RESULT_H

#include "swift/Basic/StringExtras.h"
#include "swift/IDE/CodeCompletionResultType.h"
#include "swift/IDE/CodeCompletionString.h"

namespace swift {
namespace ide {

struct CodeCompletionResultSink;

/// Describes the origin of the code completion result.
///
/// This enum is ordered from the contexts that are "nearest" to the code
/// completion point to "outside" contexts.
enum class SemanticContextKind : uint8_t {
  /// Used in cases when the concept of semantic context is not applicable.
  None,

  /// A declaration from the same function.
  Local,

  /// A declaration found in the immediately enclosing nominal decl.
  CurrentNominal,

  /// A declaration found in the superclass of the immediately enclosing
  /// nominal decl.
  Super,

  /// A declaration found in the non-immediately enclosing nominal decl.
  ///
  /// For example, 'Foo' is visible at (1) because of this.
  /// \code
  ///   struct A {
  ///     typealias Foo = Int
  ///     struct B {
  ///       func foo() {
  ///         // (1)
  ///       }
  ///     }
  ///   }
  /// \endcode
  OutsideNominal,

  /// A declaration from the current module.
  CurrentModule,

  /// A declaration imported from other module.
  OtherModule,

  MAX_VALUE = OtherModule
};

enum class CodeCompletionFlairBit : uint8_t {
  /// **Deprecated**. Old style catch-all prioritization.
  ExpressionSpecific = 1 << 0,

  /// E.g. override func foo() { super.foo() ...
  SuperChain = 1 << 1,

  /// Argument label and type. i.e. 'label: <#Ty#>'.
  ArgumentLabels = 1 << 2,

  /// E.g. decl introducer or modifiers ('enum', 'protocol', 'public', etc.) at
  /// top-level.
  CommonKeywordAtCurrentPosition = 1 << 3,

  /// E.g. type decl introducer ('enum', 'class', etc.) in a function body.
  RareKeywordAtCurrentPosition = 1 << 4,

  /// E.g. protocol names at an expression position.
  RareTypeAtCurrentPosition = 1 << 5,

  /// E.g. referencing a type, function, etc… at top level position in a non
  /// script/main.swift file
  ExpressionAtNonScriptOrMainFileScope = 1 << 6,
};

using CodeCompletionFlair = OptionSet<CodeCompletionFlairBit>;

/// The declaration kind of a code completion result, if it is a declaration.
enum class CodeCompletionDeclKind : uint8_t {
  Module,
  Class,
  Actor,
  Struct,
  Enum,
  EnumElement,
  Protocol,
  AssociatedType,
  TypeAlias,
  GenericTypeParam,
  Constructor,
  Destructor,
  Subscript,
  StaticMethod,
  InstanceMethod,
  PrefixOperatorFunction,
  PostfixOperatorFunction,
  InfixOperatorFunction,
  FreeFunction,
  StaticVar,
  InstanceVar,
  LocalVar,
  GlobalVar,
  PrecedenceGroup,
};

enum class CodeCompletionLiteralKind : uint8_t {
  ArrayLiteral,
  BooleanLiteral,
  ColorLiteral,
  DictionaryLiteral,
  IntegerLiteral,
  ImageLiteral,
  NilLiteral,
  StringLiteral,
  Tuple,
};

enum class CodeCompletionOperatorKind : uint8_t {
  None,
  Unknown,
  Bang,       // !
  NotEq,      // !=
  NotEqEq,    // !==
  Modulo,     // %
  ModuloEq,   // %=
  Amp,        // &
  AmpAmp,     // &&
  AmpStar,    // &*
  AmpPlus,    // &+
  AmpMinus,   // &-
  AmpEq,      // &=
  LParen,     // ( -- not really an operator, but treated as one in some cases.
  Star,       // *
  StarEq,     // *=
  Plus,       // +
  PlusEq,     // +=
  Minus,      // -
  MinusEq,    // -=
  Dot,        // .
  DotDotDot,  // ...
  DotDotLess, // ..<
  Slash,      // /
  SlashEq,    // /=
  Less,       // <
  LessLess,   // <<
  LessLessEq, // <<=
  LessEq,     // <=
  Eq,         // =
  EqEq,       // ==
  EqEqEq,     // ===
  Greater,    // >
  GreaterEq,  // >=
  GreaterGreater,   // >>
  GreaterGreaterEq, // >>=
  QuestionDot,      // ?.
  Caret,            // ^
  CaretEq,          // ^=
  Pipe,             // |
  PipeEq,           // |=
  PipePipe,         // ||
  TildeEq,          // ~=

  MAX_VALUE = TildeEq
};

enum class CodeCompletionKeywordKind : uint8_t {
  None,
#define KEYWORD(X) kw_##X,
#define POUND_KEYWORD(X) pound_##X,
#include "swift/Syntax/TokenKinds.def"
};

enum class CompletionKind : uint8_t {
  None,
  Import,
  UnresolvedMember,
  DotExpr,
  StmtOrExpr,
  PostfixExprBeginning,
  PostfixExpr,
  PostfixExprParen,
  KeyPathExprObjC,
  KeyPathExprSwift,
  TypeDeclResultBeginning,
  TypeSimpleBeginning,
  TypeIdentifierWithDot,
  TypeIdentifierWithoutDot,
  CaseStmtKeyword,
  CaseStmtBeginning,
  NominalMemberBeginning,
  AccessorBeginning,
  AttributeBegin,
  AttributeDeclParen,
  EffectsSpecifier,
  PoundAvailablePlatform,
  CallArg,
  LabeledTrailingClosure,
  ReturnStmtExpr,
  YieldStmtExpr,
  ForEachSequence,

  /// The \c in keyword in a for-each loop.
  ForEachInKw,
  AfterPoundExpr,
  AfterPoundDirective,
  PlatformConditon,
  AfterIfStmtElse,
  GenericRequirement,
  PrecedenceGroup,
  StmtLabel,
  ForEachPatternBeginning,
  TypeAttrBeginning,
  OptionalBinding,
};

enum class CodeCompletionDiagnosticSeverity : uint8_t {
  None,
  Error,
  Warning,
  Remark,
  Note,

  MAX_VALUE = Note
};

/// Reasons why a code completion item might not be recommended in a certain
/// context.
/// This enum is split into two subsets: \c ContextFreeNotRecommendedReason and
/// \c ContextualNotRecommendedReason. When adding a case to this enum also add
/// it to either of those.
/// Context-free diagnostics are independent of the context they are used in.
/// The not-recommended reason can thus be cached as part of a context free
/// code completion result.
/// Contextual not recommended reasons depend on the context they are used in.
/// E.g. \c InvalidAsyncContext depends on whether the usage context is async or
/// not.
enum class NotRecommendedReason : uint8_t {
  None = 0,                    // both contextual and context-free
  RedundantImport,             // contextual
  RedundantImportIndirect,     // contextual
  Deprecated,                  // context-free
  SoftDeprecated,              // context-free
  InvalidAsyncContext,         // contextual
  CrossActorReference,         // contextual
  VariableUsedInOwnDefinition, // contextual

  MAX_VALUE = VariableUsedInOwnDefinition
};

/// TODO: We consider deprecation warnings as context free although they don't
/// produce deprecation warnings insdide context that have the same or a
/// stronger deprecation attribute.
/// E.g.
/// In SDK:
/// \code
/// @available(iOS, deprecated: 12.0)
/// func deprecatedFunc()
/// \endcode
///
/// User code:
/// \code
/// @available(iOS, deprecated: 12.0)
/// func insiderUserDeprecatedContext() {
///   #^COMPLETE^#
/// }
/// \endcode
/// suggests \c deprecatedFunc as deprecated although it doesn't produce a
/// diagnostic during compilation. But this allows us to show deprecation
/// warnings for cached results.
enum class ContextFreeNotRecommendedReason : uint8_t {
  None = 0,
  Deprecated,
  SoftDeprecated,
  MAX_VALUE = SoftDeprecated
};

enum class ContextualNotRecommendedReason : uint8_t {
  None = 0,
  RedundantImport,
  RedundantImportIndirect,
  InvalidAsyncContext,
  CrossActorReference,
  VariableUsedInOwnDefinition,
  MAX_VALUE = VariableUsedInOwnDefinition
};

enum class CodeCompletionResultKind : uint8_t {
  Declaration,
  Keyword,
  Pattern,
  Literal,
  BuiltinOperator,

  MAX_VALUE = BuiltinOperator
};

/// The parts of a \c CodeCompletionResult that are not dependent on the context
/// it appears in and can thus be cached.
class ContextFreeCodeCompletionResult {
  CodeCompletionResultKind Kind : 3;
  static_assert(int(CodeCompletionResultKind::MAX_VALUE) < 1 << 3, "");

  union {
    CodeCompletionDeclKind Decl;
    CodeCompletionLiteralKind Literal;
    CodeCompletionKeywordKind Keyword;
    uint8_t Opaque;
  } AssociatedKind;
  static_assert(sizeof(AssociatedKind) == sizeof(AssociatedKind.Opaque),
                "Opaque should cover all bits");

  CodeCompletionOperatorKind KnownOperatorKind : 6;
  static_assert(int(CodeCompletionOperatorKind::MAX_VALUE) < 1 << 6, "");

  bool IsSystem : 1;
  CodeCompletionString *CompletionString;
  NullTerminatedStringRef ModuleName;
  NullTerminatedStringRef BriefDocComment;
  ArrayRef<NullTerminatedStringRef> AssociatedUSRs;
  CodeCompletionResultType ResultType;

  ContextFreeNotRecommendedReason NotRecommended : 3;
  static_assert(int(ContextFreeNotRecommendedReason::MAX_VALUE) < 1 << 3, "");

  CodeCompletionDiagnosticSeverity DiagnosticSeverity : 3;
  static_assert(int(CodeCompletionDiagnosticSeverity::MAX_VALUE) < 1 << 3, "");

  NullTerminatedStringRef DiagnosticMessage;
  NullTerminatedStringRef FilterName;

public:
  /// Memberwise initializer. \p AssociatedKInd is opaque and will be
  /// interpreted based on \p Kind. If \p KnownOperatorKind is \c None and the
  /// completion item is an operator, it will be determined based on the
  /// compleiton string.
  ///
  /// \note The caller must ensure that the \p CompleitonString and all the
  /// \c Ref types outlive this result, typically by storing them in the same
  /// \c CodeCompletionResultSink as the result itself.
  ContextFreeCodeCompletionResult(
      CodeCompletionResultKind Kind, uint8_t AssociatedKind,
      CodeCompletionOperatorKind KnownOperatorKind, bool IsSystem,
      CodeCompletionString *CompletionString,
      NullTerminatedStringRef ModuleName,
      NullTerminatedStringRef BriefDocComment,
      ArrayRef<NullTerminatedStringRef> AssociatedUSRs,
      CodeCompletionResultType ResultType,
      ContextFreeNotRecommendedReason NotRecommended,
      CodeCompletionDiagnosticSeverity DiagnosticSeverity,
      NullTerminatedStringRef DiagnosticMessage,
      NullTerminatedStringRef FilterName)
      : Kind(Kind), KnownOperatorKind(KnownOperatorKind), IsSystem(IsSystem),
        CompletionString(CompletionString), ModuleName(ModuleName),
        BriefDocComment(BriefDocComment), AssociatedUSRs(AssociatedUSRs),
        ResultType(ResultType), NotRecommended(NotRecommended),
        DiagnosticSeverity(DiagnosticSeverity),
        DiagnosticMessage(DiagnosticMessage), FilterName(FilterName) {
    this->AssociatedKind.Opaque = AssociatedKind;
    assert((NotRecommended == ContextFreeNotRecommendedReason::None) ==
               (DiagnosticSeverity == CodeCompletionDiagnosticSeverity::None) &&
           "Result should be not recommended iff it has a diagnostic");
    assert((DiagnosticSeverity == CodeCompletionDiagnosticSeverity::None) ==
               DiagnosticMessage.empty() &&
           "Completion item should have diagnostic message iff the diagnostics "
           "severity is not none");
    assert(CompletionString && "Result should have a completion string");
    if (isOperator() && KnownOperatorKind == CodeCompletionOperatorKind::None) {
      this->KnownOperatorKind = getCodeCompletionOperatorKind(CompletionString);
    }
    assert(!isOperator() ||
           getKnownOperatorKind() != CodeCompletionOperatorKind::None &&
               "isOperator implies operator kind != None");
  }

  /// Constructs a \c Pattern or \c BuiltinOperator result.
  ///
  /// \note The caller must ensure that the \p CompletionString and \c
  /// StringRefs outlive this result, typically by storing them in the same
  /// \c CodeCompletionResultSink as the result itself.
  static ContextFreeCodeCompletionResult *createPatternOrBuiltInOperatorResult(
      CodeCompletionResultSink &Sink, CodeCompletionResultKind Kind,
      CodeCompletionString *CompletionString,
      CodeCompletionOperatorKind KnownOperatorKind,
      NullTerminatedStringRef BriefDocComment,
      CodeCompletionResultType ResultType,
      ContextFreeNotRecommendedReason NotRecommended,
      CodeCompletionDiagnosticSeverity DiagnosticSeverity,
      NullTerminatedStringRef DiagnosticMessage);

  /// Constructs a \c Keyword result.
  ///
  /// \note The caller must ensure that the \p CompletionString and
  /// \p BriefDocComment outlive this result, typically by storing them in
  /// the same \c CodeCompletionResultSink as the result itself.
  static ContextFreeCodeCompletionResult *
  createKeywordResult(CodeCompletionResultSink &Sink,
                      CodeCompletionKeywordKind Kind,
                      CodeCompletionString *CompletionString,
                      NullTerminatedStringRef BriefDocComment,
                      CodeCompletionResultType ResultType);

  /// Constructs a \c Literal result.
  ///
  /// \note The caller must ensure that the \p CompletionString outlives this
  /// result, typically by storing them in the same \c CodeCompletionResultSink
  /// as the result itself.
  static ContextFreeCodeCompletionResult *
  createLiteralResult(CodeCompletionResultSink &Sink,
                      CodeCompletionLiteralKind LiteralKind,
                      CodeCompletionString *CompletionString,
                      CodeCompletionResultType ResultType);

  /// Constructs a \c Declaration result.
  ///
  /// \note The caller must ensure that the \p CompletionString and all
  /// \c StringRefs outlive this result, typically by storing them in the same
  /// \c CodeCompletionResultSink as the result itself.
  static ContextFreeCodeCompletionResult *createDeclResult(
      CodeCompletionResultSink &Sink, CodeCompletionString *CompletionString,
      const Decl *AssociatedDecl, NullTerminatedStringRef ModuleName,
      NullTerminatedStringRef BriefDocComment,
      ArrayRef<NullTerminatedStringRef> AssociatedUSRs,
      CodeCompletionResultType ResultType,
      ContextFreeNotRecommendedReason NotRecommended,
      CodeCompletionDiagnosticSeverity DiagnosticSeverity,
      NullTerminatedStringRef DiagnosticMessage);

  CodeCompletionResultKind getKind() const { return Kind; }

  /// Returns the raw associated kind which will be interpreted differently
  /// depending on the completion item's kind.
  uint8_t getOpaqueAssociatedKind() const { return AssociatedKind.Opaque; }

  CodeCompletionDeclKind getAssociatedDeclKind() const {
    assert(getKind() == CodeCompletionResultKind::Declaration);
    return AssociatedKind.Decl;
  }

  CodeCompletionLiteralKind getLiteralKind() const {
    assert(getKind() == CodeCompletionResultKind::Literal);
    return AssociatedKind.Literal;
  }

  CodeCompletionKeywordKind getKeywordKind() const {
    assert(getKind() == CodeCompletionResultKind::Keyword);
    return AssociatedKind.Keyword;
  }

  CodeCompletionOperatorKind getKnownOperatorKind() const {
    assert(isOperator());
    return KnownOperatorKind;
  }

  bool isSystem() const { return IsSystem; };

  CodeCompletionString *getCompletionString() const { return CompletionString; }

  NullTerminatedStringRef getModuleName() const { return ModuleName; }

  NullTerminatedStringRef getBriefDocComment() const { return BriefDocComment; }

  ArrayRef<NullTerminatedStringRef> getAssociatedUSRs() const {
    return AssociatedUSRs;
  }

  const CodeCompletionResultType &getResultType() const { return ResultType; }

  ContextFreeNotRecommendedReason getNotRecommendedReason() const {
    return NotRecommended;
  }

  CodeCompletionDiagnosticSeverity getDiagnosticSeverity() const {
    return DiagnosticSeverity;
  }
  NullTerminatedStringRef getDiagnosticMessage() const {
    return DiagnosticMessage;
  }

  NullTerminatedStringRef getFilterName() const { return FilterName; }

  bool isOperator() const {
    if (getKind() == CodeCompletionResultKind::Declaration) {
      switch (getAssociatedDeclKind()) {
      case CodeCompletionDeclKind::PrefixOperatorFunction:
      case CodeCompletionDeclKind::PostfixOperatorFunction:
      case CodeCompletionDeclKind::InfixOperatorFunction:
        return true;
      default:
        return false;
      }
    } else {
      return getKind() == CodeCompletionResultKind::BuiltinOperator;
    }
  }

  /// Helper methods used during initialization of
  /// \c ContextFreeCodeCompletionResult.
  static CodeCompletionOperatorKind
  getCodeCompletionOperatorKind(const CodeCompletionString *str);
  static CodeCompletionDeclKind getCodeCompletionDeclKind(const Decl *D);
  static bool getDeclIsSystem(const Decl *D);
};

/// A single code completion result enriched with information that depend on
/// the completion's usage context.
class CodeCompletionResult {
  const ContextFreeCodeCompletionResult &ContextFree;
  SemanticContextKind SemanticContext : 3;
  static_assert(int(SemanticContextKind::MAX_VALUE) < 1 << 3, "");

  unsigned char Flair : 8;

  /// Contextual diagnostics. If the contextual not recommended reason is
  /// \c None, then the context free diagnostic will be shown to the user.
  ContextualNotRecommendedReason NotRecommended : 4;
  static_assert(int(ContextualNotRecommendedReason::MAX_VALUE) < 1 << 4, "");

  CodeCompletionDiagnosticSeverity DiagnosticSeverity : 3;
  static_assert(int(CodeCompletionDiagnosticSeverity::MAX_VALUE) < 1 << 3, "");

  NullTerminatedStringRef DiagnosticMessage;

  /// The number of bytes to the left of the code completion point that
  /// should be erased first if this completion string is inserted in the
  /// editor buffer.
  unsigned NumBytesToErase : 7;

public:
  static const unsigned MaxNumBytesToErase = 127;

private:
  CodeCompletionResultTypeRelation TypeDistance : 3;
  static_assert(int(CodeCompletionResultTypeRelation::MAX_VALUE) < 1 << 3, "");

  /// Memberwise initializer
  /// The \c ContextFree result must outlive this result. Typically, this is
  /// done by allocating the two in the same sink or adopting the context free
  /// sink in the sink that allocates this result.
  CodeCompletionResult(const ContextFreeCodeCompletionResult &ContextFree,
                       SemanticContextKind SemanticContext,
                       CodeCompletionFlair Flair, uint8_t NumBytesToErase,
                       CodeCompletionResultTypeRelation TypeDistance,
                       ContextualNotRecommendedReason NotRecommended,
                       CodeCompletionDiagnosticSeverity DiagnosticSeverity,
                       NullTerminatedStringRef DiagnosticMessage)
      : ContextFree(ContextFree), SemanticContext(SemanticContext),
        Flair(Flair.toRaw()), NotRecommended(NotRecommended),
        DiagnosticSeverity(DiagnosticSeverity),
        DiagnosticMessage(DiagnosticMessage), NumBytesToErase(NumBytesToErase),
        TypeDistance(TypeDistance) {}

public:
  /// Enrich a \c ContextFreeCodeCompletionResult with the following contextual
  /// information.
  /// This computes the type relation between the completion item and its
  /// expected type context.
  /// See \c CodeCompletionResultType::calculateTypeRelation for documentation
  /// on \p USRTypeContext.
  /// The \c ContextFree result must outlive this result. Typically, this is
  /// done by allocating the two in the same sink or adopting the context free
  /// sink in the sink that allocates this result.
  CodeCompletionResult(const ContextFreeCodeCompletionResult &ContextFree,
                       SemanticContextKind SemanticContext,
                       CodeCompletionFlair Flair, uint8_t NumBytesToErase,
                       const ExpectedTypeContext *TypeContext,
                       const DeclContext *DC,
                       const USRBasedTypeContext *USRTypeContext,
                       ContextualNotRecommendedReason NotRecommended,
                       CodeCompletionDiagnosticSeverity DiagnosticSeverity,
                       NullTerminatedStringRef DiagnosticMessage);

  const ContextFreeCodeCompletionResult &getContextFreeResult() const {
    return ContextFree;
  }

  /// Return a pointer to the data structure storing the context free
  /// properties.
  /// The pointer is valid as long as this result is alive.
  const ContextFreeCodeCompletionResult *getContextFreeResultPtr() {
    return &ContextFree;
  }

  /// Copy this result to \p Sink with \p newFlair . Note that this does NOT
  /// copy the context free result. Thus the caller needs to ensure that the
  /// context free result outlives the result the result returned by this
  /// method.
  CodeCompletionResult *withFlair(CodeCompletionFlair newFlair,
                                  CodeCompletionResultSink &Sink) const;

  /// Copy this result to \p Sink with \p newFlair . Note that this does NOT
  /// copy the context free result. Thus the caller needs to ensure that the
  /// context free result outlives the result the result returned by this
  /// method.
  CodeCompletionResult *withContextFreeResultSemanticContextAndFlair(
      const ContextFreeCodeCompletionResult &NewContextFree,
      SemanticContextKind NewSemanticContext, CodeCompletionFlair NewFlair,
      CodeCompletionResultSink &Sink) const;

  CodeCompletionResultKind getKind() const {
    return getContextFreeResult().getKind();
  }

  CodeCompletionDeclKind getAssociatedDeclKind() const {
    return getContextFreeResult().getAssociatedDeclKind();
  }

  CodeCompletionLiteralKind getLiteralKind() const {
    return getContextFreeResult().getLiteralKind();
  }

  CodeCompletionKeywordKind getKeywordKind() const {
    return getContextFreeResult().getKeywordKind();
  }

  bool isOperator() const { return getContextFreeResult().isOperator(); }

  CodeCompletionOperatorKind getOperatorKind() const {
    return getContextFreeResult().getKnownOperatorKind();
  }

  bool isSystem() const { return getContextFreeResult().isSystem(); }

  CodeCompletionResultTypeRelation getExpectedTypeRelation() const {
    return TypeDistance;
  }

  /// Get the contextual not-recommended reason. This disregards context-free
  /// not recommended reasons.
  ContextualNotRecommendedReason getContextualNotRecommendedReason() const {
    return NotRecommended;
  }

  /// Return the contextual not recommended reason if there is one. If there is
  /// no contextual not recommended reason, return the context-free not
  /// recommended reason.
  NotRecommendedReason getNotRecommendedReason() const {
    switch (NotRecommended) {
    case ContextualNotRecommendedReason::None:
      switch (getContextFreeResult().getNotRecommendedReason()) {
      case ContextFreeNotRecommendedReason::None:
        return NotRecommendedReason::None;
      case ContextFreeNotRecommendedReason::Deprecated:
        return NotRecommendedReason::Deprecated;
      case ContextFreeNotRecommendedReason::SoftDeprecated:
        return NotRecommendedReason::SoftDeprecated;
      }
    case ContextualNotRecommendedReason::RedundantImport:
      return NotRecommendedReason::RedundantImport;
    case ContextualNotRecommendedReason::RedundantImportIndirect:
      return NotRecommendedReason::RedundantImportIndirect;
    case ContextualNotRecommendedReason::InvalidAsyncContext:
      return NotRecommendedReason::InvalidAsyncContext;
    case ContextualNotRecommendedReason::CrossActorReference:
      return NotRecommendedReason::CrossActorReference;
    case ContextualNotRecommendedReason::VariableUsedInOwnDefinition:
      return NotRecommendedReason::VariableUsedInOwnDefinition;
    }
  }

  SemanticContextKind getSemanticContext() const { return SemanticContext; }

  CodeCompletionFlair getFlair() const { return CodeCompletionFlair(Flair); }

  /// Modify "flair" of this result *in place*.
  void setFlair(CodeCompletionFlair flair) { Flair = flair.toRaw(); }

  bool isNotRecommended() const {
    return getNotRecommendedReason() != NotRecommendedReason::None;
  }

  unsigned getNumBytesToErase() const { return NumBytesToErase; }

  CodeCompletionString *getCompletionString() const {
    return getContextFreeResult().getCompletionString();
  }

  NullTerminatedStringRef getModuleName() const {
    return getContextFreeResult().getModuleName();
  }

  NullTerminatedStringRef getBriefDocComment() const {
    return getContextFreeResult().getBriefDocComment();
  }

  ArrayRef<NullTerminatedStringRef> getAssociatedUSRs() const {
    return getContextFreeResult().getAssociatedUSRs();
  }

  /// Get the contextual diagnostic severity. This disregards context-free
  /// diagnostics.
  CodeCompletionDiagnosticSeverity getContextualDiagnosticSeverity() const {
    return DiagnosticSeverity;
  }

  /// Get the contextual diagnostic message. This disregards context-free
  /// diagnostics.
  NullTerminatedStringRef getContextualDiagnosticMessage() const {
    return DiagnosticMessage;
  }

  /// Return the contextual diagnostic severity if there was a contextual
  /// diagnostic. If there is no contextual diagnostic, return the context-free
  /// diagnostic severity.
  CodeCompletionDiagnosticSeverity getDiagnosticSeverity() const {
    if (NotRecommended != ContextualNotRecommendedReason::None) {
      return DiagnosticSeverity;
    } else {
      return getContextFreeResult().getDiagnosticSeverity();
    }
  }

  /// Return the contextual diagnostic message if there was a contextual
  /// diagnostic. If there is no contextual diagnostic, return the context-free
  /// diagnostic message.
  NullTerminatedStringRef getDiagnosticMessage() const {
    if (NotRecommended != ContextualNotRecommendedReason::None) {
      return DiagnosticMessage;
    } else {
      return getContextFreeResult().getDiagnosticMessage();
    }
  }

  NullTerminatedStringRef getFilterName() const {
    return getContextFreeResult().getFilterName();
  }

  /// Print a debug representation of the code completion result to \p OS.
  void printPrefix(raw_ostream &OS) const;
  SWIFT_DEBUG_DUMP;
};

} // end namespace ide
} // end namespace swift

namespace llvm {
template <> struct DenseMapInfo<swift::ide::CodeCompletionKeywordKind> {
  using Kind = swift::ide::CodeCompletionKeywordKind;
  static Kind getEmptyKey() { return Kind(~0u); }
  static Kind getTombstoneKey() { return Kind(~1u); }
  static unsigned getHashValue(const Kind &Val) { return unsigned(Val); }
  static bool isEqual(const Kind &LHS, const Kind &RHS) { return LHS == RHS; }
};
template <> struct DenseMapInfo<swift::ide::CodeCompletionLiteralKind> {
  using Kind = swift::ide::CodeCompletionLiteralKind;
  static Kind getEmptyKey() { return Kind(~0u); }
  static Kind getTombstoneKey() { return Kind(~1u); }
  static unsigned getHashValue(const Kind &Val) { return unsigned(Val); }
  static bool isEqual(const Kind &LHS, const Kind &RHS) { return LHS == RHS; }
};
template <> struct DenseMapInfo<swift::ide::CodeCompletionDeclKind> {
  using Kind = swift::ide::CodeCompletionDeclKind;
  static Kind getEmptyKey() { return Kind(~0u); }
  static Kind getTombstoneKey() { return Kind(~1u); }
  static unsigned getHashValue(const Kind &Val) { return unsigned(Val); }
  static bool isEqual(const Kind &LHS, const Kind &RHS) { return LHS == RHS; }
};
}

#endif // SWIFT_IDE_CODECOMPLETION_RESULT_H
