#pragma once

#include <algorithm>
#include <string_view>

namespace cpplox
{

enum class TokenType
{
    // Single-character tokens.
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    Comma,
    Dot,
    Minus,
    Plus,
    Semicolon,
    Slash,
    Star,

    // One or two character tokens.
    Bang,
    BangEqual,
    Equal,
    EqualEqual,
    Greater,
    GreaterEqual,
    Less,
    LessEqual,

    // Literals
    Identifier,
    String,
    Number,

    // Keywords
    And,
    Class,
    Else,
    False,
    Fun,
    For,
    If,
    Nil,
    Or,
    Print,
    Return,
    Super,
    This,
    True,
    Var,
    While,

    Error,
    Eof,
};

[[nodiscard]] constexpr std::string_view ToString( const TokenType type )
{
    switch ( type )
    {
    case TokenType::LeftParen:
        return "LeftParen";
    case TokenType::RightParen:
        return "RightParen";
    case TokenType::LeftBrace:
        return "LeftBrace";
    case TokenType::RightBrace:
        return "RightBrace";
    case TokenType::Comma:
        return "Comma";
    case TokenType::Dot:
        return "Dot";
    case TokenType::Minus:
        return "Minus";
    case TokenType::Plus:
        return "Plus";
    case TokenType::Semicolon:
        return "Semicolon";
    case TokenType::Slash:
        return "Slash";
    case TokenType::Star:
        return "Star";
    case TokenType::Bang:
        return "Bang";
    case TokenType::BangEqual:
        return "BangEqual";
    case TokenType::Equal:
        return "Equal";
    case TokenType::EqualEqual:
        return "EqualEqual";
    case TokenType::Greater:
        return "Greater";
    case TokenType::GreaterEqual:
        return "GreaterEqual";
    case TokenType::Less:
        return "Less";
    case TokenType::LessEqual:
        return "LessEqual";
    case TokenType::Identifier:
        return "Identifier";
    case TokenType::String:
        return "String";
    case TokenType::Number:
        return "Number";
    case TokenType::And:
        return "And";
    case TokenType::Class:
        return "Class";
    case TokenType::Else:
        return "Else";
    case TokenType::False:
        return "False";
    case TokenType::Fun:
        return "Fun";
    case TokenType::For:
        return "For";
    case TokenType::If:
        return "If";
    case TokenType::Nil:
        return "Nil";
    case TokenType::Or:
        return "Or";
    case TokenType::Print:
        return "Print";
    case TokenType::Return:
        return "Return";
    case TokenType::Super:
        return "Super";
    case TokenType::This:
        return "This";
    case TokenType::True:
        return "True";
    case TokenType::Var:
        return "Var";
    case TokenType::While:
        return "While";
    case TokenType::Error:
        return "Error";
    case TokenType::Eof:
        return "Eof";
    default:
        return "Unknown";
    }
}

struct Token
{
    TokenType type = TokenType::Eof;
    std::string_view lexeme;
    unsigned int line = 0;
};

class Scanner
{
private:
    std::string_view::const_iterator start;
    std::string_view::const_iterator current;
    std::string_view source;
    unsigned int current_line;

public:
    Scanner( const std::string_view source )
        : start( source.begin() )
        , current( source.begin() )
        , source( source )
        , current_line( 1 )
    {}

    Token ScanToken();

private:
    void SkipWhitespaceAndComments();
    [[nodiscard]] Token ScanString();
    [[nodiscard]] Token ScanNumber();
    [[nodiscard]] Token ScanIdentifier();
    [[nodiscard]] TokenType IdentifierType();
    [[nodiscard]] inline constexpr TokenType CheckKeyword(
        const unsigned int offset, std::string_view keyword, const TokenType type ) const
    {
        return std::equal( keyword.begin(), keyword.end(), start + offset, current ) ? type : TokenType::Identifier;
    }

    /**
     *  @brief  Checks whether the current character matches the expected character. If it does it
     *          advances the current position and returns true. Otherwise, it returns false and does
     *          not advance.
     */
    [[nodiscard]] bool Match( const char expectedCharacter );
    [[nodiscard]] inline char Peek() const;
    [[nodiscard]] inline char PeekNext() const;

    [[nodiscard]] inline constexpr bool IsAtEnd() const
    {
        return current == source.end();
    }

    [[nodiscard]] inline char Advance()
    {
        const auto character = Peek();
        current++;
        return character;
    }

    [[nodiscard]] inline Token MakeToken( const TokenType type ) const
    {
        return Token{
            .type = type,
            .lexeme = std::string_view{ start, current },
            .line = current_line,
        };
    }

    [[nodiscard]] inline Token ErrorToken( const std::string_view message ) const
    {
        return Token{
            .type = TokenType::Error,
            .lexeme = std::string_view{ message.begin(), message.end() },
            .line = current_line,
        };
    }
};

} // namespace cpplox
