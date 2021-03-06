
#include <Pixy2.h>

#define solenoid A5
#define BAM moveForward(4); digitalWrite(A5, HIGH); delay(300); digitalWrite(A5, LOW);
#define secretDataType byte

int defaultSpeed = 55;
double leftPWMSpeed = 40;
double rightPWMSpeed = 40;

int in1 = 7;
int in2 = 8;
int in3 = 6;
int in4 = 4;

int pwmRight = 9;
int pwmLeft = 5;

int encoderLeft = 3;
int encoderRight = 2;

Pixy2 pixy;

int pwmOutputA = defaultSpeed;
int pwmOutputB = defaultSpeed;

int rightCount = 0;
int rightPrevVal;

int leftCount = 0;
int leftPrevVal;

boolean ballFound = false;
boolean ballInSight = false;

//long leftTimeStamps[20];
//long rightTimeStamps[20];

double leftVel;
double rightVel;
int leftHead = 0;
int rightHead = 0;

int ball_y = 0;

//conversion of encoders to degrees
int coF = 9000;
                                                                                     
long currentTime = 0;
long previousTime = 0;

//used for determining wheel velocities
int timeHead = 0;
double leftRates[20];
double rightRates[20];

int lastEncRight = 0;
int lastEncLeft = 0;

boolean firstRight = true;
boolean firstLeft = true;

long lastRightDerivativeTime = 0;
long lastLeftDerivativeTime = 0;

int prevY;


long timez[100];
int spedz[100];
int count = 0;

void setup() {
  // put your setup code here, to run once:

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(solenoid, OUTPUT);

  pinMode(encoderLeft, INPUT);
  pinMode(encoderRight, INPUT);

  attachInterrupt(digitalPinToInterrupt(encoderLeft), incrementLeft, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderRight), incrementRight, CHANGE);

  pinMode(pwmRight, OUTPUT);
  pinMode(pwmLeft, OUTPUT);

  analogWrite(pwmRight, pwmOutputA);//Right
  analogWrite(pwmLeft, pwmOutputB);//Left

  Serial.begin(9600);

  pixy.init();
}

int index;
int mode = 0;

void loop() {

  switch (mode) {
    case 0:
      //search mode 
      Serial.println("mode = 0");

      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);

      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);

      ballInSight = false;

      for (int i = 0; i < pixy.ccc.getBlocks(); i ++)
      {
        if (pixy.ccc.blocks[i].m_signature == 1)
        {
          index = i;
          ballFound = true;
          ballInSight = true;

          analogWrite(pwmRight, 0);
          analogWrite(pwmLeft, 0);
        }
      }

      if (pixy.ccc.getBlocks() > 0 && ballInSight)
      {
        mode = 1;
        firstRight = true;
        firstLeft = true;
      }

      else
      {
        moveRightWheel(360);
        moveLeftWheel(0);
        firstRight = false;
        firstLeft = false;
      }

      break;
    case 1: //hunt mode
      {


        Serial.print("mode = 1 ");
        Serial.println(prevY);

        
        if (pixy.ccc.getBlocks() <= 0)
        {
          if (prevY > 200)
          {
            firstRight = true;
            firstLeft = true;
//            for(int i = 5; i >= 0; i--)
//            {
//              pwmOutputA-=pwmOutputA/i;
//              pwmOutputB-=pwmOutputB/i;
//              analogWrite(pwmRight, pwmOutputA);
//              analogWrite(pwmLeft, pwmOutputB);
//              delay(100);
//            }
            
            mode = 2;
          }

          else
            mode = 0;
          break;
        }

        int angle = 60 * (pixy.ccc.blocks[index].m_x / 316. ) - 30;

        //        analogWrite(pwmRight, defaultSpeed);
        //        analogWrite(pwmLeft, defaultSpeed);
        ball_y = pixy.ccc.blocks[index].m_y;

        pwmOutputA = 50 -  angle;
        pwmOutputB = 80 +  2 * angle;

        double distanceConstantA = .1;
        double distanceConstantB = .16;

        if (ball_y > 150)
        {
          pwmOutputA -= ball_y * distanceConstantA;
          pwmOutputB -= ball_y * distanceConstantB;
        }


        //        moveRightWheel(360 - 3 * angle);
        //        moveLeftWheel(360 + 3 * angle);
        analogWrite(pwmRight, pwmOutputA);
        analogWrite(pwmLeft, pwmOutputB);




        firstRight = false;
        firstLeft = false;

        prevY = ball_y;




        break;
      }

    case 2:
      {
        //pounce


        Serial.println("mode = 2");


        firstRight = false;
        firstLeft = false;
        secretDataType goals = 0;
        boolean weSeeDaBall = false;
        int avgAngle = 0;
        int usefulThing = 999;
        int anotherUsefulThing = 0;
        for (int i = 0; i < pixy.ccc.getBlocks(); i ++)
        {
          if (pixy.ccc.blocks[i].m_signature == 2)
          {
            index = i;
            avgAngle += 60 * (pixy.ccc.blocks[i].m_x / 316. ) - 30;
            goals++;
            if(usefulThing == 999)
              usefulThing = i;
          }

          if (pixy.ccc.blocks[i].m_signature == 1)
          {
            weSeeDaBall = true;
            ball_y = pixy.ccc.blocks[i].m_y;
          }
        }
        avgAngle /= goals;
        boolean seesBoth = abs(60 * (pixy.ccc.blocks[usefulThing].m_x/ 316. ) - 30 - avgAngle) > 3;

        
        if (abs(avgAngle) > 5 || goals == 0)
        {
//          moveRightWheel(240);
//          moveLeftWheel(150);
          analogWrite(pwmLeft, 54);
          analogWrite(pwmRight, 34);
          
          
          if (weSeeDaBall)
          {
            mode = 1;
            break;
          }

          firstRight = false;
          firstLeft = false;
        }
        Serial.print("     Goals: ");
        Serial.print(goals);
        Serial.print("     We see the ball: ");
        Serial.print(weSeeDaBall);
        Serial.print("     SeesBoth: ");
        Serial.println(seesBoth);

        if (goals >= 2 && (ball_y > 190 || !weSeeDaBall) && seesBoth)
        {
          mode = 3;
        }



        break;
      }
    case 3:
      Serial.print("mode = 3");
      
      BAM;//!!!!!!!!!
      BAM;//!!!!!!!!!
      delay(10000);
      mode = 0;
      break;
    default:
      break;
  }
  //Serial.println(rightTimeStamps[(rightHead)] - rightTimeStamps[(rightHead+1)]%(sizeof(rightTimeStamps) / sizeof(long)));
  // put your main code here, to run repeatedly:

  //  Serial.print(leftVel * 1000/40);
  //  Serial.print("       ");
  //  Serial.print(rightVel *  1000/40);
  //  Serial.print("       ");
  //  Serial.print(40/rightVel);
  //  Serial.print("       ");

  //  int rh = rightHead;
  //  for (int i = 0; i < 10; i++)
  //  {
  //    //Serial.print(rightTimeStamps[i]);
  //    if (i == rh)
  //      Serial.print("<--");
  //    else {
  //      Serial.print(" , ");
  //    }
  //
  //  }
  //  Serial.println();
  //




  int len = sizeof(leftRates) / sizeof(double);
  //Serial.print(len);
  //Serial.print("      ");
  currentTime = millis();

  leftRates[timeHead] =  (double) leftCount / (currentTime - previousTime);
  rightRates[timeHead] =  (double) rightCount / (currentTime - previousTime);



  timeHead++;
  timeHead %= len;

  leftCount = 0;
  rightCount = 0;
  previousTime = currentTime;

  double sumRight = 0;
  double sumLeft = 0;
  for (int i = 0; i < len ; i ++)
  {
    sumRight += rightRates[i];
    sumLeft += leftRates[i];
  }
  rightVel = sumRight / len;
  leftVel = sumLeft / len;

  //  Serial.print("   ");
  //  Serial.print(rightVel * coF);
  //  Serial.print("   ");
  //  Serial.println(leftVel * coF);
  //
  //  if (count % 10 == 0)
  //  {
  //    timez[count / 10] = currentTime;
  //    spedz[count / 10] = leftVel * coF;
  //  }
  //  count++;
  //  if (currentTime > 20000)
  //  {
  //    Serial.print("[");
  //    for (int i = 0; i < 100; i++)
  //    {
  //
  //      Serial.print(timez[i]);
  //      if (i < 99)
  //        Serial.print(", ");
  //    }
  //    Serial.println("]");
  //
  //    Serial.print("[");
  //    for (int i = 0; i < 100; i++)
  //    {
  //
  //      Serial.print(spedz[i]);
  //      if (i < 99)
  //        Serial.print(", ");
  //    }
  //    Serial.println("]");
  //    delay(6238164731206574819);
  //  }
}

void incrementLeft()
{
  int b = digitalRead(encoderLeft);
  if (b != lastEncLeft) {
    leftCount ++;
    lastEncLeft = 1 - lastEncLeft;
    /*
      int len = sizeof(rightTimeStamps) / sizeof(long);
      rightTimeStamps[(rightHead)] = millis();
      rightVel =  (double)len / (rightTimeStamps[(rightHead)] - rightTimeStamps[(rightHead + 1) % len]);

      rightHead++;
      rightHead %= len;
    */
  }
  delay(1);

}

void incrementRight()
{
  int b = digitalRead(encoderRight);
  if (b != lastEncRight) {
    rightCount ++;
    lastEncRight = 1 - lastEncRight;
    /*
      int len = sizeof(rightTimeStamps) / sizeof(long);
      rightTimeStamps[(rightHead)] = millis();
      rightVel =  (double)len / (rightTimeStamps[(rightHead)] - rightTimeStamps[(rightHead + 1) % len]);

      rightHead++;
      rightHead %= len;
    */
  }
  delay(1);
}


void moveForward(int distance) //inches
{
  leftCount = 0;
  rightCount = 0;

  int counts = (int) (((double)distance / 0.19625) + 0.5);
  //Serial.println(counts);

  analogWrite(pwmRight, 30);
  analogWrite(pwmLeft, 50);

  while (rightCount < counts || leftCount < counts)
  {
    int diffCount = leftCount - rightCount;

    Serial.print("LeftCount: ");
    Serial.print(leftCount);
    Serial.print("   Right Count: ");
    Serial.println(rightCount);

    if (diffCount > 0)
    {
      analogWrite(pwmRight, defaultSpeed + 10 * diffCount);
    }

    else if (diffCount < 0)
    {
      analogWrite(pwmLeft, defaultSpeed - 10 * diffCount);
    }

    if (rightCount > counts)
      analogWrite(pwmRight, 0);
    if (leftCount > counts)
      analogWrite(pwmLeft, 0);
  }

  analogWrite(pwmRight, 0);
  analogWrite(pwmLeft, 0);

}
/*
  void huntMode()
  {
  analogWrite(pwmRight, 0);
  analogWrite(pwmLeft, 0);
  delay(1000);

  leftCount = 0;
  rightCount = 0;

  while (true)
  {
    if (!(pixy.ccc.getBlocks() > 0 && ballInSight))
      return;
    int angle = 60 * (pixy.ccc.blocks[index].m_x / 316. ) - 30;

    int desiredVel = 45;

    //    analogWrite(pwmRight, defaultSpeed);
    //    analogWrite(pwmLeft, defaultSpeed);

    //    pwmOutputA = defaultSpeed - 1 * angle;
    //    pwmOutputB = defaultSpeed + 1 * angle;

    moveRightWheel(defaultSpeed - angle);
    moveLeftWheel(defaultSpeed + angle);

    ball_y = pixy.ccc.blocks[index].m_y;

    analogWrite(pwmRight, pwmOutputA);
    analogWrite(pwmLeft, pwmOutputB);
  }
  }
*/
int prevRightError = 10000000;

void moveRightWheel(double desiredVel)
{
  long currentTime = millis();

  if (prevRightError = 10000000)
    prevRightError = desiredVel;
  double measuredVel = (double)coF * rightVel;
  int e = desiredVel - measuredVel;
  if (firstRight)
  { //16x - 220
    rightPWMSpeed = 0.041 * desiredVel + 26.8;
    //    Serial.println("HI");
  }


  rightPWMSpeed += (e) * 0.001 + 0.000 * (e - prevRightError) / (currentTime - lastRightDerivativeTime);

  //  Serial.print("     ");
  //  Serial.print("right    ");
  //  Serial.print(rightPWMSpeed);
  //  Serial.print("     ");
  //  Serial.print("der    ");
  //  Serial.print((e - prevRightError) / (currentTime - lastRightDerivativeTime));
  //  Serial.print("      ");

  rightPWMSpeed = min(200, max(0, rightPWMSpeed));
  analogWrite(pwmRight, (int) rightPWMSpeed);
  prevRightError = e;

  //  Serial.print("wanted right = ");
  //  Serial.print(rightVel);
  //  Serial.print("      ");
  lastRightDerivativeTime = currentTime;
}

int prevLeftError = 100000000000;

void moveLeftWheel(double desiredVel)
{ //12x - 507
  if (prevLeftError == 100000000000)
    prevLeftError = desiredVel;

  if (firstLeft)
  {
    leftPWMSpeed = 0.0775 * desiredVel + 46.2;
  }
  double measuredVel = (double)coF * leftVel;
  //Serial.print(" measured ");
  //Serial.print(measuredVel);
  //  Serial.print("     ");
  //  Serial.print("left    ");
  //  Serial.print(leftPWMSpeed);
  //  Serial.print("      ");

  double k = 0.001;
  int currentTime = millis();

  int e = desiredVel - measuredVel;
  //  if(e >0)
  //    k=0.003;

  leftPWMSpeed += e * k + 0 * (e - prevLeftError) / ((double)(currentTime - lastLeftDerivativeTime));
  leftPWMSpeed = min(200, max(0, leftPWMSpeed));
  analogWrite(pwmLeft, (int)leftPWMSpeed);
  //Serial.print("der    ");
  //Serial.print((e-prevLeftError)/((double)(currentTime - lastLeftDerivativeTime)));
  //Serial.print("      ");
  prevLeftError = e;
  //Serial.print(" wanted left = ");
  //Serial.print(leftPWMSpeed);
  lastLeftDerivativeTime = currentTime;

}

