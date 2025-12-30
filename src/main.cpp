#include "chunk.hpp"
#include "debug.hpp"

#include <cstdint>

int main()
{
    Chunk chunk;

    auto constantIndex = chunk.AddConstant( 1.2 );
    chunk.WriteChunk( OpCode::OP_CONSTANT );
    chunk.WriteChunk( static_cast<uint8_t>( constantIndex ) );

    chunk.WriteChunk( OpCode::OP_RETURN );

    debug::DisassembleChunk( chunk, "test chunk" );

    return 0;
}
