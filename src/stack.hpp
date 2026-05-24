#pragma once

#include <vector>
#include <stdexcept>

namespace cpplox
{

template<class T>
class Stack
{
private:
    std::vector<T> m_elements;

public:
    [[nodiscard]] bool empty() const noexcept
    {
        return m_elements.empty();
    }

    [[nodiscard]] size_t size() const noexcept
    {
        return m_elements.size();
    }

    [[nodiscard]] const T& top() const noexcept
    {
        return m_elements.back();
    }

    void push( const T& value )
    {
        m_elements.push_back( value );
    }

    void pop() noexcept
    {
        m_elements.pop_back();
    }

    [[nodiscard]] const T& peek( const size_t distance ) const
    {
        if ( distance >= m_elements.size() )
        {
            throw std::out_of_range( "Stack::peek: distance is out of range" );
        }

        return m_elements.at( m_elements.size() - 1 - distance );
    }
};

} // namespace cpplox
