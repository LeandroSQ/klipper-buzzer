import time
import gpiod
import random
import glob
import sys
from os import path

# Constants
GPIO_PIN = 64
GPIO_CHIP = "gpiochip0"
PITCH_OFFSET = 0

# Setup
chip = gpiod.Chip(GPIO_CHIP)
line = chip.get_line(GPIO_PIN)
line.request(consumer="melody", type=gpiod.LINE_REQ_DIR_OUT)

# Parser
def parse_melody(filename):
    melody = []
    semitones = PITCH_OFFSET
    factor = 1
    is_first_line = True
    with open(filename, "r") as f:
        for line in f:
            # The first line will contain the pitch offset
            if is_first_line:
                semitones += int(line.strip())
                is_first_line = False
                factor = 2 ** (semitones / 12)
                continue

            note, duration = line.strip().split(" ")
            melody.append((int(int(note) * factor), int(duration)))

    return melody

def get_random_melody():
    if len(sys.argv) > 1 and path.exists(sys.argv[1]):
        filename = sys.argv[1]
        print(f"Playing {filename}...")
    else:
        directory = path.dirname(path.realpath(__file__))
        songs = glob.glob(path.join(directory, "songs", "*.txt"))
        filename = songs[random.randint(0, len(songs) - 1)]
        print(f"Playing {filename}...")
    return parse_melody(filename)

# Player
def play_tone(hz, duration_ms):
    duration = duration_ms / 1000  # Convert ms to seconds
    if hz == 0:  # Just rest
        time.sleep(duration)
        return

    period = 1 / hz  # seconds
    duty = 0.12  # 10% duty cycle for softer sound

    # Pre-calculate all cycle timestamps
    cycles = int(duration * hz)
    start_time = time.perf_counter()

    # Generate timestamps for the entire note duration
    on_times = [start_time + (i * period) for i in range(cycles)]
    off_times = [t + (period * duty) for t in on_times]

    # Iterate through pre-calculated timestamps
    for on_time, off_time in zip(on_times, off_times):
        # Wait for exact on-time
        while time.perf_counter() < on_time: pass
        line.set_value(1)

        # Wait for exact off-time
        while time.perf_counter() < off_time: pass
        line.set_value(0)

def play_melody(melody):
    for note, duration in melody:
        play_tone(note, duration)

# Main
try:
    melody = get_random_melody()
    play_melody(melody)
except Exception as e:
    print("Error: ", e)
finally:
    line.set_value(0)  # Make sure the buzzer is off
    line.release()
    chip.close()
