/* callbacks.h -- This file is part of SimpleDevelop.
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

#ifndef _CALLBACKS_H
#define _CALLBACKS_H

#include <gtk/gtk.h>

void sd_project_tree_expand (GtkTreeView *view, GtkTreeIter *iter,
			     GtkTreePath *path, gpointer user_data);

#endif
