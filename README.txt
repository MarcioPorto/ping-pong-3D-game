README file for Interactive Graphics HW 2:				
Marcio Porto & Charles Park								

---------------------------------------------------------

For this homework, we were supposed to implement a 3D Ping Pong table and also a small simulation of a table tennis game. The computer serves the ball, which can be hit with our paddle.

We were initially only given code to keep track of the paddle and move it around using the mouse. We had implement everything else ourselves.

First, we drew the ping pong table, the net and the ball, according to the dimensions listed on the assignment handout. The net was drawn usign two for loops, one for the horizontal and one for the vertical lines. Then, we updated the ball's position and velocity according to the acceleration of gravity using the equation provided on page 4 of the assignment handout and handled the contact of the ball with the table using the formula on one of the lecture slides available on Moodle.

Next we handled the contact between the ball and the net and the ball and the paddle by using variations of the same formula used above to handle the contact with the table. We also adjusted the weight of the x, y and z components of the paddle when it hits the ball to make it eaier to hit angled shots. 

The next step was to handle the case when the paddle is moved so fast that the ball just goes through it when it should actually have been hit by it. We did that by creating a line segment betweent the position of the paddle at the current time and its previous position. Then we checked to see how far the ball was from this line segment. If it was less than 8 cm away (the radius of the paddle) then that means the ball should be hit, so we just move the ball to the front of the paddle, where it was supposed to be, and the game goes on!

To close off, we had our game print out some prompts to indicate what was going on in the game as it happened. Some examples of these messages were:
	- "Nice shot - your point!"
	- "Out of bounds - opponent's point!"
	- "Hit the net - opponent's point!"
	- "Bounced on your side before going over the net - opponent's point!"

Additionally, we also thought it would be cool to keep track of the score of the game, so we print that to the console every time someone scores.