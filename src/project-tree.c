/* project-tree.c -- This file is part of SimpleDevelop.
   Copyright (C) 2021 XNSC

   SimpleDevelop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   SimpleDevelop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with SimpleDevelop. If not, see <https://www.gnu.org/licenses/>. */

#include "project-tree.h"
#include "sd-window.h"

void
sd_project_tree_populate (GtkTreeStore *store, GtkTreeIter *parent, GFile *file)
{
  GError *err = NULL;
  GtkTreeIter child;
  GFileEnumerator *en =
    g_file_enumerate_children (file, G_FILE_ATTRIBUTE_STANDARD_NAME ","
			       G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME,
			       G_FILE_QUERY_INFO_NONE, NULL, &err);
  if (err != NULL)
    {
      gchar *path = g_file_get_path (file);
      if (g_error_matches (err, G_IO_ERROR, G_IO_ERROR_NOT_DIRECTORY))
	g_warning ("%s is not a directory, skipping", path);
      else
	g_critical ("Failed to read contents of %s: %s", path, err->message);
      g_free (path);
      g_error_free (err);
      return;
    }

  while (TRUE)
    {
      GFileInfo *info;
      GFile *subdir;
      const gchar *dispname;
      if (!g_file_enumerator_iterate (en, &info, NULL, NULL, &err))
	goto finish;
      if (info == NULL)
	break;

      dispname = g_file_info_get_display_name (info);
      gtk_tree_store_append (store, &child, parent);
      gtk_tree_store_set (store, &child, NAME_COLUMN, dispname, FG_COLUMN,
			  "Black", -1);

      if (g_file_info_get_file_type (info) == G_FILE_TYPE_DIRECTORY)
        {
	  subdir = g_file_get_child (file, g_file_info_get_name (info));
	  sd_project_tree_populate (store, &child, subdir);
	}
    }

 finish:
  if (err != NULL)
    {
      gchar *path = g_file_get_path (file);
      g_critical ("Failed to read contents of %s: %s", path, err->message);
      g_free (path);
      g_error_free (err);
    }
  g_object_unref (en);
}
