# Levedica

Levedia (spelled Lev-ay-dicka) is a WIP strategy game for the Nintendo Wii. You can select a country, and conquer the world! ...or atleast the part of it that's on the map :)


## Prerequisites to running
You must have a formatted SD card in your Wii (or your emulator) with a map.png and map.csv (these are the map files themselves), as well as a states.csv and countries.csv (this is the scenario). I'll add an example map and scenario, but you can make it yourself:

- Map files: Convert using https://github.com/Laky2k8/geojson2bitmap from an EPSG:3035 format GeoJSON map
- Scenario files: No editor yet, coming soon (hopefully)


## Controls
- UP and DOWN: Move in the country selection menu
- A: `Select` in the country selection menu, state info ingame
- B: Paints the selected province to your color, the `Conquer` button basically


## How to build

Step 0: Make sure you have the Wii homebrew development libraries and SDK installed

Step 1: Clone the repository

Step 2: run `make`

Step 3: The game is waiting for you under `Levedica.elf`!


## How to run on an actual Wii

I don't really know because I don't have a Wii yet... will add instructions here once I get one ;)
