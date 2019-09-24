/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * data_reader_dump.c
 *
 * Copyright (C) 2019 David Oberhollenzer <goliath@infraroot.at>
 */
#include "config.h"

#include "sqfs/block_processor.h"
#include "sqfs/data_reader.h"
#include "highlevel.h"
#include "util.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int sqfs_data_reader_dump(sqfs_data_reader_t *data,
			  const sqfs_inode_generic_t *inode,
			  int outfd, size_t block_size, bool allow_sparse)
{
	sqfs_block_t *blk;
	uint64_t filesz;
	size_t i, diff;
	int err;

	sqfs_inode_get_file_size(inode, &filesz);

	if (allow_sparse && ftruncate(outfd, filesz))
		goto fail_sparse;

	for (i = 0; i < inode->num_file_blocks; ++i) {
		if (SQFS_IS_SPARSE_BLOCK(inode->block_sizes[i]) &&
		    allow_sparse) {
			if (filesz < block_size) {
				diff = filesz;
				filesz = 0;
			} else {
				diff = block_size;
				filesz -= block_size;
			}

			if (lseek(outfd, diff, SEEK_CUR) == (off_t)-1)
				goto fail_sparse;
		} else {
			err = sqfs_data_reader_get_block(data, inode, i, &blk);
			if (err) {
				fprintf(stderr, "error reading "
					"data block: %d\n", err);
				return -1;
			}

			if (write_data("writing uncompressed block",
				       outfd, blk->data, blk->size)) {
				free(blk);
				return -1;
			}

			filesz -= blk->size;
			free(blk);
		}
	}

	if (filesz > 0) {
		if (sqfs_data_reader_get_fragment(data, inode, &blk)) {
			fputs("error reading fragment block", stderr);
			return -1;
		}

		if (write_data("writing uncompressed fragment", outfd,
			       blk->data, blk->size)) {
			free(blk);
			return -1;
		}

		free(blk);
	}

	return 0;
fail_sparse:
	perror("creating sparse output file");
	return -1;
}