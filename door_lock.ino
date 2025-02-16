

// by hichem belarbi 

#include <LiquidCrystal_I2C.h> 
#include <Keypad.h>

#include <Servo.h>


int ledpin =12;// a pin for a led that indicates the door state (open or closed - on:open _ off:closed)
int servopin=11;
int resetpin=10; // a pin for the reset button 

// define a servo motor to modolise the door lock
Servo lock_servo;

// defining the keypad parametres
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns
LiquidCrystal_I2C lcd(0x27, 20, 4);
char keys[ROW_NUM][COLUMN_NUM] = {
{'1','2','3', 'A'},
{'4','5','6', 'B'},
{'7','8','9', 'C'},
{'d','0','o', 'D'}
};


// an array to store the code
char code[5]={'0','0','0','0','0'};
// an array to store the code typed by the user 
char typed_code[5]={'-','-','-','-','-'};
// a couter to count the number of charecters typed
int code_counter=0;
// a bool to verify if there is a code or we have to creat one 
int code_is_set=0;
// a bool to detecte the reset
int reset_state=HIGH;

// a bool to save the door state (open or closed)
int door_state=0;
// configuring the keypad pins
byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
// a keypad object
Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

void setup(){
  lcd.init(); // initialisation of the displayer
  lcd.clear();
  lcd.backlight(); 
  lcd.setCursor(0, 0); 
  lock_servo.attach(servopin);
  pinMode(ledpin, OUTPUT);
  pinMode(resetpin,INPUT_PULLUP);

  lcd.print("DOOR LOCK.");// the welcomming message
  delay(2000);
  lcd.clear();
  lcd.print("enter new code:");
  delay(2000);
  lcd.clear();
  lcd.print("*:delete  #:ok"); // *:delete the last charctere shown _ #: confirm the code typed
  lcd.setCursor(0,1);

  lcd.print("> -----"); // a patern to show that the code can support 5 charecteres only 
lcd.setCursor(2,1);

}

void loop(){
  char key = keypad.getKey();// reading the charctere typed 
  int reset=digitalRead(resetpin); // reading the reset pin 
  if (reset==LOW)
  {
    reset_state=LOW;
    show_reset_setup(); // showing a message asking to write the old code before resetting the code 
  }

  
  if ((code_counter<5   || key=='d') && key!='o' && code_is_set==0 )// seting a new code 
  {
  
  
  if(key && key!='d') // a charctere other than d which means delete 
  {
    lcd.print(key);  // showing the charctere typ ed 
    delay(100);
    code[code_counter]=key;// add the charctere to the array that stores the code 
    code_counter++;
  }
  else if (key && key=='d' && code_counter>0)  {
    delete_char();
  }
  
  }



  else if ( (code_counter<5   || key=='d') && key!='o' && code_is_set==1)// xhecking the code typed (compairing it with the code stored)
  {

    
  if(key && key!='d') 
  {
    lcd.print(key);  
    delay(100);
    typed_code[code_counter]=key;
    code_counter++;
  }
  else if (key && key=='d' && code_counter>0)  {
    delete_char();
  }

  
  }




  else if (key=='o' ) // confirming the typed code 
  {
    switch (code_is_set) // is it a new code setting or a typed code check
    {
      case 1: // first case : cheching the code typed 
        if (chech_code())// the code is coorect 
        {
          lcd.clear();
          lcd.print("success !");
          delay(1000);
          if (reset_state==HIGH) // not resetting the code 
          {
            switch (door_state)  // is the door open or closed 
                {
              case 1: // the door is closed 
                close_door();
                break;
              case 0:// the door is open 
                open_door();
                break;
            }
            }
            else{                 // resetting the code 
              code_is_set=0;
              lcd.clear();
              lcd.print("enter new code:");
              lcd.setCursor(0,1);

              lcd.print("> -----"); // a patern to show that the code can support 5 charecteres only 
              lcd.setCursor(2,1);
              reset_state=HIGH;

            }
            clear_typing(); // cleaning the typed code 
          
        }
        else{
          wrong_code_flash(); // showing that the code typed is incorrect 
        }
      break;
      case 0: // setting a new code 
        if (code_counter<5) // the code is less than 5 charcteres 
        {
          short_code_flash(); /// showing  that the code is short 
        }
        else  { // code is valid 
          code_set(); // set the new code 
          open_door();// open the door , we suppose its initially closed 
        }
        break;
    }

  }


}

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


void clear_typing() // a function to clean the typed code , initialisation
{
  for(int i=0;i<5;i++)
  {
    typed_code[i]='-';
  }
  code_counter=0;
}





void show_reset_setup() // a function to show a message to guide the user to enter the old code before setting the code 
{

  lcd.clear();
  lcd.print("enter old code:");
  lcd.setCursor(0,1);
  lcd.print("> -----");
  lcd.setCursor(2,1);

  clear_typing();
}




void delete_char() // a function to delete the last charactere typed 
{
    code_counter--;
    lcd.setCursor(2+code_counter,1);
    lcd.print("-");
    lcd.setCursor(2+code_counter,1);
}



void short_code_flash() // a function to alert that the code typed is short and needs to be 5 charcteres exactly 
{
  // show error 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("code must have");
        lcd.setCursor(0,1);
        lcd.print("5 characteres !");
        delay(3000);
  // returning 
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("*:delete  #:ok");
        lcd.setCursor(0,1);

        lcd.print("> -----");
lcd.setCursor(2,1);
        for (int i=0;i<code_counter;i++)
        {
          lcd.print(code[i]);
        }
      }

void wrong_code_flash()// a function to alert that the typed code is incorrcte 
{

    lcd.setCursor(0,1);
    lcd.print("incorrect !    ");

    delay(3000);
// returning 


    lcd.setCursor(0,1);

    lcd.print("> -----     ");
    lcd.setCursor(2,1);
    for (int i=0;i<code_counter;i++)
    {
      lcd.print(typed_code[i]);
    }
}



void code_set() // storing the new code 
{

      lcd.clear();
      lcd.print("Success !");
      delay(1500);
      code_is_set=1;
      code_counter=0;
}

void open_door() // a function to turn the servo modeling the lock opening 
{
  lcd.clear();
  lock_servo.write(180); // changing the servo position to 180°
  lcd.print("opening...");
  delay(2000); // a delay to assure that the door is really open 
  lcd.clear();
  lcd.print("door is open !");
  lcd.setCursor(0,1);
  door_state=1;
  delay(2000);

  lcd.clear();
  lcd.print("code to close :");
  lcd.setCursor(0,1);
  lcd.print("> -----");
  lcd.setCursor(2,1);

  digitalWrite(ledpin,HIGH);
}


void close_door() // a function to turn the servo modeling the lock closing 
{
  lcd.clear();
  lock_servo.write(0);
  lcd.print("closing...");
  delay(2000);
  lcd.clear();
  lcd.print("door is closed !");
  lcd.setCursor(0,1);
  door_state=0;
  delay(2000);

  lcd.clear();
  lcd.print("code to open :");
  lcd.setCursor(0,1);
  lcd.print("> -----");
  lcd.setCursor(2,1);

  digitalWrite(ledpin,LOW);
}




int chech_code() // a function to compaire the typed code with code stored 
{
  for (int i=0;i<5;i++)
  {
    if (code[i]!=typed_code[i])
    {
      return 0;
    }
  }
  return 1;
}
