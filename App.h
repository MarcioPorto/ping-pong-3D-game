/**
  Comp 394-01 S15 
  Assignment #2 Ping Pong 3D
  Marcio Porto and Charles Park
**/

#ifndef App_h
#define App_h

#include <G3D/G3DAll.h>

class App : public GApp {
public:
    
  App(const GApp::Settings& settings = GApp::Settings());

  virtual void onInit();

  virtual void onUserInput(UserInput *uinput); 
  
  virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt);
  
  virtual void onGraphics3D(RenderDevice* rd, Array< shared_ptr<Surface> >& surface);


  // Use these functions to access the current state of the paddle!
  Vector3 getPaddlePosition() { return paddleFrame.translation; }
  Vector3 getPaddleNormal() { return Vector3(0,0,-1); }
  Vector3 getPaddleVelocity() { return paddleVel; }


private:
  // Returns the ball location at a specific time

  static const double GRAVITY;

  Vector3 position;
  Vector3 nextPosition;
  Vector3 previousPosition;
  Vector3 ballVelocity;
  Vector3 tempPaddlePosition;
  Vector3 tempBallPosition;
    
  double initSpeed;
  double initAngle;
  Vector3 initVelocity;

  Vector3 newPos;
  Vector3 prePos;

  String text;

  int negZTracker;
  int posZTracker;
  int textTracker;
  int playerScore;
  int opponentScore;
  

protected:

  // This CoordinateFrame stores position and rotation data for the paddle.
  CoordinateFrame paddleFrame;
  
  // This vector stores the paddle's current velocity.
  Vector3 paddleVel;
};

#endif
