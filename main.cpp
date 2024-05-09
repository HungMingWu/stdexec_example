#include <iostream>
#include <stdexec/execution.hpp>
#include <exec/task.hpp>
#include "exec/static_thread_pool.hpp"

using namespace stdexec;

namespace test {

    template <class _JustTag, class _SetTag>
    struct __just_impl {
        template <class _Sender>
        using __compl_sigs =     //
            completion_signatures< //
            __mapply<            //
            __qf<_SetTag>,
            __decay_t<__data_of<_Sender>>>>;

        int value = 0;
        static empty_env get_env(__ignore) noexcept {
            return {};
        }

        template <sender_expr_for<_JustTag> _Sender>
        static __compl_sigs<_Sender> get_completion_signatures(_Sender&&, __ignore) {
            return {};
        }

        template <class _Data, class _Receiver>
        static void start(_Data&& __data, _Receiver&& __rcvr) noexcept {
            __tup::__apply(
                [&]<class... _Ts>(_Ts&... __ts) noexcept {
                _SetTag()(std::move(__rcvr), std::move(__ts)...);
            },
                __data);
        }
    };

    struct just_t : __just_impl<just_t, set_value_t> {
        template <__movable_value... _Ts>
        auto operator()(_Ts&&... __ts) const noexcept((__nothrow_decay_copyable<_Ts> && ...)) {
            return __make_sexpr<just_t>(__tuple{ (_Ts&&)__ts... });
        }
    };
    inline constexpr just_t just{};
}

struct Recv {
    using receiver_concept = stdexec::receiver_t;

    friend void tag_invoke(set_value_t, Recv&&, int v) noexcept {
        std::cout << "get value: " << v << "\n";
    }
#if 0
    friend void tag_invoke(set_error_t, Recv&&, std::string) noexcept {
    }
#endif
#if 0
    friend void tag_invoke(set_stopped_t, recv&&) noexcept {
    }



    friend empty_env tag_invoke(get_env_t, const recv&) noexcept {
        return {};
    }
#endif
};

struct Send {
    template <typename R>
    struct op {
        R r_;
        friend void tag_invoke(stdexec::start_t, op& self) noexcept {
            stdexec::set_value(std::move(self.r_), 42);
        }
    };

    using sender_concept = stdexec::sender_t;
    using completion_signatures = stdexec::completion_signatures<stdexec::set_value_t(int)>;
    template <class R>
    friend op<R> tag_invoke(stdexec::connect_t, Send, R r) {
        std::cout << "Create connect\n";
        return { r };
    }
};

sender auto create(sender auto begin, int v)
{
    struct state {
        ~state() { std::cout << "state have been destruct\n"; }
    };
    //auto inner_state = std::make_shared<state>();
    sender auto zero = let_value(begin, [v] { 
        state state;
        return just(v); 
    });
    sender auto first = then(
        zero,
        [](int v) {
            return v + 1;
        });
    sender auto second = then(
        first,
        [](int v) {
            return v * 100;
        });
    return second;
}

#define A2(a1, a2) ((a1)+(a2))

#define A_VA(...) A2(__VA_ARGS__)

int main()
{
    printf("%d\n", A_VA(1, 2));
    static_assert(stdexec::sender<Send>);
    static_assert(stdexec::receiver<Recv>);
    auto sender1 = Send{};
    for (int i = 0; i < 5; i++) {
        sync_wait(sender1);
    }
    exec::static_thread_pool ctx{ 8 };
    scheduler auto sch = ctx.get_scheduler();
    auto inner_sender = create(schedule(sch), 999);
    std::cout << "start wait\n";
    for (int i = 0; i < 5; i++) {
        auto [value] = sync_wait(inner_sender).value();
        std::cout << value << "\n";
    }
    return 0;
}