// cv functions
int16_t cv_mem[NUM_OUTPUTS];
void force_cv()
{
    for (int output = 0; output < NUM_OUTPUTS; output++)
    {
        cv_mem[output] = 0;
    }
}

void update_cv(int cv_num)
{
    // this clears mods that were applied -- so we can start fresh
    OutputData *outptr = &outputs[selected_output.get()];

    force_cv();

    outptr->offset = 0;
    outptr->scale = 1.0;
    update_idle_value();
    output_display();
}

// scales a float value by adjusted linear. Range of adj: param/100 -> param*100, cv of ADC_FS_OVER_2 returns param*1
// for param=1 return val = 10 raised to the .2Vi
float cv_scale_pow(int cv, float param)
{
    float temp = ((float)cv / ADC_FS_OVER_4) - 2.0;
    return pow(10, temp) * param;
}

// simple scale -- 0 through 1*param
// (5/3.32 * 1241.2) + 2048 = 3917.247
// (5/3.01 * 1241.2) + 2048 = 4109.794
#define CV_FS 3917.247
float cv_scale(int cv, float param)
{
    // return ((float)cv / (float)ADC_FS) * param;
    return ((float)cv / CV_FS) * param;
}

int cv_active_time(int cv)
{
    return max(1, (cv - DAC_MID) / 4);
}

// returns a value from -2048 to 2048
int16_t cv_offset(int16_t cv_val)
{
    return cv_val * 1.17;
}

bool sole_cv(int cv_num, int cv_type, Greenface_gadget bonk_output)
{
    return ((cv_num == OUTPUT_CV0 && bonk_output.get_param(OUTPUT_CV1) != cv_type) || (cv_num == OUTPUT_CV1 && bonk_output.get_param(OUTPUT_CV0) != cv_type));
}

// sets output data with appropriate cv modifier value
// cv_val is corrected value from adc
// it ranges from -1650 to 1650 and corresponds to the voltage at the ADC input.
void cv_set(int cv_num, int output, int16_t cv_val)
{

    // scale cv_val from +/- 1650 to 0-4095
    // 1.2412 is the ratio of the DAC_FS/2 to the ADC_FS (2048/1650)
    int16_t cv_val_conditioned = cv_val * CV_MOD_SCALE + DAC_MID;

    // if (cv_num == OUTPUT_CV0)
    //     Serial.println("cv_val_conditioned: " + String(cv_val_conditioned) + " cv_mem: " + String(cv_mem[output]) + " output: " + String(output));

    OutputData *outptr = &outputs[output];
    Greenface_gadget bonk_output = (*bonk_outputs[output]);
    byte cv_type = bonk_output.get_param(cv_num);

    if (cv_type > 0)
    {
        if (abs(cv_val_conditioned - cv_mem[output]) > 2)
        {
            // Serial.println("Active time: " + String(outptr->active_time));
            // Serial.println("CV val: " + String(cv_val_conditioned) + " mem: " + String(cv_mem[output]));
            static float temp_offset;
            static float temp_scale;
            static uint16_t temp_active_time;

            // now set value
            switch (cv_type)
            {
            case CV_SCALE:
                if (sole_cv(cv_num, CV_SCALE, bonk_output))
                {
                    outptr->scale = cv_scale_pow(cv_val_conditioned, 1.0);
                }
                else
                {
                    switch (cv_num)
                    {
                    case OUTPUT_CV0:
                        temp_scale = cv_scale_pow(cv_val_conditioned, 1.0);
                        break;
                    case OUTPUT_CV1:
                        outptr->scale = cv_scale_pow(cv_val_conditioned, temp_scale);
                        break;
                    }
                }
                break;
            case CV_OFFSET:
                if (sole_cv(cv_num, CV_OFFSET, bonk_output))
                {
                    outptr->offset = cv_offset(cv_val);
                    // ui.terminal_debug("CV Val: " + String(cv_val) + " Offset: " + String(outptr->offset));
                }
                else
                {
                    switch (cv_num)
                    {
                    case OUTPUT_CV0:
                        temp_offset = cv_offset(cv_val) / 2;
                        break;
                    case OUTPUT_CV1:
                        outptr->offset = cv_offset(cv_val) / 2 + temp_offset;
                        // outptr->offset = cv_offset(cv_val) / 2 + 0;
                        break;
                    }
                }
                break;
#ifndef FAFNIRS_FIRE
            case CV_ACTIVE_TIME:
                if (sole_cv(cv_num, CV_ACTIVE_TIME, bonk_output))
                {
                    outptr->time_inc = cv_active_time(cv_val_conditioned);
                    // ui.terminal_debug("CV Time Inc: " + String(outptr->time_inc) + " ms ");
                }
                else
                {
                    switch (cv_num)
                    {
                    case OUTPUT_CV0:
                        temp_active_time = cv_active_time(cv_val_conditioned);
                        break;
                    case OUTPUT_CV1:
                        outptr->time_inc = cv_active_time(temp_active_time);
                        break;
                    }
                }
                break;
#endif
            case CV_IDLE_VALUE:
                set_idle_value(cv_val_conditioned, output);
                break;
            case CV_RANDOMNESS:
                set_randomness_factor(max(0, cv_val_conditioned - DAC_MID) * 100.0 / ADC_FS, bonk_output.get_param(OUTPUT_SCALE) / 100.0, output);
                break;
            }
            cv_mem[output] = cv_val_conditioned;
        }
    }
}

// this is an interrupt fxn!
// called by TimerHandler
void apply_cv(int output_num, int cv_num, OutputData *outptr)
{
    int cv_type = (*bonk_outputs[output_num]).get_param(cv_num);
    int val = outptr->output_value; // using int gives us headroom for math
    int offset_adjust = DAC_MID * (1 - outptr->scale);

    switch (cv_type)
    {
    case CV_SCALE:
        val *= outptr->scale;
        val += offset_adjust;
        outptr->output_value = constrain(val, 0, DAC_FS);
        break;
    case CV_OFFSET:
        val += outptr->offset;
        outptr->output_value = constrain(val, 0, DAC_FS);
        break;
    }
}
