// --- Pin Definitions ---
#define STEP_X 18
#define DIR_X 19
#define STEP_Y 25
#define DIR_Y 26
#define EN_PIN 23

// Limit switches
#define X_MIN 32
#define X_MAX 33
#define Y_MIN 12
#define Y_MAX 13

// Joystick
#define JOY_X 36
#define JOY_Y 39
#define JOY_SW 27

// --- Motion Settings ---
int stepDelay = 100;  // microseconds between steps (lower = faster)
int deadzone = 300;   // joystick center tolerance

long positionX = 0;
long positionY = 0;

long maximumStepsX = 0;
long maximumStepsY = 0;

bool hitXMin;
bool hitXMax;
bool hitYMin;
bool hitYMax;

int machineActionInt;

void determineParameters()
{

  positionY = 0;

  maximumStepsX = 0;
  maximumStepsY = 0;

  hitXMin = false;
  hitXMax = false;
  hitYMin = false;
  hitYMax = false;
  
  // search max Xpoint
  digitalWrite(DIR_X, HIGH);
  while(!hitXMax)
  {
    hitXMax = digitalRead(X_MAX);
    stepMotor(STEP_X);
  }

  // search min Xpoint
  digitalWrite(DIR_X, LOW);
  while(!hitXMin)
  {
    hitXMin = digitalRead(X_MIN);
    stepMotor(STEP_X);
    maximumStepsX++;
  }
  positionX = 0;

  // search max Ypoint
  digitalWrite(DIR_Y, HIGH);
  while(!hitYMax)
  {
    hitYMax = digitalRead(Y_MAX);
    stepMotor(STEP_Y);
  }
  // search min Ypoint
  digitalWrite(DIR_Y, LOW);
  while(!hitYMin)
  {
    hitYMin = digitalRead(Y_MIN);
    stepMotor(STEP_Y);
    maximumStepsY++;
  }
  positionY = 0;
  positionX = positionX + moveSanderLeft(160, stepDelay);
  positionY = positionY + moveSanderUp(160, stepDelay);
  // set engine to position (160.160)

}

long moveSanderLeft(int steps, int speedInterval)
{
  long aux=0;

  digitalWrite(DIR_X,LOW);
  for (int x=0;x<steps;x++)
  {
    if (digitalRead(X_MIN) == HIGH) {
      stepMotor(STEP_X);
      aux++;
    }
  }
  return aux;
}

long moveSanderRight(int steps, int speedInterval)
{
  long aux=0;

  digitalWrite(DIR_X,HIGH);
  for (int x=0;x<steps;x++)
  {
    if (digitalRead(X_MAX) == HIGH) {
      stepMotor(STEP_X);
      aux++;
    }
  }
  return aux;
}

long moveSanderUp(int steps, int speedInterval)
{
  long aux=0;

  digitalWrite(DIR_Y,LOW);
  for (int x=0;x<steps;x++)
  {
    if (digitalRead(Y_MIN) == HIGH) {
      stepMotor(STEP_Y);
      aux++;
    }
  }
  return aux;
}

long moveSanderDown(int steps, int speedInterval)
{
  long aux;

  digitalWrite(DIR_Y,HIGH);
  for (int x=0;x<steps;x++)
  {
    if (digitalRead(Y_MAX) == HIGH) {
      stepMotor(STEP_Y);
      aux++;
    }
  }
  return aux;
}

int getNumber(String prompt)
{
   int aux;
   char dummy[10];
   byte dummyBytes;
   Serial.print(prompt);
   while (Serial.available() == 0){}
   aux = Serial.parseInt();
   while (Serial.available() != 0)
   {
      dummyBytes = Serial.readBytes(dummy, 1);
   }
   Serial.println(aux);

   return aux;
}

int getAction(){

  int aux;

  Serial.println(F("Requested Action"));
  Serial.println(F("1 = Set motorspeed (50-800)"));
  Serial.println(F("2 = Joystick action"));
  Serial.println(F("3 = Set area / Home"));
  Serial.println(F("4 = Full sweep"));
  Serial.println(F("5 = test 3200 left"));
  Serial.println(F("6 = test 3200 right"));
  Serial.println(F("7 = test 3200 up"));
  Serial.println(F("8 = test 3200 down"));
      
  aux = getNumber ("?");

   return aux;
}

void fullSweep()
{
  determineParameters();
  for (int x=0;x<maximumStepsX;x=x+6400)
  {
    positionY = positionY + moveSanderUp(maximumStepsY, stepDelay);
    positionX = positionX + moveSanderLeft(3200, stepDelay);
    positionY = positionY - moveSanderDown(maximumStepsY, stepDelay);
    positionX = positionX + moveSanderLeft(3200, stepDelay);
  }
  for (int x=0;x<maximumStepsX;x=x+6400)
  {
    positionY = positionY + moveSanderUp(maximumStepsY, stepDelay);
    positionX = positionX - moveSanderRight(3200, stepDelay);
    positionY = positionY - moveSanderDown(maximumStepsY, stepDelay);
    positionX = positionX - moveSanderRight(3200, stepDelay);
  }
}

void stepMotor(int stepPin) {
  digitalWrite(stepPin, HIGH);
  delayMicroseconds(stepDelay);
  digitalWrite(stepPin, LOW);
  delayMicroseconds(stepDelay);
}

bool moveWithJoyStick()
{
  bool stayInLoop = true;

  while (stayInLoop)
  {
    int joyX = analogRead(JOY_X);
    int joyY = analogRead(JOY_Y);

    // --- X Axis ---
    if (joyX < 2048 - deadzone) {  // move left
      Serial.print("Move left");
      Serial.println((digitalRead(X_MIN) == HIGH)?"-Free":"-Blocked");
      if (digitalRead(X_MIN) == HIGH) {
        digitalWrite(DIR_X, LOW);
        stepMotor(STEP_X);
      }
    } else if (joyX > 2048 + deadzone) { // move right
      Serial.print("Move right");
      Serial.println((digitalRead(X_MAX) == HIGH)?"-Free":"-Blocked");
      if (digitalRead(X_MAX) == HIGH) {
        digitalWrite(DIR_X, HIGH);
        stepMotor(STEP_X);
      }
    }

    // --- Y Axis ---
    if (joyY < 2048 - deadzone) {  // move down
      Serial.print("Move down");
      Serial.println((digitalRead(Y_MIN) == HIGH)?"-Free":"-Blocked");
      if (digitalRead(Y_MIN) == HIGH) {
        digitalWrite(DIR_Y, LOW);
        stepMotor(STEP_Y);
      }
    } else if (joyY > 2048 + deadzone) { // move up
      Serial.print("Move up");
      Serial.println((digitalRead(Y_MAX) == HIGH)?"-Free":"-Blocked");
      if (digitalRead(Y_MAX) == HIGH) {
        digitalWrite(DIR_Y, HIGH);
        stepMotor(STEP_Y);
      }
    }

    // Joystick button (optional)
    if (digitalRead(JOY_SW) == LOW) {
      Serial.println("Joystick button pressed");
      stayInLoop = false;
    }
  }
}

void handleMachineAction(int action)
{
  switch (action)
  {
    case 1:
      stepDelay = getNumber ("number of microseconds delay: ");
      break;
    case 2 :
      moveWithJoyStick();
      break;
    case 3:
      determineParameters();
      Serial.print("Maximum steps X axis: ");
      Serial.println(maximumStepsX);
      Serial.print("Maximum steps Y axis: ");
      Serial.println(maximumStepsY);
      Serial.print("Position : (");
      Serial.print(positionX);
      Serial.print(",");
      Serial.print(positionY);
      Serial.println(")");
      break;
    case 4:
      fullSweep();
      break;
    case 5:
      positionX = 0;
      positionX = positionX + moveSanderLeft(3200, stepDelay);
      Serial.print("Position : (");
      Serial.print(positionX);
      Serial.print(",");
      Serial.print(positionY);
      Serial.println(")");
      break;
    case 6:
      positionX = 0;
      positionX = positionX - moveSanderRight(3200, stepDelay);
      Serial.print("Position : (");
      Serial.print(positionX);
      Serial.print(",");
      Serial.print(positionY);
      Serial.println(")");
      break;
    case 7:
      positionY = 0;
      positionY = positionY + moveSanderUp(3200, stepDelay);
      Serial.print("Position : (");
      Serial.print(positionX);
      Serial.print(",");
      Serial.print(positionY);
      Serial.println(")");
      break;
    case 8:
      positionY = 0;
      positionY = positionY - moveSanderUp(3200, stepDelay);
      Serial.print("Position : (");
      Serial.print(positionX);
      Serial.print(",");
      Serial.print(positionY);
      Serial.println(")");
      break;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(STEP_X, OUTPUT);
  pinMode(DIR_X, OUTPUT);
  pinMode(STEP_Y, OUTPUT);
  pinMode(DIR_Y, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  digitalWrite(EN_PIN, LOW); // enable drivers

  pinMode(X_MIN, INPUT_PULLUP);
  pinMode(X_MAX, INPUT_PULLUP);
  pinMode(Y_MIN, INPUT_PULLUP);
  pinMode(Y_MAX, INPUT_PULLUP);

  pinMode(JOY_SW, INPUT_PULLUP);
}

void loop() {

  machineActionInt = getAction();
  handleMachineAction (machineActionInt);
}
