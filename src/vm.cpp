#include "vm.hpp"
#include "debug.hpp"
#include "value.hpp"
#include "chunk.hpp"

#include <print>
#include <cstdint>
#include <functional>

#define DEBUG_TRACE_EXECUTION

namespace cpplox
{
// Is there a way to reduce the scope of this variable while keeping the code readable?
VM vm;
} // namespace cpplox

namespace
{

static constexpr uint8_t ReadByte()
{
    using namespace cpplox;
    return vm.pChunk->code.at( vm.instructionIndex++ );
}

static constexpr cpplox::Value ReadConstant()
{
    using namespace cpplox;
    return vm.pChunk->constants.values.at( ReadByte() );
}

template<class TOperation>
cpplox::Value BinaryOperation( cpplox::VM& vm, TOperation&& op )
{
    const auto rhs = vm.PopValue();
    const auto lhs = vm.PopValue();
    return op( lhs, rhs );
}

static cpplox::InterpretResult Run()
{
    using namespace cpplox;
    while ( true )
    {
#ifdef DEBUG_TRACE_EXECUTION
        std::println( "          {}", vm.stack );
        (void)debug::DisassembleInstruction( *vm.pChunk, vm.instructionIndex );
#endif
        const auto instructionByte = ReadByte();
        const auto instruction = static_cast<OpCode>( instructionByte );
        switch ( instruction )
        {
        case OpCode::OP_CONSTANT:
        {
            vm.stack.push( ReadConstant() );
            break;
        }

        case OpCode::OP_ADD:
        {
            vm.stack.push( BinaryOperation( vm, std::plus<>() ) );
            break;
        }

        case OpCode::OP_SUBTRACT:
        {
            vm.stack.push( BinaryOperation( vm, std::minus<>() ) );
            break;
        }

        case OpCode::OP_MULTIPLY:
        {
            vm.stack.push( BinaryOperation( vm, std::multiplies<>() ) );
            break;
        }

        case OpCode::OP_DIVIDE:
        {
            vm.stack.push( BinaryOperation( vm, std::divides<>() ) );
            break;
        }

        case OpCode::OP_NEGATE:
        {
            vm.stack.push( -vm.PopValue() );
            break;
        }

        case OpCode::OP_RETURN:
        {
            std::println( "{}", vm.PopValue() );
            return InterpretResult::INTERPRET_OK;
        }

        default:
            return InterpretResult::INTERPRET_RUNTIME_ERROR;
        }
    }
}

} // namespace

namespace cpplox
{

InterpretResult Interpret( Chunk* pChunk )
{
    vm.pChunk = pChunk;
    vm.instructionIndex = 0;
    return Run();
}

} // namespace cpplox
