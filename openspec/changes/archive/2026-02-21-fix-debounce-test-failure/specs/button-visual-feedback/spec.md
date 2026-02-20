## MODIFIED Requirements

### Requirement: Button Feedback Phase
#### Scenario: Visual Feedback on Info Button press
- **WHEN** user presses the "INFO" button
- **THEN** the transition MUST be deferred by exactly 100ms
- **AND** the performance tests MUST account for this delay to avoid false failures

### Requirement: Feedback Non-blocking but Deferred
#### Scenario: Sequential interactions
- **WHEN** a button is in feedback phase
- **THEN** the system MUST remain responsive and not block the execution loop for the 100ms duration
