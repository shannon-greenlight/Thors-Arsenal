String old_status = "";
void terminal_print_status(String status_string, bool force = false)
{
    // static String old_status = "";
    // ui.terminal_debug("Status string: <" + status_string + ">");
    if (status_string.length() > 0)
    {
        int pos = TERMINAL_WIDTH / 2 - status_string.length() / 2;
        pos = pos + pos % 2;
        if (ui.terminal_mirror && (status_string != old_status || force))
        {
            // ui.t.setCursor(STATUS_ROW, "1");
            // ui.t.clrToEOL();
            ui.t.setCursor(STATUS_ROW, String(pos));
            ui.t.print(status_string);
            old_status = status_string;
        }
    }
}

uint16_t get_chars_per_line()
{
    int16_t x1, y1;
    uint16_t w, h;

    ui.display->getTextBounds(" ", 0, 0, &x1, &y1, &w, &h);
    return SCREEN_WIDTH / w;
    // Serial.println("Char width: " + String(w));
    // Serial.println("Chars / line: " + String(SCREEN_WIDTH / w));
}

void restore_display()
{
    ui.all_on(!usb_direct_enabled());
    ui.reset_inactivity_timer();
    selected_fxn->display();
    terminal_print_status(old_status, true);
}
