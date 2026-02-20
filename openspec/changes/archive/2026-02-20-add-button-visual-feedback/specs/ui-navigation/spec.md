## MODIFIED Requirements

### Requirement: Feedback Non-blocking but Deferred
Feedback MUST NOT block the application loop (must remain responsive), but the resulting state transition MUST be deferred.

#### Scenario: Sequential interactions
- **WHEN** a button is in feedback phase
- **THEN** subsequent touch events on the same or other buttons MUST be ignored until the phase completes and the state transitions
