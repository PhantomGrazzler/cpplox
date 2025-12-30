#pragma once

#include "value.hpp"

#include <cstdint>
#include <vector>
#include <string_view>
#include <utility>

enum class OpCode : std::uint8_t
{
    OP_CONSTANT,
    OP_RETURN,
};

constexpr [[nodiscard]] std::string_view ToString( const OpCode opcode )
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
    ValueArray constants;

    constexpr void WriteChunk( const OpCode opcode )
    {
        code.push_back( std::to_underlying( opcode ) );
    }

    constexpr void WriteChunk( const uint8_t byte )
    {
        code.push_back( byte );
    }

    constexpr size_t AddConstant( const Value value )
    {
        constants.values.push_back( value );
        return constants.values.size() - 1;
    }
};
