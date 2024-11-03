# Game Database JSON Format

The `gamedatabase.json` file contains information about xbox360 games, including their title, ID, achievements, and other metadata. The file is structured as a JSON object with a single key, `games`, which holds an array of game entries.
This file should be placed in the rootfolder of velocity.

Each game entry in the games array contains the following fields:

nm: (String) The name of the game.
tid: (String) The title ID of the game in hexadecimal format.
achc: (String) The count of achievements associated with the game.
ttlgs: (String) The total gamerscore available in the game.
ttlac: (String) The total number of achievements in the game.
ttlmac: (String) The total number of multiplayer achievements in the game.
ttlfac: (String) The total number of friend achievements in the game.

## JSON Structure

```json
{
    "games": [
        {
            "nm": "Game Title",
            "tid": "TitleID",
            "achc": "AchievementCount",
            "ttlgs": "TotalGamerscore",
            "ttlac": "TotalAchievements",
            "ttlmac": "TotalMultiplayerAchievements",
            "ttlfac": "TotalFriendAchievements"
        },
        ...
    ]
}
