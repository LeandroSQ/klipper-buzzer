# klipper-buzzer

This repo contains the tools for using a buzzer with Klipper.

## C - V2

### Installation

A complete **rewrite**, it **uses C and LGPIO with software PWM and timers** to play the tunes, the **performance impact is minimal while sound quality remains the same** as the previous version. It does require a bit more setup but it's worth it, as the previous python version hijacks the CPU core to ensure precise timing which can lead to 100% CPU usage on a single core.

1. Install the buzzer on your GPIO pins.
    - These buzzers usually have polarity, if not playing correctly check if you didn't swap the GND and GPIO pins!
    - If you are using a Raspberry Pi, you can use PWM and use Rpi.GPIO and skip this.
    - If using other boards without PWM, like the BTT PI v1.2 (Like I am) you can continue.
        - For BTT Pi, you can refer to this [WIKI](https://github.com/bigtreetech/CB1#40-pin-gpio) to find the correct GPIO pin to use.

![image](https://github.com/user-attachments/assets/e1671abd-5c2f-426b-b0eb-4af76ae402fd)

2. Install [gcode_shell_command](https://github.com/dw-0/kiauh/blob/master/docs/gcode_shell_command.md) using Kiauh under **Extensions**.

3. Install [lgpio](https://abyz.me.uk/lg/lgpio.html) on your system.

```bash
sudo apt install swig -y
cd src
wget http://abyz.me.uk/lg/lg.zip -O /tmp/lg.zip
unzip /tmp/lg.zip -d include
cd include
make
cd include/lg
sudo make install
```

4. Configure GPIO permissions (required for non-root access).

To allow the program to access GPIO devices without running as root, you need to set up proper permissions:

```bash
# Create a gpio group
sudo groupadd gpio

# Add your user to the gpio group (replace with your username)
sudo usermod -aG gpio $USER

# Create a udev rule
sudo nano /etc/udev/rules.d/99-gpio.rules
```

Add this line to the file:
```
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", GROUP="gpio", MODE="0660"
```

Save and exit, then reload udev rules:
```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

**Important:** Log out and log back in (or reboot) for the group membership to take effect.

Verify the permissions are correct:
```bash
ls -l /dev/gpiochip*
# Should show: crw-rw---- root gpio /dev/gpiochip0
```

5. Clone and build the source code.

```bash
git clone git@github.com:LeandroSQ/klipper-buzzer.git ~/buzzer
cd ./buzzer
chmod +x ./tools/build.sh
./tools/build.sh
```

6. Add the following to your `printer.cfg` file.

```yaml
[gcode_shell_command PLAY_TUNE]
command: /home/$(whoami)/buzzer/buzzer &
timeout: 2.
verbose: True

[gcode_macro PLAY_TUNE]
gcode:
    RUN_SHELL_COMMAND CMD=PLAY_TUNE
```

7. **(EXTRA)** You can also specify a file to be played instead of randomly picking one:
```yaml
[gcode_shell_command PLAY_ONE_PIECE]
command: /home/$(whoami)/buzzer/buzzer "songs/one piece - we are.txt" &
timeout: 2.
verbose: True

[gcode_macro PLAY_ONE_PIECE]
gcode:
    RUN_SHELL_COMMAND CMD=PLAY_ONE_PIECE
```

8. **(EXTRA)** You can append your newly added GCODE Macro at the end of the machine GCODE on your slicer. So everytime a print is finished it will play your tunes.
![image](https://github.com/user-attachments/assets/ee4f7bb0-3a61-4916-9351-c6b456767cfc)


### Adding songs

1. Find a `.mid` file of the song you want to add.
2. You can clean up the channels to keep only the melody you want using a midi editor like [Signal](https://signal.vercel.app/edit).
3. Convert the `.mid` file to notes and duration, you can use the [MIDI to M300 Converter](https://alexyu132.github.io/midi-m300/), remove `M300`, `S` and `P` from the output and add `0` on the first line.
    - First line is reserved for pitch modulation, `0` is a good start.
    - The following format per line! `NOTE_IN_HZ DURATION_IN_MS`
    - You can use `./tools/convert.sh <song>.txt` to convert the output to the correct format.
4. Save it as a `.txt` file in the `songs` folder.
5. **(Extra)** If not sounding as expected, you can change the number on the first line of your `.txt` to shift the pitch, in semitones. Usually with passive buzzers the higher the better! Try 6, 12, 18 and 24 and see what sounds best.

### Usage

You can play a song by running the following command:

```yaml
./buzzer/buzzer songs/<song>.txt
```

Or to pick a random song, just omit the filename or provide a folder:

```yaml
./buzzer/buzzer
# or
./buzzer/buzzer /tmp/another_folder_with_songs
```

## Python - V1
<details>
    <summary>Click to expand</summary>

### Installation

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
command: /usr/bin/python3 /home/$(whoami)/buzzer/src/buzzer.py &
timeout: 30
verbose: True

[gcode_macro PLAY_TUNE]
gcode:
    RUN_SHELL_COMMAND CMD=PLAY_TUNE
```

6. **(EXTRA)** You can also specify a file to be played instead of randomly picking one:
```yaml
[gcode_shell_command PLAY_ONE_PIECE]
command: /usr/bin/python3 /home/$(whoami)/buzzer/src/buzzer.py "songs/one piece - we are.txt" &
timeout: 30
verbose: True

[gcode_macro PLAY_ONE_PIECE]
gcode:
    RUN_SHELL_COMMAND CMD=PLAY_ONE_PIECE
```

7. **(EXTRA)** You can append your newly added GCODE Macro at the end of the machine GCODE on your slicer. So everytime a print is finished it will play your tunes.
![image](https://github.com/user-attachments/assets/ee4f7bb0-3a61-4916-9351-c6b456767cfc)

### Usage

You can play a song by running the following command:

```yaml
python3 src/buzzer.py songs/<song>.txt
```

Or to pick a random song, just omit the filename:

```yaml
python3 src/buzzer.py
```

</details>

## References

| Name | Link |
| -- | -- |
| rootiest's zippy-klipper, I converted a few tunes from there | https://github.com/rootiest/zippy-klipper_config/blob/master/macros/tunes.cfg |
| Klipper's Gcode Shell Command | https://www.klipper3d.org/G-Codes.html#gcode-shell-command |
| 'Create Marlin like M300 beeper tone' reddit thread | https://www.reddit.com/r/klippers/comments/o775te/create_marlin_like_m300_beeper_tone/ |
| 'Gcode Music' reddit thread | https://www.reddit.com/r/ender3/comments/nd992l/gcode_music/ |
