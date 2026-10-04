#pragma once

#include "value.hpp"

namespace cpplox
{

enum class ObjectType
{
    String,
};

struct Obj
{
    ObjectType Type;
    Obj* pNext;
};

struct ObjString final : public Obj
{
    std::size_t Length;
    char* Chars;
};

/**
 *  @brief  Creates a new `ObjString` object that takes ownership of the supplied `chars` object.
 *  @param[in]  chars Pointer to a character string.
 *  @param[in]  length Length of the character string.
 */
[[nodiscard]] ObjString* TakeString( char* chars, std::size_t length );

/**
 *  @brief  Creates a new `ObjString` object that allocates new memory into which the supplied `chars` are copied.
 *  @param[in]  chars Pointer to a character string.
 *  @param[in]  length Length of the character string.
 */
[[nodiscard]] ObjString* CopyString( const char* chars, std::size_t length );

[[nodiscard]] static inline bool IsObjType( const Value& value, ObjectType type )
{
    return IsObj( value ) && AsObj( value )->Type == type;
}

[[nodiscard]] static inline bool IsString( const Value& value )
{
    return IsObjType( value, ObjectType::String );
}

[[nodiscard]] inline ObjString* AsString( const Value& value )
{
    return static_cast<ObjString*>( AsObj( value ) );
}

} // namespace cpplox
