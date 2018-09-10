/*
 *
 * atfblastusb.c
 *
 * Minimal GAL and SPLD programmer
 *
 * New GTK+ GUI.
 *
 * GAL programming code based on ATFBlast by Bruce Abbott.
 *
 * Bruce's ATFBlast was based on GALBlast by Manfred Winterhoff.
 *
 * This program is freeware.  Use at your own risk.
 *
 * 20180906 Leeland Heins
 *
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkkeysyms-compat.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>

#include "atfblastusb.h"

/* FIXME -- this needs to be done better. */
char open_file_name[FILENAME_SIZE];
char *open_file_ptr;

GtkWidget *text_view;

/*
 *
 * error_dialog()
 *
 * Display an informational dialog box.
 *
 */
void error_dialog(char *message, char *errtxt)
{
  GtkWidget *errdialog;
  int response;

  errdialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", message);

  if (errtxt != NULL) {
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(errdialog), "%s", errtxt);
  }

  response = gtk_dialog_run(GTK_DIALOG(errdialog));

  g_print("response was %d", response);

  gtk_widget_destroy(errdialog);
}

#ifdef IN_PROGRESS
void checksum_message(int checksum1, int checksum2)
{
  char msg[256];

  sprintf(msg, "Checksum given %04X calculated %04X.", checksum1, checksum2);
  error_dialog(msg, NULL);

  return;
}


#endif

/*
 *
 * do_openJEDEC()
 *
 * Open a JEDEC file.
 *
 */
void do_openJEDEC(char *filename)
{
  FILE *jedec_file;
  char jedec_text[JDEC_BUFFER_SIZE];
  GtkTextBuffer *buffer;
  char ch;
  gchar *buf;
  GError *error = NULL;

  /* --- Display message --- */
  /*g_print("Open JEDEC file function goes here.\n");*/
  /*g_print("filename=%s\n", filename);*/

  strncpy(open_file_ptr, filename, (FILENAME_SIZE - 1));

  jedec_file = fopen(filename, "r");

  if (jedec_file) {
    /*g_print("Contents of %s:\n", filename);*/
    /*while (fscanf(jedec_file, "%s", str) != EOF) {
      g_print("%s", str);
    }*/
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    while ((ch = fgetc(jedec_file)) != EOF) {
      if (ch == '\r') {
        strcat(jedec_text, "\n");
        g_print("%c", '\n');
      }
      if (ch >= ' ' && ch < 128) {
        strcat(jedec_text, &ch);
        g_print("%c", ch);
      }
    }

    /*g_print("\n");*/

    buf = g_strdup_printf("%s", g_convert(jedec_text, strlen(jedec_text), "UTF-8", "ISO-8859-1", NULL, NULL, &error));

    if (error != NULL) {
      g_print("Couldn't convert file %s to UTF-8\n", filename);
      /* Pop up error message */
      error_dialog("Error converting file to UTF-8", NULL);
      g_error_free(error);
    } else {
      gtk_text_buffer_set_text(buffer, buf, -1);
    }
  } else {
    /* Pop up error message */
    error_dialog("Error opening file.", NULL);
  }

  fclose(jedec_file);
}

/*
 *
 * do_saveJEDEC()
 *
 * Save a JEDEC file.
 *
 */
void do_saveJEDEC(char *filename)
{
  GtkTextBuffer *buffer;
  GtkTextIter start;
  GtkTextIter end;
  gchar *jedec_text;
  /*GError *err;*/
  int result = FALSE;
  FILE *jedec_file;
  /*gchar *chptr;*/

  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);

  /* --- Display message --- */
  g_print("Save JEDEC file function goes here.\n");
  g_print("filename=%s\n", filename);

  jedec_text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

  g_print("jedec_text=%s\n", jedec_text);

  if (filename != NULL) {
    g_print("Save it to %s\n", filename);
    /*result = g_file_set_contents(filename, jedec_text, -1, &err);*/
    jedec_file = fopen(filename, "w");

    fputc(0x02, jedec_file);

    /* Write data to file converting characters */
    /*chptr = jedec_text;
    while (*chptr != 0x00) {
      if (*chptr == '\n') {
        fputc('\r', jedec_file);
      }
      fputc(*chptr, jedec_file);
    }*/
    g_fprintf(jedec_file, "%s", jedec_text);

    fputc(0x03, jedec_file);

    fclose(jedec_file);
  }

  if (result == FALSE) {
    /*error_message(err->message);
    g_error_free(err);*/
    g_print("ERROR\n");
  }
  
  g_free(jedec_text);
}

/*
 *
 * openJEDEC()
 *
 * Open JEDEC file dialog
 *
 */
void openJEDEC()
{
  GtkWidget *dialog;
  gint res;
  GtkFileFilter *jedFilt;
  GtkFileFilter *allFilesFilt;

  jedFilt = gtk_file_filter_new();
  gtk_file_filter_set_name(jedFilt, "JED Files");
  gtk_file_filter_add_pattern(jedFilt, "*.[Jj][Ee][Dd]");

  allFilesFilt = gtk_file_filter_new();
  gtk_file_filter_set_name(allFilesFilt, "All Files");
  gtk_file_filter_add_pattern(allFilesFilt, "*");

  dialog = gtk_file_chooser_dialog_new("Open JEDEC",
                                       NULL,
                                       GTK_FILE_CHOOSER_ACTION_OPEN,
                                       GTK_STOCK_CANCEL,
                                       GTK_RESPONSE_CANCEL,
                                       GTK_STOCK_OPEN,
                                       GTK_RESPONSE_ACCEPT,
                                       NULL);

  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), jedFilt);
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), allFilesFilt);

  res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    filename = gtk_file_chooser_get_filename(chooser);
    do_openJEDEC(filename);
    g_free(filename);
  }

  gtk_widget_destroy(dialog);
}

/*
 *
 * saveJEDEC()
 *
 * Save file dialog
 *
 */
void saveJEDEC()
{
  /*g_print("save JEDEC file function goes here.\n");*/
  GtkWidget *dialog;
  GtkFileChooser *chooser;
  gint res;

  dialog = gtk_file_chooser_dialog_new("Open JEDEC",
                                       NULL,
                                       GTK_FILE_CHOOSER_ACTION_SAVE,
                                       GTK_STOCK_CANCEL,
                                       GTK_RESPONSE_CANCEL,
                                       GTK_STOCK_SAVE,
                                       GTK_RESPONSE_ACCEPT,
                                       NULL);

  chooser = GTK_FILE_CHOOSER(dialog);

  if (strncmp(open_file_ptr, "", (FILENAME_SIZE - 1)) != 0) {
    gtk_file_chooser_set_filename(chooser, open_file_ptr);
  } else {
    gtk_file_chooser_set_current_name(chooser, "untitled.jed");
  }

  res = gtk_dialog_run(GTK_DIALOG(dialog));
  if (res == GTK_RESPONSE_ACCEPT) {
    char *filename;
    /*GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);*/
    filename = gtk_file_chooser_get_filename(chooser);
    do_saveJEDEC(filename);
    g_free(filename);
  }

  gtk_widget_destroy(dialog);
}

/*
 *
 * writePES():
 *
 * Write PES function.
 *
 */
void writePES(GtkWidget *widget, gpointer data)
{
  /* --- Display message --- */
  g_print("Write PES function goes here.\n");
  error_dialog("Write PES function goes here.\n", NULL);
}

/*
 *
 * readGAL():
 *
 * Read GAL function.
 *
 */
void readGAL(GtkWidget *widget, gpointer data)
{
  /* --- Display message --- */
  g_print("Read GAL function goes here.\n");
  error_dialog("Read GAL function goes here.\n", NULL);
}

/*
 *
 * verifyGAL():
 *
 * Verify GAL function.
 *
 */
void verifyGAL(GtkWidget *widget, gpointer data)
{
  /* --- Display message --- */
  g_print("Verify GAL function goes here.\n");
  error_dialog("Verify GAL function goes here.\n", NULL);
}

/*
 *
 * security():
 *
 * Security function.
 *
 */
void security(GtkWidget *widget, gpointer data)
{
  /* --- Display message --- */
  g_print("SECURITY function goes here.\n");
  error_dialog("SECURITY function goes here.\n", NULL);
}

/*
 *
 * writeGAL():
 *
 * Write GAL function.
 *
 */
void writeGAL(GtkWidget *widget, gpointer data)
{
  /* --- Display message --- */
  g_print("Write GAL function goes here.\n");
  error_dialog("Write GAL function goes here.\n", NULL);
}

/*
 *
 * eraseGAL():
 *
 * Erase GAL function.
 *
 */
void eraseGAL(GtkWidget *widget, gpointer data)
{
  /* --- Display message --- */
  g_print("Erase GAL function goes here.\n");
  error_dialog("Erase GAL function goes here.\n", NULL);
}

/*
 *
 * eraseALL():
 *
 * Erase ALL function.
 *
 */
void eraseALL(GtkWidget *widget, gpointer data)
{
  /* --- Display message --- */
  g_print("ERASE ALL function goes here.\n");
  error_dialog("ERASE ALL function goes here.\n", NULL);
}

/*
 *
 * contents()
 *
 * contents Volume from the menu.
 *
 */
void contents(GtkWidget *widget, gpointer data)
{
  /* --- Display message --- */
  g_print("contents function goes here.\n");
}

/*
 *
 * Show the about box
 *
 */
void show_about(GtkWidget *widget, gpointer data)
{
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file("/usr/share/icons/atfblastusb.png", NULL);

  GtkWidget *dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "ATFBlastUSB");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.1");
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"(c) 2018 Leeland Heins");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
     "Minimal GAL and SPLD programmer.\nGAL programming code based on ATFBlast by Bruce Abbott.\nATFBlast was based on GALBlast by Manfred Winterhoff.\nThis program is freeware.  Use at your own risk.");
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
     "http://softwarejanitor.com");
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  g_object_unref(pixbuf), pixbuf = NULL;
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}


int main(int argc, char *argv[])
{
  /* Our root window */
  GtkWidget *window;

  /* Primary app vbox */
  GtkWidget *vbox;

  GtkWidget *menubar;

  /* Controls area vbox */
  GtkWidget *vbox2;

  /* Controls area hboxes */
  GtkWidget *hbox1;
  GtkWidget *hbox2;
  GtkWidget *hbox3;

  /* File menu widgets */
  GtkWidget *fileMenu;
  GtkWidget *fileMi;
  GtkWidget *openMi;
  GtkWidget *saveMi;
  GtkWidget *fileSep1;
  GtkWidget *quitMi;

  /* Help menu widgets */
  GtkWidget *helpMenu;
  GtkWidget *helpMi;
  GtkWidget *contentsMi;
  GtkWidget *helpSep1;
  GtkWidget *aboutATFBlastUSBMi;

  /* For keyboard accellerators */
  GtkAccelGroup *accel_group = NULL;

  GtkWidget *statusbar;

  GtkWidget *deviceLabel;

  GList *deviceComboItems = NULL;
  GtkWidget *deviceCombo;

  GtkWidget *typeLabel;

  GList *typeComboItems = NULL;
  GtkWidget *typeCombo;

  GtkWidget *writePESButton;

  GtkWidget *openButton;

  GtkWidget *readGALButton;

  GtkWidget *verifyGALButton;

  GtkWidget *securityButton;

  GtkWidget *saveButton;

  GtkWidget *writeGALButton;

  GtkWidget *eraseGALButton;

  GtkWidget *eraseALLButton;

  GtkWidget *scrolled_window;

  GtkTextBuffer *buffer;

  open_file_ptr = (char *)open_file_name;

  /* Initialise GTK+ passing to it all command line arguments  */
  gtk_init(&argc, &argv);

  /* create a new window, set values */
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
  gtk_window_set_title(GTK_WINDOW(window), "ATFBlastUSB: minimal GAL and SPLD programmer");
  gtk_window_set_icon_from_file(GTK_WINDOW(window), "/usr/share/icons/atfblastusb.png", NULL);

  /* Primary app vbox */
  vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), vbox);

  /* Controls area vbox */
  vbox2 = gtk_vbox_new(FALSE, 0);

  /* Controls area hboxes */
  hbox1 = gtk_hbox_new(FALSE, 0);
  hbox2 = gtk_hbox_new(FALSE, 0);
  hbox3 = gtk_hbox_new(FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox2), hbox1, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), hbox2, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), hbox3, TRUE, TRUE, 0);

  /* Primary app menu bar */
  menubar = gtk_menu_bar_new();
  /* File menu */
  fileMenu = gtk_menu_new();
  /* Help menu */
  helpMenu = gtk_menu_new();

  /* For keyboard accellerators */
  accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

  /* File menu bar items */
  fileMi = gtk_menu_item_new_with_mnemonic("_File");

  openMi = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, accel_group);

  saveMi = gtk_image_menu_item_new_from_stock(GTK_STOCK_SAVE, accel_group);

  fileSep1 = gtk_separator_menu_item_new();

  quitMi = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);

  gtk_widget_add_accelerator(quitMi, "activate", accel_group, GDK_q, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  /* Help menu bar items */
  helpMi = gtk_menu_item_new_with_mnemonic("_Help");
  contentsMi = gtk_menu_item_new_with_mnemonic("_Contents...");
  helpSep1 = gtk_separator_menu_item_new();
  aboutATFBlastUSBMi = gtk_menu_item_new_with_label("About ATFBlastUSB");

  /* Set up file menu */
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), openMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), saveMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), fileSep1);
  gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);

  /* Add file menu to menu bar */
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileMi);

  /* Set up help menu */
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(helpMi), helpMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), contentsMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), helpSep1);
  gtk_menu_shell_append(GTK_MENU_SHELL(helpMenu), aboutATFBlastUSBMi);
  /* Add help menu to menu bar */
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), helpMi);

  /* Add menu bar to vbox */
  gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

  /* connect 'open jedec' menu item to the create welcomeage dialog event */
  g_signal_connect(G_OBJECT(openMi), "activate", G_CALLBACK(openJEDEC), (gpointer)"Open JEDEC");

  /* connect 'save jedec' menu item to the create welcomeage dialog event */
  g_signal_connect(G_OBJECT(saveMi), "activate", G_CALLBACK(saveJEDEC), (gpointer)"Save JEDEC");

  /* Add controls area vbox to vbox */
  gtk_box_pack_start(GTK_BOX(vbox), vbox2, FALSE, FALSE, 0);

  /* Add the status bar */
  statusbar = gtk_statusbar_new();
  gtk_box_pack_end(GTK_BOX(vbox), statusbar, FALSE, TRUE, 0);

  /* Connect 'quit' menu item to the window's "destroy" event */
  g_signal_connect(G_OBJECT(quitMi), "activate", G_CALLBACK(gtk_main_quit), (gpointer)"Quit");

  /* connect 'contents' menu item to the contents box event */
  g_signal_connect(G_OBJECT(contentsMi), "activate", G_CALLBACK(contents), (gpointer)"Contents");

  /* connect 'about' menu item to the about box event */
  g_signal_connect(G_OBJECT(aboutATFBlastUSBMi), "activate", G_CALLBACK(show_about), (gpointer)"About ATFBlastUSB");

  deviceLabel = gtk_label_new("Device: ");
  gtk_misc_set_alignment(GTK_MISC(deviceLabel), 0, 0.5);

  gtk_box_pack_start(GTK_BOX(hbox1), deviceLabel, TRUE, TRUE, 0);

  /* Create a list of the devices */
  deviceComboItems = g_list_append(deviceComboItems, "USB ATFBlaster");

  /* Make a combo box for devices */
  deviceCombo = gtk_combo_new();

  /* Create the drop down portion of the combo */
  gtk_combo_set_popdown_strings(GTK_COMBO(deviceCombo), deviceComboItems);

  /* --- Default the text in the field to a value --- */
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(deviceCombo)->entry), "USB ATFBlaster");

  /* --- Make the edit portion non-editable.  They can pick a
   *     value from the drop down, they just can't end up with
   *     a value that's not in the drop down.
   */
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(deviceCombo)->entry), FALSE);

  gtk_box_pack_start(GTK_BOX(hbox1), deviceCombo, TRUE, TRUE, 0);

  typeLabel = gtk_label_new("Type: ");
  gtk_misc_set_alignment(GTK_MISC(typeLabel), 0, 0.5);

  gtk_box_pack_start(GTK_BOX(hbox1), typeLabel, TRUE, TRUE, 0);

  /* Create a list of the device types */
  typeComboItems = g_list_append(typeComboItems, "GAL16V8");
  typeComboItems = g_list_append(typeComboItems, "GAL20V8");
  typeComboItems = g_list_append(typeComboItems, "GAL22V10");
  typeComboItems = g_list_append(typeComboItems, "ATF16V8B");
  typeComboItems = g_list_append(typeComboItems, "ATF22V10B");
  typeComboItems = g_list_append(typeComboItems, "ATF22V10C");

  /* Make a combo box for device types */
  typeCombo = gtk_combo_new();

  /* Create the drop down portion of the combo */
  gtk_combo_set_popdown_strings(GTK_COMBO(typeCombo), typeComboItems);

  /* --- Default the text in the field to a value --- */
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(typeCombo)->entry), "ATF22V10C");

  /* --- Make the edit portion non-editable.  They can pick a
   *     value from the drop down, they just can't end up with
   *     a value that's not in the drop down.
   */
  gtk_entry_set_editable(GTK_ENTRY(GTK_COMBO(typeCombo)->entry), FALSE);

  gtk_box_pack_start(GTK_BOX(hbox1), typeCombo, TRUE, TRUE, 0);

  /* Create Write PES button */
  writePESButton = gtk_button_new_with_label("Write PES");
  gtk_box_pack_start(GTK_BOX(hbox1), writePESButton, TRUE, TRUE, 0);
  gtk_widget_show(writePESButton);

  /* Create Open button */
  openButton = gtk_button_new_with_label("Open JEDEC");
  gtk_box_pack_start(GTK_BOX(hbox2), openButton, TRUE, TRUE, 0);
  gtk_widget_show(openButton);

  /* Create Read GAL button */
  readGALButton = gtk_button_new_with_label("Read GAL");
  gtk_box_pack_start(GTK_BOX(hbox2), readGALButton, TRUE, TRUE, 0);
  gtk_widget_show(readGALButton);

  /* Create Verify GAL button */
  verifyGALButton = gtk_button_new_with_label("Verify GAL");
  gtk_box_pack_start(GTK_BOX(hbox2), verifyGALButton, TRUE, TRUE, 0);
  gtk_widget_show(verifyGALButton);

  /* Create SECURITY button */
  securityButton = gtk_button_new_with_label("SECURITY");
  gtk_box_pack_start(GTK_BOX(hbox2), securityButton, TRUE, TRUE, 0);
  gtk_widget_show(securityButton);

  /* Create Save button */
  saveButton = gtk_button_new_with_label("Save JEDEC");
  gtk_box_pack_start(GTK_BOX(hbox3), saveButton, TRUE, TRUE, 0);
  gtk_widget_show(saveButton);

  /* Create Write GAL button */
  writeGALButton = gtk_button_new_with_label("Write GAL");
  gtk_box_pack_start(GTK_BOX(hbox3), writeGALButton, TRUE, TRUE, 0);
  gtk_widget_show(writeGALButton);

  /* Create Erase GAL button */
  eraseGALButton = gtk_button_new_with_label("Erase GAL");
  gtk_box_pack_start(GTK_BOX(hbox3), eraseGALButton, TRUE, TRUE, 0);
  gtk_widget_show(eraseGALButton);

  /* Create ERASE ALL button */
  eraseALLButton = gtk_button_new_with_label("ERASE ALL");
  gtk_box_pack_start(GTK_BOX(hbox3), eraseALLButton, TRUE, TRUE, 0);
  gtk_widget_show(eraseALLButton);

  /* connect Write PES Button to the Write PES event */
  g_signal_connect(G_OBJECT(writePESButton), "clicked", G_CALLBACK(writePES), (gpointer)"Write PES");

  /* connect Open Button to the openJEDEC event */
  g_signal_connect(G_OBJECT(openButton), "clicked", G_CALLBACK(openJEDEC), (gpointer)"Open JEDEC");

  /* connect Read GAL Button to the Read GAL event */
  g_signal_connect(G_OBJECT(readGALButton), "clicked", G_CALLBACK(readGAL), (gpointer)"Read GAL");

  /* connect Verify GAL Button to the Write PES event */
  g_signal_connect(G_OBJECT(verifyGALButton), "clicked", G_CALLBACK(verifyGAL), (gpointer)"Verify GAL");

  /* connect SECURITY Button to the open_file event */
  g_signal_connect(G_OBJECT(securityButton), "clicked", G_CALLBACK(security), (gpointer)"SECURITY");

  /* connect Save Button to the open_file event */
  g_signal_connect(G_OBJECT(saveButton), "clicked", G_CALLBACK(saveJEDEC), (gpointer)"Save JEDEC");

  /* connect Write GAL Button to the Read GAL event */
  g_signal_connect(G_OBJECT(writeGALButton), "clicked", G_CALLBACK(writeGAL), (gpointer)"Write GAL");

  /* connect Erase GAL Button to the Read GAL event */
  g_signal_connect(G_OBJECT(eraseGALButton), "clicked", G_CALLBACK(eraseGAL), (gpointer)"Erase GAL");

  /* connect ERASE ALL Button to the Read ALL event */
  g_signal_connect(G_OBJECT(eraseALLButton), "clicked", G_CALLBACK(eraseALL), (gpointer)"ERASE ALL");

  deviceLabel = gtk_label_new("Device: ");
  /* create a new scrolled window. */
  scrolled_window = gtk_scrolled_window_new(NULL, NULL);

  gtk_widget_set_size_request(scrolled_window, 512, 256);

  gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 10);

  /* the policy is one of GTK_POLICY AUTOMATIC, or GTK_POLICY_ALWAYS.
   * GTK_POLICY_AUTOMATIC will automatically decide whether you need
   * scrollbars, whereas GTK_POLICY_ALWAYS will always leave the scrollbars
   * there.  The first one is the horizontal scrollbar, the second,
   * the vertical. */
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  /* The dialog window is created with a vbox packed into it. */                 
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
  gtk_widget_show(scrolled_window);

  /* Create a multiline text widget */
  text_view = gtk_text_view_new();

  /* Obtain the buffer associated with the widget */
  buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  /* Set the default buffer text. */
  gtk_text_buffer_set_text(buffer, "", -1);

  /* pack the table into the scrolled window */
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), text_view);
  gtk_widget_show(text_view);

  /* connect the windows "destroy" event */
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

  /* set the window as visible */
  gtk_widget_show_all(window);

  /* run the GTK+ main loop */
  gtk_main();

  return 0;
}

