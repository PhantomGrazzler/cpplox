#include "vm.hpp"
#include "debug.hpp"
#include "value.hpp"
#include "chunk.hpp"
#include "compiler.hpp"

#include <print>
#include <cstdint>
#include <functional>
#include <string_view>

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
        case OpCode::Constant:
        {
            vm.stack.push( ReadConstant() );
            break;
        }

        case OpCode::Add:
        {
            vm.stack.push( BinaryOperation( vm, std::plus<>() ) );
            break;
        }

        case OpCode::Subtract:
        {
            vm.stack.push( BinaryOperation( vm, std::minus<>() ) );
            break;
        }

        case OpCode::Multiply:
        {
            vm.stack.push( BinaryOperation( vm, std::multiplies<>() ) );
            break;
        }

        case OpCode::Divide:
        {
            vm.stack.push( BinaryOperation( vm, std::divides<>() ) );
            break;
        }

        case OpCode::Negate:
        {
            vm.stack.push( -vm.PopValue() );
            break;
        }

        case OpCode::Return:
        {
            std::println( "{}", vm.PopValue() );
            return InterpretResult::Ok;
        }

        default:
            return InterpretResult::RuntimeError;
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

InterpretResult Interpret( const std::string_view source )
{
    auto chunk = Compile( source );
    if ( !chunk.has_value() )
    {
        return InterpretResult::CompileError;
    }

    vm.pChunk = &( *chunk );
    vm.instructionIndex = 0;

    return Run();
}

} // namespace cpplox
