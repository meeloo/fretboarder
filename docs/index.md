#  Fretboarder
##  Fusion 360 plugins for luthiers

<iframe width="560" height="315" src="https://www.youtube.com/embed/HSuFJhZLFmo" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

## Easily create models of complex fretboards
Features:
* almost WYSIWYG: see what the fretboard will look like in 2D as you change parameters
* supports up to 20 strings
* simple and compound radius
* left and right handed
* single or multiscale
* optional zero fret
* any string spacing at the nut and at the bridge
* configurable fret tang width and depth
* configurable nut size
* blind fret slot support (to hide the end of the fret tangs in the fretboard) 
* create nicely rounded frets for rendering purpose

## New in v0.5beta
* Fretboarder doesn't disable parametric modeling anymore. You will get the full history and objects created by the plugin in the timeline.  

### New in v0.4
* fixed many fretboard calculation bugs that would crash Fusion 360 or display error messages
* added a progress dialog to give feedback during the generation of the fretboard and frets
* added loading and saving of presets to .frt files (using the JSon format)
* disable the OK button when you are trying to create an impossible fretboard. In case you don't undertand why the OK button is disabled try to increase the fretboard thickness or decrease its radiuses...
### New in interim v0.4a
* fixed 1 string instruments
* fixed presets were scaled to 10% of their real sizes
* nut width were not correctly computed 

### New in v0.3
* design time graphics to help you see in real time what the fretboard will look like!
* Use 100 as the perpendicular fret to make the bridge perpendicular in a multiscale instrument.
* You can use negative perpandicular fret index to shift the whole fretboard angle.
* Added tooltips explaining every parameter in the UI
* Removed some construction planes and sketches
* FIXED: crash when asking for a perpandicular fret beyond the 23rd
### Added in v0.3a
* moved the UI to tabs, some computers couldn't display the whole UI vertically

### New in v0.2
* more than 200 times faster!
* more accurate fret slot calculation
* creates the actual frets in addition to the fret slots! (optional)
* apply some default materials

## Known bugs and todo list
* double string instruments (12 string guitars, mandolins, 8 string basses...)
* better options for string spacing
* offer a way to compute the fret spacing from the nut width instead of entering inter string spacing
* add a way to give string gauges and use it for string spacing 

## Alpha version = Use at your own risks!
This is only alpha grade software.
I have tested and used it on my computers and it worked fine. Let me know (<fretboarder@meeloo.net>) if you find anything strange though.  

## Installation
Dowload the archive for your OS. In the archive you will find one file name fretboarder.dylib on macOS and fretboarder.dll on windows. Copy it to your Fusion 360 add ins folder. More information on how to do that can be found on the [Autodesk Fusion 360 Help site](https://knowledge.autodesk.com/support/fusion-360/troubleshooting/caas/sfdcarticles/sfdcarticles/How-to-install-an-ADD-IN-and-Script-in-Fusion-360.html)

Here is a small video showing how to install the plugin on Windows:
<iframe width="560" height="315" src="https://www.youtube.com/embed/e96nMz-E10Y" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>

## Download
download [Fretboarder v0.5 for Windows](bin/windows/Fretboarder_win_0.5.zip)

download [Fretboarder v0.5 macOS](bin/macOS/Fretboarder_mac_0.5.zip)

## Contact
For all questions and feedback, please send an email to <fretboarder@meeloo.net>
