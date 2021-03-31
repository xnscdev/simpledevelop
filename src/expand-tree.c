/* expand-tree.c -- This file is part of SimpleDevelop.
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

#include "callbacks.h"
#include "sd-window.h"

void
sd_project_tree_expand (GtkTreeView *view, GtkTreeIter *iter, GtkTreePath *path,
			gpointer user_data)
{
  GtkTreeModel *model = gtk_tree_view_get_model (view);
  GtkTreeIter child;
  gchar *name;
  gchar *color;
  gboolean unloaded;

  if (gtk_tree_model_iter_n_children (model, iter) != 1)
    return;
  g_return_if_fail (gtk_tree_model_iter_children (model, &child, iter));

  gtk_tree_model_get (model, &child, NAME_COLUMN, &name, FG_COLUMN, &color, -1);
  unloaded = g_strcmp0 (name, "Loading...") == 0
    && g_strcmp0 (color, "LightSlateGray") == 0;
  g_free (name);
  g_free (color);
  if (!unloaded)
    return;
  g_message ("Expanded a tree!");
}
