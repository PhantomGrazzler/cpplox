#include "debug.hpp"

#include <print>

namespace
{

[[nodiscard]] size_t SimpleInstruction( const OpCode opcode, size_t offset )
{
    std::println( "{}", ToString( opcode ) );
    return offset + 1;
}

} // namespace

namespace debug
{

void DisassembleChunk( const Chunk& chunk, const std::string_view name )
{
    std::println( "== {} ==", name );
    for ( size_t offset = 0; offset < chunk.code.size(); )
    {
        offset = DisassembleInstruction( chunk, offset );
    }
}

size_t DisassembleInstruction( const Chunk& chunk, size_t offset )
{
    std::print( "{:04} ", offset );
    const OpCode instruction = chunk.code.at( offset );

    switch ( instruction )
    {
    case OpCode::OP_RETURN:
        return SimpleInstruction( instruction, offset );
    default:
        std::println( "Unknown opcode {}", static_cast<std::uint8_t>( instruction ) );
        return offset + 1;
    }
}

} // namespace debug
