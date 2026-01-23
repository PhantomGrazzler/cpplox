#pragma once

#include "chunk.hpp"

#include <string_view>

// #define DEBUG_TRACE_EXECUTION
#define DEBUG_PRINT_CODE

namespace cpplox::debug
{

void DisassembleChunk( const Chunk& chunk, const std::string_view name );
[[nodiscard]] size_t DisassembleInstruction( const Chunk& chunk, const size_t offset );

} // namespace cpplox::debug
