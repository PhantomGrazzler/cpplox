#include "vm.hpp"
#include "debug.hpp"
#include "value.hpp"
#include "chunk.hpp"
#include "compiler.hpp"

#include <print>
#include <cstdint>
#include <functional>
#include <string_view>
#include <expected>

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

static void RuntimeError( const std::string_view message )
{
    using namespace cpplox;
    std::println( "Runtime error: {}", message );
    std::println( "[line {}] in script", vm.pChunk->lines.at( vm.instructionIndex - 1 ) );
    vm.stack = {};
}

template<class TOperation>
std::expected<cpplox::Value, cpplox::InterpretResult> BinaryOperation( cpplox::VM& vm, TOperation&& op )
{
    if ( !std::holds_alternative<double>( vm.stack.peek( 0 ) ) ||
         !std::holds_alternative<double>( vm.stack.peek( 1 ) ) )
    {
        RuntimeError( "Operands must be numbers." );
        return std::unexpected( cpplox::InterpretResult::RuntimeError );
    }
    else
    {
        const auto rhs = std::get<double>( vm.PopValue() );
        const auto lhs = std::get<double>( vm.PopValue() );
        return op( lhs, rhs );
    }
}

[[nodiscard]] static bool IsFalsey( const cpplox::Value& value )
{
    return IsNil( value ) || ( IsBool( value ) && !AsBool( value ) );
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

        case OpCode::Nil:
        {
            vm.stack.push( Value{} );
            break;
        }

        case OpCode::True:
        {
            vm.stack.push( Value( true ) );
            break;
        }

        case OpCode::False:
        {
            vm.stack.push( Value( false ) );
            break;
        }

        case OpCode::Equal:
        {
            const auto rhs = vm.PopValue();
            const auto lhs = vm.PopValue();
            vm.stack.push( AsBool( ValuesEqual( lhs, rhs ) ) );
            break;
        }

        case OpCode::Greater:
        {
            if ( const auto result = BinaryOperation( vm, std::greater<>() ); !result.has_value() )
            {
                return result.error();
            }
            else
            {
                vm.stack.push( result.value() );
            }
            break;
        }

        case OpCode::Less:
        {
            if ( const auto result = BinaryOperation( vm, std::less<>() ); !result.has_value() )
            {
                return result.error();
            }
            else
            {
                vm.stack.push( result.value() );
            }
            break;
        }

        case OpCode::Add:
        {
            if ( const auto result = BinaryOperation( vm, std::plus<>() ); !result.has_value() )
            {
                return result.error();
            }
            else
            {
                vm.stack.push( result.value() );
            }
            break;
        }

        case OpCode::Subtract:
        {
            if ( const auto result = BinaryOperation( vm, std::minus<>() ); !result.has_value() )
            {
                return result.error();
            }
            else
            {
                vm.stack.push( result.value() );
            }
            break;
        }

        case OpCode::Multiply:
        {
            if ( const auto result = BinaryOperation( vm, std::multiplies<>() ); !result.has_value() )
            {
                return result.error();
            }
            else
            {
                vm.stack.push( result.value() );
            }
            break;
        }

        case OpCode::Divide:
        {
            if ( const auto result = BinaryOperation( vm, std::divides<>() ); !result.has_value() )
            {
                return result.error();
            }
            else
            {
                vm.stack.push( result.value() );
            }
            break;
        }

        case OpCode::Not:
        {
            vm.stack.push( Value( IsFalsey( vm.PopValue() ) ) );
            break;
        }

        case OpCode::Negate:
        {
            if ( const auto& topValue = vm.stack.top(); !std::holds_alternative<double>( topValue ) )
            {
                RuntimeError( "Operand must be a number." );
                return InterpretResult::RuntimeError;
            }
            else
            {
                vm.stack.push( -std::get<double>( vm.PopValue() ) );
            }
            break;
        }

        case OpCode::Return:
        {
            std::println( "{}", ToString( vm.PopValue() ) );
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
