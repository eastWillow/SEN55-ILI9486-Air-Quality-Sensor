# Capability: emulator-split

## ADDED Requirements

### Requirement: Emulator Modes
The emulator must support two distinct execution modes: a test mode for automated verification and an interactive mode for user exploration.

#### Scenario: Automated Test Execution
- **WHEN** the emulator is launched with the `--test` argument
- **THEN** it runs for a fixed number of frames (e.g., 25)
- **THEN** it saves the current screen content to `screenshot.bmp`
- **THEN** it terminates automatically with exit code 0

#### Scenario: User Interactive Session
- **WHEN** the emulator is launched without arguments
- **THEN** it runs indefinitely until manually closed
- **THEN** it accepts and processes mouse/keyboard input for interaction
