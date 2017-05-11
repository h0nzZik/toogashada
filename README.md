# Topic: 2D multiplayer network-based shooter game

The game will be played seen from above on a rectangular map with moving obstacles.
Players can be assigned to teams competing in various game modes. The game mode, as well
as other game settings, can be configured by game admin running the server.

## Game modes:
* Classic - kill all enemies.
* Deadmatch - the points is to collect as many kills as possible, while reducing losts. Players are respawned a few seconds after death.
* Capture the flag

## Player controls
Player can rotate 360 degrees using mouse, run forward or step sideways/backwards using keyboard and shoot. Every player will see only a portion of map around himself.

## Usage
Server/clients will be run and configured from command line. GUI for menu may be optionally added.

example:
```
$ game --server --port=1000 --player-name="honza" --player-team="red"
$ game --client --ip=192.168.1.18 --port=10000 --player-name="pepa" --player-team="blue"
```

When players are connected, the game admin can initiate the game.

## Implementation

We will most likely use SDL for graphics and Asio for networking.