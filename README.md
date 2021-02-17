ECWolf
======

ECWolf is a port of the Wolfenstein 3D engine based of Wolf4SDL. It combines the original Wolfenstein 3D engine with the user experience of ZDoom to create the most user and mod author friendly Wolf3D source port.

Like ZDoom, ECWolf aims to support all games which use the Wolfenstein 3D engine including Blake Stone (coming in ECWolf 3.0), Corridor 7, Operation Body Count, Rise of the Triad, and Super 3D Noah's Ark.  ECWolf will also support Macintosh Wolfenstein 3D along with all of its user created missions (coming in ECWolf 2.0).

* Single binary runs all supported games.  (Wolfenstein 3D, Spear of Destiny, ...)
* Full support for high resolution modes with aspect ratio correction including wide screen support.
* Modern control schemes (WASD + mouse).
* Mac Wolf/S3DNA/ROTT style automap.
* Unlimited save slots.
* This is actually based on the Wolf3D engine instead of a recreation or forcing into a more modern engine.
    * Software rendered using the same 8-bit ray casting.

Mod authoring
-------------

* Create mods without working with the source code!
    * Mods work with all supported platforms including Windows, Mac OS X, and Linux.
* Arbitrary high resolution texture, flat, and sprite support.
* Unlimited simultaneous moving pushwalls.
* Unlimited things.
* Unrestricted map size. (Although technical limits restrict GAMEMAPS format to 181x181.)
* Uses scripting formats and data formats similar to those of ZDoom.
    * Doom editing utilities work with ECWolf. (Except for levels)

Note that until ECWolf 2.0, while radical changes are not excepted, backwards compatibility for mods will not necessarily be kept.  See [the wiki](http://maniacsvault.net/ecwolf/wiki/Version_compatibility) for more information.

Links
-----

* [Website](http://maniacsvault.net/ecwolf/)
* [Forums](http://forum.drdteam.org/viewforum.php?f=174)
* [Wiki](http://maniacsvault.net/ecwolf/wiki/)

# RG351 Installation

All I really did here was modify some stuff. Nothing major and nothing outstanding, but it works nicely and has some features ecwolf-libretro doesn't. 
- Place ecwolf and ecwolf.pk3 files in a new folder under `/roms/ports/ecwolf`
- Merge .config folder to `/home/ark/.config`
- Place .wl6, .sod, .sd2 and .sd3 files under `/roms/ports/ecwolf`. They can all go into one folder, you don't need to create subdirectories.
- Place .sh ("Wolfenstein.sh", "Wolfenstein - Spear of Destiny.sh", etc.) files under `/roms/ports/` or `/roms/ports/ecwolf`
- Launch from Emulationstation and enjoy!

## Differences with ecwolf-libretro.so
- Native resolution support (480x320)
- Native control remapping; Can set "menu" and "pause" buttons accordingly
- Proper save support (instead of just save states)
- Ability to launch Spear of Destiny mission packs & mods

## Thanks to:
- [ECWolf-RPI for the keyboardpatch.diff](https://github.com/tpo1990/ECWolf-RPI)
- [SeongGino](https://github.com/SeongGino) for pointing me in the right place to change the resolution
- [Christian_Hatian](https://github.com/christianhaitian) & the wonderful [ArkOS](https://github.com/christianhaitian/arkos) for the RG351
