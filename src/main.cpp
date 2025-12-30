#include "chunk.hpp"
#include "debug.hpp"
#include "vm.hpp"

#include <cstdint>

int main()
{
    using namespace cpplox;

    Chunk chunk;

    auto constantIndex = chunk.AddConstant( 1.2 );
    chunk.WriteChunk( OpCode::OP_CONSTANT, 123 );
    chunk.WriteChunk( static_cast<uint8_t>( constantIndex ), 123 );

    chunk.WriteChunk( OpCode::OP_RETURN, 123 );

    // debug::DisassembleChunk( chunk, "test chunk" );
    Interpret( &chunk );

    return 0;
}
