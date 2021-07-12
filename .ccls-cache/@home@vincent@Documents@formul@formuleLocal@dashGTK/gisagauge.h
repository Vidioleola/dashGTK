#ifndef __GISAGAUGE_H__
#define __GISAGAUGE_H__

#include <gtk-3.0/gtk/gtk.h>

#include <cairo/cairo.h> //for drawing

#define ANGLE_START G_PI
#define ANGLE_STOP 2 * G_PI
#define OFF_DRAW 10

G_BEGIN_DECLS

/* Standart GObject macros */
#define GISA_TYPE_GAUGE (gisa_gauge_get_type())
#define GISA_GAUGE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GISA_TYPE_GAUGE, GisaGauge))
#define GISA_GAUGE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GISA_TYPE_GAUGE, GisaGaugeClass))
#define GISA_IS_GAUGE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GISA_TYPE_GAUGE))
#define GISA_IS_GAUGE_CLASS(klass) (G_TYPE_CHECk_CLASS_TYPE((klass), GISA_TYPE_GAUGE))
#define GISA_GAUGE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GISA_TYPE_GAUGE, GisaGaugeClass))

/* Type definition */
typedef struct _GisaGauge GisaGauge;
typedef struct _GisaGaugeClass GisaGaugeClass;
typedef struct _GisaGaugePrivate GisaGaugePrivate;

struct _GisaGauge
{
    GtkWidget parent;
    /*< Private >*/
    GisaGaugePrivate *priv;
};

struct _GisaGaugeClass
{
    GtkWidgetClass parent_class;
};

/* public API */
GType gisa_gauge_get_type(void) G_GNUC_CONST;
GtkWidget *gisa_gauge_new(void);

//set value widget
void gisa_gauge_set_value(GisaGauge *widget, gdouble value);
//get value widget
gdouble gisa_gauge_get_value(GisaGauge *widget);

//set max value
void gisa_gauge_set_max_value(GisaGauge *widget, gdouble maxValue);
//get max value
gdouble gisa_gauge_get_max_value(GisaGauge *widget);

//set min value
void gisa_gauge_set_min_value(GisaGauge *widget, gdouble minValue);
//get min value
gdouble gisa_gauge_get_min_value(GisaGauge *widget);

G_END_DECLS

#endif /*__GISAGAUGE_H__*/