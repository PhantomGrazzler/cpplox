#include "vm.hpp"
#include "object.hpp"

namespace
{

void AssignObjMembers( cpplox::Obj* pObj, cpplox::ObjectType type )
{
    pObj->Type = type;
    pObj->pNext = cpplox::vm.pObjects;
    cpplox::vm.pObjects = pObj;
}

[[nodiscard]] static cpplox::ObjString* AllocateString( char* chars, std::size_t length )
{
    auto* pString = new cpplox::ObjString();
    AssignObjMembers( pString, cpplox::ObjectType::String );
    pString->Chars = chars;
    pString->Length = length;

    using namespace cpplox;
    vm.strings.insert( pString );

    return pString;
}

} // namespace

namespace cpplox
{

[[nodiscard]] static auto FindInternedString( const char* chars, std::size_t length )
{
    return std::find_if( vm.strings.cbegin(), vm.strings.cend(), [chars, length]( const auto* pString ) {
        return pString->Length == length && std::memcmp( pString->Chars, chars, length ) == 0;
    } );
}

ObjString* TakeString( char* chars, std::size_t length )
{
    auto iter = FindInternedString( chars, length );
    if ( iter != vm.strings.cend() )
    {
        delete[] chars;
        return *iter;
    }

    return AllocateString( chars, length );
}

ObjString* CopyString( const char* chars, std::size_t length )
{
    // Check to see if we have this string already interned before attempting to allocate memory for it on the heap.
    auto iter = FindInternedString( chars, length );
    if ( iter != vm.strings.cend() )
    {
        return *iter;
    }

    char* heapChars = new char[length + 1];
    std::memcpy( heapChars, chars, length );
    heapChars[length] = '\0';
    return AllocateString( heapChars, length );
}

} // namespace cpplox
