import mod;
#include <iostream>
#include <stdexec/execution.hpp>
#include "exec/static_thread_pool.hpp"

using namespace stdexec;

sender auto create(sender auto begin)
{
    sender auto first = then(
        begin,
        [&](int v) {
            return v + 1;
        });
    sender auto second = then(
        first,
        [](int v) {
            return v * 100;
        });
    return second;
}

int main()
{
    exec::static_thread_pool ctx{ 8 };
    scheduler auto sch = ctx.get_scheduler();
    auto start = then(schedule(sch), []() { return 999; });
    auto [value] = sync_wait(create(start)).value();
    std::cout << value << "\n";
    return 0;
}