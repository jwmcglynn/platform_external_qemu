#include "third_party/darwinn/port/default/cleanup.h"

#include <functional>
#include <string>
#include <type_traits>

#include "testing/base/public/gmock.h"
#include "testing/base/public/gunit.h"

namespace platforms {
namespace darwinn {
namespace {

using AnyCleanup = gtl::Cleanup<std::function<void()>>;

template <typename T1, typename T2>
void AssertTypeEq() {
  static_assert(std::is_same<T1, T2>::value, "unexpected type");
}

TEST(CleanupTest, BasicLambda) {
  std::string s = "active";
  {
    auto s_cleaner = gtl::MakeCleanup([&s] { s.assign("cleaned"); });
    EXPECT_EQ("active", s);
  }
  EXPECT_EQ("cleaned", s);
}

TEST(FinallyTest, NoCaptureLambda) {
  // Noncapturing lambdas are just structs and use aggregate initializers.
  // Make sure MakeCleanup is compatible with that kind of initialization.
  static std::string& s = *new std::string;
  s.assign("active");
  {
    auto s_cleaner = gtl::MakeCleanup([] { s.append(" clean"); });
    EXPECT_EQ("active", s);
  }
  EXPECT_EQ("active clean", s);
}

TEST(CleanupTest, Release) {
  std::string s = "active";
  {
    auto s_cleaner = gtl::MakeCleanup([&s] { s.assign("cleaned"); });
    EXPECT_EQ("active", s);
    s_cleaner.release();
  }
  EXPECT_EQ("active", s);  // no cleanup should have occurred.
}

TEST(FinallyTest, TypeErasedWithoutFactory) {
  std::string s = "active";
  {
    AnyCleanup s_cleaner([&s]{ s.append(" clean"); });
    EXPECT_EQ("active", s);
  }
  EXPECT_EQ("active clean", s);
}

struct Appender {
  Appender(std::string* s, const std::string& msg)
      : s_(s), msg_(msg) {}
  void operator()() const { s_->append(msg_); }
  std::string* s_;
  std::string msg_;
};

TEST(CleanupTest, NonLambda) {
  std::string s = "active";
  {
    auto c = gtl::MakeCleanup(Appender(&s, " cleaned"));
    AssertTypeEq<decltype(c), gtl::Cleanup<Appender>>();
    EXPECT_EQ("active", s);
  }
  EXPECT_EQ("active cleaned", s);
}

TEST(CleanupTest, Assign) {
  std::string s = "0";
  {
    auto clean1 = gtl::MakeCleanup(Appender(&s, " 1"));
    auto clean2 = gtl::MakeCleanup(Appender(&s, " 2"));
    EXPECT_EQ("0", s);
    clean2 = std::move(clean1);
    EXPECT_EQ("0 2", s);
  }
  EXPECT_EQ("0 2 1", s);
}

TEST(CleanupTest, AssignAny) {
  // Check that implicit conversions can happen in assignment.
  std::string s = "0";
  {
    auto clean1 = gtl::MakeCleanup(Appender(&s, " 1"));
    AnyCleanup clean2 = gtl::MakeCleanup(Appender(&s, " 2"));
    EXPECT_EQ("0", s);
    clean2 = std::move(clean1);
    EXPECT_EQ("0 2", s);
  }
  EXPECT_EQ("0 2 1", s);
}

TEST(CleanupTest, AssignFromReleased) {
  std::string s = "0";
  {
    auto clean1 = gtl::MakeCleanup(Appender(&s, " 1"));
    auto clean2 = gtl::MakeCleanup(Appender(&s, " 2"));
    EXPECT_EQ("0", s);
    clean1.release();
    clean2 = std::move(clean1);
    EXPECT_EQ("0 2", s);
  }
  EXPECT_EQ("0 2", s);
}

TEST(CleanupTest, AssignToReleased) {
  std::string s = "0";
  {
    auto clean1 = gtl::MakeCleanup(Appender(&s, " 1"));
    auto clean2 = gtl::MakeCleanup(Appender(&s, " 2"));
    EXPECT_EQ("0", s);
    clean2.release();
    EXPECT_EQ("0", s);
    clean2 = std::move(clean1);
    EXPECT_EQ("0", s);
  }
  EXPECT_EQ("0 1", s);
}

TEST(CleanupTest, AssignToDefaultInitialized) {
  std::string s = "0";
  {
    auto clean1 = gtl::MakeCleanup(Appender(&s, " 1"));
    {
      AnyCleanup clean2;
      EXPECT_EQ("0", s);
      clean2 = std::move(clean1);
      EXPECT_EQ("0", s);
    }
    EXPECT_EQ("0 1", s);
  }
  EXPECT_EQ("0 1", s);
}

class CleanupReferenceTest : public testing::Test {
 public:
  struct F {
    int* cp;
    int* i;
    F(int* cp, int* i) : cp(cp), i(i) {}
    F(const F& o) : cp(o.cp), i(o.i) { ++*cp; }
    F& operator=(const F& o) { cp = o.cp; i = o.i; ++*cp; return *this; }
    F(F&&) = default;
    F& operator=(F&&) = default;
    void operator()() const { ++*i; }
  };
  int copies_ = 0;
  int calls_ = 0;
  F f_ = F(&copies_, &calls_);

  static int g_calls;
  void SetUp() override { g_calls = 0; }
  static void CleanerFunction() { ++g_calls; }
};
int CleanupReferenceTest::g_calls = 0;

TEST_F(CleanupReferenceTest, FunctionPointer) {
  {
    auto c = gtl::MakeCleanup(&CleanerFunction);
    AssertTypeEq<decltype(c), gtl::Cleanup<void (*)()>>();
    EXPECT_EQ(0, g_calls);
  }
  EXPECT_EQ(1, g_calls);
  // Test that a function reference decays to a function pointer.
  {
    auto c = gtl::MakeCleanup(CleanerFunction);
    AssertTypeEq<decltype(c), gtl::Cleanup<void (*)()>>();
    EXPECT_EQ(1, g_calls);
  }
  EXPECT_EQ(2, g_calls);
}

TEST_F(CleanupReferenceTest, AssignLvalue) {
  std::string s = "0";
  Appender app1(&s, "1");
  Appender app2(&s, "2");
  {
    auto c = gtl::MakeCleanup(app1);
    c.release();
    c = gtl::MakeCleanup(app2);
    EXPECT_EQ("0", s);
    app1();
    EXPECT_EQ("01", s);
  }
  EXPECT_EQ("012", s);
}

TEST_F(CleanupReferenceTest, FunctorLvalue) {
  // Test that MakeCleanup(lvalue) produces Cleanup<F>, not Cleanup<F&>.
  EXPECT_EQ(0, copies_);
  EXPECT_EQ(0, calls_);
  {
    auto c = gtl::MakeCleanup(f_);
    AssertTypeEq<decltype(c), gtl::Cleanup<F>>();
    EXPECT_EQ(1, copies_);
    EXPECT_EQ(0, calls_);
  }
  EXPECT_EQ(1, copies_);
  EXPECT_EQ(1, calls_);
  {
    auto c = gtl::MakeCleanup(f_);
    EXPECT_EQ(2, copies_);
    EXPECT_EQ(1, calls_);
    F f2 = c.release();  // release is a move.
    EXPECT_EQ(2, copies_);
    EXPECT_EQ(1, calls_);
    auto c2 = gtl::MakeCleanup(f2);  // copy
    EXPECT_EQ(3, copies_);
    EXPECT_EQ(1, calls_);
  }
  EXPECT_EQ(3, copies_);
  EXPECT_EQ(2, calls_);
}

TEST_F(CleanupReferenceTest, FunctorRvalue) {
  {
    auto c = gtl::MakeCleanup(std::move(f_));
    AssertTypeEq<decltype(c), gtl::Cleanup<F>>();
    EXPECT_EQ(0, copies_);
    EXPECT_EQ(0, calls_);
  }
  EXPECT_EQ(0, copies_);
  EXPECT_EQ(1, calls_);
}

TEST_F(CleanupReferenceTest, FunctorReferenceWrapper) {
  {
    auto c = gtl::MakeCleanup(std::cref(f_));
    AssertTypeEq<decltype(c), gtl::Cleanup<std::reference_wrapper<const F>>>();
    EXPECT_EQ(0, copies_);
    EXPECT_EQ(0, calls_);
  }
  EXPECT_EQ(0, copies_);
  EXPECT_EQ(1, calls_);
}

}  // namespace
}  // namespace darwinn
}  // namespace platforms
