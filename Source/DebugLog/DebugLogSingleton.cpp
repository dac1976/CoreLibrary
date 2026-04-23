#include "DebugLog/DebugLogSingleton.h"

#if defined(CORE_LIB_LOKI)
#include <atomic>

namespace core_lib
{
namespace log
{

static std::atomic<bool> g_logSingletonExists = false;

} // namespace log

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

} // namespace core_lib
#else
namespace core_lib
{

core_lib::log::default_log_t& DebugLogInstance()
{
    return debug_singelton_t::Instance();
}

bool DebugLogExists()
{
    return debug_singelton_t::TryInstance() != nullptr;
}

void DebugLogGracefulDelete()
{
    debug_singelton_t::Destroy();
}

} // namespace core_lib
#endif // CORE_LIB_LOKI
