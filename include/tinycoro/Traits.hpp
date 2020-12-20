//
// Created by Asmei on 11/15/2020.
//

#ifndef TINYCORO_TRAITS_HPP
#define TINYCORO_TRAITS_HPP
#include <coroutine>

namespace tinycoro
{
    template <typename T, typename U>
    concept awaiter_trait = requires(T a)
    {
        {
            a.await_ready()
        }
        ->std::same_as<bool>;
        {
            a.await_resume()
        }
        ->std::convertible_to<U>;
    }
    &&(
        requires(T a) {
            {
                a.await_suspend(std::coroutine_handle<>{})
            }
            ->std::same_as<bool>;
        } ||
        requires(T a) {
            {
                a.await_suspend(std::coroutine_handle<>{})
            }
            ->std::same_as<void>;
        }
    );

    //Concept for scheduler.
    //Each scheduler must have a schedule method which returns a awaiter
    template <typename S>
    concept scheduler_trait = requires(S s)
    {
        {
            s.schedule()
        }
        ->awaiter_trait<void>;
    };

} // namespace tinycoro

#endif // TINYCORO_TRAITS_HPP
