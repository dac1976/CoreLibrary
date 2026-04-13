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

struct ReinitType
{
    ReinitType()
    : value(-1)
    {
    }

    explicit ReinitType(int v)
    : value(v)
    {
    }

    int value;
};

} // anonymous namespace

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

TEST(ManagedSingletonTest, TryInstanceReturnsNullBeforeCreation)
{
    using Singleton = core_lib::ManagedSingleton<TryType>;

    Singleton::Destroy();
    EXPECT_EQ(nullptr, Singleton::TryInstance());
}

TEST(ManagedSingletonTest, InstanceCreatesObjectOnFirstUse)
{
    using Singleton = core_lib::ManagedSingleton<DefaultType>;

    Singleton::Destroy();

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

    Singleton::Destroy();

    SameType& a = Singleton::Instance();
    SameType& b = Singleton::Instance();

    EXPECT_EQ(&a, &b);
    EXPECT_EQ(Singleton::TryInstance(), &a);

    Singleton::Destroy();
}

TEST(ManagedSingletonTest, FirstCallCanUseConstructorArguments)
{
    using Singleton = core_lib::ManagedSingleton<ArgType>;

    Singleton::Destroy();

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

    Singleton::Destroy();

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

    Singleton::Destroy();

    DestroyType& instance = Singleton::Instance(88);
    EXPECT_EQ(88, instance.value);
    EXPECT_NE(nullptr, Singleton::TryInstance());

    Singleton::Destroy();

    EXPECT_EQ(nullptr, Singleton::TryInstance());
}

TEST(ManagedSingletonTest, InstanceAfterDestroyRecreatesNewObject)
{
    using Singleton = core_lib::ManagedSingleton<ReinitType>;

    Singleton::Destroy();

    ReinitType& first = Singleton::Instance(321);
    EXPECT_EQ(321, first.value);
    EXPECT_EQ(&first, Singleton::TryInstance());

    Singleton::Destroy();

    EXPECT_EQ(nullptr, Singleton::TryInstance());

    ReinitType& second = Singleton::Instance(654);
    EXPECT_EQ(654, second.value);
    EXPECT_EQ(&second, Singleton::TryInstance());

    Singleton::Destroy();
}

TEST(ManagedSingletonTest, ReinitUsesNewConstructorArgumentsAfterDestroy)
{
    using Singleton = core_lib::ManagedSingleton<ReinitType>;

    Singleton::Destroy();

    ReinitType& first = Singleton::Instance(10);
    EXPECT_EQ(10, first.value);
    EXPECT_EQ(&first, Singleton::TryInstance());

    Singleton::Destroy();

    EXPECT_EQ(nullptr, Singleton::TryInstance());

    ReinitType& second = Singleton::Instance(42);
    EXPECT_EQ(42, second.value);
    EXPECT_EQ(&second, Singleton::TryInstance());

    Singleton::Destroy();
}

TEST(ManagedSingletonTest, TryInstanceReturnsRecreatedObjectAfterDestroyAndReinit)
{
    using Singleton = core_lib::ManagedSingleton<ReinitType>;

    Singleton::Destroy();

    ReinitType& first = Singleton::Instance(77);
    EXPECT_EQ(77, first.value);
    EXPECT_EQ(&first, Singleton::TryInstance());

    Singleton::Destroy();
    EXPECT_EQ(nullptr, Singleton::TryInstance());

    ReinitType& second = Singleton::Instance(88);
    EXPECT_EQ(88, second.value);
    EXPECT_EQ(&second, Singleton::TryInstance());

    Singleton::Destroy();
}

TEST(ManagedSingletonTest, ConcurrentInstanceCallsStillYieldSingleObject)
{
    using Singleton = core_lib::ManagedSingleton<ThreadType>;

    Singleton::Destroy();
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