#include "compiler.hpp"
#include "scanner.hpp"
#include "chunk.hpp"
#include "value.hpp"
#include "debug.hpp"

#include <string_view>
#include <print>
#include <utility>
#include <cstdint>
#include <optional>
#include <charconv>
#include <system_error>
#include <limits>
#include <functional>
#include <unordered_map>

namespace
{

using namespace cpplox;

struct ParseRule;
[[nodiscard]] static const ParseRule& GetRule( const TokenType type );

struct Parser
{
    Token current;
    Token previous;
    bool hadError = false;
    bool panicMode = false;
};
static Parser parser;
static Chunk compilingChunk;
static Scanner scanner{ "" };

static Chunk& CurrentChunk()
{
    return compilingChunk;
}

enum class Precedence
{
    None,
    Assignment, // =
    Or,         // or
    And,        // and
    Equality,   // == !=
    Comparison, // < > <= >=
    Term,       // + -
    Factor,     // * /
    Unary,      // - !
    Call,       // . ()
    Primary,
};

struct ParseRule
{
    std::function<void()> prefix;
    std::function<void()> infix;
    Precedence precedence;
};

static void ErrorAt( const Token& token, const std::string_view message )
{
    if ( parser.panicMode )
    {
        return;
    }
    parser.panicMode = true;

    std::print( "[line {}] Error", token.line );

    if ( token.type == TokenType::Eof )
    {
        std::print( " at end" );
    }
    else if ( token.type == TokenType::Error )
    {
        // Nothing.
    }
    else
    {
        std::print( " at '{}'", token.lexeme );
    }

    std::println( ": {}", message );
    parser.hadError = true;
}

static void ErrorAtCurrent( const std::string_view message )
{
    ErrorAt( parser.current, message );
}

static void Error( const std::string_view message )
{
    ErrorAt( parser.previous, message );
}

[[nodiscard]] static Precedence NextPrecedence( const Precedence precedence )
{
    if ( precedence == Precedence::Primary )
    {
        Error( "No precedence higher than Primary." );
        return Precedence::Primary;
    }
    else
    {
        return static_cast<Precedence>( static_cast<int>( precedence ) + 1 );
    }
}

static void Advance()
{
    parser.previous = parser.current;

    while ( true )
    {
        parser.current = scanner.ScanToken();
        if ( parser.current.type != TokenType::Error )
        {
            break;
        }

        ErrorAtCurrent( parser.current.lexeme );
    }
}

static void Consume( const TokenType type, const std::string_view message )
{
    if ( parser.current.type == type )
    {
        Advance();
        return;
    }

    ErrorAtCurrent( message );
}

static void EmitByte( const OpCode opcode )
{
    CurrentChunk().WriteChunk( opcode, parser.previous.line );
}

static void EmitByte( const std::uint8_t byte )
{
    CurrentChunk().WriteChunk( byte, parser.previous.line );
}

static void EmitBytes( const OpCode opcode, const std::uint8_t byte )
{
    EmitByte( opcode );
    EmitByte( byte );
}

static void EmitBytes( const OpCode opcode1, const OpCode opcode2 )
{
    EmitByte( opcode1 );
    EmitByte( opcode2 );
}

static void EmitReturn()
{
    EmitByte( OpCode::Return );
}

[[nodiscard]] static std::uint8_t MakeConstant( const Value value )
{
    const auto constantIndex = CurrentChunk().AddConstant( value );

    if ( constantIndex > std::numeric_limits<std::uint8_t>::max() )
    {
        Error( "Too many constants in one chunk." );
        return 0;
    }
    else
    {
        return static_cast<std::uint8_t>( constantIndex );
    }
}

static void EmitConstant( const Value value )
{
    EmitBytes( OpCode::Constant, MakeConstant( value ) );
}

static void EndCompiler()
{
    EmitReturn();

#ifdef DEBUG_PRINT_CODE
    if ( !parser.hadError )
    {
        debug::DisassembleChunk( CurrentChunk(), "code" );
    }
#endif
}

static void ParsePrecedence( const Precedence precedence )
{
    Advance();
    const auto& prefixRule = GetRule( parser.previous.type ).prefix;
    if ( prefixRule == nullptr )
    {
        Error( "Expect expression." );
        return;
    }

    prefixRule();

    while ( precedence <= GetRule( parser.current.type ).precedence )
    {
        Advance();
        const auto& infixRule = GetRule( parser.previous.type ).infix;
        if ( infixRule != nullptr )
        {
            infixRule();
        }
        else
        {
            Error( "Expect infix operator." );
            return;
        }
    }
}

static void Binary()
{
    const auto operatorType = parser.previous.type;
    const auto rulePrecedence = GetRule( operatorType ).precedence;
    ParsePrecedence( NextPrecedence( rulePrecedence ) );

    switch ( operatorType )
    {
    case TokenType::BangEqual:
        EmitBytes( OpCode::Equal, OpCode::Not );
        break;
    case TokenType::EqualEqual:
        EmitByte( OpCode::Equal );
        break;
    case TokenType::Greater:
        EmitByte( OpCode::Greater );
        break;
    case TokenType::GreaterEqual:
        EmitBytes( OpCode::Less, OpCode::Not );
        break;
    case TokenType::Less:
        EmitByte( OpCode::Less );
        break;
    case TokenType::LessEqual:
        EmitBytes( OpCode::Greater, OpCode::Not );
        break;
    case TokenType::Plus:
        EmitByte( OpCode::Add );
        break;
    case TokenType::Minus:
        EmitByte( OpCode::Subtract );
        break;
    case TokenType::Star:
        EmitByte( OpCode::Multiply );
        break;
    case TokenType::Slash:
        EmitByte( OpCode::Divide );
        break;
    default:
        Error( "Unknown binary operator." );
        return;
    }
}

static void Literal()
{
    const auto literalType = parser.previous.type;
    switch ( literalType )
    {
    case TokenType::Nil:
        EmitByte( OpCode::Nil );
        break;
    case TokenType::False:
        EmitByte( OpCode::False );
        break;
    case TokenType::True:
        EmitByte( OpCode::True );
        break;
    default:
        Error( "Unknown literal type." );
        break;
    }
}

static void Expression()
{
    ParsePrecedence( Precedence::Assignment );
}

static void Grouping()
{
    Expression();
    Consume( TokenType::RightParen, "Expect ')' after expression." );
}

static void Number()
{
    double doubleValue;
    auto [_, ec] = std::from_chars(
        parser.previous.lexeme.data(),
        parser.previous.lexeme.data() + parser.previous.lexeme.size(),
        doubleValue );
    if ( ec == std::errc() )
    {
        EmitConstant( Value{ doubleValue } );
    }
    else
    {
        ErrorAtCurrent( "Failed to parse as number." );
    }
}

static void Unary()
{
    const auto operatorType = parser.previous.type;
    ParsePrecedence( Precedence::Unary );

    switch ( operatorType )
    {
    case TokenType::Bang:
        EmitByte( OpCode::Not );
        break;
    case TokenType::Minus:
        EmitByte( OpCode::Negate );
        break;
    default:
        Error( "Unknown unary operator." );
        return;
    }
}

static std::unordered_map<TokenType, ParseRule> Rules = {
    { TokenType::LeftParen, { .prefix = &Grouping, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::RightParen, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::LeftBrace, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::RightBrace, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Comma, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Dot, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Minus, { .prefix = &Unary, .infix = &Binary, .precedence = Precedence::Term } },
    { TokenType::Plus, { .prefix = nullptr, .infix = &Binary, .precedence = Precedence::Term } },
    { TokenType::Semicolon, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Slash, { .prefix = nullptr, .infix = &Binary, .precedence = Precedence::Factor } },
    { TokenType::Star, { .prefix = nullptr, .infix = &Binary, .precedence = Precedence::Factor } },
    { TokenType::Bang, { .prefix = &Unary, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::BangEqual, { .prefix = nullptr, .infix = &Binary, .precedence = Precedence::Equality } },
    { TokenType::Equal, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::EqualEqual, { .prefix = nullptr, .infix = &Binary, .precedence = Precedence::Equality } },
    { TokenType::Greater, { .prefix = nullptr, .infix = &Binary, .precedence = Precedence::Comparison } },
    { TokenType::GreaterEqual, { .prefix = nullptr, .infix = &Binary, .precedence = Precedence::Comparison } },
    { TokenType::Less, { .prefix = nullptr, .infix = &Binary, .precedence = Precedence::Comparison } },
    { TokenType::LessEqual, { .prefix = nullptr, .infix = &Binary, .precedence = Precedence::Comparison } },
    { TokenType::Identifier, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::String, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Number, { .prefix = &Number, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::And, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Class, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Else, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::False, { .prefix = &Literal, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Fun, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::For, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::If, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Nil, { .prefix = &Literal, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Or, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Print, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Return, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Super, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::This, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::True, { .prefix = &Literal, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Var, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::While, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Error, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
    { TokenType::Eof, { .prefix = nullptr, .infix = nullptr, .precedence = Precedence::None } },
};

[[nodiscard]] static const ParseRule& GetRule( const TokenType type )
{
    return Rules.at( type );
}

} // namespace

namespace cpplox
{

std::optional<Chunk> Compile( const std::string_view source )
{
    scanner = Scanner{ source };
    compilingChunk = Chunk{};

    Advance();
    Expression();
    Consume( TokenType::Eof, "Expect end of expression." );
    EndCompiler();

    return parser.hadError ? std::nullopt : std::optional<Chunk>{ std::move( compilingChunk ) };
}

} // namespace cpplox
