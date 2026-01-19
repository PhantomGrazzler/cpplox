#include "compiler.hpp"
#include "scanner.hpp"

#include <string_view>
#include <print>

namespace cpplox
{

void Compile( const std::string_view source )
{
    unsigned int line = 0u;
    Scanner scanner{ source };

    while ( true )
    {
        const auto token = scanner.ScanToken();

        if ( token.line != line )
        {
            std::print( "{:4} ", token.line );
            line = token.line;
        }
        else
        {
            std::print( "   | " );
        }

        std::println(
            "{} '{}'",
            ToString( token.type ),
            std::string_view{ token.start, token.end } );

        if ( token.type == TokenType::Eof )
        {
            break;
        }
    }
}

} // namespace cpplox
