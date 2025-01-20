#include <Wire.h>
// Keyboard functions

void select_button_down(void);
void scan_keyboard(void);

// #define KEYBOARD_ROW_0 0
#define KEYBOARD_ROW_1 A0
#define KEYBOARD_ROW_2 A1

#define SELECT_BUTTON_PIN 4

#define KEYBOARD_COLUMN_0 A2
#define KEYBOARD_COLUMN_1 A3
#define KEYBOARD_COLUMN_2 A6
#define KEYBOARD_COLUMN_3 A7

//
// "class" vars
volatile int keypress = 0; // set by several sources
volatile unsigned long select_button_down_time = 0;
volatile boolean select_button_pushed;
volatile uint8_t keys_row_0;
volatile uint8_t keys_row_1;
volatile uint8_t keys_row_2;

boolean esc_mode = false;
boolean select_button_available;

void reset_keyboard()
{
	// set the rows LOW so that any keypress will trigger interrupt
	// Row 0 is wired high
	digitalWrite(KEYBOARD_ROW_1, LOW);
	digitalWrite(KEYBOARD_ROW_2, LOW);
	keys_row_0 = 0;
	keys_row_1 = 0;
	keys_row_2 = 0;
	esc_mode = false;
	keypress = 0;
	select_button_pushed = false;
	select_button_available = false;
	key_held_down = false;
}

uint8_t crunch_key()
{
	uint8_t keys_row;
	// keys_row = !digitalRead(KEYBOARD_COLUMN_0);
	keys_row = analogRead(KEYBOARD_COLUMN_0) < 100 ? 1 : 0; // crazy fix for input not going low enough
	keys_row |= !digitalRead(KEYBOARD_COLUMN_1) << 1;
	keys_row |= !digitalRead(KEYBOARD_COLUMN_2) << 2;
	keys_row |= !digitalRead(KEYBOARD_COLUMN_3) << 3;
	return keys_row;
}

bool key_available()
{
	return (select_button_available) || keys_row_0 || keys_row_1 || keys_row_2;
}

void wait_all_keys_up()
{
	unsigned long started = millis();
	key_held_down = false;
	while (key_available() && millis() < started + 2000)
	{
		delay(10);
		scan_keyboard();
	}
	key_held_down = millis() >= started + 2000;
}

void scan_keyboard()
{
	uint8_t same_cnt = 0;
	uint8_t sanity_cnt = 0;
	int last_val = 0;
	int current_val;
	while (sanity_cnt++ < 100 && same_cnt < 5)
	{
		// go row by row
		digitalWrite(KEYBOARD_ROW_1, HIGH);
		digitalWrite(KEYBOARD_ROW_2, HIGH);
		keys_row_0 = crunch_key();

		// digitalWrite(KEYBOARD_ROW_0, HIGH);
		digitalWrite(KEYBOARD_ROW_1, LOW);
		digitalWrite(KEYBOARD_ROW_2, HIGH);
		delay(1);
		keys_row_1 = crunch_key();

		// digitalWrite(KEYBOARD_ROW_0, HIGH);
		digitalWrite(KEYBOARD_ROW_1, HIGH);
		digitalWrite(KEYBOARD_ROW_2, LOW);
		delay(1);
		keys_row_2 = crunch_key();

		current_val = keys_row_0 << 8 | keys_row_1 << 4 | keys_row_2;
		if (current_val == last_val)
		{
			// hooray
			same_cnt++;
		}
		else
		{
			same_cnt = 0;
		}
		last_val = current_val;
		// delay(20);
	}
	// now check rotary push button
	unsigned long now = millis();
	if (select_button_pushed)
	{
		bool being_held = !digitalRead(SELECT_BUTTON_PIN);
		if (being_held)
		{
			if (now > select_button_down_time + 2000)
			{
				keypress = '*';
				select_button_available = true;
				select_button_pushed = false;
			}
		}
		else
		{
			keypress = '!';
			select_button_available = true;
			select_button_pushed = false;
		}
	}
	else
	{
		select_button_available = false;
	}
	if (key_available() && ui.screen_saved)
	{
		restore_display();
		wait_all_keys_up();
		reset_keyboard();
	}
	// Serial.println("Sanity: " + String(sanity_cnt) + " Same: " + String(same_cnt));
}

void calc_keypress()
{
	esc_mode = false;
	if (keys_row_0 == 1)
	{
		esc_mode = true;
		keypress = 'A'; // up 65
		return;
	}
	if (keys_row_0 == 2)
	{
		esc_mode = true;
		keypress = 'B'; // down
		return;
	}
	if (keys_row_0 == 4)
	{
		esc_mode = true;
		keypress = 'D'; // left
		return;
	}
	if (keys_row_0 == 8)
	{
		esc_mode = true;
		keypress = 'C'; // right
		return;
	}

	if (keys_row_1 == 1)
	{
		// keypress = '0';
		select_fxn(OUTPUT_0_FXN);
		return;
	}
	if (keys_row_1 == 2)
	{
		select_fxn(OUTPUT_1_FXN);
		return;
	}
	if (keys_row_1 == 4)
	{
		select_fxn(OUTPUT_2_FXN);
		return;
	}
	if (keys_row_1 == 8)
	{
		select_fxn(OUTPUT_3_FXN);
		return;
	}

	if (keys_row_2 == 1)
	{
		select_fxn(OUTPUT_4_FXN);
		return;
	}
	if (keys_row_2 == 2)
	{
		select_fxn(OUTPUT_5_FXN);
		return;
	}
	if (keys_row_2 == 4)
	{
		select_fxn(OUTPUT_6_FXN);
		return;
	}
	if (keys_row_2 == 8)
	{
		select_fxn(OUTPUT_7_FXN);
		return;
	}
}

#include <vector>

// Step 1: Define a common parent class
class Animal
{
public:
	virtual void speak() const = 0; // Pure virtual function
	virtual ~Animal() {}			// Virtual destructor for proper cleanup
};

// Step 2: Define derived classes
class Dog : public Animal
{
public:
	void speak() const override
	{
		ui.terminal_debug("Woof!");
	}
};

class Cat : public Animal
{
public:
	void speak() const override
	{
		ui.terminal_print("Meow!");
	}
};

void test_animal()
{
	// Step 3: Create an array (or vector) of pointers to the parent class
	std::vector<Animal *> animals;
	animals.push_back(new Dog()); // Adding Dog object
	animals.push_back(new Cat()); // Adding Cat object

	// Step 4: Access elements through base class pointers
	for (Animal *animal : animals)
	{
		animal->speak();
	}

	// Cleanup
	for (Animal *animal : animals)
	{
		delete animal;
	}
}

void process_key()
{
	static int count = 1;
	if (false && keypress > 0)
		ui.terminal_debug("Process key: " + String(keypress));
	// unsigned int test[10];
	switch (keypress)
	{
	case '~':
		// dac.factory_reset();
		return;

		ui.terminal_debug("Testing Done!");
		return;
		ee_info.dump(0, 64);
		return;
		// ui.terminal_debug("Board Gen: " + String(settings_fxn.get_param(SETTINGS_BOARD_GENERATION)));
		// Serial.println("Fxn name: " + settings_fxn.name);
		// ui.do_snowflakes(false);
		return;
		for (int i = 0; i < NUM_OUTPUT_PARAMS; ++i)
		{
			Serial.println(output0.get_label(i) + ": " + output_text[i]);
		}
		Serial.println(); // Move to the next line after printing all columns of a row
		Serial.println(); // Move to the next line after printing all columns of a row

		// Print the array formatted into rows and columns
		for (int i = 0; i < NUM_OUTPUT_PARAMS; ++i)
		{
			// Serial.println("Row: " + String(i));
			Serial.println(output0.get_label(i));
			for (int j = 0; j < 20; ++j)
			{
				Serial.print(output_text_extra[i][j] + "^^^ ");
				if (j < 19)
				{
					Serial.print(" ...\t... "); // Print a tab between columns
				}
			}
			Serial.println(); // Move to the next line after printing all columns of a row
		}

		// for (int i = 0; i < NUM_OUTPUT_PARAMS; ++i)
		// {
		// 	for (int j = 0; j < NUM_WAVEFORMS; ++j)
		// 	{
		// 		Serial.println(output_text_extra[i][j]);
		// 	}
		// }

		// for (int i = 0; i < NUM_OUTPUTS; i++)
		// {
		// 	Serial.println("Output: " + String(i) + " offset: " + String(bonk_outputs[i]->offsets[OUTPUT_IDLE_VALUE]));
		// 	Serial.println("Array: " + String(i) + " offset: " + String(output_offsets[i][OUTPUT_IDLE_VALUE]));
		// 	Serial.println("");
		// }
		// ui.terminal_debug("digit_num: " + String(selected_fxn->digit_num));
		// Serial.println("dp: " + String(selected_fxn->decimal_places[selected_fxn->param_num]));
		// // selected_fxn->init_param();
		// send_data_to_USB('/');
		break;
	case 'Z': // disable display and terminal
		ui.all_off();
		break;
	case 'z':
		restore_display();
		break;
	case '!':
		activate();
		break;
	case 'u':
		adjust_group(1);
		break;
	case 'd':
		adjust_group(-1);
		break;
	case '*':
		select_fxn(SETTINGS_FXN);
		break;
	case 'i':
		selected_fxn->insert_char(' ');
		break;
	case 127:
		selected_fxn->remove_char();
		break;
	// case '+':
	// 	select_fxn(remembered_fxn.get() + 1);
	// 	break;
	// case '-':
	// 	select_fxn(remembered_fxn.get() - 1);
	// 	break;
	case 'A': // up arrow in esc mode A
		if (esc_mode)
		{
			dec_param_num();
			// selected_fxn->inc_param_num_by(-1);
		}
		break;
	case 'B': // dn arrow B
		if (esc_mode)
		{
			inc_param_num();
			// selected_fxn->inc_param_num_by(1);
		}
		break;
	case 'C': // right arrow in esc mode C
		if (esc_mode)
		{
			// selected_fxn->inc_dig_num_by(1);
			inc_dig_num();
		}
		break;
	case 'D': // left arrow D
		if (esc_mode)
		{
			// selected_fxn->inc_dig_num_by(-1);
			dec_dig_num();
		}
		break;
	}
	esc_mode = keypress == 27 || keypress == '[';
}

void keyboard_update_trigger(int val, int channel, int selected_trig_enable)
{
	Greenface_gadget *remember_fxn = selected_fxn;
	if (!in_bounce_fxn())
	{
		triggered_fxn[channel]->param_put(val, selected_trig_enable);
		triggered_fxn[channel]->param_num = selected_trig_enable;
		set_trigger(channel);
	}
	else
	{
		// ui.terminal_debug("Update bounce trig enable: " + String(selected_trig_enable) + " val: " + String(val) + " channel: " + String(channel));
		bounce_fxns[channel]->param_num = selected_trig_enable;
		bounce_fxns[channel]->param_put(val, selected_trig_enable);
		selected_fxn = bounce_fxns[channel];
		bounce_update_trigger();
		selected_fxn = remember_fxn;
	}
}

void process_cmd(String in_str)
{
	// Serial.println("Process cmd: " + in_str);
	// ui.terminal_debug("Process cmd: " + in_str);

	char cmd = in_str.charAt(0);
	int int_param = in_str.substring(1).toInt();
	int dig1 = in_str.substring(1, 2).toInt();
	int dig2 = in_str.substring(2, 3).toInt();
	int selected_trig_enable = get_selected_trig_enable();

	char scmd = in_str.charAt(1);
	String subcmd = in_str.substring(2, 4);
	int sub_int_param = in_str.substring(4).toInt();

	int selected_output_temp;
	int param_num = selected_fxn->param_num;
	Greenface_gadget *temp_fxn;
	int temp_int;
	String temp_string;

	// ui.terminal_debug("Process cmd: " + in_str + " int_param: " + String(in_str.substring(0).toInt()) + " cmd: " + String(cmd));
	switch (cmd)
	{
	case '[':
		esc_mode = true;
		cmd = in_str.charAt(1);
		break;
	case '=':
		// ui.terminal_debug("char 1: " + String(in_str.charAt(1)));
		selected_output_temp = selected_output.get();
		if (in_str == "=" || (in_str.charAt(0) == '=' && in_str.charAt(1) == 'I'))
		{
			temp_int = (in_str == "=" || in_str == "=I") ? selected_output_temp : in_str.substring(2).toInt();
			temp_int = constrain(temp_int, 0, NUM_OUTPUTS - 1);
			temp_string = "DAC Value: " + String(dac.get_dac_value(temp_int));
			if (usb_direct_enabled())
			{
				temp_string = "<h3>" + temp_string + "</h3>";
				temp_string += "<p>Output: " + String(temp_int) + "</p>";
				send_msg_to_USB(temp_string);
			}
			else
			{
				ui.terminal_debug(temp_string + " Output: " + String(temp_int));
			}
			// ui.terminal_debug("Idle Value: " + String(outputs[selected_output_temp].idle_value));
		}
		else
		{
			if (in_str.charAt(1) == 'O')
			{
				selected_output_temp = in_str.charAt(2) - '0';
				temp_int = in_str.substring(3).toInt();
			}
			else
			{
				temp_int = int_param;
			}
			dac.out(selected_output_temp, temp_int);
			send_ok_to_USB(cmd);
		}
		break;
	case ':':
		selected_fxn->put_dig_num(int_param);
		selected_fxn->printParams();
		break;
	case 'f':
		select_fxn(int_param);
		break;
	case 'p':
		selected_fxn->put_param_num(int_param);
		selected_fxn->printParams();
		break;
	case 'c':
		clear_trigger(int_param, selected_trigger->trig_num);
		// ui.terminal_debug("Clearing trigger for output: " + String(int_param));
		break;
	case 't':
		// select trigger
		select_trigger(dig1);
		// selected_fxn->digit_num = digit_num; // restore digit_num
		// selected_fxn->printParams();
		if (in_str.charAt(2) == '!')
		{
			selected_trigger->trigger();
			// if (!in_bounce_fxn())
			// {
			// 	selected_fxn->param_num = param_num; // restore param_num, we only needed a trigger, not change of param
			// }
			selected_fxn->param_num = param_num; // restore param_num, we only needed a trigger, not change of param
		}
		break;
	case 'w':
		// ui.terminal_debug("Wave data: " + csv_elem(in_str, ',', 0));
		// ui.terminal_debug("Wave data: " + in_str);
		recv_user_waveform(in_str);
		break;
	case 'I':
		init_group();
		// selected_fxn->init_param();
		// selected_fxn->printParam();
		break;
	case 'T':
		// ui.terminal_debug("Modify Trigger! Output: " + String(dig2) + " Trigger: " + String(selected_trig_enable));
		switch (dig1)
		{
		case 0:
			// ui.terminal_debug("Disable Trigger! Output: " + String(dig2));
			keyboard_update_trigger(0, dig2, selected_trig_enable);
			break;
		case 1:
			// Serial.println("Enable Trigger! Output: " + String(dig2));
			keyboard_update_trigger(1, dig2, selected_trig_enable);
			break;
		case 2:
			// ui.terminal_debug("Toggle Trigger! Output: " + String(dig2));
			temp_int = get_selected_trigger_toggled(dig2, selected_trig_enable);
			keyboard_update_trigger(temp_int, dig2, selected_trig_enable);
			break;
		case 3:
			// ui.terminal_debug("Disable All Triggers!");
			disable_all_triggers();
			break;
		case 4:
			// ui.terminal_debug("Trigger All Triggers!");
			for (int i = 0; i < NUM_TRIGGERS; i++)
			{
				select_trigger(i);
				selected_trigger->trigger();
			}
			break;
		case 5:
			// ui.terminal_debug("Clear All Triggers!");
			clear_all_triggers();
			break;
		case 6:
			// ui.terminal_debug("Display All Triggers!");
			if (!usb_direct_enabled())
			{
				trigger_report();
			}
			break;
		}
		break;
	case 'U':
		settings_put_usb_direct(int_param);
		break;
	case 'J':
		switch (int_param)
		{
		case 0:
			ui.display_off();
			break;
		case 1:
			ui.display_on();
			selected_fxn->display();
			break;
		}
		break;
	case 'M':
		if (!usb_direct_enabled())
		{
			switch (int_param)
			{
			case 1:
				input_cal_params_macro();
				break;
			case 2:
				output_cal_params_macro();
				break;
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
				user_waveform_params_macro(int_param - 10);
				break;
			default:
				gen_params_macro(selected_fxn);
			}
		}
		break;
	case 'P':
		if (in_output_fxn())
		{
			put_group(int_param % 100, OUTPUT_PHASE);
			// selected_fxn->param_put(int_param % 100, OUTPUT_PHASE);
			update_phase();
		}
		break;
	case 'D':
		// ui.clearTerminal();
		// ui.terminal_debug("Dump: " + String(dig1) + " " + String(dig2));
		if (!usb_direct_enabled())
		{
			dump_waveform_or_pattern(dig1, dig2);
		}
		break;
	case 'G':
		group = int_param;
		// ui.terminal_debug("Group: " + String(group));
		break;
	case 'g':
		group_active = (int_param > 0);
		// ui.terminal_debug("Group: " + String(group));
		break;
	case 'O':
		// ui.terminal_debug("Offset - Group: " + String(group));
		put_group(int_param, in_bounce_fxn() ? BOUNCE_OFFSET : OUTPUT_OFFSET);
		break;
	case 'S':
		put_group(int_param, in_bounce_fxn() ? BOUNCE_SCALE : OUTPUT_SCALE);
		break;
	case 'A':
		put_group(int_param, get_active_time_index());
		break;
	case 's':
		// ui.terminal_debug("Here's the scmd: " + String(scmd) + " and here's the subcmd: " + subcmd + " and here's the sub_int_param: " + String(sub_int_param));
#ifdef FAFNIRS_FIRE
		switch (scmd)
		{
		case 'B':
			if (subcmd == "BR")
			{
				put_group(sub_int_param, BEAT_BEAT_RATE);
			}
			else if (subcmd == "ID")
			{
				put_group(sub_int_param, BEAT_INIT_DELAY);
			}
			else if (subcmd == "IT")
			{
				put_group(sub_int_param, BEAT_IDLE_TIME);
			}
			else if (subcmd == "MV")
			{
				put_group(sub_int_param, BEAT_MOD_VALUE);
			}
			break;
		case 'T':
			if (subcmd == "PL")
			{
				put_group(sub_int_param, TOG_PULSE_LEN);
			}
			break;
		case 'G':
			if (subcmd == "ID")
			{
				put_group(sub_int_param, TRIG_INIT_DELAY);
			}
			else if (subcmd == "PL")
			{
				put_group(sub_int_param, TRIG_PULSE_LEN);
			}
			else if (subcmd == "GL")
			{
				put_group(sub_int_param, TRIG_GAP_LEN);
			}
			else if (subcmd == "NP")
			{
				put_group(sub_int_param, TRIG_NUM_PULSES);
			}
			break;
		}
#endif
		break;
	case 'R':
		put_group(int_param, OUTPUT_RANDOMNESS);
		break;
	case 'V':
		put_group(int_param, OUTPUT_IDLE_VALUE);
		break;
	case 'Q':
		put_group(int_param, OUTPUT_QUANTIZE);
		break;
#ifdef WIFI_INSTALLED
	case 'W':
		ui.clearTerminal();
		printWifiStatus();
		break;
#endif
	case '$':
		selected_fxn->put_string_var(urlDecode(in_str.substring(1)));
		break;
	case '/':
		// comment character. Do nothing
		break;
	case '#':
		selected_fxn->put_param_w_offset(int_param);
		if (in_output_fxn())
		{
			switch (selected_fxn->param_num)
			{
			case OUTPUT_SCALE:
			case OUTPUT_OFFSET:
			case OUTPUT_RANDOMNESS:
			case OUTPUT_QUANTIZE:
				apply_params_to_waveform(selected_output.get());
				graph_waveform(selected_output.get());
				break;

			default:
				break;
			}
		}
		break;
	}

	// Serial.println("Process cmd: " + in_str + " int_param: " + String(in_str.substring(0).toInt()) + " cmd: " + String(cmd));
	if (cmd >= '-' && cmd <= '9' && cmd != '/')
	{
		put_group(in_str.substring(0).toInt(), selected_fxn->param_num);

		if (in_output_fxn())
		{
			switch (selected_fxn->param_num)
			{
			case OUTPUT_SCALE:
			case OUTPUT_OFFSET:
			case OUTPUT_RANDOMNESS:
			case OUTPUT_QUANTIZE:
				apply_params_to_waveform(selected_output.get());
				graph_waveform(selected_output.get());
				break;

			default:
				break;
			}
		}
	}

	if (String(cmd) != "\n" && cmd != 'p')
	{
		if (String(cmd) == "%")
		{
			keypress = 27;
		}
		else
		{
			keypress = cmd;
		}

		process_key();
	}
}

void check_keyboard()
{
	scan_keyboard();
	if (key_available())
	{
		force_cv();
		calc_keypress();
		wait_all_keys_up();
		process_key();
		delay(10);
		reset_keyboard();
	}
}

void check_serial()
{
	static String in_str = ""; // for serial input
	static boolean cmd_available = false;
	static bool entering_string = false; // or entering a number or comment
	char cmd;

	if (Serial.available() > 0)
	{
		char c = Serial.read();
		bool is_esc_char = esc_mode && (c == 'A' || c == 'B' || c == 'C' || c == 'D');
		// ui.terminal_debug(String(int(c)) + " esc: " + String(esc_mode));
		// Serial.println(in_str + " .. " + String(c));
		if ((c == '$' || c == '-' || c == '+' || c == '/') && selected_fxn->param_is_stringvar())
		{
			entering_string = true;
			selected_fxn->set_cursor_to_param();
		}
		if (!entering_string && (in_str.length() < 2 || is_esc_char) && (c == 'i' || c == 127 || c == '~' || c == '*' || c == '!' || c == 'u' || c == 'd' || c == 'z' || c == 'Z' || is_esc_char))
		{
			// process this immediately with process_keypress
			keypress = c;
		}
		else
		{
			if (c == '\r')
			{
				// process this with process_cmd
				cmd_available = true;
				entering_string = false;
			}
			else
			{
				if (entering_string && c != '$')
				{
					ui.terminal_print(c); // echo string to terminal
				}
				if (c == 27 || c == '[')
				{
					esc_mode = true;
				}
				else
				{
					in_str += c;
					if (c == '!')
					{
						cmd_available = true;
						entering_string = false;
					}
				}
			}
		}
		if (keypress || cmd_available)
		{
			force_cv(); // a keypress resets many functions. Do this to keep CV mods running
			if (cmd_available)
			{
				cmd = in_str[0];
				process_cmd(in_str);
				in_str = "";
				keypress = 0;
				cmd_available = false;
			}
			else
			{
				cmd = keypress;
				process_key();
				reset_keyboard();
				in_str = "";
			}
			if (usb_direct_enabled() && !sending_status && cmd != '~' && cmd != 'U' && cmd != '\0')
			{
				// ui.t.println("Howdy!");
				switch (cmd)
				{
				case '=':
					break;
				case 'e':
					send_ok_to_USB(cmd);
					break;
				default:
					cmd = (cmd == '\n') ? ' ' : cmd;
					send_data_to_USB(cmd);
				}
			}
			// if (usb_direct_enabled() && !sending_status && cmd == 't')
			// {
			// 	send_trig_status_to_USB();
			// }
		}
	}
}

void select_button_down()
{
	if (!select_button_pushed) // ignore falling noise
	{
		select_button_pushed = true;
		select_button_down_time = millis();
	}
}

void keyboard_begin()
{
	pinMode(KEYBOARD_ROW_1, OUTPUT);
	pinMode(KEYBOARD_ROW_2, OUTPUT);
	pinMode(KEYBOARD_COLUMN_0, INPUT);
	pinMode(KEYBOARD_COLUMN_1, INPUT);
	pinMode(KEYBOARD_COLUMN_2, INPUT);
	pinMode(KEYBOARD_COLUMN_3, INPUT);
	reset_keyboard();
	delay(200);
	attachInterrupt(digitalPinToInterrupt(SELECT_BUTTON_PIN), select_button_down, FALLING);
}
