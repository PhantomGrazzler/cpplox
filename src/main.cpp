#include "chunk.hpp"
#include "vm.hpp"
#include "value.hpp"

#include <cstdint>
#include <print>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <filesystem>

namespace
{

static void PushConstant( cpplox::Chunk& chunk, const cpplox::Value value )
{
    auto constantIndex = chunk.AddConstant( value );
    chunk.WriteChunk( cpplox::OpCode::Constant, 1 );
    chunk.WriteChunk( static_cast<uint8_t>( constantIndex ), 1 );
}

void ExecuteTestChunks()
{
    using namespace cpplox;

    // -((1.2 + 3.4) / 5.6)
    {
        Chunk chunk;

        PushConstant( chunk, 1.2 );
        PushConstant( chunk, 3.4 );

        chunk.WriteChunk( OpCode::Add, 1 );

        PushConstant( chunk, 5.6 );

        chunk.WriteChunk( OpCode::Divide, 1 );
        chunk.WriteChunk( OpCode::Negate, 1 );

        chunk.WriteChunk( OpCode::Return, 2 );
        Interpret( &chunk );
    }

    // 1 * 2 + 3
    {
        Chunk chunk;

        PushConstant( chunk, 1 );
        PushConstant( chunk, 2 );

        chunk.WriteChunk( OpCode::Multiply, 1 );

        PushConstant( chunk, 3 );

        chunk.WriteChunk( OpCode::Add, 1 );

        chunk.WriteChunk( OpCode::Return, 2 );
        Interpret( &chunk );
    }

    // 1 + 2 * 3
    {
        Chunk chunk;

        PushConstant( chunk, 1 );
        PushConstant( chunk, 2 );
        PushConstant( chunk, 3 );

        chunk.WriteChunk( OpCode::Multiply, 1 );
        chunk.WriteChunk( OpCode::Add, 1 );

        chunk.WriteChunk( OpCode::Return, 2 );
        Interpret( &chunk );
    }

    // 1 + 2 * 3 - 4 / -5
    {
        Chunk chunk;

        PushConstant( chunk, 1 );
        PushConstant( chunk, 2 );
        PushConstant( chunk, 3 );

        chunk.WriteChunk( OpCode::Multiply, 1 );

        PushConstant( chunk, 4 );
        PushConstant( chunk, 5 );

        chunk.WriteChunk( OpCode::Negate, 1 );
        chunk.WriteChunk( OpCode::Divide, 1 );
        chunk.WriteChunk( OpCode::Subtract, 1 );
        chunk.WriteChunk( OpCode::Add, 1 );

        chunk.WriteChunk( OpCode::Return, 2 );
        Interpret( &chunk );
    }
}

static void Repl()
{
    std::println( "PhantomGrazzler's C++ Lox interpreter" );

    while ( true )
    {
        std::print( "> " );

        if ( std::string line; std::getline( std::cin, line ) )
        {
            if ( line == "exit" )
            {
                std::println( "Exiting..." );
                break;
            }

            const auto result = cpplox::Interpret( line );
            if ( result == cpplox::InterpretResult::CompileError )
            {
                std::println( "Compile error." );
            }
        }

        std::println();
    }
}

static void RunFile( const std::filesystem::path& path )
{
    using namespace cpplox;

    std::ifstream file{ path };
    if ( !file.is_open() )
    {
        std::println( stderr, "Failed to open file '{}'", path.string() );
        exit( 74 );
    }

    const std::string fileContents{ std::istreambuf_iterator<char>{ file }, {} };
    file.close();

    const auto result = Interpret( fileContents );

    if ( result == InterpretResult::CompileError )
    {
        exit( 65 );
    }
    if ( result == InterpretResult::RuntimeError )
    {
        exit( 70 );
    }
}

} // namespace

int main( const int argc, const char* argv[] )
{
    if ( argc == 1 )
    {
        Repl();
    }
    else if ( argc == 2 )
    {
        const auto path = std::filesystem::path{ argv[1] };
        std::println( "Executing file: {}", argv[1] );
        RunFile( path );
    }
    else
    {
        std::println( stderr, "Usage: cpplox.exe [script]" );
        std::exit( 64 );
    }

    return 0;
}
