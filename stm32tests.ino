
int lA = 0;
int started = false;
unsigned long tStarted = 0;
int counter = 0;

int threshold = 15, thisthreshold = 10;
int val = pow(2, 14); // Or pow(2, 15) / 2

struct coil
{
  int raw, mean;
  float peak, peakMax;

  void peakCalc()
  {
    if (peak <= raw)
    {
      peak = raw;
    }else if ((peak * 0.7) > raw)
    {
      peak = raw;
    }

    if (peakMax < peak)
    {
      peakMax = peak;
    }
  }
  
  coil ()
  {
    raw = mean = 0;
    peak = 0.0f;
    peakMax = 0.0f;
  }
} A, B, C;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PB1, INPUT_PULLDOWN);
  pinMode(PB0, INPUT_PULLDOWN);
  pinMode(PA7, INPUT_PULLDOWN);
}

void resetData()
{
  A = B = C = coil();
}

void mean(int &v1, int &v2)
{
  if (v1 > 3)
    v1 -= 3;
  
  v1 = (v1 + v2) / 2;
}

void setThisThreshold(int a)
{
  thisthreshold = a * 0.6;
  if (thisthreshold < threshold)
    thisthreshold = threshold;
}

void clampi(int &v, int minv, int maxv)
{
  if (v < minv)
    v = minv;

  if (v > maxv)
    v = maxv;
}

void loop() {
  // put your main code here, to run repeatedly:
  static unsigned long thresoldTime = 0;
  int tA = analogRead(PB0);
  mean(A.raw, tA);
  A.peakCalc();
  
  if (started)
  {
    int tB = analogRead(PB1);
    int tC = analogRead(PA7);
    
    mean(B.raw, tB);
    mean(C.raw, tC);
    
    B.peakCalc();
    C.peakCalc();
  }else 
    B = C = coil();

  if (!started)
  {
    bool isValidThreshold = true;
    if (A.raw > threshold)
    {
      if (thresoldTime == 0)
      {
        thresoldTime = millis();
        isValidThreshold = false;
      }
    }else
    {
      isValidThreshold = false;
      thresoldTime = 0;
    }

    isValidThreshold = isValidThreshold && millis() > (thresoldTime + 5);
    
    if (isValidThreshold && A.peakMax > A.peak)
    {
      started = true;
      setThisThreshold(A.peakMax);
      tStarted = millis();
      thresoldTime = 0;
      resetData();
    }
  }else if (tStarted > (millis() + 200))
  {
    tStarted = false;
    resetData();
  }else
  {
    if (B.peakMax > thisthreshold)
    {
      val -= 256;
      clampi(val, 0, pow(2, 15) - 1);
      started = false;
      resetData();
      Serial.println(val);
      delay((millis() - tStarted) / 3);
    }else if (C.peakMax > thisthreshold)
    {
      val += 256;
      clampi(val, 0, pow(2, 15) - 1);
      started = false;
      resetData();
      Serial.println(val);
      delay((millis() - tStarted) / 3);
    }
  }
}
