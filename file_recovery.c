#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>

#define EXT4_SUPERBLOCK_OFFSET 1024
#define EXT4_SUPERBLOCK_SIZE 1024
#define EXT4_INODE_SIZE 256
#define EXT4_GOOD_OLD_FIRST_INO 11

#define MAX_INPUT_SIZE 256

struct ext4_superblock {
    unsigned int s_inodes_count;
    unsigned int s_blocks_count_lo;
    unsigned int s_r_blocks_count_lo;
    unsigned int s_free_blocks_count_lo;
    unsigned int s_free_inodes_count;
    unsigned int s_first_data_block;
    unsigned int s_log_block_size;
    unsigned int s_log_cluster_size;
    unsigned int s_blocks_per_group;
    unsigned int s_clusters_per_group;
    unsigned int s_inodes_per_group;
    unsigned int s_mtime;
    unsigned int s_wtime;
    unsigned short s_mnt_count;
    short s_max_mnt_count;
    unsigned short s_magic;
    unsigned short s_state;
    unsigned short s_errors;
    unsigned short s_minor_rev_level;
    unsigned int s_lastcheck;
    unsigned int s_checkinterval;
    unsigned int s_creator_os;
    unsigned int s_rev_level;
    unsigned short s_def_resuid;
    unsigned short s_def_resgid;
    unsigned int s_first_ino;
    unsigned short s_inode_size;
    unsigned short s_block_group_nr;
    unsigned int s_feature_compat;
    unsigned int s_feature_incompat;
    unsigned int s_feature_ro_compat;
    unsigned char s_uuid[16];
    char s_volume_name[16];
    char s_last_mounted[64];
    unsigned int s_algorithm_usage_bitmap;
    unsigned char s_prealloc_blocks;
    unsigned char s_prealloc_dir_blocks;
    unsigned short s_reserved_gdt_blocks;
    unsigned char s_journal_uuid[16];
    unsigned int s_journal_inum;
    unsigned int s_journal_dev;
    unsigned int s_last_orphan;
    unsigned int s_hash_seed[4];
    unsigned char s_def_hash_version;
    unsigned char s_jnl_backup_type;
    unsigned short s_desc_size;
    unsigned int s_default_mount_opts;
    unsigned int s_first_meta_bg;
    unsigned int s_mkfs_time;
    unsigned int s_jnl_blocks[17];
    unsigned int s_blocks_count_hi;
    unsigned int s_r_blocks_count_hi;
    unsigned int s_free_blocks_count_hi;
    unsigned short s_min_extra_isize;
    unsigned short s_want_extra_isize;
    unsigned int s_flags;
    unsigned short s_raid_stride;
    unsigned short s_mmp_interval;
    unsigned long long s_mmp_block;
    unsigned int s_raid_stripe_width;
    unsigned char s_log_groups_per_flex;
    unsigned char s_reserved_char_pad;
    unsigned short s_reserved_pad;
    unsigned char s_padding[2];
    unsigned int s_reserved[162];
};

struct ext4_inode {
    unsigned short i_mode;
    unsigned short i_uid;
    unsigned int i_size_lo;
    unsigned int i_atime;
    unsigned int i_ctime;
    unsigned int i_mtime;
    unsigned int i_dtime;
    unsigned short i_gid;
    unsigned short i_links_count;
    unsigned int i_blocks_lo;
    unsigned int i_flags;
    unsigned int i_osd1;
    unsigned int i_block[15];
    unsigned int i_generation;
    unsigned int i_file_acl_lo;
    unsigned int i_size_high;
    unsigned int i_obso_faddr;
    unsigned char i_osd2[12];
    unsigned short i_extra_isize;
    unsigned short i_checksum_hi;
    unsigned int i_ctime_extra;
    unsigned int i_mtime_extra;
    unsigned int i_atime_extra;
    unsigned int i_crtime;
    unsigned int i_crtime_extra;
    unsigned int i_version_hi;
    unsigned int i_projid;
};

struct deleted_file {
    int inode_num;
    unsigned int size;
    unsigned int dtime;
};

void handle_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

void read_superblock(int fd, struct ext4_superblock *sb) {
    if (lseek(fd, EXT4_SUPERBLOCK_OFFSET, SEEK_SET) == -1) {
        handle_error("lseek to superblock");
    }
    if (read(fd, sb, EXT4_SUPERBLOCK_SIZE) != EXT4_SUPERBLOCK_SIZE) {
        handle_error("read superblock");
    }
}

void read_inode(int fd, struct ext4_superblock *sb, int inode_num, struct ext4_inode *inode) {
    int block_size = 1024 << sb->s_log_block_size;
    int inodes_per_block = block_size / sb->s_inode_size;
    int inode_table_block = sb->s_first_data_block + 1;
    int block_group = (inode_num - 1) / sb->s_inodes_per_group;
    int index_in_group = (inode_num - 1) % sb->s_inodes_per_group;
    int block_in_group = index_in_group / inodes_per_block;
    int offset_in_block = (index_in_group % inodes_per_block) * sb->s_inode_size;

    int inode_offset = (inode_table_block + block_in_group) * block_size + offset_in_block;
    if (lseek(fd, inode_offset, SEEK_SET) == -1) {
        handle_error("lseek to inode");
    }
    if (read(fd, inode, sb->s_inode_size) != sb->s_inode_size) {
        handle_error("read inode");
    }
}

void recover_data(int fd, struct ext4_inode *inode, const char *output_filename) {
    FILE *output = fopen(output_filename, "wb");
    if (output == NULL) {
        handle_error("fopen output file");
    }

    int block_size = 1024 << inode->i_osd1;
    for (int i = 0; i < 15 && inode->i_block[i]; i++) {
        char *buffer = malloc(block_size);
        if (buffer == NULL) {
            handle_error("malloc buffer");
        }
        if (lseek(fd, inode->i_block[i] * block_size, SEEK_SET) == -1) {
            free(buffer);
            handle_error("lseek to data block");
        }
        if (read(fd, buffer, block_size) != block_size) {
            free(buffer);
            handle_error("read data block");
        }
        fwrite(buffer, 1, block_size, output);
        free(buffer);
    }

    fclose(output);
}

void list_deleted_files(int fd, struct ext4_superblock *sb, struct deleted_file **files, int *count) {
    *count = 0;
    *files = NULL;
    struct ext4_inode inode;
    for (int i = EXT4_GOOD_OLD_FIRST_INO; i < sb->s_inodes_count; i++) {
        read_inode(fd, sb, i, &inode);
        if (inode.i_dtime != 0) {
            *files = realloc(*files, (*count + 1) * sizeof(struct deleted_file));
            if (*files == NULL) {
                handle_error("realloc files");
            }
            (*files)[*count].inode_num = i;
            (*files)[*count].size = inode.i_size_lo;
            (*files)[*count].dtime = inode.i_dtime;
            (*count)++;
        }
    }
}

void print_deleted_files(const struct deleted_file *files, int count) {
    printf("Deleted Files:\n");
    for (int i = 0; i < count; i++) {
        printf("%d: Inode %d, Size %u bytes, Deleted time %u\n", i + 1, files[i].inode_num, files[i].size, files[i].dtime);
    }
}

void recover_selected_files(int fd, struct ext4_superblock *sb, const struct deleted_file *files, int count, const int *selected_indices, int selected_count) {
    struct ext4_inode inode;
    for (int i = 0; i < selected_count; i++) {
        int index = selected_indices[i];
        if (index < 0 || index >= count) {
            printf("Invalid index %d\n", index);
            continue;
        }
        read_inode(fd, sb, files[index].inode_num, &inode);
        char output_filename[256];
        snprintf(output_filename, sizeof(output_filename), "recovered_file_%d.bin", files[index].inode_num);
        recover_data(fd, &inode, output_filename);
        printf("Recovered data to %s\n", output_filename);
    }
}

void prompt_user_for_recovery(int fd, struct ext4_superblock *sb, struct deleted_file *files, int file_count) {
    while (1) {
        print_deleted_files(files, file_count);

        printf("\nEnter the numbers of the files you want to recover (separated by spaces), or 'q' to quit:\n");
        char input[MAX_INPUT_SIZE];
        if (fgets(input, sizeof(input), stdin) == NULL) {
            handle_error("fgets input");
        }

        if (tolower(input[0]) == 'q') {
            free(files);
            close(fd);
            printf("Exiting...\n");
            exit(EXIT_SUCCESS);
        }

        int selected_indices[file_count];
        int selected_count = 0;
        char *token = strtok(input, " ");
        while (token != NULL) {
            int index = atoi(token) - 1;
            if (index >= 0 && index < file_count) {
                selected_indices[selected_count++] = index;
            } else {
                printf("Invalid index %d, ignoring...\n", index + 1);
            }
            token = strtok(NULL, " ");
        }

        recover_selected_files(fd, sb, files, file_count, selected_indices, selected_count);
    }
}

void display_menu() {
    printf("\n=== Ext4 Recovery CLI ===\n");
    printf("1. List deleted files\n");
    printf("2. Recover selected files\n");
    printf("3. Exit\n");
    printf("Select an option: ");
}

void execute_option(int fd, struct ext4_superblock *sb, struct deleted_file *files, int file_count, int option) {
    switch (option) {
        case 1:
            print_deleted_files(files, file_count);
            break;
        case 2:
            prompt_user_for_recovery(fd, sb, files, file_count);
            break;
        case 3:
            free(files);
            close(fd);
            printf("Exiting...\n");
            exit(EXIT_SUCCESS);
        default:
            printf("Invalid option. Please try again.\n");
    }
}

int main() {
    const char *disk_path = "/dev/sdb1";
    int fd = open(disk_path, O_RDONLY);
    if (fd == -1) {
        handle_error("open disk");
    }

    struct ext4_superblock sb;
    read_superblock(fd, &sb);

    struct deleted_file *files;
    int file_count;
    list_deleted_files(fd, &sb, &files, &file_count);

    int option;
    while (1) {
        display_menu();
        if (scanf("%d", &option) != 1) {
            handle_error("scanf option");
        }
        while (getchar() != '\n');  // clear the input buffer
        execute_option(fd, &sb, files, file_count, option);
    }

    return 0;
}
