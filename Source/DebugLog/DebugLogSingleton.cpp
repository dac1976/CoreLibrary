#include "DebugLogSingleton.h"

#if defined(CORE_LIB_NO_LOKI)

#include <memory>

namespace core_lib
{
namespace log
{

static std::unique_ptr<default_log_t> g_logSingleton{};

} // namespace log
} // namespace core_lib

core_lib::log::default_log_t& DebugLogInstance()
{
    if (!core_lib::log::g_logSingleton)
    {
        core_lib::log::g_logSingleton.reset(new core_lib::log::default_log_t());
    }

    return *core_lib::log::g_logSingleton;
}

bool DebugLogExists()
{
    return nullptr != core_lib::log::g_logSingleton.get();
}

void DebugLogGracefulDelete()
{
    core_lib::log::g_logSingleton.reset();
}

#else // !defined(CORE_LIB_NO_LOKI)

#include <atomic>

namespace core_lib
{
namespace log
{

static std::atomic<bool> g_logSingletonExists = false;

} // namespace log
} // namespace core_lib

core_lib::log::default_log_t& DebugLogInstance()
{
    auto& ref                      = debug_singelton_t::Instance();
    core_lib::log::g_logSingletonExists = true;
    return ref;
}

bool DebugLogExists()
{
    return core_lib::log::g_logSingletonExists;
}

void DebugLogGracefulDelete()
{
    debug_singelton_deleter_t::GracefulDelete();
    core_lib::log::g_logSingletonExists = false;
}

#endif // CORE_LIB_NO_LOKI
