#include <stdio.h>
#include <gtk-3.0/gtk/gtk.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include "dash.h"
#include "gisagauge.h"

#define WINDOW_W 800
#define WINDOW_H 480

#define N_MOTORS 4
#define N_CELLS_PER_SEGMENT 100
#define N_SEGMENTS 6

char tmpBuffer[128];

typedef struct
{
    uint32_t id;
    size_t len;
    uint8_t *dat;
} ricsData_t;

typedef struct
{
    float rpm;
    bool active;
    bool error_overspeed;
    bool error_sincos;
} Motor_t;

typedef struct
{
    double voltage;
    double temperature;
} Cell_t;

typedef struct
{
    Cell_t cells[N_CELLS_PER_SEGMENT];
} Segment_t;

struct
{
    Segment_t segments[N_SEGMENTS];
} Accumulator;

Motor_t motors[N_MOTORS];

pthread_t tId;

GtkWidget *window;
GtkWidget *mainGrid;
GtkWidget *gauge;
GtkWidget *label_Messages;
GtkWidget *label_Time;
GtkWidget *label_Voltage;
GtkWidget *label_Temperature;
GtkWidget *label_Voltage_Value;
GtkWidget *label_Temperature_Value;
GtkWidget *label_AMS_Errors;
GtkWidget *label_AMS_Errors_Value;
GtkWidget *label_Inverters_Errors;
GtkWidget *label_Inverters_Errors_Value;

static gboolean handleGui(gpointer userdata);

void on_window_main_destroy();
void *startGUI(void *ptr);

void updateMotorFromCan(uint8_t motorNumber, uint8_t *dat);
void updateMotorGUI();
void updateAMSErrorsGUI(uint32_t id, size_t len, uint8_t *dat);
void updateAMSVoltageGUI(uint32_t id, size_t len, uint8_t *dat);
void updateAMSTemperatureGUI(uint32_t id, size_t len, uint8_t *dat);
void updateTimerGUI(uint32_t id, size_t len, uint8_t *dat);

bool rics_init(void)
{
    printf("Init from dynlib\n");
    if (!pthread_create(&tId, NULL, startGUI, NULL))
    {
        printf("Thread created\n");
    }
    else
    {
        printf("Thread creation failed\n");
        return false;
    }
    return true;
}
bool rics_start(int32_t node)
{
    printf("Start from dynlib\n");
    return true;
}
bool rics_can_callback(uint32_t id, size_t len, uint8_t *dat)
{
    ricsData_t *data = (ricsData_t *)malloc(sizeof(ricsData_t));
    uint8_t *newData = (uint8_t *)malloc(sizeof(uint8_t) * len);
    memcpy(newData, dat, len);
    data->dat = newData;
    data->id = id;
    data->len = len;
    gdk_threads_add_idle(handleGui, data); //important, all calls MUST be from same thread for GTK, hence this fonction

    return true;
}
void *startGUI(void *ptr)
{
    for (int i = 0; i < N_MOTORS; i++)
    {
        motors[i].rpm = 0;
        motors[i].active = false;
        motors[i].error_overspeed = false;
        motors[i].error_sincos = false;
    }

    gtk_init(NULL, NULL);
    GError *err = NULL;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_W, WINDOW_H);
    gtk_window_resize(GTK_WINDOW(window), WINDOW_W, WINDOW_H);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "DASH FormUL 2021");
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_main_destroy), NULL);

    mainGrid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(mainGrid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(mainGrid), TRUE);

    gauge = gisa_gauge_new();
    gisa_gauge_set_max_value(GISA_GAUGE(gauge), 15000);
    gisa_gauge_set_min_value(GISA_GAUGE(gauge), 0);
    gisa_gauge_set_value(GISA_GAUGE(gauge), 7500);
    gtk_widget_set_hexpand(gauge, TRUE);
    gtk_widget_set_vexpand(gauge, TRUE);
    gtk_grid_attach(GTK_GRID(mainGrid), GTK_WIDGET(gauge), 1, 0, 2, 2);

    label_Messages = gtk_label_new_with_mnemonic("Messages");
    gtk_grid_attach(GTK_GRID(mainGrid), label_Messages, 0, 0, 1, 1);

    label_Time = gtk_label_new_with_mnemonic("Time");
    gtk_grid_attach(GTK_GRID(mainGrid), label_Time, 3, 0, 1, 1);

    label_Voltage = gtk_label_new_with_mnemonic("Voltage");
    gtk_grid_attach(GTK_GRID(mainGrid), label_Voltage, 1, 2, 1, 1);

    label_Temperature = gtk_label_new_with_mnemonic("Temperature");
    gtk_grid_attach(GTK_GRID(mainGrid), label_Temperature, 2, 2, 1, 1);

    label_Voltage_Value = gtk_label_new_with_mnemonic("----");
    gtk_grid_attach(GTK_GRID(mainGrid), label_Voltage_Value, 1, 3, 1, 1);

    label_Temperature_Value = gtk_label_new_with_mnemonic("----");
    gtk_grid_attach(GTK_GRID(mainGrid), label_Temperature_Value, 2, 3, 1, 1);

    label_AMS_Errors = gtk_label_new_with_mnemonic("AMS Errors");
    gtk_grid_attach(GTK_GRID(mainGrid), label_AMS_Errors, 0, 2, 1, 1);

    label_AMS_Errors_Value = gtk_label_new_with_mnemonic("---------");
    gtk_grid_attach(GTK_GRID(mainGrid), label_AMS_Errors_Value, 0, 3, 1, 1);

    label_Inverters_Errors = gtk_label_new_with_mnemonic("Inverters Errors");
    gtk_grid_attach(GTK_GRID(mainGrid), label_Inverters_Errors, 3, 2, 1, 1);

    label_Inverters_Errors_Value = gtk_label_new_with_mnemonic("---------");
    gtk_grid_attach(GTK_GRID(mainGrid), label_Inverters_Errors_Value, 3, 3, 1, 1);

    GtkWidget *empty_0 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_grid_attach(GTK_GRID(mainGrid), empty_0, 0, 1, 1, 1);
    GtkWidget *empty_1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_grid_attach(GTK_GRID(mainGrid), empty_1, 3, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(window), mainGrid);

    gtk_widget_show_all(window);
    gtk_main();

    printf("_______________________ END OF GUI THREAD _____________________________\n");
    pthread_exit(NULL);
}
void on_window_main_destroy()
{
    gtk_main_quit();
}
static gboolean handleGui(gpointer userdata)
{
    ricsData_t *data = (ricsData_t *)userdata;
    uint32_t id = data->id;
    size_t len = data->len;
    uint8_t *dat = data->dat;

    if ((id & 0x0FF00000) == 0x04400000)
    {
        int8_t motorNumber = (id >> (4 * 4)) & 0x0F - 1;
        updateMotorFromCan(motorNumber, dat);
        updateMotorGUI();
    }

    if ((id & 0xFFFF0000) == 0x02070000)
    {
        updateAMSErrorsGUI(id, len, dat);
    }

    if (id == 0x0000000C)
    {
        updateTimerGUI(id, len, dat);
    }

    if ((id & 0xFFFF0000) == 0x02020000)
    {
        updateAMSVoltageGUI(id, len, dat);
    }

    if ((id & 0xFFFF0000) == 0x02030000)
    {
        updateAMSTemperatureGUI(id, len, dat);
    }

    free(dat);
    return G_SOURCE_REMOVE;
}
void updateMotorFromCan(uint8_t motorNumber, uint8_t *dat)
{
    if (motorNumber < N_MOTORS && motorNumber > -1)
    {
        motors[motorNumber].rpm = (dat[0] << 8) + dat[1];
        if (dat[0] & 0x80)
        {
            motors[motorNumber].rpm -= 65536;
        }
    }

    uint8_t state = dat[2];
    motors[motorNumber].active = (bool)state;

    uint8_t errors = (dat[6] << 8) + dat[7];
    motors[motorNumber].error_overspeed = errors & 0x2;
    motors[motorNumber].error_sincos = errors & 0x4;
}
void updateMotorGUI()
{
    char invertersErrorsBuffer[128];
    snprintf(invertersErrorsBuffer, sizeof(invertersErrorsBuffer), "---------");
    double rmpMoy = 0;
    uint8_t nActiveMotors = 0;
    for (int i = 0; i < N_MOTORS; i++)
    {
        if (motors[i].active)
        {
            nActiveMotors++;
            rmpMoy += motors[i].rpm;
            if (motors[i].error_sincos)
            {
                snprintf(tmpBuffer, sizeof(tmpBuffer), "err sincos motor %d\n", i);
                strcat(invertersErrorsBuffer, tmpBuffer);
            }
            if (motors[i].error_overspeed)
            {
                snprintf(tmpBuffer, sizeof(tmpBuffer), "err overspeed motor %d\n", i);
                strcat(invertersErrorsBuffer, tmpBuffer);
            }
        }
    }
    if (nActiveMotors > 0)
    {
        rmpMoy /= nActiveMotors;
    }
    gisa_gauge_set_value(GISA_GAUGE(gauge), floor(rmpMoy));
    gtk_label_set_text(GTK_LABEL(label_Inverters_Errors_Value), invertersErrorsBuffer);
}
void updateAMSErrorsGUI(uint32_t id, size_t len, uint8_t *dat)
{
    uint64_t err = 0;
    for (int i = 0; i < 8; i++)
    {
        err += (dat[i] << (8 * (7 - i)));
    }
    snprintf(tmpBuffer, sizeof(tmpBuffer), "0x%08X", err);
    gtk_label_set_text(GTK_LABEL(label_AMS_Errors_Value), tmpBuffer);
}
void updateAMSVoltageGUI(uint32_t id, size_t len, uint8_t *dat)
{
    int startix = (id & 0xFF00) / 0x100;
    for (int i = 1; i < 9; i++)
    {
        int cell = (8 * startix + i - 1) % 20;
        int seg = (8 * startix + i - 1) / 20;
        Accumulator.segments[seg].cells[cell].voltage = 0.02 * dat[i - 1];
    }
    double voltageMoy = 0;
    double max = 0;
    double min = 4.5;
    int nCells = 0;
    for (int i = 0; i < N_SEGMENTS; i++)
    {
        for (int j = 0; j < N_CELLS_PER_SEGMENT; j++)
        {
            double voltageValue = Accumulator.segments[i].cells[j].voltage;
            if (voltageValue > 0)
            {
                if (voltageValue > max)
                    max = voltageValue;
                if (voltageValue < min)
                    min = voltageValue;

                nCells++;
                voltageMoy += voltageValue;
            }
        }
    }
    double bpVoltage = voltageMoy;
    if (nCells > 0)
        voltageMoy /= nCells;

    snprintf(tmpBuffer, sizeof(tmpBuffer), "%5.2f V ~ %6.2f V\n%6.2f ~ %6.2f",
             bpVoltage, voltageMoy, max, min);
    gtk_label_set_text(GTK_LABEL(label_Voltage_Value), tmpBuffer);
}
void updateAMSTemperatureGUI(uint32_t id, size_t len, uint8_t *dat)
{
    int startix = (id & 0xFF00) / 0x100;
    for (int i = 1; i < 9; i++)
    {
        int cell = 1 + (8 * startix + i - 1) % 32;
        int seg = 1 + (8 * startix + i - 1) / 32;
        Accumulator.segments[seg].cells[cell].temperature = dat[i - 1];
    }

    double tempMoy = 0;
    float min = 255;
    float max = 0;
    int nCells = 0;
    for (int i = 0; i < N_SEGMENTS; i++)
    {
        for (int j = 0; j < N_CELLS_PER_SEGMENT; j++)
        {
            double value = Accumulator.segments[i].cells[j].temperature;
            if (value > 0 & value < 150)
            {
                if (value > max)
                    max = value;
                if (value < min)
                    min = value;

                nCells++;
                tempMoy += value;
            }
        }
    }
    if (nCells > 0)
        tempMoy /= nCells;

    snprintf(tmpBuffer, sizeof(tmpBuffer), "%6.2f \u2103 \n%6.2f ~ %6.2f",
             tempMoy, max, min);
    gtk_label_set_text(GTK_LABEL(label_Temperature_Value), tmpBuffer);
}
void updateTimerGUI(uint32_t id, size_t len, uint8_t *dat)
{
    snprintf(tmpBuffer, sizeof(tmpBuffer), "%c%c%c%c%c%c%c%c", dat[0], dat[1], dat[2], dat[3], dat[4], dat[5], dat[6], dat[7]);
    gtk_label_set_text(GTK_LABEL(label_Time), tmpBuffer);
}
