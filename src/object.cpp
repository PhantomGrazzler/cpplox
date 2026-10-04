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

    return pString;
}

} // namespace

namespace cpplox
{

ObjString* TakeString( char* chars, std::size_t length )
{
    return AllocateString( chars, length );
}

ObjString* CopyString( const char* chars, std::size_t length )
{
    char* heapChars = new char[length + 1];
    std::memcpy( heapChars, chars, length );
    heapChars[length] = '\0';
    return AllocateString( heapChars, length );
}

} // namespace cpplox
