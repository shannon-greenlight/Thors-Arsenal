// for when you just want to do nothing
void noop() {}

void code_red(bool on)
{
    analogWrite(LEDR, on ? 0 : 255);
    analogWrite(LEDB, 255);
    // analogWrite(LEDG, 255);
}

void red_alert()
{
    analogWrite(LEDR, 0);
    analogWrite(LEDB, 255);
    // analogWrite(LEDG, 255);
}

String csv_elem(String s, char delim, int instance)
{
    // vector <String> out;
    int indx = 0;
    int found = 0;
    for (unsigned int i = 0; i < s.length() + 1; i++)
    {
        if (s.charAt(i) == delim || i == s.length())
        {
            if (found == instance)
            {
                return s.substring(indx, i);
            }
            found++;
            indx = i + 1;
        }
    }
    return "CSV element not found!";
}

int csv_to_array(String input_string, String *output_array)
{
    int i = 0;
    do
    {
        output_array[i] = csv_elem(input_string, ',', i);
        i++;
    } while (output_array[i] != "CSV element not found!");
    return i;
}

String array_to_csv(String *input_array, int array_size)
{
    String out = "";
    for (int i = 0; i < array_size; i++)
    {
        out += input_array[i];
        if (i < array_size - 1)
        {
            out += ",";
        }
    }
    return out;
}

String csv_replace(String input_string, char delim, int instance, String replacement)
{
    int indx = 0;
    int found = 0;
    for (unsigned int i = 0; i < input_string.length() + 1; i++)
    {
        if (input_string.charAt(i) == delim || i == input_string.length())
        {
            if (found == instance)
            {
                return input_string.substring(0, indx) + replacement + input_string.substring(i);
            }
            found++;
            indx = i + 1;
        }
    }
    return "CSV element not found!";
}

String enquote(String value)
{
    return "\"" + value + "\"";
}

String encase(String value, String container)
{
    return container + value + container;
}
