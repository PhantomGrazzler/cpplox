#pragma once

#include <vector>
#include <variant>
#include <string>

namespace cpplox
{

struct Nil
{};
using Value = std::variant<Nil, bool, double>;

[[nodiscard]] std::string ToString( const Value& value );
[[nodiscard]] bool IsNil( const Value& value );
[[nodiscard]] bool IsBool( const Value& value );
[[nodiscard]] bool AsBool( const Value& value );
[[nodiscard]] double AsNumber( const Value& value );
[[nodiscard]] bool ValuesEqual( const Value& lhs, const Value& rhs );

struct ValueArray
{
    std::vector<Value> values;
};

} // namespace cpplox
