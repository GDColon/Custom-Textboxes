# Custom Textboxes

This mod allows you to spawn custom alert boxes, chests, and dialogue boxes at any time. Useful for memes, video editing, etc.

Custom popups can be defined and edited in `steamapps/common/Geometry Dash/geode/config/colon.customtextboxes` (basic JSON understanding expected)

Main keybinds (can be changed in settings if you have a tiny ahh keyboard):
- Press HOME to spawn a popup
- Press PG UP to spawn a dialogue box
- Press INSERT to spawn a chest

Additionally:
- Hold SHIFT to "queue" the textbox, which delays it until the next menu button you click
- Hold ALT to display the secondary popup/chest/etc
- Press SHIFT+ESC to instantly close every active textbox

Most fields support \<cg>color tags\</c>, which are documented here: https://wyliemaster.github.io/gddocs/#/topics/tags?id=tags

In most cases, you can also type `\n` to go to a new line

### Table of Contents
1. [General JSON Syntax](#general-json-syntax)
2. **[Custom Popups](#custom-popups)**
3. **[Custom Dialogue Boxes](#custom-dialogue-boxes)**
4. **[Custom Chests](#custom-chests)**
5. [Using Custom Textures and Sounds](#using-custom-textures-and-sounds)
6. [Callbacks](#callbacks)
7. [Extra Resources](#extra-resources)


## General JSON Syntax

If you've never seen a JSON file before, it's [pretty easy to understand](<https://www.w3schools.com/js/js_json_datatypes.asp>). (just watch out for missing or trailing commas lol)

**Editing JSON files in Notepad is a very bad idea.** If you take a minute or two to install [VSCode](<https://code.visualstudio.com/download>) or similar, it will highlight all the errors and help you out with lots of other things.

The general syntax for ALL FILES is:

```js
{
    "to-show": "name",  // spawned when pressing hotkey
    "to-show-alt": "alt-name",  // spawned when holding alt and pressing hotkey

    "name": {
        // data goes here...
    },

    "alt-name": {
        // data goes here, used when alt is held
    },

    "another-one" {
        // store as many of these as you like!
        // but they will only appear if you set "to-show" to it
        // jsons are checked live and do not require a restart
    }
}
```

Here's an example of how `custom_popups.json` can look like:
```json
{
    "to-show": "hello",
    "to-show-alt": "hello2",

    "hello": {
        "title": "Custom Textboxes",
        "content": "Hello, world!"
    },

    "hello2": {
        "title": "Oh cool",
        "content": "This one appears when holding <cy>ALT</c>!",
        "button": "Nice!"
    }
}
```

Note that **ALL** properties (title, content, etc) have defaults and can be left out.


## Custom Popups
Spawns a customizable textbox

Define in `custom_popups.json`

Default keybind is HOME

**Base properties** are:
- `type` (string)
    - "popup" (default: the blue FLAlertLayer we know and love)
    - "icon" (the popup when selecting a locked icon)
    - "shop" (the popup when purchasing an item)
    - "comment" (the popup when entering a comment)
    - "mod" (the mod send popup, why not)
- `title` (string or Text) - The header text
- `content` (string or Text) - The main body text
- `button` (string or Button) - Button 1, defaults to "OK"
- `button2` (string or Button) - Button 2, if you want one
    - Button 2 is also the "cancel" button for "comment" and "mod" types
- `customButtons` (array of Buttons) - Adds additional custom buttons. You probably want to provide x and y values for them
- `width` (float) - Width of the popup, default is 350 (normal popup type only)
- `background` (string, int, or [filename](#texture-files-can-be-read-from)) - Background color of the popup
    - Main ones are brown, blue, green, purple, grey, lightgrey, transparent (GJ_square0#.png)
    - Can also do fancy (default for FLAlertLayers), black, white, whiteoutline, roundgrey, cyan, red, darkpurple, darkblue, geode, trans
- `sound` ([filename](#sound-files-can-be-read-from) or [Sound](#sound-object)) - Sound effect to play when popup appears
- `instant` (bool) - Disables the bouncing animation when spawning

**Button properties** can be a string, OR an object with any of:
- `content` (string or Text) - The message on the button
- `background` (string) - Button color
    - green, cyan, pink, grey, darkgrey, red, geodeblue, geodepink, geodepurple, geodeblack or trans
    - Uses GJ_button_0#.png texture in resources folder
- `texture` ([filename](#texture-files-can-be-read-from)) - Custom button texture, overrides background
    - Use a name in the resources folder, e.g. "GJ_button_03.png"
- `width` (float) - Custom width for the button
- `height` (float) - Maybe you wanna change the height too idk
- `x`, `y` (float) - Custom positioning for the button (0, 0 is the middle of the button row)
- `z` (int) - Z order for the button. Buttons with a higher Z order will be placed in front of others
- `scale` (float) - Scale factor for the whole button
- `onClick` ([Callback](#callbacks)) - Spawns something when the button is clicked
- `pulse` (object) - Enables size pulsing for the button
    - `size` (float) - Scale factor at peak of pulse, default is 1.1
    - `speed` (float) - How long one pulse takes, default is 1.0
- Example: `{ "content": "Hello!", "background": "red", "pulse": { "size": 1.5, "speed": 0.5, }`

**Text properties** (such as title) can be a string, OR an object with any of:
- `text` (string) - The message content
- `color` (array) - RGB color for the message, e.g. `[255, 128, 0]`
- `font` (string) - Bitmap font name, e.g. "gjFont51.fnt" (not supported in some places)
- `scale` (float) - Scale factor
- Example: `{ "text": "Well met!", "color": [255, 0, 0], "font": "goldFont.fnt" }`

Extra properties exclusive to icon/shop type:
- `subtitle` (string or Text) - Secondary message, normally says the icon number
- `closeButton` (bool or [Callback](#callbacks)) - Adds a close button to the corner
- `icon` (int or object) - Icon to display. Uses a cube if a number is provided, otherwise:
    - `id` (int) - Icon ID
    - `type` (string or int) - The type of icon to display
        - cube, ship, wave, color1, deathEffect, item, shipFire, etc
        - [Full list here](#icon-enums)
    - `col1`, `col2`, `colG` - Icon colors
- `customTexture` ([filename](#texture-files-can-be-read-from) or object) - A texture to replace the icon with. A little bit hacky
    - `name` ([filename](#texture-files-can-be-read-from)) - Texture name, e.g. "GJ_bigMoon_001.png"
    - `scale` (float) - Scale factor for the texture
    - Example: `{ "texture": "pixelart_1276_001.png", "scale": 2 }`

**Extra properties exclusive to shop type:**
- `shopType` (int) - Which shop to base the textbox on, 4 says diamond shards instead of orbs
- `price` (int) - Shop price

**Extra properties exclusive to comment type:**
- `commentType` (string or int) - Type of comment input
    - Can be level, profile, friend, list, or listname
- `charLimit` (int) - Character limit for the text field
- `percent` (int) - Percentage on level when clicking the checkbox

**Extra properties exclusive to mod type:**
- `platformer` (bool) - Use moons instead of stars

Advanced example:
```json
{
    "type": "popup",
    "background": "blue",
    "title": {
        "text": "Ad time!",
        "color": [255, 255, 128],
        "font": "gjFont02.fnt"
    },
    "content": "Watch an ad to continue playing more <cl>Geometry Dash</c>!\n\n<cr>Ads</c> -> <cy>$$</c> -> <cg>Time</c> -> <cl>Update 2.21</c>",
    "width": 400,
    "sound": {
        "name": "sfx/s2297.ogg",
        "speed": 2
    },
    "button": {
        "content": {
            "text": "Watch ad!",
            "font": "gjFont59.fnt"
        },
        "background": "pink",
        "pulse": { "size": 1.2, "speed": 0.5 }
    }
}
```


## Custom Dialogue Boxes

Spawns a sequence of custom dialogue boxes

Define in `custom_dialogues.json`

Default keybind is PG UP

Properties are:
- **`messages`** (array) - List of messages in the dialogue sequence
    - See the message properties below
- `background` (string, int, or [filename](#texture-files-can-be-read-from)) - Textbox background color
    - Main ones are brown, blue, green, purple, grey, lightgrey, transparent (GJ_square0#.png)
    - Also supports the same shortcut strings as the `background` property for popups (darkblue and darkpurple look really cool)
    - Can also be a custom texture
- `position` (string) - Position on screen
    - center, top, bottom
- `onComplete` ([Callback](#callbacks)) - Spawns something when the dialogue finishes
    - Can be used to chain multiple dialogues together

Message properties:
- `name` (string) - Textbox title, usually a character name
- `content` (string) - Message, supports many tags listed below
- `icon` (string or int) - Dialogue portrait to use
    - You can view all the available portraits in the game's resources folder (dialogIcon_###.png)
    - e.g. `{ "icon": 8 }` will use dialogIcon008.png which is Scratch
    - If you're lazy, you can also use strings like "scratch", "shopkeeper", "wraith", etc. ([full list here](#dialogue-portrait-strings))
    - Enter 0 for no portrait at all
- `customIcon` ([filename](#texture-files-can-be-read-from)) - Alternatively, a texture to load as the custom dialogue portrait
- `scale` (float) - Scale factor for message, default is 1.0
- `iconScale` (float) - Scale factor for portrait, default is 1.0
- `nameColor` (array) - RGB color for textbox title, e.g. `[255, 128, 0]`
- `unskippable` (bool) - Prevents clicking to skip the message animation
- `autoSkip` (bool) - Automatically advances to the next message without waiting for input
- `setBackground` (string, int, or [filename](#texture-files-can-be-read-from)) - Changes the textbox background for this and all subsequent messages
    - Might be buggy if the background is a different size
- `sound` ([filename](#sound-files-can-be-read-from) or [Sound](#sound-object)) - Optional sound effect to play when this message is reached
- `textSound` ([filename](#sound-files-can-be-read-from) or [Sound](#sound-object)) - Optional sound effect to play for *each character* that appears
- `pauseMusic` (bool) - Pauses the background music when reaching this message for """comedic effect""". Set to `false` to unpause
    - Music automatically unpauses when the dialogue finishes, unless the `onComplete` callback also contains `pauseMusic: true`

Along with color tags, dialogue boxes also support:
- Delay tags:
    - `<d010`> waits 0.1 seconds before continuing
    - Number args must always be exactly 3 digits
- Fade tags:
    - `<i100>hello</i>` fades in this phrase over 1 second
    - `<i000>hello</i>` spawns the phrase instantly
- Shake tags:
    - `<s260>HELP</s>` shakes the phrase with an intensity of 260
    - Rob usually uses 260
- Extra variables I added:
    - `<...>` is a shortcut for `<d010>.<d010>.<d010>.` which Rob loves to use
    - `<username>` becomes your username

Example:
```json
{
    "color": "blue",
    "position": "top",
    "iconID": 60,
    "messages": [
        {
            "name": "Scratch",
            "content": "It's me, <cl>Scratch!</c>",
            "icon": 10
        },
        {
            "name": "Scratch",
            "content": "Check it out<...> I have some <cy><s100>free candy!</s></c>",
            "icon": 26
        },
        {
            "name": "???",
            "content": "<i100><cr>Take it if you dare...</c></i>",
            "customIcon": "diffIcon_10_btn_001.png",
            "scale": 0.8,
            "nameColor": [100, 80, 120],
            "unskippable": true
        }
    ],
    "onComplete": {
        "playSound": "reward01.ogg",
        "spawnIcon": { "id": 60, "type": "cube" }
    }
},
```

## Custom Chests

Spawns a chest that **does not affect your stats**.

Because of this, the stat count in the corner will count up to your current stats. For example, if a chest gives 5 fire shards and you have 100, you'll see the counter go from 95 to 100. Yes there can be negative numbers.

Define in `custom_chests.json`

Default keybind is INSERT

Properties are:
- `type` (string or int) - Chest type. Affects texture, bg color, and sound
    - Can be small, big, 1key, 5key, 10key, 25key, 50key, 100key, gold
- `hideStats` (bool) - Hides the stat counter that appears in the top right corner
- `sound` ([filename](#sound-files-can-be-read-from) or [Sound](#sound-object)) - Optional sound effect to play when the chest appears
- `openSound` ([filename](#sound-files-can-be-read-from) or [Sound](#sound-object)) - Optional sound effect to play when the chest opens
- `onClaim` ([Callback](#callbacks)) - Spawns something when pressing the claim button
- **`rewards`** (array) - All chest contents go here. There seems to be a limit that varies based on what's in the chest, so some rewards may not always show

Reward properties (non-icon):
- `type` (string or int) - Reward type
    - orb, key, diamond, fireShard, soulShard, goldKey, etc
    - [Full list here](#icon-enums)
- `amount` (int) - Amount to reward
- `rewardID` (int) - I don't remember what this one does but it's the third arg of GJRewardObject::create

Reward properties (icon, auto-detected from `iconID` property):
- `iconType` (string or int) - The type of icon to display
    - cube, ship, wave, color1, deathEffect, item, shipFire, etc
    - [Full list here](#icon-enums)
- `iconID` (int) - Icon ID for the selected type
    - Strings are supported for the `item` type (e.g. "blueKey", "masterEmblem", "firePath")

Example:
```json
{
    "type": "50key",
    "hideStats": true,
    "rewards": [
        { "type": "orbs", "amount": 2500 },
        { "type": "goldKey", "amount": 3 },
        { "type": "lightShard", "amount": 5 },
        { "iconID": "masterEmblem", "iconType": "item" },
        { "iconID": 60, "iconType": "cube" }
    ]
}
```


## Using Custom Textures and Sounds
Many properties support custom textures or sounds! To use them, simply enter their filename. 

#### Texture files can be read from:

- **The game's resources folder**
    - Make sure to remove the `-hd` and `-uhd` suffixes or it won't work
    - e.g. `"dialogIcon_052.png"`, which is Scratch in a chopper
    - You can *technically* add custom files and folders here, but I would strongly encourage using the Texture Loader mod to load from a separate pack folder
- **Any spritesheet**
    - Every spritesheet texture has its own filename. If you don't know them, I suggest you check out https://gdcolon.com/gdsplitter
    - e.g. `"diffIcon_10_btn_001.png"`, which is the extreme demon face from GJ_GameSheet03, or `"pixelart_1276_001.png"` which is Rattledash
- **Texture pack folders**
    - If you have the Texture Loader mod installed, you can add custom textures to any pack folder and it will load from there if it exists
    - e.g. `"my_image.png"`, if you put it in the root directory of an enabled texture pack folder
    - Subdirectories are also supported, e.g. `"portraits/spoon.png"`
    - Make sure to reload the pack when adding/removing files!

#### Sound files can be read from:
- **The game's resources folder**
    - e.g. `"achievement_01.ogg"`
    - Sound effects **used in The Tower** can be accessed from the /sfx directory, e.g. `"sfx/s13587.ogg"` for "Well met"
    - SFX and music downloaded to the AppData folder cannot be accessed
    - You can *technically* add audio files here, but seriously just use the Texture Loader mod
- **Texture pack folders**
    - Simply place the audio file in there and you're good to go
    - e.g. `"my_sound.wav"`, if you put it in the root directory of an enabled texture pack folder
    - Make sure to reload the pack when adding/removing files!


## Callbacks
Callbacks allow you to spawn additional textboxes when buttons are pressed, dialogues are finished, etc. Their properties are:
- `spawnPopup`, `spawnChest`, `spawnDialogue` (string) - Spawns a custom textbox with specified ID
    - Can only choose one of these
- `spawnIcon` (int or object) - Bounces this icon around, like when unlocking Supporter
    - Can be a cube ID, or an object with ID + type, e.g. `{ "id": 10, "type": "ship" }`
- `playSound` (string or [Sound](#sound-object)) - Plays a sound effect
- `instantSound` (string or [Sound](#sound-object)) - Plays a sound effect, occurs before delay
- `delay` (float) - Waits this many seconds before spawning
- `ignore` (bool) - Popups only: If true, the popup will remain open after clicking
    - Allows clicking the button multiple times, which can be used to create sequences. ESC always closes a popup
- `pauseMusic` - Pauses the background music (for very niche cases). Set to `false` to unpause
    - Meant for textbox chaining, otherwise it stops all menu music until you enter a level or unpause in another callback/dialogue
- `goto` (int) - For callback sequences (see below), go to this index after running

For example, this dialogue spawns the textbox with ID "choice" when it finishes:
```json
{
    "color": "purple",
    "messages": [
        {
            "name": "Scratch",
            "content": "Do you prefer <cy>cinnamon</c> or <cl>butterscotch</c>?",
            "icon": 27
        }
    ],
    "onComplete": {
        "delay": 0.25,
        "spawnPopup": "choice"
    }
}
```

### Callback Sequences

**Callbacks can also be arrays!** Every time the callback is activated, it will spawn the next one in the sequence. This only works on alerts with buttons using the `ignore` property, so they don't close.

When the last callback in the array is activated, it will **not** loop and instead repeat the last one every time. However, you can use the `goto` property to jump to that index in the sequence. For example `"goto": 0` on the final callback will loop back to the start.

Here's an example of a popup that plays a looping, rising tone:
```json
{
    "title": "Music",
    "content": "Try clicking the button below!",
    "button": {
        "text": "Boop",
        "background": "pink",
        "pulse": { "speed": 2 },
        "onClick": [
            { "playSound": { "name": "sfx/s4400.ogg", "speed": 0 }, "ignore": true },
            { "playSound": { "name": "sfx/s4400.ogg", "speed": 2 }, "ignore": true },
            { "playSound": { "name": "sfx/s4400.ogg", "speed": 4 }, "ignore": true },
            { "playSound": { "name": "sfx/s4400.ogg", "speed": 5 }, "ignore": true },
            { "playSound": { "name": "sfx/s4400.ogg", "speed": 7 }, "ignore": true },
            { "playSound": { "name": "sfx/s4400.ogg", "speed": 9 }, "ignore": true },
            { "playSound": { "name": "sfx/s4400.ogg", "speed": 11 }, "ignore": true },
            {
                "playSound": { "name": "sfx/s4400.ogg", "speed": 12 },
                "spawnIcon": 280,
                "ignore": true,
                "goto": 0
            }
        ]
    }
}
```

## Extra resources

### Sound object
The "sound", "playSound", and other sound-related properties support a string filename such as "explode_11.ogg", or an object with:
- `name` ([filename](#sound-files-can-be-read-from)) - Sound filename, must be in resources or a texture pack folder
- `volume` (float) - Volume to play at, default is 1
- `speed` (float) - Speed adjustment, in semitones. Default is 0 and a full octave (200% speed) is 12

Additionally, you can provide an array with multiple sound objects and it will choose one randomly.

### Dialogue Portrait Strings
If you don't want to find portraits by their ID, you can enter these strings instead. I don't recommend it, but it works if you're lazy or just want to make something quick.

A number next to the name indicates how many different variations it has. For example, the Keymaster has 4 variations so you could enter `keymaster`, `keymaster2`, `keymaster3`, or `keymaster4`. Variations are ordered by lowest to highest ID in the resources folder.

Available portrait strings are:

- `spooky` (1)
- `keymaster` (4)
- `gatekeeper` (2)
- `wraith` (4)
- `shopkeeper` (9)
- `scratch` (8)
- `potbor` (7)
- `mechanic` (9)
- `diamond` (6)
- `monster` (2)
- `zolguroth` (1)
- `chopper` (1)
- `robtop` (1)


### Icon Enums

While properties like `icon.id`, `icon.type`, etc support numerical IDs, they also support strings as shortcuts. These are:

Icon types (1-15):

`cube`, `color1`, `color2`, `ship`, `ball`, `ufo`, `wave`, `robot`, `spider`, `trail`, `deathEffect`, `item`, `swing`, `jetpack`, `shipFire`

Reward types (1-15):

`fireShard`, `iceShard`, `poisonShard`, `shadowShard`, `lavaShard`, `key`, `keys`, `orb`, `orbs`, `diamond`, `diamonds`, `icon`, `earthShard`, `bloodShard`, `metalShard`, `lightShard`, `soulShard`, `goldKey`

Item types (1-19):

`blueKey`, `greenKey`, `orangeKey`, `masterEmblem`, `purpleKey`, `firePath`, `icePath`, `poisonPath`, `shadowPath`, `lavaPath`, `earthPath`, `bloodPath`, `metalPath`, `lightPath`, `soulPath`, `musicCustomizer`, `musicUnlocker`, `robotAnimationSlow`, `robotAnimationFast`, `spiderAnimationFast`

e.g. `{ "type": "item", "id": "firePath" }`