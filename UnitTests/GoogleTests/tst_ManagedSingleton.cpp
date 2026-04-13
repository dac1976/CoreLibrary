
#ifndef DISABLE_MANAGED_SINGLETON_TESTS

#include "Singleton/ManagedSingleton.hpp"
#include <atomic>
#include <thread>
#include <vector>
#include "gtest/gtest.h"

namespace
{

// -----------------------------------------------------------------------------
// Test helper types
// -----------------------------------------------------------------------------

struct DefaultType
{
    int value {42};
};

struct ArgType
{
    explicit ArgType(int v)
    : value(v)
    {
    }

    ArgType()
    : value(-1)
    {
    }

    int value;
};

struct DestroyType
{
    DestroyType()
    : value(7)
    {
    }

    explicit DestroyType(int v)
    : value(v)
    {
    }

    int value;
};

struct TryType
{
    int value {99};
};

struct ThreadType
{
    ThreadType()
    {
        ++ctorCount;
    }

    static std::atomic<int> ctorCount;
    int value {1234};
};

std::atomic<int> ThreadType::ctorCount{0};

} // anonymous namespace

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

TEST(ManagedSingletonTest, TryInstanceReturnsNullBeforeCreation)
{
    using Singleton = core_lib::ManagedSingleton<TryType>;

    EXPECT_EQ(nullptr, Singleton::TryInstance());

    // Cleanup just in case a prior failed run left state around.
    Singleton::Destroy();
}

TEST(ManagedSingletonTest, InstanceCreatesObjectOnFirstUse)
{
    using Singleton = core_lib::ManagedSingleton<DefaultType>;

    DefaultType& instance = Singleton::Instance();

    EXPECT_EQ(42, instance.value);
    EXPECT_NE(nullptr, Singleton::TryInstance());

    Singleton::Destroy();
}

TEST(ManagedSingletonTest, InstanceReturnsSameObjectEachTime)
{
    struct SameType
    {
        int value {55};
    };

    using Singleton = core_lib::ManagedSingleton<SameType>;

    SameType& a = Singleton::Instance();
    SameType& b = Singleton::Instance();

    EXPECT_EQ(&a, &b);
    EXPECT_EQ(Singleton::TryInstance(), &a);

    Singleton::Destroy();
}

TEST(ManagedSingletonTest, FirstCallCanUseConstructorArguments)
{
    using Singleton = core_lib::ManagedSingleton<ArgType>;

    ArgType& instance = Singleton::Instance(123);

    EXPECT_EQ(123, instance.value);
    EXPECT_EQ(&instance, Singleton::TryInstance());

    Singleton::Destroy();
}

TEST(ManagedSingletonTest, SubsequentCallsIgnoreDifferentConstructorArguments)
{
    struct StickyArgType
    {
        StickyArgType()
        : value(-1)
        {
        }

        explicit StickyArgType(int v)
        : value(v)
        {
        }

        int value;
    };

    using Singleton = core_lib::ManagedSingleton<StickyArgType>;

    StickyArgType& first = Singleton::Instance(10);
    StickyArgType& second = Singleton::Instance(999);

    EXPECT_EQ(&first, &second);
    EXPECT_EQ(10, first.value);
    EXPECT_EQ(10, second.value);

    Singleton::Destroy();
}

TEST(ManagedSingletonTest, DestroyResetsLiveInstanceAndTryInstanceReturnsNull)
{
    using Singleton = core_lib::ManagedSingleton<DestroyType>;

    DestroyType& instance = Singleton::Instance(88);
    EXPECT_EQ(88, instance.value);
    EXPECT_NE(nullptr, Singleton::TryInstance());

    Singleton::Destroy();

    EXPECT_EQ(nullptr, Singleton::TryInstance());
}

TEST(ManagedSingletonTest, InstanceAfterDestroyReturnsFallbackObject)
{
    using Singleton = core_lib::ManagedSingleton<DestroyType>;

    DestroyType& original = Singleton::Instance(321);
    EXPECT_EQ(321, original.value);

    Singleton::Destroy();

    EXPECT_EQ(nullptr, Singleton::TryInstance());

    // After destroy, Instance() should return the fallback default-constructed object.
    DestroyType& fallback1 = Singleton::Instance();
    DestroyType& fallback2 = Singleton::Instance();

    EXPECT_EQ(7, fallback1.value);
    EXPECT_EQ(&fallback1, &fallback2);
    EXPECT_NE(&original, &fallback1);
    EXPECT_EQ(nullptr, Singleton::TryInstance());
}

TEST(ManagedSingletonTest, ConcurrentInstanceCallsStillYieldSingleObject)
{
    using Singleton = core_lib::ManagedSingleton<ThreadType>;

    ThreadType::ctorCount.store(0);

    constexpr int threadCount = 16;
    std::vector<std::thread> threads;
    std::vector<ThreadType*> results(threadCount, nullptr);

    for (int i = 0; i < threadCount; ++i)
    {
        threads.emplace_back([i, &results]()
        {
            results[i] = &Singleton::Instance();
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    ASSERT_NE(nullptr, results[0]);

    for (int i = 1; i < threadCount; ++i)
    {
        EXPECT_EQ(results[0], results[i]);
    }

    EXPECT_EQ(1, ThreadType::ctorCount.load());
    EXPECT_EQ(results[0], Singleton::TryInstance());

    Singleton::Destroy();
}

#endif // DISABLE_MANAGED_SINGLETON_TESTS