#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <vector>

// Helper to execute command and return exit code
int RunCommand(const char *cmd) { return system(cmd); }

// Helper to read BMP file into buffer
std::vector<char> ReadFile(const char *filename) {
  std::ifstream file(filename, std::ios::binary);
  std::vector<char> buffer((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
  return buffer;
}

class EmulatorTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Clean up previous artifacts
    remove("screenshot.bmp");
  }

  void TearDown() override {
    // Optional cleanup
  }
};

TEST_F(EmulatorTest, RunsInTestModeAndProducesOutput) {
  // Run emulator in test mode
  int result = RunCommand("../DisplayEmulator --test");
  EXPECT_EQ(result, 0)
      << "Emulator failed to execute or returned non-zero exit code";

  // Verify screenshot exists
  std::ifstream screenshot("screenshot.bmp");
  EXPECT_TRUE(screenshot.good()) << "screenshot.bmp was not created";
}

TEST_F(EmulatorTest, OutputMatchesReference) {
  // Ensure reference exists
  std::ifstream reference("tests/reference/golden_screen.bmp");
  if (!reference.good()) {
    FAIL() << "Reference image not found at tests/reference/golden_screen.bmp. "
              "Run once to generate it.";
  }

  // Run emulator
  RunCommand("../DisplayEmulator --test");

  // Compare files
  auto actual = ReadFile("screenshot.bmp");
  auto expected = ReadFile("tests/reference/golden_screen.bmp");

  // Simple byte comparison for now.
  // In real world, we might want fuzzy comparison for rendering differences.
  ASSERT_EQ(actual.size(), expected.size()) << "File sizes differ";
  // Check first header bytes to confirm BMP
  if (actual.size() > 2) {
    EXPECT_EQ(actual[0], 'B');
    EXPECT_EQ(actual[1], 'M');
  }

  // Compare content
  // We allow some tolerance or just strict equality for this initial pass
  EXPECT_EQ(actual, expected) << "Screenshot content differs from reference";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
