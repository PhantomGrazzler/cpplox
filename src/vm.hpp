#pragma once

#include "chunk.hpp"
#include "value.hpp"
#include "stack.hpp"

#include <string_view>

namespace cpplox
{

enum class InterpretResult
{
    Ok,
    CompileError,
    RuntimeError,
};

InterpretResult Interpret( Chunk* pChunk );
InterpretResult Interpret( const std::string_view source );

struct VM
{
    Chunk* pChunk = nullptr;
    // Using an index here instead of a pointer directly into the chunk's code may be a decision
    // that I regret later. From Crafting Interpreters it states that using a pointer is faster than
    // indexing into an array. Maybe the compiler will be able to optimize this well enough?
    //
    // TODO: Can we use std::span instead to have pointer-like behaviour while keeping safety?
    size_t instructionIndex = 0;
    Stack<Value> stack;

    [[nodiscard]] Value PopValue()
    {
        const auto value = stack.top();
        stack.pop();
        return value;
    }
};

} // namespace cpplox
