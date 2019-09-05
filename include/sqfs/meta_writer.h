/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * meta_writer.h
 *
 * Copyright (C) 2019 David Oberhollenzer <goliath@infraroot.at>
 */
#ifndef SQFS_META_WRITER_H
#define SQFS_META_WRITER_H

#include "config.h"

#include "sqfs/compress.h"
#include "sqfs/id_table.h"
#include "sqfs/data.h"

typedef struct sqfs_meta_writer_t sqfs_meta_writer_t;

#ifdef __cplusplus
extern "C" {
#endif

/* Create a meta data reader using a given compressor to compress data.
   Internally prints error message to stderr on failure.
   If keep_in_mem is true, the blocks are collected in memory and must
   be explicitly flushed to disk using meta_write_write_to_file.
*/
sqfs_meta_writer_t *sqfs_meta_writer_create(int fd, compressor_t *cmp,
					    bool keep_in_mem);

void sqfs_meta_writer_destroy(sqfs_meta_writer_t *m);

/* Compress and flush the currently unfinished block to disk. Returns 0 on
   success, internally prints error message to stderr on failure */
int sqfs_meta_writer_flush(sqfs_meta_writer_t *m);

/* Returns 0 on success. Prints error message to stderr on failure. */
int sqfs_meta_writer_append(sqfs_meta_writer_t *m, const void *data,
			    size_t size);

/* Query the current block start position and offset within the block */
void sqfs_meta_writer_get_position(const sqfs_meta_writer_t *m,
				   uint64_t *block_start,
				   uint32_t *offset);

/* Reset all internal state, including the current block start position. */
void sqfs_meta_writer_reset(sqfs_meta_writer_t *m);

/* If created with keep_in_mem true, write the collected blocks to disk.
   Does not flush the current block. Writes error messages to stderr and
   returns non-zero on failure. */
int sqfs_meta_write_write_to_file(sqfs_meta_writer_t *m);

#ifdef __cplusplus
}
#endif

#endif /* SQFS_META_WRITER_H */
