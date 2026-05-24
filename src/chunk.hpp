#pragma once

#include "value.hpp"

#include <cstdint>
#include <vector>
#include <string_view>
#include <utility>

namespace cpplox
{

enum class OpCode : std::uint8_t
{
    Constant,
    Nil,
    True,
    False,
    Equal,
    Greater,
    Less,
    Add,
    Subtract,
    Multiply,
    Divide,
    Not,
    Negate,
    Return,
};

[[nodiscard]] constexpr std::string_view ToString( const OpCode opcode )
{
    switch ( opcode )
    {
    case OpCode::Constant:
        return "Constant";
    case OpCode::Nil:
        return "nil";
    case OpCode::True:
        return "true";
    case OpCode::False:
        return "false";
    case OpCode::Equal:
        return "Equal";
    case OpCode::Greater:
        return "Greater";
    case OpCode::Less:
        return "Less";
    case OpCode::Add:
        return "Add";
    case OpCode::Subtract:
        return "Subtract";
    case OpCode::Multiply:
        return "Multiply";
    case OpCode::Divide:
        return "Divide";
    case OpCode::Not:
        return "Not";
    case OpCode::Negate:
        return "Negate";
    case OpCode::Return:
        return "Return";
    default:
        return "Unknown opcode";
    }
}

struct Chunk
{
    std::vector<uint8_t> code;
    std::vector<int> lines;
    ValueArray constants;

    constexpr void WriteChunk( const OpCode opcode, const int line )
    {
        code.push_back( std::to_underlying( opcode ) );
        lines.push_back( line );
    }

    constexpr void WriteChunk( const uint8_t byte, const int line )
    {
        code.push_back( byte );
        lines.push_back( line );
    }

    constexpr size_t AddConstant( const Value value )
    {
        constants.values.push_back( value );
        return constants.values.size() - 1;
    }
};

} // namespace cpplox
