# klipper-buzzer

This repo contains the tools for using a buzzer with Klipper.

## Installation

1. Install the buzzer on your GPIO pins.
    - These buzzers usually have polarity, if not playing correctly check if you didn't swap the GND and GPIO pins!
    - If you are using a Raspberry Pi, you can use PWM and use Rpi.GPIO and skip this.
    - If using other boards without PWM, like the BTT PI v1.2 (Like I am) you can continue.
        - For BTT Pi, you can refer to this [WIKI](https://github.com/bigtreetech/CB1#40-pin-gpio) to find the correct GPIO pin to use.

![image](https://github.com/user-attachments/assets/e1671abd-5c2f-426b-b0eb-4af76ae402fd)

2. Install `gpiod` and `python3-gpiod` on your system.

```bash
sudo apt install gpiod python3-gpiod
```

3. Install [gcode_shell_command](https://github.com/dw-0/kiauh/blob/master/docs/gcode_shell_command.md) using Kiauh under **Extensions**.

4. Clone this repo.

```bash
git clone git@github.com:LeandroSQ/klipper-buzzer.git ~/buzzer
```

5. Add the following to your `printer.cfg` file.

```yaml
[gcode_shell_command PLAY_TUNE]
command: /usr/bin/python3 /home/$(whoami)/buzzer/buzzer.py &
timeout: 2.
verbose: True

[gcode_macro PLAY_TUNE]
gcode:
    RUN_SHELL_COMMAND CMD=PLAY_TUNE
```

6. **(EXTRA)** You can also specify a file to be played instead of randomly picking one:
```yaml
[gcode_shell_command PLAY_ONE_PIECE]
command: /usr/bin/python3 /home/$(whoami)/buzzer/buzzer.py "songs/one piece - we are.txt" &
timeout: 2.
verbose: True

[gcode_macro PLAY_ONE_PIECE]
gcode:
    RUN_SHELL_COMMAND CMD=PLAY_ONE_PIECE
```

7. **(EXTRA)** You can append your newly added GCODE Macro at the end of the machine GCODE on your slicer. So everytime a print is finished it will play your tunes.
![image](https://github.com/user-attachments/assets/ee4f7bb0-3a61-4916-9351-c6b456767cfc)


## Adding songs

1. Find a `.mid` file of the song you want to add.
2. You can clean up the channels to keep only the melody you want using a midi editor like [Signal](https://signal.vercel.app/edit).
3. Convert the `.mid` file to notes and duration, you can use the [MIDI to M300 Converter](https://alexyu132.github.io/midi-m300/), remove `M300`, `S` and `P` from the output and add `0` on the first line.
    - First line is reserved for pitch modulation, `0` is a good start.
    - The following format per line! `NOTE_IN_HZ DURATION_IN_MS`
4. Save it as a `.txt` file in the `songs` folder.
5. **(Extra)** If not sounding as expected, you can change the number on the first line of your `.txt` to shift the pitch, in semitones. Usually with passive buzzers the higher the better!

## Usage

You can play a song by running the following command:

```yaml
python3 buzzer.py songs/<song>.txt
```

Or to pick a random song, just omit the filename:

```yaml
python3 buzzer.py
```

## References

| Name | Link |
| -- | -- |
| rootiest's zippy-klipper, I converted a few tunes from there | https://github.com/rootiest/zippy-klipper_config/blob/master/macros/tunes.cfg |
| Klipper's Gcode Shell Command | https://www.klipper3d.org/G-Codes.html#gcode-shell-command |
| 'Create Marlin like M300 beeper tone' reddit thread | https://www.reddit.com/r/klippers/comments/o775te/create_marlin_like_m300_beeper_tone/ |
| 'Gcode Music' reddit thread | https://www.reddit.com/r/ender3/comments/nd992l/gcode_music/ |
