/*
 * The little filesystem
 *
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef LFS_H
#define LFS_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

/// Definitions ///
#define LFS_CHECK_SUM

/// Version info ///

// Software library version
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define LFS_VERSION 0x00010007
#define LFS_VERSION_MAJOR (0xffff & (LFS_VERSION >> 16))
#define LFS_VERSION_MINOR (0xffff & (LFS_VERSION >>  0))

// Version of On-disk data structures
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions

#define LFS_DISK_VERSION 0x00010001
#define LFS_DISK_VERSION_MAJOR (0xffff & (LFS_DISK_VERSION >> 16))
#define LFS_DISK_VERSION_MINOR (0xffff & (LFS_DISK_VERSION >>  0))

// Type definitions
typedef UINT32 lfs_size_t;
typedef UINT32 lfs_off_t;

typedef int  lfs_ssize_t;
typedef int  lfs_soff_t;

typedef UINT32 lfs_block_t;

// Max name size in bytes
#ifndef LFS_NAME_MAX
#define LFS_NAME_MAX 255
#endif

// Max file size in bytes
#ifndef LFS_FILE_MAX
#define LFS_FILE_MAX 2147483647
#endif

// Possible error codes, these are negative to allow
// valid positive return values
enum lfs_error {
    LFS_ERR_OK       = 0,    // No error
    LFS_ERR_BLOCK    = -1,   // Error LFS block
    LFS_ERR_IO       = -5,   // Error during device operation
    LFS_ERR_CORRUPT  = -52,  // Corrupted
    LFS_ERR_NOENT    = -2,   // No directory entry
    LFS_ERR_EXIST    = -17,  // Entry already exists
    LFS_ERR_NOTDIR   = -20,  // Entry is not a dir
    LFS_ERR_ISDIR    = -21,  // Entry is a dir
    LFS_ERR_NOTEMPTY = -39,  // Dir is not empty
    LFS_ERR_BADF     = -9,   // Bad file number
    LFS_ERR_FBIG     = -27,  // File too large
    LFS_ERR_INVAL    = -22,  // Invalid parameter
    LFS_ERR_NOSPC    = -28,  // No space left on device
    LFS_ERR_NOMEM    = -12,  // No more memory available
};

// File types
enum lfs_type {
    LFS_TYPE_REG        = 0x11,
    LFS_TYPE_DIR        = 0x22,
    LFS_TYPE_SUPERBLOCK = 0x2e,
};

// File open flags
enum lfs_open_flags {
    // open flags
    LFS_O_RDONLY = 1,        // Open a file as read only
    LFS_O_WRONLY = 2,        // Open a file as write only
    LFS_O_RDWR   = 3,        // Open a file as read and write
    LFS_O_CREAT  = 0x0100,   // Create a file if it does not exist
    LFS_O_EXCL   = 0x0200,   // Fail if a file already exists
    LFS_O_TRUNC  = 0x0400,   // Truncate the existing file to zero size
    LFS_O_APPEND = 0x0800,   // Move to end of file on every write

    // internally used flags
    LFS_F_DIRTY   = 0x10000, // File does not match storage
    LFS_F_WRITING = 0x20000, // File has been written since last flush
    LFS_F_READING = 0x40000, // File has been read since last flush
    LFS_F_ERRED   = 0x80000, // An error occured during write
};

// File seek flags
enum lfs_whence_flags {
    LFS_SEEK_SET = 0,   // Seek relative to an absolute position
    LFS_SEEK_CUR = 1,   // Seek relative to the current file position
    LFS_SEEK_END = 2,   // Seek relative to the end of the file
};

// Configuration provided during initialization of the littlefs
struct lfs_config {
    // Opaque user provided context that can be used to pass
    // information to the block device operations
    void *context;

    // Read a region in a block. Negative error codes are propogated
    // to the user.
    int (*read)(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, void *buffer, lfs_size_t size);

    // Program a region in a block. The block must have previously
    // been erased. Negative error codes are propogated to the user.
    // May return LFS_ERR_CORRUPT if the block should be considered bad.
    int (*prog)(const struct lfs_config *c, lfs_block_t block,
            lfs_off_t off, const void *buffer, lfs_size_t size);

    // Erase a block. A block must be erased before being programmed.
    // The state of an erased block is undefined. Negative error codes
    // are propogated to the user.
    // May return LFS_ERR_CORRUPT if the block should be considered bad.
    int (*erase)(const struct lfs_config *c, lfs_block_t block);

    // Sync the state of the underlying block device. Negative error codes
    // are propogated to the user.
    int (*sync)(const struct lfs_config *c);

    // Minimum size of a block read. This determines the size of read buffers.
    // This may be larger than the physical read size to improve performance
    // by caching more of the block device.
    lfs_size_t read_size;

    // Minimum size of a block program. This determines the size of program
    // buffers. This may be larger than the physical program size to improve
    // performance by caching more of the block device.
    // Must be a multiple of the read size.
    lfs_size_t prog_size;

    // Size of an erasable block. This does not impact ram consumption and
    // may be larger than the physical erase size. However, this should be
    // kept small as each file currently takes up an entire block.
    // Must be a multiple of the program size.
    lfs_size_t block_size;

    // Number of erasable blocks on the device.
    lfs_size_t block_count;

    // Number of blocks to lookahead during block allocation. A larger
    // lookahead reduces the number of passes required to allocate a block.
    // The lookahead buffer requires only 1 bit per block so it can be quite
    // large with little ram impact. Should be a multiple of 32.
    lfs_size_t lookahead;

    // Optional, statically allocated read buffer. Must be read sized.
    void *read_buffer;

    // Optional, statically allocated program buffer. Must be program sized.
    void *prog_buffer;

    // Optional, statically allocated lookahead buffer. Must be 1 bit per
    // lookahead block.
    void *lookahead_buffer;

    // Optional, statically allocated buffer for files. Must be program sized.
    // If enabled, only one file may be opened at a time.
    void *file_buffer;

    // Number of erase cycles before littlefs evicts metadata logs and moves
    // the metadata to another block. Suggested values are in the
    // range 100-1000, with large values having better performance at the cost
    // of less consistent wear distribution.
    //
    // Set to -1 to disable block-level wear-leveling.
    int block_cycles;
};

// Optional configuration provided during lfs_file_opencfg
struct lfs_file_config {
    // Optional, statically allocated buffer for files. Must be program sized.
    // If NULL, malloc will be used by default.
    void *buffer;
};

// File info structure
struct lfs_info {
    // Type of the file, either LFS_TYPE_REG or LFS_TYPE_DIR
    UINT8 type;

    // Size of the file, only valid for REG files
    lfs_size_t size;

    // Name of the file stored as a null-terminated string
    char name[LFS_NAME_MAX+1];
};


/// littlefs data structures ///
typedef struct lfs_entry {
    lfs_off_t off;

    struct lfs_disk_entry {
        UINT8 type;
        UINT8 elen;
        UINT8 alen;
        UINT8 nlen;
        union {
            struct {
#ifdef LFS_CHECK_SUM
                UINT16 head;
                UINT16 cksum;
#else
                lfs_block_t head;
#endif
                lfs_size_t size;
            } file;
            lfs_block_t dir[2];
        } u;
    } d;
} lfs_entry_t;

typedef struct lfs_cache {
    lfs_block_t block;
    lfs_off_t off;
    UINT8 *buffer;
} lfs_cache_t;

typedef struct lfs_file {
    struct lfs_file *next;
    lfs_block_t pair[2];
    lfs_off_t poff;

#ifdef LFS_CHECK_SUM
    UINT16 head;
    UINT16 cksum;
#else
    lfs_block_t head;
#endif
    lfs_size_t size;

    const struct lfs_file_config *cfg;
    UINT32 flags;
    lfs_off_t pos;
    lfs_block_t block;
    lfs_off_t off;
    lfs_cache_t cache;
} lfs_file_t;

typedef struct lfs_dir {
    struct lfs_dir *next;
    lfs_block_t pair[2];
    lfs_off_t off;

    lfs_block_t head[2];
    lfs_off_t pos;

    struct lfs_disk_dir {
        UINT32 rev;
        lfs_size_t size;
        lfs_block_t tail[2];
    } d;
} lfs_dir_t;

typedef struct lfs_superblock {
    lfs_off_t off;

    struct lfs_disk_superblock {
        UINT8 type;
        UINT8 elen;
        UINT8 alen;
        UINT8 nlen;
        lfs_block_t root[2];
        UINT32 block_size;
        UINT32 block_count;
        UINT32 version;
        char magic[8];
    } d;
} lfs_superblock_t;

typedef struct lfs_free {
    lfs_block_t off;
    lfs_block_t size;
    lfs_block_t i;
    lfs_block_t ack;
    UINT32 *buffer;
} lfs_free_t;

// The littlefs type
typedef struct lfs {
    const struct lfs_config *cfg;

    lfs_block_t root[2];
    lfs_file_t *files;
    lfs_dir_t *dirs;

    lfs_cache_t rcache;
    lfs_cache_t pcache;

    lfs_free_t free;
    BOOL deorphaned;
    BOOL moving;
} lfs_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
