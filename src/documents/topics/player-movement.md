# Movement (STUB)

<!--
	Day 43 - https://www.youtube.com/watch?v=LoTRzRFEk5I
	Day 44 - Bounce http://youtu.be/2nyNOJoVC8M?t=5m44s
		Inner Product
		Outer Product
-->

## Diagonal movement

Consider following implementation:

```C
HorizontalSpeed = 0
VerticalSpeed = 0
if (IsKeyPressed(Left)) {
	HorizontalSpeed = -1
}
if (IsKeyPressed(Right)) {
	HorizontalSpeed = 1
}
if (IsKeyPressed(Top)) {
	VerticalSpeed = -1
}
if (IsKeyPressed(Down)) {
	VerticalSpeed = 1
}
X += HorizontalSpeed * Time
Y += VerticalSpeed * Time
```

If a single key is pressed then the player will move 1 unit. However if you will try to move diagonally (in two directions), then the player will move 1 unit horizontally and 1 unit vertically. So the player will actually move 1.41421 unit.

```C
Distance = Square(HorizontalSpeed) + Square(VerticalSpeed);
if (Distance > 1)
{
	HorizontalSpeed *= (1 / SquareRoot(Distance))
	VerticalSpeed   *= (1 / SquareRoot(Distance))
}
```

Note: The code assumes that `HorizontalSpeed` and `VerticalSpeed` are normalized (in range of -1 to 1).

Day 47 (at beginning).

## Gliding along the walls

(Day 50)

This is related to collision detection. Some implementations have problem that their collision detection stops movement in all directions when a collision is detected. Casey is solving this by simulating the collision and a slide along the wall to move the player in full distance.

His algorithm does a limited amount (4 in initial implementation; Day 47) of tries to calculate the resulting position:


# Jump

Not discussed, yet.