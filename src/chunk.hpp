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
    OP_CONSTANT,
    OP_RETURN,
};

[[nodiscard]] constexpr std::string_view ToString( const OpCode opcode )
{
    switch ( opcode )
    {
    case OpCode::OP_RETURN:
        return "OP_RETURN";
    case OpCode::OP_CONSTANT:
        return "OP_CONSTANT";
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
