#include "value.hpp"
#include "object.hpp"

#include <cstring>

namespace cpplox
{

static std::string ToString( const Obj* pObj )
{
    switch ( pObj->Type )
    {
    case ObjectType::String:
        return static_cast<const ObjString*>( pObj )->Chars;
        break;

    default:
        return "Unknown Object Type";
    }
}

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
            else if ( std::holds_alternative<Obj*>( val ) )
            {
                return ToString( AsObj( val ) );
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

bool IsNumber( const Value& value )
{
    return std::holds_alternative<double>( value );
}

bool IsObj( const Value& value )
{
    return std::holds_alternative<Obj*>( value );
}

bool AsBool( const Value& value )
{
    return std::get<bool>( value );
}

double AsNumber( const Value& value )
{
    return std::get<double>( value );
}

Obj* AsObj( const Value& value )
{
    return std::get<Obj*>( value );
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
            else if ( std::holds_alternative<Obj*>( lhs ) )
            {
                if ( IsString( lhs ) && IsString( rhs ) )
                {
                    const auto* pLhs = AsString( lhs );
                    const auto* pRhs = AsString( rhs );
                    return pLhs->Length == pRhs->Length && std::memcmp( pLhs->Chars, pRhs->Chars, pLhs->Length ) == 0;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        },
        lhs );
}

} // namespace cpplox
