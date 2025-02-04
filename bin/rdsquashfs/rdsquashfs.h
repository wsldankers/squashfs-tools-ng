/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * rdsquashfs.h
 *
 * Copyright (C) 2019 David Oberhollenzer <goliath@infraroot.at>
 */
#ifndef RDSQUASHFS_H
#define RDSQUASHFS_H

#include "config.h"
#include "common.h"
#include "fstree.h"
#include "util/util.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>

#if defined(__APPLE__) && defined(__MACH__)
#define lsetxattr(path, name, value, size, flags) \
	setxattr(path, name, value, size, 0, flags | XATTR_NOFOLLOW)
#endif
#endif
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>

enum UNPACK_FLAGS {
	UNPACK_CHMOD = 0x01,
	UNPACK_CHOWN = 0x02,
	UNPACK_QUIET = 0x04,
	UNPACK_NO_SPARSE = 0x08,
	UNPACK_SET_XATTR = 0x10,
	UNPACK_SET_TIMES = 0x20,
};

enum {
	OP_NONE = 0,
	OP_LS,
	OP_CAT,
	OP_UNPACK,
	OP_DESCRIBE,
	OP_RDATTR,
	OP_STAT,
};

typedef struct {
	int op;
	int rdtree_flags;
	int flags;
	char *cmdpath;
	const char *unpack_root;
	const char *image_name;
} options_t;

void list_files(const sqfs_tree_node_t *node);

int stat_file(const sqfs_tree_node_t *node);

int restore_fstree(sqfs_tree_node_t *root, int flags);

int update_tree_attribs(sqfs_xattr_reader_t *xattr,
			const sqfs_tree_node_t *root, int flags);

int fill_unpacked_files(size_t blk_sz, const sqfs_tree_node_t *root,
			sqfs_data_reader_t *data, int flags);

int describe_tree(const sqfs_tree_node_t *root, const char *unpack_root);

int dump_xattrs(sqfs_xattr_reader_t *xattr, const sqfs_inode_generic_t *inode);

void process_command_line(options_t *opt, int argc, char **argv);

#endif /* RDSQUASHFS_H */
