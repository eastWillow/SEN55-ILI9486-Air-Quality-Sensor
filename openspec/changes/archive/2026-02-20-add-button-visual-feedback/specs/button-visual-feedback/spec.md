## ADDED Requirements

### Requirement: Button Feedback Phase
Interactive buttons MUST provide visual feedback for a fixed duration before triggering their action.

#### Scenario: Visual Feedback on Info Button press
- **WHEN** user presses the "INFO" button
- **THEN** the INFO button colors MUST be inverted (Background: BLUE, Text: WHITE)
- **AND** the application MUST maintain this state for at least 100ms
- **AND** the screen transition to Information screen MUST occur only after the 100ms duration

#### Scenario: Visual Feedback on Back Button press
- **WHEN** user presses the "BACK" button
- **THEN** the BACK button colors MUST be inverted (Background: BLUE, Text: WHITE)
- **AND** the application MUST maintain this state for at least 100ms
- **AND** the screen transition to Main screen MUST occur only after the 100ms duration
