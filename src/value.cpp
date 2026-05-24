#include "value.hpp"

namespace cpplox
{

std::string ToString( const Value& value )
{
    return std::visit(
        []( const Value& val ) -> std::string {
            if ( std::holds_alternative<double>( val ) )
            {
                return std::to_string( std::get<double>( val ) );
            }
            else if ( std::holds_alternative<bool>( val ) )
            {
                return std::get<bool>( val ) ? "true" : "false";
            }
            else if ( std::holds_alternative<Nil>( val ) )
            {
                return "nil";
            }
            else
            {
                return "Unknown Value";
            }
        },
        value );
}

bool IsNil( const Value& value )
{
    return std::holds_alternative<Nil>( value );
}

bool IsBool( const Value& value )
{
    return std::holds_alternative<bool>( value );
}

bool AsBool( const Value& value )
{
    return std::get<bool>( value );
}

double AsNumber( const Value& value )
{
    return std::get<double>( value );
}

bool ValuesEqual( const Value& lhs, const Value& rhs )
{
    // Types must match for equality.
    if ( lhs.index() != rhs.index() )
    {
        return false;
    }

    return std::visit(
        [&rhs]( const Value& lhs ) {
            if ( std::holds_alternative<Nil>( lhs ) )
            {
                return true;
            }
            else if ( std::holds_alternative<bool>( lhs ) )
            {
                return AsBool( lhs ) == AsBool( rhs );
            }
            else if ( std::holds_alternative<double>( lhs ) )
            {
                return AsNumber( lhs ) == AsNumber( rhs );
            }
            else
            {
                return false;
            }
        },
        lhs );
}

} // namespace cpplox
