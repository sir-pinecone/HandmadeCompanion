---
title: "Error-based Drawing Algorithms"
videoId: "q79-Qh2suMY"
markers:
    "0:11": "Glimpse into the future beyond RSI Break"
    "0:49": "Blackboard: Bresenham"
    "2:43": "Blackboard: Drawing a pixel at a time directly to a CRT"
    "6:48": "Blackboard: Constructing an algorithm for drawing a line"
    "15:19": "Blackboard: Picking whether horizontal or vertical is the better choice, i.e. error-based drawing"
    "27:04": "Blackboard: Constructing an algorithm for drawing a circle"
    "29:42": "\"I don't want to be at the club. This is not dance time\" (!quote 371)"
    "35:23": "Blackboard: Optimising this circle-plotting routine"
    "37:06": "Blackboard: Transforming the implicit line-drawing equation into the circle one"
    "40:09": "Internet: Bresenham's line algorithm [see Resources, Wikipedia]"
    "42:12": "elxenoaizd Q: I remember reading that Bresenham works for the first octant, and they had two cases to handle (slope >= 1 and slope < 1). Will this dx = sign(x1 - x0) address both of these concerns?"
    "43:49": "Connor_Rentz Q: Do we ever do line drawing on Handmade Hero?"
    "44:03": "elxenoaizd Q: I'm still not 100% sure how Bresenham figured out 'if horizontal is better'. You mentioned they used the line equation y = mx+b. How does that help them in knowing if horizontal is better?"
    "46:38": "elxenoaizd Q: There were multiple extensions / optimizations done on Bresenham like drawing only half the line and mirroring it, and Run Slicing by Michael Abrash and Symmetric Double Step by Xialon Wu. Any thoughts on those?"
    "48:25": "Pseudonym73 Q: Little-known fact, Bresenham's method predates framebuffers by about five years. It was actually designed for a drum plotter"
    "49:22": "arrrjt Q: I feel it should be deciding between \"move right\" and \"move diagonally right and down\", instead of either \"left\" or \"down\""
    "50:53": "elxenoaizd Q: Can that method be used to draw wireframe triangles, or is there a faster way to draw wireframe triangles like flat-bottom / flat-top trigs?"
    "51:33": "XpansionPak Q: Is this ever used for uniform grid pathfinding?"
    "52:16": "nxsy Q: For tomorrow: Given an entity with position and current velocity and maximum acceleration, and a target position, how do you determine the correct acceleration (direction and speed) each frame so that you stop on the target point?"
    "53:55": "Pseudonym73 Q: Oh, FYI, this is how I used to derive Bresenham's algorithm [see Resources, Pastebin]"
    "54:41": "elxenoaizd Q: Was the cast from float to int really expensive at the time vs nowaday? is bresenham still relavent today or can we get away with a naive line drawing algo with floating math?"
    "55:14": "nxsy Q: Acceleration is constrained to a maximum amount."
    "56:04": "Longboolean Q: Was this also used for bezier curves?"
    "56:51": "cubercaleb Q: Off-topic but still on the topic of math: During these RSI streams do you think you could go over the math for replacing cmath, specifically the trig functions?"
    "57:07": "CaptainKraft Q: Does OpenGL have this stuff implemented already?"
    "57:14": "elxenoaizd Q: If you were to implement a line drawing function in Handmade Hero, what algorithm would you go for?"
    "58:01": "Wrap things up"
---

## Quotes

* 371\. I don't want to be at the club. This is not dance time (29:42)

## Resources

* Pastebin: [Pseudonym73's derivation of Bresenham](http://pastebin.com/c7AGHAUT)
* Wikipedia: '[Bresenham's line algorithm](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)'
