#!/bin/bash

# Check if tmux is installed
if ! command -v tmux &> /dev/null; then
    echo "Error: tmux is not installed. Please install tmux to use this feature."
    exit 1
fi

# Check arguments
if [ -z "$1" ]; then
    echo "Usage: $0 \"<prompt>\""
    exit 1
fi

PROMPT="$1"
OUTPUT_FILE="/tmp/gemini_output_$$"

# Define the command (using 'gemini' as the CLI command based on docs)
# We assume 'gemini' is in the PATH. 
CMD="gemini --prompt \"$PROMPT\""

# Run in tmux detached window
tmux new-window -d -n "gemini_agent" "($CMD) > $OUTPUT_FILE 2>&1 ; echo 'Done' >> $OUTPUT_FILE"

echo "Started gemini agent in background for prompt: \"$PROMPT\""
echo "Output will be available at: $OUTPUT_FILE"
echo "You can check status with: tmux list-windows"
