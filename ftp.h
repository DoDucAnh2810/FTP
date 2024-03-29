#ifndef __FTP_H__
#define __FTP_H__

/* Send the data from the file at target_path to the connection at conn_fd*/
int ftp(int conn_fd, char *target_path);

#endif