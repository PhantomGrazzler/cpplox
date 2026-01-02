#include "chunk.hpp"
#include "vm.hpp"
#include "value.hpp"

#include <cstdint>

namespace
{

static void PushConstant( cpplox::Chunk& chunk, const cpplox::Value value )
{
    auto constantIndex = chunk.AddConstant( value );
    chunk.WriteChunk( cpplox::OpCode::OP_CONSTANT, 1 );
    chunk.WriteChunk( static_cast<uint8_t>( constantIndex ), 1 );
}

} // namespace

int main()
{
    using namespace cpplox;

    // -((1.2 + 3.4) / 5.6)
    {
        Chunk chunk;

        PushConstant( chunk, 1.2 );
        PushConstant( chunk, 3.4 );

        chunk.WriteChunk( OpCode::OP_ADD, 1 );

        PushConstant( chunk, 5.6 );

        chunk.WriteChunk( OpCode::OP_DIVIDE, 1 );
        chunk.WriteChunk( OpCode::OP_NEGATE, 1 );

        chunk.WriteChunk( OpCode::OP_RETURN, 2 );
        Interpret( &chunk );
    }

    // 1 * 2 + 3
    {
        Chunk chunk;

        PushConstant( chunk, 1 );
        PushConstant( chunk, 2 );

        chunk.WriteChunk( OpCode::OP_MULTIPLY, 1 );

        PushConstant( chunk, 3 );

        chunk.WriteChunk( OpCode::OP_ADD, 1 );

        chunk.WriteChunk( OpCode::OP_RETURN, 2 );
        Interpret( &chunk );
    }

    // 1 + 2 * 3
    {
        Chunk chunk;

        PushConstant( chunk, 1 );
        PushConstant( chunk, 2 );
        PushConstant( chunk, 3 );

        chunk.WriteChunk( OpCode::OP_MULTIPLY, 1 );
        chunk.WriteChunk( OpCode::OP_ADD, 1 );

        chunk.WriteChunk( OpCode::OP_RETURN, 2 );
        Interpret( &chunk );
    }

    // 1 + 2 * 3 - 4 / -5
    {
        Chunk chunk;

        PushConstant( chunk, 1 );
        PushConstant( chunk, 2 );
        PushConstant( chunk, 3 );

        chunk.WriteChunk( OpCode::OP_MULTIPLY, 1 );

        PushConstant( chunk, 4 );
        PushConstant( chunk, 5 );

        chunk.WriteChunk( OpCode::OP_NEGATE, 1 );
        chunk.WriteChunk( OpCode::OP_DIVIDE, 1 );
        chunk.WriteChunk( OpCode::OP_SUBTRACT, 1 );
        chunk.WriteChunk( OpCode::OP_ADD, 1 );

        chunk.WriteChunk( OpCode::OP_RETURN, 2 );
        Interpret( &chunk );
    }

    return 0;
}
