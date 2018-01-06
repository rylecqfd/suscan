/*

  Copyright (C) 2017 Gonzalo José Carracedo Carballal

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program.  If not, see
  <http://www.gnu.org/licenses/>

*/

#ifndef _GUI_SPECTRUM_H
#define _GUI_SPECTRUM_H

#include <glib-object.h>
#include <gtk/gtk.h>
#include <util.h>
#include <stdint.h>
#include <sigutils/softtune.h>

G_BEGIN_DECLS

#define SUGTK_TYPE_SPECTRUM            (sugtk_spectrum_get_type ())
#define SUGTK_SPECTRUM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), SUGTK_TYPE_SPECTRUM, SuGtkSpectrum))
#define SUGTK_SPECTRUM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST  ((klass), SUGTK_TYPE_SPECTRUM, SuGtkSpectrumClass))
#define SUGTK_IS_SPECTRUM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), SUGTK_TYPE_SPECTRUM))
#define SUGTK_IS_SPECTRUM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE  ((klass), SUGTK_TYPE_SPECTRUM))
#define SUGTK_SPECTRUM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS  ((obj), SUGTK_TYPE_SPECTRUM, SuGtkSpectrumClass))

/* Layout defines */
#define SUGTK_SPECTRUM_ALPHA .01

#define SUGTK_SPECTRUM_HORIZONTAL_DIVS 20
#define SUGTK_SPECTRUM_VERTICAL_DIVS   10

#define SUGTK_SPECTRUM_DX (1. / SUGTK_SPECTRUM_HORIZONTAL_DIVS)
#define SUGTK_SPECTRUM_DY (1. / SUGTK_SPECTRUM_VERTICAL_DIVS)

#define SUGTK_SPECTRUM_SCALE_DELTA .1

#define SUGTK_SPECTRUM_LEFT_PADDING 30
#define SUGTK_SPECTRUM_TOP_PADDING 5

#define SUGTK_SPECTRUM_RIGHT_PADDING 5
#define SUGTK_SPECTRUM_BOTTOM_PADDING 30

#define SUGTK_SPECTRUM_AUTO_LEVEL_RANGE_SCALE_DB 1.5

/* Defaults */
#define SUGTK_SPECTRUM_FREQ_OFFSET_DEFAULT 0
#define SUGTK_SPECTRUM_FREQ_SCALE_DEFAULT  1
#define SUGTK_SPECTRUM_DBS_PER_DIV_DEFAULT 10
#define SUGTK_SPECTRUM_REF_LEVEL_DEFAULT   0
#define SUGTK_SPECTRUM_AGC_ALPHA .1

#define SUGTK_SPECTRUM_SETTER_PROTO(type, name) \
  void JOIN(sugtk_spectrum_set_, name) (SuGtkSpectrum *spect, type value)

#define SUGTK_SPECTRUM_GETTER_PROTO(type, name) \
  type JOIN(sugtk_spectrum_get_, name) (const SuGtkSpectrum *spect)

enum SuGtkSpectrumParam {
  SUGTK_SPECTRUM_PARAM_FREQ_OFFSET,
  SUGTK_SPECTRUM_PARAM_FREQ_SCALE,
  SUGTK_SPECTRUM_PARAM_REF_LEVEL,
  SUGTK_SPECTRUM_PARAM_DBS_PER_DIV,
};

enum SuGtkSpectrumMode {
  SUGTK_SPECTRUM_MODE_SPECTROGRAM,
  SUGTK_SPECTRUM_MODE_WATERFALL,
};

struct _SuGtkSpectrum;

typedef void (*SuGtkSpectrumMenuActionCallback) (
    struct _SuGtkSpectrum *spect,
    gdouble freq,
    const struct sigutils_channel *channel,
    gpointer data);

struct _SuGtkSpectrumMenuContext {
  struct _SuGtkSpectrum *spect;
  SuGtkSpectrumMenuActionCallback action;
  gpointer data;
};

typedef struct _SuGtkSpectrumMenuContext SuGtkSpectrumMenuContext;

struct _SuGtkSpectrum
{
  GtkDrawingArea parent_instance;

  /* Spectrum data */
  gdouble *psd_data;
  gdouble *psd_data_smooth;
  guint    psd_size;
  gdouble  N0;
  gdouble  fc;

  /* Spectrum parameters */
  guint samp_rate;

  /* Widget geometry */
  gdouble width;
  gdouble height;

  /* Geometry of the plot area */
  gdouble g_width;
  gdouble g_height;

  /* Surfaces */
  cairo_surface_t *sf_spectrum; /* This is actually the main surface */
  cairo_surface_t *sf_wf[2]; /* Used for keeping the waterfall history */
  gboolean flip;

  /* Zoom and centering state */
  gdouble freq_offset;
  gdouble freq_scale;
  gdouble ref_level;
  gdouble dbs_per_div;

  /* Spectrum behavior */
  enum SuGtkSpectrumMode mode;
  gboolean has_menu;
  gboolean show_channels;
  gboolean auto_level;
  gboolean dc_skip;
  gboolean smooth_N0;
  gdouble  agc_alpha;

  /* Autolevel state */
  gdouble last_max;

  /* Scrolling and motion states */
  gboolean dragging;
  gboolean selecting;
  gfloat   last_x;
  gfloat   last_y;

  gfloat   prev_ev_x;
  gdouble  original_ref_level;
  gdouble  original_freq_offset;

  /* Channel integration */
  struct sigutils_channel selection;
  PTR_LIST(struct sigutils_channel, channel);

  /* Channel menu */
  GtkMenu     *channelMenu;
  GtkMenuItem *channelHeaderMenuItem;
  struct sigutils_channel menu_channel;
  gdouble menu_fc;

  PTR_LIST(SuGtkSpectrumMenuContext, context);
};

struct _SuGtkSpectrumClass
{
  GtkDrawingAreaClass parent_class;
};

typedef struct _SuGtkSpectrum      SuGtkSpectrum;
typedef struct _SuGtkSpectrumClass SuGtkSpectrumClass;

GType sugtk_spectrum_get_type(void);
GtkWidget *sugtk_spectrum_new(void);

const struct sigutils_channel *sugtk_spectrum_lookup_channel(
    const SuGtkSpectrum *spect,
    gdouble fc);

void sugtk_spectrum_update(
    SuGtkSpectrum *spectrum,
    gdouble *spectrum_data,
    guint spectrum_size,
    guint samp_rate,
    gdouble fc,
    gdouble N0);

void sugtk_spectrum_update_channels(
    SuGtkSpectrum *spect,
    struct sigutils_channel **channel_list,
    unsigned int channel_count);

gboolean sugtk_spectrum_add_menu_action(
    SuGtkSpectrum *spect,
    const gchar *label,
    SuGtkSpectrumMenuActionCallback action,
    gpointer data);

void sugtk_spectrum_reset(SuGtkSpectrum *spect);

SUGTK_SPECTRUM_SETTER_PROTO(gboolean, show_channels);
SUGTK_SPECTRUM_SETTER_PROTO(gboolean, auto_level);
SUGTK_SPECTRUM_SETTER_PROTO(gboolean, dc_skip);
SUGTK_SPECTRUM_SETTER_PROTO(gboolean, smooth_N0);
SUGTK_SPECTRUM_SETTER_PROTO(gboolean, has_menu);
SUGTK_SPECTRUM_SETTER_PROTO(enum SuGtkSpectrumMode, mode);
SUGTK_SPECTRUM_SETTER_PROTO(gdouble, freq_offset);
SUGTK_SPECTRUM_SETTER_PROTO(gdouble, freq_scale);
SUGTK_SPECTRUM_SETTER_PROTO(gdouble, ref_level);
SUGTK_SPECTRUM_SETTER_PROTO(gdouble, dbs_per_div);
SUGTK_SPECTRUM_SETTER_PROTO(gdouble, agc_alpha);
SUGTK_SPECTRUM_SETTER_PROTO(gdouble, N0);
SUGTK_SPECTRUM_SETTER_PROTO(guint, samp_rate);

SUGTK_SPECTRUM_GETTER_PROTO(gboolean, show_channels);
SUGTK_SPECTRUM_GETTER_PROTO(gboolean, auto_level);
SUGTK_SPECTRUM_GETTER_PROTO(gboolean, dc_skip);
SUGTK_SPECTRUM_GETTER_PROTO(gboolean, smooth_N0);
SUGTK_SPECTRUM_GETTER_PROTO(gboolean, has_menu);
SUGTK_SPECTRUM_GETTER_PROTO(enum SuGtkSpectrumMode, mode);
SUGTK_SPECTRUM_GETTER_PROTO(gdouble, freq_offset);
SUGTK_SPECTRUM_GETTER_PROTO(gdouble, freq_scale);
SUGTK_SPECTRUM_GETTER_PROTO(gdouble, ref_level);
SUGTK_SPECTRUM_GETTER_PROTO(gdouble, dbs_per_div);
SUGTK_SPECTRUM_GETTER_PROTO(gdouble, agc_alpha);
SUGTK_SPECTRUM_GETTER_PROTO(gdouble, N0);
SUGTK_SPECTRUM_GETTER_PROTO(guint, samp_rate);

G_END_DECLS

#endif /* _GUI_SPECTRUM_H */
