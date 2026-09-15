#!/usr/bin/env python3
import os
import sys
import subprocess
import json
import hashlib
from PIL import Image

from aalpy.base import SUL
from aalpy.learning_algs import run_Lstar
from aalpy.oracles import RandomWMethodEqOracle
from aalpy.utils import save_automaton_to_file

# Paths
PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
BUILD_DIR = os.path.join(PROJECT_ROOT, "build")
DRIVER_BIN = os.path.join(BUILD_DIR, "aalpy_sul_driver")
GOLDEN_SAMPLES_DIR = os.path.join(PROJECT_ROOT, "tests", "integration", "reference_screenshots")
GOLDEN_JSON = os.path.join(PROJECT_ROOT, "tests", "integration", "aalpy_golden_samples.json")
DOT_OUTPUT = os.path.join(PROJECT_ROOT, "tests", "integration", "gui_state_machine")

class GUISUL(SUL):
    def __init__(self, driver_path, golden_dir):
        super().__init__()
        self.driver_path = driver_path
        self.golden_dir = golden_dir
        os.makedirs(self.golden_dir, exist_ok=True)
        self.current_sequence = []
        self.state_screenshot_map = {}

    def pre(self):
        """Reset the SUL state before starting a new query."""
        self.current_sequence = []

    def post(self):
        """Cleanup after a query sequence finishes."""
        pass

    def step(self, letter):
        """
        Execute an input action on the GUI SUL and return the output observation.
        letter: 'press_info', 'press_trend', 'press_back', 'tick'
        """
        self.current_sequence.append(letter)

        # Build execution args for the driver
        # We replay the entire sequence from reset to ensure deterministic, reproducible state transitions
        cmd_args = [self.driver_path, "reset"]
        for act in self.current_sequence:
            if act == "press_info":
                cmd_args.extend(["click", "435", "25"])
            elif act == "press_trend":
                cmd_args.extend(["click", "355", "25"])
            elif act == "press_back":
                cmd_args.extend(["click", "45", "295"])
            elif act == "tick":
                cmd_args.extend(["tick", "5"])

        cmd_args.extend(["state"])

        try:
            res = subprocess.run(cmd_args, capture_output=True, text=True, check=True)
            lines = res.stdout.strip().splitlines()
            state_str = lines[-1] if lines else "UNKNOWN"
        except Exception as e:
            print(f"Error running driver: {e}", file=sys.stderr)
            state_str = "ERROR"

        # Capture screenshot for golden sample if state is a known valid state
        if state_str in ["MAIN", "INFO", "TREND"]:
            golden_filename = f"aalpy_{state_str.lower()}.bmp"
            golden_path = os.path.join(self.golden_dir, golden_filename)
            if state_str not in self.state_screenshot_map:
                # Save screenshot
                ss_cmd = list(cmd_args)
                # replace 'state' at the end with screenshot command
                ss_cmd[-1] = "screenshot"
                ss_cmd.append(golden_path)
                subprocess.run(ss_cmd, capture_output=True, check=True)
                self.state_screenshot_map[state_str] = golden_filename

        return state_str


def main():
    if not os.path.exists(DRIVER_BIN):
        print(f"Driver binary not found at {DRIVER_BIN}. Please build the project first.", file=sys.stderr)
        sys.exit(1)

    print("Initializing AALpy GUI SUL...")
    sul = GUISUL(DRIVER_BIN, GOLDEN_SAMPLES_DIR)

    # Input alphabet for GUI interaction
    alphabet = ["press_info", "press_trend", "press_back", "tick"]

    print("Running L* Active Learning algorithm...")
    # Oracle for equivalence queries (Random W Method)
    eq_oracle = RandomWMethodEqOracle(alphabet=alphabet, sul=sul, walks_per_state=10, walk_len=5)

    # Run L* learning algorithm to learn Mealy machine
    learned_model = run_Lstar(
        alphabet=alphabet,
        sul=sul,
        eq_oracle=eq_oracle,
        automaton_type="mealy",
        print_level=1
    )

    print("\nState Machine Learning Complete!")
    print(f"Discovered States ({len(learned_model.states)}): {[s.state_id for s in learned_model.states]}")

    # Save DOT visualization and model
    save_automaton_to_file(learned_model, path=DOT_OUTPUT, file_type="dot")
    print(f"Saved state machine visualization to {DOT_OUTPUT}.dot")

    # Write golden samples mapping JSON
    golden_json_data = {
        "discovered_states": [s.state_id for s in learned_model.states],
        "state_screenshots": sul.state_screenshot_map
    }
    with open(GOLDEN_JSON, "w") as f:
        json.dump(golden_json_data, f, indent=2)
    print(f"Saved golden samples catalog to {GOLDEN_JSON}")


if __name__ == "__main__":
    main()
