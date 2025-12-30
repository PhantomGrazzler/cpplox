#pragma once

#include "chunk.hpp"

#include <string_view>

namespace debug
{

void DisassembleChunk( const Chunk& chunk, const std::string_view name );
[[nodiscard]] size_t DisassembleInstruction( const Chunk& chunk, size_t offset );

} // namespace debug
