/* sd-project-tree.h -- This file is part of SimpleDevelop.
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

#ifndef _SD_PROJECT_TREE_H
#define _SD_PROJECT_TREE_H

#include "sd-window.h"

enum
{
  NAME_COLUMN = 0,
  FG_COLUMN,
  FILE_COLUMN,
  N_COLUMNS
};

G_BEGIN_DECLS

#define SD_TYPE_PROJECT_TREE sd_project_tree_get_type ()
G_DECLARE_FINAL_TYPE (SDProjectTree, sd_project_tree, SD, PROJECT_TREE,
		      GtkTreeView)

struct _SDProjectTree
{
  GtkTreeView parent;
};

SDProjectTree *sd_project_tree_new (SDWindow *window, GFile *file);

G_END_DECLS

#endif
