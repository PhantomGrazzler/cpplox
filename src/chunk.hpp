#pragma once

#include <cstdint>
#include <vector>
#include <string_view>

enum class OpCode : std::uint8_t
{
    OP_RETURN,
};

constexpr std::string_view ToString( OpCode opcode )
{
    switch ( opcode )
    {
    case OpCode::OP_RETURN:
        return "OP_RETURN";
    default:
        return "Unknown opcode";
    }
}

struct Chunk
{
    std::vector<OpCode> code;
};
