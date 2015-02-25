# Linear interpolation (STUB)

Linear interpolation (also called *lerp*) is a simple but powerful function very often used in games in various forms. A very common use is to smoothly interpolate between two values:

```C
Lerp(A, B, t) {
    return A + t * (B - A)
}
```

Values `A` and `B` can be any number, but `t` must be in range from 0 to 1. For example: You want to move your enemy from position 0 to position 100 in 10 seconds, then to get the smooth position in each frame you do:

```C
SmoothPosition = Lerp(0, 100, 10 / SecondsPassed)
```