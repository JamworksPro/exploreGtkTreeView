#include <stdio.h>
#include <gtk/gtk.h>

GtkWidget *AppWindow;

enum
{
	FIELDONE,
	FIELDTWO,
	N_COLUMNS
};

static GtkWidget *g_scrolledwindow;
static GtkTreeView *g_treeView;
static GtkTreeStore *g_treeStore;
static GtkTreeSelection *g_CurrentTreeViewSelection;

static GtkEntry *g_txtFieldOne;
static GtkEntry *g_txtFieldTwo;
static GtkButton *g_btnAdd;

GtkEventController *g_GtkEventController;

static gboolean key_pressed(GtkEventControllerKey* self, guint keyval, guint keycode, GdkModifierType* state, gpointer user_data)
{
	printf("%d\n", keycode);
	if(keycode == 119)
	{
	  GtkTreeIter iter;
	  GtkTreeModel *model=NULL;
		gboolean b = gtk_tree_selection_get_selected(g_CurrentTreeViewSelection, &model, &iter);
		gboolean bValidIter = gtk_tree_store_remove(g_treeStore, &iter);
		int i=0;
	}
	return false;
}

static void OnAdd(GtkButton *self, gpointer user_data)
{
  GtkTreeIter iter;
  gtk_tree_store_append(g_treeStore, &iter, NULL);
  gtk_tree_store_set(g_treeStore, &iter, FIELDONE, gtk_entry_get_text(g_txtFieldOne), FIELDTWO, gtk_entry_get_text(g_txtFieldTwo), -1);
}

static void OnValueChanged(GtkCellRendererText *self, gchar *path, gchar *new_text, gpointer user_data)
{
  GtkTreeIter iter;
  GtkTreeModel *model=NULL;
	gboolean b = gtk_tree_selection_get_selected(g_CurrentTreeViewSelection, &model, &iter);
	gtk_tree_store_set(g_treeStore, &iter, FIELDTWO, new_text, -1);
}

static int OnTreeViewSelectionChanged(GtkTreeSelection *selection, gpointer data)
{
	g_CurrentTreeViewSelection = selection;
  GtkTreeIter iter;
	GtkTreeModel *model=NULL;
	gboolean b = gtk_tree_selection_get_selected(g_CurrentTreeViewSelection, &model, &iter);
	gchar *item1;
	gchar *item2;
	gtk_tree_model_get(model, &iter, 0, &item1, 0, &item2, -1);
	printf("FIELDONE = %s, FIELDTWO = %s\n",item1, item2);
	return 0;
}

static void activate(GtkApplication *app, gpointer user_data)
{
  GtkBuilder *builder = gtk_builder_new ();

  GError *error = nullptr;
  gtk_builder_add_from_file (builder, "exploreTreeView.xml", &error);

  AppWindow = (GtkWidget*)gtk_builder_get_object (builder, "appExploreTreeView");
  gtk_window_set_application (GTK_WINDOW (AppWindow), app);

  g_txtFieldOne = (GtkEntry*)gtk_builder_get_object(builder, "txtFieldOne");
  g_txtFieldTwo = (GtkEntry*)gtk_builder_get_object(builder, "txtFieldTwo");

  g_btnAdd = (GtkButton*)gtk_builder_get_object(builder, "btnAdd");
	g_signal_connect(g_btnAdd, "clicked", G_CALLBACK(OnAdd), NULL);

  g_scrolledwindow = (GtkWidget*)gtk_builder_get_object(builder, "scrolledwindow");

//Build the Message Headers Table
	{
		GtkTreeViewColumn *column;
		GtkCellRenderer *renderer;

		//Create a model using the treeStore model though there are
		//other models in which to build a GtkTreeModel
		g_treeStore = gtk_tree_store_new(N_COLUMNS,
															 G_TYPE_STRING,
															 G_TYPE_STRING);

		g_treeView = (GtkTreeView*)gtk_tree_view_new_with_model(GTK_TREE_MODEL(g_treeStore));
		gtk_tree_view_set_activate_on_single_click((GtkTreeView*)g_treeView, true);

		//Setup the selection handler
		g_CurrentTreeViewSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW(g_treeView));
		gtk_tree_selection_set_mode(g_CurrentTreeViewSelection, GTK_SELECTION_SINGLE);
		g_signal_connect (G_OBJECT (g_CurrentTreeViewSelection), "changed", G_CALLBACK (OnTreeViewSelectionChanged), NULL);

		//Setup the GtkTreeView to handle keyboard events
		g_GtkEventController = gtk_event_controller_key_new((GtkWidget*)g_treeView);
		g_signal_connect(g_GtkEventController, "key-pressed", G_CALLBACK(key_pressed), NULL);

		//Create the FIELDONE column and append it to the g_treeView object
		renderer = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("Field One", renderer, "text", FIELDONE, NULL);
		gtk_tree_view_column_set_sort_column_id(column, FIELDONE);
		gtk_tree_view_column_set_alignment(column, 0.0);
		gtk_tree_view_column_set_resizable(column, true);

		gtk_tree_view_append_column(GTK_TREE_VIEW(g_treeView), column);

		//Create the FIELDTWO column and append it to the g_treeView object
		renderer = gtk_cell_renderer_text_new ();
		g_object_set(renderer, "editable", TRUE, NULL);
		g_signal_connect(renderer, "edited", (GCallback) OnValueChanged, NULL);
		column = gtk_tree_view_column_new_with_attributes ("Field Two", renderer, "text", FIELDTWO, NULL);
		gtk_tree_view_column_set_sort_column_id (column, FIELDTWO);
		gtk_tree_view_column_set_resizable(column, true);

		gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(column), 150);
		gtk_tree_view_append_column(GTK_TREE_VIEW(g_treeView), column);
	}

	gtk_container_add((GtkContainer*)g_scrolledwindow, (GtkWidget*)g_treeView);

  gtk_widget_show_all(GTK_WIDGET (AppWindow));

  g_object_unref (builder);
}

int main (int argc, char *argv[])
{
  GtkApplication *app = gtk_application_new ("com.jamworkspro.exploretreeview", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  int status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
