#include <Adafruit_MCP4725.h>
#include <Wire.h>
#include <TimerOne.h>  
//interrupt code

volatile long echo_start = 0;
volatile long echo_end = 0;
volatile long echo_duration = 0;
int steadyCount=0;

int rc_output = A7;
int source_input = A6;
int flag = 0;

int16_t Output_DAC_int,Input_Source_int,output_DAC;
float  Output_DAC_volts,Input_Source_volts,output,K,output_DAC_float,error;

Adafruit_MCP4725 DAC;
#define MCP4725_ADDR 0x60 

void setup() {
  // put your setup code here, to run once:
 
  Serial.begin(9600);
  Serial.println("Arduino Ready");
  delay(100);

  Timer1.initialize(10);
  Timer1.attachInterrupt(interrupt_readVal);

  Wire.begin();
  sei();  // Enable interrupts
  //DAC.setVoltage(0,false);
}

void loop() {
  K=1;
/*
  // Source Voltage Reading
  if(flag == 1) {
    //Input_Source_int=analogRead(source_input); // Reads the analog quanitzed bit which is an INT
    Input_Source_volts=(Input_Source_int*0.004887);  // Voltage converted to voltage which is a FLOAT
    Serial.print("Input Voltage: ");
    Serial.println(Input_Source_volts);
    flag = 0;
  }
*/
  // RC Filter Voltage Reading
  if(flag == 2) {
    //Input_Source_int=analogRead(source_input); // Reads the analog quanitzed bit which is an INT
    Input_Source_volts=(Input_Source_int*0.004887);  // Analog converted to voltage which is a FLOAT
    //Serial.print("Input Voltage: ");
    //Serial.println(Input_Source_volts);

    //Output_ADC_int=analogRead(rc_output); // Reads the analog quanitzed bit which is an INT
    Output_DAC_volts=(Output_DAC_int*0.004887);  // Analog converted to voltage which is a FLOAT
    //Serial.print("Input Voltage: ");
    //Serial.println(Output_DAC_int);

    flag = 0;
  }

    // Error between Source Voltage - RC Filter Voltage
    error=Input_Source_volts-Output_DAC_volts; // Input - Output voltage after conversion
    //Serial.print("error: ");
    //Serial.println(error);
    
    if (abs(error) < 0.01){ // If the difference is nearly zero (under 0.01V difference)
      steadyCount += 1;
    };
    
    // DAC Output Voltage Conversion
    output=(K*error)+(Input_Source_volts); // Summer and Proportional Gain Equation = new_Output
    output_DAC_float= abs(output/0.00122);  // DAC float to int conversion: 5V_ref/4096(resolution) = 0.00122
    output_DAC=(int) output_DAC_float; // convert new_Output to int
    //Serial.print("Output voltage: ");
    //Serial.println(output);
    Wire.beginTransmission(MCP4725_ADDR);
    Wire.write(64);                     // cmd to update the DAC
    Wire.write(output_DAC >> 4);        // the 8 most significant bits...
    Wire.write((output_DAC & 15) << 4); // the 4 least significant bits...
    Wire.endTransmission();

    //interrupt_readVal(); // WE NEED TO WRITE AN INTERRUPT FUNCTION 
 
}

//interrupt_readVal() External interrupt
//Called every time the feedbacK Voltage equals the Input Voltage 
void interrupt_readVal()
{
    
    //Input_Source_int=analogRead(source_input);
    //delay(1);
    Input_Source_int=analogRead(source_input);
    Input_Source_int=analogRead(source_input);
    Input_Source_int=analogRead(source_input);
    //delay(5);
    //flag = 1;

    //Output_DAC_int=analogRead(rc_output);
    //delay(1);
    Output_DAC_int=analogRead(rc_output);
    Output_DAC_int=analogRead(rc_output);
    Output_DAC_int=analogRead(rc_output);
    //delay(5);
    flag = 2;

    
    if (steadyCount > 3){                                          
      echo_end = micros();            // Timestamp for when interrupt occured
      echo_duration = echo_end - echo_start; // calculate the closed loop time (timestampe of interrupt - timestamp of program start)
      Serial.println("Time to reach stability : ");
      Serial.println(echo_duration);
      echo_start = micros(); // new timestamp start of Program 
      steadyCount = 0;
    }
  }

