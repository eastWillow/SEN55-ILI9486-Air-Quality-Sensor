## ADDED Requirements

### Requirement: Trend Chart Data Management
The system SHALL maintain a historical buffer of PM2.5 readings, updating sequentially.

#### Scenario: Storing periodic readings
- **WHEN** 10 seconds elapse
- **THEN** the system SHALL append the current PM2.5 reading to the historical buffer
- **AND** shift older readings to maintain a fixed capacity corresponding to the screen width

### Requirement: Trend Chart Visualization
The system SHALL display the stored PM2.5 data as a line chart on the Trend UI screen.

#### Scenario: Rendering the chart
- **WHEN** the user navigates to the Trend screen
- **THEN** the system SHALL draw a line chart connecting the data points in the buffer
- **AND** automatically scale the Y-axis based on the minimum and maximum values currently in the buffer
- **AND** clear the previous Y-axis label text to prevent visual artifacting (overlapping digits) during scale transitions

### Requirement: Y-axis Scale Stability Test
The system SHALL be verified for Y-axis rendering stability across multiple scaling operations.

#### Scenario: Verifying scale reset
- **WHEN** the Y-axis scales from a small range (e.g. 0-20) to a large range (0-100) and back to the original range
- **THEN** the pixels remaining on the screen MUST be identical to the initial state (no overlapping artifacts)
