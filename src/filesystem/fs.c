#include "fs.h"

/* NOTES:
 * Each file is a page
 * Will be using Node Graph VFS model based on POSIX
 * fs_node contains function pointers for each of the 
 * file operations. This is so that these can be updated
 * /use different ones for different devices.
 * Is completely pointless for this but not hard to implement
 * so may as well.
 */

/* K:V pair of inode and virtual address that's the base of the file */
kv* file_lookup;


fs_node *fs_root = 0; // The root of the filesystem.

uint32_t read_fs(fs_node *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    // Has the node got a read callback?
    if (node->read != 0)
        return node->read(node, offset, size, buffer);
    else
        return 0;
}

uint32_t write_fs(fs_node *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
    // Has the node got a write callback?
    if (node->write != 0)
        return node->write(node, offset, size, buffer);
    else
        return 0;
}

void open_fs(fs_node *node, uint8_t read, uint8_t write)
{
    // Has the node got an open callback?
    if (node->open != 0)
        return node->open(node);
}

void close_fs(fs_node *node)
{
    // Has the node got a close callback?
    if (node->close != 0)
        return node->close(node);
}

struct dirent *readdir_fs(fs_node *node, uint32_t index)
{
    // Is the node a directory, and does it have a callback?
    if ( (node->type) == DIRECTORY &&
         node->readdir != 0 )
        return node->readdir(node, index);
    else
        return 0;
}

fs_node *finddir_fs(fs_node *node, char *name)
{
    // Is the node a directory, and does it have a callback?
    if ( (node->type) == DIRECTORY &&
         node->finddir != 0 )
        return node->finddir(node, name);
    else
        return 0;
}