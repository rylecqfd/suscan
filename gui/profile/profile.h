/*

  Copyright (C) 2018 Gonzalo José Carracedo Carballal

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

#ifndef _GUI_PROFILE_H
#define _GUI_PROFILE_H

#include <sigutils/sigutils.h>
#include <suscan.h>
#include <analyzer/source.h>

#include <gtk/gtk.h>

struct suscan_gui;

struct suscan_gui_profile {
  struct suscan_gui *gui;
  GtkBuilder *builder;
  GtkWidget *root;
  GtkWidget *selector;

  /* GUI Widgets */
  GtkEntry *frequencyEntry;
  GtkEntry *sampleRateEntry;
  GtkSpinButton *averageSpinButton;
  GtkCheckButton *iqBalanceCheckButton;
  GtkCheckButton *removeDcCheckButton;
  GtkComboBoxText *deviceComboBoxText;
  GtkComboBoxText *antennaComboBoxText;
  GtkSpinButton *channelSpinButton;
  GtkEntry *bandwidthEntry;
  GtkRadioButton *sdrRadioButton;
  GtkRadioButton *fileRadioButton;
  GtkFileChooserButton *pathFileChooserButton;
  GtkComboBoxText *formatComboBoxText;
  GtkCheckButton *loopCheckButton;
  GtkLabel *profileNameLabel;

  GtkGrid *sdrControlsFrame;
  GtkGrid *fileControlsFrame;

  suscan_source_config_t *config; /* Got from source_config_walk, borrowed */
  suscan_source_t *source;

  SUBOOL changed;
  SUBOOL in_callback;
};

typedef struct suscan_gui_profile suscan_gui_profile_t;

SUINLINE void
suscan_gui_profile_set_gui(
    suscan_gui_profile_t *profile,
    struct suscan_gui *gui)
{
  profile->gui = gui;
}

SUINLINE struct suscan_gui *
suscan_gui_profile_get_gui(suscan_gui_profile_t *profile)
{
  return profile->gui;
}

SUINLINE GtkWidget *
suscan_gui_profile_get_root(const suscan_gui_profile_t *profile)
{
  return profile->root;
}

SUINLINE GtkWidget *
suscan_gui_profile_get_selector(const suscan_gui_profile_t *profile)
{
  return profile->selector;
}

SUINLINE SUBOOL
suscan_gui_profile_has_changed(const suscan_gui_profile_t *profile)
{
  return profile->changed;
}

SUINLINE void
suscan_gui_profile_reset_changed(suscan_gui_profile_t *profile)
{
  profile->changed = SU_FALSE;
}

/*************************** Internal API ************************************/
void suscan_gui_profile_update_sensitivity(suscan_gui_profile_t *profile);

SUBOOL suscan_gui_profile_load_all_widgets(suscan_gui_profile_t *profile);

SUBOOL suscan_gui_profile_refresh_config(suscan_gui_profile_t *profile);

SUBOOL suscan_gui_profile_refresh_gui(suscan_gui_profile_t *profile);

void suscan_gui_profile_destroy(suscan_gui_profile_t *profile);

suscan_gui_profile_t *suscan_gui_profile_new(suscan_source_config_t *cfg);

#endif /* _GUI_PROFILE_H */
