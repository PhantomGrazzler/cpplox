#include "debug.hpp"

#include <print>

namespace
{

[[nodiscard]] size_t SimpleInstruction( const cpplox::OpCode opcode, const size_t offset )
{
    std::println( "{}", ToString( opcode ) );
    return offset + 1;
}

[[nodiscard]] size_t ConstantInstruction(
    const cpplox::OpCode opcode, const cpplox::Chunk& chunk, const size_t offset )
{
    const auto constantIndex = chunk.code.at( offset + 1 );
    std::println(
        "{:16} {:4} '{}'",
        ToString( opcode ),
        constantIndex,
        chunk.constants.values.at( constantIndex ) );

    return offset + 2;
}

} // namespace

namespace cpplox::debug
{

void DisassembleChunk( const Chunk& chunk, const std::string_view name )
{
    std::println( "== {} ==", name );
    for ( size_t offset = 0; offset < chunk.code.size(); )
    {
        offset = DisassembleInstruction( chunk, offset );
    }
}

size_t DisassembleInstruction( const Chunk& chunk, const size_t offset )
{
    std::print( "{:04} ", offset );
    if ( offset > 0 && chunk.lines.at( offset ) == chunk.lines.at( offset - 1 ) )
    {
        std::print( "   | " );
    }
    else
    {
        std::print( "{:4} ", chunk.lines.at( offset ) );
    }

    const auto instructionByte = chunk.code.at( offset );
    const auto instruction = static_cast<OpCode>( instructionByte );

    switch ( instruction )
    {
    case OpCode::OP_RETURN:
        return SimpleInstruction( instruction, offset );
    case OpCode::OP_CONSTANT:
        return ConstantInstruction( instruction, chunk, offset );
    default:
        std::println( "Unknown opcode {}", instructionByte );
        return offset + 1;
    }
}

} // namespace cpplox::debug
