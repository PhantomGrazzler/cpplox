#pragma once

#include "chunk.hpp"

#include <string_view>
#include <optional>

namespace cpplox
{

std::optional<Chunk> Compile( const std::string_view source );

} // namespace cpplox
