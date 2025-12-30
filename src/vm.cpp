#include "vm.hpp"
#include "debug.hpp"
#include "value.hpp"

#include <print>

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
        case OpCode::OP_RETURN:
        {
            std::println( "{}", vm.stack.top() );
            vm.stack.pop();
            return InterpretResult::INTERPRET_OK;
        }

        case OpCode::OP_CONSTANT:
        {
            vm.stack.push( ReadConstant() );
            break;
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
