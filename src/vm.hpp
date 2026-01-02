#pragma once

#include "chunk.hpp"
#include "value.hpp"

#include <stack>

namespace cpplox
{

enum class InterpretResult
{
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
};

InterpretResult Interpret( Chunk* pChunk );

struct VM
{
    Chunk* pChunk = nullptr;
    // Using an index here instead of a pointer directly into the chunk's code may be a decision
    // that I regret later. From Crafting Interpreters it states that using a pointer is faster than
    // indexing into an array. Maybe the compiler will be able to optimize this well enough?
    //
    // TODO: Can we use std::span instead to have pointer-like behaviour while keeping safety?
    size_t instructionIndex = 0;
    std::stack<Value> stack;

    [[nodiscard]] Value PopValue()
    {
        const auto value = stack.top();
        stack.pop();
        return value;
    }
};

} // namespace cpplox
