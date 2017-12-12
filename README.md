# Sigrok-Logic-Modifier
This is a small SDL tool that makes it possible to toggle data in a decompressed Sigrok logic analyzer file

The need for this application arose when I was analyzing I²C with Sigrok and PulseView.

Although PulseView is an amazing tool to see and record what was happening on the I²C bus, there were some glitches on the data and clock lines which made it hard for PulseView to decode.

It either incorrectly decoded the data or didn't decode it at all.

Not being that good of a programmer I didn't know how this could be added to PulseView directly, so I made my own with the help of SDL2.

The Sigrok files (.sr) are simple zip files, which you can extract to a folder.

The folder will contain multiple logic files containing your data.
It also has a metadata file containing information on probes, size, ...

From what I've seen, Sigrok logic files are restricted to 10MB.

This application needs both a logic file and the metadata file.

Here are the software's arguments :
```C
  // -r for raw data file
  // -m for metadata
  // -h for help, show this
  // -t for triggering of a probe [default = 0]
  // -o for start idx offset [default = 0x00000000]
```

An example would be "**software.exe -r logic-1-1 -m metadata <-t 1> <-o 1000000>**"

Once the files have been modified, you can then re-zip the files and rename to a .sr.

## How to use
1. Extract Sigrok file with a zip extracter
2. Launch program with at least -r and -m
3. Modify & save file
4. Quit program
6. Zip all the files back to a .sr

Once you have your modified / corrected Sigrok file, you can reopen it into PulseView. When adding the decoder to the data, it won't fail anymore as all the glitches have been removed.

## Controls
These are not the best, but they work for me. Feel free to change to whatever is more convenient for you
### Keyboard:
- s : save data to file
- z : undo once
- shift+z : undo all
- left arrow : move 1/3 screen left (changes depending on zoom)
- right arrow : move 1/3 screen to the right (changes depending on zoom)
- numpad plus : zoom in
- numpad minus : zoom out
- numpad 1 : find previous falling edge in probe used for triggering
- numpad 2 : find next falling edge in probe used for triggering
- numpad 4 : find previous rising edge in probe used for triggering
- numpad 5 : find next rising edge in probe used for triggering
- escape : quit

### Mouse:
Only when dots are displayed do the mouse functions work !
- Left click on a dot : toggle that data point from 0 to 1 or 1 to 0
- Left click and drag : all selected data points within the blue rectangle will be toggled

Whenever the zoom makes it so data points are at least 4 pixels from each other, then dots are added to the waveforms.

## Screenshot:
Below is one of those glitchy data points, that first small low pulse at the start of the second waveform (I²C SCK):
<img src="https://i.imgur.com/k3T2Uuq.png">
Here it is zoomed in, with dots:
<img src="https://i.imgur.com/nhXZ4wE.png">
Here you can see the click and drag feature which draws a rectangle from where you clicked to your mouse position:
<img src="https://i.imgur.com/nw29DCe.png">

## Other stuff
Below the waveforms there are hatch marks for every second, starting from 0. The above images are waveforms of 10MB data files with a sample rate of 2MHz, which should be 5,2 seconds long.

There's also display bar, which indicate where you are in the data. This is not a scroll bar (yet).

## License
This software is licensed under the MIT License, see License file for more info.
