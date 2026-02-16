#ifndef MutexHelpersHPP
#define MutexHelpersHPP

//-----------------------------------------------------------------------------
#include <mutex>

#if !defined(HGL_NO_SHARED_MUTEX)
#include <shared_mutex>
#endif

//-----------------------------------------------------------------------------
#define STD_LOCK_GUARD(m) std::lock_guard<decltype(m)> lock(m)

#define STD_UNIQUE_LOCK(m) std::unique_lock<decltype(m)> lock(m)

#if !defined(HGL_NO_SHARED_MUTEX)
#define STD_SHARED_LOCK(m) std::shared_lock<decltype(m)> lock(m)
#endif

#define STD_SCOPED_LOCK(...) std::scoped_lock lock(__VA_ARGS__)

//-----------------------------------------------------------------------------
#endif //MutexHelpersHPP
