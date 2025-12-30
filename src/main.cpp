#include "chunk.hpp"
#include "debug.hpp"

int main()
{
    Chunk chunk;
    chunk.code.push_back( OpCode::OP_RETURN );
    debug::DisassembleChunk( chunk, "test chunk" );

    return 0;
}
