/**
Comp 394 S15 Assignment #2 Ping Pong 3D
Charles Park and Marcio Porto
**/

#include "App.h"
#include <iostream>
#include <sstream>
using namespace std;

// Tells C++ to invoke command-line main() function even on OS X and Win32.
G3D_START_AT_MAIN();

const double App::GRAVITY = 981;

int main(int argc, const char* argv[]) {
	(void)argc; (void)argv;
	GApp::Settings settings(argc, argv);

	// Change the window and other startup parameters by modifying the
	// settings class.  For example:
	settings.window.width       = 1280; 
	settings.window.height      = 720;

	return App(settings).run();
}




App::App(const GApp::Settings& settings) : GApp(settings) {
	renderDevice->setColorClearValue(Color3(0.2, 0.2, 0.2));
	renderDevice->setSwapBuffersAutomatically(true);
}


void App::onInit() {
	GApp::onInit();
	// Turn on the developer HUD
	createDeveloperHUD();
	debugWindow->setVisible(false);
	developerWindow->setVisible(false);
	developerWindow->cameraControlWindow->setVisible(false);
	showRenderingStats = false;

	// Setup the camera with a good initial position and view direction to see the table
	activeCamera()->setPosition(Vector3(0,100,250));
	activeCamera()->lookAt(Vector3(0,0,0), Vector3(0,1,0));
	activeCamera()->setFarPlaneZ(-1000);

	position = Vector3(0,30,-130);
	nextPosition = Vector3(0,30,-130);
	previousPosition = Vector3(0,0,0);
	tempPaddlePosition = Vector3(0,0,0);
	tempBallPosition = Vector3(0,0,0);

	initSpeed = 200;
	initAngle = toRadians(45.0);
	initVelocity = Vector3(0, initSpeed*1, initSpeed*2);
	ballVelocity = initVelocity;

	Vector3 newPos;
	Vector3 prePos;
	text = "";

	negZTracker = 0;
	posZTracker = 0;
	textTracker = 0;
	playerScore = 0;
	opponentScore = 0;

 	stringstream score1;
 	stringstream score2;
 	score1 << playerScore;
 	score2 << opponentScore;
 	string playerString = score1.str();
 	string opponentString = score2.str();
 	cout << "Player Score: " + playerString + " - Opponent Score: " + opponentString << "\n";

}


void App::onUserInput(UserInput *uinput) {

	// This block of code maps the 2D position of the mouse in screen space to a 3D position
	// 20 cm above the ping pong table.  It also rotates the paddle to make the handle move
	// in a cool way.  It also makes sure that the paddle does not cross the net and go onto
	// the opponent's side.
	Vector2 mouseXY = uinput->mouseXY();
	float xneg1to1 = mouseXY[0] / renderDevice->width() * 2.0 - 1.0;
	float y0to1 = mouseXY[1] / renderDevice->height();
	Matrix3 rotZ = Matrix3::fromAxisAngle(Vector3(0,0,1), aSin(-xneg1to1));    
	Vector3 lastPaddlePos = paddleFrame.translation;
	paddleFrame = CoordinateFrame(rotZ, Vector3(xneg1to1 * 100.0, 20.0, G3D::max(y0to1 * 137.0 + 20.0, 0.0)));
	prePos = newPos;
	newPos = paddleFrame.translation;

	// This is a weighted average.  Update the velocity to be 10% the velocity calculated 
	// at the previous frame and 90% the velocity calculated at this frame.
	paddleVel = 0.1*paddleVel + 0.9*(newPos - lastPaddlePos);


	// This returns true if the SPACEBAR was pressed
	if (uinput->keyPressed(GKey(' '))) {
		// This is where you can "serve" a new ball from the opponent's side of the net 
		// toward you. I found that a good initial position for the ball is: (0, 30, -130).  
		// And, a good initial velocity is (0, 200, 400).  As usual for this program, all 
		// units are in cm.
		
		position = Vector3(0,30,-130);
		nextPosition = Vector3(0,30,-130);
		ballVelocity = initVelocity;
		text = "";
		negZTracker = 0;
		posZTracker = 0;
		textTracker = 0;
	}
}



void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
	GApp::onSimulation(rdt, sdt, idt);
	// rdt is the change in time (dt) in seconds since the last call to onSimulation
	// So, you can slow down the simulation by half if you divide it by 2.
	rdt *= 0.4;

	// Here are a few other values that you may find useful..
	// Radius of the ball = 2cm
	// Radius of the paddle = 8cm
	// Acceleration due to gravity = 981cm/s^2 in the negative Y direction
	// See the diagram in the assignment handout for the dimensions of the ping pong table
	
	position = nextPosition;
	LineSegment line = LineSegment::fromTwoPoints(Point3(prePos.x,prePos.y,prePos.z), Point3(newPos.x,newPos.y,newPos.z));


	// Table collision
	if((position.x > -152/2 && position.x < 152/2) && (position.y < 3) && (position.z > -274/2 && position.z < 274/2)){

		ballVelocity = ballVelocity - 2*(ballVelocity.dot(Vector3(0,1,0)))*Vector3(0,1,0);
		ballVelocity *= 0.85;

		if(position.z < 0){
			negZTracker += 1;
		}

		else{
			posZTracker += 1;
		}

		if(position.y < 4){
			position.y = 4;
		}
	}

	// Net coliision
	else if((position.x > - 91.5 && position.x < 91.5) && (position.y > 2 && position.y < 17.25) && (position.z > -5 && position.z < 5)){
		ballVelocity = ballVelocity - 2*(ballVelocity.dot(Vector3(0,0,1)))*Vector3(0,0,1);
		ballVelocity *= 0.7;

		if(textTracker == 0){
			text = "Hit the net - opponent's point";
			textTracker += 1;
			opponentScore += 1;

			stringstream score1;
 			stringstream score2;
 			score1 << playerScore;
 			score2 << opponentScore;
 			string playerString = score1.str();
 			string opponentString = score2.str();
 			cout << "Player Score: " + playerString + " - Opponent Score: " + opponentString << "\n";
		}
	}
	
	// Paddle collision
	else if((((position.x - newPos.x)*(position.x - newPos.x) + (position.y - newPos.y)*(position.y - newPos.y)) < 8*8) && (position.z > newPos.z - 3 && position.z < newPos.z + 3)){

		ballVelocity = ballVelocity - 2*(ballVelocity.dot(Vector3(0,0,1)))*Vector3(0,0,1);
		ballVelocity += Vector3(paddleVel.x*10,paddleVel.y,paddleVel.z);

		
		if(position.z > newPos.z-3 && position.z < newPos.z+3){
			position.z = newPos.z-3;
			ballVelocity += Vector3(paddleVel.x*5,paddleVel.y,paddleVel.z);
		}
	}
	
	else{
		ballVelocity = ballVelocity + rdt*Vector3(0,-GRAVITY,0);
	}

	if(position.z < -274/2 || position.y < -10 || position.x < -152/2 || position.x > 152/2){

		if(negZTracker == 1 & posZTracker == 1){
			if(textTracker == 0){
				text = "Nice shot - your point!";
				textTracker += 1;
				playerScore += 1;

				stringstream score1;
 				stringstream score2;
 				score1 << playerScore;
 				score2 << opponentScore;
 				string playerString = score1.str();
 				string opponentString = score2.str();
 				cout << "Player Score: " + playerString + " - Opponent Score: " + opponentString << "\n";
			}	
		}

		else if(negZTracker == 1 & posZTracker > 1){
			if(textTracker == 0){
				text = "Bounced on your side before going over the net - opponent's point";
				textTracker += 1;
				opponentScore += 1;

				stringstream score1;
 				stringstream score2;
 				score1 << playerScore;
 				score2 << opponentScore;
 				string playerString = score1.str();
 				string opponentString = score2.str();
 				cout << "Player Score: " + playerString + " - Opponent Score: " + opponentString << "\n";
			}
		}

		else{
			if(textTracker == 0){
				text = "Out of bounds - opponent's point";
				textTracker += 1;
				opponentScore += 1;

				stringstream score1;
 				stringstream score2;
 				score1 << playerScore;
 				score2 << opponentScore;
 				string playerString = score1.str();
 				string opponentString = score2.str();
 				cout << "Player Score: " + playerString + " - Opponent Score: " + opponentString << "\n";
			}
		}
	}

	if(line.distance(Point3(position.x, position.y, position.z)) < 9.0){
		position.x = newPos.x;
		position.y = newPos.y;
		position.z = newPos.z-9;
		ballVelocity += Vector3(paddleVel.x*5,paddleVel.y,paddleVel.z);
	}
	
	nextPosition = position + rdt*ballVelocity + Vector3(0,0.5*rdt*rdt*-GRAVITY,0);	
	
	// Fixes Rare Case of Paddle Collision
	if((nextPosition.z > newPos.z) && (position.z < newPos.z)){
		nextPosition.z -= 2;
	}

	// Visually seeing the variables to debug code
	//cout << position.z << "ball" <<"\n";
	//cout << newPos.z << "pad" << "\n";
	//cout << position;
	//cout << nextPosition;
	//cout << newPos;
	//cout << ballVelocity;

}


void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) {
	rd->clear();


	// Draw the paddle using 2 cylinders
	rd->pushState();
	rd->setObjectToWorldMatrix(paddleFrame);
	Cylinder paddle(Vector3(0,0,-0.5), Vector3(0,0,0.5), 8.0);
	Cylinder handle(Vector3(0,-7.5,0), Vector3(0,-16,0), 1.5);
	Draw::cylinder(paddle, rd, Color3(0.5,0,0), Color4::clear());
	Draw::cylinder(handle, rd, Color3(0.3,0.4,0), Color4::clear());
	rd->popState();  


	// Draw Paddle Shadow
	rd->pushState();
	tempPaddlePosition = newPos;
	tempPaddlePosition.y = 2;
	if((tempPaddlePosition.x > -152/2+6 && tempPaddlePosition.x < 152/2-6) && (tempPaddlePosition.z > -274/2+6 && tempPaddlePosition.z < 274/2-6)){
		Cylinder shadowPaddle(Vector3(tempPaddlePosition.x,tempPaddlePosition.y,tempPaddlePosition.z),Vector3(tempPaddlePosition.x,tempPaddlePosition.y+.05,tempPaddlePosition.z),8.0);
		Draw::cylinder(shadowPaddle, rd, Color3(0.1,0.1,0.1), Color4::clear());
	}
	rd->popState();


	// Draw Ball
	rd->pushState();
    //Sphere sphere(Vector3(0,30,-130), 2.0);
    Sphere ball(position, 2.0);
    Draw::sphere(ball, rd, Color3::white(), Color4::clear());
	rd->popState();


	// Draw Ball Shadow
	rd->pushState();
	tempBallPosition = position;
	tempBallPosition.y = 2;
	if((tempBallPosition.x > -152/2 && tempBallPosition.x < 152/2) && (tempBallPosition.z > -274/2 && tempBallPosition.z < 274/2)){
		Cylinder shadowBall(Vector3(tempBallPosition.x,tempBallPosition.y,tempBallPosition.z),Vector3(tempBallPosition.x,tempBallPosition.y+.05,tempBallPosition.z),1.0);
		Draw::cylinder(shadowBall, rd, Color3(0.1,0.1,0.1), Color4::clear());
	}
	rd->popState();


	// Draw Table
	rd->pushState();
	Box table(Vector3(-152.5/2, 0, 274/2), Vector3(152.5/2, 2, -274/2));
	Draw::box(table, rd, Color3(0.0,0.5,0.2), Color4(1.0,1.0,1.0,1.0));
	rd->popState();


	// Draw Support
	rd->pushState();
    Box support(Vector3(-50,0,137), Vector3(50,-35,0));
    Draw::box(support, rd, Color3(0.6,0.6,0.6), Color4::clear());
	rd->popState();


	// Draw Lines
	rd->pushState();
	LineSegment middleLine = LineSegment::fromTwoPoints(Point3(0, 2, -274/2), Point3(0, 2, 274/2));
    Draw::lineSegment(middleLine, rd, Color3(1.0,1.0,1.0));
	rd->popState();

	// Draw Net
	rd->pushState();	
	// Vertical Lines
	for (double x = -91.5; x <= 91.5; x = x + 1.5)
    {
    	if (x == -91.5 || x == 91.5) {
    		LineSegment net = LineSegment::fromTwoPoints(Point3(x,17.25,0), Point3(x,2,0));
    		Draw::lineSegment(net, rd, Color3(0.5,0.5,0.5));
    	}
    	else {
    		LineSegment net = LineSegment::fromTwoPoints(Point3(x,17.25,0), Point3(x,2,0));
    		Draw::lineSegment(net, rd, Color3(0.0,0.0,0.0));
    	}
    }
    // Horizontal Lines
    for (double y = 17.25; y >= 2; y = y - 1)
    {
    	if (y == 17.25 || y == 2) {
    		LineSegment net = LineSegment::fromTwoPoints(Point3(-91.5,y,0), Point3(91.5,y,0));
    		Draw::lineSegment(net, rd, Color3(0.5,0.5,0.5));
    	}
    	else {
    		LineSegment net = LineSegment::fromTwoPoints(Point3(-91.5,y,0), Point3(91.5,y,0));
    		Draw::lineSegment(net, rd, Color3(0.0,0.0,0.0));
    	}
    }
    rd->popState();


    // Drawing Text
    rd->pushState();
    debugFont->draw3D(rd, text, CoordinateFrame(Vector3(-274/2,100,0)), 10.0, Color3::red());
    rd->popState();


	// Call to make the GApp show the output of debugDraw
	drawDebugShapes();
}



