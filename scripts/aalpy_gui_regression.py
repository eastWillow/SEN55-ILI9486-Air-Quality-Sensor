#!/usr/bin/env python3
import os
import sys
import subprocess
import json

PROJECT_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
BUILD_DIR = os.path.join(PROJECT_ROOT, "build")
DRIVER_BIN = os.path.join(BUILD_DIR, "aalpy_sul_driver")
GOLDEN_SAMPLES_DIR = os.path.join(PROJECT_ROOT, "tests", "integration", "reference_screenshots")
GOLDEN_JSON = os.path.join(PROJECT_ROOT, "tests", "integration", "aalpy_golden_samples.json")
DOT_FILE = os.path.join(PROJECT_ROOT, "tests", "integration", "gui_state_machine.dot")

def compare_images(actual_path, expected_path):
    """Compare two images using ImageMagick compare CLI."""
    if not os.path.exists(expected_path):
        print(f"FAILED: Expected image missing {expected_path}")
        return False, "Reference missing"

    cmd = ["compare", "-metric", "AE", actual_path, expected_path, "null:"]
    res = subprocess.run(cmd, capture_output=True, text=True)
    stderr = res.stderr.strip()
    try:
        diff_pixels = int(stderr.split()[0]) if stderr else 0
        return diff_pixels == 0, f"Pixel diff: {diff_pixels}"
    except Exception as e:
        # Fallback to file byte comparison if compare output format differs
        with open(actual_path, "rb") as f1, open(expected_path, "rb") as f2:
            equal = f1.read() == f2.read()
            return equal, "Byte comparison match" if equal else "Byte comparison mismatch"

def main():
    if not os.path.exists(DRIVER_BIN):
        print(f"Driver binary not found at {DRIVER_BIN}", file=sys.stderr)
        sys.exit(1)

    if not os.path.exists(GOLDEN_JSON):
        print(f"Golden samples JSON not found at {GOLDEN_JSON}", file=sys.stderr)
        sys.exit(1)

    with open(GOLDEN_JSON, "r") as f:
        golden_catalog = json.load(f)

    screenshot_map = golden_catalog.get("state_screenshots", {})

    print("--- Starting AALpy Model-Based GUI Regression Test ---")

    # Test sequences to cover all states and transitions in the learned model
    test_sequences = [
        ("MAIN State Check", [], "MAIN"),
        ("Transition to INFO", ["press_info"], "INFO"),
        ("Return to MAIN from INFO", ["press_info", "press_back"], "MAIN"),
        ("Transition to TREND", ["press_trend"], "TREND"),
        ("Return to MAIN from TREND", ["press_trend", "press_back"], "MAIN"),
        ("Self loop tick in MAIN", ["tick"], "MAIN"),
        ("Self loop tick in INFO", ["press_info", "tick"], "INFO"),
        ("Self loop tick in TREND", ["press_trend", "tick"], "TREND"),
    ]

    all_passed = True

    for test_name, actions, expected_state in test_sequences:
        cmd_args = [DRIVER_BIN, "reset"]
        for act in actions:
            if act == "press_info":
                cmd_args.extend(["click", "435", "25"])
            elif act == "press_trend":
                cmd_args.extend(["click", "355", "25"])
            elif act == "press_back":
                cmd_args.extend(["click", "45", "295"])
            elif act == "tick":
                cmd_args.extend(["tick", "5"])

        # Query state
        state_args = list(cmd_args) + ["state"]
        res = subprocess.run(state_args, capture_output=True, text=True, check=True)
        actual_state = res.stdout.strip().splitlines()[-1] if res.stdout.strip() else "UNKNOWN"

        if actual_state != expected_state:
            print(f"[FAIL] {test_name}: State mismatch. Expected {expected_state}, got {actual_state}")
            all_passed = False
            continue

        # Save actual screenshot and compare against Golden Sample
        golden_file = screenshot_map.get(expected_state)
        if not golden_file:
            print(f"[FAIL] {test_name}: No golden sample defined for state {expected_state}")
            all_passed = False
            continue

        golden_path = os.path.join(GOLDEN_SAMPLES_DIR, golden_file)
        actual_ss_path = os.path.join(GOLDEN_SAMPLES_DIR, f"actual_regression_{expected_state.lower()}.bmp")

        ss_args = list(cmd_args) + ["screenshot", actual_ss_path]
        subprocess.run(ss_args, capture_output=True, check=True)

        match, details = compare_images(actual_ss_path, golden_path)

        # Cleanup actual screenshot on success
        if match:
            if os.path.exists(actual_ss_path):
                os.remove(actual_ss_path)
            print(f"[PASS] {test_name} -> State: {actual_state} | Screenshot match ({details})")
        else:
            print(f"[FAIL] {test_name} -> State: {actual_state} | Screenshot mismatch ({details})")
            all_passed = False

    print("-----------------------------------------------------")
    if all_passed:
        print("ALL AALpy GUI Regression Tests PASSED!")
        sys.exit(0)
    else:
        print("AALpy GUI Regression Tests FAILED!")
        sys.exit(1)

if __name__ == "__main__":
    main()
