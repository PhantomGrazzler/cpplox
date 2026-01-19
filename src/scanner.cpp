#include "scanner.hpp"

#include <string_view>
#include <cctype>

namespace
{

[[nodiscard]] static inline bool IsAlpha( const unsigned char character )
{
    return std::isalpha( character ) || character == '_';
}

} // namespace

namespace cpplox
{

Token Scanner::ScanToken()
{
    SkipWhitespaceAndComments();
    start = current;

    if ( IsAtEnd() )
    {
        return MakeToken( TokenType::Eof );
    }

    const auto character = Advance();
    if ( IsAlpha( character ) )
    {
        return ScanIdentifier();
    }
    if ( std::isdigit( character ) )
    {
        return ScanNumber();
    }

    switch ( character )
    {
    case '(':
        return MakeToken( TokenType::LeftParen );
    case ')':
        return MakeToken( TokenType::RightParen );
    case '{':
        return MakeToken( TokenType::LeftBrace );
    case '}':
        return MakeToken( TokenType::RightBrace );
    case ';':
        return MakeToken( TokenType::Semicolon );
    case ',':
        return MakeToken( TokenType::Comma );
    case '.':
        return MakeToken( TokenType::Dot );
    case '-':
        return MakeToken( TokenType::Minus );
    case '+':
        return MakeToken( TokenType::Plus );
    case '/':
        return MakeToken( TokenType::Slash );
    case '*':
        return MakeToken( TokenType::Star );
    case '!':
        return MakeToken( Match( '=' ) ? TokenType::BangEqual : TokenType::Bang );
    case '=':
        return MakeToken( Match( '=' ) ? TokenType::EqualEqual : TokenType::Equal );
    case '<':
        return MakeToken( Match( '=' ) ? TokenType::LessEqual : TokenType::Less );
    case '>':
        return MakeToken( Match( '=' ) ? TokenType::GreaterEqual : TokenType::Greater );
    case '"':
        return ScanString();
    default:
        return ErrorToken( "Unexpected character." );
    }
}

void Scanner::SkipWhitespaceAndComments()
{
    while ( true )
    {
        const auto character = Peek();
        switch ( character )
        {
        case '\0':
            return;
        case '\n':
            current_line++;
            [[fallthrough]];
        case ' ':
        case '\r':
        case '\t':
            current++;
            break;
        case '/':
            if ( PeekNext() == '/' )
            {
                // A comment goes until the end of the line.
                while ( Peek() != '\n' && !IsAtEnd() )
                {
                    current++;
                }
            }
            else
            {
                // We saw a '/' but it wasn't the start of a comment.
                return;
            }
            break;
        default:
            return;
        }
    }
}

Token Scanner::ScanString()
{
    while ( Peek() != '"' && !IsAtEnd() )
    {
        if ( Peek() == '\n' )
        {
            current_line++;
        }
        current++;
    }

    if ( IsAtEnd() )
    {
        return ErrorToken( "Unterminated string." );
    }

    current++;
    return MakeToken( TokenType::String );
}

Token Scanner::ScanNumber()
{
    while ( std::isdigit( Peek() ) )
    {
        current++;
    }

    if ( Peek() == '.' && std::isdigit( PeekNext() ) )
    {
        // Consume the ".".
        current++;
        while ( std::isdigit( Peek() ) )
        {
            current++;
        }
    }

    return MakeToken( TokenType::Number );
}

Token Scanner::ScanIdentifier()
{
    while ( IsAlpha( Peek() ) || std::isdigit( Peek() ) )
    {
        current++;
    }

    return MakeToken( IdentifierType() );
}

TokenType Scanner::IdentifierType()
{
    switch ( *start )
    {
    case 'a':
        return CheckKeyword( 1, "nd", TokenType::And );
    case 'c':
        return CheckKeyword( 1, "lass", TokenType::Class );
    case 'e':
        return CheckKeyword( 1, "lse", TokenType::Else );
    case 'f':
        if ( current - start > 1 )
        {
            switch ( *( start + 1 ) )
            {
            case 'a':
                return CheckKeyword( 2, "lse", TokenType::False );
            case 'o':
                return CheckKeyword( 2, "r", TokenType::For );
            case 'u':
                return CheckKeyword( 2, "n", TokenType::Fun );
            }
        }
        break;
    case 'i':
        return CheckKeyword( 1, "f", TokenType::If );
    case 'n':
        return CheckKeyword( 1, "il", TokenType::Nil );
    case 'o':
        return CheckKeyword( 1, "r", TokenType::Or );
    case 'p':
        return CheckKeyword( 1, "rint", TokenType::Print );
    case 'r':
        return CheckKeyword( 1, "eturn", TokenType::Return );
    case 's':
        return CheckKeyword( 1, "uper", TokenType::Super );
    case 't':
        if ( current - start > 1 )
        {
            switch ( *( start + 1 ) )
            {
            case 'h':
                return CheckKeyword( 2, "is", TokenType::This );
            case 'r':
                return CheckKeyword( 2, "ue", TokenType::True );
            }
        }
        break;
    case 'v':
        return CheckKeyword( 1, "ar", TokenType::Var );
    case 'w':
        return CheckKeyword( 1, "hile", TokenType::While );
    }

    return TokenType::Identifier;
}

bool Scanner::Match( const char expectedCharacter )
{
    if ( IsAtEnd() )
    {
        return false;
    }
    if ( *current != expectedCharacter )
    {
        return false;
    }

    current++;
    return true;
}

inline char Scanner::Peek() const
{
    if ( IsAtEnd() )
    {
        return '\0';
    }

    return *current;
}

inline char Scanner::PeekNext() const
{
    if ( IsAtEnd() || current + 1 == source.end() )
    {
        return '\0';
    }

    return *( current + 1 );
}

} // namespace cpplox
