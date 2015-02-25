# Spatial partitioning (STUB)

Games usually have thousands of various entities in the world, but only a few of them needs to be updated, rendered or considered in collision detection. Usually you only work with entities that are visible on the screen.

The trouble is that iterating over thousands of entities to only find those that are visible on the screen each frame is inefficient. Spatial partitioning optimizes this significantly.

The basic implementation:

- Say you have world of size 1000&times;1000 pixels.
- Split that into tiles of size 100&times;100. So you have 10 tiles horizontally and 10 tiles vertically.
- Each tile will have an array of entities that are positioned on it.
- Everytime an entity moves, we check whether it is still on the same tile. If it has moved to another tile, we remove the entity from the previous tile and add it to this new tile.

