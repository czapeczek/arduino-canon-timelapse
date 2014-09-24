
#include <LiquidCrystal.h>

//some initial definitions
int lcd_key     = 0;
int adc_key_in  = 0;
int time;
int shot_interval = 0;
boolean show_menu = true;

//menu 
char* menu_items[4] = {"Set Interval","Set Amount","Go!", "STOP!"};
int menu_count = 4;
int actual_item = 0;

// a set of intervals to choose from interface
int intervals[11] = {1, 2, 3, 5, 8, 12, 20, 24, 30, 60, 100};
int int_count = 11;
int actual_interval = 2;

// a set of types of intervals to choose from interfave, it lets us enwise the interval spectrum without a long list of intervals
char* interval_units[3] = {"   Seconds    ", "   Minutes    ", "    Hours     "};
long intervals_multiplier[3] = {1000, 60000, 3600000};
int int_units_count = 3;
int actual_unit = 0;

// a sets of amounts, we can choose shooting time or number of shots to be taken
char* amount_types[] = {"    Times     ", "   Seconds    ", "   Minutes    ","    Hours     "};
int amount_types_count = 4;
int amount_types_units[4] = {0,1000, 60000, 3600000};
int actual_amount_type = 0;

// a set of amount to be chosen from the interface
long amounts[] = {5, 10, 24, 50, 100, 200, 500, 1000, 2000, 5000, 10000};
int amounts_count = 11;
int actual_amount = 0;


long int_millis; // interval trigger variable
long counter = 0; 
long shots_counter = 0;

#define shotPin 3

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define screenRows 2
#define screenCols 16

// read the buttons, this is the methid i borrowed from example use site and i decided to leave it, so i havent change it.
int read_LCD_buttons() {
 adc_key_in = analogRead(0);
 // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  

 // For V1.0 comment the other threshold and use the one below:
/*
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
*/
 return btnNONE;  // when all others fail, return this...
}

// initialize the lcd
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


// this method simply sets signal on a defined pin connected to opto transistor which is wiring cables connected to my canon
void shoot_once() {
      digitalWrite(shotPin,HIGH);
      //hold this state to let the camera shoot the photo, if the signal is too short, it may not 'feel' it
      delay(250);
      digitalWrite(shotPin,LOW);
      shots_counter++; // increase the counter to show how many photos have already been taken
}

void setup() {
  lcd.begin(screenCols,screenRows);
  pinMode(shotPin,OUTPUT);
}

void menu() {
  
  lcd.home();
  // for all items of menu, print them. For actual selected menu item, add "<" at the end, to let user see what is selected.
  for(int i = 0, j = actual_item; i < screenRows && j < menu_count; i++) {
    lcd.setCursor(0,i);
    String menu_item = String(j+1);
    menu_item += ".";
    menu_item += menu_items[j];
    if(j == actual_item) {
      menu_item += " <";
    }
    lcd.print(menu_item);
    j++;
  }
  // simply listen to buttons and change the actual selected item
  switch (read_LCD_buttons()) {  
    case btnDOWN: {
      actual_item++;
      lcd.clear();
      delay(150);
      break;
    }
    case btnUP: {
      actual_item--;
      lcd.clear();
      delay(150);
      break;
    }
    case btnSELECT: {
      select();
      break;
    }
  }
  
  // avoid the overflow
  if(actual_item >= menu_count || actual_item < 0) {
    actual_item = 0;
  }
  
}

// enter or leave menu after pressing select
void select() {
  lcd.clear();
  delay(150);
  show_menu = !show_menu;
}

// set the interval, pressing right-left changes the type, up-down value
void set_interval() {
  
  lcd.home();
  lcd.print("<");
  lcd.print(interval_units[actual_unit]);
  lcd.print(">");
  lcd.setCursor(7,1);
  lcd.print(intervals[actual_interval]);
  
  switch (read_LCD_buttons()) {  
    case btnRIGHT: {
      actual_unit++;
      lcd.clear();
      delay(150);
      break;
    }

    case btnLEFT: {
      actual_unit--;
      lcd.clear();
      delay(150);
      break;
    }

    case btnDOWN: {
      actual_interval--;
      lcd.clear();
      delay(150);
      break;
    }
    case btnUP: {
      actual_interval++;
      lcd.clear();
      delay(150);
      break;
    }
    case btnSELECT: {
      counter = 0;
      select();
    }
  }
  
  if(actual_unit >= int_units_count || actual_unit < 0) {
    actual_unit = 0;
  }
  
  if(actual_interval >= int_count) {
    actual_interval = 0;
  }
  
  if(actual_interval < 0) {
    actual_interval = int_count-1;
  }
  
}

// set the amount, pressing right-left changes the type, up-down value, this i probably could refactor to unify with interval function
void set_amount() {

  lcd.home();
  lcd.print("<");
  lcd.print(amount_types[actual_amount_type]);
  lcd.print(">");
  lcd.setCursor(7,1);
  lcd.print(amounts[actual_amount]);

  switch (read_LCD_buttons()) {  
    case btnRIGHT: {
      actual_amount_type++;
      lcd.clear();
      delay(150);
      break;
    }

    case btnLEFT: {
      actual_amount_type--;
      lcd.clear();
      delay(150);
      break;
    }

    case btnDOWN: {
      actual_amount--;
      lcd.clear();
      delay(150);
      break;
    }
    case btnUP: {
      actual_amount++;
      lcd.clear();
      delay(150);
      break;
    }
    case btnSELECT: {
      select();
    }
  }

  if(actual_amount_type >= amount_types_count || actual_amount_type < 0) {
    actual_amount = 0;
  }
  
  if(actual_amount >= amounts_count) {
    actual_amount = 0;
  }
  
  if(actual_amount < 0) {
    actual_amount = amounts_count-1;
  }  
  
}

// start shooting
void start_shooting() {
  //check whats the time
  long now = millis();
  //if this it the first iteration, set when to stop, shoot first photo, increase counters depending on if its time contraint or number of shots
  if(counter == 0) {
    int_millis = now + intervals[actual_interval]*intervals_multiplier[actual_unit];
    shoot_once();
    if(actual_amount_type == 0) {
      counter++;
    } else {
      counter = now + amounts[actual_amount] * amount_types_units[actual_amount_type];
    }
  }
  // if time/shots constrains hasnt been reached...
  if((actual_amount_type == 0 && counter < amounts[actual_amount]) || (actual_amount_type > 0 && now < counter)) {
    lcd.home();
    if(actual_amount_type == 0) {
      String cnt = String(counter);
      cnt += " / ";
      cnt += amounts[actual_amount];
      lcd.print(cnt);
    } else {
      lcd.print(shots_counter);
    }
    lcd.setCursor(0,1);
    lcd.print(round((int_millis - now)/1000));
    // if this is the time to shoot a photo
    if(now >= int_millis) {
      // set the next shooting time
      int_millis = now + intervals[actual_interval]*intervals_multiplier[actual_unit];
      shoot_once();
      if(actual_amount_type == 0) {
        counter++;
      }
    }
  } else {
    // if shooting has ended
    counter = 0;
    shots_counter = 0;
    show_menu = !show_menu;
  }
  // you can always leave the screen to menu
  switch (read_LCD_buttons()) {  
    case btnSELECT: {
      select();
    }
  }
}

// main dispatch
void loop() {
  
  while(show_menu) menu();
  while(!show_menu && actual_item == 0) set_interval();
  while(!show_menu && actual_item == 1) set_amount();
  while(actual_item == 2 && !show_menu) start_shooting();

}

