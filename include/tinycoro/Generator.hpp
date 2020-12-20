//
// Created by Asmei on 11/21/2020.
//

#ifndef TINYCORO_GENERATOR_HPP
#define TINYCORO_GENERATOR_HPP
#include <coroutine>
#include <exception>
#include <variant>
namespace tinycoro
{
    /*
     * Coroutine type which supports co_yield keyword. Values are produced lazy and synchronously.
     *
     * Coroutine body cannot use co_await keyword.
     *
     * Any exception will be propagate to the user by begin() or operator++() functions.
     */
    template <typename T>
    class Generator
    {
    public:
        struct GeneratorPromiseType;
        using promise_type = GeneratorPromiseType;
        using promise_coro_handle = std::coroutine_handle<promise_type>;

        Generator() = default;
        Generator(promise_type& promise) : coroHandle(promise_coro_handle::from_promise(promise))
        {}

        ~Generator()
        {
            if(this->coroHandle)
            {
                this->coroHandle.destroy();
            }
        }
        Generator(Generator&& other) noexcept : coroHandle(std::exchange(other.coroHandle, {}))
        {}

        Generator& operator=(Generator&& other) noexcept
        {
            if(!this != other)
            {
                this->coroHandle = other.coroHandle;
                other.coroHandle = nullptr;
            }

            return *this;
        }

        Generator(const Generator&) = delete;
        Generator& operator=(const Generator&) = delete;

        struct Iterator;

        Iterator begin();
        Iterator end();

    private:
        explicit Generator(promise_coro_handle coroutine) noexcept : coroHandle(coroutine)
        {}
        promise_coro_handle coroHandle = nullptr;
    };

    template <typename T>
    struct Generator<T>::GeneratorPromiseType
    {
        std::variant<const T*, std::exception_ptr> value;

        Generator<T> get_return_object()
        {
            return Generator<T>{promise_coro_handle::from_promise(*this)};
        }

        std::suspend_always initial_suspend()
        {
            return {};
        }

        std::suspend_always final_suspend()
        {
            return {};
        }

        std::suspend_always yield_value(const T& valRef)
        {
            this->value = std::addressof(valRef);
            return {};
        }

        void return_void()
        {}

        template <typename Expression>
        Expression&& await_transform(Expression&& expression)
        {
            static_assert(sizeof(expression) == 0, "co_await is not supported in coroutines of type generator");
            return std::forward<Expression>(expression);
        }

        void unhandled_exception()
        {
            this->value = std::current_exception();
        }

        void rethrowExceptionIfExists()
        {
            if(auto exceptionPtr = std::get_if<std::exception_ptr>(&this->value))
            {
                std::rethrow_exception(*exceptionPtr);
            }
        }
    };

    template <typename T>
    struct Generator<T>::Iterator
    {
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        promise_coro_handle coroHandle;

        bool operator==(const Iterator& other) const
        {
            return this->coroHandle == other;
        }

        bool operator!=(const Iterator& other) const
        {
            return this->coroHandle != other.coroHandle;
        }

        Iterator& operator++()
        {
            this->coroHandle.resume();

            if(this->coroHandle.done())
            {
                this->coroHandle = nullptr;
            }

            return *this;
        }

        Iterator& operator++(int) = delete;

        const T& operator*() const
        {
            return *std::get<const T*>(this->coroHandle.promise().value);
        }

        const T* operator->() const
        {
            return std::addressof(operator*());
        }
    };

    template <typename T>
    typename Generator<T>::Iterator Generator<T>::begin()
    {
        if(not this->coroHandle)
        {
            return Iterator{nullptr};
        }

        this->coroHandle.resume();

        if(this->coroHandle.done())
        {
            this->coroHandle.promise().rethrowExceptionIfExists();
            return Iterator{nullptr};
        }

        return Iterator{this->coroHandle};
    }

    template <typename T>
    typename Generator<T>::Iterator Generator<T>::end()
    {
        return Iterator{nullptr};
    }

} // namespace tinycoro

#endif // TINYCORO_GENERATOR_HPP
