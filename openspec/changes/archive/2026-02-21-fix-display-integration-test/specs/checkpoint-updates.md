## ADDED Requirements

### Requirement: Interactive Checkpoint Validation
The integration test suite must validate visual output for interactive elements, specifically button feedback and transitions to new screens (like the trend chart).

#### Scenario: Button Feedback Visual State
- **GIVEN** the application is in MAIN state
- **WHEN** any button (e.g., INFO or TREND) is pressed
- **THEN** the button background color changes to indicate feedback
- **AND** capturing a screenshot within 100ms of the press reflects this feedback state
- **AND** the screenshot matches the `feedback_info.bmp` reference

#### Scenario: Trend Chart Rendering
- **GIVEN** the application has recorded historical trend data
- **WHEN** the TREND state is entered
- **THEN** the trend chart renders correctly with data points and axes
- **AND** the screenshot matches the `trend_chart.bmp` reference

### Requirement: Baseline Maintenance for UI Enhancements
When intentional UI enhancements (like feedback states or layout fixes) are implemented, the integration test baselines must be updated to ensure continued validation of the improved state.

#### Scenario: Updating Reference Benchmarks
- **GIVEN** visual regressions in `CheckpointButtonFeedback` or `CheckpointChart` are confirmed as intentional UI improvements
- **WHEN** the "actual" screenshots are manually validated for correctness
- **THEN** they replace the dated reference images in `tests/integration/reference_screenshots/`
