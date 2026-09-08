# Daisy development

When making changes to Daisy follow these guidelines

1. If at all possible make a C++ unit test that covers the implementation. In many cases this can be done using a coding AI.
2.
   * **No existing dai test:** If no dai system test covers the implementation, create one and generate baseline output. Review the output to ensure it is as expected.
   * **Existing dai test:** If an existing dai system test covers the implementation, you must compare the output and think. Which values change? Are the changes reasonable? If yes, update the baseline otherwise fix your implementation.
3. Ensure all tests pass locally. See [testing.md](testing.md)
4. Make a pull request against main. This will start automated cross-platform checks. All checks must pass.
5. If you want to create a new release, follow the guidelines at [making-a-release.md](making-a-release.md)
