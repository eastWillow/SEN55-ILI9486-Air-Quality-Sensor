---
description: Run the full "Invent Loop" to create a new feature or change, from exploration to commit.
---

This workflow implements the "Invent Loop" described in README.md.

1. **Explore the Task**
   Start by understanding the scope and requirements.
   - Run: `/opsx-explore` or `/opsx-new` to begin a new change.

2. **Check Test Coverage**
   Verify if existing tests cover the new feature.
   - If **Yes**: Proceed to Step 3 (Discuss).
   - If **No**:
     1. **Generate Test**: Create a new test case for the feature.
        - *Note: You can use the parallel agent script:*
          ```bash
          ./.agent/scripts/run_gemini_parallel.sh "Generate unit tests for <feature>"
          ```
     2. **Commit Test**: Commit the test case once generated.
     3. Repeat Step 1 (Explore) or proceed to Step 3.

3. **Discuss & Context Check**
   Ensure the agent understands the task and has enough context.
   - Run: `/opsx-continue` to discuss the implementation plan.
   - Ask: "Do you have enough context to proceed?"
   - If **Yes**: Proceed to Step 5 (Gen Code).
   - If **No**:
     - **Teach**: Provide more context or explanation.
     - Repeat Step 3.

5. **Generate Code**
   Implement the feature.
   - Run: `/opsx-apply` to generate the code based on the plan.

6. **Verify**
   Check if the implementation meets the requirements.
   - Run: `/opsx-verify` to run tests and verification steps.
   - If **OK**: Proceed to Step 7 (Commit).
   - If **Not OK**:
     - **Teach Spec**: Update the specification or provide feedback.
     - Repeat Step 5 (Gen Code).

7. **Commit**
   Finalize the change.
   - Run: `/opsx-sync` to sync specs.
   - Run: `/opsx-archive` to archive the change.
   - Run: `git commit -am "feat: <feature_name>"` to commit the changes.
